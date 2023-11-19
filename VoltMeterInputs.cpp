#include "VoltMeterInputs.hpp"

VoltMeterInputs::VoltMeterInputs(const VoltMeterInputConfig& config) {
    // Initialize the pins to 0
    initPin(config.inn_msel_0);
    initPin(config.inn_msel_1);
    initPin(config.inn_msel_2);
    initPin(config.inn_msel_3);
    initPin(config.inp_msel_0);
    initPin(config.inp_msel_1);
    initPin(config.inp_msel_2);
    initPin(config.inp_msel_3);
    initPin(config.in_rel_sel);  // Initialize the new pin
}

void VoltMeterInputs::initPin(uint pin) {
    gpio_init(pin);
    gpio_set_dir(pin, GPIO_OUT);
    gpio_put(pin, 0);
}
