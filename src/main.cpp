#include <stdio.h>
#include <pico/stdlib.h>
#include "Cli.hpp"
#include "PrintMacros/printMacros.h"
#include <pico/multicore.h>
#include "ShiftRegister595.hpp"
#include "FreeRTOS.h"
#include "task.h"
#include <hardware/spi.h>
#include "HWDefs.h"
#include "VmHw.hpp"

// ShiftRegister595 hw_cal_sr(spi0, 25, 2, 3);
// ShiftRegister595 hw_gain_sr(spi0, 24, 2, 3);
void core1_entry();

int main()
{
    // setup_default_uart();
    // hw_cal_sr.clear();
    // hw_gain_sr.clear();

    stdio_init_all();
    vm_hw_defalt_int();

        // cli_init_default();

    print_info("Config Complete\n");
    sleep_ms(1000);

    multicore_launch_core1(core1_entry);
    while (1)
    {
        ;

        // print_info("Hello, world!\n");

        // cli_read_and_process();
        // sleep_ms(5000);
    }

    return 0;
}

// Core 1 Main Code
void core1_entry()
{
    // // Configure Core 1 Interrupt
    // multicore_fifo_clear_irq();
    // irq_set_exclusive_handler(SIO_IRQ_PROC1, core1_interrupt_handler);

    // irq_set_enabled(SIO_IRQ_PROC1, true);

    // Infinte While Loop to wait for interrupt
    while (1)
    {
        if (adc_data_q.num_items())
        {

            float outv;
            adc_data_q.dequeue(outv);

            unsigned char *c = reinterpret_cast<unsigned char *>(&outv);
            printf("*%c%c%c%c*", c[3], c[2], c[1], c[0]); //(d.v >> 24) & 0xff, (d.v >> 16) & 0xff, (d.v >> 8) & 0xff, d.v & 0xff);
        }
    }
}

void vApplicationStackOverflowHook(TaskHandle_t pxTask, char *pcTaskName)
{
    (void)pcTaskName;
    (void)pxTask;

    /* Run time stack overflow checking is performed if
    configCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2.  This hook
    function is called if a stack overflow is detected.  This function is
    provided as an example only as stack overflow checking does not function
    when running the FreeRTOS Windows port. */
    print_error("stack overfolw on, line: %s, task: %s ", __LINE__, pcTaskName);
}