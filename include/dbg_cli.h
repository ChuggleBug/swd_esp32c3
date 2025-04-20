
#ifndef __DBG_CLI_H
#define __DBG_CLI_H

#include <string.h>
#include <stdint.h>
#include <ctype.h>

#include <Arduino.h>


#define STR_BUFSZ 256
#define ARGV_MAX 10

static char str_buf[STR_BUFSZ];
static char *argv[ARGV_MAX];

// General Program flow
void cli_loop();
static int parse_args(char *str, uint32_t strsz);
static void readline(char *buf, int32_t bufsz);

// External definitions handling function
extern const char *cmd_header;
extern int handle_input(int argc, char **argv);

// General utilities
static inline bool is_newline(char c) {
    return (c == '\n') || (c == '\r');
}

void cli_loop() {
    while (1) {
        Serial.print(cmd_header);
        Serial.flush();
        readline(str_buf, STR_BUFSZ);
        int argc = parse_args(str_buf, strlen(str_buf));
        if (handle_input(argc, argv) == -1) {
            break;
        }
    }
}


static int parse_args(char *str, uint32_t strsz) {
    uint32_t str_start = 0;
    uint32_t arg_cnt = 0;
    for (uint32_t i = 0; i <= strsz; i++) {
        if (isspace(str[i]) || str[i] == '\0') {
            str[i] = '\0';
            argv[arg_cnt++] = str + str_start;
            str_start = i+1;
        }
    }
    return arg_cnt;
}

void readline(char *buf, int32_t bufsz) {
    // Read from serial until a newline
    // Additionally writes input for 
    while (!Serial.available());
    char c = Serial.read();
    uint32_t rd_cnt = 0;
    while (rd_cnt < bufsz - 1) {
        if (is_newline(c)) {
            Serial.print("\n\r");
            // Clear buffer for next read
            while (Serial.available()) {
                Serial.read();
            }
            break;
        }
        else if (c == '\b') {
            if (rd_cnt > 0) {
                rd_cnt--;
                Serial.print("\b \b");
            }
        } else {
            Serial.print(c);
            buf[rd_cnt++] = c;
        }
        while (!Serial.available());
        c = Serial.read();
    }
    buf[rd_cnt] = '\0';
}


#endif // __DBG_CLI_H