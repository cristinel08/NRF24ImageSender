#include <RF24.h>
#include <chrono>
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
	auto timeToReceiveAndTransmit = std::chrono::high_resolution_clock::now();
	double timeTook {0.0};
	uint8_t received{0};
	NRF24 nrf24 = NRF24();
	
	nrf24.RxMode(rxAddress, 76);
	// nrf24.TxMode(txAddress, 76);
	nrf24.OpenWritingPipe(txAddress);
	while(1)
	{

		while(nrf24.IsDataAvailable(1))
		{ 	
			timeToReceiveAndTransmit = std::chrono::high_resolution_clock::now();
			// usleep(10);
			// usleep(500);
			// usleep(300);
			//varianta ce a mers mai rpd
			// usleep(900);
			// nrf24.Set2Rx();
			// nrf24.Set2Tx();
			// usleep(10);
			nrf24.TransmitData(dataTx);	
			// timeTook = 
			// 	std::chrono::duration_cast<std::chrono::nanoseconds>
			// 	(
			// 		std::chrono::high_resolution_clock::now() - timeToReceiveAndTransmit
			// 	).count();
			nrf24.ReceiveData(dataRx);
			// if (dataRx[0] != anotherValue)
			// {
			// 	// usleep(900);
			// 	// usleep(1000);
			// 	anotherValue = dataRx[0];
			// 	// printf("%32s\n",dataRx);
			// }
			received++;
			if(received >= 128)
			{
				printf("Received: %d\n", received++);
				// exit(0);
			}
		}
		// sleep(1);
	}
}

