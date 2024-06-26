#include "pico/stdlib.h"
#include "hardware/spi.h"
#define UINT8 uint8_t
#define LED_BOARD 16
#define millis() to_ms_since_boot(get_absolute_time())
#include <iostream>
#include <string.h>
#include <memory>
#include "nrf24l01.h"
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
	 * NRF24 destructor shutdowns the radio module and clears the RP2040
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
		UINT8* address, 
		UINT8 const& channel
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
		UINT8* data, 
		uint8_t const& dataSize
	);
	/*******************************************************************
	 * RxMode()
	 * Used to set the radio module in Rx mode
	 * IN: @param address needs to be the same with the TX	of the other module
	 * IN: @param channel needs to be the same with the other NRF24
	 *******************************************************************/
	void RxMode(
		UINT8* address, 
		UINT8 const& data
	);
	/*******************************************************************
	 * ReceiveData()
	 * Used to receive data
	 * 
	 * OUT: @param data maxim 32 pixels received
	 * @return: false if the size of the rx_fifo is empty else true
	 *******************************************************************/
	bool ReceiveData(
		UINT8* data
	);
	/*******************************************************************
	 * IsDataAvailable()
	 * Returns true if there are any data on the pipe number.
	 * There can be maximum 6 pipes waiting for data
	 * IN: @param pipeNr checks on this pipe number
	 * @return: true if there are data in the pipe else false
	 *******************************************************************/
	UINT8 IsDataAvailable(
		UINT8 const& pipeNr
	);
	/*******************************************************************
	 * OpenWritingPipe()
	 * Used setup the Tx address and starts to listen for Ack on pipe 0
	 * IN: @param address same as TxMode
	 *******************************************************************/
	void OpenWritingPipe(
		UINT8* address
	);
private:
	/*******************************************************************
	 * SendCommand()
	 * Used to send commands to the radio module. Commands like FLUSH_FIFO
	 * IN: @param cmd is the command sent to the module
	 *******************************************************************/
	void SendCommand(
		UINT8 const& cmd
	);
	/*******************************************************************
	 * EnablePin()
	 * Used to sets a pin high
	 * IN: @param pin represents the pin number of the GPIO
	 *******************************************************************/
	void EnablePin(
		UINT8 const& pin
	);
	/*******************************************************************
	 * DisablePin()
	 * Used to sets a pin low
	 * IN: @param pin represents the pin number of the GPIO
	*******************************************************************/
	void DisablePin(
		UINT8 const& pin
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
		UINT8 const& reg,
		UINT8* data, 
		uint8_t const& size
	);
	/*******************************************************************
	 * WriteReg()
	 * Used to Write a value to one register.
	 * IN: @param reg is the address of the register
	 * IN: @param data the value to set the register
	*******************************************************************/
	void WriteReg(
		UINT8 const& reg, 
		UINT8 const& data
	);
	/*******************************************************************
	 * ReadReg()
	 * Used to Read a value from one register.
	 * IN: @param reg is the address of the register
	 * @return: the value of the register
	*******************************************************************/
	UINT8 ReadReg(
		UINT8 const& reg
	);
	/*******************************************************************
	 * GetStatus()
	 * Used to get the status of the radio module
	 * @return: the current status of the device
	*******************************************************************/
	UINT8 GetStatus(
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
	 * Set2Tx()
	 * Used to set the radio from Rx to Tx 
	*******************************************************************/
	void Set2Tx(
		void
	);
	/*******************************************************************
	 * ReadMulti()
	 * Used to read multiple data from one register.
	 * 
	 * IN: @param reg is the address of the register
	 * OUT: @param data - is the values of the register
	 * IN: @param size - number of values to read from the register
	 * )
	*******************************************************************/
	void ReadMulti(
		UINT8 const& reg, 
		UINT8* data, 
		uint8_t const& size
	);
	UINT8 configReg{};
	UINT8 channel{};
	UINT8 status;
	uint8_t lenData;
	UINT8 spiTx[33]; //32 bytes date + 1 command
	UINT8 spiRx[33];	//32 bytes date + 1 command 
};