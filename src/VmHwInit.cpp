
#include <stdio.h>
#include <pico/stdlib.h>
#include <hardware/spi.h>
#include "ShiftRegister595.hpp"
#include "ADS127L11/ADS127L11.hpp"
#include "HWDefs.h"
#include "AFE.hpp"
#include "VoltMeterInputs.hpp"
#include "ppBuf.hpp"
#include "trigger.hpp"
#include "PrintMacros/printMacros.h"
#include "VmHw.hpp"

#include "fifo/fifo.hpp"
Fifo<float, 1024> adc_data_q;

// embxx::container::StaticQueue<float, 2048> adc_data_q;

VoltMeterInputConfig vm_input_config = {
    .inn_msel_0 = INN_MUX_SEL_0_PIN,
    .inn_msel_1 = INN_MUX_SEL_1_PIN,
    .inn_msel_2 = INN_MUX_SEL_2_PIN,
    .inn_msel_3 = INN_MUX_SEL_3_PIN,
    .inp_msel_0 = INP_MUX_SEL_0_PIN,
    .inp_msel_1 = INP_MUX_SEL_1_PIN,
    .inp_msel_2 = INP_MUX_SEL_2_PIN,
    .inp_msel_3 = INP_MUX_SEL_3_PIN,
    .in_rel_sel = INP_REL_SEL_PIN};

// Create a VoltMeterInputs object
VoltMeterInputs vm_Inputs(vm_input_config);
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
    print_info("Attaching GPIO IRQ on ADC DRDY pin %i\n", MAIN_ADC_DRDY_PIN);
    gpio_set_irq_enabled_with_callback(MAIN_ADC_DRDY_PIN, GPIO_IRQ_EDGE_FALL, true, &main_adc_read_callback);
}
void stop_adc_conv_pin_interrupt()
{
    gpio_set_irq_enabled_with_callback(MAIN_ADC_DRDY_PIN, GPIO_IRQ_EDGE_FALL, false, &main_adc_read_callback);
}

void print_adc_regs()
{
    print_info("\n*** ADS127L11 REG DUMP BEGIN ***\n");
    print_info("DEV_ID = 0x%X\n", main_adc.read_reg(ADS127L11::reg_t::DEV_ID));
    print_info("REV_ID = 0x%X\n", main_adc.read_reg(ADS127L11::reg_t::REV_ID));
    print_info("STATUS = 0x%X\n", main_adc.read_reg(ADS127L11::reg_t::STATUS));
    print_info("CONTROL = 0x%X\n", main_adc.read_reg(ADS127L11::reg_t::CONTROL));
    print_info("MUX = 0x%X\n", main_adc.read_reg(ADS127L11::reg_t::MUX));
    print_info("CONFIG1 = 0x%X\n", main_adc.read_reg(ADS127L11::reg_t::CONFIG1));
    print_info("CONFIG2 = 0x%X\n", main_adc.read_reg(ADS127L11::reg_t::CONFIG2));
    print_info("CONFIG3 = 0x%X\n", main_adc.read_reg(ADS127L11::reg_t::CONFIG3));
    print_info("CONFIG4 = 0x%X\n", main_adc.read_reg(ADS127L11::reg_t::CONFIG4));
    print_info("OFFSET2 = 0x%X\n", main_adc.read_reg(ADS127L11::reg_t::OFFSET2));
    print_info("OFFSET1 = 0x%X\n", main_adc.read_reg(ADS127L11::reg_t::OFFSET1));
    print_info("OFFSET0 = 0x%X\n", main_adc.read_reg(ADS127L11::reg_t::OFFSET0));
    print_info("GAIN2 = 0x%X\n", main_adc.read_reg(ADS127L11::reg_t::GAIN2));
    print_info("GAIN1 = 0x%X\n", main_adc.read_reg(ADS127L11::reg_t::GAIN1));
    print_info("GAIN0 = 0x%X\n", main_adc.read_reg(ADS127L11::reg_t::GAIN0));
    print_info("CRC = 0x%X\n", main_adc.read_reg(ADS127L11::reg_t::CRC));
    print_info("*** ADS127L11 REG DUMP END ***\n");
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

    // main_adc.set_config3_reg({.delay = 0, .filter = 0b01000}); // 01000b = sinc4, OSR = 12, DR = 1066.666 KHz
    // main_adc.set_config3_reg({.delay = 0, .filter = 0b11100}); //  11100b = sinc3, OSR = 26667, DR = 480 Hz
    // main_adc.set_config3_reg({.delay = 0, .filter = 0b01101}); //  01101b = sinc4, OSR = 128
    // main_adc.set_config3_reg({.delay = 0, .filter = 0b01100}); //  01100b = sinc4, OSR = 64
    // main_adc.set_config3_reg({.delay = 0, .filter = 0b10010}); // 10010b = sinc4, OSR = 4096, 3.125 KHz
    main_adc.set_config3_reg({.delay = 0, .filter = 0b00101}); // 00101b = wideband, OSR = 1024, 12.5 KHz
    // main_adc.set_config3_reg({.delay = 0, .filter = 0b10001}); //  10001b = sinc4, OSR = 2048, 6.25 KHz

    main_adc.set_config4_reg({.CLK_SEL = 0,
                              .CLK_DIV = 0,
                              .DATA = 0,
                              .SPI_CRC = 0,
                              .REG_CRC = 0,
                              .STATUS = 1});

    main_adc.start_cov();

    print_info("\nFinished main ADC Setup\n");
    print_adc_regs();
    // main_adc.spi_interface_reset();
}

float _gain_of_afe = 1;
void init_voltmeter()
{
    //     hw_gain_sr.send(0xff);
    //     hw_cal_sr.send(0xff);
    sleep_ms(1000);

    init_main_adc();
    _gain_of_afe = (1.0f / 8.0f);
    vm_AFE.set_fda_gain(AFE::fda_gain_t::GAIN_DIV_8);
    sleep_ms(1000);
    // vm_AFE.short_inn_inp();
    vm_AFE.set_input_gain(AFE::input_gain_t::GAIN_1);
    sleep_ms(100);

    vm_AFE.set_attenuation(AFE::attenuation_t::DIV_1);
    sleep_ms(100);

    vm_AFE.connect_negative_input_buffer_to_inn();
    sleep_ms(100);

    vm_Inputs.setInn(VoltMeterInputs::InputSelection::INPUT_VIN);
    sleep_ms(100);
    vm_Inputs.setInp(VoltMeterInputs::InputSelection::INPUT_VIN);
    sleep_ms(100);
    vm_Inputs.setInputAttenuation(VoltMeterInputs::InputAttenuation::DIV_1);
    sleep_ms(100);

    init_start_adc_conv_pin_interrupt();
}

void main_adc_process_task()
{

    ADS127L11::conv_data_t d = main_adc.get_conv_data();
    d.v = (d.v / _gain_of_afe);
    adc_data_q.enqueue(d.v);

    // unsigned char *c = reinterpret_cast<unsigned char *>(&d.v);
    // printf("*%c%c%c%c*", c[3], c[2], c[1], c[0]); //(d.v >> 24) & 0xff, (d.v >> 16) & 0xff, (d.v >> 8) & 0xff, d.v & 0xff);

    // putchar('*');
    // putchar(c[3]);
    // putchar(c[2]);
    // putchar(c[1]);
    // putchar(c[0]);
    // putchar('*');
}

void main_adc_read_callback(uint gpio, uint32_t events)
{
    static uint64_t entry_time = time_us_64();
    static uint64_t exit_time;
    // _adc_isr_callback_time_us = entry_time - exit_time;

    main_adc_process_task();
    // pp_adc_data_buf.append(d.v);
    // main_adc.bin_conv(d);
    //     printf("EVNT=%u CB=%u v= %f b= 0x%x SR: 0x%x\n",
    //            events, gpio, d.v, d.data, d.status_reg);

    exit_time = time_us_64();
    // _adc_isr_process_time_us = exit_time - entry_time;
}

void vm_hw_defalt_int()
{
    stdio_init_all();
    // sleep_ms(1000);
    gpio_set_function(SPI0_CLK_PIN, GPIO_FUNC_SPI);
    gpio_set_function(SPI0_MOSI_PIN, GPIO_FUNC_SPI);
    gpio_set_function(SPI0_MISO_PIN, GPIO_FUNC_SPI);

    spi_init(spi_default, 1000e3);
    // gpio_set_function(0, GPIO_FUNC_SPI);
    init_voltmeter();
}