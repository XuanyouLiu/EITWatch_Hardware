# EITWatch firmware

ESP-IDF firmware for the EITWatch analog front end. Targets a **Seeed XIAO ESP32-S3** on headers `P2`/`P3` and drives:

| Part | Role |
|------|------|
| AD5930 | 50 kHz waveform generator |
| ADG738 × 4 | Electrode matrix switches |
| AD5270 × 2 | Drive / sense gain pots |
| AD7450 | 12-bit SPI ADC |

Board files and BOM: see [`../README.md`](../README.md).

## Layout

```text
firmware/
├── main/            # app entry, calibration, measurement task
├── middleware/      # HAL, mux/SPI orchestration, hardware tests
├── drivers/         # AD5930, AD5270, ADG738, AD7450 drivers
├── CMakeLists.txt
└── sdkconfig.defaults
```

Pin macros live in [`drivers/board_pins.h`](drivers/board_pins.h) (XIAO ESP32-S3 GPIO numbers for pads `D0`–`D10`).

## Requirements

- [ESP-IDF](https://docs.espressif.com/projects/esp-idf/) **v5.2+**
- USB data cable to the XIAO

## Build, flash, monitor

```bash
# 1) Load ESP-IDF (path may differ on your machine)
. $HOME/esp/esp-idf/export.sh

# 2) Build for the XIAO ESP32-S3
cd firmware
idf.py set-target esp32s3
idf.py build

# 3) Flash and open the serial monitor
#    macOS:  ls /dev/tty.usbmodem*
#    Linux:  ls /dev/ttyACM*
idf.py -p /dev/tty.usbmodemXXXX flash monitor
```

Detach the monitor with `Ctrl+]`.

After flash, `app_main` initializes SPI, ADC, muxes, digipots, and the 50 kHz source, then starts the FreeRTOS measurement task. Frames are printed / streamed over USB Serial JTAG.

### Quick bring-up checklist

1. Seat the XIAO with the USB port oriented as on the silkscreen.
2. Power the board and confirm the port appears.
3. Flash with the commands above.
4. For first-article boards, temporarily call `test_function()` from `app_main` (see `middleware/hardware-test.c`) to exercise ADC, signal generator, and mux paths before running the full schedule.

## License

Firmware is licensed under the [Apache License 2.0](LICENSE). Hardware design files in the parent directory use CERN-OHL-P-2.0; see [`../LICENSE`](../LICENSE).
