#pragma once

#include <stdio.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_err.h"

#define N_ELEC 8
#define NUM_SENSE_PAIRS (N_ELEC - 3)  /* 5: from 6 remaining electrodes, 5 adjacent sense pairs */

/* ---- Protocol selection: uncomment exactly one ---- */
// #define PROTOCOL_ADJACENT_40
// #define PROTOCOL_STAR_40
// #define PROTOCOL_FIXED_SOURCE_280
#define PROTOCOL_FIXED_POS_35

#if defined(PROTOCOL_FIXED_SOURCE_280)
  #define NUM_ELECTRODE_PAIRS (N_ELEC * (N_ELEC - 1))  /* 56 injection pairs */
#elif defined(PROTOCOL_FIXED_POS_35)
  #define NUM_ELECTRODE_PAIRS (N_ELEC - 1)              /* 7 injection pairs (fixed src_pos) */
#else
  #define NUM_ELECTRODE_PAIRS N_ELEC                     /* 8 injection pairs  */
#endif

#define TOTAL_CHANNELS (NUM_ELECTRODE_PAIRS * NUM_SENSE_PAIRS)

#define BUFFER_LEN 64
#define CALIBRATION_DSP_BUCKET 7

void calibrate(void);
void init_protocol(void);

typedef struct {
    uint8_t src_pos;
    uint8_t src_neg;
    uint8_t sense_pos;
    uint8_t sense_neg;
    uint16_t ewma_amp;
} Calibration_t;

extern const uint16_t SCR_RDATA_CONST;
extern const uint16_t SNS_RDATA_CONST;

extern Calibration_t pair_calibration_map[NUM_ELECTRODE_PAIRS][NUM_SENSE_PAIRS];
extern TaskHandle_t meas_task;
