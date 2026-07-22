/**
 * @file ad5720.c - AD5720 digital potentiometer driver (HAL-agnostic).
 * Author: Novel Alam, Date: Nov 12 2025.
 */

#include "AD5270_DigiPot.h"
#include "board_pins.h"
#include "driver/spi_master.h"
#include "driver/spi_common.h"
#include "esp_log.h"

#define MAX_RDAC 1023

static const char *TAG = "AD5270";

/* Handle for ad5270 spi devices */
static spi_device_handle_t ad5270_handles[2] = {NULL, NULL};

/* Configuration Structs */
spi_device_interface_config_t ad5270_configs[2] = {
    [SRC_INAMP_HANDLE] = {
        .command_bits = 0,
        .address_bits = 0,
        .dummy_bits = 0,
        .mode = 1, //(CPOL = 0, CPHA = 1)
        .clock_source = SPI_CLK_SRC_DEFAULT, 
        .duty_cycle_pos = 128,
        .cs_ena_pretrans = 0,
        .cs_ena_posttrans = 0,
        .clock_speed_hz = SPI_MASTER_FREQ_8M,
        .input_delay_ns = 0,
        .spics_io_num = PIN_CS_DRIVE, 
        .flags = 0, 
        .queue_size = 1,
        .pre_cb = NULL,
        .post_cb = NULL
    },
    [SENSE_INAMP_HANDLE] = {
        .command_bits = 0,
        .address_bits = 0,
        .dummy_bits = 0,
        .mode = 1, //(CPOL = 0, CPHA = 1)
        .clock_source = SPI_CLK_SRC_DEFAULT, 
        .duty_cycle_pos = 128,
        .cs_ena_pretrans = 0,
        .cs_ena_posttrans = 0,
        .clock_speed_hz = SPI_MASTER_FREQ_8M,
        .input_delay_ns = 0,
        .spics_io_num = PIN_CS_MEAS, 
        .flags = 0, 
        .queue_size = 1,
        .pre_cb = NULL,
        .post_cb = NULL
    }
};

int ad5270_init(uint8_t dev_handle) {

    if (dev_handle != SRC_INAMP_HANDLE && dev_handle != SENSE_INAMP_HANDLE) {
        #if DEBUG
        ESP_LOGE(TAG, "Invalid device handle: %d", dev_handle);
        #endif
        return ESP_ERR_INVALID_ARG;
    }

    if (ad5270_handles[dev_handle] != NULL) {
        ESP_LOGW(TAG, "Device %d already initialized", dev_handle);
        return ESP_OK;
    }

    esp_err_t ret;

    /* Create a ad5270 device on the spi bus*/
     ret = spi_bus_add_device(
        SPI2_HOST, 
        &ad5270_configs[dev_handle],
        &ad5270_handles[dev_handle]
    ); 

    

    if ( ret != ESP_OK ) {
        #if DEBUG
        ESP_LOGE(TAG, "Failed to add AD5270 (handle %d) to spi: %s", dev_handle, esp_err_to_name(ret));
        #endif
        return ret;
    } else {
        ESP_LOGI(TAG, "Added AD5270 (handle %d) to bus", dev_handle);
    }


    spi_transaction_t msg1;
    msg1.flags = SPI_TRANS_USE_TXDATA | SPI_TRANS_USE_RXDATA;
    msg1.length = 16;
    msg1.rxlength = 0;
    msg1.tx_data[0]= 0x1C;
    msg1.tx_data[1]= 0x03;
    msg1.rx_data[0] = 0;
    msg1.rx_data[1] = 0;



    
    /* Send command to allow free movement of wiper*/
    ret = spi_device_transmit(ad5270_handles[dev_handle], &msg1);
    
    if ( ret != ESP_OK ) {
        #if DEBUG
        ESP_LOGE(TAG, "Failed to transmit init command to device %d: %s", dev_handle, esp_err_to_name(ret));
        #endif
        return ret;
    } else {
        ESP_LOGI(TAG, "Device %d initialized successfully", dev_handle);
    }      

    vTaskDelay(10);

    return ESP_OK;

}

int ad5270_set_wiper(uint16_t r_code, uint8_t dev_handle) {
     if (dev_handle != SRC_INAMP_HANDLE && dev_handle != SENSE_INAMP_HANDLE) {
        #if DEBUG
        ESP_LOGE(TAG, "Invalid device handle: %d", dev_handle);
        #endif
        return ESP_ERR_INVALID_ARG;
    }

    if (ad5270_handles[dev_handle] == NULL) {
        #if DEBUG
        ESP_LOGE(TAG, "Device %d not initialized", dev_handle);
        #endif
        return ESP_ERR_INVALID_STATE;
    }

    if (r_code > MAX_RDAC) {
        #if DEBUG
        ESP_LOGE(TAG, "Resistance code %d out of range (max %d)", r_code, MAX_RDAC);
        #endif
        return ESP_ERR_INVALID_ARG;
    }

        
    if (spi_device_acquire_bus(ad5270_handles[dev_handle], portMAX_DELAY) != ESP_OK) {
        ESP_LOGE(TAG, "Failed to acquire SPI bus for device %d", dev_handle);
        return ESP_ERR_TIMEOUT;
    }

    /* Data to send: command + 10-bit RDAC */
    uint16_t data_tx = (uint16_t)((1U << 10) | (r_code & 0x03FF));
    spi_transaction_t msg_set_wiper = {
        .flags = SPI_TRANS_USE_TXDATA,
        .length = 16,
    };
    msg_set_wiper.tx_data[0] = (uint8_t)((data_tx >> 8) & 0xff);
    msg_set_wiper.tx_data[1] = (uint8_t)(data_tx & 0xff);

    esp_err_t ret = spi_device_polling_transmit(ad5270_handles[dev_handle], &msg_set_wiper);
    spi_device_release_bus(ad5270_handles[dev_handle]);
    
    if ( ret != ESP_OK ) {
        #if DEBUG
        ESP_LOGE(TAG, "Failed to set wiper for device %d: %s", dev_handle, esp_err_to_name(ret));
        #endif
        return ret;
    } else {
        ESP_LOGD(TAG, "Wiper set to %d for device %d", r_code, dev_handle);
    }

    return ESP_OK;

}
