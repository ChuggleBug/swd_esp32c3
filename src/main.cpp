// Define the pin for the onboard LED
#include <Arduino.h>

#include "libswd/libswd.h"
#include "esp32mini_driver.h"

// Note for tm4c123gh6pm
//  - SWDIO <--> TMS
//  - SWCLK <--> TCK
// tm4c123gh6pm is set to JTAG by default
// needs to be converted
#define JTAG_DEFAULT

static Esp32Driver driver;
static swd::SWDHost host = swd::SWDHost(&driver);

void get_idcode();

uint8_t calculateParity(uint32_t data) {
    uint8_t parity = 0;
    while (data) {
        parity ^= (data & 1);  // XOR each bit
        data >>= 1;
    }
    return parity;  // Returns 1 if odd number of 1s, otherwise 0
}

void setup() {
    Serial.begin(9600);
    delay(1000);

    Serial.println("Host Begin");


    pinMode(LED, OUTPUT);
    digitalWrite(LED, HIGH);
    delay(1000);
    digitalWrite(LED, LOW);
    delay(1000);

    #ifdef JTAG_DEFAULT
        Serial.print("Send init sequence\n");
        host.init();   
    #endif
}

void loop() {
    // For now, values are hard coded

    // Read IDCode
    // Packet:
    //  - Start: 1
    //  - APnDP: 0
    //  - RnW: 1
    //  - A[2:3]: 00
    //  - Parity: 1
    //  - Stop: 0
    //  - Park: 1
    uint8_t packet = 0b10100101;

    host.sendPacket(packet);
    host.turnaround(1);
    uint32_t ack = host.readACK();
    // Reads have no turnaround
    uint32_t rdata = host.readBits(32);
    uint32_t parity = host.readBits(1);
    host.turnaround(1);

    Serial.printf("Packet send: 0x%x\n", packet);
    Serial.printf("ACK read: 0x%x\n", ack);
    if (ack == swd::ACK::OK) {
        Serial.printf("ACK was OK\n");
    }
    Serial.printf("rdata: 0x%x, parity: 0x%x\n", rdata, parity);

    // Spin since test is over
    while (1) {
        digitalWrite(LED, HIGH);
        delay(1000);
        digitalWrite(LED, LOW);
        delay(1000);
    }
}
