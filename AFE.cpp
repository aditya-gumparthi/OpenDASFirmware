// AFE.cpp
#include "AFE.hpp"

// Constructor implementation
AFE::AFE(const AFEConfig& config)
    : gain_sr(config.spi, config.gain_latch_pin, config.gain_clock_pin, config.gain_data_pin),
      cal_sr(config.spi, config.cal_latch_pin, config.cal_clock_pin, config.cal_data_pin) {}

// set_fda_gain method implementation
void AFE::set_fda_gain(fda_gain_t gain) {
    uint8_t gainValue = static_cast<uint8_t>(gain) << 1; // Shift left to align with bits 1 to 3
    gain_sr.send(gainValue);
}

// Other methods implementation...
