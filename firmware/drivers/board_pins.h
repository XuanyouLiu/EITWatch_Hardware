#pragma once

#include <driver/gpio.h>

/* Digipot handle indices used by AD5270_DigiPot. */
#define SRC_INAMP_HANDLE 0
#define SENSE_INAMP_HANDLE 1

/* Seeed XIAO ESP32-C3 pin map for the EITWatch AFE. */
#define PIN_SPI_MOSI GPIO_NUM_9
#define PIN_SPI_MISO GPIO_NUM_10
#define PIN_SPI_SCLK GPIO_NUM_8

#define PIN_CS_ADC GPIO_NUM_2
#define PIN_CS_DRIVE GPIO_NUM_5
#define PIN_CS_MEAS GPIO_NUM_6
#define PIN_CS_MUX GPIO_NUM_7
#define PIN_CS_AD5930 GPIO_NUM_20

#define PIN_CTRL GPIO_NUM_4
#define PIN_MSB GPIO_NUM_3

#ifdef USE_ESP_ADC
#define PIN_ESP_ADC_INPUT GPIO_NUM_4
#endif
