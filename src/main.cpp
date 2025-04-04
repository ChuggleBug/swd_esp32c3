
#include <Arduino.h>

#include <libswd/libswd.h>
#include <libswd/logger.h>
#include "esp32mini_driver.h"

// Note for tm4c123gh6pm
//  - SWDIO <--> TMS
//  - SWCLK <--> TCK
// device is AHB-AP MEM-AP
// by default, DRW transactions are 8 bits. check CSW register
// by default, TAR auto are off for DRW transactions, check CSW

// tm4c123gh6pm is set to JTAG by default
// needs to be converted

using namespace swd;

// Simple class for logging
class Esp32Logger : public Logger {

  public:

    void begin(uint32_t baud) {
        Serial.begin(baud);
    }

  protected:

    virtual void write(const char *str) override {
        Serial.print(str);
    }

    virtual void flush() override {
        Serial.flush();
    }
};

static Esp32Driver driver;
static SWDHost host = SWDHost(&driver);

static Esp32Logger logger;

void setup() {
    logger.begin(9600);
    Logger::setLogger(&logger);
    Logger::setLogLevel(LogLevel::DEBUG);
    
    // Mostly the host not writing to serial 
    // output too early 
    delay(2000);

    host.reset();
}

void loop() {
    uint32_t target = 0x20000200;
    uint32_t data = 0x80;

    // Currently the target device listens for bit 7 @ 0x20000200
    host.writePort(AP::TAR, target);
    host.writePort(AP::DRW, data);

    // Only run this test code once
    while (1);
}
