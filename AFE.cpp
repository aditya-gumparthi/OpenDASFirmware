#include "AFE.hpp"

// Constructor implementation
AFE::AFE(const AFEConfig &config)
    : gain_sr(config.spi, config.gain_latch_pin, config.gain_clock_pin, config.gain_data_pin),
      cal_sr(config.spi, config.cal_latch_pin, config.cal_clock_pin, config.cal_data_pin),
      gain_sr_state(0),
      cal_sr_state(0) {}

void AFE::set_fda_gain(fda_gain_t gain)
{
    gain_sr_state &= ~0x0E;                             // Clear bits 1-3
    gain_sr_state |= (static_cast<uint8_t>(gain) << 1); // Set new gain
    gain_sr.send(gain_sr_state);
}

void AFE::set_input_gain(input_gain_t gain)
{
    gain_sr_state &= ~0x30;                             // Clear bits 4-5
    gain_sr_state |= (static_cast<uint8_t>(gain) << 4); // Set new input gain
    gain_sr.send(gain_sr_state);
}

void AFE::set_attenuation(attenuation_t attenuation)
{
    gain_sr_state &= ~0xC0;                                    // Clear bits 6-7
    gain_sr_state |= (static_cast<uint8_t>(attenuation) << 6); // Set new attenuation
    gain_sr.send(gain_sr_state);
}

void AFE::connect_negative_input_buffer_to_inn()
{
    cal_sr_state &= ~0x40; // Clear the second last bit
    cal_sr_state |= 0x40;  // Set the second last bit
    cal_sr.send(cal_sr_state);
}

void AFE::connect_ref_mid_scale_to_inp()
{
    cal_sr_state &= ~0x03; // Clear the first two bits
    cal_sr_state |= 0x01;  // Set the first bit
    cal_sr.send(cal_sr_state);
}

void AFE::connect_ref_full_scale_to_inp()
{
    cal_sr_state &= ~0x03; // Clear the first two bits
    cal_sr_state |= 0x02;  // Set the second bit
    cal_sr.send(cal_sr_state);
}

void AFE::connect_ref_mid_scale_to_inn()
{
    cal_sr_state &= ~0x0C; // Clear the third and fourth bits
    cal_sr_state |= 0x08;  // Set the fourth bit
    cal_sr.send(cal_sr_state);
}

void AFE::connect_ref_full_scale_to_inn()
{
    cal_sr_state &= ~0x0C; // Clear the third and fourth bits
    cal_sr_state |= 0x04;  // Set the third bit
    cal_sr.send(cal_sr_state);
}

void AFE::short_inn_inp()
{
    cal_sr_state &= ~0xC0; // Clear the last two bits
    cal_sr_state |= 0x80;  // Set the last bit
    cal_sr.send(cal_sr_state);
}
