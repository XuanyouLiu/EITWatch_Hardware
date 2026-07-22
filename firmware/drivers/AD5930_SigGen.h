/*
 * AD5930.h
 *
 * Short description:
 *   Minimal header for an AD5930 signal generator driver.
 *   Provides initialization and start signal 
 *
 * Author: Novel Alam
 * Date: 2025-11-14
 */

#ifndef AD5930_SIGGEN_H
#define AD5930_SIGGEN_H

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

#define MAX_FREQ (1e6)
#define MCLK_FREQ (50e6)

#define UINT24_MAX (0xFFFFFFU)



typedef enum {
    C_REG = 0b0000,
    N_INCR = 0b0001,
    DELTA_FREQ_LOW = 0b0010,
    DELTA_FREQ_HIGH = 0b0011,
    INC_INTERVAL = 0b0100,
    BURST_INTERVAL = 0b1000,
    START_FREQ_LOW = 0b1100,
    START_FREQ_HIGH = 0b1101,
} AD5930_regmap;

#define AD5930_PACKET(reg_addr, data) ( (((reg_addr) & 0xF) << 12) | ((data) & 0xFFF) )

/**
 * @brief Initialize the AD5930 signal generator.
 * 
 * @param init_freq The initial frequency to output in Hz.
 * @return 0 on success, or an error code.
 */
int AD5930_init(float init_freq);



#endif