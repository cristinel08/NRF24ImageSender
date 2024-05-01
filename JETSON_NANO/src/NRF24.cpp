#include "NRF24.h"
#include "nrf24l01.h"

NRF24::~NRF24()
{
	//shut down the nrf
	disablePin(CE_PIN);
	WriteReg(CONFIG, 0x00);
	spiClose(SPI_init_);
	disablePin(CSN_PIN);
}
void NRF24::WriteReg (const char& reg, const char& data)
{
	char buf[2]{static_cast<char>(W_REGISTER | reg), data};
	// char cmd = W_REGISTER;
	
	//pentru a selecta
	disablePin(CSN_PIN);
	if(verify_)
	{
		verify_ = spiXfer(SPI_init_, buf, nullptr, 2);
	}

	if(verify_ < 0)
	{
		std::cout << "SPI port transfer failed. Error code: " << verify_ << "\n";
		exit(init_);
	}
	enablePin(CSN_PIN);
}

void NRF24::WriteRegMulti(const char& reg, char* data, int len)
{

	char* rxData = spiRx;
	char* txData = spiTx;
	size = len + 1; //32 bytes data + 1 byte command
	
	*txData++ = W_REGISTER | (REGISTER_MASK & reg);

	memcpy(txData, data, sizeof(char) * len);

	disablePin(CSN_PIN);
	verify_ = spiXfer(SPI_init_, spiTx, spiRx, size);
	enablePin(CSN_PIN);

	status = *rxData;


}
//read 1 byte
char NRF24::ReadReg (const char& reg)
{
	char cmd[2]{static_cast<char>(R_REGISTER | reg), NOP};
	//pentru a selecta dispozitivul
	{
		gpioWrite(CSN_PIN,0);
		verify_ = spiXfer(SPI_init_, cmd, spiRx, 2);
		gpioWrite(CSN_PIN,1);
	}
	return spiRx[1];
}
void NRF24::ReadMulti(const char& reg, char* data, int len)
{	
	//pentru a selecta dispozitivul
	char* txData = spiTx;
	char* rxData = spiRx;
	size = len + 1; // 32 data + 1 command
	*txData = R_REGISTER | reg;
	{
		disablePin(CSN_PIN);
		verify_ = spiXfer(SPI_init_, txData, rxData, size);
		enablePin(CSN_PIN);
	}
	status = *rxData++;
	memcpy(data, rxData, sizeof(char) * len);

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

	SPI_init_ = spiOpen(0, 10'000'000, 0, 1, 8, 0, 0);

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
	disablePin(CSN_PIN);
	usleep(5);

	
	WriteReg(SETUP_RETR, 0x00);	//Retransmision(5, 15)

	WriteReg(DYNPD, 0x00);	//disable dynamic payload

	WriteReg(EN_AA, 0x00); // Enable Auto-ack on all pipes

	WriteReg(EN_RXADDR, 0x03);//, SPI_init, init); //0 data pipes enabled

	for(int i = 0; i < 6; i++)
	{
		WriteReg(RX_ADDR_P0 + i, 32);
	}
	WriteReg(FEATURE, 0x00); //enable ack_payload and dynamic_payload

	WriteReg(SETUP_AW, 0x03); 	//5 bytes for the tx/rx address

	WriteReg(RF_CH, 0x4c);	    //Channel will be setup in  during TX or RX

	WriteReg(RF_SETUP, 0x06);	//Power 18dB, data rate = 1Mbps
	
	WriteReg(STATUS, 0x70);

	SendCommand(FLUSH_RX);
	SendCommand(NOP);
	SendCommand(FLUSH_TX);
	SendCommand(NOP);


	WriteReg(CONFIG, 1 << EN_CRC | 1 << CRC0);

	usleep(5000);

	enablePin(CE_PIN);
	enablePin(CSN_PIN);
}

void NRF24::enablePin(const uint8_t& pin)
{
	gpioWrite(pin, 1);
}

void NRF24::disablePin(const uint8_t& pin)
{
	gpioWrite(pin,0);
}
void NRF24::OpenWritingAddress(char* address)
{
	WriteRegMulti(RX_ADDR_P0, address, 5);
	WriteRegMulti(TX_ADDR, address, 5);
}
void NRF24::TxMode(char* address, const char& channel)
{
	disablePin(CE_PIN);
	//enablePin(CSN_PIN);

	//select the channel
	WriteReg(RF_CH, channel); 
	
	//write the tx address
	WriteRegMulti(RX_ADDR_P0, address, 5);
	WriteRegMulti(TX_ADDR, address, 5);
	
	//power up the device 
	config = ReadReg(CONFIG);
	config = config | (1 << 1);
	WriteReg(CONFIG, config);

	enablePin(CE_PIN);
}	

void NRF24::SendCommand(char cmd)
{
	disablePin(CSN_PIN);
	verify_ = spiXfer(SPI_init_, &cmd, nullptr, 1);
	enablePin(CSN_PIN);
}

void NRF24::TransmitData(char* data)
{
	if(ReadReg(CONFIG) % 2)
	{
		disablePin(CE_PIN);
		WriteReg(CONFIG, ReadReg(CONFIG) - 1);
		enablePin(CE_PIN);
		usleep(140);
	}
	char* txData = spiTx;
	char* rxData = spiRx;
	size = 32;
	*txData++ = W_TX_PAYLOAD;
	memcpy(txData, data, sizeof(char) * size);

	size = size + 1;

	disablePin(CSN_PIN);
	if(verify_)
	{	
		verify_ = spiXfer(SPI_init_, spiTx, spiRx, size);
	}
	enablePin(CSN_PIN);
	fifo = ReadReg(FIFO_STATUS);
	if(!(fifo&(1<<3)))
	{
		if(!(fifo&(1<<3)))
		{
			printf("It transmited data %32s\n", data);
			status = ReadReg(STATUS);
			if (status & (1 << MAX_RT))
			{
				WriteReg(STATUS, 1 << MAX_RT);
			}
			if (status & (1<< TX_DS))
			{
				WriteReg(STATUS, 1 << TX_DS);
			}						
		}

		else
		{
			std::cout << "The device isn't connected\n";
			SendCommand(FLUSH_TX);
		}

	}
	

}


void NRF24::RxMode(char* address, const char& channel)
{
	disablePin(CE_PIN);

	//select the channel
	WriteReg(RF_CH, channel);
	//select data pipe 1

    //write the tx address
	WriteRegMulti(RX_ADDR_P1, address, 5);

	//setting the payload to maximum 32 bits
	WriteReg(RX_PW_P1, 32);

	//power up the device in RX mode
	config = ReadReg(CONFIG);
	config = config | (1 << 1) | (1 << 0);
	WriteReg(CONFIG, config);
	WriteReg(STATUS, 0x70);
	en_rxaddr = ReadReg(EN_RXADDR);

	WriteReg(EN_RXADDR, en_rxaddr | (1 << 1)); 
	enablePin(CE_PIN);

}	

bool NRF24::IsDataAvailable(const uint8_t& pipeNr)
{
	if(ReadReg(CONFIG) % 2 == 0)
	{
		disablePin(CE_PIN);
		WriteReg(CONFIG, ReadReg(CONFIG) + 1);
		enablePin(CE_PIN);
		usleep(130);
	}
	status = ReadReg(STATUS);
	if ((status & (1 << 6)) && 
	    (status & (pipeNr << 1)))
	{
		WriteReg(STATUS, (1 << RX_DR));
		return 1;
	}
	return 0;
}

bool NRF24::ReceiveData(char* data, const uint8_t& copySize)
{
	char* currentData = data;
	char* txData = spiTx;
	char* rxData = spiRx;
	size = 32 + 1;
	*txData++ = R_RX_PAYLOAD;
	size = 32 + 1;
	gpioWrite(CSN_PIN, 0);
	verify_ = spiXfer(SPI_init_, spiTx, spiRx, size);
	status = *rxData++;
	enablePin(CSN_PIN);
	memcpy(data, rxData,sizeof(char)*copySize);
	return true;
}




