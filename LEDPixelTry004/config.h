#ifndef CONFIG_H
#define CONFIG_H

#include <FastLED.h>
#include "palettes.h"

// --- Feature Flags ---
// To resolve memory (RAM) errors, comment out features here.
// The Fire Pattern is disabled by default as it uses a lot of RAM.
// #define ENABLE_ONBOARD_DISPLAY
// #define ENABLE_FIRE_PATTERN
#define WEB_PREVIEW_SAMPLING_RATE 4

// --- Strip Configuration (Pins 6 & 7) ---
#define DATA_PIN 6
#define DATA_PIN_2 7
#define NUM_LEDS 600
#define NUM_SECTIONS 6
#define SECTION_LENGTH (NUM_LEDS / NUM_SECTIONS)
#define LED_TYPE_STRIP_PIN6 WS2811
#define LED_TYPE_STRIP_PIN7 WS2812B
#define COLOR_ORDER RGB
#define MAX_CURRENT_MILLIAMPS 10000
// NOTE: This is a mixed-voltage system. The lowest voltage (5V) is used for
// global power management calculations as a safety precaution.
#define POWER_VOLTAGE 5

// --- Brightness Configuration ---
#define DEFAULT_STRIP_BRIGHTNESS 255  // Safe default brightness (0-255)
#define DEFAULT_MATRIX_BRIGHTNESS 255 // Safe default brightness (0-255)

// --- Matrix Configuration (Pin 5) ---
#define MATRIX_DATA_PIN 5
#define MATRIX_NUM_LEDS 1536
#define MATRIX_LED_TYPE WS2811
#define MATRIX_COLOR_ORDER RGB
#define MATRIX_WIDTH 16
#define MATRIX_HEIGHT 96
#define MATRIX_SINGLE_PANEL_HEIGHT 48

const int SCROLL_DELAY_MS = 100;

enum DisplayMode { SECTIONAL, WHOLE };

typedef void (*SimplePatternList)(CRGB*, int);

struct DesignConfig {
  SimplePatternList pattern;
  CRGBPalette16 palette;
  uint8_t speed;
  uint8_t duration_seconds;
};

extern const DesignConfig gDesignConfigs[] PROGMEM;
extern const uint8_t gNumPatterns;

extern const char* const kPatternNames[] PROGMEM;

#endif // CONFIG_H