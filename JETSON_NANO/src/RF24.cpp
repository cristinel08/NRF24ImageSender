#include"RF24.h"
#include "nrf24l01.h"
//write a single byte
void NRF24::WriteReg (char reg, char data)
{
	char buf[2];
	buf[0] = W_REGISTER | reg;//|1<<5;
	buf[1] = data;
	// char cmd = W_REGISTER;
	
	//pentru a selecta
	disablePin(CSN_PIN);
	// verify_ = spiXfer(SPI_init_, &cmd, nullptr, 1);
	usleep(10);
	if(verify_)
	{
		verify_ = spiXfer(SPI_init_, buf, nullptr, 2);
		usleep(10);
	}

	if(verify_ < 0)
	{
			std::cout << "SPI port transfer failed. Error code: " << verify_ << "\n";
			exit(init_);
	}
	enablePin(CSN_PIN);
}
//write multiple datas
void NRF24::WriteRegMulti(char reg, char* data,int size)
{
	disablePin(CSN_PIN);
	char* rxData = spiRx;
	char* txData = spiTx;
	int lenData = size + 1; //32 bytes data + 1 byte command
	
	*txData++ = W_REGISTER | (REGISTER_MASK & reg);

	while (size--)
	{
		*txData++ = *data++;
	}

	verify_ = spiXfer(SPI_init_, spiTx, spiRx, lenData);

	char status = *rxData;

	enablePin(CSN_PIN);
}
//read 1 byte
char NRF24::ReadReg (char reg)
{
	char data='0';
	int status{};
	char spiRx[32]{};
	char cmd[2]{};
	cmd[0] = R_REGISTER | reg;
	cmd[1] = NOP;
	//pentru a selecta dispozitivul
	gpioWrite(CSN_PIN,0);
	int verify_ = spiXfer(SPI_init_, cmd, spiRx, 2);
	status = (int)spiRx[0];
	data = spiRx[1];
	gpioWrite(CSN_PIN,1);
	return data;
}
void NRF24::ReadMulti(char reg, char* data, int size)
{	
	//pentru a selecta dispozitivul
	// char cmd = R_REGISTER | reg;
	char* txData = spiTx;
	char* rxData = spiRx;
	int lenData = size + 1; // 32 data + 1 command
	*txData = R_REGISTER | reg;
	while (size--)
	{
		*txData = NOP;
	}
	disablePin(CSN_PIN);
	verify_ = spiXfer(SPI_init_, txData, rxData, lenData);
	char status = *rxData++;
	while(--lenData)
	{
		*data++ = *rxData++;
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

	SPI_init_ = spiOpen(0, 10000000, 0, 1, 8, 0, 0);

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
	// enablePin(CSN_PIN);
	disablePin(CSN_PIN);
	usleep(5);

	WriteReg(SETUP_RETR, 0x00);	//Retransmision(5, 15)

	WriteReg(DYNPD, 0x00);	//disable dynamic payload

	WriteReg(EN_AA, 0x3F); // Enable Auto-ack on all pipes

	WriteReg(EN_RXADDR, 0x03);//, SPI_init, init); //0 data pipes enabled

	for(int i = 0; i < 6; i++)
	{
		WriteReg(RX_ADDR_P0 + i, 32);
	}
	WriteReg(FEATURE, 0);

	WriteReg(SETUP_AW, 0x03); 	//5 bytes for the tx/rx address

	WriteReg(RF_CH, 0x4c);	    //Channel will be setup in  during TX or RX

	WriteReg(RF_SETUP, 0x06);	//Power 18dB, data rate = 1Mbps
	
	WriteReg(STATUS, 0x70);

	// WriteReg(FLUSH_RX, NOP);	//RESET RX
	char command = FLUSH_RX;
	spiXfer(SPI_init_, &command, nullptr, 1);
	command = FLUSH_TX;
	spiXfer(SPI_init_, &command, nullptr, 1);
	// WriteReg(FLUSH_TX, NOP);	//RESET TX

	WriteReg(CONFIG, 1 << EN_CRC | 1 << CRC0);

	usleep(5000);

	enablePin(CE_PIN);
	enablePin(CSN_PIN);
	// disablePin(CSN_PIN);
}

void NRF24::enablePin(int pin)
{
	gpioWrite(pin, 1);
}

void NRF24::disablePin(int pin)
{
	gpioWrite(pin,0);
}
void NRF24::OpenWritingPipe(char* address)
{
	WriteRegMulti(RX_ADDR_P0, address, 5);
	WriteRegMulti(TX_ADDR, address, 5);
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
	// WriteReg(CONFIG, 0x0f);
	enablePin(CE_PIN);
	//disablePin(CSN_PIN);
}	

void NRF24::SendCommand(char cmd)
{
	disablePin(CSN_PIN);
	verify_ = spiXfer(SPI_init_, &cmd, nullptr, 1);
	enablePin(CSN_PIN);
}

void NRF24::TransmitData(char* data)
{
	//char cmd
	char* txData = spiTx;
	char* rxData = spiRx;
	const char* current = data;
	int size = 32 + 1;
	*txData++ = W_TX_PAYLOAD;
	while(size--)
	{
		*txData++ = *current++;
	}
	//disablePin(CSN_PIN);
	size = 33;

	disablePin(CSN_PIN);

	// verify_ = spiXfer(SPI_init_, (char*)W_TX_PAYLOAD, nullptr, 1);
	if(verify_)
	{	
		verify_ = spiXfer(SPI_init_, spiTx, spiRx, size);
	}

	enablePin(CSN_PIN);

	usleep(1);

	char fifo = ReadReg(FIFO_STATUS);
	SendCommand(NOP);
	// SendCommand(FLUSH_TX);
	// SendCommand(NOP);
	
	// if(fifo == 33)
	// {
	// 	if(!(fifo&(1<<3)))
	// 	{
	// 		std::cout << "It transmited data\n";
	// 		SendCommand(FLUSH_TX);
	// 		SendCommand(NOP);
	// 	}
	// 	else
	// 	{
	// 		std::cout << "The device isn't connected\n";
	// 		//nrfSendCommand(FLUSH_TX);
	// 	}
	// }

}


void NRF24::RxMode(char* address, char channel)
{
	disablePin(CE_PIN);
	//enablePin(CSN_PIN);

	//select the channel
	WriteReg(RF_CH, channel);
	//select data pipe 1

    //write the tx address
	WriteRegMulti(RX_ADDR_P1, address, 5);

	//setting the payload to maximum 32 bits
	WriteReg(RX_PW_P1, 32);

	//power up the device in RX mode
	char config = ReadReg(CONFIG);
	config = config | (1 << 1) | (1 << 0);
	WriteReg(CONFIG, config);
	WriteReg(STATUS, 0x70);
	char en_rxaddr = ReadReg(EN_RXADDR);

	WriteReg(EN_RXADDR, en_rxaddr | (1<<1)); 
	// WriteReg(CONFIG, 0x0e);
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
	char* currentData = data;
	char* txData = spiTx;
	char* rxData = spiRx;
	int size = 32 + 1;
	*txData++ = R_RX_PAYLOAD;
	while (--size)
	{
		*txData++ = NOP;
	}
	size = 32 + 1;
	gpioWrite(CSN_PIN, 0);
	verify_ = spiXfer(SPI_init_, spiTx, spiRx, size);
	char status = *rxData++;
	memcpy(data, rxData,sizeof(char)*32);
	enablePin(CSN_PIN);

	char fifo = ReadReg(FIFO_STATUS);
	if(fifo & (1 << 1))
	{
		SendCommand(FLUSH_RX);
		WriteReg(STATUS, 1 << RX_DR);
	}
}




