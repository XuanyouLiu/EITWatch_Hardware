#pragma once

#include <stddef.h>
#include <stdint.h>

#include "board_pins.h"

/**
 * @brief Start the signal generator at a specific frequency.
 *
 * @param freq Frequency in Hz.
 * @return 0 on success, or an error code.
 */
int signal_gen_start(float freq);

/**
 * @brief Initialize the SPI bus.
 *
 * @return 0 on success, or an error code.
 */
int init_spi(void);

/**
 * @brief Set the gain of the source instrumentation amplifier.
 *
 * @param src_gain Gain setting for the source digipot.
 * @return 0 on success, or an error code.
 */
int set_src_inamp_gain(uint16_t src_gain);

/**
 * @brief Set the gain of the sense instrumentation amplifier.
 *
 * @param sense_gain Gain setting for the sense digipot.
 * @return 0 on success, or an error code.
 */
int set_sense_inamp_gain(uint16_t sense_gain);

/**
 * @brief Read raw samples from the ADC.
 *
 * @param buf Destination buffer.
 * @param len Number of samples to read.
 * @return 0 on success, or an error code.
 */
int adcRead(uint16_t *buf, size_t len);

/**
 * @brief Route source and sense electrodes through the ADG738 matrix.
 *
 * @param src_pos   Positive source electrode (1-based).
 * @param src_neg   Negative source electrode (1-based).
 * @param sense_pos Positive sense electrode (1-based).
 * @param sense_neg Negative sense electrode (1-based).
 * @return 0 on success, or an error code.
 */
int set_mux(uint8_t src_pos, uint8_t src_neg, uint8_t sense_pos, uint8_t sense_neg);

/**
 * @brief Initialize the instrumentation-amplifier digipots.
 *
 * @return 0 on success, or an error code.
 */
int init_inamp_pots(void);

/**
 * @brief Initialize the multiplexers.
 *
 * @return 0 on success, or an error code.
 */
int init_mux(void);

/**
 * @brief Initialize the ADC.
 *
 * @return 0 on success, or an error code.
 */
int adc_init(void);

/**
 * @brief Estimate amplitude from a buffer using a std-dev window.
 */
uint16_t calc_std_dev_mag(int16_t *buf, uint16_t buf_len, float std_multiplier);
