#include "CameraLib.h"
#include "RF24.h"
#include <cstring>

int main() 
{
  CameraLib camLib{};
  NRF24 nrf24{};
  camLib.StartCapture();
  char txAddress[] = {"2Node"};
  char rxAddress[] = {"1Node"};
  nrf24.RxMode(rxAddress, 76);
  nrf24.OpenWritingPipe(txAddress);
  uint8_t lenghtTransmission[32]{};
  uint8_t* dataToTransmit = new uint8_t[32]{};
  for(int i = 0; i < 32; i++)
  {
    lenghtTransmission[i] = 0;
  }
  lenghtTransmission[0] = 0x0A;
  lenghtTransmission[1] = 0x0D;
  uint8_t* imageBuf{nullptr};
  int length{0};
  while(1)
  {
    imageBuf = camLib.ReadFifoCam(length);
    if (imageBuf != nullptr && length > 0)
    {
      uint8_t* dataImg = imageBuf;
      lenghtTransmission[2] = (length >> 24) & 0xFF;
      lenghtTransmission[3] = (length >> 16) & 0xFF;
      lenghtTransmission[4] = (length >> 8) & 0xFF;
      lenghtTransmission[5] =  length & 0xFF;
      nrf24.TransmitData(lenghtTransmission);
      sleep_ms(20);

      for(int i = 0; i < length; i = i + 32)
      {
        // dataImg = dataImg + 32;
        // nrf24.TransmitData(dataTransmission);
        if(i + 32 < length)
        {
          memcpy(dataToTransmit, dataImg, sizeof(uint8_t) * 32);
          camLib.SerialUsb(dataToTransmit, 32);
          dataImg = dataImg + 32;
        }
        else
        {
          memcpy(dataToTransmit, dataImg, sizeof(uint8_t) * (length - i));
          camLib.SerialUsb(dataToTransmit, length - i);
          dataImg = dataImg + (length - i);
        }

        //  for(int i = 0; i < 32; i++)
        //  {
        //     printf("Data to Transmit: %x", dataToTransmit[i]);
        //  }
         if(dataToTransmit != nullptr)
         {
            nrf24.TransmitData(dataToTransmit);
         }

        //  printf("Index: %d", i);
        sleep_ms(20);
      }
      printf("%.4s\n",std::to_string(length).c_str());
      // camLib.SerialUsb(imageBuf, length);
      camLib.FreeFifoCam(imageBuf);
    }

  }
  delete[] dataToTransmit;
}

