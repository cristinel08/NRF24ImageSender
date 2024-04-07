#include<RF24.h>
#include <opencv2/opencv.hpp>            // C++
#include <opencv2/core/version.hpp>
#include <opencv2/imgcodecs.hpp>
#include<iostream>
#include<time.h>
#include<string>
#include <cstring>
#include<ctime>

int main()
{
	char dataTx[32]{"Buna Cristi am trimis de pe Jet"};
	char dataRx[32]{};
	char txAddress[]{"1Node"};
	char rxAddress[]{"2Node"};
	char data[32]{"a,b,c,d,e,f,g,h,i,j,k,l,m,n,p,q"};
	std::vector<char> jpegImg{};
	NRF24 nrf24 = NRF24();
	char* populateJpeg{nullptr};
	nrf24.RxMode(rxAddress, 76);
	bool dataTransmited{true};
	nrf24.OpenWritingPipe(txAddress);
	// nrf24.TxMode(txAddress, 76);
	// nrf24.OpenWritingPipe(txAddress);
	int size = 0;
	// int numReceived{0};
	int indexJpeg = 0;
	uint8_t copySize {32};
	cv::Mat img{};
	// int indexImg = 0;
	while(1)
	{
		
		if(nrf24.IsDataAvailable(1))
		{
			nrf24.ReceiveData(dataRx, copySize);
			if(dataRx[0] == 0x0A && dataRx[1] == 0x0D)
			{
				size = (int)((unsigned char)dataRx[2] << 24 | 
					        (unsigned char)dataRx[3] << 16 | 
					        (unsigned char)dataRx[4] << 8  |
					        (unsigned char)dataRx[5]);
				if(!jpegImg.empty())
				{
					populateJpeg = nullptr;
					jpegImg.clear();
				}
				jpegImg = std::vector<char>(size);
				copySize = 32;
				populateJpeg = jpegImg.data();
				while(size > 0)
				{
					if(nrf24.IsDataAvailable(1))
					{	
						size = size - 32;
						
						if(size > 0)
						{
							// memcpy(populateJpeg, dataRx, sizeof(char)*32);
							nrf24.ReceiveData(populateJpeg, copySize);
							populateJpeg = populateJpeg + 32;
						}
						else
						{
							nrf24.ReceiveData(populateJpeg, size + 32);
							populateJpeg = populateJpeg + (size + 32);
							// memcpy(populateJpeg, dataRx, sizeof(char) * (size + 32));
						}
						
						// populateJpeg = populateJpeg + 32;
					}					
					// sleep(1);
				}
				img = cv::imdecode(jpegImg, cv::IMREAD_COLOR);
				if(!img.empty())
				{
					cv::imshow("Image test", img);
					//cv::waitKey(1);
					// cv::imwrite();
					// cv::imwrite("imgs/" + std::to_string(indexImg)
					// 			 +".jpg", img);
					cv::waitKey(1);
					// std::cout << "Wrote image: " << indexImg;
					// indexImg++;
				}
	
				jpegImg.clear();
				populateJpeg = nullptr;
			}
			// if(!jpegImg.empty() && size == 0)
			// {
			// 	cv::Mat img = cv::imdecode(jpegImg, cv::IMREAD_GRAYSCALE);
			// 	cv::imshow("Image test", img);
			// 	cv::waitKey(1);
			// 	jpegImg.clear();
			// }

		}
		// dataTransmited = !dataTransmited;	
		// usleep(1);
	}
}

