// AFE.hpp
#ifndef AFE_HPP
#define AFE_HPP

#include "ShiftRegister595.hpp"
#include "hardware/spi.h"

class AFE {
public:
    // FDA gain enumeration
    enum class fda_gain_t {
        GAIN_DIV_8 = 0,
        GAIN_DIV_4 = 1,
        GAIN_DIV_2 = 2,
        GAIN_1 = 3,
        GAIN_2 = 4,
        GAIN_4 = 5,
        GAIN_8 = 6,
        GAIN_16 = 7
    };

    // Input gain enumeration
    enum class input_gain_t {
        GAIN_1 = 0b01,
        GAIN_10 = 0b10
    };

    // Attenuation enumeration
    enum class attenuation_t {
        DIV_1 = 0b01,
        DIV_10 = 0b10
    };

    // Configuration struct for AFE
    struct AFEConfig {
        spi_inst_t *spi;
        uint gain_latch_pin;
        uint gain_clock_pin;
        uint gain_data_pin;
        uint cal_latch_pin;
        uint cal_clock_pin;
        uint cal_data_pin;
    };

    // Constructor
    AFE(const AFEConfig& config);

    // Set methods for gain_sr
    void set_fda_gain(fda_gain_t gain);
    void set_input_gain(input_gain_t gain);
    void set_attenuation(attenuation_t attenuation);

    // Set methods for cal_sr
    void connect_negative_input_buffer_to_inn();
    void connect_ref_mid_scale_to_inp();
    void connect_ref_full_scale_to_inp();
    void connect_ref_mid_scale_to_inn();
    void connect_ref_full_scale_to_inn();
    void short_inn_inp();

private:
    ShiftRegister595 gain_sr;
    ShiftRegister595 cal_sr;
    uint8_t gain_sr_state; // To maintain the state of gain_sr
    uint8_t cal_sr_state;  // To maintain the state of cal_sr
};

#endif // AFE_HPP
