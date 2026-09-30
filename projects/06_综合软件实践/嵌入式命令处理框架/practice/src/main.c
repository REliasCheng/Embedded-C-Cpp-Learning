#include "command.h"

#include <stdio.h>

int main(void)
{
    static const char input[] = "led on\nmode 2\nstatus\n";
    CommandEngine engine;
    CommandResult result;
    char response[64];
    size_t i;

    command_engine_init(&engine);
    for (i = 0U; input[i] != '\0'; ++i) {
        if (!command_engine_feed(&engine, (unsigned char)input[i])) {
            fprintf(stderr, "receive buffer full\n");
            return 1;
        }
        while (command_engine_process(&engine, response, sizeof(response), &result)) {
            printf("%s\n", response);
        }
    }
    return 0;
}
