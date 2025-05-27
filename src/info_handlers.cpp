
#include <libswd/libswd.h>

#include "info_handlers.h"

extern "C" {
    #include <cli_config.h>
    #include <cli_types.h>
    #include <cli_loop.h>
}

extern swd::dap::DAP dap;
extern swd::SWDHost host;


int information_handler(int argc, const char **argv) {
    writeln("information_handler");
    return 1;
}

int port_handler(int argc, const char **argv) {
    writeln("port_handler");
    return 1;
}

int help_handler(int argc, const char **argv) {
    writeln("help_handler");
    return 1;
}


int host_handler(int argc, const char **argv) {
    if (argc > 1) {
        return exec_from_table(argv[1], host_subcmds, host_subcmds_size, argc - 1, argv + 1);
    }
    writeln("Host Status:");
    writeln(" - Its an esp32 I guess...");
    return 0;
}
int host_reset_handler(int argc, const char **argv) {
    dap.reset(); 
    host.reset();

    writeln("Host reset. Target reset vector caught");
    return 0;
}