#include "onboard_display.h"
#include "config.h"

#ifdef ENABLE_ONBOARD_DISPLAY
#include "Arduino_LED_Matrix.h"

extern ArduinoLEDMatrix matrix;
extern String gScrollText; // This is not used in the new version, but keep for compatibility
extern int gScrollTextX;
extern unsigned long gLastScrollTime;

// A simple animation to show the board is working.
const uint32_t a[] = {0x0, 0x3c00, 0x0};
const uint32_t b[] = {0x0, 0x7e00, 0x0};
const uint32_t c[] = {0x0, 0xff00, 0x0};

void setScrollText(const String& text) {
    // This function is now a placeholder to avoid breaking the main sketch.
    // The new implementation uses a simple, non-blocking animation instead of text
    // to ensure stability and low memory usage.
    (void)text;
}

void updateScrollingText() {
    // This function now just cycles through a simple animation instead of scrolling text.
    if (millis() - gLastScrollTime > 300) {
        gLastScrollTime = millis();
        int frame = (gLastScrollTime / 300) % 4;
        if (frame == 0) matrix.loadFrame(a);
        if (frame == 1) matrix.loadFrame(b);
        if (frame == 2) matrix.loadFrame(c);
        if (frame == 3) matrix.loadFrame(b);
    }
}

#endif // ENABLE_ONBOARD_DISPLAY
