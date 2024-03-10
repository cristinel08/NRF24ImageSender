/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>

#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "blink.pio.h"

// Write `period` to the input shift register
void pio_pwm_set_period(PIO pio, uint sm, uint32_t period) {
    pio_sm_set_enabled(pio, sm, false);
    pio_sm_put_blocking(pio, sm, period);
    pio_sm_exec(pio, sm, pio_encode_pull(false, false));
    pio_sm_exec(pio, sm, pio_encode_out(pio_isr, 32));
    pio_sm_set_enabled(pio, sm, true);
}

// Write `level` to TX FIFO. State machine will copy this into X.
void pio_pwm_set_level(PIO pio, uint sm, uint32_t level) {
    pio_sm_put_blocking(pio, sm, level);
}

int main() {
    stdio_init_all();
#ifndef PICO_DEFAULT_LED_PIN
#warning pio/pwm example requires a board with a regular LED
    puts("Default LED pin was not defined");
#else

    // todo get free sm
    PIO pio = pio0;
    int sm[3] = {0, 1, 2};
    uint offset = pio_add_program(pio, &pwm_program);
    printf("Loaded program at %d\n", offset);

    pwm_program_init(pio, sm[0], offset, PICO_DEFAULT_LED_PIN);
    pwm_program_init(pio, sm[1], offset, 16);
    pwm_program_init(pio, sm[2], offset, 17);
    for (int i = 0; i < 3; i++)
    {
        pio_pwm_set_period(pio, sm[i], (1u << 16) - 1);
    }

    // pio_pwm_set_period(pio, 1, (1u << 16) - 1);
    // pio_pwm_set_period(pio, 2, (1u << 16) - 1);
    int level = 0;
    while (true) {
        printf("Level = %d\n", level);
        for (int i = 0; i < 3; i++)
        {
            pio_pwm_set_level(pio, sm[i], level * level);
        }
 
        // pio_pwm_set_level(pio, 1, level * level);
        // pio_pwm_set_level(pio, 2, level * level);
        level = (level + 1) % 256;
        sleep_ms(10);
    }
#endif
}
