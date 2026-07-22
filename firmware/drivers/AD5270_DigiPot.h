/*
 * AD5270.h
 *
 * Short description:
 *   Minimal header for an AD5270 digital potentiometer driver.
 *   Provides initialization and basic wiper read/write prototypes.
 *
 * Author: Novel Alam
 * Date: 2025-11-13
 */

#ifndef AD5270_DIGIPOT_H
#define AD5270_DIGIPOT_H

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

#include "driver/spi_master.h"


#define MAX_R ((uint16_t)100000) //max resistance is 100kO
#define NUM_STEP ((uint16_t)1024) //number of discrete steps 




/* Return codes: 0 on success, negative on error */
/**
 * @brief Initialize the AD5270 digital potentiometer.
 * 
 * @param dev_handle The device handle index (SRC_INAMP_HANDLE or SENSE_INAMP_HANDLE).
 * @return 0 on success, or a negative error code.
 */
int ad5270_init(uint8_t dev_handle);

/* Set the wiper position to value btw 0 and 1023 */
/**
 * @brief Set the wiper position of the AD5270.
 * 
 * @param r_code The resistance code to set (0 to 1023).
 * @param dev_handle The device handle index (SRC_INAMP_HANDLE or SENSE_INAMP_HANDLE).
 * @return 0 on success, or a negative error code.
 */
int ad5270_set_wiper(uint16_t r_code, uint8_t dev_handle);



#endif /* AD5270_DIGIPOT_H */
