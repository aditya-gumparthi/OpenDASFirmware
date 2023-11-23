#ifndef VOLTMETERINPUTS_HPP
#define VOLTMETERINPUTS_HPP

#include "pico/stdlib.h"
#include "hardware/spi.h"

// Define the configuration struct
struct VoltMeterInputConfig
{
    uint inn_msel_0;
    uint inn_msel_1;
    uint inn_msel_2;
    uint inn_msel_3;
    uint inp_msel_0;
    uint inp_msel_1;
    uint inp_msel_2;
    uint inp_msel_3;
    uint in_rel_sel; // Relay pin
};

class VoltMeterInputs
{
public:
    // Enumerations for input attenuation
    enum InputAttenuation
    {
        DIV_1,
        DIV_5
    };

    // Enumerations for input selection
    enum InputSelection
    {
        INPUT_VIN,
        CAL_SHORT,
        CAL_REF_MIDSCALE,
        CAL_REF_FULLSCALE
    };

    VoltMeterInputs(const VoltMeterInputConfig &config);
    void setInputAttenuation(InputAttenuation attenuation);
    void setInn(InputSelection selection);
    void setInp(InputSelection selection);

private:
    VoltMeterInputConfig config;
    void initPin(uint pin);
    void clearInnPins();
    void clearInpPins();
};

#endif // VOLTMETERINPUTS_HPP
