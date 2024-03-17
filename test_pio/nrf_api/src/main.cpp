#include "RF24.h"
#ifndef  JETSON_BOARD
#include <JetsonGPIO.h>
#include <time.h>
#include <string>
#include <ctime>
#endif



int main()
{
	char dataTx[32]{"Buna Cristi de pe raspberry pi!"};
	char txAddress[] = {"2Node"};
	char rxAddress[] = {"1Node"};
	char data[32] = {};
	bool once = true;
	bool received = true;
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

			// if(nrf24.IsDataAvailable(1))
			// {
			// 	// printf("Data Available\n");
			// 	while(nrf24.ReceiveData(data))
			// 	{
			// 		// printf("Data received while loop: %32s\n",data);
			// 		// received = false;
			// 	}
			// 	nrf24.SendCommand(FLUSH_RX);
			// 	// if(received)
			// 	// {
			// 	// 	printf("Data received outside while loop: %32s\n",data);
			// 	// }
			// 	// nrf24.TxMode(txAddress, 76);
			// 	// received = true;
			// }
			nrf24.TransmitData((uint8_t*)dataTx);
			printf("NumTrans %d\n", ++numTrans);
		}
		sleep_ms(1000);

	}
}

