/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdlib.h>
#include <stdio.h>

#include "pico/stdlib.h"
#include "pio_spi.h"

// This program instantiates a PIO SPI with each of the four possible
// CPOL/CPHA combinations, with the serial input and output pin mapped to the
// same GPIO. Any data written into the state machine's TX FIFO should then be
// serialised, deserialised, and reappear in the state machine's RX FIFO.

#define SPI_PORT spi1
#define PIN_SCK  14
#define PIN_MOSI 11
#define PIN_MISO 12
#define PIN_CS   13

#define BUF_SIZE 20

void test(const pio_spi_inst_t *spi) {
    static uint8_t txbuf[BUF_SIZE];
    static uint8_t rxbuf[BUF_SIZE];
    printf("TX:");
    for (int i = 0; i < BUF_SIZE; ++i) {
        txbuf[i] = rand() >> 16;
        rxbuf[i] = 0;
        printf(" %02x", (int) txbuf[i]);
    }
    printf("\n");

    pio_spi_write8_read8_blocking(spi, txbuf, rxbuf, BUF_SIZE);

    printf("RX:");
    bool mismatch = false;
    for (int i = 0; i < BUF_SIZE; ++i) {
        printf(" %02x", (int) rxbuf[i]);
        mismatch = mismatch || rxbuf[i] != txbuf[i];
    }
    if (mismatch)
        printf("\nNope\n");
    else
        printf("\nOK\n");
}

void writeSPI(const pio_spi_inst_t *spi, uint8_t reg, uint8_t data)
{
    uint8_t tx[2];
    tx[0] = reg | 0x80;
    tx[1] = data;
    gpio_put(PIN_CS, 0);
    pio_spi_write8_blocking(spi, tx, 2);
    gpio_put(PIN_CS, 1);
}

void readSPI(const pio_spi_inst_t* spi, uint8_t reg, uint8_t* data)
{
    reg = reg & (~0x80);
    gpio_put(PIN_CS, 0);
    pio_spi_write8_blocking(spi, &reg, 1);
    pio_spi_read8_blocking(spi, data, 1);
    gpio_put(PIN_CS, 1);
}


int main() {
    stdio_init_all();
    gpio_init(25);
    gpio_set_dir(25, GPIO_OUT);
    gpio_put(25, 1);
    sleep_ms(100);

    pio_spi_inst_t spi = {
            .pio = pio0,
            .sm = 0
    };
    float clkdiv = 31.25f;  // 1 MHz @ 125 clk_sys
    uint cpha0_prog_offs = pio_add_program(spi.pio, &spi_cpha0_program);
    uint cpha1_prog_offs = pio_add_program(spi.pio, &spi_cpha1_program);

    gpio_init(PIN_CS);
    gpio_set_dir(PIN_CS, GPIO_OUT);
    gpio_put(PIN_CS, 1);
    uint8_t data = 0;
    // for (int cpha = 0; cpha <= 1; ++cpha) {
        // for (int cpol = 0; cpol <= 1; ++cpol) {
            // printf("CPHA = %d, CPOL = %d\n", cpha, cpol);
    pio_spi_init(spi.pio, spi.sm,
                    cpha0_prog_offs,
                    8,       // 8 bits per SPI frame
                    clkdiv,
                    0,
                    0,
                    PIN_SCK,
                    PIN_MOSI,
                    PIN_MISO
    );

    writeSPI(&spi, 0x00, 0x58);
    sleep_ms(10);
    readSPI(&spi, 0x00, &data);
    if (data == 0x58)
    {
        gpio_put(25, 0);
        sleep_ms(500);

    }
            // test(&spi);
            // sleep_ms(10);
        // }
    // }
}
