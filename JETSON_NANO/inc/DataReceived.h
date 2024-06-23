#include <vector>
#include "NRF24.h"
#include <thread>
#include <memory>
#include <chrono>
#include <mutex>
#pragma once

#define CHRONO std::chrono

class DataReceived
{

public:
    DataReceived();
    ~DataReceived();
    void StartReceiving(
        void
    );
    void CopyData(
        char* jpegMainData, 
        bool& copied,
        int& jpegSize
    );
    

private:
    void ColectData(
        void
    );
    void ReceiveJpgData(
        int& jpgImgSize
    );

private:                
    char dataRx_[32]                        {  };
    char txAddress_[6]                      { "1Node" };
    char rxAddress_[6]                      { "2Node" };
    std::vector<char> jpegImg_              {  };
    int jpegDataSize_                       { 0 };
    const int8_t BYTES_RECEIVED             { 32 };
    int copyJpegSize_                       { 0 };
    char* populateJpeg_                     { nullptr };
    std::unique_ptr<NRF24> nrf24_           { nullptr };
    std::unique_ptr<char[]> copyJpegData_   { nullptr };
    uint8_t channel_                        { 76 };
    char cmd_                               { '\0' };
    bool done_                              { false };
    bool copyValue_                         { false };
    std::thread receiveThread;
    std::mutex copyMutex;
};