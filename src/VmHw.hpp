#ifndef VM_HW_HPP
#define VM_HW_HPP
#include "fifo/fifo.hpp"
extern Fifo<float, 1024> adc_data_q;

void vm_hw_defalt_int();

#endif /* VM_HW_HPP */
