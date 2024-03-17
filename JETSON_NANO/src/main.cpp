#include<RF24.h>
#include<iostream>
#include<time.h>
#include<string>
#include<ctime>

// char ReadReg (char reg, int SPI_init_);
// void WriteReg (char reg, char data, int SPI_init_);
// //using namespace GPIO; // optional
// int main()
// {
//     int init_ = gpioInitialise();
//     if(init_ < 0)
// 	{
// 		std::cout << "Problema initializare gpio cu " << init_ << "\n";
// 		exit(init_);
//         return -1;
// 	}
// 	int verify_ = gpioSetMode(CE_PIN, JET_OUTPUT);
// 	if(verify_ < 0)
// 	{
// 		std::cout << "Nu a mers setarea pinului CE, codul: " << verify_ << "\n";
// 		exit(init_);
//         return -2;
// 	}
// 	verify_ = gpioSetMode(CSN_PIN, JET_OUTPUT);
//     if(verify_ < 0)
// 	{
// 		std::cout << "Nu a mers setarea pinului CSN, codul: " << verify_ << "\n";
// 		exit(init_);
// 	}
// 	else
// 	{
// 		std::cout << "A mers setarea pinului CSN. Codul: " << verify_ << "\n";
// 	}

// 	int SPI_init_ = spiOpen(0, 10000000, 0, 5, 8, 0, 0);

// 	if (SPI_init_ < 0)
// 	{
// 			/* Port SPI2 opening failed */
// 		printf("Port SPI1 opening failed. Error code:  %d\n", SPI_init_);
// 		exit(init_);
//         return -3;
// 	}
// 	usleep(5);
// 	// WriteReg(SETUP_RETR, 5 << ARD | 15, SPI_init_);
//     WriteReg(CONFIG, 0x02, SPI_init_);
// 	char readChannel = 0x0F;
// 	// WriteReg(RF_SETUP, readChannel, SPI_init_);
// 	readChannel = ReadReg(CONFIG, SPI_init_);
//     return 0;
// }


// void WriteReg (char reg, char data, int SPI_init_)
// {
// 	int status{};
// 	char buf[2]{};
// 	char spiRx[32]{};
// 	buf[0] = W_REGISTER | reg;//|1<<5;
// 	buf[1] = data;
// 	// char cmd = W_REGISTER;
	
// 	//pentru a selecta
// 	int verify_ = gpioWrite(CSN_PIN,0);
// 	verify_ = gpioWrite(CE_PIN, 1);
// 	// verify_ = spiXfer(SPI_init_, &cmd, nullptr, 1);
// 	// usleep(10);
// 	if(verify_)
// 	{
// 		verify_ = spiXfer(SPI_init_, buf, spiRx, 2);
// 		// usleep(10);
// 	}
// 	status = (int)spiRx[0];
// 	gpioWrite(CSN_PIN,1);
// }

// char ReadReg (char reg, int SPI_init_)
// {
// 	char data='0';
// 	int status{};
// 	char spiRx[32]{};
// 	char cmd[2]{};
// 	cmd[0] = R_REGISTER | reg;
// 	cmd[1] = NOP;
// 	//pentru a selecta dispozitivul
// 	gpioWrite(CSN_PIN,0);
// 	int verify_ = spiXfer(SPI_init_, cmd, spiRx, 2);
// 	status = (int)spiRx[0];
// 	data = spiRx[1];
// 	gpioWrite(CSN_PIN,1);
// 	return data;
// }

int main()
{
	char dataTx[32]{"Buna Cristi am trimis de pe Jet"};
	char txAddress[]{"1Node"};
	char rxAddress[]{"2Node"};
	char data[32]{"a,b,c,d,e,f,g,h,i,j,k,l,m,n,p,q"};

	NRF24 nrf24 = NRF24();
	
	// nrf24.RxMode(rxAddress, 76);
	nrf24.TxMode(txAddress, 76);
	bool dataTransmited{true};
	// nrf24.OpenWritingPipe(txAddress);
	while(1)
	{
		if(dataTransmited)
		{
			nrf24.TransmitData(data);
		}
		else
		{
			nrf24.TransmitData(dataTx);
		}
		dataTransmited = !dataTransmited;
		sleep(1);
	}
}

