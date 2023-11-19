#include <stdio.h>
#include <pico/stdlib.h>
#include <hardware/spi.h>
#include "ShiftRegister595.hpp"

ShiftRegister595 hw_cal_sr(spi_default, 25, 2, 3);
ShiftRegister595 hw_gain_sr(spi_default, 24, 2, 3);


int main()
{
    stdio_init_all();
    spi_init(spi_default, 1000e3);
    gpio_set_function(0, GPIO_FUNC_SPI);
    // gpio_set_function(2, GPIO_FUNC_SPI);
    // gpio_set_function(3, GPIO_FUNC_SPI);

    // gpio_init(25);
    // gpio_init(24);

    // gpio_set_dir(25, GPIO_OUT);
    // gpio_set_dir(24, GPIO_OUT);

    // gpio_put(25, 1);
    // gpio_put(24, 1);

    // gpio_put(25, 0);
    // gpio_put(24, 0);


    
    // spi_write_blocking(spi_default, 0x00, 1);
    // gpio_put(25, 1);
    // gpio_put(24, 1);


    while (1)
    {
        printf("Hello, world!\n");
        sleep_ms(1000);
    }
    return 0;
}