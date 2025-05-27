
#include <stdbool.h>
#include <stddef.h> 
#include <string.h>

#include "strutils.h"

bool starts_with(const char* str, const char* prefix) {
    if (str == NULL && prefix != NULL) {
        return false;
    }
    if (prefix == NULL) {
        return true;
    }
    if (strlen(str) < strlen(prefix)) {
        return false;
    }
    for (int i = 0; i < strlen(prefix); i++) {
        if (str[i] != prefix[i]) {
            return false;
        }
    }
    return true;
}

uint32_t parse_int(const char *str, bool *is_hex) {
    uint32_t data;
    bool hex_repr;
    if (starts_with(str, "0x")) {
        hex_repr = true;
        data = strtol(str, NULL, 16);
    } else {
        hex_repr = false;
        data = strtol(str, NULL, 10);
    }
    if (is_hex != NULL) {
        *is_hex = hex_repr;
    }
    return data;
}