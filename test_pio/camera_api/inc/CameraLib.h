#pragma once
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "hardware/clocks.h"
#include "CameraOV2640.h"
// #include "tusb.h"
#include "string"

class CameraLib
{
    public:
        CameraLib();
        uint8_t* ReadFifoCam(int& length);
        void FreeFifoCam(uint8_t* imageBuf_);
        void StartCapture();
        void SerialUsb(uint8_t* imageBuf_, const int& length);
    private:
        int SerialUSBAvailable(void);
        int SerialUsbRead(void);

    private:
        const uint8_t CS { 13 };
        bool toggle { true };
        ArduCAM myCAM { OV2640, CS };
        uint8_t start_capture {0};
        uint8_t vid{};
        uint8_t pid{};
        uint8_t* imageBuf{nullptr};
        uint8_t cameraCommand{};
};