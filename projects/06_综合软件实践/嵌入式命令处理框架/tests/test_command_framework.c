#include "command.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

static CommandResult run_line(CommandEngine *engine, const char *line,
                              char *response, size_t response_size)
{
    CommandResult result = COMMAND_EMPTY;
    size_t i;

    for (i = 0U; line[i] != '\0'; ++i) {
        assert(command_engine_feed(engine, (unsigned char)line[i]));
    }
    assert(command_engine_process(engine, response, response_size, &result));
    return result;
}

static void assert_state_equal(const DeviceState *actual, const DeviceState *expected)
{
    assert(actual->led_on == expected->led_on);
    assert(actual->mode == expected->mode);
    assert(actual->accepted_commands == expected->accepted_commands);
}

static void test_success_and_exact_capacity(void)
{
    CommandEngine engine;
    char led_response[sizeof("OK LED=ON")];
    char mode_response[sizeof("OK MODE=3")];
    char status_response[sizeof("LED=ON MODE=3 COUNT=3")];

    command_engine_init(&engine);
    assert(run_line(&engine, "led on\n", led_response, sizeof(led_response)) == COMMAND_OK);
    assert(engine.device.led_on);
    assert(strcmp(led_response, "OK LED=ON") == 0);

    assert(run_line(&engine, "mode 3\n", mode_response, sizeof(mode_response)) == COMMAND_OK);
    assert(engine.device.mode == 3U);
    assert(strcmp(mode_response, "OK MODE=3") == 0);

    assert(run_line(&engine, "status\n", status_response, sizeof(status_response)) == COMMAND_OK);
    assert(strcmp(status_response, "LED=ON MODE=3 COUNT=3") == 0);
}

static void test_response_failure_preserves_state(void)
{
    CommandEngine engine;
    DeviceState before;
    char response[64];
    char short_response[64] = "KEEP";
    unsigned i;

    command_engine_init(&engine);
    assert(run_line(&engine, "led on\n", response, sizeof(response)) == COMMAND_OK);
    assert(run_line(&engine, "mode 3\n", response, sizeof(response)) == COMMAND_OK);

    before = engine.device;
    assert(run_line(&engine, "led off\n", short_response, 1U) ==
           COMMAND_RESPONSE_TOO_SMALL);
    assert_state_equal(&engine.device, &before);
    assert(strcmp(short_response, "KEEP") == 0);

    assert(run_line(&engine, "led off\n", short_response,
                    sizeof("OK LED=OFF") - 1U) == COMMAND_RESPONSE_TOO_SMALL);
    assert_state_equal(&engine.device, &before);
    assert(strcmp(short_response, "KEEP") == 0);

    assert(run_line(&engine, "mode 2\n", short_response,
                    sizeof("OK MODE=2") - 1U) == COMMAND_RESPONSE_TOO_SMALL);
    assert_state_equal(&engine.device, &before);
    assert(strcmp(short_response, "KEEP") == 0);

    assert(run_line(&engine, "status\n", short_response, 5U) ==
           COMMAND_RESPONSE_TOO_SMALL);
    assert_state_equal(&engine.device, &before);
    assert(strcmp(short_response, "KEEP") == 0);

    assert(run_line(&engine, "mode 1\n", NULL, 0U) == COMMAND_RESPONSE_TOO_SMALL);
    assert_state_equal(&engine.device, &before);

    assert(run_line(&engine, "led off\n", short_response, 0U) ==
           COMMAND_RESPONSE_TOO_SMALL);
    assert_state_equal(&engine.device, &before);

    for (i = 0U; i < 3U; ++i) {
        assert(run_line(&engine, "led off\n", short_response, 1U) ==
               COMMAND_RESPONSE_TOO_SMALL);
        assert_state_equal(&engine.device, &before);
    }

    assert(run_line(&engine, "led off\n", response, sizeof(response)) == COMMAND_OK);
    assert(!engine.device.led_on);
    assert(engine.device.mode == before.mode);
    assert(engine.device.accepted_commands == before.accepted_commands + 1U);
}

static void test_invalid_commands_preserve_state(void)
{
    CommandEngine engine;
    DeviceState before;
    char response[] = "UNCHANGED";

    command_engine_init(&engine);
    before = engine.device;

    assert(run_line(&engine, "led\n", response, sizeof(response)) == COMMAND_BAD_ARGUMENT);
    assert_state_equal(&engine.device, &before);
    assert(run_line(&engine, "led blink\n", response, sizeof(response)) == COMMAND_BAD_ARGUMENT);
    assert_state_equal(&engine.device, &before);
    assert(run_line(&engine, "mode\n", response, sizeof(response)) == COMMAND_BAD_ARGUMENT);
    assert_state_equal(&engine.device, &before);
    assert(run_line(&engine, "mode 4\n", response, sizeof(response)) == COMMAND_BAD_ARGUMENT);
    assert_state_equal(&engine.device, &before);
    assert(run_line(&engine, "mode 2x\n", response, sizeof(response)) == COMMAND_BAD_ARGUMENT);
    assert_state_equal(&engine.device, &before);
    assert(run_line(&engine, "mode 1 extra\n", response, sizeof(response)) == COMMAND_BAD_ARGUMENT);
    assert_state_equal(&engine.device, &before);
    assert(run_line(&engine, "unknown\n", response, sizeof(response)) == COMMAND_UNKNOWN);
    assert_state_equal(&engine.device, &before);
    assert(strcmp(response, "UNCHANGED") == 0);
}

static void test_parser_regression(void)
{
    CommandEngine engine;
    char response[64];

    command_engine_init(&engine);
    assert(run_line(&engine, "\n", response, sizeof(response)) == COMMAND_EMPTY);
    assert(run_line(&engine, "     \n", response, sizeof(response)) == COMMAND_EMPTY);
    assert(run_line(&engine, "  led   on  \n", response, sizeof(response)) == COMMAND_OK);
    assert(engine.device.led_on);
    assert(run_line(&engine, " mode 2 \n", response, sizeof(response)) == COMMAND_OK);
    assert(engine.device.mode == 2U);
    assert(run_line(&engine, "status extra\n", response, sizeof(response)) == COMMAND_BAD_ARGUMENT);
    assert(run_line(&engine, "one two three four five\n", response, sizeof(response)) ==
           COMMAND_BAD_ARGUMENT);
    assert(run_line(&engine, "led\ton\n", response, sizeof(response)) == COMMAND_UNKNOWN);
}

static void test_parser_isolation(void)
{
    CommandEngine first;
    CommandEngine second;
    CommandResult result = COMMAND_EMPTY;
    char response[64];
    size_t i;
    const char first_part[] = "led ";
    const char second_part[] = "on\n";

    command_engine_init(&first);
    command_engine_init(&second);

    for (i = 0U; first_part[i] != '\0'; ++i) {
        assert(command_engine_feed(&first, (uint8_t)first_part[i]));
    }
    assert(!command_engine_process(&first, response, sizeof(response), &result));

    assert(run_line(&second, "mode 2\n", response, sizeof(response)) == COMMAND_OK);
    assert(second.device.mode == 2U);

    for (i = 0U; second_part[i] != '\0'; ++i) {
        assert(command_engine_feed(&first, (uint8_t)second_part[i]));
    }
    assert(command_engine_process(&first, response, sizeof(response), &result));
    assert(result == COMMAND_OK);
    assert(first.device.led_on);
    assert(second.device.mode == 2U);
}

static void test_input_overflow_recovery(void)
{
    CommandEngine engine;
    DeviceState before;
    CommandResult result;
    char response[64];
    char long_line[COMMAND_LINE_CAPACITY + 8U];
    size_t i;

    command_engine_init(&engine);
    before = engine.device;
    for (i = 0U; i + 2U < sizeof(long_line); ++i) {
        long_line[i] = 'x';
    }
    long_line[i++] = '\n';
    long_line[i] = '\0';

    result = run_line(&engine, long_line, response, sizeof(response));
    assert(result == COMMAND_INPUT_OVERFLOW);
    assert(strcmp(response, "ERR INPUT_OVERFLOW") == 0);
    assert_state_equal(&engine.device, &before);

    assert(run_line(&engine, "led on\n", response, sizeof(response)) == COMMAND_OK);
    assert(engine.device.led_on);
}

static void test_null_arguments(void)
{
    CommandEngine engine;
    CommandResult result = COMMAND_EMPTY;
    char response[64];
    const char line[] = "status\n";
    size_t i;

    command_engine_init(NULL);
    command_engine_init(&engine);
    assert(!command_engine_feed(NULL, (uint8_t)'x'));
    assert(!command_engine_process(NULL, response, sizeof(response), &result));
    assert(!command_engine_process(&engine, response, sizeof(response), NULL));

    for (i = 0U; line[i] != '\0'; ++i) {
        assert(command_engine_feed(&engine, (uint8_t)line[i]));
    }
    assert(!command_engine_process(&engine, response, sizeof(response), NULL));
    assert(command_engine_process(&engine, response, sizeof(response), &result));
    assert(result == COMMAND_OK);
}

static void feed_bytes(CommandEngine *engine, const uint8_t *bytes, size_t length)
{
    size_t i;
    for (i = 0U; i < length; ++i) {
        assert(command_engine_feed(engine, bytes[i]));
    }
}

static void test_embedded_nul_rejects_whole_frame_and_recovers(void)
{
    static const uint8_t nul_first[] = {0, 'l', 'e', 'd', ' ', 'o', 'n', '\n'};
    static const uint8_t nul_middle[] = {'l', 'e', 'd', ' ', 'o', 'n', 0,
                                          'g', 'a', 'r', 'b', 'a', 'g', 'e', '\n'};
    static const uint8_t nul_last[] = {'m', 'o', 'd', 'e', ' ', '3', 0, '\n'};
    static const uint8_t split_first[] = {'l', 'e', 'd', ' ', 'o', 'n', 0};
    static const uint8_t split_last[] = {'m', 'o', 'd', 'e', ' ', '3', '\n'};
    static const uint8_t two_commands[] = {'l', 'e', 'd', ' ', 'o', 'n', '\n',
                                            'm', 'o', 'd', 'e', ' ', '2', '\n'};
    const uint8_t *invalid[] = {nul_first, nul_middle, nul_last};
    const size_t lengths[] = {sizeof(nul_first), sizeof(nul_middle), sizeof(nul_last)};
    CommandEngine engine;
    DeviceState before;
    CommandResult result = COMMAND_OK;
    char response[64];
    size_t i;

    command_engine_init(&engine);
    before = engine.device;
    for (i = 0U; i < sizeof(invalid) / sizeof(invalid[0]); ++i) {
        feed_bytes(&engine, invalid[i], lengths[i]);
        assert(command_engine_process(&engine, response, sizeof(response), &result));
        assert(result == COMMAND_INVALID_BYTE);
        assert(strcmp(response, "ERR INVALID_BYTE") == 0);
        assert_state_equal(&engine.device, &before);
    }

    feed_bytes(&engine, split_first, sizeof(split_first));
    assert(!command_engine_process(&engine, response, sizeof(response), &result));
    feed_bytes(&engine, split_last, sizeof(split_last));
    assert(command_engine_process(&engine, response, sizeof(response), &result));
    assert(result == COMMAND_INVALID_BYTE);
    assert_state_equal(&engine.device, &before);

    feed_bytes(&engine, two_commands, sizeof(two_commands));
    assert(command_engine_process(&engine, response, sizeof(response), &result));
    assert(result == COMMAND_OK && engine.device.led_on);
    assert(command_engine_process(&engine, response, sizeof(response), &result));
    assert(result == COMMAND_OK && engine.device.mode == 2U);
    assert(!command_engine_process(&engine, response, sizeof(response), &result));
}

static void test_ring_overflow_discards_until_lf(void)
{
    CommandEngine engine;
    CommandResult result = COMMAND_EMPTY;
    DeviceState before;
    char response[64];
    size_t i;

    command_engine_init(&engine);
    before = engine.device;
    for (i = 0U; i < RING_BUFFER_CAPACITY; ++i) {
        assert(command_engine_feed(&engine, (uint8_t)'x'));
    }
    assert(!command_engine_feed(&engine, (uint8_t)'x'));
    assert(!command_engine_process(&engine, response, sizeof(response), &result));
    assert_state_equal(&engine.device, &before);
    assert(command_engine_feed(&engine, (uint8_t)'\n'));
    assert(command_engine_process(&engine, response, sizeof(response), &result));
    assert(result == COMMAND_INPUT_OVERFLOW);
    assert(strcmp(response, "ERR INPUT_OVERFLOW") == 0);
    assert_state_equal(&engine.device, &before);
    assert(run_line(&engine, "led on\n", response, sizeof(response)) == COMMAND_OK);
    assert(engine.device.led_on);
}

int main(void)
{
    test_success_and_exact_capacity();
    test_response_failure_preserves_state();
    test_invalid_commands_preserve_state();
    test_parser_regression();
    test_parser_isolation();
    test_input_overflow_recovery();
    test_null_arguments();
    test_embedded_nul_rejects_whole_frame_and_recovers();
    test_ring_overflow_discards_until_lf();

    puts("command framework tests passed (9 cases)");
    return 0;
}
