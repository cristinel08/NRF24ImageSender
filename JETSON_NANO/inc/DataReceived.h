#include <opencv2/opencv.hpp>       
#include <opencv2/core/version.hpp>
#include <opencv2/imgcodecs.hpp>
#include <vector>
#include "NRF24.h"
#include <thread>
#include <memory>
#include <mutex>
#pragma once

class DataReceived
{

public:
    DataReceived();
    ~DataReceived();
    void StartReceiving();
    void CopyData();

private:
    void ColectData();
    void DecodeAndSaveImg();
    void CopyJpgImg(int16_t& jpgImgSize);

private:
	cv::Mat img_                               {  };
    char dataRx_[32]                           {  };
    uint32_t indexImg_                         { 0 };
    char txAddress_[6]                         { "1Node" };
    char rxAddress_[6]                         { "2Node" };
    std::vector<char> jpegImg_                 {  };
    int16_t jpegDataSize_                      { 0 };
    const uint8_t BYTES_RECEIVED               { 32 };
    char* populateJpeg_                        { nullptr };
    std::unique_ptr<NRF24> nrf24_              { nullptr };
    std::thread receiveThread;
    std::mutex copyMutex;
    uint8_t channel_                           { 76 };
    bool done_                                 { false };

};