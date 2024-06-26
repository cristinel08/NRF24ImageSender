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
class ProcessImg
{
public:
    ProcessImg(
        void
    );
    ~ProcessImg() = default;
    void StartProcess(
        void
    );
private:
    static constexpr int const CHANNELS         { 3 }; // request RGB image
    void DecompressJpeg(
        uint8_t* jpegImg
    );
    void SaveJpegImg(
        uint8_t* jpegImg
    );
    void BiliniarInterpol(
        const cv::Mat& inputImage, 
        cv::Mat& outImg, 
        double scaleFactor
    );
    void ConvertRGB2BGR(
        uint8_t* data,
        uint   dataSize
    );
    void CheckCommand(
        void
    );
    void DisplayAndCheckCmd(
        void
    );
    std::vector<uint8_t>jpegReceive_;
    // std::ofstream imgsSaved_;
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