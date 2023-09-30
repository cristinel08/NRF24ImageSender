#include<jetgpio.h>
#include<iostream>
#include"RF24.h"

#define CE_PIN 18
#define CSN_PIN 21

void nrf24_WriteReg (uint8_t reg, uint8_t data);
void nrf24_Init(int SPI_init, int init);
void nrf24_WriteReg (char reg, char data, int SPI_init, int init);
void nrf24_WriteRegMulti(char reg, char* buf, int size, int SPI_init, int init);
char nrf24_ReadReg (char reg, int SPI_init, int init);
void nrf24_ReadMulti (char reg, char* data, int size,int SPI_init, int init);
void enablePin(int pin);
void disablePin(int pin);
//void enable_CE();
//void disable_CE();

int main()
{
	int init = gpioInitialise();
	if(init < 0)
	{
		std::cout << "Problema initializare gpio cu " << init << "\n";
		exit(init);
	}
	else
	{
		std::cout << "Jetgpio initialisation OK. Return: " << init << "\n";
	}
	int start1 = gpioSetMode(CE_PIN, JET_OUTPUT);
	if(start1 < 0)
	{
		std::cout << "Nu a mers setarea pinului CE, codul: " << start1 << "\n";
		exit(init);
	}
	else
	{
		std::cout << "A mers setarea pinului CE. Codul: " << start1 << "\n";
	}

	start1 = gpioSetMode(CSN_PIN, JET_OUTPUT);

        if(start1 < 0)
        {
                std::cout << "Nu a mers setarea pinului CSN, codul: " << start1 << "\n";
                exit(init);
        }
        else
        {
                std::cout << "A mers setarea pinului CSN. Codul: " << start1 << "\n";
        }

	int SPI_init = spiOpen(0, 250000, 0, 0, 8, 1, 1);

	if (SPI_init < 0)
	{
   		/* Port SPI2 opening failed */
  		printf("Port SPI1 opening failed. Error code:  %d\n", SPI_init);
   		exit(init);
	}
	else
	{
   		/* Port SPI2 opened  okay*/
		printf("Port SPI1 opened OK. Return code:  %d\n", SPI_init);
	}
}
//write a single byte
void nrf24_WriteReg (char reg, char data, int SPI_init, int init)
{
	char buf[2];
	buf[0] = reg|1<<5;
	buf[1] = data;
	//pentru a selecta
	disablePin(CSN_PIN);
	int SPI_stat = spiXfer(SPI_init, buf, nullptr, 2);
        if(SPI_stat < 0)
        {
                std::cout << "SPI port transfer failed. Error code: " << SPI_stat << "\n";
                exit(init);
        }
        else
        {
                std::cout << "Spi port transfer OK. Return code: " << SPI_stat << "\n";
        }
	enablePin(CSN_PIN);
}
//write multiple datas
void nrf24_WriteRegMulti(char reg, char* data, int size, int SPI_init, int init)
{
	char buf[2];
	buf[0] = reg|1<<5;

        disablePin(CSN_PIN);
        int SPI_stat = spiXfer(SPI_init, buf,nullptr, 1);
        if(SPI_stat < 0)
        {
                std::cout << "SPI port transfer failed. Error code: " << SPI_stat << "\n";
                exit(init);
        }
        else
        {
                std::cout << "Spi port transfer OK";
        }

        SPI_stat = spiXfer(SPI_init, data, nullptr, size);
        if(SPI_stat < 0)
        {
                std::cout << "SPI port transfer fail\n";
                exit(init);
        }
        else
        {
                std::cout << "Spi port transfer OK\n";
        }
        enablePin(CSN_PIN);
}
//read 1 byte
char nrf24_ReadReg (char reg, int SPI_init, int init)
{
	char data='0';
	
	//pentru a selecta dispozitivul
	disablePin(CSN_PIN);
	int SPI_stat = spiXfer(SPI_init, &reg, &data, 1);
        if(SPI_stat < 0)
        {
                std::cout << "SPI port read 1byte transfer failed. Error code: " << SPI_stat << "\n";
                exit(init);
        }
        else
        {
                std::cout << "Spi port read 1byte transfer OK. Return code: " << SPI_stat << "\n";
        }
	enablePin(CSN_PIN);
}
void nrf24_ReadMulti (char reg, char* data, int size,int SPI_init, int init)
{	
	//pentru a selecta dispozitivul
	disablePin(CSN_PIN);
	int SPI_stat = spiXfer(SPI_init, &reg, data, size);
        if(SPI_stat < 0)
        {
                std::cout << "SPI port read 1byte transfer failed. Error code: " << SPI_stat << "\n";
                exit(init);
        }
        else
        {
                std::cout << "Spi port read 1byte transfer OK. Return code: " << SPI_stat << "\n";
        }
	enablePin(CSN_PIN);
}

void nrfSendCommand(char cmd, int SPI_init, int init)
{
	disablePin(CSN_PIN);
	int SPI_stat = spiXfer(SPI_init, &cmd, nullptr, 1);
	if(SPI_stat < 0)
        {
                std::cout << "SPI port transfer failed. Error code: " << SPI_stat << "\n";
                exit(init);
        }
        else
        {
                std::cout << "Spi port transfer OK. Return code: " << SPI_stat << "\n";
        }
	enablePin(CSN_PIN);
}

void nrf24_Init(int SPI_init, int init)
{
	disablePin(CE_PIN);
	enablePin(CSN_PIN);

	nrf24_WriteReg(CONFIG, 0,SPI_init,init); //config later

	nrf24_WriteReg(EN_AA, 0, SPI_init, init); // No Auto Ack

	nrf24_WriteReg(EN_RXADDR, 0, SPI_init, init); //0 data pipes enabled

	nrf24_WriteReg(SETUP_AW, 0x03,SPI_init, init);	//5 bytes for the tx/rx address

	nrf24_WriteReg(SETUP_RETR, 0, SPI_init, init); //No Retransmision

	nrf24_WriteReg(RF_CH, 0, SPI_init, init);     //Channel will be setup in  during TX or RX

	nrf24_WriteReg(RF_SETUP, 0x0E, SPI_init, init);	//Power 0dB, data rate = 2Mbps

	enablePin(CE_PIN);
	disablePin(CSN_PIN);
}

void enablePin(int pin)
{
	gpioWrite(pin, 1);
}

void disablePin(int pin)
{
	gpioWrite(pin,0);
}

void NRF24_TxMode(char* Address, char channel)
{
	disablePin(CE_PIN);
	enablePin(CSN_PIN);
	
	nrf24_Writing (RF_CH, channel);

}	



