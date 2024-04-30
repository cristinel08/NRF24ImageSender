#ifndef INC_RF24_H_
#define INC_RF24_H_


/* Memory Map */
#define CONFIG      0x00
#define EN_AA       0x01
#define EN_RXADDR   0x02
#define SETUP_AW    0x03
#define SETUP_RETR  0x04
#define RF_CH       0x05
#define RF_SETUP    0x06
#define STATUS      0x07
#define OBSERVE_TX  0x08
#define CD          0x09
#define RX_ADDR_P0  0x0A
#define RX_ADDR_P1  0x0B
#define RX_ADDR_P2  0x0C
#define RX_ADDR_P3  0x0D
#define RX_ADDR_P4  0x0E
#define RX_ADDR_P5  0x0F
#define TX_ADDR     0x10
#define RX_PW_P0    0x11
#define RX_PW_P1    0x12
#define RX_PW_P2    0x13
#define RX_PW_P3    0x14
#define RX_PW_P4    0x15
#define RX_PW_P5    0x16
#define FIFO_STATUS 0x17
#define DYNPD	    0x1C
#define FEATURE	    0x1D

/* Instruction Mnemonics */
#define R_REGISTER    0x00
#define W_REGISTER    0x20
#define REGISTER_MASK 0x1F
#define W_ACK_PAYLOAD 0xA8
#define ACTIVATE      0x50
#define R_RX_PL_WID   0x60
#define R_RX_PAYLOAD  0x61
#define W_TX_PAYLOAD  0xA0
#define W_ACK_PAYLOAD 0xA8
#define FLUSH_TX      0xE1
#define FLUSH_RX      0xE2
#define REUSE_TX_PL   0xE3
#define NOP           0xFF

#define ARD 		4
#define RX_DR 		6
#define TX_DS		5
#define MAX_RT 		4
#define EN_CRC		3
#define CRC0		2
#define PWR_UP		1

#ifndef JETSON_BOARD
#define CE_PIN 		15
#define CSN_PIN 	24
#else
#define NRF_SPI_PORT  spi0
#define SCK_PIN       2
#define MOSI_PIN      3  
#define MISO_PIN      4
#define CE_PIN        7
#define CSN_PIN       8
#endif

#define RX_DWN        0xFD
#define TX_DWN        0xFC


#endif /* INC_RF24_H_ */