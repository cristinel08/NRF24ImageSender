#include "DataReceived.h"

DataReceived::DataReceived()
{
    copyJpegData_ = std::make_unique<char[]>(UINT16_MAX);
    jpegImg_.reserve(UINT16_MAX);
    for(uint16_t i = 0; i < UINT16_MAX; i++)
    {
        jpegImg_.emplace_back(0);
    }
    nrf24_ = std::make_unique<NRF24>();
    nrf24_->RxMode(rxAddress_, channel_);
	nrf24_->OpenWritingPipe(txAddress_);
}
DataReceived::~DataReceived()
{
    //terminate the second thread
    {
        std::unique_lock<std::mutex>lk(copyMutex);
        done_ = true;
    }

    receiveThread.join();
    jpegImg_.clear();
    populateJpeg_ = nullptr;
    
}

void DataReceived::StartReceiving()
{
    receiveThread = std::thread
    (
        [this]()
        {
            ColectData();
        }
    );
}

void DataReceived::CopyData(
    char* jpegData, 
    bool& copied, 
    int& jpegSize
)
{
    std::unique_lock<std::mutex>lk(copyMutex);
    if(copyValue_)
    {
        memcpy(jpegData, copyJpegData_.get(), sizeof(char) * copyJpegSize_);
        jpegSize = copyJpegSize_;
        copyValue_ = false;
        copied = true;
    }
}

void DataReceived::ReceiveJpgData(int& jpgImgSize)
{
    // nrf24_->StartTransferring();
    int8_t receiveSize{BYTES_RECEIVED};
    bool startFrame{false};
    while(jpgImgSize > 0)
    {
        if(nrf24_->IsDataAvailable(1))
        {	
            nrf24_->ResetRxIrq();
            while(nrf24_->ReceiveData(populateJpeg_, receiveSize, startFrame))
            // nrf24_->ReceiveData(populateJpeg_, receiveSize,startFrame);
            {
                // if(jpgImgSize==jpegDataSize_)
                // {
                    // if(!startFrame)
                    {
                        jpgImgSize = jpgImgSize - receiveSize;
                        populateJpeg_ = populateJpeg_ + receiveSize * sizeof(char);
                        if(jpgImgSize - receiveSize < 0)
                        {
                            receiveSize = jpgImgSize;
                        }
    
                    }
                    // else
                    // {
                    //     startFrame=false;
                    // }

                // }
                // else
                // {
                //        jpgImgSize = jpgImgSize - receiveSize;
                //        populateJpeg_ = populateJpeg_ + receiveSize * sizeof(char);
                //        if(jpgImgSize - BYTES_RECEIVED < 0)
                //         {
                //             receiveSize = jpgImgSize;
                //         }
                        
                        
                // }
            }
            
            
        }			
    }
        populateJpeg_ = jpegImg_.data();
        {
            std::unique_lock<std::mutex>lk(copyMutex);
            if(populateJpeg_ != nullptr)
            {
                memcpy(copyJpegData_.get(), populateJpeg_, sizeof(char) * copyJpegSize_);
                copyValue_ = true;
            }
            
        }
}


void DataReceived::ColectData()
{
    int tmpSize{};
    int8_t receiveSize{BYTES_RECEIVED};
    bool startFrame{false};
    while(true)
    {
        {
            std::unique_lock<std::mutex>lk(copyMutex);
            if(done_)
            {
                break;
            }
        }
        if(nrf24_->IsDataAvailable(1))
		{ 
            nrf24_->ReceiveData(dataRx_, BYTES_RECEIVED, startFrame);
            
            if(startFrame)
            {
                startFrame = false;
                if(jpegDataSize_)
                {
                    for (uint16_t i{0}; i < jpegDataSize_; i++)
                    {
                        jpegImg_[i] = 0;
                    }
                }
                jpegDataSize_ = static_cast<int>
                (
                    (unsigned char)dataRx_[2] << 24 | 
                    (unsigned char)dataRx_[3] << 16 | 
                    (unsigned char)dataRx_[4] << 8  |
                    (unsigned char)dataRx_[5]
                );
                if(jpegDataSize_ > 0)
                {
                    {
                        std::unique_lock<std::mutex>lk(copyMutex);
                        copyJpegSize_ = jpegDataSize_;
                    }
                    tmpSize = jpegDataSize_;
                    receiveSize = BYTES_RECEIVED;
                    populateJpeg_ = jpegImg_.data();                
                    ReceiveJpgData(tmpSize);
                }

            }
        }
    }
}
