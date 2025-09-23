# NRF24ImageSender

This project searches to facilitate the wireless image transmission using limited hardware for transmission, such as NRF24L01. The goal is to achieve a simplified system that does not require wifi to transmit images between points.

---

## Table of Contents

- [NRF24ImageSender](#nrf24imagesender)
  - [Table of Contents](#table-of-contents)
  - [Overview](#overview)
  - [Features](#features)
  - [Hardware Requirements](#hardware-requirements)
  - [Software Requirements](#software-requirements)
  - [Results](#results)
  - [Future Developments](#future-developments)
    - [Software](#software)
    - [Hardware](#hardware)

---

## Overview

- This project is looking to achieve a short distance transmission for low resolution images.
- This project was intended for an interaction between a RPi Pico (sender) and a NVIDIA Jetson Nano (receiver).
- The goal for such device is to provide insightfull data, that can be further processed by a more advance system and offer object detections,frame saving, or any other advance image processing.

---

## Features

- Real-time data/image processing
- Embedded Systems
- Easy-to-build with CMake

---

## Hardware Requirements

- NRF24L01 wireless modules
- Arducam Mini 2MP Plus - OV2640
- RPi Pico development board
- NVIDIA Jetson Nano development board

---

## Software Requirements

| Platform    | Requirements                         |
|-------------|--------------------------------------|
| **Jetson**  | GCC, CMake, OpenCV      |
| **RPi Pico**| RPi Pico SDK, CMake     |

---

## Results

Achieved a wireless image transmission. The frames were saved individually on Jetson and the video was obtained by combining the images into a single file, achieving the following video:

<!-- <video width="320" height = "240" controls>
<source src="https://raw.githubusercontent.com/username/repo/branch/JETSON_NANO/imgs/out.mp4">
</video> -->

[Download video](JETSON_NANO/imgs/out.mp4)

In this version the frame-rate does not offer advantages, but such a system can still be used to monitor crops, for example. The limitation with the NRF24L01 is noticeble, a higher frame-rate can be obtained by sacrificing the clarity of the image and modify the quantization and resolution of the JPEG image, but this offers a drawback in terms of clarity, making it harder to detect various objects.

## Future Developments

### Software

- Implement a TCP/IP, socket programming to send the frames to a server and visuale them on the web.

- Eliminate the redundant code. For example, keeping a higher version code for the NRF24L01.

- Integrate the YOLO (You Only Look Once), and train it to detect soil moisture.

### Hardware

- Integrate a solar pannel and charge the LIPO battery of the RPi Pico
