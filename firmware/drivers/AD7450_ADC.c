#include "AD7450_ADC.h"
#include "board_pins.h"
#include "driver/spi_master.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "soc/spi_struct.h"  // For spi_dev_t and GPSPI2

// Use the SPI driver path for stability across resets/reflashes.
// The direct-register fast path is kept below for reference/debugging.
// #define USE_DIRECT_REGISTERS

static const char *TAG = "AD7450";
static spi_device_handle_t ad7450_handle = NULL;

uint16_t AD7450_Read_Direct_Registers(void);

int AD7450_init() {
    if (ad7450_handle != NULL) {
        ESP_LOGW(TAG, "AD7450 already initialized");
        return ESP_OK;
    }

    spi_device_interface_config_t devcfg = {
        .command_bits = 0,
        .address_bits = 0,
        .dummy_bits = 0,
        .mode = 2, // CPOL=1, CPHA=0 (SCLK idle high, sample on rising edge)
        .clock_speed_hz = 20 * 1000 * 1000, // 8 MHz
        .spics_io_num = PIN_CS_ADC,
        .queue_size = AD7450_QUEUE_SIZE,
    };

    esp_err_t ret = spi_bus_add_device(SPI2_HOST, &devcfg, &ad7450_handle);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to add device to SPI bus: %s", esp_err_to_name(ret));
        return ret;
    }

    // Perform a dummy read to ensure the device is powered up and in normal mode
    spi_transaction_t t;
    memset(&t, 0, sizeof(t));
    t.length = 16;
    t.flags = SPI_TRANS_USE_RXDATA; // Use TX data to ensure MOSI is driven (though ignored by ADC)
    
    ret = spi_device_transmit(ad7450_handle, &t);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Dummy read failed: %s", esp_err_to_name(ret));
        return ret;
    }

    ESP_LOGI(TAG, "AD7450 initialized successfully");
    return ESP_OK;
}

int AD7450_Read(uint16_t *buf, uint32_t len)
{
#ifdef USE_DIRECT_REGISTERS
    if (!ad7450_handle) {
        ESP_LOGE(TAG, "AD7450 read failed: device not initialized");
        return -1;
    }

    // if (spi_device_acquire_bus(ad7450_handle, portMAX_DELAY) != ESP_OK) {
    //     ESP_LOGE(TAG, "AD7450 read failed: could not acquire SPI bus");
    //     return -1;
    // }

    // Use direct register access (bypasses all HAL/LL layers)
    for (uint32_t i = 0; i < len; i++) {
        buf[i] = AD7450_Read_Direct_Registers();
    }

    // spi_device_release_bus(ad7450_handle);
    return 0;
#else
    // Use ESP-IDF SPI driver
    if (!ad7450_handle) return -1;

    spi_transaction_t t;
    uint8_t rx[2];

    memset(&t, 0, sizeof(t));
    t.length = 16;
    t.rx_buffer = rx;

    for (uint32_t i = 0; i < len; i++) {
        if (spi_device_polling_transmit(ad7450_handle, &t) != ESP_OK) {
            return -1;
        }

        uint16_t result = (uint16_t)((rx[0] << 8) | rx[1]);
        // AD7450 sample is 12-bit; keep the lower 12 bits
        buf[i] = result & 0x0FFF;
    }

    return 0;
#endif
}

/**
 * @brief Direct register-level 16-bit SPI transaction (bypasses all LL wrapper functions)
 * 
 * This function writes directly to SPI2 peripheral registers in the exact sequence
 * captured from the trace output, replicating a 16-bit full-duplex transaction
 * without calling any spi_ll_* functions.
 * 
 * IMPORTANT: SPI bus must be initialized first via AD7450_init()
 * This function manually controls CS and writes to registers directly.
 * 
 * @return 16-bit value read from SPI data buffer
 */
uint16_t AD7450_Read_Direct_Registers(void)
{
    spi_dev_t *hw = &GPSPI2;

    // Save original register values to restore later
    uint32_t orig_ctrl = hw->ctrl.val;
    uint32_t orig_user = hw->user.val;
    uint32_t orig_user1 = hw->user1.val;
    uint32_t orig_user2 = hw->user2.val;
    uint32_t orig_ms_dlen = hw->ms_dlen.val;
    uint32_t orig_addr = hw->addr;
    uint32_t orig_misc = hw->misc.val;
    uint32_t orig_dma_conf = hw->dma_conf.val;

    static bool cs_gpio_configured = false;
    if (!cs_gpio_configured) {
        gpio_reset_pin(PIN_CS_ADC);
        gpio_set_direction(PIN_CS_ADC, GPIO_MODE_OUTPUT);
        gpio_set_level(PIN_CS_ADC, 1);
        cs_gpio_configured = true;
        for (volatile int i = 0; i < 100; i++);
    }
    
    // [spi_ll_clear_int_stat]
    hw->dma_int_raw.val = 0;

    // [spi_ll_get_running_cmd]
    while (hw->cmd.usr || hw->cmd.update) {
    }

    gpio_set_level(PIN_CS_ADC, 0);

    // [spi_ll_master_set_line_mode]
    hw->ctrl.val = 0x00340000;
    hw->user.val = 0x18000241;

    // [spi_ll_set_dummy]
    hw->user.usr_dummy = 0;

    // [spi_ll_set_mosi_bitlen] + [spi_ll_set_miso_bitlen]
    hw->ms_dlen.val = 0x0000000f;

    // [spi_ll_set_dummy] + [spi_ll_set_addr_bitlen] + [spi_ll_set_command_bitlen]
    // Write full register values to match trace exactly:
    //   user1.val = 0xf87f00ff (usr_addr_bitlen=-1, usr_dummy_cyclelen=-1)
    //   user2.val = 0xf8000000 (usr_command_bitlen=-1, usr_command_value=0)
    hw->user1.val = 0xf87f00ff;
    hw->user2.val = 0xf8000000;
    hw->user.usr_addr = 0;
    hw->user.usr_command = 0;

    // [spi_ll_set_address]
    hw->addr = 0x00000000;

    // [spi_ll_master_keep_cs]
    hw->misc.val = 0x2000003e;

    // [spi_ll_enable_mosi] + [spi_ll_enable_miso]
    hw->user.usr_mosi = 1;
    hw->user.usr_miso = 1;

    hw->dma_conf.dma_rx_ena = 0;
    hw->dma_conf.dma_tx_ena = 0;

    // [spi_ll_apply_config]
    hw->cmd.update = 1;
    while (hw->cmd.update) {
    }

    // [spi_ll_user_start]
    hw->cmd.usr = 1;

    // [spi_ll_usr_is_done]
    for (volatile int nop_i = 0; nop_i < 16; nop_i++) {
        asm("nop");
    }

    // [spi_ll_read_buffer]
    uint32_t data_word = hw->data_buf[0];

    gpio_set_level(PIN_CS_ADC, 1);

    // Clear completion interrupt for next transaction
    hw->dma_int_raw.val = 0;

    // Restore original register values
    hw->ctrl.val = orig_ctrl;
    hw->user.val = orig_user;
    hw->user1.val = orig_user1;
    hw->user2.val = orig_user2;
    hw->ms_dlen.val = orig_ms_dlen;
    hw->addr = orig_addr;
    hw->misc.val = orig_misc;
    hw->dma_conf.val = orig_dma_conf;

    // ESP-IDF's spi_ll_read_buffer copies bytes in little-endian memory order:
    // For data_buf[0]=0xffffc407: byte[0]=0x07, byte[1]=0xc4
    // Then middleware does (rx[0]<<8)|rx[1] = (0x07<<8)|0xc4 = 0x07c4 = 1988
    // So we must byte-swap the lower 16 bits to match.
    uint8_t lo = (uint8_t)(data_word & 0xFF);          // rx[0]
    uint8_t hi = (uint8_t)((data_word >> 8) & 0xFF);    // rx[1]
    uint16_t result = (lo << 8) | hi;

    // AD7450 sample is 12-bit; keep the lower 12 bits
    uint16_t adc_12bit = result & 0x0FFF;

    return adc_12bit;
}
