#include "cli.h"

/**
 * @brief Convert string to decimal number.
 * @param[in] str String with number.
 * @param[in, out] dec Pointer to the number value. Should be initialized as 0.
 */
static void str_to_dec(char const *str, uint32_t *dec)
{
    uint8_t tmp = 0U;
    while (*str >= '0' && *str <= '9')
    {
        tmp = *str - '0';
        *dec = *dec * 10U + tmp;
        str++;
    }
}

/**
 * @brief Convert number to the string.
 * @param[in] dec Number to be converted.
 * @param[out] str String. Buffer size must be at least CLI_STR_WITH_NUM_SIZE.
 */
static void dec_to_str(uint32_t dec, char *str)
{
    uint8_t len = 0U;
    char tmp_str[CLI_STR_WITH_NUM_SIZE];
    do
    {
        tmp_str[len++] = (dec % 10U) + '0';
        dec /= 10U;
    } while (dec);

    while (len)
        *str++ = tmp_str[--len];

    *str = '\0';
}

/**
 * @brief Print the CLI prompt via USART1.
 */
static void cli_print_prompt(void)
{
    usart1_transmit(">>>\r\n");
}

/**
 * @brief Get command from rx_ring_buffer.
 * @param[out] cmd Buffer where the received command will be stored.
 */
static void cli_get_command(uint8_t * cmd)
{
    //buffer is not overflow & ring buffer has data
    for (uint8_t i = 0U;
        ((i < CLI_COMMAND_SIZE) && (ring_buffer_get(&rx_ring_buffer, &cmd[i])) == RING_BUFFER_OK);
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

/**
 * @brief Change LED period.
 * @param[in] time String containing desired period value, in milliseconds.
 * @note If the user sets period less than 100 ms, or more than 10000 ms,
 * period will be set as 1000 ms.
 *
 * @warning Not implemented yet.
 */
static void cli_led_process(char * time)
{
    uint32_t dec_time = 0U;
    str_to_dec(time, &dec_time);
    if (dec_time < 100U || dec_time > 10000U)
    {
        usart1_transmit("The period value is out of range. Period set as 1000 milliseconds\r\n");
        dec_time = 1000U;
        return;
    }
    //Change LED period
}

/**
 * @brief Print help message.
 */
static void cli_print_help(void)
{
    usart1_transmit("MCU receive next commands\r\n");
    usart1_transmit("    led <time_in_ms> - set leds period as <time>\r\n");
    usart1_transmit("    help - print that message\r\n");
    usart1_transmit("    uptime - print uptime\r\n");
    usart1_transmit("    reset  - reset the system\r\n");
    cli_print_prompt();
}

/**
 * @brief Print uptime in seconds.
 */
static void cli_print_uptime(void)
{
    uint32_t dec = uptime_ms / 1000U;
    char tmp[CLI_STR_WITH_NUM_SIZE];
    dec_to_str(dec, tmp);
    usart1_transmit("Uptime: ");
    usart1_transmit(tmp);
    usart1_transmit("\r\n");
    cli_print_prompt();
}

/**
 * @brief Process received command via USART1.
 */
void cli_process(void)
{
    char cmd[CLI_COMMAND_SIZE];
    cli_get_command((uint8_t*)cmd);

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