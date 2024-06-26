#include "CameraLib.h"

// set pin 10 as the slave select for the digital pot:

int main() 
{
  CameraLib camLib{};
  camLib.StartCapture();
  uint8_t* imageBuf{nullptr};
  int length{0};
  while(1)
  {
    imageBuf = camLib.ReadFifoCam(length);
    if (imageBuf != nullptr && length > 0)
    {
      camLib.SerialUsb(imageBuf, length);
      camLib.FreeFifoCam(imageBuf);
    }

  }
}
