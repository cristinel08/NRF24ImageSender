#include "RF24.h"
#ifndef  JETSON_BOARD
#include <JetsonGPIO.h>
#include <time.h>
#include <string>
#include <ctime>
#endif



int main()
{
	char dataTx[] = {"Buna Cristi!"};
	char txAddress[] = {"2Node"};
	char rxAddress[] = {"1Node"};
	char data[32] = {};
	bool once = true;

	NRF24 nrf24 = NRF24();
	
	nrf24.RxMode(rxAddress, 76);
	while(1)
	{
		if(tud_cdc_connected())
		{
			if(once)
			{
				printf("Terminal connected\n");
				once = false;
			}
		
			if(nrf24.IsDataAvailable(1))
			{
				// printf("Data Available\n");
				while(nrf24.ReceiveData(data))
				{
					printf("Data received: %32s\n",data);
				}
				printf("Data received: %32s\n",data);
				// printf(data);
				// for(int i = 0; i < 32; i++)
				// {
					// printf(data[i]);
				// }
			}
		}
		sleep_ms(1000);

	}
}

