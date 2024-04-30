#include "RF24.h"
#ifndef  JETSON_BOARD
#include <JetsonGPIO.h>
#include <time.h>
#include <string>
#include <ctime>
#endif



int main()
{
	UINT8 dataTx[32]		{"0Buna Cristi de pe raspberry pi"};
	UINT8 dataACK[32]		{"ACK____________________________"};
	UINT8 txAddress[6]		{"2Node"};
	UINT8 rxAddress[6] 		{"1Node"};
	UINT8 data[32] 		 	{};
	bool once 				{true};
	bool received			{false};
	uint8_t numTrasmission 	{0};
	NRF24 nrf24{};
	// nrf24.TxMode(rxAddress, 76);
	nrf24.RxMode(rxAddress, 76);
	nrf24.OpenWritingPipe(txAddress);
	while(1)
	{
		if(once)
		{
			printf("Terminal connected\n");
			once = false;
		}
		received  = !received;
		dataTx[0] = received;
		nrf24.TransmitData(dataTx);
		sleep_us(500);
		nrf24.Set2Rx();
		sleep_us(2000);
		printf("Transmited %d\n", numTrasmission++);
		if(numTrasmission >= 128)
		{
			break;
		}
		while(nrf24.IsDataAvailable(1) == 0)
		{
			nrf24.Set2Tx();
			nrf24.TransmitData(dataTx);
			sleep_us(500);
			nrf24.Set2Rx();
			sleep_us(2000);
		}
		nrf24.ReceiveData(data);
		printf("%32s\n", data);
		
	}
}

