idf_component_register(
    SRCS "AD7450_ADC.c" "AD5930_SigGen.c" "AD5270_DigiPot.c" "ADG738_MUX.c"
    INCLUDE_DIRS "."
    REQUIRES driver esp_timer
)
