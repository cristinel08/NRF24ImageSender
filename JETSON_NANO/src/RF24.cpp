#include"RF24.h"

int main()
{
	char dataTx[] = {"Buna Cristi!"};
	char txAddress[] = {0xEE,0xDD,0xCC,0xBB,0xAA};
	char rxAddress[] = {0xEE,0xDD,0xCC,0xBB,0xAA};
	char data[32] = {};

	NRF24 nrf24 = NRF24();
	//nrf24.TxMode(txAddress, 5);
	//nrf24.TransmitData(dataTx);

	nrf24.RxMode(rxAddress, 10);
	while(1)
	{
		if(nrf24.IsDataAvailable(1))
		{
			nrf24.ReceiveData(data);
			std::cout << data << '\n';
		}
		else 
		{
			std::cout << 
			"Data is not available\n";
			break;
		}
	}
}

//write a single byte
void NRF24::WriteReg (char reg, char data)
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
void NRF24::WriteRegMulti(char reg, char* data,int size)
{
	//char buf[size];
	std::unique_ptr<char[]>buf = std::make_unique<char[]>(size);
	//std::strcpy(
	buf.get()[0] = reg|1<<5;
	strcpy(buf.get(), data);
	//buf[1] = data;

        disablePin(CSN_PIN);
        verify_ = spiXfer(SPI_init_, buf.get(),nullptr, size);
        if(verify_ < 0)
        {
                std::cout << "SPI port transfer failed. Error code: " << verify_ << "\n";
                exit(init_);
        }
        else
        {
                std::cout << "Spi port transfer OK";
        }
        enablePin(CSN_PIN);
}
//read 1 byte
char NRF24::ReadReg (char reg)
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
void NRF24::ReadMulti(char reg, char* data, int size)
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

void NRF24::SendCommand(char cmd)
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

	WriteReg(CONFIG, 0);//,SPI_init,init); //config later

	WriteReg(EN_AA, 0);//, SPI_init, init); // No Auto Ack

	WriteReg(EN_RXADDR, 0);//, SPI_init, init); //0 data pipes enabled

	WriteReg(SETUP_AW, 0x03);//,SPI_init, init);	//5 bytes for the tx/rx address

	WriteReg(SETUP_RETR, 0);//, SPI_init, init); //No Retransmision

	WriteReg(RF_CH, 0);//, SPI_init, init);     //Channel will be setup in  during TX or RX

	WriteReg(RF_SETUP, 0x0E);//, SPI_init, init);	//Power 0dB, data rate = 2Mbps

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

void NRF24::TxMode(char* address, char channel)
{
	disablePin(CE_PIN);
	//enablePin(CSN_PIN);

	//select the channel
	WriteReg(RF_CH, channel); 
	
	//write the tx address
	WriteRegMulti(TX_ADDR, address, 5);
	
	//power up the device 
	char config = ReadReg(CONFIG);
	config = config | (1 << 1);
	WriteReg(CONFIG, config);
	enablePin(CE_PIN);
	//disablePin(CSN_PIN);
}	

void NRF24::TransmitData(char* data)
{
	//char cmd
	//disablePin(CSN_PIN);

	SendCommand(W_TX_PAYLOAD);
	disablePin(CSN_PIN);
	verify_ = spiXfer(SPI_init_, data, nullptr,						  32);
	enablePin(CSN_PIN);

	usleep(1);

	char fifo = ReadReg(FIFO_STATUS);
	
	if(fifo&(1<<4))
	{
		if(!(fifo&(1<<3)))
		{
			std::cout << "It transmited data\n";
			SendCommand(FLUSH_TX);
		}
		else
		{
			std::cout << "The device isn't connected\n";
			//nrfSendCommand(FLUSH_TX);
		}
	}

}


void NRF24::RxMode(char* address, char channel)
{
        disablePin(CE_PIN);
        //enablePin(CSN_PIN);

        //select the channel
        WriteReg(RF_CH, channel);
	
	char en_rxaddr = ReadReg(EN_RXADDR);

	//select data pipe 1
	WriteReg(EN_RXADDR, en_rxaddr | (1<<1)); 

        //write the tx address
        WriteRegMulti(RX_ADDR_P1, address, 5);

	//setting the payload to maximum 32 bits
	WriteReg(RX_PW_P1, 32);

        //power up the device in RX mode
        char config = ReadReg(CONFIG);
        config = config | (1 << 1) | (1 << 0);
        WriteReg(CONFIG, config);
        enablePin(CE_PIN);
        //disablePin(CSN_PIN);

}	

uint8_t NRF24::IsDataAvailable(int pipeNr)
{
	char status = ReadReg(STATUS);

	if ((status & (1 << 6)) && 
	    (status & (pipeNr << 1)))
	{
		WriteReg(STATUS, (1 << 6));
		return 1;
	}
	return 0;
}

void NRF24::ReceiveData(char* data)
{

        SendCommand(R_RX_PAYLOAD);
        disablePin(CSN_PIN);
        verify_ = spiXfer(SPI_init_, nullptr
				   , data, 32);
        enablePin(CSN_PIN);

        usleep(1);

	SendCommand(FLUSH_RX);
}




