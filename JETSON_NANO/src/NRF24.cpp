#include"NRF24.h"
#include "nrf24l01.h"
#include <chrono>
auto start = std::chrono::steady_clock::now();
uint32_t millis()
{
	auto end = std::chrono::steady_clock::now();
	return std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
}
//write a single byte
NRF24::~NRF24(
	void
)
{
	DisablePin(CE_PIN);
	WriteReg(CONFIG, 0x00);
	spiClose(SPI_init_);
	DisablePin(CSN_PIN);
	gpioTerminate();
}
void NRF24::WriteReg (
	uint8_t const& reg, 
	uint8_t const& data
)
{
	uint8_t buf[2];
	buf[0] = W_REGISTER | reg;//|1<<5;
	buf[1] = data;	
	//pentru a selecta
	if(verify_)
	{
		DisablePin(CSN_PIN);
		verify_ = spiXfer(SPI_init_, buf, spiRx, 2);
		EnablePin(CSN_PIN);
	}
	status = spiRx[0];
	if(verify_ < 0)
	{
		std::cout << "SPI port transfer failed. Error code: " << verify_ << "\n";
		exit(init_);
	}
}
//write multiple datas
void NRF24::WriteRegMulti(
	uint8_t const& reg, 
	uint8_t* data, 
	uint8_t size
)
{

	uint8_t* rxData = spiRx;
	uint8_t* txData = spiTx;
	uint8_t lenData = size + 1; //32 bytes data + 1 byte command
	
	*txData++ = W_REGISTER | (REGISTER_MASK & reg);

	while (size--)
	{
		*txData++ = *data++;
	}
	DisablePin(CSN_PIN);
	verify_ = spiXfer(SPI_init_, spiTx, spiRx, lenData);
	EnablePin(CSN_PIN);
	status = *rxData;
}
//read 1 byte
uint8_t NRF24::ReadReg (
	uint8_t const& reg
)
{
	uint8_t cmd[2]{};
	cmd[0] = R_REGISTER | reg;
	cmd[1] = NOP;
	//pentru a selecta dispozitivul
	gpioWrite(CSN_PIN,0);
	verify_ = spiXfer(SPI_init_, cmd, spiRx, 2);
	gpioWrite(CSN_PIN,1);
	status=spiRx[0];
	return spiRx[1];
}
void NRF24::ReadMulti(
	uint8_t const& reg, 
	uint8_t* data, 
	uint8_t size
)
{	
	//pentru a selecta dispozitivul
	uint8_t* txData = spiTx;
	uint8_t* rxData = spiRx;
	*txData = R_REGISTER | reg;
	for(uint8_t i{0}; i < size; i++)
	{
		*txData++=NOP;
	}
	DisablePin(CSN_PIN);
	verify_ = spiXfer(SPI_init_, txData, rxData, size + 1);
	EnablePin(CSN_PIN);
	status = *rxData++;
	memcpy(data, rxData, sizeof(uint8_t) * size);
}

NRF24::NRF24(
	void
)
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

	SPI_init_ = spiOpen(0, 10'00'000, 0, 0, 8, 0, 0);

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

	DisablePin(CE_PIN);
	EnablePin(CSN_PIN);
	usleep(5'000);

	
	WriteReg(SETUP_RETR, 0x1F);	//Retransmision(5, 15)

	WriteReg(DYNPD, 0x00);	//disable dynamic payload

	WriteReg(EN_AA, 0x3F); // Enable Auto-ack on all pipes

	WriteReg(EN_RXADDR, 0x03);//, SPI_init, init); //0 data pipes enabled

	for(int i = 0; i < 6; i++)
	{
		WriteReg(RX_ADDR_P0 + i, 32);
	}
	WriteReg(FEATURE, 0x00); //enable ack_payload and dynamic_payload

	WriteReg(SETUP_AW, 0x03); 	//5 bytes for the tx/rx address

	WriteReg(RF_CH, 0x4c);	    //Channel will be setup in  during TX or RX

	WriteReg(RF_SETUP, 0x06);	//Power 0dB, data rate = 1Mbps
	
	WriteReg(STATUS, 0x70);
	SendCommand(FLUSH_RX);
	SendCommand(FLUSH_TX);

	WriteReg(CONFIG, 1 << EN_CRC | ~(1 << CRC0));

	usleep(5000);
}

void NRF24::EnablePin(
	uint8_t const& pin
)
{
	gpioWrite(pin, 1);
	usleep(5);
}

void NRF24::DisablePin(
	uint8_t const& pin
)
{
	gpioWrite(pin,0);
	usleep(5);
}
void NRF24::OpenWritingPipe(
	uint8_t* address
)
{
	WriteRegMulti(RX_ADDR_P0, address, 5);
	WriteRegMulti(TX_ADDR, address, 5);
}
void NRF24::TxMode(
	uint8_t* address, 
	uint8_t const& channel
)
{
	DisablePin(CE_PIN);
	//select the channel
	WriteReg(RF_CH, channel); 
	
	//write the tx address
	WriteRegMulti(RX_ADDR_P0, address, 5);
	WriteRegMulti(TX_ADDR, address, 5);
	
	//power up the device 
	uint8_t config = ReadReg(CONFIG);
	config = config | (1 << 1);
	WriteReg(CONFIG, config);
	EnablePin(CE_PIN);
}	

void NRF24::SendCommand(
	uint8_t const& cmd
)
{
	uint8_t buf[1]{cmd};
	DisablePin(CSN_PIN);
	verify_ = spiXfer(SPI_init_, buf, nullptr, 1);
	EnablePin(CSN_PIN);
}

bool NRF24::TransmitData(
	uint8_t* data
)
{
	Set2Tx();
	DisablePin(CE_PIN);
	uint8_t* txData = spiTx;
	uint8_t* rxData = spiRx;
	uint8_t const* current = data;
	uint8_t size = 32 + 1;
	*txData++ = W_TX_PAYLOAD;
	while(--size)
	{
		*txData++ = *current++;
	}
	size = 33;
	DisablePin(CSN_PIN);
	if(verify_)
	{	
		verify_ = spiXfer(SPI_init_, spiTx, spiRx, sizeof(uint8_t) * size);
	}
	EnablePin(CSN_PIN);
	EnablePin(CE_PIN);
	uint32_t timer = millis();
	while (!(GetStatus() & ((1 << TX_DS) | (1 << MAX_RT))))
	{
		if(millis() - timer > 95)
		{
			return false;
		}
	}
	
	WriteReg(STATUS, 1 << RX_DR | 1 << TX_DS | 1 << MAX_RT);
	if(status & (1 << MAX_RT))
	{
		SendCommand(FLUSH_TX);
		return false;
	}
	return true;
}

uint8_t NRF24::GetStatus(
	void
)
{
	WriteReg(NOP, NOP);
	return status;
}

void NRF24::RxMode(
	uint8_t* address, 
	uint8_t const& channel
)
{
	DisablePin(CE_PIN);
	//select the channel
	WriteReg(RF_CH, channel);

	WriteRegMulti(RX_ADDR_P1, address, 5);

	//setting the payload to maximum 32 bits
	WriteReg(RX_PW_P1, 32);

	//power up the device in RX mode
	status = ReadReg(CONFIG);
	status = status | (1 << PWR_UP) | (1 << 0);
	WriteReg(CONFIG, status);
	WriteReg(STATUS, 0x70);
	status = ReadReg(EN_RXADDR);

	WriteReg(EN_RXADDR, status | (1 << 1)); 
	EnablePin(CE_PIN);
}	

void NRF24::Set2Rx(
	void
)
{
	status = ReadReg(CONFIG);
	if (status % 2 == 0)
	{
		DisablePin(CE_PIN);
		WriteReg(CONFIG, status | (1 << 0));
		usleep(280);
		EnablePin(CE_PIN);
	}
}

void NRF24::Set2Tx(
	void
)
{
	status = ReadReg(CONFIG);
	if (status % 2)
	{
		DisablePin(CE_PIN);
		WriteReg(CONFIG, status & ~(1 << 0));
		usleep(280);
		EnablePin(CE_PIN);
		receive = true;
	}
}

uint8_t NRF24::IsDataAvailable(uint8_t const& pipeNr)
{
	Set2Rx();
	status = ReadReg(STATUS);
	if ((status & (1 << RX_DR)) && 
	    (status & (pipeNr << 1)))
	{
		resetRxIrq=true;
		ResetRxIrq();
		return 1;
	}
	return 0;
}
void NRF24::StopTransferring(
	void
)
{
	DisablePin(CE_PIN);
}
void NRF24::StartTransferring(
	void
)
{
	EnablePin(CE_PIN);
}
bool NRF24::ReceiveData(
	uint8_t* data, 
	uint8_t const& lenData, 
	bool& startFrame
)
{
	uint8_t fifoStatus{};
	fifoStatus = ReadReg(FIFO_STATUS);
	if(!(fifoStatus &(1 << 0)))
	{
		uint8_t* txData = spiTx;
		uint8_t* rxData = spiRx;

		int size = 33;
		*txData++ = R_RX_PAYLOAD;
		while (--size)
		{
			*txData++ = NOP;
		}
		size = 32 + 1;

		DisablePin(CSN_PIN);
		verify_ = spiXfer(SPI_init_, spiTx, spiRx, size);
		EnablePin(CSN_PIN);
		status = *rxData++;
		if(*rxData == 0x0a && *(rxData+1)==0x0d)
		{
			startFrame = true;
		}
		memcpy(data, rxData,sizeof(uint8_t)*lenData);
	}		
	return!(fifoStatus & (1 << 0));	
}
void NRF24::ResetRxIrq(
	void
)
{
    if(resetRxIrq)
	{
		resetRxIrq = false;       
    	WriteReg(STATUS, (1 << RX_DR)|( 1 << TX_DS )|(1 << MAX_RT) );  
 	}
} 



