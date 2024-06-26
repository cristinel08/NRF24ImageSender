#include "DataReceived.h"

DataReceived::DataReceived(
    void
)
{
    copyJpegData_ = std::make_unique<uint8_t[]>(UINT16_MAX);
    jpegImg_.reserve(UINT16_MAX);
    for(uint16_t i = 0; i < UINT16_MAX; i++)
    {
        jpegImg_.emplace_back(0);
    }
    nrf24_ = std::make_unique<NRF24>();
    nrf24_->RxMode((uint8_t*)rxAddress_, channel_);
	nrf24_->OpenWritingPipe((uint8_t*)txAddress_);
}
DataReceived::~DataReceived(
    void
)
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

void DataReceived::StartReceiving(
    void
)
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
    uint8_t* jpegData, 
    bool& copied, 
    int& jpegSize
)
{
    std::unique_lock<std::mutex>lk(copyMutex);
    if(copyValue_)
    {
        memcpy(jpegData, copyJpegData_.get(), sizeof(uint8_t) * copyJpegSize_);
        jpegSize = copyJpegSize_;
        copyValue_ = false;
        copied = true;
    }
}

void DataReceived::ReceiveJpgData(
    int& jpgImgSize
)
{
    int8_t receiveSize{BYTES_RECEIVED};
    bool startFrame{false};
    while(jpgImgSize > 0)
    {
        if(nrf24_->IsDataAvailable(1))
        {	
            nrf24_->ResetRxIrq();
            while(nrf24_->ReceiveData(populateJpeg_, receiveSize, startFrame))
            {
                jpgImgSize = jpgImgSize - receiveSize;
                populateJpeg_ = populateJpeg_ + receiveSize * sizeof(uint8_t);
                if(jpgImgSize - receiveSize < 0)
                {
                    receiveSize = jpgImgSize;
                }
            }            
        }			
    }
        populateJpeg_ = jpegImg_.data();
        if(populateJpeg_ != nullptr)
        {
            std::unique_lock<std::mutex>lk(copyMutex);
            memcpy(copyJpegData_.get(), populateJpeg_, sizeof(uint8_t) * copyJpegSize_);
            copyValue_ = true;            
        }
}


void DataReceived::ColectData(
    void
)
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
                    (unsigned uint8_t)dataRx_[2] << 24 | 
                    (unsigned uint8_t)dataRx_[3] << 16 | 
                    (unsigned uint8_t)dataRx_[4] << 8  |
                    (unsigned uint8_t)dataRx_[5]
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
