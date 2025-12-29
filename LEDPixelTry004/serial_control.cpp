#include "serial_control.h"
#include <Arduino.h>
#include "config.h"

extern void setPattern(uint8_t newPattern);

void handleSerialInput() {
    if (Serial.available()) {
        String input = Serial.readStringUntil('\n');
        input.trim();
        if (input.length() > 0) {
            int req = input.toInt();
            if (req >= 1 && req <= gNumPatterns) {
                setPattern(req - 1);
            }
        }
    }
}