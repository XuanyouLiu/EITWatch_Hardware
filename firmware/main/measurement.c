#include <stdio.h>
#include <math.h>

#include "measurement.h"
#include "calibration.h"
#include "hardware.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_rom_sys.h"

static const char *TAG = "MEASUREMENT";

#define TOTAL_MEASUREMENTS (NUM_ELECTRODE_PAIRS * NUM_SENSE_PAIRS)

/** Alpha for per-channel EWMA smoothing (0..1). 1.0 ~= near bypass. */
#define AMP_FILTER_ALPHA  1.0f

uint16_t adc_packet_buffers[MAX_ADC_PACKETS][ADC_READINGS_PER_PACKET] = {0};


/* -----------------------------------------------------------------------
 * FIR bandpass filter (Hamming window) was used here previously, but is
 * now disabled. The code below has been intentionally commented out to
 * simplify the signal path.
 *
 * // #define FIR_NUM_TAPS  21
 * // #define ADC_BUF_LEN   64
 * // #define FIR_OUT_LEN   (ADC_BUF_LEN - FIR_NUM_TAPS + 1)
 * //
 * // static const float fir_coeffs[FIR_NUM_TAPS] = { ... };
 * //
 * // static void fir_filter(const int16_t *in, float *out) { ... }
 * ----------------------------------------------------------------------- */
#define ADC_BUF_LEN   64

static uint16_t calc_peak_to_peak_with_stats(uint16_t *out_min, uint16_t *out_max, uint16_t *out_mean) {
    uint16_t raw[ADC_BUF_LEN];

    if (adcRead(raw, ADC_BUF_LEN) != 0) {
        ESP_LOGE(TAG, "Failed to read ADC samples");
        return 0;
    }

    if (out_min != NULL && out_max != NULL && out_mean != NULL) {
        uint16_t min_v = raw[0];
        uint16_t max_v = raw[0];
        uint32_t sum_v = 0;
        for (uint16_t i = 0; i < ADC_BUF_LEN; i++) {
            uint16_t v = raw[i];
            if (v < min_v) min_v = v;
            if (v > max_v) max_v = v;
            sum_v += v;
        }
        *out_min = min_v;
        *out_max = max_v;
        *out_mean = (uint16_t)(sum_v / ADC_BUF_LEN);
    }

    /* FIR path disabled: compute amplitude directly from raw samples. */
    return calc_std_dev_mag((int16_t *)raw, ADC_BUF_LEN, 1);
}

uint16_t calc_peak_to_peak(void) {
    return calc_peak_to_peak_with_stats(NULL, NULL, NULL);
}

void measurement_task(void* args) {
    const uint16_t total_measurements = NUM_ELECTRODE_PAIRS * NUM_SENSE_PAIRS;

    ESP_LOGI(TAG, "Measurement task starting");

    set_src_inamp_gain(SCR_RDATA_CONST);
    set_sense_inamp_gain(SNS_RDATA_CONST);

    while (1) {
        for (uint8_t src_elec_pair = 0; src_elec_pair < NUM_ELECTRODE_PAIRS; src_elec_pair++) {
            for (uint8_t sense_elec_pair = 0; sense_elec_pair < NUM_SENSE_PAIRS; sense_elec_pair++) {
                Calibration_t* curr_config = &pair_calibration_map[src_elec_pair][sense_elec_pair];

                if (set_mux(curr_config->src_pos, curr_config->src_neg,
                            curr_config->sense_pos, curr_config->sense_neg) != ESP_OK) {
                    ESP_LOGE(TAG, "Failed to set mux");
                    continue;
                }

                esp_rom_delay_us(50);
        // vTaskDelay(pdMS_TO_TICKS(700));

                uint16_t amplitude = 0;
                amplitude = calc_peak_to_peak();
                float prev = (float)curr_config->ewma_amp;
                float smoothed = (1.0f - AMP_FILTER_ALPHA) * prev + AMP_FILTER_ALPHA * (float)amplitude;
                curr_config->ewma_amp = (uint16_t)smoothed;
            }

        }

        for (uint16_t i = 0; i < total_measurements; i++) {
            Calibration_t* cfg = &pair_calibration_map[i / NUM_SENSE_PAIRS][i % NUM_SENSE_PAIRS];
            printf("%u ", (unsigned)cfg->ewma_amp);
        }
        printf("\n");

        #if DEBUG
        ESP_LOGI(TAG, "End of Cycle");
        #endif
    }
}
