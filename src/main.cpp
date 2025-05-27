
#include <ctype.h>
#include <string.h>

#include <Arduino.h>
#include <libswd/libswd.h>

extern "C" {    
    #include <cli.h>
}

#include "esp32mini_driver.h"

// Note for tm4c123gh6pm
//  - SWDIO <--> TMS
//  - SWCLK <--> TCK
// device is AHB-AP MEM-AP
// by default, DRW transactions are 8 bits. check CSW register
// by default, TAR auto are off for DRW transactions, check CSW

// tm4c123gh6pm is set to JTAG by default
// needs to be converted

// Simple class for logging
class Esp32Logger : public Logger {

  public:
    void begin(uint32_t baud) { Serial.begin(baud); }

  protected:
    virtual void write(const char *str) override { Serial.print(str); }

    virtual void flush() override { Serial.flush(); }
};

// Implementation specific classes
static Esp32Driver driver;
static Esp32Logger logger;

// High level debug
// In reality the dap should not need to be initialized here
// but is done to expose the DAP port
auto dap = swd::dap::DAP(&driver);
auto host = swd::SWDHost(&driver);

void PC_inject_tiva_example();
void PC_inject_stm32_example();
void FPB_inject_example();

extern "C" {
    extern uint8_t *tiva_data;
    extern uint32_t tiva_data_size;
    extern uint8_t *stm32_data;
    extern uint32_t stm32_data_size;
}

void setup() {  
    // Technically also Serial so two calls
    // aren't needed  
    logger.begin(9600);
    while (!Serial);

    LogLevel ll = LogLevel::INFO;

    Logger::setLogger(&logger);
    Logger::setLogLevel(ll);

    delay(3000); // Mostly time given to open the monitor screen
    Serial.println("Starting Host");

    dap.reset();
    host.reset();
}

void loop() {
    Serial.println("Entering CLI loop");

    // "exit" to run manual code
    cli_loop();
    
    Serial.println("Testing Manual Code");

    PC_inject_stm32_example();

    Serial.println("Entering CLI");       
}

void PC_inject_stm32_example() {
    // Example code inject using the PC

    // Needs to be aligned on any word address.
    // Technically Thumb-2 targets can have halfword alignment
    uint32_t base = 0x2000001C;

    Serial.printf("Writing Progam binary at 0x%08x\n\r", base);
    host.memoryWriteBlock(base, stm32_data, stm32_data_size);

    Serial.println("Halting processor");
    host.haltTarget();

    Serial.printf("Setting Program Counter to continue at 0x%08x\n\r", base);
    host.registerWrite(swd::target::REG::DebugReturnAddress, base);

    Serial.println("Continuing Processor");
    host.continueTarget();
}

void PC_inject_tiva_example() {
    // Example code inject using the PC

    // Step 1:
    // Needs to be aligned on any word address.
    // Technically Thumb-2 targets can have halfword alignment
    uint32_t base = 0x200020E0;

    // Step 2:
    Serial.printf("Writing Progam binary at 0x%08x\n\r", base);
    host.memoryWriteBlock(base, tiva_data, tiva_data_size);

    // Step 3:
    Serial.println("Halting processor");
    host.haltTarget();

    // Step 4:
    Serial.printf("Setting Program Counter to continue at 0x%08x\n\r", base);
    host.registerWrite(swd::target::REG::DebugReturnAddress, base);

    // Step 5:
    Serial.println("Continuing Processor");
    host.continueTarget();
}


// IGNORE ME
void FPB_inject_example() {
    // Needs to be aligned according to ARMv7-M Architectural Reference C1.11.4
    // In this case it is 8 word alignment


    uint32_t base = 0x200020E0;

    // Instruction address to jump from
    // this is known beforehand
    uint32_t instruction_addr = 0x2e0; 

    

    Serial.printf("Writing Progam binary at 0x%08x\n\r", base);
    host.memoryWriteBlock(base+2, tiva_data, tiva_data_size);

    Serial.println("Setting Flash Patch remap address");
    host.setRemapAddress(base);

    Serial.println("Setting remap comparator address");
    host.addRemapComparator(instruction_addr);

    Serial.println("Resetting host for good measure");
    host.reset();
}
