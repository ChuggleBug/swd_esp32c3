
#ifndef __DATA_HANDLERS_H
#define __DATA_HANDLERS_H

#include <stddef.h>

#include <cli_types.h>

#ifdef __cplusplus
extern "C" {
#endif


int memory_handler(int argc, const char **argv);
int mem_write_handler(int argc, const char **argv);
int mem_read_handler(int argc, const char **argv);

const cmd_elem_t memory_subcmds[] = {
    {"write", "w", mem_write_handler},
    {"read", "r", mem_read_handler}
};
const int memory_subcmds_size = sizeof(memory_subcmds) / sizeof(cmd_elem_t);


int register_handler(int argc, const char **argv);
int reg_write_handler(int argc, const char **argv);
int reg_read_handler(int argc, const char **argv);

// Parent command checks if registers can be read
const cmd_elem_t register_subcmds[] = {
    {"write", "w", reg_write_handler},
    {"read", "r", reg_read_handler}
};
const int register_subcmds_size = sizeof(register_subcmds) / sizeof(cmd_elem_t);

#ifdef __cplusplus
}
#endif

#endif // __DATA_HANDLERS_H