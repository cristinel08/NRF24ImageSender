#define JETSON_BOARD

#ifndef JETSON_BOARD
#include <jetgpio.h>
#include <unistd.h>
#define UINT8 char
#else
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "tusb.h"
#define UINT8 uint8_t
#define LED_BOARD 16
#endif
#include <iostream>
#include <string.h>
#include <memory>
#include "nrf24l01.h"
#pragma once

class NRF24{
	public:
		NRF24();
		~NRF24();
		void enablePin(int pin);
		void disablePin(int pin);
		void TxMode(UINT8* address,UINT8 channel);
		void Set2Rx();
		void Set2Tx();
		void TransmitData(UINT8* data);
		void RxMode(UINT8* address,UINT8 data);
		bool ReceiveData(UINT8* data);
		UINT8 IsDataAvailable(int);
		void OpenWritingPipe(UINT8* address);
		void SendCommand(UINT8 cmd);

	private:
		//variable that checks the proper
		//use for functions in the jetson
		//lib
		int init_;
		int SPI_init_;
		int verify_;
		UINT8 spiTx[33]; //32 bytes date + 1 command
		UINT8 spiRx[33];	//32 bytes date + 1 command 
		void WriteRegMulti(UINT8 reg, UINT8* data, int size);
		void WriteReg(UINT8 reg,UINT8 data);
		UINT8 ReadReg(UINT8 reg);
		void ReadMulti(UINT8 reg,UINT8* data,int size);
};
