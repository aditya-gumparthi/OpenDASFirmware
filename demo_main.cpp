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
// ShiftRegister595 hw_cal_sr(spi_default, 25, 2, 3);
// ShiftRegister595 hw_gain_sr(spi_default, 24, 2, 3);

int64_t _adc_isr_process_time_us = 1;
int64_t _adc_isr_callback_time_us = 1;
int64_t _trigger_frame_print_process_time_us = 1;
int64_t _trigger_frame_print_callback_time_us = 1;

float _gain_of_afe = 1;
ppBuf<float, 1024u> pp_adc_data_buf;
template <typename T, const size_t N>
void printTriggerData(ppBuf<T, N> &buffer, float trigger_level, SignalType signal, size_t before_n, size_t after_n)
{
    static uint64_t entry_time = time_us_64();
    static uint64_t exit_time;
    _trigger_frame_print_callback_time_us = entry_time - exit_time;

    static size_t lastProcessedIteration = 0;

    // Check if the buffer is fresh
    if (buffer.iteration_counter == lastProcessedIteration)
    {
        return; // Buffer already processed
    }

    // Update the last processed iteration
    lastProcessedIteration = buffer.iteration_counter;

    int triggerIndex = findTriggerIndex(buffer, trigger_level, signal);
    if (triggerIndex != -1)
    {
        // Ensure there is enough space after the trigger
        if (triggerIndex + after_n >= N)
        {
            printf("Trigger ignored due to insufficient space after trigger index.\n");
            return;
        }

        // Calculate start and end indices for printing
        size_t start = triggerIndex > before_n ? triggerIndex - before_n : 0;
        size_t end = triggerIndex + after_n;

        // Print values before and after the trigger
        printf("Trigger found at index: %d\n", triggerIndex);
        printf("<TFrame [%i] tsmpl_us=%i>", (end - start), (_adc_isr_callback_time_us));
        for (size_t i = start; i <= end; ++i)
        {
            printf("%f, ", buffer.read_active_buf(i));
        }
        printf("\n");

        exit_time = time_us_64();
        _trigger_frame_print_process_time_us = exit_time - entry_time;
        printf("Trigger print took: %i us\n", _trigger_frame_print_process_time_us);
    }
    {
        printf("No trigger found.\n");
    }
}

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
    main_adc.set_config3_reg({.delay = 0, .filter = 0b11100}); //  11100b = sinc3, OSR = 26667
    // main_adc.set_config3_reg({.delay = 0, .filter = 0b01101}); //  01101b = sinc4, OSR = 128
    // main_adc.set_config3_reg({.delay = 0, .filter = 0b01100}); //  01100b = sinc4, OSR = 64

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
    //     hw_gain_sr.send(0xff);
    //     hw_cal_sr.send(0xff);
    sleep_ms(1000);

    init_main_adc();
    _gain_of_afe = 1.0f / 8.0f;
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

void main_adc_read_callback(uint gpio, uint32_t events)
{
    static uint64_t entry_time = time_us_64();
    static uint64_t exit_time;
    _adc_isr_callback_time_us = entry_time - exit_time;

    ADS127L11::conv_data_t d = main_adc.get_conv_data();
    d.v = (d.v / _gain_of_afe);
    pp_adc_data_buf.append(d.v);
    // main_adc.bin_conv(d);
    //     printf("EVNT=%u CB=%u v= %f b= 0x%x SR: 0x%x\n",
    //            events, gpio, d.v, d.data, d.status_reg);

    exit_time = time_us_64();
    _adc_isr_process_time_us = exit_time - entry_time;
}

int main()
{
    stdio_init_all();
    // sleep_ms(1000);
    gpio_set_function(SPI0_CLK_PIN, GPIO_FUNC_SPI);
    gpio_set_function(SPI0_MOSI_PIN, GPIO_FUNC_SPI);
    gpio_set_function(SPI0_MISO_PIN, GPIO_FUNC_SPI);

    spi_init(spi_default, 1000e3);
    // gpio_set_function(0, GPIO_FUNC_SPI);
    init_voltmeter();

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
        printTriggerData(pp_adc_data_buf, 0.005f, SignalType::RISING, 32u, 256u);
        // printf("Hello, world!\n");
        // sleep_ms(1000);
    }
    return 0;
}