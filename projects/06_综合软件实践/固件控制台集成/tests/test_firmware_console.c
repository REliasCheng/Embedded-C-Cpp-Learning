#include "firmware_console.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

static CommandResult run_command(FirmwareConsole *console, const char *line,
                                 char *response, size_t response_size)
{
    CommandResult result = COMMAND_EMPTY;
    size_t i;

    for (i = 0U; line[i] != '\0'; ++i) {
        assert(firmware_console_feed(console, (uint8_t)line[i]));
    }
    assert(firmware_console_process(console, response, response_size, &result));
    return result;
}

static void assert_device_equal(const DeviceState *actual, const DeviceState *expected)
{
    assert(actual->led_on == expected->led_on);
    assert(actual->mode == expected->mode);
    assert(actual->accepted_commands == expected->accepted_commands);
}

static void test_integrated_success_path(void)
{
    FirmwareConsole console;
    char response[64];

    assert(firmware_console_init(&console));
    assert(run_command(&console, "mode 1\n", response, sizeof(response)) == COMMAND_OK);
    assert(strcmp(response, "OK MODE=1") == 0);
    assert(!console.command.device.led_on);

    firmware_console_tick(&console);
    assert(console.heartbeat_count == 0U);
    firmware_console_tick(&console);
    assert(console.heartbeat_count == 1U);
    assert(console.command.device.led_on);

    assert(run_command(&console, "led off\n", response, sizeof(response)) == COMMAND_OK);
    assert(run_command(&console, "status\n", response, sizeof(response)) == COMMAND_OK);
    assert(strcmp(response, "LED=OFF MODE=1 COUNT=3") == 0);

    assert(run_command(&console, "mode 0\n", response, sizeof(response)) == COMMAND_OK);
    firmware_console_tick(&console);
    firmware_console_tick(&console);
    assert(console.heartbeat_count == 2U);
    assert(!console.command.device.led_on);
}

static void test_command_failure_preserves_device_state(void)
{
    FirmwareConsole console;
    DeviceState before;
    char response[64];
    char short_response[] = "KEEP";

    assert(firmware_console_init(&console));
    assert(run_command(&console, "mode 2\n", response, sizeof(response)) == COMMAND_OK);
    before = console.command.device;

    assert(run_command(&console, "led on\n", short_response, 1U) ==
           COMMAND_RESPONSE_TOO_SMALL);
    assert_device_equal(&console.command.device, &before);
    assert(strcmp(short_response, "KEEP") == 0);

    assert(run_command(&console, "mode 9\n", response, sizeof(response)) ==
           COMMAND_BAD_ARGUMENT);
    assert_device_equal(&console.command.device, &before);
    assert(run_command(&console, "unknown\n", response, sizeof(response)) == COMMAND_UNKNOWN);
    assert_device_equal(&console.command.device, &before);

    assert(run_command(&console, "led on\n", response, sizeof(response)) == COMMAND_OK);
    assert(console.command.device.led_on);
}

static void test_consecutive_command_isolation(void)
{
    FirmwareConsole console;
    CommandResult result = COMMAND_EMPTY;
    char response[64];
    const char input[] = "led on\nmode 3\nstatus\n";
    size_t i;

    assert(firmware_console_init(&console));
    for (i = 0U; input[i] != '\0'; ++i) {
        assert(firmware_console_feed(&console, (uint8_t)input[i]));
    }

    assert(firmware_console_process(&console, response, sizeof(response), &result));
    assert(result == COMMAND_OK);
    assert(strcmp(response, "OK LED=ON") == 0);
    assert(firmware_console_process(&console, response, sizeof(response), &result));
    assert(result == COMMAND_OK);
    assert(strcmp(response, "OK MODE=3") == 0);
    assert(firmware_console_process(&console, response, sizeof(response), &result));
    assert(result == COMMAND_OK);
    assert(strcmp(response, "LED=ON MODE=3 COUNT=3") == 0);
    assert(!firmware_console_process(&console, response, sizeof(response), &result));
}

static void test_input_buffer_overflow_recovery(void)
{
    FirmwareConsole console;
    DeviceState before;
    CommandResult result = COMMAND_EMPTY;
    char response[64];
    size_t i;

    assert(firmware_console_init(&console));
    before = console.command.device;

    for (i = 0U; i < RING_BUFFER_CAPACITY; ++i) {
        assert(firmware_console_feed(&console, (uint8_t)'x'));
    }
    assert(!firmware_console_feed(&console, (uint8_t)'x'));
    assert(!firmware_console_process(&console, response, sizeof(response), &result));
    assert(firmware_console_feed(&console, (uint8_t)'\n'));
    assert(firmware_console_process(&console, response, sizeof(response), &result));
    assert(result == COMMAND_INPUT_OVERFLOW);
    assert(strcmp(response, "ERR INPUT_OVERFLOW") == 0);
    assert_device_equal(&console.command.device, &before);

    assert(run_command(&console, "led on\n", response, sizeof(response)) == COMMAND_OK);
    assert(console.command.device.led_on);
}

static void test_null_arguments(void)
{
    FirmwareConsole console;
    CommandResult result = COMMAND_EMPTY;
    char response[64];

    assert(!firmware_console_init(NULL));
    assert(firmware_console_init(&console));
    assert(!firmware_console_feed(NULL, (uint8_t)'x'));
    assert(!firmware_console_process(NULL, response, sizeof(response), &result));
    firmware_console_tick(NULL);
}

int main(void)
{
    test_integrated_success_path();
    test_command_failure_preserves_device_state();
    test_consecutive_command_isolation();
    test_input_buffer_overflow_recovery();
    test_null_arguments();

    puts("firmware console integration tests passed (5 cases)");
    return 0;
}
