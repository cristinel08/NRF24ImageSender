#include "CameraLib.h"
#include "RF24.h"
#include <cstring>
#include <chrono>

int main() 
{
  CameraLib camLib{};
  NRF24 nrf24{};
  camLib.StartCapture();
  UINT8 txAddress[] = {"2Node"};
  UINT8 rxAddress[] = {"1Node"};
  nrf24.RxMode(rxAddress, 76);
  nrf24.OpenWritingPipe(txAddress);
  uint8_t lenghtTransmission[32]{};
  uint8_t* dataToTransmit = new uint8_t[32]{};
  // uint8_t ackData[32] = {"..............................."};
  // bool isAvailable{false};
  for(int i = 0; i < 32; i++)
  {
    lenghtTransmission[i] = 0;
  }
  lenghtTransmission[0] = 0x0A;
  lenghtTransmission[1] = 0x0D;
  uint8_t* imageBuf{nullptr};
  // auto time = std::chrono::high_resolution_clock::now();
  // std::chrono::duration<float> duration{};
  // std::chrono::microseconds us;
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
      while(!nrf24.TransmitData(lenghtTransmission))
      {
        // sleep_us(500);
      }
      sleep_ms(5);

      for(int i = 0; i < length;)
      {
        // dataImg = dataImg + 32;
        // nrf24.TransmitData(dataTransmission);
        // if(dataToTransmit != NULL && dataToTransmit[0] == '\0')
        // {
          if(i + 32 < length)
          {
            memcpy(dataToTransmit, dataImg, sizeof(uint8_t) * 32);
            // camLib.SerialUsb(dataToTransmit, 32);
            // dataImg = dataImg + 32;
          }
          else
          {
            memcpy(dataToTransmit, dataImg, sizeof(uint8_t) * (length - i));
            // camLib.SerialUsb(dataToTransmit, length - i);
            // dataImg = dataImg + (length - i);
          }
        // }


        //  for(int i = 0; i < 32; i++)
        //  {
        //     printf("Data to Transmit: %x", dataToTransmit[i]);
        //  }
         if(dataToTransmit != nullptr)
         {
            if(nrf24.TransmitData(dataToTransmit))
            {
              if(i + 32 < length)
              {
                dataImg = dataImg + 32;
              }
              else
              {
                dataImg = dataImg + (length - i);
              }
              i = i + 32;
              // dataToTransmit[0] = '\0';
              // sleep_ms(10);
            }

         }

        //  printf("Index: %d", i);
    
      }
      // printf("%.4s\n",std::to_string(length).c_str());
      // camLib.SerialUsb(imageBuf, length);
      camLib.FreeFifoCam(imageBuf);
      sleep_ms(300);
      // sleep_ms(10);
    }

  }
  delete[] dataToTransmit;
}

