#include "DataReceived.h"
#include <vector>

int main()
{
	std::vector<char> jpegReceive;
	jpegReceive.reserve(UINT16_MAX);
	for(uint16_t i = 0; i < UINT16_MAX; i++)
	{
		jpegReceive.emplace_back(0);
	}

	// std::unique_ptr<char[]> jpegReceive = std::make_unique<char[]>(UINT16_MAX);
	bool copied { false };
	DataReceived receiveData{};
	receiveData.StartReceiving();
	while(1)
	{
		// receiveData.
		receiveData.CopyData(jpegReceive.data(), copied);
		if (copied)
		{
			if(receiveData.DecodeAndSaveImg(jpegReceive))
			{
				break;
			}
			copied = false;
		}
	}
	jpegReceive.clear();
	// std::cin >> a;
}

