#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/dma.h"
#include "hardware/spi.h"
#include "hardware/clocks.h"
#include "CameraOV2640.h"
#include "hardware/irq.h"
#include "ov2640.h"
#include <cstdlib>
#include "stdio.h"
#include "bsp/board.h"
#include "tusb.h"
#include "string"
#include "pico/mutex.h"
#include <iostream>

#define LED_BOARD 16
static mutex_t usb_mutex;
void SerialUsb(uint8_t* buffer,uint32_t lenght);
int SerialUSBAvailable(void);
int SerialUsbRead(void);
#define BMPIMAGEOFFSET 66
uint8_t bmp_header[BMPIMAGEOFFSET] =
{
  0x42, 0x4D, 0x36, 0x58, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x42, 0x00, 0x00, 0x00, 0x28, 0x00,
  0x00, 0x00, 0x40, 0x01, 0x00, 0x00, 0xF0, 0x00, 0x00, 0x00, 0x01, 0x00, 0x10, 0x00, 0x03, 0x00,
  0x00, 0x00, 0x00, 0x58, 0x02, 0x00, 0xC4, 0x0E, 0x00, 0x00, 0xC4, 0x0E, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF8, 0x00, 0x00, 0xE0, 0x07, 0x00, 0x00, 0x1F, 0x00,
  0x00, 0x00
};

// set pin 10 as the slave select for the digital pot:
const uint8_t CS = 13;
bool is_header = false;
int mode = 0;
uint8_t start_capture = 0;
ArduCAM myCAM( OV2640, CS );
uint8_t read_fifo_burst(ArduCAM myCAM);
int main() 
{
  gpio_init(LED_BOARD);
  gpio_set_dir(LED_BOARD, GPIO_OUT);
  bool toggle = true;
  gpio_put(LED_BOARD, toggle);
  int value=0;
  uint8_t vid, pid;
  uint8_t cameraCommand;
  stdio_init_all();
  tusb_init();
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
	while (1) {
			//Check if the ArduCAM SPI bus is OK
			myCAM.write_reg(ARDUCHIP_TEST1, 0x55);
			cameraCommand = myCAM.read_reg(ARDUCHIP_TEST1);
			if (cameraCommand != 0x55) {
				printf(" SPI interface Error!");
				sleep_ms(1000); continue;
			} else {
				printf("ACK CMD SPI interface OK.END"); break;
			}
	}
  
	while (1) {
		//Check if the camera module type is OV2640
		myCAM.wrSensorReg8_8(0xff, 0x01);
		myCAM.rdSensorReg8_8(OV2640_CHIPID_HIGH, &vid);
		myCAM.rdSensorReg8_8(OV2640_CHIPID_LOW, &pid);
		if ((vid != 0x26 ) && (( pid != 0x41 ) || ( pid != 0x42 ))) {
			printf("Can't find OV2640 module!");
			sleep_ms(1000); continue;
		}
		else {
			printf("OV2640 detected.END"); break;
		}
  }


	 //Change to JPEG capture mode and initialize the OV5642 module
  myCAM.set_format(JPEG);
  myCAM.InitCAM();
  myCAM.OV2640_set_JPEG_size(OV2640_320x240);
  sleep_ms(1000);
  myCAM.clear_fifo_flag();

  myCAM.OV2640_set_Special_effects(BW);
  usart_Command = 0xff;
  // myCAM.OV2640_set_Contrast(Contrast2);
  // myCAM.OV2640_set_Light_Mode(Sunny);
  start_capture = 2;
  mode = 2;

  while (mode == 2)
  {
    if(SerialUSBAvailable())
    {
      usart_Command=SerialUsbRead();
    }
    if (usart_Command == 0x21)
    {
      start_capture = 0;
      mode = 0;
      usart_Command=0xff;
      printf("ACK CMD CAM stop video streaming. END");
      break;
    }   
    if (start_capture == 2)
    {
      myCAM.flush_fifo();
      myCAM.clear_fifo_flag();
      //Start capture
      myCAM.start_capture();
      start_capture = 0;
    }
    if (myCAM.get_bit(ARDUCHIP_TRIG, CAP_DONE_MASK))
    {
      toggle = !toggle;
      gpio_put(LED_BOARD, toggle);
      read_fifo_burst(myCAM);
      start_capture = 2;
    }
    
  }
  gpio_put(LED_BOARD, 0);
  return 0;
}

uint8_t read_fifo_burst(ArduCAM myCAM)
{
  int i , count;
  int length = myCAM.read_fifo_length();
  //std::cout << std::to_string(length).c_str();
  printf("%.4s\n",std::to_string(length).c_str());
  uint8_t * imageBuf =(uint8_t *) malloc(length*sizeof(uint8_t));
  i = 0 ;
  myCAM.CS_LOW();
  myCAM.set_fifo_burst();//Set fifo burst mode
  spi_read_blocking(SPI_PORT, BURST_FIFO_READ,imageBuf, length);
  myCAM.CS_HIGH();
  SerialUsb(imageBuf, length);
  free(imageBuf);
  return 1;
}


void SerialUsb(uint8_t* buf,uint32_t length)
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
                n2 = tud_cdc_write(buf + i, n);
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

int SerialUSBAvailable(void)
{
  return tud_cdc_available();
} 

int SerialUsbRead(void) 
{
  if (tud_cdc_connected() && tud_cdc_available()) 
  {
    return tud_cdc_read_char();
  }
  return -1;
}