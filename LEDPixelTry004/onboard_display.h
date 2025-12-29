#ifndef ONBOARD_DISPLAY_H
#define ONBOARD_DISPLAY_H

#include "config.h" // Needed for the feature flag

#ifdef ENABLE_ONBOARD_DISPLAY
#include <Arduino.h>

void setScrollText(const String& text);
void updateScrollingText();

#else 
// If the feature is disabled, provide empty stubs to prevent
// "function not defined" errors in the main sketch.
static inline void setScrollText(const String& text) { (void)text; }
static inline void updateScrollingText() {}

#endif // ENABLE_ONBOARD_DISPLAY
#endif // ONBOARD_DISPLAY_H
