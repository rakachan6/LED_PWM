#include "cli.h"

static void str_to_dec(char const *str, uint32_t *dec)
{
    uint8_t tmp = 0U;
    while (*str >= '0' && *str <= '9')
    {
        tmp = *str - '0';
        *dec = *dec * 10U + tmp;
        str++;
    }
    if (*dec <= 100U || *dec >= 100000U)
    {
        *dec = 1000U;
        usart1_transmit("Time value is out of range. LED period set as 1000 milliseconds\r\n");
    }
}
static void dec_to_str(uint32_t dec, char *str)
{
    uint8_t len = 0U;
    char tmp_str[11U];
    do
    {
        tmp_str[len++] = (dec % 10U) + '0';
        dec /= 10U;
    } while (dec);

    while (len)
        *str++ = tmp_str[--len];

    *str = '\0';
}

static void cli_print_prompt(void)
{
    usart1_transmit(">>>\r\n");
}

static void cli_get_command(ring_buffer_t *rb, uint8_t * cmd)
{
    //buffer is not overflow & ring buffer has data
    for (uint8_t i = 0U;
        ((i < CLI_COMMAND_SIZE) && (ring_buffer_get(rb, &cmd[i])) == RING_BUFFER_OK);
        i++)
    {
        if ((cmd[i] == '\r') || (cmd[i] == '\n') || (cmd[i] == '\0'))
        {
            cmd[i] = '\0';
            break;
        }
    }
    cmd[CLI_COMMAND_SIZE - 1U] = '\0';
}

static void cli_led_process(char * time)
{
    uint32_t dec_time = 0U;
    str_to_dec(time, &dec_time);

    //Change LED period
}

static void cli_print_help(void)
{
    usart1_transmit("MCU receive next commands\r\n");
    usart1_transmit("    led <time_in_ms> - set leds period as <time>\r\n");
    usart1_transmit("    help - print that message\r\n");
    usart1_transmit("    uptime - print uptime\r\n");
    usart1_transmit("    reset  - reset the system\r\n");
    cli_print_prompt();
}
static void cli_print_uptime(void)
{
    uint32_t dec = uptime_ms;
    char tmp[11U];
    dec_to_str(dec, tmp);
    usart1_transmit("Uptime: ");
    usart1_transmit(tmp);
    usart1_transmit("\r\n");
    cli_print_prompt();
}

void cli_process(void)
{
    char cmd[CLI_COMMAND_SIZE];
    cli_get_command(&rx_ring_buffer, (uint8_t*)cmd);

    if (strncmp(cmd, "led ", 4) == 0)
        cli_led_process(&cmd[4]);
    else if (strcmp(cmd, "help") == 0U)
        cli_print_help();
    else if (strcmp(cmd, "uptime") == 0U)
        cli_print_uptime();
    else if (strcmp(cmd, "reset") == 0U)
        mcu_reset();
    else
        usart1_transmit("Command not found\r\n");
}