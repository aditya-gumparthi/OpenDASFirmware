#include <stdio.h>
#include <pico/stdlib.h>
#include "Cli.hpp"
#include "PrintMacros/printMacros.h"
#include "ShiftRegister595.hpp"
#include "FreeRTOS.h"
#include "task.h"
#include <hardware/spi.h>

ShiftRegister595 hw_cal_sr(spi0, 25, 2, 3);
ShiftRegister595 hw_gain_sr(spi0, 24, 2, 3);

int main()
{
    // setup_default_uart();
    hw_cal_sr.clear();
    hw_gain_sr.clear();

    stdio_init_all();
    sleep_ms(1000);

    cli_init_default();

    printf("Hello, world!\n");

    while (1)
    {
        // print_info("Hello, world!\n");

        cli_read_and_process();
        sleep_ms(5000);
    }

    return 0;
}

void vApplicationStackOverflowHook( TaskHandle_t pxTask, char *pcTaskName )
{
	( void ) pcTaskName;
	( void ) pxTask;

	/* Run time stack overflow checking is performed if
	configCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2.  This hook
	function is called if a stack overflow is detected.  This function is
	provided as an example only as stack overflow checking does not function
	when running the FreeRTOS Windows port. */
	print_error("stack overfolw on, line: %s, task: %s ", __LINE__, pcTaskName );
}