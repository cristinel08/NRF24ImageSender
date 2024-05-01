#include "RF24.h"
#include <chrono>
#include <thread>
#ifndef  PICO
#include <JetsonGPIO.h>
#include <time.h>
#include <string>
#include <ctime>
#endif



int main()
{
	UINT8 dataTx[32]		{"Buna Cristi de pe raspberry pi!"};
	UINT8 dataACK[32]		{"ACK____________________________"};
	UINT8 txAddress[6]		{"2Node"};
	UINT8 rxAddress[6] 		{"1Node"};
	UINT8 data[32] 		 	{};
	uint8_t retransmit		{2};
	bool once 				{true};
	uint8_t numTrasmission 	{0};
	NRF24 nrf24{};
	auto timeProgram = std::chrono::high_resolution_clock::now();
	double takenTime {0.0};
	nrf24.RxMode(rxAddress, 76);
	nrf24.OpenWritingPipe(txAddress);
	while(1)
	{
		if(once)
		{
			printf("Terminal connected\n");
			once = false;
		}
		while(data[0] == '\0' && (data != NULL))
		{
			while(nrf24.IsDataAvailable(1) == 0 && retransmit)
			{
				nrf24.Set2Tx();
				nrf24.TransmitData(dataTx);
				nrf24.Set2Rx();
				retransmit--;
				sleep_us(2500);
			}
			nrf24.ReceiveData(data);
			retransmit = 2;
		}
		numTrasmission++;
		// printf("%32s\n", data);
		data[0] = '\0';
		if(numTrasmission >= 128)
		{
			break;
		}
	}
	takenTime = std::chrono::duration_cast<std::chrono::milliseconds>
			(
				std::chrono::high_resolution_clock::now() - timeProgram
			).count();
	printf
	(
		"The program took %f ms to transmit %d\n", 
		takenTime,
		numTrasmission * 32
	);
}

