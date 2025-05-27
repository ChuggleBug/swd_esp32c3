
#include <Arduino.h>
#include <libswd/libswd.h>


extern swd::dap::DAP dap;
extern swd::SWDHost host;

// String equality utility
static inline bool strequal(const char *lhs, const char *rhs) { return strcasecmp(lhs, rhs) == 0; }

// Memory read and write
int handle_mem_input(int argc, char **argv) {
    if (argc < 2) {
        Serial.println("No commands provided to mem");
        return 0;
    }
    // Reading from memory
    if (argc == 3 && strequal(argv[1], "read")) {
        uint32_t addr = strtoll(argv[2], nullptr, 16);
        Serial.printf("Memory read @ 0x%08x\n\r", addr);
        host.memoryRead(addr).andThen(
            [](uint32_t data) { Serial.printf("Data read: 0x%08x\n\r", data); },
            []() { Serial.println("No data read"); });
    }
    // Writing to memory
    else if (argc == 4 && strequal(argv[1], "write")) {
        uint32_t addr = strtoll(argv[2], nullptr, 16);
        uint32_t data = strtoll(argv[3], nullptr, 16);
        Serial.printf("Memory write 0x%08x to 0x%08x\n\r", data, addr);
        if (host.memoryWrite(addr, data)) {
            Serial.println("Data writen successfully");
        } else {
            Serial.println("Data wat not written");
        }
    } else {
        // Handle case where no command was given at all
        Serial.printf("Invalid mem command: %s\n\r", argv[1]);
    }

    return 0;
}

// DAP read and write
int handle_port_input(int argc, char **argv) {
    using namespace swd::dap;

    if (argc < 2) {
        Serial.println("No commands provided to port");
        return 0;
    }

    enum class RegType { NONE, DP, AP };
    union Reg {
        DP dp;
        AP ap;
    } reg;

    RegType type = RegType::NONE;
    const char *name = argc >= 3 ? argv[2] : nullptr;

    // Match register name
    if (name) {
        // DP Registers
        if (strequal(name, "ABORT")) {
            reg.dp = DP::ABORT;
            type = RegType::DP;
        } else if (strequal(name, "IDCODE")) {
            reg.dp = DP::IDCODE;
            type = RegType::DP;
        } else if (strequal(name, "CTRL_STAT")) {
            reg.dp = DP::CTRL_STAT;
            type = RegType::DP;
        } else if (strequal(name, "WCR")) {
            reg.dp = DP::WCR;
            type = RegType::DP;
        } else if (strequal(name, "RESEND")) {
            reg.dp = DP::RESEND;
            type = RegType::DP;
        } else if (strequal(name, "SELECT")) {
            reg.dp = DP::SELECT;
            type = RegType::DP;
        } else if (strequal(name, "RDBUFF")) {
            reg.dp = DP::RDBUFF;
            type = RegType::DP;
        } else if (strequal(name, "ROUTESEL")) {
            reg.dp = DP::ROUTESEL;
            type = RegType::DP;
        }

        // AP Registers
        else if (strequal(name, "CSW")) {
            reg.ap = AP::CSW;
            type = RegType::AP;
        } else if (strequal(name, "TAR")) {
            reg.ap = AP::TAR;
            type = RegType::AP;
        } else if (strequal(name, "DRW")) {
            reg.ap = AP::DRW;
            type = RegType::AP;
        } else if (strequal(name, "DB0")) {
            reg.ap = AP::DB0;
            type = RegType::AP;
        } else if (strequal(name, "DB1")) {
            reg.ap = AP::DB1;
            type = RegType::AP;
        } else if (strequal(name, "DB2")) {
            reg.ap = AP::DB2;
            type = RegType::AP;
        } else if (strequal(name, "DB3")) {
            reg.ap = AP::DB3;
            type = RegType::AP;
        } else if (strequal(name, "CFG")) {
            reg.ap = AP::CFG;
            type = RegType::AP;
        } else if (strequal(name, "BASE")) {
            reg.ap = AP::BASE;
            type = RegType::AP;
        } else if (strequal(name, "IDR")) {
            reg.ap = AP::IDR;
            type = RegType::AP;
        }
    }

    const char *cmd = argv[1];

    if (strequal(cmd, "read") && argc == 3) {
        if (type == RegType::DP) {
            Serial.printf("Reading DP Register %s\n\r", name);
            dap.readPort(reg.dp).andThen(
                [](uint32_t data) { Serial.printf("Data read: 0x%08x\n\r", data); },
                []() { Serial.println("No data read"); });
        } else if (type == RegType::AP) {
            Serial.printf("Reading AP Register %s\n\r", name);
            dap.readPort(reg.ap).andThen(
                [](uint32_t data) { Serial.printf("Data read: 0x%08x\n\r", data); },
                []() { Serial.println("No data read"); });
        } else {
            Serial.println("Unknown register");
        }
    }

    else if (strequal(cmd, "write") && argc == 4) {
        uint32_t data = strtoll(argv[3], nullptr, 16);

        if (type == RegType::DP) {
            dap.writePort(reg.dp, data);
            Serial.printf("Wrote 0x%08x to DP register %s\n\r", data, name);
        } else if (type == RegType::AP) {
            dap.writePort(reg.ap, data);
            Serial.printf("Wrote 0x%08x to AP register %s\n\r", data, name);
        } else {
            Serial.println("Unknown register");
        }
    }

    else {
        Serial.printf("Invalid port command: %s\n\r", cmd);
    }

    return 0;
}

// Core register read and write (only when stopped)
int handle_reg_input(int argc, char **argv) {
    using namespace swd::target;
    if (argc < 2) {
        Serial.println("No commands provided to reg");
        return 0;
    }

    if (!host.isTargetHalted()) {
        Serial.println("Target it not halted");
    }

    if (argc == 3 && strequal(argv[1], "read")) {
        REG reg;
        const char *reg_name = argv[2];

        if (strequal(reg_name, "R0"))
            reg = REG::R0;
        else if (strequal(reg_name, "R1"))
            reg = REG::R1;
        else if (strequal(reg_name, "R2"))
            reg = REG::R2;
        else if (strequal(reg_name, "R3"))
            reg = REG::R3;
        else if (strequal(reg_name, "R4"))
            reg = REG::R4;
        else if (strequal(reg_name, "R5"))
            reg = REG::R5;
        else if (strequal(reg_name, "R6"))
            reg = REG::R6;
        else if (strequal(reg_name, "R7"))
            reg = REG::R7;
        else if (strequal(reg_name, "R8"))
            reg = REG::R8;
        else if (strequal(reg_name, "R9"))
            reg = REG::R9;
        else if (strequal(reg_name, "R10"))
            reg = REG::R10;
        else if (strequal(reg_name, "R11"))
            reg = REG::R11;
        else if (strequal(reg_name, "R12"))
            reg = REG::R12;
        else if (strequal(reg_name, "SP"))
            reg = REG::SP;
        else if (strequal(reg_name, "LR"))
            reg = REG::LR;
        else if (strequal(reg_name, "RETURN"))
            reg = REG::DebugReturnAddress;
        else if (strequal(reg_name, "XPSR"))
            reg = REG::xPSR;
        else if (strequal(reg_name, "MSP"))
            reg = REG::MSP;
        else if (strequal(reg_name, "PSP"))
            reg = REG::PSP;
        else if (strequal(reg_name, "CONTROL")) {
            reg = REG::CONTROL_FAULTMASK_BASEPRI_PRIMASK;
        }
        else {
            Serial.println("Unknown Register");
            return 0;
        }

        Serial.printf("Reading Register %s\n\r", reg_name);
        host.registerRead(reg).andThen(
            [](uint32_t data) { Serial.printf("Data read: 0x%08x\n\r", data); },
            []() { Serial.println("No data read"); });
    } else if (argc == 4 && strequal(argv[1], "write")) {
        REG reg;
        const char *reg_name = argv[2];
        uint32_t data = strtoll(argv[3], nullptr, 16);

        if (strequal(reg_name, "R0"))
            reg = REG::R0;
        else if (strequal(reg_name, "R1"))
            reg = REG::R1;
        else if (strequal(reg_name, "R2"))
            reg = REG::R2;
        else if (strequal(reg_name, "R3"))
            reg = REG::R3;
        else if (strequal(reg_name, "R4"))
            reg = REG::R4;
        else if (strequal(reg_name, "R5"))
            reg = REG::R5;
        else if (strequal(reg_name, "R6"))
            reg = REG::R6;
        else if (strequal(reg_name, "R7"))
            reg = REG::R7;
        else if (strequal(reg_name, "R8"))
            reg = REG::R8;
        else if (strequal(reg_name, "R9"))
            reg = REG::R9;
        else if (strequal(reg_name, "R10"))
            reg = REG::R10;
        else if (strequal(reg_name, "R11"))
            reg = REG::R11;
        else if (strequal(reg_name, "R12"))
            reg = REG::R12;
        else if (strequal(reg_name, "SP"))
            reg = REG::SP;
        else if (strequal(reg_name, "LR"))
            reg = REG::LR;
        else if (strequal(reg_name, "RETURN"))
            reg = REG::DebugReturnAddress;
        else if (strequal(reg_name, "XPSR"))
            reg = REG::xPSR;
        else if (strequal(reg_name, "MSP"))
            reg = REG::MSP;
        else if (strequal(reg_name, "PSP"))
            reg = REG::PSP;
        else if (strequal(reg_name, "CONTROL")) {
            reg = REG::CONTROL_FAULTMASK_BASEPRI_PRIMASK;
        }
        else {
            Serial.println("Unknown Register");
            return 0;
        }

        Serial.printf("Writing 0x%08x to Register %s\n\r", data, reg_name);
        if (host.registerWrite(reg, data)) {
            Serial.println("Data writen successfully");
        } else {
            Serial.println("Data wat not written");
        }
    } else {
        Serial.printf("Invalid prog command: %s\n\r", argv[1]);
    }
    return 0;
}

// Program State management (start, stop, step)
int handle_prog_input(int argc, char **argv) {
    if (argc < 2) {
        Serial.println("No commands provided to prog");
        return 0;
    }
    bool cmd_status = false;
    if (strequal(argv[1], "halt")) {
        cmd_status = host.haltTarget();
    } else if (strequal(argv[1], "step")) {
        cmd_status = host.stepTarget();
    } else if (strequal(argv[1], "reset")) {
        cmd_status = host.resetTarget();
    } else if (strequal(argv[1], "hreset")) {
        cmd_status = host.haltAfterResetTarget();
    } else if (strequal(argv[1], "continue")) {
        cmd_status = host.continueTarget();
    } else if (strequal(argv[1], "status")) {
        Serial.printf("==== Status Report ====\n\r");
        Serial.printf("     isStopped(): %s\n\r", host.isStopped() ? "true" : "false");
        Serial.printf("isTargetHalted(): %s\n\r", host.isTargetHalted() ? "true" : "false");
        Serial.printf("  isDAPStopped(): %s\n\r", host.isDAPStopped() ? "true" : "false");
        return 0;
    } else {
        Serial.printf("Invalid prog command: %s\n\r", argv[1]);
    }
    if (cmd_status) {
        Serial.println("Program command successful");
        if (strequal(argv[1], "step")) { 
            host.registerRead(swd::target::REG::DebugReturnAddress).andThen(
                [](uint32_t data) { Serial.printf("Current Instruction address: 0x%08x\n\r", data); });
        }
    } else {
        Serial.println("Program command failed");
    }

    return 0;
}