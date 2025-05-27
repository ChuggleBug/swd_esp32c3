
#ifndef __INFO_HANDLERS_H
#define __INFO_HANDLERS_H

#include <cli_types.h>

#ifdef __cplusplus
extern "C" {
#endif

int information_handler(int argc, const char **argv);
int port_handler(int argc, const char **argv);
int help_handler(int argc, const char **argv);

int host_handler(int argc, const char **argv); 
int host_reset_handler(int argc, const char **argv);

const cmd_elem_t host_subcmds[] = {
    {"reset", NULL, host_reset_handler},
};
const int host_subcmds_size = sizeof(host_subcmds) / sizeof(cmd_elem_t);

#ifdef __cplusplus
}
#endif

#endif // __INFO_HANDLERS_H