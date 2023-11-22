#include "ShiftRegister595.hpp"

ShiftRegister595::ShiftRegister595(spi_inst_t *spi, uint latch_pin, uint clock_pin, uint data_pin)
    : spi_port(spi), gpio_latch(latch_pin), gpio_clock(clock_pin), gpio_data(data_pin)
{
    // Initialize SPI
    spi_init(spi_port, 1000 * 1000); // 1 MHz SPI speed
    gpio_set_function(gpio_clock, GPIO_FUNC_SPI);
    gpio_set_function(gpio_data, GPIO_FUNC_SPI);

    // Initialize latch pin
    gpio_init(gpio_latch);
    gpio_set_dir(gpio_latch, GPIO_OUT);
    gpio_put(gpio_latch, 1); // Start with latch high

    // Initialize shift register with all zeros
    clear();
}

void ShiftRegister595::startTransmission()
{
    asm volatile("nop \n nop \n nop"); // No operation for delay
    gpio_put(gpio_latch, 0);           // Set latch low before starting transmission
    asm volatile("nop \n nop \n nop"); // No operation for delay
}

void ShiftRegister595::endTransmission()
{
    asm volatile("nop \n nop \n nop"); // No operation for delay
    gpio_put(gpio_latch, 1);           // Set latch high after transmission
    asm volatile("nop \n nop \n nop"); // No operation for delay
}

void ShiftRegister595::send(uint8_t data)
{
    startTransmission();
    spi_write_blocking(spi_port, &data, 1);
    endTransmission();
}

void ShiftRegister595::clear()
{
    send(0x00); // Send a byte of zeros to clear the shift register
}
