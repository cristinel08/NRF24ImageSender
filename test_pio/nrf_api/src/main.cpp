#include<iostream>
#include "RF24.h"
#ifdef JETSON_BOARD
#include <JetsonGPIO.h>
#include<time.h>
#include<string>
#include<ctime>
#endif



int main()
{
	char dataTx[] = {"Buna Cristi!"};
	char txAddress[] = {"1Node"};
	char rxAddress[] = {"2Node"};
	char data[32] = {};

	NRF24 nrf24 = NRF24();
	
	nrf24.RxMode(rxAddress, 76);
	while(1)
	{
		if(nrf24.IsDataAvailable(1))
		{
			nrf24.ReceiveData(data);
			for(int i = 0; i < 32; i++)
			{
				std::cout << data[i] << " ";
			}
			std::cout << "\n";
		}
	}
}

