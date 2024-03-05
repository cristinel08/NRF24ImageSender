#pragma once

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/clocks.h"
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "hardware/spi.h"
#include "pico/binary_info.h"


#define JPEG	1

#define OV2640_160x120 		0	//160x120
#define OV2640_176x144 		1	//176x144
#define OV2640_320x240 		2	//320x240
#define OV2640_352x288 		3	//352x288
#define OV2640_640x480		4	//640x480
#define OV2640_800x600 		5	//800x600
#define OV2640_1024x768		6	//1024x768
#define OV2640_1280x1024	7	//1280x1024
#define OV2640_1600x1200	8	//1600x1200

#define MAX_FIFO_SIZE		0x5FFFF			//384KByte

#define RWBIT               0x80  //READ AND WRITE BIT IS 
                                  //BIT[7] WITH SPI FOR ArduChip

#define ARDUCHIP_FRAMES	    0x01  //FRAME control register, 
                                  //Bit[2:0] = Number of frames 
                                  //to be captured	

#define ARDUCHIP_TIM       	0x03  //Timming control	

#define ARDUCHIP_FIFO      	0x04  //FIFO and I2C control
#define FIFO_CLEAR_MASK    	0x01
#define FIFO_START_MASK    	0x02
#define FIFO_RDPTR_RST_MASK 0x10
#define FIFO_WRPTR_RST_MASK 0x20

/*spi pin source*/
#define SPI_PORT spi1
#define PIN_SCK  14
#define PIN_MOSI 11
#define PIN_MISO 12
#define PIN_CS   13

/*i2c pin source */
#define I2C_PORT i2c1
#define PIN_SDA  26
#define PIN_SCL  27
#define WRITE_BIT 0x80

#define UART_ID uart0
#define BAUD_RATE 921600
#define DATA_BITS 8
#define STOP_BITS 1
#define PARITY    UART_PARITY_NONE
#define UART_TX_PIN 0
#define UART_RX_PIN 1


struct sensor_reg {
	uint16_t reg;
	uint16_t val;
};


class CameraOV2640
{

public:
    CameraOV2640();
    
    void Init();

    void CsSelect();

    void CsUnselect();

    void WriteRegSPI(
        uint8_t addr, 
        uint8_t data
        );
    
    void WriteRegI2c(
        uint8_t regID,
        uint8_t regDat
        );

    uint8_t ReadRegSPI(
        uint8_t addr
        );

    void ReadRegI2c(
        uint8_t regID, 
        uint8_t* regDat
        );

private:
    static void on_uart_rx(void* userData);

private:
    const uint8_t cameraSensorAddr_ { 0x30 };
    uint8_t cameraCommand { 0 };
    uint8_t readSPI_;
    uint8_t buf_[2] {};
    uint8_t idH_ {};
    uint8_t idL_ {};


};