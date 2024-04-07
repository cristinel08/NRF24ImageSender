#define JETSON_BOARD

#ifndef JETSON_BOARD
#include <jetgpio.h>
#include <unistd.h>
#else
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "tusb.h"
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
		void enablePin(const uint8_t& pin);
		void disablePin(const uint8_t& pin);
		void TxMode(char* address, const char& channel);
		void TransmitData(uint8_t* data);
		void RxMode(char* address, const char& data);
		bool ReceiveData(char* data);
		uint8_t IsDataAvailable(const uint8_t&);
		void OpenWritingPipe(char* address);
		void SendCommand(char cmd);

	private:
		//variable that checks the proper
		//use for functions in the jetson
		//lib
		int init_;
		int SPI_init_;
		int verify_;
		#ifndef JETSON_BOARD
		char spiTx[33]; //32 bytes date + 1 command
		char spiRx[33];	//32 bytes date + 1 command 
		#else
		uint8_t spiTx[33];
		uint8_t spiRx[33];
		#endif
		char fifo{};
		char status{};
		char config{};
		uint8_t size{};
		char en_rxaddr{};
		void WriteRegMulti(const char& reg,char* data, uint8_t size);
		void WriteReg(const char& reg, const char& data);
		char ReadReg(const char& reg);
		void ReadMulti(const char& reg,char* data, uint8_t size);
};
