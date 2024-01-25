#ifndef CLI_HPP
#define CLI_HPP

// #include "embedded_cli.h"
#include <pico/stdlib.h>
#include <pico/error.h>
#include <stdio.h>

#define print_cli_reply(...)             \
    do                                   \
    {                                    \
                                         \
        printf("[REPLY]: " __VA_ARGS__); \
    } while (0)

const size_t CLI_BUFFER_SIZE = 2048;
const size_t CLI_RX_BUFFER_SIZE = 64;
const size_t CLI_CMD_BUFFER_SIZE = 64;
const size_t CLI_HISTORY_SIZE = 128;
const size_t CLI_BINDING_COUNT = 32;
const char invitation_str[] = ">";

void cli_init_default();
void cli_read_and_process();

#endif /* CLI_HPP */
