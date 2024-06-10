#define PICO

#ifndef PICO
#include <jetgpio.h>
#include <unistd.h>
#define UINT8 char
#else
#include "pico/stdlib.h"
#include "hardware/spi.h"
#define UINT8 uint8_t
#define LED_BOARD 16
#define millis() to_ms_since_boot(get_absolute_time())
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
		void TxMode(UINT8* address, const UINT8& channel);
		void Set2Rx();
		void Set2Tx();
		bool TransmitData(UINT8* data, uint8_t const& dataSize);
		void RxMode(UINT8* address, const UINT8& data);
		bool ReceiveData(UINT8* data);
		UINT8 IsDataAvailable(const uint8_t&);
		void OpenWritingPipe(UINT8* address);
		void SendCommand(const UINT8& cmd);

	private:
		void WriteRegMulti(const UINT8& reg, UINT8* data, int size);
		void WriteReg(const UINT8& reg, const UINT8& data);
		UINT8 ReadReg(const UINT8& reg);
		UINT8 GetStatus();
		void ReadMulti(const UINT8& reg, UINT8* data, int size);
		#ifndef PICO
		int init_;
		int SPI_init_;
		int verify_;
		#endif
		UINT8 configReg{};
		UINT8 channel{};
		UINT8 status;
		uint8_t lenData;
		UINT8 spiTx[33]; //32 bytes date + 1 command
		UINT8 spiRx[33];	//32 bytes date + 1 command 
};
