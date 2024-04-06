#include "RF24.h"
#ifndef  JETSON_BOARD
#include <JetsonGPIO.h>
#include <time.h>
#include <string>
#include <ctime>
#endif



int main()
{
	UINT8 dataTx[32]		{"Buna Cristi de pe raspberry pi!"};
	UINT8 ackTransmit[32]	{"WaitForAckTime................."};
	UINT8 txAddress[]		{"2Node"};
	UINT8 rxAddress[] 		{"1Node"};
	UINT8 data[32] 		 	{};
	bool once = true;
	bool received = true;
	NRF24 nrf24 = NRF24();
	// nrf24.TxMode(rxAddress, 76);
	nrf24.RxMode(rxAddress, 76);
	nrf24.OpenWritingPipe(txAddress);
	int numTrans {0};
	int retrans {3};
	bool isDataAvailable {0};

	while(1)
	{
		if(tud_cdc_connected())
		{
			if(once)
			{
				printf("Terminal connected\n");
				once = false;
			}


			// nrf24.Set2Tx();
			// nrf24.TransmitData(dataTx);
			// nrf24.Set2Rx();
			// isDataAvailable = nrf24.IsDataAvailable(1);
			// while(!isDataAvailable)
			// {
			// 	isDataAvailable = nrf24.IsDataAvailable(1);
			// }
			// nrf24.ReceiveData(data);
			// printf("NumTrans %d\n", ++numTrans);
			// while(nrf24.IsDataAvailable(1))
			// {
			// 	nrf24.ReceiveData(data);
			// }
			// isDataAvailable = false;	
			// nrf24.TransmitData(dataTx);
			while(nrf24.IsDataAvailable(1))
			{
				nrf24.ReceiveData(data);
				// sleep_ms(3);
				// sleep_ms(2);
				while(retrans)
				{
					nrf24.TransmitData(dataTx);
					// sleep_ms(1);
					retrans--;
				}
				printf((const char*)data);
				printf("\n");
				printf("Num transaction:%d \n", ++numTrans);

			}
			retrans = 2;
			// while(retrans > 0)
			// {
				
				// retrans--;
				// sleep_ms(1);
			// }
			// retrans = 3;
			// nrf24.SendCommand(FLUSH_RX);
		}
		else
		{
			numTrans = 0;
		}
		// sleep_ms(1000);

	}
}

