idf_component_register(
    SRCS "main.c" "calibration.c" "measurement.c"
    INCLUDE_DIRS "."
    REQUIRES middleware driver
)
