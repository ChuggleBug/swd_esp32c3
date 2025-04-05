
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


bool write_byte(uint32_t addr, uint8_t data);
Optional<uint8_t> read_byte(uint32_t addr);

void setup() {
    logger.begin(9600);
    Logger::setLogger(&logger);
    Logger::setLogLevel(LogLevel::INFO);
    
    // Mostly the host not writing to serial 
    // output too early 
    delay(2000);

    host.reset();
}

void loop() {
    uint32_t target = 0x20000200;
    uint32_t data = 0x80;

    write_byte(target, data);
    auto byte = read_byte(target);
    
    if ( byte.hasValue() ) {
        Serial.printf("Data Read: 0x%x\n\r", byte.getValue());
    } else {
        Serial.println("No data read");
    }

    while (1);
}


bool write_byte(uint32_t addr, uint8_t data) {
    return host.writePort(AP::TAR, addr) && host.writePort(AP::DRW, (uint32_t) data);
}

Optional<uint8_t> read_byte(uint32_t addr) {
    auto data = Optional<uint32_t>::none();
    if (host.writePort(AP::TAR, addr) && ( data = host.readPort(AP::DRW)).hasValue() ) {
        return Optional<uint8_t>::of((uint8_t) data.getValue());
    }
    return Optional<uint8_t>::none();
}
