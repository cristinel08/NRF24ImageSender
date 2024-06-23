#include<jetgpio.h>
#include<iostream>
#include<string.h>
#include "nrf24l01.h"
#include <unistd.h>
#include <thread>
#include <chrono>
#include <memory>
#pragma once

class NRF24{
	public:
		NRF24();
		~NRF24();
		void enablePin(const uint8_t& pin);
		void disablePin(const uint8_t& pin);
		void TxMode(char* address, const char& channel);
		bool TransmitData(char* data);
		void RxMode(char* address, const char& data);
		void Set2Tx();
		void Set2Rx();
		bool ReceiveData(char* data, const uint8_t& lenData, bool& startFrame);
		uint8_t IsDataAvailable(const uint8_t&);
		void OpenWritingPipe(char* address);
		void SendCommand(const char& cmd);
		void StopTransferring();
		void StartTransferring();
		void ResetRxIrq();


	private:
		//variable that checks the proper
		//use for functions in the jetson
		//lib
		int init_;
		int SPI_init_;
		int verify_;
		char spiTx[33]; //32 bytes date + 1 command
		char spiRx[33];	//32 bytes date + 1 command 
		char status{};
		bool receive{false};
		bool record{false};
		bool resetRxIrq{false};

		void WriteRegMulti(const char& reg,char* data, uint8_t size);
		void WriteReg(const char& reg, const char& data);
		char ReadReg(const char& reg);
		char GetStatus();
		void ReadMulti(const char& reg,char* data, uint8_t size);
};
