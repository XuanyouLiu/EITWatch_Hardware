#pragma once

#include <driver/gpio.h>

/* Digipot handle indices used by AD5270_DigiPot. */
#define SRC_INAMP_HANDLE 0
#define SENSE_INAMP_HANDLE 1

/*
 * Seeed XIAO ESP32-S3 pin map for the EITWatch AFE.
 * Macros use chip GPIO numbers; comments list the XIAO pad (D0-D10)
 * that the PCB header is wired to.
 */
#define PIN_SPI_MOSI GPIO_NUM_8   /* D9 */
#define PIN_SPI_MISO GPIO_NUM_9   /* D10 */
#define PIN_SPI_SCLK GPIO_NUM_7   /* D8 */

#define PIN_CS_ADC GPIO_NUM_1     /* D0 */
#define PIN_CS_DRIVE GPIO_NUM_4   /* D3 */
#define PIN_CS_MEAS GPIO_NUM_5    /* D4 */
#define PIN_CS_MUX GPIO_NUM_6     /* D5 */
#define PIN_CS_AD5930 GPIO_NUM_44 /* D7 */

#define PIN_CTRL GPIO_NUM_3       /* D2 */
#define PIN_MSB GPIO_NUM_2        /* D1 */

#ifdef USE_ESP_ADC
#define PIN_ESP_ADC_INPUT GPIO_NUM_3 /* D2 */
#endif
