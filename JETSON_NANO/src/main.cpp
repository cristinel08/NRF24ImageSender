#include "ProcessImg.h"
#include <memory>
int main()
{
	std::unique_ptr<ProcessImg> processImg{nullptr};
	processImg = std::make_unique<ProcessImg>();
	processImg->StartProcess();
}

