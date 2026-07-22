#include <stdio.h>
#include <string.h>
#include "calibration.h"
#include "hardware.h"
#include "esp_err.h"
#include "esp_log.h"

static const char *TAG = "CALIBRATION";

const uint16_t SCR_RDATA_CONST = 60;
const uint16_t SNS_RDATA_CONST = 60;

#define TOTAL_TARE_MEASUREMENTS (NUM_ELECTRODE_PAIRS * NUM_SENSE_PAIRS)
#define TARE_PRINT_COUNT 40

/* ===================================================================
 * Protocol-specific electrode maps
 * =================================================================== */

#ifdef PROTOCOL_ADJACENT_40
Calibration_t pair_calibration_map[NUM_ELECTRODE_PAIRS][NUM_SENSE_PAIRS] = {
    [0][0] = {.src_pos = 1, .src_neg = 2, .sense_pos = 3, .sense_neg = 4},
    [0][1] = {.src_pos = 1, .src_neg = 2, .sense_pos = 4, .sense_neg = 5},
    [0][2] = {.src_pos = 1, .src_neg = 2, .sense_pos = 5, .sense_neg = 6},
    [0][3] = {.src_pos = 1, .src_neg = 2, .sense_pos = 6, .sense_neg = 7},
    [0][4] = {.src_pos = 1, .src_neg = 2, .sense_pos = 7, .sense_neg = 8},
    [1][0] = {.src_pos = 2, .src_neg = 3, .sense_pos = 4, .sense_neg = 5},
    [1][1] = {.src_pos = 2, .src_neg = 3, .sense_pos = 5, .sense_neg = 6},
    [1][2] = {.src_pos = 2, .src_neg = 3, .sense_pos = 6, .sense_neg = 7},
    [1][3] = {.src_pos = 2, .src_neg = 3, .sense_pos = 7, .sense_neg = 8},
    [1][4] = {.src_pos = 2, .src_neg = 3, .sense_pos = 8, .sense_neg = 1},
    [2][0] = {.src_pos = 3, .src_neg = 4, .sense_pos = 5, .sense_neg = 6},
    [2][1] = {.src_pos = 3, .src_neg = 4, .sense_pos = 6, .sense_neg = 7},
    [2][2] = {.src_pos = 3, .src_neg = 4, .sense_pos = 7, .sense_neg = 8},
    [2][3] = {.src_pos = 3, .src_neg = 4, .sense_pos = 8, .sense_neg = 1},
    [2][4] = {.src_pos = 3, .src_neg = 4, .sense_pos = 1, .sense_neg = 2},
    [3][0] = {.src_pos = 4, .src_neg = 5, .sense_pos = 6, .sense_neg = 7},
    [3][1] = {.src_pos = 4, .src_neg = 5, .sense_pos = 7, .sense_neg = 8},
    [3][2] = {.src_pos = 4, .src_neg = 5, .sense_pos = 8, .sense_neg = 1},
    [3][3] = {.src_pos = 4, .src_neg = 5, .sense_pos = 1, .sense_neg = 2},
    [3][4] = {.src_pos = 4, .src_neg = 5, .sense_pos = 2, .sense_neg = 3},
    [4][0] = {.src_pos = 5, .src_neg = 6, .sense_pos = 7, .sense_neg = 8},
    [4][1] = {.src_pos = 5, .src_neg = 6, .sense_pos = 8, .sense_neg = 1},
    [4][2] = {.src_pos = 5, .src_neg = 6, .sense_pos = 1, .sense_neg = 2},
    [4][3] = {.src_pos = 5, .src_neg = 6, .sense_pos = 2, .sense_neg = 3},
    [4][4] = {.src_pos = 5, .src_neg = 6, .sense_pos = 3, .sense_neg = 4},
    [5][0] = {.src_pos = 6, .src_neg = 7, .sense_pos = 8, .sense_neg = 1},
    [5][1] = {.src_pos = 6, .src_neg = 7, .sense_pos = 1, .sense_neg = 2},
    [5][2] = {.src_pos = 6, .src_neg = 7, .sense_pos = 2, .sense_neg = 3},
    [5][3] = {.src_pos = 6, .src_neg = 7, .sense_pos = 3, .sense_neg = 4},
    [5][4] = {.src_pos = 6, .src_neg = 7, .sense_pos = 4, .sense_neg = 5},
    [6][0] = {.src_pos = 7, .src_neg = 8, .sense_pos = 1, .sense_neg = 2},
    [6][1] = {.src_pos = 7, .src_neg = 8, .sense_pos = 2, .sense_neg = 3},
    [6][2] = {.src_pos = 7, .src_neg = 8, .sense_pos = 3, .sense_neg = 4},
    [6][3] = {.src_pos = 7, .src_neg = 8, .sense_pos = 4, .sense_neg = 5},
    [6][4] = {.src_pos = 7, .src_neg = 8, .sense_pos = 5, .sense_neg = 6},
    [7][0] = {.src_pos = 8, .src_neg = 1, .sense_pos = 2, .sense_neg = 3},
    [7][1] = {.src_pos = 8, .src_neg = 1, .sense_pos = 3, .sense_neg = 4},
    [7][2] = {.src_pos = 8, .src_neg = 1, .sense_pos = 4, .sense_neg = 5},
    [7][3] = {.src_pos = 8, .src_neg = 1, .sense_pos = 5, .sense_neg = 6},
    [7][4] = {.src_pos = 8, .src_neg = 1, .sense_pos = 6, .sense_neg = 7},
};
#endif

#ifdef PROTOCOL_STAR_40
Calibration_t pair_calibration_map[NUM_ELECTRODE_PAIRS][NUM_SENSE_PAIRS] = {
    [0][0] = {.src_pos = 1, .src_neg = 4, .sense_pos = 7, .sense_neg = 2},
    [0][1] = {.src_pos = 1, .src_neg = 4, .sense_pos = 2, .sense_neg = 5},
    [0][2] = {.src_pos = 1, .src_neg = 4, .sense_pos = 5, .sense_neg = 8},
    [0][3] = {.src_pos = 1, .src_neg = 4, .sense_pos = 8, .sense_neg = 3},
    [0][4] = {.src_pos = 1, .src_neg = 4, .sense_pos = 3, .sense_neg = 6},
    [1][0] = {.src_pos = 4, .src_neg = 7, .sense_pos = 2, .sense_neg = 5},
    [1][1] = {.src_pos = 4, .src_neg = 7, .sense_pos = 5, .sense_neg = 8},
    [1][2] = {.src_pos = 4, .src_neg = 7, .sense_pos = 8, .sense_neg = 3},
    [1][3] = {.src_pos = 4, .src_neg = 7, .sense_pos = 3, .sense_neg = 6},
    [1][4] = {.src_pos = 4, .src_neg = 7, .sense_pos = 6, .sense_neg = 1},
    [2][0] = {.src_pos = 7, .src_neg = 2, .sense_pos = 5, .sense_neg = 8},
    [2][1] = {.src_pos = 7, .src_neg = 2, .sense_pos = 8, .sense_neg = 3},
    [2][2] = {.src_pos = 7, .src_neg = 2, .sense_pos = 3, .sense_neg = 6},
    [2][3] = {.src_pos = 7, .src_neg = 2, .sense_pos = 6, .sense_neg = 1},
    [2][4] = {.src_pos = 7, .src_neg = 2, .sense_pos = 1, .sense_neg = 4},
    [3][0] = {.src_pos = 2, .src_neg = 5, .sense_pos = 8, .sense_neg = 3},
    [3][1] = {.src_pos = 2, .src_neg = 5, .sense_pos = 3, .sense_neg = 6},
    [3][2] = {.src_pos = 2, .src_neg = 5, .sense_pos = 6, .sense_neg = 1},
    [3][3] = {.src_pos = 2, .src_neg = 5, .sense_pos = 1, .sense_neg = 4},
    [3][4] = {.src_pos = 2, .src_neg = 5, .sense_pos = 4, .sense_neg = 7},
    [4][0] = {.src_pos = 5, .src_neg = 8, .sense_pos = 3, .sense_neg = 6},
    [4][1] = {.src_pos = 5, .src_neg = 8, .sense_pos = 6, .sense_neg = 1},
    [4][2] = {.src_pos = 5, .src_neg = 8, .sense_pos = 1, .sense_neg = 4},
    [4][3] = {.src_pos = 5, .src_neg = 8, .sense_pos = 4, .sense_neg = 7},
    [4][4] = {.src_pos = 5, .src_neg = 8, .sense_pos = 7, .sense_neg = 2},
    [5][0] = {.src_pos = 8, .src_neg = 3, .sense_pos = 6, .sense_neg = 1},
    [5][1] = {.src_pos = 8, .src_neg = 3, .sense_pos = 1, .sense_neg = 4},
    [5][2] = {.src_pos = 8, .src_neg = 3, .sense_pos = 4, .sense_neg = 7},
    [5][3] = {.src_pos = 8, .src_neg = 3, .sense_pos = 7, .sense_neg = 2},
    [5][4] = {.src_pos = 8, .src_neg = 3, .sense_pos = 2, .sense_neg = 5},
    [6][0] = {.src_pos = 3, .src_neg = 6, .sense_pos = 1, .sense_neg = 4},
    [6][1] = {.src_pos = 3, .src_neg = 6, .sense_pos = 4, .sense_neg = 7},
    [6][2] = {.src_pos = 3, .src_neg = 6, .sense_pos = 7, .sense_neg = 2},
    [6][3] = {.src_pos = 3, .src_neg = 6, .sense_pos = 2, .sense_neg = 5},
    [6][4] = {.src_pos = 3, .src_neg = 6, .sense_pos = 5, .sense_neg = 8},
    [7][0] = {.src_pos = 6, .src_neg = 1, .sense_pos = 4, .sense_neg = 7},
    [7][1] = {.src_pos = 6, .src_neg = 1, .sense_pos = 7, .sense_neg = 2},
    [7][2] = {.src_pos = 6, .src_neg = 1, .sense_pos = 2, .sense_neg = 5},
    [7][3] = {.src_pos = 6, .src_neg = 1, .sense_pos = 5, .sense_neg = 8},
    [7][4] = {.src_pos = 6, .src_neg = 1, .sense_pos = 8, .sense_neg = 3},
};
#endif

#ifdef PROTOCOL_FIXED_SOURCE_280
Calibration_t pair_calibration_map[NUM_ELECTRODE_PAIRS][NUM_SENSE_PAIRS] = {0};

static void init_fixed_source_280(void) {
    uint8_t pair_idx = 0;

    for (uint8_t src = 1; src <= N_ELEC; src++) {
        for (uint8_t sink = 1; sink <= N_ELEC; sink++) {
            if (sink == src) continue;

            uint8_t remaining[N_ELEC - 2];
            uint8_t r = 0;
            for (uint8_t e = 1; e <= N_ELEC; e++) {
                if (e != src && e != sink) {
                    remaining[r++] = e;
                }
            }

            for (uint8_t s = 0; s < NUM_SENSE_PAIRS; s++) {
                pair_calibration_map[pair_idx][s].src_pos   = src;
                pair_calibration_map[pair_idx][s].src_neg   = sink;
                pair_calibration_map[pair_idx][s].sense_pos = remaining[s];
                pair_calibration_map[pair_idx][s].sense_neg = remaining[s + 1];
                pair_calibration_map[pair_idx][s].ewma_amp  = 0;
            }
            pair_idx++;
        }
    }

    ESP_LOGI(TAG, "Fixed-source scanning: %u injection pairs x %u sense = %u channels",
             pair_idx, NUM_SENSE_PAIRS, pair_idx * NUM_SENSE_PAIRS);
}
#endif

#ifdef PROTOCOL_FIXED_POS_35
Calibration_t pair_calibration_map[NUM_ELECTRODE_PAIRS][NUM_SENSE_PAIRS] = {0};

#define FIXED_SRC_POS 1

static void init_fixed_pos_35(void) {
    uint8_t pair_idx = 0;
    const uint8_t src = FIXED_SRC_POS;

    for (uint8_t sink = 1; sink <= N_ELEC; sink++) {
        if (sink == src) continue;

        uint8_t remaining[N_ELEC - 2];
        uint8_t r = 0;
        for (uint8_t e = 1; e <= N_ELEC; e++) {
            if (e != src && e != sink) {
                remaining[r++] = e;
            }
        }

        for (uint8_t s = 0; s < NUM_SENSE_PAIRS; s++) {
            pair_calibration_map[pair_idx][s].src_pos   = src;
            pair_calibration_map[pair_idx][s].src_neg   = sink;
            pair_calibration_map[pair_idx][s].sense_pos = remaining[s];
            pair_calibration_map[pair_idx][s].sense_neg = remaining[s + 1];
            pair_calibration_map[pair_idx][s].ewma_amp  = 0;
        }
        pair_idx++;
    }

    ESP_LOGI(TAG, "Fixed-pos scanning (src_pos=%u): %u injection pairs x %u sense = %u channels",
             src, pair_idx, NUM_SENSE_PAIRS, pair_idx * NUM_SENSE_PAIRS);
}
#endif

/* ===================================================================
 * Protocol init (call once before measurement starts)
 * =================================================================== */
void init_protocol(void) {
#if defined(PROTOCOL_FIXED_SOURCE_280)
    init_fixed_source_280();
    ESP_LOGI(TAG, "Protocol: FIXED_SOURCE_280 (%u channels)", TOTAL_CHANNELS);
#elif defined(PROTOCOL_FIXED_POS_35)
    init_fixed_pos_35();
    ESP_LOGI(TAG, "Protocol: FIXED_POS_35 (%u channels)", TOTAL_CHANNELS);
#elif defined(PROTOCOL_STAR_40)
    ESP_LOGI(TAG, "Protocol: STAR_40 (%u channels)", TOTAL_CHANNELS);
#elif defined(PROTOCOL_ADJACENT_40)
    ESP_LOGI(TAG, "Protocol: ADJACENT_40 (%u channels)", TOTAL_CHANNELS);
#else
    #error "No EIT protocol defined. Define one of: PROTOCOL_STAR_40, PROTOCOL_ADJACENT_40, PROTOCOL_FIXED_SOURCE_280, PROTOCOL_FIXED_POS_35"
#endif
}

/* ===================================================================
 * Tare / calibration (unchanged logic)
 * =================================================================== */

static void capture_tare_frame(uint16_t tare_values[TOTAL_TARE_MEASUREMENTS]) {
    uint8_t idx = 0;

    for (uint8_t src_elec_pair = 0; src_elec_pair < NUM_ELECTRODE_PAIRS; src_elec_pair++) {
        for (uint8_t sense_elec_pair = 0; sense_elec_pair < NUM_SENSE_PAIRS; sense_elec_pair++) {
            Calibration_t* curr_config = &pair_calibration_map[src_elec_pair][sense_elec_pair];
            uint16_t amplitude = 0;

            if (set_mux(curr_config->src_pos,
                        curr_config->src_neg,
                        curr_config->sense_pos,
                        curr_config->sense_neg) != ESP_OK) {
                #if DEBUG
                ESP_LOGE(TAG, "Failed to set mux for tare pair [%u][%u]", src_elec_pair, sense_elec_pair);
                #endif
            } else {
                vTaskDelay(1);

                uint16_t tare_samples[BUFFER_LEN] = {0};
                size_t buffer_len = sizeof(tare_samples) / sizeof(tare_samples[0]);
                if (adcRead(tare_samples, buffer_len) != ESP_OK) {
                    #if DEBUG
                    ESP_LOGE(TAG, "Failed to read tare for pair [%u][%u]", src_elec_pair, sense_elec_pair);
                    #endif
                } else {
                    amplitude = calc_std_dev_mag((int16_t*)tare_samples, buffer_len, 2);
                }
            }

            tare_values[idx++] = amplitude;
        }
    }
}

static void print_tare_frame(const uint16_t tare_values[TOTAL_TARE_MEASUREMENTS]) {
    for (uint16_t i = 0; i < TOTAL_TARE_MEASUREMENTS; i++) {
        printf("%u ", tare_values[i]);
    }
    printf("\n");
}

void calibrate(void) {
    vTaskDelay(pdMS_TO_TICKS(1000));

    if (set_src_inamp_gain(SCR_RDATA_CONST) != ESP_OK) {
        #if DEBUG
        ESP_LOGE(TAG, "Failed to set source inamp gain");
        #endif
        return;
    }

    if (set_sense_inamp_gain(SNS_RDATA_CONST) != ESP_OK) {
        #if DEBUG
        ESP_LOGE(TAG, "Failed to set fixed sense inamp gain");
        #endif
        return;
    }

    uint16_t tare_values[TOTAL_TARE_MEASUREMENTS] = {0};

    ESP_LOGI(TAG, "Printing %u tare measurements (%u channels each)", TARE_PRINT_COUNT, TOTAL_TARE_MEASUREMENTS);
    printf("TARE\n");

    for (uint8_t tare_capture = 0; tare_capture < TARE_PRINT_COUNT; tare_capture++) {
        capture_tare_frame(tare_values);
        print_tare_frame(tare_values);
    }
}
