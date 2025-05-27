
#include <stdbool.h>
#include <stdarg.h>
#include <stdio.h>

#include <Arduino.h>

extern "C" {
    #include <cli_config.h>
}

const char *SHELL_PROMPT = "swd $ "; 

void write_str(const char* str, int cnt) {
    Serial.print(str);
    Serial.flush();
}

void write_char(char c) {
    Serial.print(c);
    Serial.flush();
}

void write_fmt(const char* fmt, ...) {
    char buffer[256];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buffer, sizeof(buffer), fmt, args);
    va_end(args);
    write_str(buffer, strlen(buffer));
}

bool read_ready() {
    return Serial.available();
}

char read_char() {
    return Serial.read();
}

void clear_read_buf() {
    while (Serial.available()) {
        Serial.read();
    }
}