/**
 * @file ad5930.c - ad5930 signal generator (HAL-agnostic).
 * Author: Novel Alam, Date: Nov 12 2025.
 */

#include "AD5930_SigGen.h"
#include "board_pins.h"
#include "driver/spi_master.h"
#include "driver/spi_common.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *TAG = "AD5930";

/* Spi Config stuff*/
static const spi_device_interface_config_t ad5930_spi_config = {
    .command_bits = 0,
    .address_bits = 0,
    .dummy_bits = 0,
    .mode = 2,
    .duty_cycle_pos = 128,
    .spics_io_num = PIN_CS_AD5930,
    .queue_size = 1,
    .clock_speed_hz = SPI_MASTER_FREQ_8M

};

/* Spi handle*/
static spi_device_handle_t ad5930_handle = NULL;

/* Used for setting frequency*/
static spi_transaction_t msg_set_freq = {
    .flags = SPI_TRANS_USE_TXDATA | SPI_TRANS_USE_RXDATA,
    .length = 16,
    .rx_data = {0}
};





/*Add Signal generator to spi bus (assuming intialized)*/

int AD5930_init(float init_freq) {
    esp_err_t ret = ESP_OK;

    if (ad5930_handle == NULL) {
        ret = spi_bus_add_device(SPI2_HOST, &ad5930_spi_config, &ad5930_handle);
        if ( ret != ESP_OK ) {
            #if DEBUG
            ESP_LOGE(TAG, "Failed to add AD5930 to spi bus: %s", esp_err_to_name(ret));
            #endif
            return ret;
        } else {
            ESP_LOGI(TAG, "Added AD5930 to bus");
        }
        gpio_reset_pin(PIN_CTRL);
        gpio_set_direction(PIN_CTRL, GPIO_MODE_OUTPUT);
    }

    gpio_set_level(PIN_CTRL, 0);
    vTaskDelay(pdMS_TO_TICKS(100));

    /* Set up the configuration register*/

    /* B24 = 0 (start freq high and low regs can be written independently)
    * DAC ENABLE = 1 (DAC enabled)
    * SINE/TRI = 1 (sine output)
    * MSBOUTEN = 1 (MSBOUT enabled)
    * CW/BURST = 1 (no mid-scale output after burst)
    * INT/EXT BURST = 1 (burst controlled by CTRL pin)
    * INT/EXT INCR = 1 (frequency increment controlled by CTRL pin)
    * MODE = 1 (frequency saw sweep)
    * SYNCSEL = 0 (SYNCOUT outputs pulse at each freq increment)
    * SYNCOUTEN = 0 (SYNCOUT disabled)
    */

    uint16_t packet0 = AD5930_PACKET(C_REG, 0b011111110011);

    /* Send configuration packet */
    ESP_LOGI(TAG, "Sending config packet: 0x%04X", packet0);
    msg_set_freq.tx_data[0] = (packet0 >> 8) & 0xff;
    msg_set_freq.tx_data[1] = (packet0) & 0xff;
    ESP_LOGI(TAG, "SPI Bytes: [0x%02X, 0x%02X]", msg_set_freq.tx_data[0], msg_set_freq.tx_data[1]);

    ret = spi_device_transmit(ad5930_handle, &msg_set_freq);
    
    if ( ret != ESP_OK ) {
        #if DEBUG
        ESP_LOGE(TAG, "Failed to set config for ad5930: %s", esp_err_to_name(ret));
        #endif
        return ret;
    } 
    vTaskDelay(pdMS_TO_TICKS(10));

    
    /* Scale according to MCLK_FREQ */
    uint32_t converted_freq = ( ( (float) init_freq ) / ( (float) MCLK_FREQ ) ) * UINT24_MAX;
    
    
    uint16_t freq_low = converted_freq & 0xFFF;
    uint16_t freq_high = (converted_freq >> 12U) & 0xFFF;
    
    
    uint16_t packet1 = AD5930_PACKET(START_FREQ_LOW, freq_low);
    uint16_t packet2 = AD5930_PACKET(START_FREQ_HIGH, freq_high);
    
    /* Send low freq packet */
    ESP_LOGI(TAG, "Sending first packet: 0x%04X", packet1);
    msg_set_freq.tx_data[0] = (packet1 >> 8) & 0xff;
    msg_set_freq.tx_data[1] = (packet1) & 0xff;
    ESP_LOGI(TAG, "SPI Bytes: [0x%02X, 0x%02X]", msg_set_freq.tx_data[0], msg_set_freq.tx_data[1]);


    ret = spi_device_transmit(ad5930_handle, &msg_set_freq);
    
    if ( ret != ESP_OK ) {
        #if DEBUG
        ESP_LOGE(TAG, "Failed to set freq for ad5930: %s", esp_err_to_name(ret));
        #endif
        return ret;
    } 
    vTaskDelay(pdMS_TO_TICKS(10));

       


    /* Send second packet */
    ESP_LOGI(TAG, "Sending high freq packet: 0x%04X", packet2);
    msg_set_freq.tx_data[0] = (packet2 >> 8) & 0xff;
    msg_set_freq.tx_data[1] = (packet2) & 0xff;
    ESP_LOGI(TAG, "SPI Bytes: [0x%02X, 0x%02X]", msg_set_freq.tx_data[0], msg_set_freq.tx_data[1]);

    ret = spi_device_transmit(ad5930_handle, &msg_set_freq);

    if ( ret != ESP_OK ) {
        #if DEBUG
        ESP_LOGE(TAG, "Failed to set freq for ad5930: %s", esp_err_to_name(ret));
        #endif
        return ret;
    }
    vTaskDelay(pdMS_TO_TICKS(10));
    gpio_set_level(PIN_CTRL, 1);
    ESP_LOGI(TAG, "AD5930 Set to %f", init_freq);
    

    return 0;



}
