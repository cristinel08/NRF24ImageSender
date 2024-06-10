#include"RF24.h"
#include "nrf24l01.h"
//write a single byte
NRF24::~NRF24()
{
	disablePin(CE_PIN);
	WriteReg(CONFIG, 0x00);
	spi_deinit(NRF_SPI_PORT);
	disablePin(CSN_PIN);
}
UINT8 NRF24::GetStatus()
{
	WriteReg(NOP, NOP);
	return status;
}
void NRF24::WriteReg (const UINT8& reg, const UINT8& data)
{
	UINT8 buf[2];
	buf[0] = W_REGISTER | reg;//|1<<5;
	buf[1] = data;
	UINT8 rxData[2]{};
	// UINT8 cmd = W_REGISTER;
	
	//pentru a selecta
	disablePin(CSN_PIN);
	// verify_ = spiXfer(SPI_init_, &cmd, nullptr, 1);
	#ifndef PICO
	// usleep(10);
	if(verify_)
	{
		verify_ = spiXfer(SPI_init_, buf, nullptr, 2);
		// usleep(10);
	}

	if(verify_ < 0)
	{
			std::cout << "SPI port transfer failed. Error code: " << verify_ << "\n";
			exit(init_);
	}
	#else
	//pico code ...
	// sleep_us(10);
	spi_write_read_blocking(NRF_SPI_PORT, buf, rxData, 2);
	#endif
	status = rxData[0];
	enablePin(CSN_PIN);
}
//write multiple datas
void NRF24::WriteRegMulti(const UINT8& reg, UINT8* data,int size)
{
	disablePin(CSN_PIN);
	UINT8* rxData = spiRx;
	UINT8* txData = spiTx;
	lenData = size + 1; //32 bytes data + 1 byte command
	
	*txData++ = W_REGISTER | (REGISTER_MASK & reg);

	while (size--)
	{
		*txData++ = *data++;
	}
	#ifndef PICO
	verify_ = spiXfer(SPI_init_, spiTx, spiRx, lenData);
	#else
	//pico_code
	spi_write_read_blocking(NRF_SPI_PORT, spiTx, spiRx, lenData);
	#endif

	status = *rxData;

	enablePin(CSN_PIN);
}
//read 1 byte
UINT8 NRF24::ReadReg (const UINT8& reg)
{
	UINT8 cmd[2]{static_cast<UINT8>(R_REGISTER | reg), NOP};
	//pentru a selecta dispozitivul
	disablePin(CSN_PIN);
	#ifndef PICO
	int verify_ = spiXfer(SPI_init_, cmd, spiRx, 2);
	#else
	//pico code
	spi_write_read_blocking(NRF_SPI_PORT, cmd, spiRx, 2);
	#endif
	enablePin(CSN_PIN);
	return spiRx[1];
}
void NRF24::ReadMulti(const UINT8& reg, UINT8* data, int size)
{	
	//pentru a selecta dispozitivul
	UINT8* txData = spiTx;
	UINT8* rxData = spiRx;
	lenData = size + 1; // 32 data + 1 command
	*txData = R_REGISTER | reg;
	while (size--)
	{
		*txData = NOP;
	}
	disablePin(CSN_PIN);
	#ifndef PICO
	verify_ = spiXfer(SPI_init_, txData, rxData, lenData);
	#else
	//pico code
	spi_write_read_blocking(NRF_SPI_PORT, txData, rxData, lenData);
	#endif
	status = *rxData++;
	while(--lenData)
	{
		*data++ = *rxData++;
	}
	enablePin(CSN_PIN);
}

NRF24::NRF24()
{
	#ifndef PICO
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
	#else
	//pico_code
	stdio_init_all();
	spi_init(NRF_SPI_PORT, 10'000'000);
	gpio_set_function(MISO_PIN, GPIO_FUNC_SPI);
	gpio_set_function(SCK_PIN, GPIO_FUNC_SPI);
	gpio_set_function(MOSI_PIN, GPIO_FUNC_SPI);
	gpio_init(CE_PIN);
	gpio_init(CSN_PIN);
	gpio_init(LED_BOARD);
	gpio_set_dir(LED_BOARD, GPIO_OUT);
	enablePin(LED_BOARD);
	gpio_set_dir(CE_PIN, GPIO_OUT);
    gpio_set_dir(CSN_PIN, GPIO_OUT);
	#endif
	disablePin(CE_PIN);
	// enablePin(CSN_PIN);
	disablePin(CSN_PIN);
	// usleep(5);
	sleep_ms(5);
	
	WriteReg(SETUP_RETR, 0x1F);	//Retransmision(5, 15)
	
	if(ReadReg(SETUP_RETR) == 0x1F)
	{
		disablePin(LED_BOARD);
		sleep_ms(500);
	}

	enablePin(LED_BOARD);
	sleep_ms(500);


	WriteReg(DYNPD, 0x00);	//disable dynamic payload
	if(ReadReg(DYNPD) == 0x00)
	{
		disablePin(LED_BOARD);
		sleep_ms(500);
	}
	enablePin(LED_BOARD);
	sleep_ms(500);

	WriteReg(EN_AA, 0x3F); // Enable Auto-ack on all pipes
	if(ReadReg(EN_AA) == 0x3F)
	{
		disablePin(LED_BOARD);
		sleep_ms(500);
	}

	enablePin(LED_BOARD);
	sleep_ms(500);

	WriteReg(EN_RXADDR, 0x03);//, SPI_init, init); //0 data pipes enabled
	if(ReadReg(EN_RXADDR) == 0x03)
	{
		disablePin(LED_BOARD);
		sleep_ms(500);
	}
	enablePin(LED_BOARD);
	sleep_ms(500);

	for(int i = 0; i < 6; i++)
	{
		WriteReg(RX_ADDR_P0 + i, 32);
	}
	WriteReg(FEATURE, 0);
	if(ReadReg(FEATURE) == 0x00)
	{
		disablePin(LED_BOARD);
		sleep_ms(500);
	}
	enablePin(LED_BOARD);
	sleep_ms(500);

	WriteReg(SETUP_AW, 0x03); 	//5 bytes for the tx/rx address
	if(ReadReg(SETUP_AW) == 0x03)
	{
		disablePin(LED_BOARD);
		sleep_ms(500);
	}
	enablePin(LED_BOARD);
	sleep_ms(500);


	WriteReg(RF_CH, 0x4c);	    //Channel will be setup in  during TX or RX
	if(ReadReg(RF_CH) == 0x4c)
	{
		disablePin(LED_BOARD);
		sleep_ms(500);
	}
	enablePin(LED_BOARD);
	sleep_ms(500);

	WriteReg(RF_SETUP, 0x06);	//Power 0dB, data rate = 2Mbps
	if(ReadReg(RF_SETUP) == 0x06)
	{
		disablePin(LED_BOARD);
		sleep_ms(500);
	}

	WriteReg(STATUS, 0x70);

	// WriteReg(FLUSH_RX, NOP);	//RESET RX
	// UINT8 command = FLUSH_RX;
	SendCommand(FLUSH_RX);
	// SendCommand(NOP);
	SendCommand(FLUSH_TX);
	// SendCommand(NOP);
	// #ifndef PICO
	// spiXfer(SPI_init_, &command, nullptr, 1);
	// command = FLUSH_TX;
	// spiXfer(SPI_init_, &command, nullptr, 1);
	// #else
	
	// #endif
	// WriteReg(FLUSH_TX, NOP);	//RESET TX

	WriteReg(CONFIG, 1 << EN_CRC | ~(1 << CRC0));


	// usleep(5000);
	sleep_ms(5);

	// enablePin(CE_PIN);
	enablePin(CSN_PIN);
	// disablePin(CSN_PIN);
}

void NRF24::enablePin(const uint8_t& pin)
{
	#ifndef PICO
	gpioWrite(pin, 1);
	#else
	gpio_put(pin,1);
	sleep_us(5);
	#endif
}

void NRF24::disablePin(const uint8_t& pin)
{
	#ifndef PICO
	gpioWrite(pin,0);
	#else
	gpio_put(pin, 0);
	sleep_us(5);
	#endif
}
void NRF24::OpenWritingPipe(UINT8* address)
{
	WriteRegMulti(RX_ADDR_P0, address, 5);
	WriteRegMulti(TX_ADDR, address, 5);
}
void NRF24::TxMode(UINT8* address, const UINT8& channel)
{
	disablePin(CE_PIN);
	//enablePin(CSN_PIN);

	//select the channel
	WriteReg(RF_CH, channel); 
	
	//write the tx address
	WriteRegMulti(TX_ADDR, address, 5);
	// WriteRegMulti(RX_ADDR_P0, address, 5);
	
	//power up the device 
	status = ReadReg(CONFIG);
	status = status | (1 << PWR_UP);
	WriteReg(CONFIG, status);
	// WriteReg(CONFIG, 0x0f);
	enablePin(CE_PIN);
	//disablePin(CSN_PIN);
}	

void NRF24::SendCommand(const UINT8& cmd)
{
	UINT8 buf[1]{cmd};
	disablePin(CSN_PIN);
	#ifndef PICO
	verify_ = spiXfer(SPI_init_, buf, nullptr, 1);
	#else
	//pico_code
	spi_write_read_blocking(NRF_SPI_PORT, buf, nullptr, 1);
	#endif
	enablePin(CSN_PIN);
}

bool NRF24::TransmitData(UINT8* data, uint8_t const& dataSize)
{
	//UINT8 cmd
	//disablePin(CSN_PIN);
	UINT8* txData = spiTx; 
	*txData++ = W_TX_PAYLOAD;
	Set2Tx();
	UINT8 size = dataSize;
	memcpy(txData, data, sizeof(UINT8) * dataSize);
	if(dataSize < 32)
	{
		for(; size < 32; size++)
		{
			spiTx[size] = 0;
		}
	}
	size = 33;
	disablePin(CSN_PIN);
	// verify_ = spiXfer(SPI_init_, (const UINT8&*)W_TX_PAYLOAD, nullptr, 1);
	#ifndef PICO
	if(verify_)
	{	
		verify_ = spiXfer(SPI_init_, spiTx, nullptr, size);
	}
	#else
	//pico code
	spi_write_read_blocking(NRF_SPI_PORT, spiTx, spiRx, size);
	//spi_write_blocking(NRF_SPI_PORT, (const UINT8*)data, 32);
	#endif
	enablePin(CSN_PIN);
	enablePin(CE_PIN);
	// sleep_ms(1);
	uint32_t timer = millis();
	while(!(GetStatus() & ((1 << TX_DS) | (1 << MAX_RT))))
	{
		if(millis() - timer > 95)
		{
			return 0;
		}
	}
	disablePin(CE_PIN);
	// SendCommand(NOP);
	WriteReg(STATUS, 1 << RX_DR | 1 << TX_DS | 1 << MAX_RT);
	if(status & (1 << MAX_RT))
	{
		SendCommand(FLUSH_RX);
		return 0;
	}
	return 1;
}


void NRF24::RxMode(UINT8* address, const UINT8& channel)
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
	status = ReadReg(CONFIG);
	status = status | (1 << 1) | (1 << 0);
	WriteReg(CONFIG, status);
	WriteReg(STATUS, 0x70);
	status = ReadReg(EN_RXADDR);

	WriteReg(EN_RXADDR, status | (1 << 1)); 
	enablePin(CE_PIN);

}	

void NRF24::Set2Rx()
{
	status = ReadReg(CONFIG);
	if(status % 2 == 0)
	{
		WriteReg(CONFIG, status | (1 << 0));
		WriteReg(EN_RXADDR, ReadReg(EN_RXADDR) & ~(1 << 0));
		enablePin(CE_PIN);


	}
}

void NRF24::Set2Tx()
{
	status = ReadReg(CONFIG);
	if(status % 2)
	{
		disablePin(CE_PIN);
		sleep_us(280);
		WriteReg(CONFIG, status & ~(1 << 0));
		WriteReg(EN_RXADDR, ReadReg(EN_RXADDR) | (1 << 0));
		// enablePin(CE_PIN);

	}
}

UINT8 NRF24::IsDataAvailable(const uint8_t& pipeNr)
{
	Set2Rx();
	status = ReadReg(STATUS);
	if ((status & (1 << RX_DR)) && 
	    (status & (pipeNr << 1)))
	{
		WriteReg(STATUS, (1 << RX_DR));
		return 1;
	}
	return 0;
}

bool NRF24::ReceiveData(UINT8* data)
{
	UINT8* txData = spiTx;
	UINT8* rxData = spiRx;
	lenData = 32 + 1;
	*txData++ = R_RX_PAYLOAD;
	while (--lenData)
	{
		*txData++ = NOP;
	}
	lenData = 32 + 1;
	disablePin(CSN_PIN);
	#ifndef PICO
	verify_ = spiXfer(SPI_init_, spiTx, spiRx, lenData);
	#else
	//pico code
	spi_write_read_blocking(NRF_SPI_PORT, spiTx, spiRx, lenData);
	#endif
	enablePin(CSN_PIN);
	status = *rxData++;
	memcpy(data, rxData, sizeof(UINT8) * (lenData - 1));
	status = ReadReg(FIFO_STATUS);
	return !(status & 1); 
}




