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
    //set a bool to false then
    {
        std::unique_lock<std::mutex>lk(copyMutex);
        done_ = true;
    }

    receiveThread.join();
    jpegImg_.clear();
    img_.release();
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

void DataReceived::CopyData(char* jpegData, bool& copied)
{
    std::unique_lock<std::mutex>lk(copyMutex);
    if(copyValue_)
    {
        memcpy(jpegData, copyJpegData_.get(), sizeof(char) * copyJpegSize_);
        copyValue_ = false;
        copied = true;
    }
}

void DataReceived::CopyJpgImg(int16_t& jpgImgSize)
{
    {
        std::unique_lock<std::mutex>lk(copyMutex);
        copyJpegSize_ = jpgImgSize;
    }
    while(jpgImgSize > 0)
    {
        if(nrf24_->IsDataAvailable(1))
        {	
            jpegDataSize_ = jpegDataSize_ - BYTES_RECEIVED;
            
            if(jpegDataSize_ > 0)
            {
                nrf24_->ReceiveData(populateJpeg_, BYTES_RECEIVED);
                populateJpeg_ = populateJpeg_ + BYTES_RECEIVED;
            }
            else
            {
                nrf24_->ReceiveData(populateJpeg_, jpegDataSize_ + BYTES_RECEIVED);
                populateJpeg_ = populateJpeg_ + jpegDataSize_ + BYTES_RECEIVED;
            }
        }					
    }
    populateJpeg_ = jpegImg_.data();
    {
        std::unique_lock<std::mutex>lk(copyMutex);
        memcpy(copyJpegData_.get(), populateJpeg_, sizeof(char) * copyJpegSize_);
        copyValue_ = true;
    }
}

bool DataReceived::DecodeAndSaveImg(std::vector<char>& jpegImg)
{
    try
    {
        img_ = cv::imdecode(jpegImg_, cv::IMREAD_COLOR);

    }
    catch(const cv::Exception& e)
    {
        std::cerr << e.what() << '\n';
        // jpegImg.clear();
    }

    if(!img_.empty())
    {
        cv::imshow("Image test", img_);
        cmd_ = cv::waitKey(1);
        if(cmd_ != '\0')
        {
            return CheckCommand();
        }
        // cv::imwrite("imgs/" + std::to_string(indexImg_)
        //                 +".jpg", img_);
        // indexImg_++;
    }

    // jpegImg.clear();

    return 0;
}

bool DataReceived::CheckCommand()
{
    bool shouldExit{false};
    switch(cmd_)
    {
        case 'q':
        case 'Q':
            shouldExit = true;
            break;
        default:
            break;
    }
    cmd_ = '\0';
    return shouldExit;   
}

void DataReceived::ColectData()
{
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
            nrf24_->ReceiveData(dataRx_, BYTES_RECEIVED);
            if(dataRx_[0] == 0x0A && dataRx_[1] == 0x0D)
            {
                jpegDataSize_ = static_cast<int>
                    (
                        (unsigned char)dataRx_[2] << 24 | 
                        (unsigned char)dataRx_[3] << 16 | 
                        (unsigned char)dataRx_[4] << 8  |
                        (unsigned char)dataRx_[5]
                    );
				// jpegImg_ = std::vector<char>(jpegDataSize_);
				populateJpeg_ = jpegImg_.data();
                CopyJpgImg(jpegDataSize_);
                DecodeAndSaveImg(jpegImg_);
            }

        }

    }
}
