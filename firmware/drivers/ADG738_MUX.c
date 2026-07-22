/*
 * ADG738_MUX.c
 *
 * Driver for four daisy-chained ADG738 8:1 matrix switches.
 * SPI payload order (LSB first in the 32-bit word):
 *   Source+, Source-, Sense+, Sense-
 *
 * Author: Novel Alam
 */

#include "ADG738_MUX.h"

#include "board_pins.h"
#include "driver/spi_master.h"
#include "esp_err.h"

static spi_device_handle_t adg738_handle = NULL;

static const spi_device_interface_config_t adg738_config = {
    .mode = 1, /* CPOL=0, CPHA=1 */
    .clock_source = SPI_CLK_SRC_DEFAULT,
    .duty_cycle_pos = 128,
    .clock_speed_hz = SPI_MASTER_FREQ_8M,
    .spics_io_num = PIN_CS_MUX,
    .queue_size = 1,
};

static spi_transaction_t message = {
    .length = 32,
    .flags = SPI_TRANS_USE_TXDATA,
};

int init_src_sense_ADG738(void)
{
    if (spi_bus_add_device(SPI2_HOST, &adg738_config, &adg738_handle) != ESP_OK) {
        return -1;
    }
    return 0;
}

int set_src_sense_ADG738(uint8_t src_pos, uint8_t src_neg,
                         uint8_t sense_pos, uint8_t sense_neg)
{
    /* Pack one-hot channel selects into a 32-bit word for the daisy chain. */
    uint32_t payload = (uint32_t)(1u << (src_pos - 1)) |
                       ((uint32_t)(1u << (src_neg - 1)) << 8) |
                       ((uint32_t)(1u << (sense_pos - 1)) << 16) |
                       ((uint32_t)(1u << (sense_neg - 1)) << 24);

    *(uint32_t *)message.tx_data = payload;

    if (spi_device_polling_transmit(adg738_handle, &message) != ESP_OK) {
        return -1;
    }
    return 0;
}
