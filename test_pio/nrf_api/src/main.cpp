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
			// nrf24.TransmitData((UINT8*)dataTx);


			// if(numTrans % 4 == 0)
			// {
				// while((!isDataAvailable))
				// {
					// sleep_ms(1);
					nrf24.Set2Tx();
					nrf24.TransmitData(dataTx);
					nrf24.Set2Rx();
					sleep_ms(1);
					isDataAvailable = nrf24.IsDataAvailable(1);
					while(!isDataAvailable)
					{
						nrf24.Set2Tx();
						nrf24.TransmitData(ackTransmit);
						nrf24.Set2Rx();
						sleep_us(1500);
						isDataAvailable = nrf24.IsDataAvailable(1);

					}
					// sleep_ms(1);
					// retrans -= 1;
					// sleep_ms(1);
					// isDataAvailable = nrf24.IsDataAvailable(1);
					// sleep_ms(5);
				// }
				// else
				// {
				// while(nrf24.IsDataAvailable(1))
				// if(isDataAvailable)
				// {
				// nrf24.ReceiveData(data);
				while(!isDataAvailable)
				{
					nrf24.Set2Tx();
					nrf24.TransmitData((UINT8*)dataTx);
					nrf24.Set2Rx();
					isDataAvailable = nrf24.IsDataAvailable(1);
				}
				nrf24.ReceiveData(data);
				printf("NumTrans %d\n", ++numTrans);
				while(nrf24.IsDataAvailable(1))
				{
					nrf24.ReceiveData(data);
				}
				isDataAvailable = false;

				// }
				// while(nrf24.IsDataAvailable(1))
				// {
			// }

			// }
	
		}
		// sleep_ms(1000);

	}
}

