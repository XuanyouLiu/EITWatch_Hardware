#pragma once

#include <stdint.h>

/**
 * @brief Initialize the four daisy-chained ADG738 multiplexers.
 *
 * @return 0 on success, or a negative error code.
 */
int init_src_sense_ADG738(void);

/**
 * @brief Set source and sense channels on the ADG738 multiplexers.
 *
 * Channel indices are 1-based (electrode 1..8).
 *
 * @param src_pos   Positive source electrode
 * @param src_neg   Negative source electrode
 * @param sense_pos Positive sense electrode
 * @param sense_neg Negative sense electrode
 * @return 0 on success, or a negative error code.
 */
int set_src_sense_ADG738(uint8_t src_pos, uint8_t src_neg,
                         uint8_t sense_pos, uint8_t sense_neg);
