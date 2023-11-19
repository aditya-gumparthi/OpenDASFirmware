#ifndef VOLTMETERINPUTS_HPP
#define VOLTMETERINPUTS_HPP

#include "pico/stdlib.h"

// Define the configuration struct
struct VoltMeterInputConfig {
    uint inn_msel_0;
    uint inn_msel_1;
    uint inn_msel_2;
    uint inn_msel_3;
    uint inp_msel_0;
    uint inp_msel_1;
    uint inp_msel_2;
    uint inp_msel_3;
    uint in_rel_sel;  // Added new pin
};

class VoltMeterInputs {
public:
    VoltMeterInputs(const VoltMeterInputConfig& config);

private:
    void initPin(uint pin);
};

#endif // VOLTMETERINPUTS_HPP
