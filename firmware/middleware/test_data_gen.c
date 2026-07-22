#include "test_data_gen.h"
#include <math.h>
#include <stdlib.h>
#include "esp_random.h"
#include <stdio.h>
#include "esp_log.h"

void generate_sine_int16(int16_t *out, float freq_hz)
{
    const float amplitude = 10000.0f;
    
    // Random start phase (0 to 2*PI)
    float start_phase = ((float)esp_random() / UINT32_MAX) * 2.0f * (float)M_PI;

    // Generate ~7 cycles worth of samples
    int num_samples = (int)(7 * TEST_GEN_FS / freq_hz);
    if (num_samples > TEST_GEN_N) num_samples = TEST_GEN_N;

    for (int n = 0; n < num_samples; n++) {
        out[n] = (int16_t)(
            amplitude * sinf((2.0f * (float)M_PI * freq_hz * n / TEST_GEN_FS) + start_phase)
        );
    }

    // Print the starting amplitude phase in degreee
    //printf("Start phase: %f degrees\n", start_phase * 180.0f / (float)M_PI);

    
    // Pad remaining samples with zero
    for (int n = num_samples; n < TEST_GEN_N; n++) {
        out[n] = 0;
    }
}

void generate_sine_int16_multi(int16_t *out, float freq_hz1, float freq_hz2, float freq_hz3)
{
    const float amplitude = 10000.0f / 3.0f;
    
    float start_phase1 = ((float)esp_random() / UINT32_MAX) * 2.0f * (float)M_PI;
    float start_phase2 = ((float)esp_random() / UINT32_MAX) * 2.0f * (float)M_PI;
    float start_phase3 = ((float)esp_random() / UINT32_MAX) * 2.0f * (float)M_PI;
    int num_samples = TEST_GEN_N;

    for (int n = 0; n < num_samples; n++) {
        float sample = amplitude * sinf((2.0f * (float)M_PI * freq_hz1 * n / TEST_GEN_FS) + start_phase1);
        sample += amplitude * sinf((2.0f * (float)M_PI * freq_hz2 * n / TEST_GEN_FS) + start_phase2);
        sample += amplitude * sinf((2.0f * (float)M_PI * freq_hz3 * n / TEST_GEN_FS) + start_phase3);
        out[n] = (int16_t)sample;
    }

    //printf("Start phase 1: %f degrees\n", start_phase1 * 180.0f / (float)M_PI);
    //printf("Start phase 2: %f degrees\n", start_phase2 * 180.0f / (float)M_PI);
    //printf("Start phase 3: %f degrees\n", start_phase3 * 180.0f / (float)M_PI);
}

void generate_sine_int16_multi_random_amp(int16_t *out, float freq_hz1, float freq_hz2, float freq_hz3)
{

    //ESP_LOGI("test_data_gen", "generate_sine_int16_multi_random_amp called");
    // Generate random amplitudes between 1000.0f and 4000.0f
    float amp1 = 1000.0f + ((float)esp_random() / UINT32_MAX) * 3000.0f;
    float amp2 = 1000.0f + ((float)esp_random() / UINT32_MAX) * 3000.0f;
    float amp3 = 1000.0f + ((float)esp_random() / UINT32_MAX) * 3000.0f;
    
    float start_phase1 = ((float)esp_random() / UINT32_MAX) * 2.0f * (float)M_PI;
    float start_phase2 = ((float)esp_random() / UINT32_MAX) * 2.0f * (float)M_PI;
    float start_phase3 = ((float)esp_random() / UINT32_MAX) * 2.0f * (float)M_PI;
    int num_samples = TEST_GEN_N;

    for (int n = 0; n < num_samples; n++) {
        float sample = amp1 * sinf((2.0f * (float)M_PI * freq_hz1 * n / TEST_GEN_FS) + start_phase1);
        sample += amp2 * sinf((2.0f * (float)M_PI * freq_hz2 * n / TEST_GEN_FS) + start_phase2);
        sample += amp3 * sinf((2.0f * (float)M_PI * freq_hz3 * n / TEST_GEN_FS) + start_phase3);
        out[n] = (int16_t)sample;
    }

    //printf("Amplitudes: %.2f, %.2f, %.2f\n", amp1, amp2, amp3);
    //printf("Start phase 1: %f degrees\n", start_phase1 * 180.0f / (float)M_PI);
    //printf("Start phase 2: %f degrees\n", start_phase2 * 180.0f / (float)M_PI);
    //printf("Start phase 3: %f degrees\n", start_phase3 * 180.0f / (float)M_PI);
}
void generate_sine_int16_multi_random_amp_clipped(int16_t *out, float freq_hz1, float freq_hz2, float freq_hz3, bool clipped, float clip_percent)
{
    //ESP_LOGI("test_data_gen", "generate_sine_int16_multi_random_amp_clipped called");
    // amp1 is dominant, fixed amplitude. Reduced to 30000 to avoid int16_t overflow (max 32767)
    float amp1 = 30000.0f;
    float amp2 = 0;
    float amp3 = 0.0;
    
    float start_phase1 = ((float)esp_random() / UINT32_MAX) * 2.0f * (float)M_PI;
    float start_phase2 = ((float)esp_random() / UINT32_MAX) * 2.0f * (float)M_PI;
    float start_phase3 = ((float)esp_random() / UINT32_MAX) * 2.0f * (float)M_PI;

    // Calculate clipping threshold for freq_hz1 only
    float clip_threshold = amp1 * clip_percent;

    int num_samples = TEST_GEN_N;

    for (int n = 0; n < num_samples; n++) {
        float sample1 = amp1 * sinf((2.0f * (float)M_PI * freq_hz1 * n / TEST_GEN_FS) + start_phase1);
        
        if (clipped) {
            if (sample1 > clip_threshold) {
                sample1 = clip_threshold;
            } else if (sample1 < -clip_threshold) {
                sample1 = -clip_threshold;
            }
        }
        
        float sample = sample1;
        sample += amp2 * sinf((2.0f * (float)M_PI * freq_hz2 * n / TEST_GEN_FS) + start_phase2);
        sample += amp3 * sinf((2.0f * (float)M_PI * freq_hz3 * n / TEST_GEN_FS) + start_phase3);

        out[n] = (int16_t)sample;
    }
}

