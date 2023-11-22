#ifndef ADS127L11_HPP
#define ADS127L11_HPP
#include <stddef.h>
#include <stdint.h>
// #include <Arduino.h>
// #include <SPI.h>
// #define spi_hw SPI

// #include <ppBuf.hpp>
#include "pico/stdlib.h"
#include "hardware/spi.h"
class ADS127L11
{
private:
    // EventResponder evnt_h;

    // bitmasks
    const uint8_t READ_REG_BM = 0x40;
    const uint8_t WRITE_REG_BM = 0x80;

    const uint8_t cs_pin, rst_pin;

    spi_inst_t *const spi_hw; // Pointer to SPI instance

    const float ref_v = 4.096f; // refrence voltage
    const size_t RESOLUTION_BITS = 24u;
    const size_t FS_UNIPOLAR = 8388608;
    const float lsb_v = ref_v / FS_UNIPOLAR; // 2.44140625e-7;

    void select_chip();
    void deselect_chip();

    void send_receive(uint8_t *send_buffer, uint8_t *rec_buf, size_t msg_size);

    void set_spi_context();

    bool status_append_en = false;
    uint8_t rec_buf[4] = {0x00, 0x00, 0x00, 0x00};
    // bool cb_while_dma = false;
    // bool init_dma_read();

    uint8_t send_buf[4] = {0x00, 0x00, 0x00, 0x00a};

public:
    typedef enum : uint8_t
    {
        DEV_ID = 0x00, // 0h
        REV_ID,        // 1h
        STATUS,        // 2h
        CONTROL,       // 3h
        MUX,           // 4h
        CONFIG1,       // 5h
        CONFIG2,       // 6h
        CONFIG3,       // 7h
        CONFIG4,       // 8h
        OFFSET2,       // 9h
        OFFSET1,       // Ah
        OFFSET0,       // Bh
        GAIN2,         // Ch
        GAIN1,         // Dh
        GAIN0,         // Eh
        CRC            // Fh
    } reg_t;

    typedef enum : uint8_t
    {
        NormalInputPolarity = 0b00,   // Normal input polarity
        InvertedInputPolarity = 0b01, // Inverted input polarity

        OffsetNoiseTest = 0b10, // Offset and noise test: AINP and AINN disconnected,
                                // ADC inputs internally shorted to (AVDD1 + AVSS) / 2

        CommonModeTest = 0b11 // Common-mode test: ADC inputs internally shorted and connected to AIN
    } InputMuxMode_t;

    typedef struct
    {
        bool REF_RNG; // Bit 6: REF_RNG - Voltage reference range selection
                      // 0b = Low-reference range
                      // 1b = High-reference range

        bool INP_RNG; // Bit 5: INP_RNG - Input range selection

        bool VCM; // Bit 4: VCM - VCM output enable
                  // 0b = 1x input range
                  // 1b = 2x input range

        bool REFP_BUF; // Bit 3: REFP_BUF - Reference positive buffer enable.

        bool AINP_BUF; // Bit 1: AINP_BUF - Analog input positive buffer enable

        bool AINN_BUF; // Bit 0: AINN_BUF - Analog input negative buffer enable

    } config1_reg_t;

    typedef struct
    {
        bool EXT_RNG = 0; // Bit 7 - Extended input range selection
                          // 0b = Standard input range
                          // 1b = 25% extended input range

        bool SDO_MODE = 0; // Bit 5 - SDO/DRDY mode selection
                           // 0b = Data output only mode
                           // 1b = Dual mode: data output and data ready

        uint8_t START_MODE = 0b00; // Bits 4-3 - START mode selection
                                   // 00b = Start/stop control mode
                                   // 01b = One-shot control mode
                                   // 10b = Synchronized control mode
                                   // 11b = Reserved

        bool SPEED_MODE = 0; // Bit 2 - Speed mode selection
                             // 0b = High-speed mode (fCLK = 25.6 MHz)
                             // 1b = Low-speed mode (fCLK = 3.2 MHz)

        bool STBY_MODE = 0; // Bit 1 - Standby mode selection
                            // 0b = Idle mode; ADC remains fully powered when conversions are stopped.
                            // 1b = Standby mode; ADC powers down when conversions are stopped. Standby mode is exited when conversions restart.

        bool PWDN = 0; // Bit 0 - Power-down mode selection
                       // 0b = Normal operation
                       // 1b = Power-down mode
    } config2_reg_t;

    typedef struct
    {
        // Bit fields for DELAY[2:0]
        uint8_t delay = 0b00; // 3 bits for DELAY[2:0] - Conversion-start delay time selection
        // Commented values for DELAY[2:0]
        // 000b = 0
        // 001b = 4
        // 010b = 8
        // 011b = 16
        // 100b = 32
        // 101b = 128
        // 110b = 512
        // 111b = 1024

        // Bit fields for FILTER[4:0]
        uint8_t filter = 0b00; // 5 bits for FILTER[4:0] - Digital filter mode and oversampling ratio selection
        // Commented values for FILTER[4:0]
        // 00000b = wideband, OSR = 32
        // 00001b = wideband, OSR = 64
        // 00010b = wideband, OSR = 128
        // 00011b = wideband, OSR = 256
        // 00100b = wideband, OSR = 512
        // 00101b = wideband, OSR = 1024
        // 00110b = wideband, OSR = 2048
        // 00111b = wideband, OSR = 4096
        // 01000b = sinc4, OSR = 12
        // 01001b = sinc4, OSR = 16
        // 01010b = sinc4, OSR = 24
        // 01011b = sinc4, OSR = 32
        // 01100b = sinc4, OSR = 64
        // 01101b = sinc4, OSR = 128
        // 01110b = sinc4, OSR = 256
        // 01111b = sinc4, OSR = 512
        // 10000b = sinc4, OSR = 1024
        // 10001b = sinc4, OSR = 2048
        // 10010b = sinc4, OSR = 4096
        // 10011b = sinc4, OSR = 32 + sinc1, OSR = 2
        // 10100b = sinc4, OSR = 32 + sinc1, OSR = 4
        // 10101b = sinc4, OSR = 32 + sinc1, OSR = 10
        // 10110b = sinc4, OSR = 32 + sinc1, OSR = 20
        // 10111b = sinc4, OSR = 32 + sinc1, OSR = 40
        // 11000b = sinc4, OSR = 32 + sinc1, OSR = 100
        // 11001b = sinc4, OSR = 32 + sinc1, OSR = 200
        // 11010b = sinc4, OSR = 32 + sinc1, OSR = 400
        // 11011b = sinc4, OSR = 32 + sinc1, OSR = 1000
        // 11100b = sinc3, OSR = 26667
        // 11101b = sinc3, OSR = 32000
    } config3_reg_t;

    typedef struct
    {
        bool CLK_SEL = 0; // Bit 7: Clock selection
                          // 0 = internal, 1 = external

        bool CLK_DIV = 0; // Bit 6: External clock divider selection
                          // 0 = no division, 1 = division by 8

        bool OUT_DRV = 0; // Bit 5: Digital output drive selection
                          // 0 = full-drive strength, 1 = half-drive strength

        bool DATA = 0; // Bit 3: Data resolution selection
                       // 0 = 24-bit resolution, 1 = 16-bit resolution

        bool SPI_CRC = 0; // Bit 2: SPI CRC enable
                          // 0 = disabled, 1 = enabled

        bool REG_CRC = 0; // Bit 1: Register map CRC enable
                          // 0 = disabled, 1 = enabled

        bool STATUS = 0; // Bit 0: STATUS byte output enable
                         // 0 = not prefixed, 1 = prefixed to conversion data
    } config4_reg_t;

    struct conv_data_t
    {
        int data;
        uint8_t status_reg = 0xff;
        float v;
    };

    ADS127L11(spi_inst_t *_spi, uint8_t cs_pin_, uint8_t rst_pin_);
    void init();

    ~ADS127L11() { ; };
    uint8_t read_reg(reg_t reg_to_read);
    void write_reg(reg_t reg_to_write, uint8_t reg_data);

    uint8_t get_status();
    void clear_stat_flags();
    void hw_reset();

    void start_cov();
    void stop_cov();

    void set_input_mux(InputMuxMode_t mode);
    void set_config1_reg(config1_reg_t config);
    void set_config2_reg(config2_reg_t config);
    void set_config3_reg(config3_reg_t config);
    void set_config4_reg(config4_reg_t config);

    conv_data_t get_conv_data();

    conv_data_t bin_conv(uint32_t bit);

    void spi_interface_reset();
};
extern ADS127L11 adc_ext;

#endif