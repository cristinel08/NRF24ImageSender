#include<RF24.h>
// #include<opencv2/core.hpp>

int main()
{
	// cv::Mat img{}
	char dataTx[32]{"Buna Cristi am trimis de pe Jet"};
	char dataRx[32]{};
	char txAddress[]{"1Node"};
	char rxAddress[]{"2Node"};
	char data[32]{"a,b,c,d,e,f,g,h,i,j,k,l,m,n,p,q"};
	bool anotherValue{false};
	uint8_t received{0};
	NRF24 nrf24 = NRF24();
	
	nrf24.RxMode(rxAddress, 76);
	// nrf24.TxMode(txAddress, 76);
	nrf24.OpenWritingPipe(txAddress);
	while(1)
	{

		while(nrf24.IsDataAvailable(1))
		{ 	
			// usleep(10);
			// usleep(500);
			nrf24.TransmitData(dataTx);	
			nrf24.ReceiveData(dataRx);
			if (dataRx[0] != anotherValue)
			{
				// usleep(900);
				// usleep(1000);
				anotherValue = !anotherValue;
				// printf("%32s\n",dataRx);
				printf("Received: %d\n", received++);
				if(received >= 128)
				{
					break;
				}
			}
		}
		// sleep(1);
	}
}

