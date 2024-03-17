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
void NRF24::WriteReg (char reg, char data)
{
	uint8_t buf[2];
	buf[0] = W_REGISTER | reg;//|1<<5;
	buf[1] = data;
	// char cmd = W_REGISTER;
	
	//pentru a selecta
	disablePin(CSN_PIN);
	// verify_ = spiXfer(SPI_init_, &cmd, nullptr, 1);
	#ifndef JETSON_BOARD
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
	#else
	//pico code ...
	sleep_us(10);
	spi_write_read_blocking(NRF_SPI_PORT, buf, nullptr, 2);
	#endif
	enablePin(CSN_PIN);
}
//write multiple datas
void NRF24::WriteRegMulti(char reg, char* data,int size)
{
	disablePin(CSN_PIN);
	uint8_t* rxData = spiRx;
	uint8_t* txData = spiTx;
	int lenData = size + 1; //32 bytes data + 1 byte command
	
	*txData++ = W_REGISTER | (REGISTER_MASK & reg);

	while (size--)
	{
		*txData++ = *data++;
	}
	#ifndef JETSON_BOARD
	verify_ = spiXfer(SPI_init_, spiTx, spiRx, lenData);
	#else
	//pico_code
	spi_write_read_blocking(NRF_SPI_PORT, spiTx, spiRx, lenData);
	#endif

	char status = *rxData;

	enablePin(CSN_PIN);
}
//read 1 byte
char NRF24::ReadReg (char reg)
{
	uint8_t data='0';
	uint8_t spiRx[32]{};
	uint8_t cmd[2]{};
	cmd[0] = R_REGISTER | reg;
	cmd[1] = NOP;
	//pentru a selecta dispozitivul
	disablePin(CSN_PIN);
	#ifndef JETSON_BOARD
	int verify_ = spiXfer(SPI_init_, cmd, spiRx, 2);
	#else
	//pico code
	spi_write_read_blocking(NRF_SPI_PORT, cmd, spiRx, 2);
	#endif
	data = spiRx[1];
	enablePin(CSN_PIN);
	return data;
}
void NRF24::ReadMulti(char reg, char* data, int size)
{	
	//pentru a selecta dispozitivul
	uint8_t* txData = spiTx;
	uint8_t* rxData = spiRx;
	int lenData = size + 1; // 32 data + 1 command
	*txData = R_REGISTER | reg;
	while (size--)
	{
		*txData = NOP;
	}
	disablePin(CSN_PIN);
	#ifndef JETSON_BOARD
	verify_ = spiXfer(SPI_init_, txData, rxData, lenData);
	#else
	//pico code
	spi_write_read_blocking(NRF_SPI_PORT, txData, rxData, lenData);
	#endif
	char status = *rxData++;
	while(--lenData)
	{
		*data++ = *rxData++;
	}
	enablePin(CSN_PIN);
}

NRF24::NRF24()
{
	#ifndef JETSON_BOARD
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
	tusb_init();
	spi_init(NRF_SPI_PORT, 10000000);
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
	sleep_us(5);
	char cmd = ' ';


	
	WriteReg(SETUP_RETR, 0x00);	//Retransmision(5, 15)
	
	if(ReadReg(SETUP_RETR) == 0x00)
	{
		disablePin(LED_BOARD);
		sleep_ms(500);
	}

	enablePin(LED_BOARD);
	sleep_ms(500);


	WriteReg(DYNPD, 0x00);	//disable dynamic payload
	if(ReadReg(SETUP_RETR) == 0x00)
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

	WriteReg(RF_SETUP, 0x06);	//Power 18dB, data rate = 1Mbps
	if(ReadReg(RF_SETUP) == 0x06)
	{
		disablePin(LED_BOARD);
		sleep_ms(500);
	}

	WriteReg(STATUS, 0x70);

	// WriteReg(FLUSH_RX, NOP);	//RESET RX
	// char command = FLUSH_RX;
	SendCommand(FLUSH_RX);
	SendCommand(FLUSH_TX);
	// #ifndef JETSON_BOARD
	// spiXfer(SPI_init_, &command, nullptr, 1);
	// command = FLUSH_TX;
	// spiXfer(SPI_init_, &command, nullptr, 1);
	// #else
	
	// #endif
	// WriteReg(FLUSH_TX, NOP);	//RESET TX

	WriteReg(CONFIG, 1 << EN_CRC | 1 << CRC0);


	// usleep(5000);
	sleep_ms(5);

	enablePin(CE_PIN);
	enablePin(CSN_PIN);
	// disablePin(CSN_PIN);
}

void NRF24::enablePin(int pin)
{
	#ifndef JETSON_BOARD
	gpioWrite(pin, 1);
	#else
	gpio_put(pin,1);
	#endif
}

void NRF24::disablePin(int pin)
{
	#ifndef JETSON_BOARD
	gpioWrite(pin,0);
	#else
	gpio_put(pin, 0);
	#endif
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
	WriteRegMulti(RX_ADDR_P0, address, 5);
	
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
	#ifndef JETSON_BOARD
	verify_ = spiXfer(SPI_init_, &cmd, nullptr, 1);
	#else
	//pico_code
	spi_write_blocking(NRF_SPI_PORT, (uint8_t*)&cmd, 1);
	#endif
	enablePin(CSN_PIN);
}

void NRF24::TransmitData(uint8_t* data)
{
	//char cmd
	//disablePin(CSN_PIN);
	if(ReadReg(CONFIG) % 2)
	{
		disablePin(CE_PIN);
		WriteReg(CONFIG, ReadReg(CONFIG) - 1);
		enablePin(CE_PIN);
		printf("ENTER IN TRANSMIT MODE\n");
	}
	uint8_t* txData = spiTx; 
	*txData++ = W_TX_PAYLOAD;
	uint8_t size = 33;
	while(--size)
	{
		*txData++ = *data++;
	}
	size = 33;
	disablePin(CSN_PIN);
	// verify_ = spiXfer(SPI_init_, (char*)W_TX_PAYLOAD, nullptr, 1);
	#ifndef JETSON_BOARD
	if(verify_)
	{	
		verify_ = spiXfer(SPI_init_, data, nullptr,	32);
	}
	#else
	//pico code
	spi_write_read_blocking(NRF_SPI_PORT, spiTx, nullptr, size);
	//spi_write_blocking(NRF_SPI_PORT, (const uint8_t*)data, 32);
	#endif

	enablePin(CSN_PIN);

	// usleep(1);
	sleep_us(1);

	char fifo = ReadReg(STATUS);
	SendCommand(NOP);
	
	if(fifo & (1 << MAX_RT) || fifo & (1 << TX_DS) || fifo & (1 << RX_DR))
	{
		// if(!(fifo&(1<<3)))
		// {
			printf("It transmited data\n");
			WriteReg(STATUS, 1 << MAX_RT | 1 << TX_DS | 1 << RX_DR);
			SendCommand(NOP);
			SendCommand(FLUSH_TX);
			SendCommand(NOP);
		// }
		// else
		// {
			// std::cout << "The device isn't connected\n";
			//nrfSendCommand(FLUSH_TX);
		// }
	}

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

	WriteReg(EN_RXADDR, en_rxaddr | (1<<0)); 
	// WriteReg(CONFIG, 0x0e);
	enablePin(CE_PIN);
	//disablePin(CSN_PIN);

}	

uint8_t NRF24::IsDataAvailable(int pipeNr)
{
	if(ReadReg(CONFIG) % 2 == 0)
	{
		disablePin(CE_PIN);
		WriteReg(CONFIG, ReadReg(CONFIG) + 1);
		printf("ENTER IN RX_MODE: \n");
		enablePin(CE_PIN);
	}
	char status = ReadReg(STATUS);
	if ((status & (1 << 6)) && 
	    (status & (pipeNr << 1)))
	{
		WriteReg(STATUS, (1 << RX_DR));
		return 1;
	}
	return 0;
}

bool NRF24::ReceiveData(char* data)
{
	uint8_t* currentData = (uint8_t*) data;
	uint8_t* txData = spiTx;
	uint8_t* rxData = spiRx;
	int size = 32 + 1;
	*txData++ = R_RX_PAYLOAD;
	while (--size)
	{
		*txData++ = NOP;
	}
	size = 32 + 1;
	disablePin(CSN_PIN);
	#ifndef JETSON_BOARD
	verify_ = spiXfer(SPI_init_, spiTx, spiRx, size);
	#else
	//pico code
	spi_write_read_blocking(NRF_SPI_PORT, spiTx, spiRx, size);
	#endif
	char status = *rxData++;
	memcpy(data, rxData, sizeof(char)*32);
	printf("%32s\n", rxData);
	enablePin(CSN_PIN);

	//asta e un bug, trb sa continui sa citesc pana cand eliberez memoria
	//din RX fifo, nu trb sa-i dau flush
	char fifo = ReadReg(FIFO_STATUS);
	return !(fifo & (1 << 0)); 
}




