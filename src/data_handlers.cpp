
#include <string.h>
#include <math.h>

#include <libswd/libswd.h>

#include "data_handlers.h"

extern "C" {
    #include <cli_config.h>
    #include <cli_types.h>
    #include <cli_loop.h>

    #include "strutils.h"
}

extern swd::dap::DAP dap;
extern swd::SWDHost host;

// ######
// MEMORY
// ######

int memory_handler(int argc, const char **argv) {
    if (argc < 1) {
        return -1;
    }
    return exec_from_table(argv[1], memory_subcmds, memory_subcmds_size, argc - 1, argv + 1);
}

static bool select_data_size(const char *str, bool *is_word) {
    if (strcasecmp(str, "word") == 0) {
        *is_word = true;
        return true;
    }
    if (strcasecmp(str, "byte") == 0) {
        *is_word = false;
        return true;
    }
    return false;
}

static bool parse_data_format(int argc, const char **argv, bool *is_word, int *cnt, uint32_t *base_addr, int *write_start_index) {
    if (argc < 3) {
        return false;
    }

    if (!select_data_size(argv[1], is_word)) {
        return false;
    }

    // If argv[2] was a hexadecimal formatted number, then count is 1
    bool is_hex;
    uint32_t parsed = parse_int(argv[2], &is_hex);

    if (is_hex) {
        *cnt = 1;
        *base_addr = parsed;
        if (write_start_index != NULL) {
            *write_start_index = 3;
        }
        return true;
    }

    

    // Otherwise it was the cnt
    *cnt = parsed;
    if (argc < 4) {
        return false;
    }
    *base_addr = parse_int(argv[3], NULL);

    // Write must provide a address to write to
    if (write_start_index != NULL) {
        *write_start_index = 4;
    }
    return true;
}

int mem_write_handler(int argc, const char **argv) {
    if (argc < 2) {
        
    }

    bool write_word;
    int cnt;
    uint32_t base_addr;
    int w_data_start;
    if (!parse_data_format(argc, argv, &write_word, &cnt, &base_addr, &w_data_start)) {
        return -1;
    }
    int w_data_end = (w_data_start + cnt < argc ? w_data_start + cnt : argc);
    int w_data_size = w_data_end - w_data_start;

    bool w_success = false;
    if (write_word) {
        uint32_t data[w_data_size];
        for (int i = 0; i < w_data_size; i++) {
            data[i] = parse_int(argv[w_data_start + i], NULL);
        }
        if (host.memoryWriteBlock(base_addr, data, w_data_size)) {
            w_success = true;
        }
    } else {
        uint8_t data[w_data_size];
        for (int i = 0; i < w_data_size; i++) {
            data[i] = parse_int(argv[w_data_start + i], NULL);
        }
        if (host.memoryWriteBlock(base_addr, data, w_data_size)) {
            w_success = true;
        }
    }
    if (w_success) {
        write_fmt("Wrote data ");
        if (cnt > 1) {
            write_fmt("block ");
        }
        write_fmt("to 0x%08x\n\r", base_addr);
        return 0;
    } 
    writeln("Could not write data");
    return -1;
}

int mem_read_handler(int argc, const char **argv) {
    if (argc < 2) {
        return -1;
    }

    bool fetch_word;
    int cnt;
    uint32_t base_addr;
    if (!parse_data_format(argc, argv, &fetch_word, &cnt, &base_addr, NULL)) {
        return -1;
    }

    // TODO: implement generic flow for both word and byte transfers
    // TODO: implement multiple byte reading
    // TODO: fix memory reading handlers
    if (fetch_word) {
        uint32_t data[cnt]; // TODO: implement static buffer
        if (host.memoryReadBlock(base_addr, data, cnt)) {
            write_fmt("Data from 0x%08x: ", base_addr);
            for (int i = 0; i < cnt; i++) {
                write_fmt("0x%08x ", data[i]);
            }
            writeln("");
            return 0;
        }
    } else {
        uint32_t data;
        // TODO: implement host.memoryReadBlock for byte data
        if (host.memoryReadBlock(base_addr, &data, 1)) {
            write_fmt("Data from 0x%08x: 0x%02x\n\r", base_addr, static_cast<uint8_t>(data));
            return 0;
        }
    }

    writeln("Could not read data");
    return -1;
}


// ########
// REGISTER
// ########

int register_handler(int argc, const char **argv) {
    if (!host.isTargetHalted()) {
        writeln("register: target is not halted");
        return -1;
    }
    return exec_from_table(argv[1], register_subcmds, register_subcmds_size, argc - 1, argv + 1);
}

typedef struct {
    const char* reg_name;
    swd::target::REG reg_bits;
} reg_2_bit_elem_t;

static const reg_2_bit_elem_t register_str_map[] = {
    // Core registers
    {"R0",  swd::target::REG::R0},  {"R1",  swd::target::REG::R1},  {"R2",  swd::target::REG::R2},  {"R3",  swd::target::REG::R3},
    {"R4",  swd::target::REG::R4},  {"R5",  swd::target::REG::R5},  {"R6",  swd::target::REG::R6},  {"R7",  swd::target::REG::R7},
    {"R8",  swd::target::REG::R8},  {"R9",  swd::target::REG::R9},  {"R10", swd::target::REG::R10}, {"R11", swd::target::REG::R11},
    {"R12", swd::target::REG::R12},

    // Special registers
    {"SP", swd::target::REG::SP}, {"LR", swd::target::REG::LR}, {"RETURN", swd::target::REG::DebugReturnAddress},
    {"xPSR", swd::target::REG::xPSR}, {"MSP", swd::target::REG::MSP}, {"PSP", swd::target::REG::PSP},
    {"CFBP", swd::target::REG::CONTROL_FAULTMASK_BASEPRI_PRIMASK}, {"FPSCR", swd::target::REG::FPSCR},

    // Floating-point registers
    {"S0",  swd::target::REG::S0},  {"S1",  swd::target::REG::S1},  {"S2",  swd::target::REG::S2},  {"S3",  swd::target::REG::S3},
    {"S4",  swd::target::REG::S4},  {"S5",  swd::target::REG::S5},  {"S6",  swd::target::REG::S6},  {"S7",  swd::target::REG::S7},
    {"S8",  swd::target::REG::S8},  {"S9",  swd::target::REG::S9},  {"S10", swd::target::REG::S10}, {"S11", swd::target::REG::S11},
    {"S12", swd::target::REG::S12}, {"S13", swd::target::REG::S13}, {"S14", swd::target::REG::S14}, {"S15", swd::target::REG::S15},
    {"S16", swd::target::REG::S16}, {"S17", swd::target::REG::S17}, {"S18", swd::target::REG::S18}, {"S19", swd::target::REG::S19},
    {"S20", swd::target::REG::S20}, {"S21", swd::target::REG::S21}, {"S22", swd::target::REG::S22}, {"S23", swd::target::REG::S23},
    {"S24", swd::target::REG::S24}, {"S25", swd::target::REG::S25}, {"S26", swd::target::REG::S26}, {"S27", swd::target::REG::S27},
    {"S28", swd::target::REG::S28}, {"S29", swd::target::REG::S29}, {"S30", swd::target::REG::S30}, {"S31", swd::target::REG::S31}
};

static const int register_str_map_size = sizeof(register_str_map) / sizeof(reg_2_bit_elem_t);

static bool get_register_mapping(const char *reg_name, swd::target::REG *reg) {
    if (reg_name == NULL) {
        return false;
    }
    for (int i = 0; i < register_str_map_size; i++) {
        if (strcasecmp(reg_name, register_str_map[i].reg_name) == 0) {
            if (reg != NULL) {
                *reg = register_str_map[i].reg_bits;
            }
            return true;
        }
    }
    return false;    
}

int reg_write_handler(int argc, const char **argv) {
    if (argc < 2) {
        return -1;
    }
    swd::target::REG reg;
    if (!get_register_mapping(argv[1], &reg)) {
        write_fmt("Invalid register: %s\n\r", argv[1]);
        return -1;
    }

    bool hex_repr;
    uint32_t w_data = parse_int(argv[2], &hex_repr);

    const char *fmt = hex_repr ? "0x%08x to %s\n\r" : "%d to %s\n\r";
    if (host.registerWrite(reg, w_data)) {
        write_str("Wrote ", 6);
        write_fmt(fmt, w_data, argv[1]);
        return 0;
    }
    write_str("Could not write", 15);
    write_fmt(fmt, w_data, argv[1]);
    return -1;
}
 
int reg_read_handler(int argc, const char **argv) {
    if (argc < 1) {
        return -1;
    }
    swd::target::REG reg;
    if (!get_register_mapping(argv[1], &reg)) {
        write_fmt("Invalid register: %s\n\r", argv[1]);
        return -1;
    }

    auto data = host.registerRead(reg);

    if (data.hasValue()) {
        write_fmt("Data from %s: 0x%08x\n\r", argv[1], data.getValue());
        return 0;
    }
    writeln("Could not read data");
    return -1;
}

