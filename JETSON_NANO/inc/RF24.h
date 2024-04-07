#include<jetgpio.h>
#include<iostream>
#include<string.h>
#include "nrf24l01.h"
#include <unistd.h>
#include<memory>
#pragma once

class NRF24{
	public:
		NRF24();
		~NRF24();
		void enablePin(const uint8_t& pin);
		void disablePin(const uint8_t& pin);
		void TxMode(char* address, const char& channel);
		void TransmitData(char* data);
		void RxMode(char* address, const char& data);
		bool ReceiveData(char* data, const uint8_t& copySize);
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
		char spiTx[33]; //32 bytes date + 1 command
		char spiRx[33];	//32 bytes date + 1 command 
		char config{};
		char fifo{};
		char en_rxaddr{};
		char status{};
		int size{0};
		void WriteRegMulti(const char& reg,char* data,int size);
		void WriteReg(const char& reg, const char& data);
		char ReadReg(const char& reg);
		void ReadMulti(const char& reg,char* data,int size);
};
