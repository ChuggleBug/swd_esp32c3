
#ifndef __FLOW_HANDLERS_H
#define __FLOW_HANDLERS_H

#include <cli_types.h>

#ifdef __cplusplus
extern "C" {
#endif

int break_handler(int argc, const char** argv);

int reset_handler(int argc, const char** argv);
int reset_catch_handler(int argc, const char** argv);
const cmd_elem_t reset_subcmds[] = {
    {"catch", "c", reset_catch_handler},
};
const int reset_subcmds_size = sizeof(reset_subcmds) / sizeof(cmd_elem_t);

int halt_handler(int argc, const char** argv);
int continue_handler(int argc, const char** argv);
int step_handler(int argc, const char** argv);
int step_handler(int argc, const char** argv);
int status_handler(int argc, const char** argv);

#ifdef __cplusplus
}
#endif

#endif // __FLOW_HANDLERS_H