#include "CameraLib.h"
#include "ov2640.h"
#define LED_BOARD 16

CameraLib::CameraLib()
{
    stdio_init_all();
    tusb_init();
    gpio_init(LED_BOARD);
    gpio_set_dir(LED_BOARD, GPIO_OUT);
    toggle = true;
    gpio_put(LED_BOARD, toggle);
	// put your setup code here, to run once:
	myCAM.Arducam_init();	
    gpio_init(CS);
    gpio_set_dir(CS, GPIO_OUT);
    gpio_put(CS, 1);
    //Reset the CPLD
    myCAM.write_reg(0x07, 0x80);
    sleep_ms(100);
    myCAM.write_reg(0x07, 0x00);
    sleep_ms(100);
}

uint8_t* CameraLib::ReadFifoCam(int& length)
{
    if(!start_capture)
    {
        myCAM.flush_fifo();
        myCAM.clear_fifo_flag();
        myCAM.start_capture();
        start_capture = 1;
    }
    if (myCAM.get_bit(ARDUCHIP_TRIG, CAP_DONE_MASK))
    {
        toggle = !toggle;
        gpio_put(LED_BOARD, toggle);
        length = myCAM.read_fifo_length();
        //std::cout << std::to_string(length).c_str();
        imageBuf = new uint8_t[length];//*sizeof(uint8_t));
        myCAM.CS_LOW();
        myCAM.set_fifo_burst();//Set fifo burst mode
        spi_read_blocking(SPI_PORT, BURST_FIFO_READ, imageBuf, length);
        myCAM.CS_HIGH();
        // SerialUsb(imageBuf, length);
        // FreeFifoCam();
        // delete[] *imageBuf;
        start_capture = 0;
        return imageBuf;
    }
    return nullptr;
}

void CameraLib::FreeFifoCam(uint8_t* imageBuf_)
{
    delete[] imageBuf_;
}

void CameraLib::StartCapture()
{
    while (1)
    {
        //Check if the ArduCAM SPI bus is OK
        myCAM.write_reg(ARDUCHIP_TEST1, 0x55);
        cameraCommand = myCAM.read_reg(ARDUCHIP_TEST1);
        if (cameraCommand != 0x55) 
        {
            printf(" SPI interface Error!");
            sleep_ms(1000); continue;
        } else 
        {
            printf("ACK CMD SPI interface OK.END"); break;
        }
    }
  
	while (1) 
    {
		//Check if the camera module type is OV2640
		myCAM.wrSensorReg8_8(0xff, 0x01);
		myCAM.rdSensorReg8_8(OV2640_CHIPID_HIGH, &vid);
		myCAM.rdSensorReg8_8(OV2640_CHIPID_LOW, &pid);
		if ((vid != 0x26 ) && (( pid != 0x41 ) || ( pid != 0x42 )))
        {
			printf("Can't find OV2640 module!");
			sleep_ms(1000); 
            continue;
		}
		else 
        {
			printf("OV2640 detected.END"); 
            break;
		}
  }
    //Change to JPEG capture mode and initialize the OV5642 module
    myCAM.set_format(JPEG);
    myCAM.InitCAM();
    myCAM.OV2640_set_JPEG_size(OV2640_640x480);
    // sleep_ms(1000);
    myCAM.clear_fifo_flag();
    myCAM.OV2640_set_Special_effects(Normal);
    // myCAM.OV2640_set_Special_effects(BW);
    // myCAM.OV2640_set_Contrast(Contrast2);
    // myCAM.OV2640_set_Light_Mode(Sunny);

}

void CameraLib::SerialUsb(uint8_t* imageBuf_, const int& length)
{
    static uint64_t last_avail_time{};
    int n{};
    int avail{};
    int n2{};
    if (tud_cdc_connected()) 
    {
        for (int i = 0; i < length;) 
        {
            n = length - i;
            avail = tud_cdc_write_available();
            if (n > avail) n = avail;
            if (n) 
            {
                n2 = tud_cdc_write(imageBuf_ + i, n);
                tud_task();
                tud_cdc_write_flush();
                i += n2;
                last_avail_time = time_us_64();
            } 
            else 
            {
                tud_task();
                tud_cdc_write_flush();
                if (!tud_cdc_connected() ||
                    (!tud_cdc_write_available() && time_us_64() > last_avail_time + 1000000 /* 1 second */)) {
                    break;
                }
            }
        }
    } 
    else 
    {
        // reset our timeout
        last_avail_time = 0;
    }
}

int CameraLib::SerialUSBAvailable()
{
    return tud_cdc_available();
    return 0;
}

int CameraLib::SerialUsbRead()
{
    if (tud_cdc_connected() && tud_cdc_available()) 
    {
    return tud_cdc_read_char();
    }
    return -1;
}