#pragma once
#include <stdint.h>
#include <stdbool.h>

#define TEST_GEN_N   64
#define TEST_GEN_FS  500000.0f   // 500 kHz sampling rate

/**
 * @brief Generate a sine wave in an int16_t buffer.
 * 
 * @param out Pointer to the output buffer (must be at least TEST_GEN_N in size).
 * @param freq_hz Frequency of the sine wave in Hz.
 */
void generate_sine_int16(int16_t *out, float freq_hz);


/**
 * @brief Generates a multi-frequency sine wave and stores it as signed 16-bit integers.
 * 
 * @param out Pointer to the output buffer where the generated sine wave samples will be stored.
 * @param freq_hz1 Frequency of the first sine wave component in Hertz.
 * @param freq_hz2 Frequency of the second sine wave component in Hertz.
 * @param freq_hz3 Frequency of the third sine wave component in Hertz.
 * 
 * @note The function combines three sine waves at different frequencies and stores the result
 *       as 16-bit signed integers. The caller must ensure that the output buffer has sufficient
 *       space to hold the generated samples.
 * 
 * @see generate_sine_int16_multi
 *
 * @see generate_sine_int16_multi
 */
void generate_sine_int16_multi(int16_t *out, float freq_hz1, float freq_hz2, float freq_hz3);

/**
 * @brief Generates a multi-frequency sine wave with random amplitudes and stores it as signed 16-bit integers.
 * 
 * @param out Pointer to the output buffer where the generated sine wave samples will be stored.
 * @param freq_hz1 Frequency of the first sine wave component in Hertz.
 * @param freq_hz2 Frequency of the second sine wave component in Hertz.
 * @param freq_hz3 Frequency of the third sine wave component in Hertz.
 */
void generate_sine_int16_multi_random_amp(int16_t *out, float freq_hz1, float freq_hz2, float freq_hz3);

/**
 * @brief Generates a multi-frequency sine wave with random amplitudes and random clipping.
 * 
 * @param out Pointer to the output buffer where the generated sine wave samples will be stored.
 * @param freq_hz1 Frequency of the first sine wave component in Hertz.
 * @param freq_hz2 Frequency of the second sine wave component in Hertz.
 * @param freq_hz3 Frequency of the third sine wave component in Hertz.
 * @param clipped If true, applies random clipping to the signal.
 * @param clip_percent Percentage of amplitude to clip at (0.0 to 1.0).
 */
void generate_sine_int16_multi_random_amp_clipped(int16_t *out, float freq_hz1, float freq_hz2, float freq_hz3, bool clipped, float clip_percent);
