#include<RF24.h>
#include<opencv4/opencv2/core.hpp>
#include<opencv4/opencv2/imgcodecs.hpp>
#include <opencv4/opencv2/highgui.hpp>
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
	int numReceived{0};
	int indexJpeg = 0;
	int testSize = 0;
	int indexImg = 0;
	while(1)
	{
		while(nrf24.IsDataAvailable(1))
		{
			nrf24.ReceiveData(dataRx);
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
				populateJpeg = jpegImg.data();
				while(size > 0)
				{
					while(nrf24.IsDataAvailable(1))
					{	
						size = size - 32;
						nrf24.ReceiveData(dataRx);
						if(size > 0)
						{
							memcpy(populateJpeg, dataRx, sizeof(char)*32);
						}
						else
						{
							memcpy(populateJpeg, dataRx, sizeof(char) * (size + 32));
						}
						
						populateJpeg = populateJpeg + 32;
						testSize++;
					}					
					// sleep(1);
				}
				cv::Mat img = cv::imdecode(jpegImg, cv::IMREAD_GRAYSCALE);
				if(!img.empty())
				{
					// cv::imshow("Image test", img);
					//cv::waitKey(1);
					// cv::imwrite();
					cv::imwrite("imgs/" + std::to_string(indexImg)
								 +".jpg", img);
					cv::waitKey(1);
					std::cout << "Wrote image: " << indexImg;
					indexImg++;
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
	}
}

