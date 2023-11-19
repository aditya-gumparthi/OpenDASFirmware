#include "VoltMeterInputs.hpp"

VoltMeterInputs::VoltMeterInputs(const VoltMeterInputConfig& config) : config(config) {
    // Initialize all pins to 0
    initPin(config.inn_msel_0);
    initPin(config.inn_msel_1);
    initPin(config.inn_msel_2);
    initPin(config.inn_msel_3);
    initPin(config.inp_msel_0);
    initPin(config.inp_msel_1);
    initPin(config.inp_msel_2);
    initPin(config.inp_msel_3);
    initPin(config.in_rel_sel);
}

void VoltMeterInputs::setInputAttenuation(InputAttenuation attenuation) {
    gpio_put(config.in_rel_sel, attenuation == DIV_5 ? 1 : 0);
}

void VoltMeterInputs::setInn(InputSelection selection) {
    clearInnPins();
    switch (selection) {
        case INPUT_VIN:
            gpio_put(config.inn_msel_0, 1);
            break;
        case CAL_SHORT:
            gpio_put(config.inn_msel_1, 1);
            break;
        case CAL_REF_MIDSCALE:
            gpio_put(config.inn_msel_2, 1);
            break;
        case CAL_REF_FULLSCALE:
            gpio_put(config.inn_msel_3, 1);
            break;
    }
}

void VoltMeterInputs::setInp(InputSelection selection) {
    clearInpPins();
    switch (selection) {
        case INPUT_VIN:
            gpio_put(config.inp_msel_0, 1);
            break;
        case CAL_SHORT:
            gpio_put(config.inp_msel_1, 1);
            break;
        case CAL_REF_MIDSCALE:
            gpio_put(config.inp_msel_2, 1);
            break;
        case CAL_REF_FULLSCALE:
            gpio_put(config.inp_msel_3, 1);
            break;
    }
}

void VoltMeterInputs::clearInnPins() {
    gpio_put(config.inn_msel_0, 0);
    gpio_put(config.inn_msel_1, 0);
    gpio_put(config.inn_msel_2, 0);
    gpio_put(config.inn_msel_3, 0);
}

void VoltMeterInputs::clearInpPins() {
    gpio_put(config.inp_msel_0, 0);
    gpio_put(config.inp_msel_1, 0);
    gpio_put(config.inp_msel_2, 0);
    gpio_put(config.inp_msel_3, 0);
}

void VoltMeterInputs::initPin(uint pin) {
    gpio_init(pin);
    gpio_set_dir(pin, GPIO_OUT);
    gpio_put(pin, 0);
}
