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
		void enablePin(int pin);
		void disablePin(int pin);
		void TxMode(char* address,char  channel);
		void TransmitData(char* data);
		void RxMode(char* address,char data);
		bool ReceiveData(char* data);
		uint8_t IsDataAvailable(int);
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

		void WriteRegMulti(char reg,char* data,int size);
		void WriteReg(char reg,char data);
		char ReadReg(char reg);
		void ReadMulti(char reg,char* data,int size);
};
