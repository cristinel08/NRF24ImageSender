#include "ReceiveFrameCamera.h"
#include "ov2640.h"
#define LED_BOARD 16

ReceiveFrameCamera::ReceiveFrameCamera(
    void
)
{
    stdio_init_all();
    gpio_init(LED_BOARD);
    gpio_set_dir(LED_BOARD, GPIO_OUT);
    toggle = true;
    gpio_put(LED_BOARD, toggle);
	myCAM.ArduCamInit(CS);	
}

uint8_t* ReceiveFrameCamera::ReadFifoCam(
    uint32_t& length
)
{
    if(!start_capture)
    {
        myCAM.FlushFifo();
        myCAM.ClearFifoFlag();
        myCAM.StartCapture();
        start_capture = 1;
    }
    if (myCAM.GetBit(ARDUCHIP_TRIG, CAP_DONE_MASK))
    {
        toggle = !toggle;
        gpio_put(LED_BOARD, toggle);
        length = myCAM.ReadFifoLength();
        imageBuf = new uint8_t[length];
        myCAM.GetFrame(&imageBuf, length);
        start_capture = 0;
        return imageBuf;
    }
    return nullptr;
}

void ReceiveFrameCamera::FreeJpegImg(
    uint8_t* imageBuf_
)
{
    delete[] imageBuf_;
}

void ReceiveFrameCamera::StartCapture(
    void
)
{
    myCAM.CheckOV2640Camera();
    //Change to JPEG capture mode and initialize the OV5642 module
    myCAM.SetFormat(JPEG);
    myCAM.InitCAM();
    myCAM.SetJpegSize(OV2640_640x480);
    myCAM.ClearFifoFlag();
    myCAM.SetSpecialEffects(Normal);
    myCAM.SetQsFactor(0x3F);

}