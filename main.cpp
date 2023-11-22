#include <stdio.h>
#include <pico/stdlib.h>
#include <hardware/spi.h>
#include "ShiftRegister595.hpp"
#include "ADS127L11/ADS127L11.hpp"
#include "HWDefs.h"
#include "AFE.hpp"

ShiftRegister595 hw_cal_sr(spi_default, 25, 2, 3);
ShiftRegister595 hw_gain_sr(spi_default, 24, 2, 3);

AFE::AFEConfig afe_config = {
    .spi = spi0,                         // SPI instance
    .gain_latch_pin = GAIN_SR_LATCH_PIN, // gain latch pin
    .gain_clock_pin = SPI0_CLK_PIN,      // gain clock pin
    .gain_data_pin = SPI0_MOSI_PIN,      // gain data pin
    .cal_latch_pin = CAL_SR_LATCH_PIN,   // calibration latch pin
    .cal_clock_pin = SPI0_CLK_PIN,       // calibration clock pin
    .cal_data_pin = SPI0_MOSI_PIN        // calibration data pin
};

// Initialize the AFE object with the configuration
AFE vm_AFE(afe_config);

ADS127L11 main_adc(spi0, MAIN_ADC_CS_PIN, MAIN_ADC_RST_PIN);

void main_adc_read_callback(uint gpio, uint32_t events);

void init_start_adc_conv_pin_interrupt()
{
    gpio_init(MAIN_ADC_DRDY_PIN);
    gpio_set_dir(MAIN_ADC_DRDY_PIN, GPIO_IN);
    printf("Attaching GPIO IRQ on ADC DRDY pin %i\n", MAIN_ADC_DRDY_PIN);
    gpio_set_irq_enabled_with_callback(MAIN_ADC_DRDY_PIN, GPIO_IRQ_EDGE_FALL, true, &main_adc_read_callback);
}
void stop_adc_conv_pin_interrupt()
{
    gpio_set_irq_enabled_with_callback(MAIN_ADC_DRDY_PIN, GPIO_IRQ_EDGE_FALL, false, &main_adc_read_callback);
}

void print_adc_regs()
{
    printf("\n*** ADS127L11 REG DUMP BEGIN ***\n");
    printf("DEV_ID = 0x%X\n", main_adc.read_reg(ADS127L11::reg_t::DEV_ID));
    printf("REV_ID = 0x%X\n", main_adc.read_reg(ADS127L11::reg_t::REV_ID));
    printf("STATUS = 0x%X\n", main_adc.read_reg(ADS127L11::reg_t::STATUS));
    printf("CONTROL = 0x%X\n", main_adc.read_reg(ADS127L11::reg_t::CONTROL));
    printf("MUX = 0x%X\n", main_adc.read_reg(ADS127L11::reg_t::MUX));
    printf("CONFIG1 = 0x%X\n", main_adc.read_reg(ADS127L11::reg_t::CONFIG1));
    printf("CONFIG2 = 0x%X\n", main_adc.read_reg(ADS127L11::reg_t::CONFIG2));
    printf("CONFIG3 = 0x%X\n", main_adc.read_reg(ADS127L11::reg_t::CONFIG3));
    printf("CONFIG4 = 0x%X\n", main_adc.read_reg(ADS127L11::reg_t::CONFIG4));
    printf("OFFSET2 = 0x%X\n", main_adc.read_reg(ADS127L11::reg_t::OFFSET2));
    printf("OFFSET1 = 0x%X\n", main_adc.read_reg(ADS127L11::reg_t::OFFSET1));
    printf("OFFSET0 = 0x%X\n", main_adc.read_reg(ADS127L11::reg_t::OFFSET0));
    printf("GAIN2 = 0x%X\n", main_adc.read_reg(ADS127L11::reg_t::GAIN2));
    printf("GAIN1 = 0x%X\n", main_adc.read_reg(ADS127L11::reg_t::GAIN1));
    printf("GAIN0 = 0x%X\n", main_adc.read_reg(ADS127L11::reg_t::GAIN0));
    printf("CRC = 0x%X\n", main_adc.read_reg(ADS127L11::reg_t::CRC));
    printf("*** ADS127L11 REG DUMP END ***\n");
}

void init_main_adc()
{
    main_adc.init();
    main_adc.hw_reset();
    print_adc_regs();
    main_adc.clear_stat_flags();
    printf("\nCleared main ADC status flags\n");
    print_adc_regs();
    main_adc.set_input_mux(ADS127L11::InputMuxMode_t::NormalInputPolarity);
    main_adc.set_config1_reg({.REF_RNG = true,
                              .INP_RNG = false,
                              .VCM = false,
                              .REFP_BUF = false,
                              .AINP_BUF = false,
                              .AINN_BUF = false});

    // main_adc.set_config3_reg({.delay = 0, .filter = 0b01000}); // 01000b = sinc4, OSR = 12
    // main_adc.set_config3_reg({.delay = 0, .filter = 0b11100}); //  11100b = sinc3, OSR = 26667
    // main_adc.set_config3_reg({.delay = 0, .filter = 0b01101}); //  01101b = sinc4, OSR = 128
    main_adc.set_config3_reg({.delay = 0, .filter = 0b01100}); //  01100b = sinc4, OSR = 64

    main_adc.set_config4_reg({.CLK_SEL = 0,
                              .CLK_DIV = 0,
                              .DATA = 0,
                              .SPI_CRC = 0,
                              .REG_CRC = 0,
                              .STATUS = 1});

    main_adc.start_cov();

    printf("\nFinished main ADC Setup\n");
    print_adc_regs();
    // main_adc.spi_interface_reset();
}

void init_voltmeter()
{
    init_main_adc();
    init_start_adc_conv_pin_interrupt();
}

void main_adc_read_callback(uint gpio, uint32_t events)
{
    ADS127L11::conv_data_t d = main_adc.get_conv_data();
    // main_adc.bin_conv(d);
    printf("EVNT=%u CB=%u v= %f b= 0x%x SR: 0x%x\n",
           events, gpio, d.v, d.data, d.status_reg);
}

int main()
{
    stdio_init_all();
    spi_init(spi_default, 1000e3);
    gpio_set_function(0, GPIO_FUNC_SPI);
    // gpio_set_function(2, GPIO_FUNC_SPI);
    // gpio_set_function(3, GPIO_FUNC_SPI);

    // gpio_init(25);
    // gpio_init(24);

    // gpio_set_dir(25, GPIO_OUT);
    // gpio_set_dir(24, GPIO_OUT);

    // gpio_put(25, 1);
    // gpio_put(24, 1);

    // gpio_put(25, 0);
    // gpio_put(24, 0);

    // spi_write_blocking(spi_default, 0x00, 1);
    // gpio_put(25, 1);
    // gpio_put(24, 1);

    while (1)
    {
        printf("Hello, world!\n");
        sleep_ms(1000);
    }
    return 0;
}