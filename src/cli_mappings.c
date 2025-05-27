
#include <stddef.h>

#include <cli_loop.h>

#include "flow_handlers.h"
#include "data_handlers.h"
#include "info_handlers.h"

const cmd_elem_t command_mapping[] = {
    // Control Flow
    {"break", "b", break_handler},
    {"reset", NULL, reset_handler},
    {"halt", "h", halt_handler},
    {"continue", "c", continue_handler},
    {"step", "s", step_handler},
    {"next", "n", step_handler}, // Same as step 
    {"status", "stat", status_handler},
    // Data Management
    {"memory", "mem", memory_handler},
    {"register", "reg", register_handler},
    // Information
    {"information", "info", information_handler}, // TODO: Not done
    {"port", NULL, port_handler},  // TODO: Not done
    {"help", NULL, help_handler}, // TODO: Not done
    {"host", NULL, host_handler}, 
};

const int command_mapping_size = sizeof(command_mapping) / sizeof(cmd_elem_t);