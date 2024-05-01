#include <jetgpio.h>
#include <iostream>
#include <string.h>
#include "nrf24l01.h"
#include <unistd.h>
#include <memory>
#pragma once

class NRF24
{
	public:
	   /***********************************************
		* NRF24()
		***********************************************/
	   /**
		* 
		* Default constructor, used to configure the 
		* peripherals and the nrf24l01 to a default 
		* configuration 
		*
		**********************************/
		NRF24();

	   /***********************************************
		* ~NRF24()
		***********************************************/
	   /**
		* 
		* Default destructor is used to close all 
		* the peripherals and shut down the nrf
		*
		***********************************************/
		~NRF24();

	   /***********************************************
		* TxMode()
		***********************************************/
	   /**
	    * 
		* Used to enter the nrf24 in Tx mode and set the
		* channel used to talk
		*
		* @param [IN] address 
		*  is the transmitted 
		*  address. It need to be an array with
		*  max 6 elements.
		* @param [IN] channel 
		*  is used to set the
		*  frequency used to transmit/receive the data,
		*  the frequency will be: 2400MHz + channel(MHz).
		*  The channel must be the same on the nrf Receiver
		*  (the other nrf)
		*
		***********************************************/
		void TxMode(char* address, const char& channel);

	   /***********************************************
		* RxMode()
		***********************************************/
	   /**
	    * 
		* Used to enter the nrf24 in Rx mode and set the
		* channel used to talk
		*
		* @param [IN] address 
		*  is the receiver 
		*  address. It needs to be an array with
		*  max 6 elements. And to be the same with the nrf 
		*  Transmiter.
		* @param [IN] channel 
		*  is used to set the frequency used to 
		*  transmit/receive the data,
		*  the frequency will be: 2400MHz + channel(MHz).
		*  The channel must be the same on the nrf Transmitter.
		*
		***********************************************/
		void RxMode(char* address, const char& data);

	   /***********************************************
		* TransmitData()
		***********************************************/
	   /**
	    * 
		* Used to transmit the data to the receiver
		* It also sets the nrf in Tx to be able to transmit
		*
		* @param [IN] data 
		*  is the data used to transmit. To be able to 
		*  transmit you will need to set
		*  the address of the nrf Transmitter to be the
		*  same with the address of the nrf Receiver
		*
		***********************************************/
		void TransmitData(char* data);

	   /***********************************************
		* ReceiveData()
		***********************************************/
	   /**
	    * 
		* Used to read the data on the nrf fifo. 
		*
		* @param [IN OUT] data 
		*  Is used to copy the data from the fifo
		* @param [IN] copySize 
		*  Is the copy data size
		* @return 
		*  true = FIFO empty
		*  false = FIFO not empty
		*
		***********************************************/
		bool ReceiveData(char* data, const uint8_t& copySize);

	   /***********************************************
		* IsDataAvailable()
		***********************************************/
	   /**
	    * 
		* Returns if there is data available in fifo
		*
		* @param [IN] pipeNr 
		*  is used to copy the data
		*  from the fifo
		* @param [IN] copySize 
		*  is the copy data size
		* @return
		*  true = Nrf receive data on pipe
		*  false = didn't receive data on that pipe
		* 
		***********************************************/
		bool IsDataAvailable(const uint8_t& pipeNr);
		
	   /***********************************************
		* OpenWritingAddress()
		***********************************************/
	   /**
	    * 
		* Sets the Tx address.
		*
		* @param [IN] address
		*  The transmiter address
		* 
		***********************************************/
		void OpenWritingAddress(char* address);

	private:
		void SendCommand(char cmd);
		void enablePin(const uint8_t& pin);
		void disablePin(const uint8_t& pin);
		void WriteRegMulti(const char& reg,char* data,int size);
		void WriteReg(const char& reg, const char& data);
		char ReadReg(const char& reg);
		void ReadMulti(const char& reg,char* data,int size);

		int init_		{};
		int SPI_init_	{};
		int verify_		{};
		char spiTx[33]	{}; //32 bytes date + 1 command
		char spiRx[33]	{};	//32 bytes date + 1 command 
		char config		{};
		char fifo		{};
		char en_rxaddr	{};
		char status		{};
		int  size		{0};
};
