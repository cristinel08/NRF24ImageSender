#include "RF24.h"
#ifndef  PICO
#include <JetsonGPIO.h>
#include <time.h>
#include <string>
#include <ctime>
#endif



int main()
{
	char dataTx0[32]{"Buna Cristi de pe raspberry pi!"};
	char dataTx1[32]{"Cristi o iubeste pe Mari multot"};
	char txAddress[] = {"2Node"};
	char rxAddress[] = {"1Node"};
	char data[32] = {};
	bool once = true;
	bool transmitData0 = true;
	NRF24 nrf24 = NRF24();
	// nrf24.TxMode(rxAddress, 76);
	nrf24.RxMode(rxAddress, 76);
	nrf24.OpenWritingPipe(txAddress);
	int numTrans {0};

	while(1)
	{
		if(tud_cdc_connected())
		{
			if(once)
			{
				printf("Terminal connected\n");
				once = false;
			}
			if(transmitData0)
			{
				nrf24.TransmitData((uint8_t*)dataTx0);
			}
			else
			{
				nrf24.TransmitData((uint8_t*)dataTx1);
			}
			transmitData0 = !transmitData0;
			printf("NumTrans %d\n", ++numTrans);
		}
		sleep_ms(1000);

	}
}

