
#include <libswd/libswd.h>

#include "flow_handlers.h"

extern "C" {
    #include <cli_config.h>
    #include <cli_types.h>
    #include <cli_loop.h>
}


extern swd::dap::DAP dap;
extern swd::SWDHost host;


int break_handler(int argc, const char** argv) {
    writeln("break_handler");
    return 1;
}

int reset_handler(int argc, const char** argv) {
    if (argc > 1) {
        return exec_from_table(argv[1], reset_subcmds, reset_subcmds_size, argc - 1, argv + 1);
    }
    if (host.resetTarget()) {
        writeln("Target reset");
        return 0;
    }
    writeln("Could not reset target");
    return -1;
}

int reset_catch_handler(int argc, const char** argv) {
    if (host.haltAfterResetTarget()) {
        writeln("Target reset and vector caught");
        return 0;
    }
    writeln("Could not reset target"); 
    return -1;
}  


int halt_handler(int argc, const char** argv) {
    if (host.haltTarget()) {
        writeln("Target Halted");
        return 0;
    } 
    writeln("Failed to halt target");
    return -1;    
}


int continue_handler(int argc, const char** argv) {
    if (host.continueTarget()) {
        writeln("Target continued");
        return 0;
    }
    writeln("Failed to continued target");
    return -1;
}

int step_handler(int argc, const char** argv) {
    if (!host.isTargetHalted()) {
        writeln("Cannot step a non halted target");
        return -1;
    }
    if (host.stepTarget()) {
        auto cur_pc = host.registerRead(swd::target::REG::DebugReturnAddress);
        if (cur_pc.hasValue()) {
            write_fmt("Current Instruction address: 0x%08x\n\r", cur_pc.getValue());
        } else {
            writeln("Target steped, but could not fetch instruction address");
        }
        return 0;
    } 

    return 1;
}

int status_handler(int argc, const char** argv) {
    writeln("==== Status Report ====");
    write_fmt("Host Running:   %s\n\r", host.isStopped() ? "true" : "false");
    write_fmt("Target Running: %s\n\r", host.isTargetHalted() ? "true" : "false");
    write_fmt("DAP Running:    %s\n\r", host.isDAPStopped() ? "true" : "false");
    return 0;
}
