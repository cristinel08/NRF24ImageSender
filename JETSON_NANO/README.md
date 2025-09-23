# JETSON NANO

Developed code for Jetson Nano.

## NRF24 Pinout

  NRF24   | LAYOUT
----------|-----------
GND  - 20 |  VCC  - 17
CE   - 15 |  CSN  - 24
SCLK - 23 |  MOSI - 19
MISO - 21 |  IRQ  - x

## Requirements
  
  1) CMake minimum version 3.10. To install the lastest cmake version you can do the following:
     1) Download the latest Unix/Linux cmake*.tar.gz version: <https://cmake.org/download/>
     2) Extract the files

        ```bash
        cd Downloads&&
        tar -xzvf cmake*.tar.gz&&
        cd <cmake_dir>
        ```

     3) Compile and intall with:

        ```bash
        ./boostrap
        make -j4
        make install
        ```

     4) Check instalation with:

        ```bash
        cmake --version
        ```

        If it failed to install, you can try again with `sudo`.

        ```bash
        sudo make install
        ```

  2) JETGPIO Library to acces the jetson gpio pins. You can follow the instructions from their github page: <https://github.com/Rubberazer/JETGPIO>

  3) You will need to activate, download, and activate the spi. The JetPack doesn't include SPI so will need to do this. I followed the steps from the jetson forums: <https://forums.developer.nvidia.com/t/how-can-i-enable-spi-communication-on-jetson-nano-module/229228/6>

  4) OpenCV 4.x. You can install it by following this link: <https://automaticaddison.com/how-to-install-opencv-4-5-on-nvidia-jetson-nano/>

## Build

  To build this project for NVIDIA Jetson Nano run:

  ```bash
    cmake -Bbuild&&
    cmake --build build
  ```

  Once you managed to build the project, you can go ahead and start the executable. This will launch the application and start the NRF to receive any data that is thrown at it.

  ```bash
    cd build&&
    ./nrf24l01
  ```
