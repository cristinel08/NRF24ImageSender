#include "CameraLib.h"
#include "NRF24.h"
#include <cstring>
#include <chrono>

int main() 
{
  ReceiveFrameCamera camLib{};
  NRF24 nrf24{};
  camLib.StartCapture();
  UINT8 txAddress[] = {"2Node"};
  UINT8 rxAddress[] = {"1Node"};
  nrf24.RxMode(rxAddress, 76);
  nrf24.OpenWritingPipe(txAddress);
  uint8_t lenghtTransmission[32]{};
  uint8_t* dataToTransmit = new uint8_t[32]{};
  lenghtTransmission[0] = 0x0A;
  lenghtTransmission[1] = 0x0D;
  uint8_t* imageBuf{nullptr};
  uint8_t transmitSize{32};
  uint32_t length{0};
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
      while(!nrf24.TransmitData(lenghtTransmission, 32))
      {
        sleep_ms(1);
      }

      for(uint32_t i = 0; i < length;)
      {
          if(i + 32 < length)
          {
            transmitSize = 32;
          }
          else
          {
            transmitSize = length - i;
          }
          if(nrf24.TransmitData(imageBuf, transmitSize))
          {
            imageBuf = imageBuf + transmitSize;
            i = i + 32;
          } 
      }
      // camLib.SerialUsb(imageBuf, length);
      camLib.FreeFifoCam(dataImg);
    }

  }
  delete[] dataToTransmit;
}

