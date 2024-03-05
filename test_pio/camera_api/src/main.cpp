#include "CameraOV2640.h"
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "hardware/spi.h"
#include "pico/binary_info.h"

#define LED_BOARD 16

int main()
{
    CameraOV2640 ov2640;
    uint8_t idH {};
    uint8_t idL {};
    ov2640.Init();
    gpio_init(LED_BOARD);
    gpio_set_dir(LED_BOARD, GPIO_OUT);
    gpio_put(LED_BOARD, 1);
    sleep_ms(500);
    ov2640.WriteRegSPI(0x00, 0x55);
    if (ov2640.ReadRegSPI(0x00) == 0x55)
    {
        printf("SPI bus normal\n");
        gpio_put(LED_BOARD, 0);
        sleep_ms(250);
    }
    gpio_put(LED_BOARD, 1);
    sleep_ms(500);
    ov2640.ReadRegI2c(0x0A, &idH);
    ov2640.ReadRegI2c(0x0B, &idL);

    if(idH == 0x26 && idL == 0x42)
    {
        printf("ov2640 detected\r\n");
        gpio_put(LED_BOARD, 0);
        sleep_ms(250);
    }
    else
    {
        printf("Can't find ov2640 sensor\r\n");
    }

    return 0;
}