#include"RF24.h"

int main()
{
	NRF24();
}

//write a single byte
void NRF24::nrf24_WriteReg (char reg, char data)
{
	char buf[2];
	buf[0] = reg|1<<5;
	buf[1] = data;
	//pentru a selecta
	disablePin(CSN_PIN);
	verify_ = spiXfer(SPI_init_, buf, nullptr, 2);
        if(verify_ < 0)
        {
                std::cout << "SPI port transfer failed. Error code: " << verify_ << "\n";
                exit(init_);
        }
        else
        {
                std::cout << "Spi port transfer OK. Return code: " << verify_ << "\n";
        }
	enablePin(CSN_PIN);
}
//write multiple datas
void NRF24::nrf24_WriteRegMulti(char reg, char* data, int size)
{
	char buf[2];
	buf[0] = reg|1<<5;

        disablePin(CSN_PIN);
        verify_ = spiXfer(SPI_init_, buf,nullptr, 1);
        if(verify_ < 0)
        {
                std::cout << "SPI port transfer failed. Error code: " << verify_ << "\n";
                exit(init_);
        }
        else
        {
                std::cout << "Spi port transfer OK";
        }

        verify_ = spiXfer(SPI_init_, data, nullptr, size);
        if(verify_ < 0)
        {
                std::cout << "SPI port transfer fail\n";
                exit(init_);
        }
        else
        {
                std::cout << "Spi port transfer OK\n";
        }
        enablePin(CSN_PIN);
}
//read 1 byte
char NRF24::nrf24_ReadReg (char reg)
{
	char data='0';
	
	//pentru a selecta dispozitivul
	disablePin(CSN_PIN);
	verify_ = spiXfer(SPI_init_, &reg, &data, 1);
        if(verify_ < 0)
        {
                std::cout << "SPI port read 1byte transfer failed. Error code: " << verify_ << "\n";
                exit(init_);
		return data;
        }
        else
        {
                std::cout << "Spi port read 1byte transfer OK. Return code: " << verify_ << "\n";
        }
	enablePin(CSN_PIN);
	return data;
}
void NRF24::nrf24_ReadMulti (char reg, char* data, int size)
{	
	//pentru a selecta dispozitivul
	disablePin(CSN_PIN);
	verify_ = spiXfer(SPI_init_, &reg, data, size);
        if(verify_ < 0)
        {
                std::cout << "SPI port read 1byte transfer failed. Error code: " << verify_ << "\n";
                exit(init_);
		return;
        }
        else
        {
                std::cout << "Spi port read 1byte transfer OK. Return code: " << verify_ << "\n";
        }
	enablePin(CSN_PIN);
}

void NRF24::nrfSendCommand(char cmd)
{
	disablePin(CSN_PIN);
	verify_ = spiXfer(SPI_init_, &cmd, nullptr, 1);
	if(verify_ < 0)
        {
                std::cout << "SPI port transfer failed. Error code: " << verify_ << "\n";
                exit(init_);
        }
        else
        {
                std::cout << "Spi port transfer OK. Return code: " << verify_ << "\n";
        }
	enablePin(CSN_PIN);
}

NRF24::NRF24()
{
	init_ = gpioInitialise();
        if(init_ < 0)
        {
                std::cout << "Problema initializare gpio cu " << init_ << "\n";
                exit(init_);
        }
        else
        {
                std::cout << "Jetgpio initialisation OK. Return: " << init_ << "\n";
        }
        verify_ = gpioSetMode(CE_PIN, JET_OUTPUT);
        if(verify_ < 0)
        {
                std::cout << "Nu a mers setarea pinului CE, codul: " << verify_ << "\n";
                exit(init_);
        }
        else
        {
                std::cout << "A mers setarea pinului CE. Codul: " << verify_ << "\n";
	}
	verify_ = gpioSetMode(CSN_PIN, JET_OUTPUT);

        if(verify_ < 0)
        {
                std::cout << "Nu a mers setarea pinului CSN, codul: " << verify_ << "\n";
                exit(init_);
        }
        else
        {
                std::cout << "A mers setarea pinului CSN. Codul: " << verify_ << "\n";
        }

        SPI_init_ = spiOpen(0, 250000, 0, 0, 8, 1, 1);

        if (SPI_init_ < 0)
        {
                /* Port SPI2 opening failed */
                printf("Port SPI1 opening failed. Error code:  %d\n", SPI_init_);
                exit(init_);
        }
        else
        {
                /* Port SPI2 opened  okay*/
                printf("Port SPI1 opened OK. Return code:  %d\n", SPI_init_);
        }

	disablePin(CE_PIN);
	enablePin(CSN_PIN);

	nrf24_WriteReg(CONFIG, 0);//,SPI_init,init); //config later

	nrf24_WriteReg(EN_AA, 0);//, SPI_init, init); // No Auto Ack

	nrf24_WriteReg(EN_RXADDR, 0);//, SPI_init, init); //0 data pipes enabled

	nrf24_WriteReg(SETUP_AW, 0x03);//,SPI_init, init);	//5 bytes for the tx/rx address

	nrf24_WriteReg(SETUP_RETR, 0);//, SPI_init, init); //No Retransmision

	nrf24_WriteReg(RF_CH, 0);//, SPI_init, init);     //Channel will be setup in  during TX or RX

	nrf24_WriteReg(RF_SETUP, 0x0E);//, SPI_init, init);	//Power 0dB, data rate = 2Mbps

	enablePin(CE_PIN);
	disablePin(CSN_PIN);
}

void NRF24::enablePin(int pin)
{
	gpioWrite(pin, 1);
}

void NRF24::disablePin(int pin)
{
	gpioWrite(pin,0);
}

void NRF24::nrf24_TxMode(char address, char channel)
{
	disablePin(CE_PIN);
	enablePin(CSN_PIN);
	
	nrf24_WriteReg(RF_CH, channel);

}	



