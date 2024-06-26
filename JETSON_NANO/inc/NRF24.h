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
	NRF24(
		void
	);
	~NRF24(
		void
	);
	void TxMode(
		uint8_t* address, 
		const uint8_t& channel
	);

	bool TransmitData(
		uint8_t* data
	);

	void RxMode(
		uint8_t* address, 
		const uint8_t& data
	);

	bool ReceiveData(
		uint8_t* data, 
		const uint8_t& lenData, 
		bool& startFrame
	);

	uint8_t IsDataAvailable(
		const uint8_t&
	);

	void OpenWritingPipe(
		uint8_t* address
	);

private:
	void SendCommand(
		const uint8_t& cmd
	);
	void EnablePin(
		const uint8_t& pin
	);
	void Set2Tx(
		void
	);
	void Set2Rx(
		void
	);
	void DisablePin(
		const uint8_t& pin
	);
	void WriteRegMulti(
		const uint8_t& reg,
		uint8_t* data, 
		uint8_t size
	);
	void WriteReg(
		const uint8_t& reg, 
		const uint8_t& data
	);
	uint8_t ReadReg(
		const uint8_t& reg
	);

	uint8_t GetStatus(
		void
	);

	void ReadMulti(
		const uint8_t& reg,
		uint8_t* data, 
		uint8_t size
	);
	
	int init_;
	int SPI_init_;
	int verify_;
	uint8_t spiTx[33]; //32 bytes date + 1 command
	uint8_t spiRx[33];	//32 bytes date + 1 command 
	uint8_t status{};
	bool receive{false};
	bool record{false};
	bool resetRxIrq{false};
};
