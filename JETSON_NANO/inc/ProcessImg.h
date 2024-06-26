#include <opencv2/opencv.hpp>       
#include <opencv2/core/version.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include "jpgd.h"
#include <array>
#include <memory>
#include <vector>
#include <fstream>
#include "DataReceived.h"
/*******************************************************************
 * ProcessImg class
 * Waits for DataReceive to get the data to process it on the main
 * thread
 *******************************************************************/
class ProcessImg
{
public:
	/*******************************************************************
	 * ProcessImg()
	 * Initilizes the data
	 *******************************************************************/
    ProcessImg(
        void
    );
    /*******************************************************************
	 * ~ProcessImg()
	 * Default destructor
	 *******************************************************************/
    ~ProcessImg() = default;

    /*******************************************************************
	 * StartProcess()
	 * Method used to start the application
	 *******************************************************************/
    void StartProcess(
        void
    );
private:
    /*******************************************************************
	 * DecompressJpeg()
	 * Method used to decompress jpeg
     * IN: @param jpegImg
	 *******************************************************************/
    void DecompressJpeg(
        uint8_t* jpegImg
    );
    /*******************************************************************
	 * SaveJpegImg()
	 * Method used to save the jpeg image in a file on imgs directory
     * IN: @param jpegImg is the pointer to the jpeg data
	 *******************************************************************/
    void SaveJpegImg(
        uint8_t* jpegImg
    );
    /*******************************************************************
	 * ConvertRGB2BGR()
	 * Method used to convert an image color from RGB to BGR
     * INOUT: @param data is the pointer to pixels of the image
	 *******************************************************************/
    void ConvertRGB2BGR(
        uint8_t* data,
        uint   dataSize
    );
    /*******************************************************************
	 * CheckCommand()
	 * Method used to check if the user pressed Q to exit the program
	 *******************************************************************/
    void CheckCommand(
        void
    );
    /*******************************************************************
	 * DisplayAndCheckCmd()
	 * Method used to Display the image, get the command from the user
     * and check if he wants to exit
	 *******************************************************************/
    void DisplayAndCheckCmd(
        void
    );
    static constexpr int const CHANNELS         { 3 }; // request RGB image
    std::vector<uint8_t>jpegReceive_;
    FILE* imgsSaved_;
	cv::Mat img_;
    cv::Mat upScaleImg_;
    int jpegSize_;
    int width_; 
    int height_;
    int actualChannels_;
    std::unique_ptr<DataReceived> reveiveData_;
    uint8_t cmd_;
    bool shouldExit_;
    bool record_;
    uint8_t fps_;
};
