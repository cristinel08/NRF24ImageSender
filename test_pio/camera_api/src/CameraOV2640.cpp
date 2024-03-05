#include "CameraOV2640.h"

CameraOV2640::CameraOV2640()
{

}
void CameraOV2640::WriteRegSPI( uint8_t addr, uint8_t data )
{
  buf_[0] = addr | WRITE_BIT; //MSB = 1 for writting
  buf_[1] = data;
  CsSelect();
  spi_write_blocking(SPI_PORT, buf_, 2);
  CsUnselect();


}
void CameraOV2640::on_uart_rx(void* userData)
 {
    CameraOV2640* camObj = (CameraOV2640*) userData;
    while (uart_is_readable(UART_ID)) 
    {
       camObj->cameraCommand = uart_getc(UART_ID);
       // Can we send it back?
       if (uart_is_writable(UART_ID))
       {
            uart_putc(UART_ID, camObj->cameraCommand);
       }
    }
}
void CameraOV2640::CsSelect()
{
    gpio_put(PIN_CS, 0);
}

void CameraOV2640::CsUnselect()
{
    gpio_put(PIN_CS, 1);
}

uint8_t CameraOV2640::ReadRegSPI ( uint8_t addr ) 
{
    addr = addr & (~WRITE_BIT); //MSB = 0 for reading
    CsSelect();
    spi_write_blocking(SPI_PORT, &addr, 1);
    sleep_ms(10);
    spi_read_blocking(SPI_PORT, 0, &readSPI_, 1);
    CsUnselect();
    sleep_ms(10);
    return readSPI_;
}

void CameraOV2640::WriteRegI2c ( uint8_t regID, uint8_t regDat )
{
    buf_[0] = regID;
    buf_[1] = regDat;
    i2c_write_blocking ( I2C_PORT, cameraSensorAddr_, buf_, 2, true );
}



void CameraOV2640::ReadRegI2c ( uint8_t regID, uint8_t* regDat )
{
    i2c_write_blocking ( I2C_PORT, cameraSensorAddr_, &regID, 1, true );
    i2c_read_blocking ( I2C_PORT, cameraSensorAddr_, regDat, 1, false );
}


void CameraOV2640::Init()
{
    stdio_init_all();

    i2c_init(I2C_PORT, 100 * 1000);
    gpio_set_function(PIN_SDA, GPIO_FUNC_I2C);
    gpio_set_function(PIN_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(PIN_SDA);
    gpio_pull_up(PIN_SCL);

    bi_decl( bi_2pins_with_func(PIN_SDA, PIN_SCL, GPIO_FUNC_I2C));

    spi_init(SPI_PORT, 8 * 1000*1000);
    gpio_set_function(PIN_MISO, GPIO_FUNC_SPI);
    gpio_set_function(PIN_SCK, GPIO_FUNC_SPI);
    gpio_set_function(PIN_MOSI, GPIO_FUNC_SPI);

    gpio_init(PIN_CS);
    gpio_set_dir(PIN_CS, GPIO_OUT);
    gpio_put(PIN_CS, 1);

    uart_init(UART_ID, BAUD_RATE);
    // Set the TX and RX pins by using the function select on the GPIO
    // Set datasheet for more information on function select
    gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);
    // Set our data format
    uart_set_format(UART_ID, DATA_BITS, STOP_BITS, PARITY);
    // Turn off FIFO's - we want to do this character by character
    uart_set_fifo_enabled(UART_ID, false);
     // Set up a RX interrupt
    // We need to set up the handler first
    // Select correct interrupt for the UART we are using
    // int UART_IRQ = UART_ID == uart0 ? UART0_IRQ : UART1_IRQ;

    // // And set up and enable the interrupt handlers
    // irq_set_exclusive_handler(UART_IRQ, on_uart_rx);
    // irq_set_enabled(UART_IRQ, true);

    // // Now enable the UART to send interrupts - RX only
    // uart_set_irq_enables(UART_ID, true, false);
}

