// AFE.hpp
#ifndef AFE_HPP
#define AFE_HPP

#include "ShiftRegister595.hpp"
#include "hardware/spi.h"

class AFE
{
public:
    // Define the FDA gain enumeration
    enum class fda_gain_t
    {
        GAIN_DIV_8 = 0, // Previously GAIN_1_8
        GAIN_DIV_4 = 1, // Previously GAIN_1_4
        GAIN_DIV_2 = 2, // Previously GAIN_1_2
        GAIN_1 = 3,
        GAIN_2 = 4,
        GAIN_4 = 5,
        GAIN_8 = 6,
        GAIN_16 = 7
    };

    // Configuration struct for AFE
    struct AFEConfig
    {
        spi_inst_t *spi;
        uint gain_latch_pin;
        uint gain_clock_pin;
        uint gain_data_pin;
        uint cal_latch_pin;
        uint cal_clock_pin;
        uint cal_data_pin;
    };

    // Constructor
    AFE(const AFEConfig &config);

    // Method to set FDA gain
    void set_fda_gain(fda_gain_t gain);

    // Other methods...
private:
    ShiftRegister595 gain_sr;
    ShiftRegister595 cal_sr;
};

#endif // AFE_HPP
