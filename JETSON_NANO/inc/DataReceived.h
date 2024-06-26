#include <vector>
#include "NRF24.h"
#include <thread>
#include <memory>
#include <chrono>
#include <mutex>
#pragma once

#define CHRONO std::chrono
/*******************************************************************
 * DataReceived class
 * Starts a thread to receive the pixels
 *******************************************************************/
class DataReceived
{
public:
	/*******************************************************************
	 * DataReceived()
	 * Initilizes the data, sets the channel and 
     * the address on Rx and Tx
	 *******************************************************************/
    DataReceived(
        void
    );
	/*******************************************************************
	 * ~DataReceived()
	 * Terminates the thread and clears the memory
	 *******************************************************************/
    ~DataReceived(
        void
    );
    /*******************************************************************
	 * StartReceiving()
	 * Starts the thread to receive the data
	 *******************************************************************/
    void StartReceiving(
        void
    );
    /*******************************************************************
	 * CopyData()
	 * Used to copy the data from a shared memory between the 2 threads
     * OUT: @param jpegMainData is the jpeg image for the process thread
     * OUT: @param copied used to signal that it is a new image
     * OUT: @param jpegSize is the @param jpegMainData size
	 *******************************************************************/
    void CopyData(
        uint8_t* jpegMainData, 
        bool& copied,
        int& jpegSize
    );
    

private:
    /*******************************************************************
	 * ColetData()
	 * Is gonna be the method that the thread uses to run until the end
     * of program. It will wait for a starting byte and then save the 
     * jpeg data size.
	 *******************************************************************/
    void ColectData(
        void
    );
    /*******************************************************************
	 * ReceiveJpgData()
	 * Is the method used to receive the jpeg data while jpgImgSize>0
     * IN: @param jpgImgSize number of bytes that needs to receive
	 *******************************************************************/
    void ReceiveJpgData(
        int& jpgImgSize
    );

private:                
    uint8_t dataRx_[32]                     {  };
    char txAddress_[6]                      { "1Node" };
    char rxAddress_[6]                      { "2Node" };
    std::vector<uint8_t> jpegImg_           {  };
    int jpegDataSize_                       { 0 };
    const int8_t BYTES_RECEIVED             { 32 };
    int copyJpegSize_                       { 0 };
    uint8_t* populateJpeg_                  { nullptr };
    std::unique_ptr<NRF24> nrf24_           { nullptr };
    std::unique_ptr<uint8_t[]> copyJpegData_{ nullptr };
    uint8_t channel_                        { 76 };
    char cmd_                               { '\0' };
    bool done_                              { false };
    bool copyValue_                         { false };
    std::thread receiveThread;
    std::mutex copyMutex;
};