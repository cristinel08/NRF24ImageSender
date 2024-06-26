#include"NRF24.h"
#include "nrf24l01.h"
//write a single byte
NRF24::~NRF24(
	void
)
{
	DisablePin(CE_PIN);
	WriteReg(CONFIG, 0x00);
	spi_deinit(NRF_SPI_PORT);
	DisablePin(CSN_PIN);
}
UINT8 NRF24::GetStatus(
	void
)
{
	WriteReg(NOP, NOP);
	return status;
}
void NRF24::WriteReg (
	UINT8 const& reg, 
	UINT8 const& data
)
{
	UINT8 buf[2];
	buf[0] = W_REGISTER | reg;
	buf[1] = data;
	//pentru a selecta
	DisablePin(CSN_PIN);
	spi_write_read_blocking(NRF_SPI_PORT, buf, spiRx, 2);
	status = spiRx[0];
	EnablePin(CSN_PIN);
}
//write multiple datas
void NRF24::WriteRegMulti(
	UINT8 const& reg, 
	UINT8* data,
	uint8_t const& size
)
{

	UINT8* rxData = spiRx;
	UINT8* txData = spiTx;
	lenData = size + 1; //32 bytes data + 1 byte command
	
	*txData++ = W_REGISTER | (REGISTER_MASK & reg);
	for(uint8_t i{0}; i < size; i++)
	{
		*txData++ = *data++;
	}
	//pico_code
	DisablePin(CSN_PIN);
	spi_write_read_blocking(NRF_SPI_PORT, spiTx, spiRx, size + 1);
	EnablePin(CSN_PIN);
	status = *rxData;
}
//read 1 byte
UINT8 NRF24::ReadReg (UINT8 const& reg)
{
	UINT8 cmd[2]{static_cast<UINT8>(R_REGISTER | reg), NOP};
	//pentru a selecta dispozitivul
	DisablePin(CSN_PIN);
	spi_write_read_blocking(NRF_SPI_PORT, cmd, spiRx, 2);
	EnablePin(CSN_PIN);
	return spiRx[1];
}
void NRF24::ReadMulti(
	UINT8 const& reg, 
	UINT8* data, 
	uint8_t const& size
)
{	
	//pentru a selecta dispozitivul
	UINT8* txData = spiTx;
	UINT8* rxData = spiRx;
	*txData = R_REGISTER | reg;
	for(uint8_t i{0}; i < size; i++)
	{
		*txData++ = NOP;
	}
	DisablePin(CSN_PIN);
	spi_write_read_blocking(NRF_SPI_PORT, txData, rxData, size + 1);
	EnablePin(CSN_PIN);
	status = *rxData++;
	memcpy(data, rxData, sizeof(UINT8) * size);
}

NRF24::NRF24(
	void
)
{
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
	EnablePin(LED_BOARD);
	gpio_set_dir(CE_PIN, GPIO_OUT);
    gpio_set_dir(CSN_PIN, GPIO_OUT);
	DisablePin(CE_PIN);
	EnablePin(CSN_PIN);
	// usleep(5);
	sleep_ms(5);
	
	WriteReg(SETUP_RETR, 0x1F);
	
	if(ReadReg(SETUP_RETR) == 0x1F)
	{
		DisablePin(LED_BOARD);
		sleep_ms(500);
	}

	EnablePin(LED_BOARD);
	sleep_ms(500);


	WriteReg(DYNPD, 0x00);	//disable dynamic payload
	if(ReadReg(DYNPD) == 0x00)
	{
		DisablePin(LED_BOARD);
		sleep_ms(500);
	}
	EnablePin(LED_BOARD);
	sleep_ms(500);

	WriteReg(EN_AA, 0x3F); // Enable Auto-ack on all pipes
	if(ReadReg(EN_AA) == 0x3F)
	{
		DisablePin(LED_BOARD);
		sleep_ms(500);
	}

	EnablePin(LED_BOARD);
	sleep_ms(500);

	WriteReg(EN_RXADDR, 0x03);//, SPI_init, init); //0 data pipes enabled
	if(ReadReg(EN_RXADDR) == 0x03)
	{
		DisablePin(LED_BOARD);
		sleep_ms(500);
	}
	EnablePin(LED_BOARD);
	sleep_ms(500);

	for(int i = 0; i < 6; i++)
	{
		WriteReg(RX_ADDR_P0 + i, 32);
	}
	WriteReg(FEATURE, 0);
	if(ReadReg(FEATURE) == 0x00)
	{
		DisablePin(LED_BOARD);
		sleep_ms(500);
	}
	EnablePin(LED_BOARD);
	sleep_ms(500);

	WriteReg(SETUP_AW, 0x03); 	//5 bytes for the tx/rx address
	if(ReadReg(SETUP_AW) == 0x03)
	{
		DisablePin(LED_BOARD);
		sleep_ms(500);
	}
	EnablePin(LED_BOARD);
	sleep_ms(500);


	WriteReg(RF_CH, 0x4c);	    //Channel will be setup in  during TX or RX
	if(ReadReg(RF_CH) == 0x4c)
	{
		DisablePin(LED_BOARD);
		sleep_ms(500);
	}
	EnablePin(LED_BOARD);
	sleep_ms(500);

	WriteReg(RF_SETUP, 0x06);	//Power 0dB, data rate = 2Mbps
	if(ReadReg(RF_SETUP) == 0x06)
	{
		DisablePin(LED_BOARD);
		sleep_ms(500);
	}

	WriteReg(STATUS, 0x70);
	SendCommand(FLUSH_RX);
	SendCommand(FLUSH_TX);
	WriteReg(CONFIG, 1 << EN_CRC | ~(1 << CRC0));
	sleep_ms(5);
}

void NRF24::EnablePin(
	uint8_t const& pin
)
{
	gpio_put(pin,1);
	sleep_us(5);
}

void NRF24::DisablePin(
	uint8_t const& pin
)
{
	gpio_put(pin, 0);
	sleep_us(5);
}
void NRF24::OpenWritingPipe(
	UINT8* address
)
{
	WriteRegMulti(RX_ADDR_P0, address, 5);
	WriteRegMulti(TX_ADDR, address, 5);
}
void NRF24::TxMode(
	UINT8* address, 
	UINT8 const& channel
)
{
	DisablePin(CE_PIN);

	//select the channel
	WriteReg(RF_CH, channel); 
	
	//write the tx address
	WriteRegMulti(TX_ADDR, address, 5);	
	//power up the device 
	status = ReadReg(CONFIG);
	status = status | (1 << PWR_UP);
	WriteReg(CONFIG, status);
	EnablePin(CE_PIN);
}	

void NRF24::SendCommand(
	UINT8 const& cmd
)
{
	UINT8 buf[1]{cmd};
	DisablePin(CSN_PIN);
	spi_write_read_blocking(NRF_SPI_PORT, buf, nullptr, 1);
	EnablePin(CSN_PIN);
}

bool NRF24::TransmitData(
	UINT8* data, 
	uint8_t const& dataSize
)
{
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
	DisablePin(CSN_PIN);
	spi_write_read_blocking(NRF_SPI_PORT, spiTx, spiRx, size);
	EnablePin(CSN_PIN);
	EnablePin(CE_PIN);
	uint32_t timer = millis();
	while(!(GetStatus() & ((1 << TX_DS) | (1 << MAX_RT))))
	{
		if(millis() - timer > 95)
		{
			return 0;
		}
	}
	DisablePin(CE_PIN);
	WriteReg(STATUS, 1 << RX_DR | 1 << TX_DS | 1 << MAX_RT);
	if(status & (1 << MAX_RT))
	{
		SendCommand(FLUSH_RX);
		return 0;
	}
	return 1;
}


void NRF24::RxMode(
	UINT8* address, 
	UINT8 const& channel
)
{
	DisablePin(CE_PIN);
	//select the channel
	WriteReg(RF_CH, channel);


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

	//select data pipe 1
	WriteReg(EN_RXADDR, status | (1 << 1)); 
	EnablePin(CE_PIN);

}	

void NRF24::Set2Rx(
	void
)
{
	status = ReadReg(CONFIG);
	if(status % 2 == 0)
	{
		WriteReg(CONFIG, status | (1 << 0));
		WriteReg(EN_RXADDR, ReadReg(EN_RXADDR) & ~(1 << 0));
		EnablePin(CE_PIN);
	}
}

void NRF24::Set2Tx(
	void
)
{
	status = ReadReg(CONFIG);
	if(status % 2)
	{
		DisablePin(CE_PIN);
		sleep_us(280);
		WriteReg(CONFIG, status & ~(1 << 0));
		WriteReg(EN_RXADDR, ReadReg(EN_RXADDR) | (1 << 0));
	}
}

UINT8 NRF24::IsDataAvailable(
	uint8_t const& pipeNr
)
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

bool NRF24::ReceiveData(
	UINT8* data
)
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
	DisablePin(CSN_PIN);
	spi_write_read_blocking(NRF_SPI_PORT, spiTx, spiRx, lenData);
	EnablePin(CSN_PIN);
	status = *rxData++;
	memcpy(data, rxData, sizeof(UINT8) * (lenData - 1));
	status = ReadReg(FIFO_STATUS);
	return !(status & 1); 
}




