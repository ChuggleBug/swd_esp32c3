#ifndef __STRUTILS_H
#define __STRUTILS_H

#include <stdint.h>

bool starts_with(const char* str, const char* prefix);

// Attempts to parse a string into a number
// an additional flag can be passed to determine if the 
// number was hexadecima
uint32_t parse_int(const char *str, bool *is_hex);


#endif // __STRUTILS_H