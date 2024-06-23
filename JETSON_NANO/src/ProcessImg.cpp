#include "ProcessImg.h"

ProcessImg::ProcessImg(

) : 
    img_                { },
    upScaleImg_         { },
    cmd_                { '\0' },
    shouldExit_         { false },
    reveiveData_        { nullptr },
    jpegSize_           { 0 },
    jpegReceive_        { },
    actualChannels_     { },
    width_              { },
    height_             { },
    record_             { false },
    fps_                { }
{
    reveiveData_ = std::make_unique<DataReceived>();
    jpegReceive_.reserve(UINT16_MAX);
    for(uint16_t i = 0; i < UINT16_MAX; i++)
    {
        jpegReceive_.emplace_back(0);
    }
}
void ProcessImg::StartProcess()
{
    bool copied { false };
	reveiveData_->StartReceiving();
	CHRONO::duration<float, std::milli> duration   {  };
	auto startTime = CHRONO::high_resolution_clock::now(); 
	uint8_t countFrames{0};
    cv::namedWindow("ImageReceived", cv::WindowFlags::WINDOW_AUTOSIZE);
	while(1)
	{
        if(shouldExit_)
        {
            if(imgsSaved_ != NULL)
            {
                fclose(imgsSaved_);   
            }  
            break;
        }
		reveiveData_->CopyData(jpegReceive_.data(), copied, jpegSize_);
		if (copied)
		{
			duration = CHRONO::high_resolution_clock::now() - startTime;
			if(duration.count() > 1000)
			{
				fps_ = countFrames;
				startTime = CHRONO::high_resolution_clock::now();
				countFrames = 0;
			}
            DecompressJpeg(jpegReceive_.data());
			copied = false;
			countFrames++;
		}
        DisplayAndCheckCmd();
	}
}

void ProcessImg::DisplayAndCheckCmd()
{
    if(!img_.empty())
    {
        //BiliniarInterpol(img_, upScaleImg_, 1.5f);
        cv::putText(img_,std::to_string(fps_), cv::Point(10, 15), cv::FONT_HERSHEY_DUPLEX, 0.5, cv::Scalar(255, 255, 255));
        cv::imshow("ImageReceived", img_);
        cmd_ = cv::waitKey(1);
        CheckCommand();
    }
}

void ProcessImg::BiliniarInterpol(
    const cv::Mat& inputImage, 
    cv::Mat& outImg, 
    double scaleFactor
) 
{
    // Get dimensions of the input image
    int originalHeight = inputImage.rows;
    int originalWidth = inputImage.cols;
    int numChannels = inputImage.channels();

    // Calculate dimensions of the new image
    int newHeight = static_cast<int>(originalHeight * scaleFactor);
    int newWidth = static_cast<int>(originalWidth * scaleFactor);

    // Initialize the new image
    if(outImg.empty())
    {
        outImg = cv::Mat(newHeight, newWidth, inputImage.type());
    }
    for (int i = 0; i < newHeight; ++i) {
        for (int j = 0; j < newWidth; ++j) {
            // Map the coordinates of the new image to the original image
            float x = i / scaleFactor;
            float y = j / scaleFactor;

            // Get the coordinates of the four surrounding pixels
            int x1 = static_cast<int>(x);
            int x2 = std::min(x1 + 1, originalHeight - 1);
            int y1 = static_cast<int>(y);
            int y2 = std::min(y1 + 1, originalWidth - 1);

            // Calculate the distances between the points
            float a = x - x1;
            float b = y - y1;

            // Perform bilinear interpolation for each channel
            for (int c = 0; c < numChannels; ++c) {
                float value = (1 - a) * (1 - b) * inputImage.at<cv::Vec3b>(x1, y1)[c] +
                              a * (1 - b) * inputImage.at<cv::Vec3b>(x2, y1)[c] +
                              (1 - a) * b * inputImage.at<cv::Vec3b>(x1, y2)[c] +
                              a * b * inputImage.at<cv::Vec3b>(x2, y2)[c];

                outImg.at<cv::Vec3b>(i, j)[c] = static_cast<uchar>(value);
            }
        }
    }
}

void ProcessImg::DecompressJpeg(
    char* jpegImg
)
{
    uint8_t* data = jpgd::decompress_jpeg_image_from_memory(
        (unsigned char*)jpegImg,
        jpegSize_,
        &width_, 
        &height_, 
        &actualChannels_, 
        CHANNELS
    );        
    if(data != nullptr)
    {
        if(imgsSaved_ != NULL)
        {
            // imgsSaved_.write(jpegImg,jpegSize_*sizeof(char));   
            fwrite(jpegImg, jpegSize_ * sizeof(char), 1, imgsSaved_);
            fclose(imgsSaved_);
            imgsSaved_=nullptr;
        }  
        else
        {
            // imgsSaved_.open("imgs/image.jpg"); 
            imgsSaved_ = fopen("imgs/test.jpg", "wb"); 

        }  
        if (img_.cols != width_ &&
            img_.rows != height_ &&
            img_.channels() != actualChannels_)
        {
            uint8_t type;
            switch (actualChannels_)
            {
            case 1:
                type = CV_8U;
                break;
            case 2:
                type = CV_8UC2;
                break;
            case 3:
                type = CV_8UC3;
                break;
            default:
                break;
            }
            img_ = cv::Mat(height_, width_, type);
        }
        ConvertRGB2BGR(data, static_cast<uint>(height_ * width_ * actualChannels_));
        memcpy(img_.data, data, actualChannels_ * width_ * height_ * sizeof(uchar));
        free(data);
    }

}
void ProcessImg::ConvertRGB2BGR(
    uchar* data,
    uint sizeData
)
{
    for(uint i{0}; i < sizeData; i=i+3)
    {
        std::swap(data[i], data[i + 2]);
    }
}
void ProcessImg::CheckCommand()
{
    switch(cmd_)
    {
        case 'q':
        case 'Q':
            shouldExit_ = true;
            break;
        case 'r':
        case 'R':
            record_ = !record_;
        default:
            break;
    }
    cmd_ = '\0';
}