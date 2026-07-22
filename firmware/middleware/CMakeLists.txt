idf_component_register(
    SRCS "hardware.c" "hardware-test.c" "test_data_gen.c"
    INCLUDE_DIRS "."
    REQUIRES drivers driver esp-dsp esp_adc
)
