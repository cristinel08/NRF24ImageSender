#pragma once
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "hardware/clocks.h"
#include "CameraOV2640.h"
#include "tusb.h"
#include "string"
/*******************************************************************
 * ReceiveFrameCamera class
 * Used to start the camera and receive frames
 *******************************************************************/

class ReceiveFrameCamera
{
public:
    /*******************************************************************
     * ReceiveFrameCamera constructor
     * Initializes the pins used to communicate with the camera
     *******************************************************************/
    ReceiveFrameCamera(
        void
    );
    /*******************************************************************
     * ReadFifoCam
     * Used to start the camera to load the frame on the fifo, then
     * to get that data
     * OUT: @param length is the jpeg data size
     * returns: jpeg data pointer
     *******************************************************************/
    uint8_t* ReadFifoCam(
        uint32_t& length
    );
    /*******************************************************************
     * FreeJpegData
     * Releases the jpeg data from memory
     * IN: @param imageBuf is jpeg data pointer
     *******************************************************************/
    void FreeJpegCam(
        uint8_t* imageBuf
    );
    /*******************************************************************
     * StartCapture
     * Checks if the camera is OV2640 and prepares it to output
     * a jpeg image
     * 
     *******************************************************************/
    void StartCapture(
        void
    );
private:
    const uint8_t CS { 13 };
    bool toggle { true };
    ArduCamOv2640 myCAM { };
    bool start_capture {0};
    uint8_t* imageBuf{nullptr};
};