# NRF24ImageSender
Bachellor Degree Project using nrf24l01 sending images from a raspberry pi pico to a jetson nano
## NRF24 PINOUT FOR JETSON NANO:
  NRF24   | LAYOUT
----------|-----------
  CE - 18 | CSN - 16 
SCLK - 21 |  MOSI - 19
MISO - 23 |  IRQ -

## Must have:
  <p>1) CMake minimum version 3.10 ( enter their website and download the Unix/Linux cmake*.tar.gz):</p>
  <p>https://cmake.org/download/</p>

  ```bash
  cd ~/Downloads
  tar -xzvf cmake*.tar.gz
  ```
  it would create a dir with the same name enter in it:
  ```bash
  cd <name of the cmake_dir>
  ```
  then execute the script bootstrap and compile and install it:
  ```bash
  ./bootstrap
  make -j4
  make install
  ```
  if the make install doesn't work try with:
  ```bash
  sudo make install
  ```
  then check if it installed:
  ```bash
  cmake --version
  ```
  2) JETGPIO Library to acces the jetson gpio pins. You can follow the instructions from this github page:
     https://github.com/Rubberazer/JETGPIO

  3) You will need to activate download and activate the spi. The JetPack doesn't include SPI so will need to do this. I followed the steps from the jetson forums:
     https://forums.developer.nvidia.com/t/how-can-i-enable-spi-communication-on-jetson-nano-module/229228/6


##What I've done so far:
  1) Installed the OS on the Jetson Nano and installed the spi drivers.
  1) Using the NRF24L01+ and the library RF24 as an example, I managed to create my own libray to receive data from the pico.
  2) Tested the time to transmit a 128x96 image between the RF24 library and my library (the RF24 was faster, but it lead to strange functionality on the Jetson).
