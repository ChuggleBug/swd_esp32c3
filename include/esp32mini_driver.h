// esp32mini_driver.h

// libswd driver implementation for the espressif esp32 super mini

#ifndef __ESP_DRIVER_H
#define __ESP_DRIVER_H

#include <Arduino.h>
#include <libswd/host.h>

// For convenience
#define LED 8

#define SWDIO 4
#define SWCLK 5

class Esp32Driver : public swd::SWDDriver {
   public:
    Esp32Driver() {
        // Clock should always be an output by the host
        pinMode(SWCLK, OUTPUT);
        pinMode(SWDIO, OUTPUT);
        digitalWrite(SWCLK, LOW);
        digitalWrite(SWDIO, LOW);
    }

    ~Esp32Driver() {
        // No destruction required
    }

    virtual void cfgSWDIOOut() override {
        pinMode(SWDIO, OUTPUT);
    }

    virtual void cfgSWDIOIn() override {
        pinMode(SWDIO, INPUT_PULLDOWN);
    }

   protected:
    virtual uint8_t readSWDIO() override {
        return digitalRead(SWDIO);
    }

    virtual void writeSWDIO(uint8_t b) override {
        digitalWrite(SWDIO, b & 1);
    }

    void setSWCLK() override {
        digitalWrite(SWCLK, HIGH);
    }

    virtual void clearSWCLK() override {
        digitalWrite(SWCLK, LOW);
    }

    virtual void hold() override {
        delayMicroseconds(1);
    }

};

#endif // __ESP_DRIVER_H