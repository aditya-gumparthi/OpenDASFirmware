#ifndef SHIFTREGISTER595_HPP
#define SHIFTREGISTER595_HPP

#include "pico/stdlib.h"
#include <stdio.h>

#include "hardware/spi.h"

class ShiftRegister595
{
private:
    spi_inst_t *const spi_port; // Pointer to SPI instance
    uint gpio_latch;
    uint gpio_clock;
    uint gpio_data;

    void startTransmission();
    void endTransmission();

public:
    ShiftRegister595(spi_inst_t *spi, uint latch_pin, uint clock_pin, uint data_pin);
    void send(uint8_t data);
    void clear();
};

#endif // SHIFTREGISTER595_HPP
