#include<stdio.h>
#include<stdint.h>
#include<unistd.h>
#include<stdlib.h>
#include<jetgpio.h>
#include<opencv2/opencv.hpp>
#include<RF24/RF24.h>
std::string gstreamer_pipeline (int capture_width, int capture_height, int display_width, int display_height, int framerate, int flip_method) {
    return "nvarguscamerasrc ! video/x-raw(memory:NVMM), width=(int)" + std::to_string(capture_width) + ", height=(int)" +
           std::to_string(capture_height) + ", framerate=(fraction)" + std::to_string(framerate) +
           "/1 ! nvvidconv flip-method=" + std::to_string(flip_method) + " ! video/x-raw, width=(int)" + std::to_string(display_width) + ", height=(int)" +
           std::to_string(display_height) + ", format=(string)BGRx ! videoconvert ! video/x-raw, format=(string)BGR ! appsink";
}

int main()
{
	printf("Buna, Cristi, din main!\n");

	int init = 0;
	init = gpioInitialise();
	if(init < 0)
	{
		printf("Nu a mers initializarea pinilor\n");
		exit(init);
		return 0;
	}
	else
	{
		printf("A mers initializarea pinilor\n");
		//gpioTerminate();
//		return 0;
		//exit(0);
	}
	float payload = 0.0;
	int SPIInit = spiOpen(0, 5000000, 0, 0, 8, 1, 1);
	
	if(SPIInit < 0)
	{
		printf("Nu a mers initializarea spi");
		exit(init);
		return 0;
	}
	else
	{
		printf("A mers initializarea spi");
	}
//	RF24 radio(15, 0);
//	if(!radio.begin())
//	{
//		printf("Nu raspunde hardware");
//		return 0;
//	}
//	uint8_t address[2][6] = {"1Node", "2Node"};
//	radio.setPayloadSize(sizeof(payload));
//
//	radio.setPALevel(RF24_PA_LOW);

//	radio.openWritingPipe(address[0]);

//	radio.openReadingPipe(1, address[1]);

//	radio.printDetails();
/*	int capture_width = 3264 ;
        int capture_height = 2464 ;
        int display_width = 640 ;
        int display_height = 480 ;
        int framerate = 21 ;
        int flip_method = 0 ;

        std::string pipeline = gstreamer_pipeline(capture_width,
		capture_height,
		display_width,
		display_height,
		framerate,
		flip_method);
        std::cout << "Using pipeline: \n\t" << pipeline << "\n";

        cv::VideoCapture cap(pipeline, cv::CAP_GSTREAMER);
	if (!cap.isOpened())
	{
		printf("Error opening camera\n");
		return -1;
	}
	cv::Mat frame;
	while (true)
	{
		if(!cap.read(frame))
		{
			printf("Empty frame\n");
			break;
		}
		cv::imshow("RPi Camera", frame);
		if(cv::waitKey(1) == 'q')
		{
			break;
		}
	}
	cap.release();
	cv::destroyAllWindows();
*/
	spiClose(SPIInit);

	// Terminating library
	gpioTerminate();

	return 0;
}



