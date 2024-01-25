#include "Cli.hpp"
#include "PrintMacros/printMacros.h"

#define EMBEDDED_CLI_IMPL
#include "embedded_cli.h"

EmbeddedCliConfig *config = embeddedCliDefaultConfig();

EmbeddedCli *cli;
CLI_UINT cliBuffer[BYTES_TO_CLI_UINTS(CLI_BUFFER_SIZE)];

bool _cli_init = false;

void cli_char_writer_usb(EmbeddedCli *embeddedCli, char c);
void cli_add_cmds();
void cmd_info(EmbeddedCli *cli, char *args, void *context);

CliCommandBinding info_binding = {
    .name = "info",
    .help = "Get device info",
    .tokenizeArgs = true,
    .context = NULL,
    .binding = cmd_info};

void cli_init_default()
{
    config->invitation = invitation_str;
    config->cliBuffer = cliBuffer;
    config->cliBufferSize = CLI_BUFFER_SIZE;
    config->rxBufferSize = CLI_RX_BUFFER_SIZE;
    config->cmdBufferSize = CLI_CMD_BUFFER_SIZE;
    config->historyBufferSize = CLI_HISTORY_SIZE;
    config->maxBindingCount = CLI_BINDING_COUNT;
    config->enableAutoComplete = true;

    uint16_t requiredSize = embeddedCliRequiredSize(config);

    cli = embeddedCliNew(config);
    if (!cli) // // CLI init failed. Is there not enough memory allocated to the CLI?
    {

        return;
    }

    cli->writeChar = cli_char_writer_usb;

    cli_add_cmds();
    _cli_init = true;
}

void cli_char_writer_usb(EmbeddedCli *embeddedCli, char c)
{
    printf("%c", c);
}

void cli_add_cmds()
{
    embeddedCliAddBinding(cli, info_binding);
}

void cmd_info(EmbeddedCli *cli, char *args, void *context)
{
    print_cli_reply("OpenDAS, HW rev A, Firmware version 0.1.0, %s %s\n", __DATE__, __TIME__);
}

void cli_read_and_process()
{

    if (_cli_init)
    {
        int resp = getchar_timeout_us(0);
        if (resp != PICO_ERROR_TIMEOUT)
        {
            embeddedCliReceiveChar(cli, static_cast<char>(resp));
        }
        embeddedCliProcess(cli);
    }
}