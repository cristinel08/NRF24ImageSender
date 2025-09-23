# RASPBERRY PI PICO

Developed code for sending images with RPi Pico.

## Pinouts

### NRF24L01

  NRF24   | LAYOUT
----------|-----------
CE   - 7  |  CSN  - 8
SCLK - 2  |  MOSI - 3
MISO - 4  |  IRQ  - x

### OV2640 Camera

OV2640 | GPIO
-------|--------
CS     |  13
MOSI   |  11
MISO   |  12
SCK    |  10
SDA    |  14
SCL    |  15

## Requirements

1) CMake Version 3.12
2) Pico SDK - it can now be installed as an extention in VSCode
3) arm-none-eabi 10.3.1
4) Ninja 1.12.1 (for Windows)

## Build

```bash
cmake -G "Ninja" -Bbuild&&
cmake --build build
```

Once it is done building, you drag the and drop the .hex file to the Pico directory.

Hold the boot button when inserting the Pico to the computer and there should appear a directory where you can drag and drop the .hex file.

After that, the microcontroller will begin executing the code, receive images and send them to the Jetson.
