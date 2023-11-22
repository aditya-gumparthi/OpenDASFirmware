#include "ADS127L11.hpp"
ADS127L11::conv_data_t currb;
// ppBuf<ADS127L11::conv_data_t, 8000> eadc_buf;

void spi_evnt_cb();

ADS127L11::ADS127L11(spi_inst_t *_spi, uint8_t cs_pin_, uint8_t rst_pin_) : spi_hw(_spi), cs_pin(cs_pin_), rst_pin(rst_pin_)
{
    ;
}

// void spi_evnt_cb()
// {
//     adc_ext.deselect_chip();
//     currb = adc_ext.bin_conv((adc_ext.rec_buf[1] << 16) | (adc_ext.rec_buf[2] << 8) | adc_ext.rec_buf[3]);
//     currb.status_reg = adc_ext.rec_buf[0];
//     eadc_buf.append(currb);
//     if (adc_ext.cb_while_dma)
//     {
//         adc_ext.cb_while_dma = false;
//         adc_ext.init_dma_read();
//     }
//     // Serial.printf("> %f %x %x\n", b.v, b.data, adc_ext.rec_buf[0]);
// }

void ADS127L11::init()

{
    gpio_init(cs_pin);
    gpio_set_dir(cs_pin, GPIO_OUT);
    deselect_chip(); // Deselect chip

    gpio_init(rst_pin);
    gpio_set_dir(rst_pin, GPIO_OUT);
    hw_reset(); // Hardware reset

    // Initialize SPI
    spi_init(spi_hw, 20000000); // Initialize SPI0 at 20 MHz
}
void ADS127L11::select_chip()
{
    gpio_put(cs_pin, 0);
    asm volatile("nop \n nop \n nop"); // No operation for delay
} // Set CS pin LOW
void ADS127L11::deselect_chip()
{
    gpio_put(cs_pin, 1);               // Set CS pin high
    asm volatile("nop \n nop \n nop"); // No operation for delay
}

void ADS127L11::hw_reset()
{
    gpio_put(rst_pin, 0); // Set reset pin low
    sleep_ms(10);         // Wait for 10 milliseconds
    gpio_put(rst_pin, 1); // Set reset pin high
}

void ADS127L11::spi_interface_reset()
{
    uint8_t ob[8] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff}, ib[8];
    spi_write_read_blocking(spi_hw, ob, ib, 8);
}

void ADS127L11::set_spi_context()
{

    // spi_hw.beginTransaction(SPISettings(50e6, MSBFIRST, SPI_MODE1));
    spi_init(spi_hw, 20e6); // Initialize SPI0 at 20 MHz
    spi_set_format(spi_hw, 8, SPI_CPOL_0, SPI_CPHA_1, SPI_MSB_FIRST);
    asm volatile("nop \n nop \n nop"); // No operation for delay

    // spi_hw.transfer(0x00);
}

void ADS127L11::send_receive(uint8_t *send_buffer, uint8_t *rec_buf, size_t msg_size)
{
    set_spi_context();
    select_chip();
    spi_write_read_blocking(spi_hw, send_buffer, rec_buf, msg_size);
    deselect_chip();
}

uint8_t ADS127L11::read_reg(reg_t reg_to_read)
{
    // frame 1 setup reg
    uint8_t send_buf[2] = {READ_REG_BM | (static_cast<uint8_t>(0x0f) & reg_to_read), 0x00};
    uint8_t rec_buf[2] = {0x00, 0x00}; // Data in this frame are either 16 bits of conversion data,
                                       // or if the read register command is sent in the prior frame,
                                       // the data field is the register data byte + 00h

    send_receive(send_buf, rec_buf, 2);
    // frame 2 for actual reg data
    uint8_t send_buf2[2] = {0x00, 0x00};
    uint8_t rec_buf2[2] = {0x00, 0x00};
    send_receive(send_buf2, rec_buf2, 2);
    if (!status_append_en)
        return rec_buf2[0]; /// rec_buf[1] is a 0x00 pad
    else
        return rec_buf2[1]; /// rec_buf[0] is status
}

void ADS127L11::write_reg(reg_t reg_to_write, uint8_t reg_data)
{
    uint8_t send_buf[2] = {WRITE_REG_BM | (static_cast<uint8_t>(0x0f) & reg_to_write), reg_data};
    uint8_t rec_buf[2] = {0x00, 0x00}; // Data in this frame are either 16 bits of conversion data,
                                       // or if the read register command is sent in the prior frame,
                                       // the data field is the register data byte + 00h

    send_receive(send_buf, rec_buf, 2);
}

uint8_t ADS127L11::get_status()
{
    return read_reg(reg_t::STATUS);
}

void ADS127L11::clear_stat_flags()
{
    write_reg(reg_t::STATUS, 0xff); // writing 1 to ALV_FLAG, POR_FLAG, SPI_ERR, REG_ERR
}

void ADS127L11::start_cov()
{
    write_reg(reg_t::CONTROL, 0x02);
}
void ADS127L11::stop_cov()
{
    write_reg(reg_t::CONTROL, 0x01);
}

void ADS127L11::set_input_mux(InputMuxMode_t mode)
{
    write_reg(reg_t::MUX, mode & 0b00000011);
}

void ADS127L11::set_config1_reg(config1_reg_t config)
{
    uint8_t reg = 0;
    reg |= (static_cast<uint8_t>(config.REF_RNG) << 6);
    reg |= (static_cast<uint8_t>(config.INP_RNG) << 5);
    reg |= (static_cast<uint8_t>(config.VCM) << 4);
    reg |= (static_cast<uint8_t>(config.REFP_BUF) << 3);
    reg |= (static_cast<uint8_t>(config.AINP_BUF) << 1);
    reg |= (static_cast<uint8_t>(config.AINN_BUF) << 0);
    write_reg(reg_t::CONFIG1, reg);
}

void ADS127L11::set_config2_reg(config2_reg_t config)
{
    uint8_t reg = 0;
    reg |= (config.EXT_RNG << 7);

    reg |= (config.SDO_MODE << 5);
    reg |= ((static_cast<uint8_t>(config.START_MODE) & 0b11) << 3);
    reg |= (config.SPEED_MODE << 2);
    reg |= (config.STBY_MODE << 1);
    reg |= config.PWDN;
    write_reg(reg_t::CONFIG2, reg);
}
void ADS127L11::set_config3_reg(config3_reg_t config)
{
    uint8_t reg = (config.delay << 5) | config.filter;
    write_reg(reg_t::CONFIG3, reg);
}

void ADS127L11::set_config4_reg(config4_reg_t config)
{
    uint8_t reg = 0;
    reg |= (config.CLK_SEL << 7);
    reg |= (config.CLK_DIV << 6);
    reg |= (config.OUT_DRV << 5);
    reg |= (config.DATA << 3);
    reg |= (config.SPI_CRC << 2);
    reg |= (config.REG_CRC << 1);
    reg |= config.STATUS;

    status_append_en = config.STATUS;

    write_reg(reg_t::CONFIG4, reg);
}

ADS127L11::conv_data_t ADS127L11::bin_conv(uint32_t bit)
{
    ADS127L11::conv_data_t out;
    out.data = bit;

    uint8_t sign_bit = (out.data & 0xFFFFFF) >> 23;

    if (!sign_bit)
        out.v = out.data * lsb_v;
    else
    {
        out.data = ~out.data;
        out.data += 1;
        out.data = out.data & 0xFFFFFF;
        out.v = out.data * lsb_v * -1;
    }
    return out;
}

// bool ADS127L11::init_dma_read()
// {
//     bool tcomp = spi_hw.transfer(send_buf, rec_buf, 4, evnt_h);
//     if (tcomp)
//         select_chip();
//     return tcomp;
// }

ADS127L11::conv_data_t ADS127L11::get_conv_data()
{
    conv_data_t out;
    // uint32_t idata = 0xffff, odata = 0x0000;
    if (!status_append_en)
    {
        send_receive(send_buf, rec_buf, 3);
        out.data = (rec_buf[0] << 16) | (rec_buf[1] << 8) | rec_buf[2];
    }
    else
    {

        // idata = spi_hw.transfer32(odata);
        send_receive(send_buf, rec_buf, 4);
        // if (spi_hw.transfer(send_buf, rec_buf, 4, evnt_h))
        //     select_chip();
        // else
        // {
        //     if (!cb_while_dma)
        //     {
        //         cb_while_dma = true;
        //     }
        // }

        out.status_reg = rec_buf[0];
        out.data = (rec_buf[1] << 16) | (rec_buf[2] << 8) | rec_buf[3];
    }
    uint8_t sign_bit = (out.data & 0xFFFFFF) >> 23;
    // // printf(">%x\n", sign_bit);
    // out.data = out.data & 0x7FFFFF;

    if (!sign_bit)
        out.v = out.data * lsb_v;
    else
    {
        out.data = ~out.data;
        out.data += 1;
        out.data = out.data & 0xFFFFFF;
        out.v = out.data * lsb_v * -1;
    }

    return out;
}
