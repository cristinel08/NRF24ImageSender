#include<jetgpio.h>
#include<iostream>
#include<string.h>
#include "nrf24l01.h"
#include <unistd.h>
#include <thread>
#include <chrono>
#include <memory>
#pragma once
/*******************************************************************
 * NRF24 class
 * Takes care of the NRF24L01 radio module
 *******************************************************************/
class NRF24{

public:
	/*******************************************************************
	 * NRF24()
	 * NRF24 constructor that configures the radio module and verifies 
	 * the spi interface
	 *******************************************************************/
	NRF24(
		void
	);
	/*******************************************************************
	 * ~NRF24()
	 * NRF24 destructor shutdowns the radio module and clears Jetson's
	 * pins
	 *******************************************************************/
	~NRF24(
		void
	);
	/*******************************************************************
	 * TxMode()
	 * Used to set the radio module in Tx mode
	 * IN: @param address needs to be the same with the RX	of the other module
	 * IN: @param channel needs to be the same with the other NRF24
	 *******************************************************************/
	void TxMode(
		uint8_t* address, 
		uint8_t const& channel
	);
	/*******************************************************************
	 * TransmitData()
	 * Used to transmit maxim 32 bytes of data to the receiver
	 * In case the data's size is lower then 32, then it would
	 * fill the rest with 0 
	 * IN: @param data represents the pixels that are gonna be transmitted
	 * IN: @param dataSize is the size of the pixels
	 *******************************************************************/
	bool TransmitData(
		uint8_t* data
	);
	/*******************************************************************
	 * RxMode()
	 * Used to set the radio module in Rx mode
	 * IN: @param address needs to be the same with the TX	of the other module
	 * IN: @param channel needs to be the same with the other NRF24
	 *******************************************************************/
	void RxMode(
		uint8_t* address, 
		uint8_t const& data
	);
	/*******************************************************************
	 * ReceiveData()
	 * Used to receive data
	 * 
	 * OUT: @param data maxim 32 pixels received
	 * @return: false if the size of the rx_fifo is empty else true
	 *******************************************************************/
	bool ReceiveData(
		uint8_t* data, 
		uint8_t const& lenData, 
		bool& startFrame
	);
	/*******************************************************************
	 * IsDataAvailable()
	 * Returns true if there are any data on the pipe number.
	 * There can be maximum 6 pipes waiting for data
	 * IN: @param pipeNr checks on this pipe number
	 * @return: true if there are data in the pipe else false
	 *******************************************************************/
	uint8_t IsDataAvailable(
		uint8_t const&
	);
	/*******************************************************************
	 * OpenWritingPipe()
	 * Used setup the Tx address and starts to listen for Ack on pipe 0
	 * IN: @param address same as TxMode
	 *******************************************************************/
	void OpenWritingPipe(
		uint8_t* address
	);

private:
	/*******************************************************************
	 * SendCommand()
	 * Used to send commands to the radio module. Commands like FLUSH_FIFO
	 * IN: @param cmd is the command sent to the module
	 *******************************************************************/
	void SendCommand(
		uint8_t const& cmd
	);
	/*******************************************************************
	 * EnablePin()
	 * Used to sets a pin high
	 * IN: @param pin represents the pin number of the GPIO
	 *******************************************************************/
	void EnablePin(
		uint8_t const& pin
	);
	/*******************************************************************
	 * DisablePin()
	 * Used to sets a pin low
	 * IN: @param pin represents the pin number of the GPIO
	*******************************************************************/
	void DisablePin(
		uint8_t const& pin
	);	
	/*******************************************************************
	 * Set2Tx()
	 * Used to set the radio from Rx to Tx 
	*******************************************************************/
	void Set2Tx(
		void
	);
	/*******************************************************************
	 * Set2Rx()
	 * Used to set the radio from Tx to Rx 
	*******************************************************************/
	void Set2Rx(
		void
	);
	/*******************************************************************
	 * WriteRegMulti()
	 * Used to Write multiple values to one register. 
	 * Such register could be the addresses used to communicate
	 * IN: @param reg is the address of the register
	 * IN: @param data the values to set the register
	 * IN: @param size the size of the values 
	*******************************************************************/
	void WriteRegMulti(
		uint8_t const& reg,
		uint8_t* data, 
		uint8_t size
	);
	/*******************************************************************
	 * WriteReg()
	 * Used to Write a value to one register.
	 * IN: @param reg is the address of the register
	 * IN: @param data the value to set the register
	*******************************************************************/
	void WriteReg(
		uint8_t const& reg, 
		uint8_t const& data
	);
	/*******************************************************************
	 * ReadReg()
	 * Used to Read a value from one register.
	 * IN: @param reg is the address of the register
	 * @return: the value of the register
	*******************************************************************/
	uint8_t ReadReg(
		uint8_t const& reg
	);
	/*******************************************************************
	 * GetStatus()
	 * Used to get the status of the radio module
	 * @return: the current status of the device
	*******************************************************************/
	uint8_t GetStatus(
		void
	);
	/*******************************************************************
	 * ReadMulti()
	 * Used to read multiple data from one register.
	 * 
	 * IN: @param reg is the address of the register
	 * OUT: @param data - is the values of the register
	 * IN: @param size - number of values to read from the register
	*******************************************************************/
	void ReadMulti(
		uint8_t const& reg,
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
