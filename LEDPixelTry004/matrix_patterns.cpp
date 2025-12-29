#include <FastLED.h>
#include "config.h"
#include "matrix_patterns.h"

// Reference the global CRGB array for the matrix
extern CRGB matrix_leds[MATRIX_NUM_LEDS];

// --- Frame Timing ---
static unsigned long last_update_time = 0;
const int SNOW_DELAY_MS = 50;
const int CANDY_CANE_DELAY_MS = 30;
const int HUE_SCROLL_DELAY_MS = 20;
const int CHRISTMAS_TREE_DELAY_MS = 40;
const int JOLLY_PULSE_DELAY_MS = 20;

// --- Global Pattern Control Variables ---
#define STRIPE_WIDTH 8 // For Candy Cane Scroll
static uint8_t scroll_offset = 0;
#define COLOR_SCALE 15 // For Hue Scroll
static uint8_t hue_offset = 0;

// --- Pattern Cycling Variables ---
void patternFallingSnow();
void patternCandyCaneScroll();
void patternChristmasTree();
void patternSantasJollyPulse();
void patternFestivePlaid();
void patternSwayingOrnaments();
void patternPeppermintSwirl();
void patternGoldenTinselSparkle();
void patternGiftRibbonChase();
void patternHollyBerries();
void patternGreenAndRedWave();
void patternHolidayLightsTwinkle();
void patternTwinklingChristmasLights();
void patternScrollingCandyCaneStripes();
void patternIcicleDrip();

uint8_t gMatrixCurrentPatternNumber = 0;
unsigned long gMatrixPatternStartTime;

const PatternInfo gPatterns[] = {
  { patternFallingSnow, "Falling Snow", 35, 100 },
  // { patternCandyCaneScroll, "Candy Cane Scroll", 10, 80 },
  { patternChristmasTree, "Christmas Tree", 30, 120 },
  { patternSantasJollyPulse, "Santas Jolly Pulse", 30, 100 },
  // { patternFestivePlaid, "Festive Plaid", 15, 100 },
  { patternSwayingOrnaments, "Swaying Ornaments", 80, 100 },
  // { patternPeppermintSwirl, "Peppermint Swirl", 10, 100 },
  { patternGoldenTinselSparkle, "Golden Tinsel Sparkle", 30, 100 },
  { patternGiftRibbonChase, "Gift Ribbon Chase", 35, 100 },
  //{ patternHollyBerries, "Holly Berries", 10, 100 },
  // { patternGreenAndRedWave, "Green and Red Wave", 10, 100 },
  { patternHolidayLightsTwinkle, "Holiday Lights Twinkle", 30, 100 },
  { patternTwinklingChristmasLights, "Twinkling Christmas Lights", 35, 100 },
  // { patternScrollingCandyCaneStripes, "Scrolling Candy Cane Stripes", 10, 100 },
  { patternIcicleDrip, "Icicle Drip", 55, 100 }
};

const uint8_t gMatrixNumPatterns = sizeof(gPatterns) / sizeof(gPatterns[0]);

// =======================================================
// MAPPING FUNCTIONS
// =======================================================
int XY(int x, int y) {
  if (x < 0 || x >= MATRIX_WIDTH || y < 0 || y >= MATRIX_HEIGHT) return -1;
  int panelIndex = y / MATRIX_SINGLE_PANEL_HEIGHT;
  int y_in_panel = y % MATRIX_SINGLE_PANEL_HEIGHT;
  int col = x; int startIndex; int i;

  if (panelIndex == 0) {
    startIndex = col * MATRIX_SINGLE_PANEL_HEIGHT;
    if (col % 2 != 0) { i = startIndex + (MATRIX_SINGLE_PANEL_HEIGHT - 1 - y_in_panel); }
    else { i = startIndex + y_in_panel; }
  } else {
    int offset = MATRIX_SINGLE_PANEL_HEIGHT * MATRIX_WIDTH;
    startIndex = col * MATRIX_SINGLE_PANEL_HEIGHT + offset;
    if (col % 2 != 0) { i = startIndex + y_in_panel; }
    else { i = startIndex + (MATRIX_SINGLE_PANEL_HEIGHT - 1 - y_in_panel); }
  }
  return i;
}

int IXY(int x, int y) {
    int invertedX = (MATRIX_WIDTH - 1) - x;
    return XY(invertedX, y);
}

int IXY_Corrected(int x, int y_logical) {
    int y_mapped = y_logical;
    if (y_logical >= MATRIX_SINGLE_PANEL_HEIGHT) {
        int offset_from_48 = y_logical - MATRIX_SINGLE_PANEL_HEIGHT;
        y_mapped = MATRIX_HEIGHT - 1 - offset_from_48;
    }
    return IXY(x, y_mapped);
}

// =======================================================
// PATTERN FUNCTIONS (Non-Blocking)
// =======================================================
#define FADE_RATE 20

void patternFallingSnow() {
    // Fade all pixels down slightly
    for (int i = 0; i < MATRIX_NUM_LEDS; i++) {
        matrix_leds[i].nscale8(255 - FADE_RATE);
    }

    // Every few frames, generate a new snowflake at a random x position on the bottom row (which is the top for the user)
    if (random8() < 80) {
        int x = random16(MATRIX_WIDTH);
        matrix_leds[IXY_Corrected(x, MATRIX_HEIGHT - 1)] = CHSV(0, 0, random8(128, 255));
    }

    // Move all the snowflakes up one row (which is down for the user)
    for (int y = 0; y < MATRIX_HEIGHT - 1; y++) {
        for (int x = 0; x < MATRIX_WIDTH; x++) {
            matrix_leds[IXY_Corrected(x, y)] = matrix_leds[IXY_Corrected(x, y + 1)];
        }
    }
}

void patternIcicleDrip() {
    // Shift all pixels "up" one row (which is down on the display)
    for (int y = 0; y < MATRIX_HEIGHT - 1; y++) {
        for (int x = 0; x < MATRIX_WIDTH; x++) {
            matrix_leds[IXY_Corrected(x, y)] = matrix_leds[IXY_Corrected(x, y + 1)];
        }
    }

    // Clear the bottom row
    for (int x = 0; x < MATRIX_WIDTH; x++) {
        matrix_leds[IXY_Corrected(x, MATRIX_HEIGHT - 1)] = CRGB::Black;
    }

    // Occasionally create a new drip at the "bottom" row (top of the display)
    if (random8() < 40) {
        int x = random16(MATRIX_WIDTH);
        matrix_leds[IXY_Corrected(x, MATRIX_HEIGHT - 1)] = CRGB::SkyBlue;
    }
}

void patternCandyCaneScroll() {
    scroll_offset++;
    for (int x = 0; x < MATRIX_WIDTH; x++) {
        for (int y = 0; y < MATRIX_HEIGHT; y++) {
            uint8_t stripe_pos = (x + y + scroll_offset) % STRIPE_WIDTH;
            CRGB color = (stripe_pos < (STRIPE_WIDTH / 2)) ? CRGB::Red : CRGB::White;
            matrix_leds[IXY_Corrected(x, y)] = color;
        }
    }
}

void patternChristmasTree() {
    fill_solid(matrix_leds, MATRIX_NUM_LEDS, CRGB(0, 5, 0));

    uint8_t time_offset = millis() / 20;

    for (int x = 0; x < MATRIX_WIDTH; x++) {
        for (int y = 0; y < MATRIX_HEIGHT; y++) {
            float dist_from_center = abs(x - (MATRIX_WIDTH / 2.0));
            float vertical_ratio = (float)y / MATRIX_HEIGHT;
            float max_dist_at_height = (0.5 - (0.5 * vertical_ratio)) * MATRIX_WIDTH;

            CRGB color;
            if (dist_from_center > max_dist_at_height) {
                color = CRGB(5, 2, 0);
            } else {
                uint8_t green_hue = 96;
                if (random8() < 5) {
                    uint8_t ornament_hue = time_offset + (x * 10) + (y * 5);
                    color = CHSV(ornament_hue, 255, 255);
                    if (random8() < 10) color = CRGB::White;
                } else {
                    uint8_t brightness = beatsin8(10, 100, 255, 0, y * 5);
                    color = CHSV(green_hue, 255, brightness);
                }
            }
            matrix_leds[IXY_Corrected(x, y)] += color;
        }
    }

    // Add a twinkling star at the top
    int starX = MATRIX_WIDTH / 2;
    int starY = 0;
    uint8_t starBrightness = beatsin8(7, 128, 255); // Twinkle effect
    matrix_leds[IXY_Corrected(starX, starY)] = CHSV(45, 100, starBrightness); // Yellowish star
}

void patternSantasJollyPulse() {
    float centerX = (MATRIX_WIDTH - 1) / 2.0;
    float centerY = (MATRIX_HEIGHT - 1) / 2.0;
    uint8_t pulse_wave = beatsin8(10, 0, 255, 0, 0);

    fadeToBlackBy(matrix_leds, MATRIX_NUM_LEDS, 20);

    for (int x = 0; x < MATRIX_WIDTH; x++) {
        for (int y = 0; y < MATRIX_HEIGHT; y++) {
            float distance = sqrt(pow(x - centerX, 2) + pow(y - centerY, 2));

            // Create a radiating wave
            uint8_t wave_val = sin8( (uint8_t)(distance * 10) - pulse_wave );

            if (wave_val > 200) { // Threshold to make it a ring
                CRGB color = CHSV(0, 255, wave_val); // Red pulse
                matrix_leds[IXY_Corrected(x, y)] += color;
            }
        }
    }
}

void patternFestivePlaid() {
  uint32_t time = millis() / 20;
  for (int y = 0; y < MATRIX_HEIGHT; y++) {
    for (int x = 0; x < MATRIX_WIDTH; x++) {
      uint8_t value = (sin8(x * 10 + time) + sin8(y * 10 + time)) / 2;
      if (value > 127) {
        matrix_leds[IXY_Corrected(x, y)] = CRGB::Red;
      } else {
        matrix_leds[IXY_Corrected(x, y)] = CRGB::Green;
      }
    }
  }
}

void patternSwayingOrnaments() {
  fadeToBlackBy(matrix_leds, MATRIX_NUM_LEDS, 30);
  for (int i = 0; i < 5; i++) {
    int x = beatsin8(10 + i, 0, MATRIX_WIDTH - 1);
    int y = beatsin8(12 - i, 0, MATRIX_HEIGHT - 1);
    matrix_leds[IXY_Corrected(x, y)] = CHSV(i * 50, 255, 255);
  }
}

void patternPeppermintSwirl() {
  uint8_t time = beat8(2);
  int centerX = MATRIX_WIDTH / 2;
  int centerY = MATRIX_HEIGHT / 2;

  for (int y = 0; y < MATRIX_HEIGHT; y++) {
    for (int x = 0; x < MATRIX_WIDTH; x++) {
      int x_dist = x - centerX;
      int y_dist = y - centerY;
      uint8_t angle = sin8(x_dist * 10) + cos8(y_dist * 10);
      uint16_t distance = sqrt16(x_dist * x_dist + y_dist * y_dist);
      uint8_t value = angle + distance * 5 - time;
      if (value % 64 < 32) {
        matrix_leds[IXY_Corrected(x, y)] = CRGB::Red;
      } else {
        matrix_leds[IXY_Corrected(x, y)] = CRGB::White;
      }
    }
  }
}

void patternGoldenTinselSparkle() {
  fadeToBlackBy(matrix_leds, MATRIX_NUM_LEDS, 20);
  if (MATRIX_HEIGHT < 5) return;
  if (random8() < 100) {
    int x = random16(MATRIX_WIDTH);
    int y = random16(MATRIX_HEIGHT - 5);
    for (int i = 0; i < 5; i++) {
      matrix_leds[IXY_Corrected(x, y + i)] = CRGB::Gold;
    }
  }
}

void patternGiftRibbonChase() {
  fadeToBlackBy(matrix_leds, MATRIX_NUM_LEDS, 40);

  uint8_t chase1 = beatsin8(20, 0, (MATRIX_WIDTH + MATRIX_HEIGHT) * 2 - 1);
  uint8_t chase2 = beatsin8(20, 0, (MATRIX_WIDTH + MATRIX_HEIGHT) * 2 - 1, 0, 32768);

  // Chase 1
  if (chase1 < MATRIX_WIDTH) {
    matrix_leds[IXY_Corrected(chase1, 0)] = CRGB::Red;
  } else if (chase1 < MATRIX_WIDTH + MATRIX_HEIGHT) {
    matrix_leds[IXY_Corrected(MATRIX_WIDTH - 1, chase1 - MATRIX_WIDTH)] = CRGB::Red;
  } else if (chase1 < MATRIX_WIDTH * 2 + MATRIX_HEIGHT) {
    matrix_leds[IXY_Corrected(MATRIX_WIDTH - 1 - (chase1 - (MATRIX_WIDTH + MATRIX_HEIGHT)), MATRIX_HEIGHT - 1)] = CRGB::Red;
  } else {
    matrix_leds[IXY_Corrected(0, MATRIX_HEIGHT - 1 - (chase1 - (MATRIX_WIDTH * 2 + MATRIX_HEIGHT)))] = CRGB::Red;
  }

  // Chase 2
  if (chase2 < MATRIX_WIDTH) {
    matrix_leds[IXY_Corrected(chase2, 0)] = CRGB::Gold;
  } else if (chase2 < MATRIX_WIDTH + MATRIX_HEIGHT) {
    matrix_leds[IXY_Corrected(MATRIX_WIDTH - 1, chase2 - MATRIX_WIDTH)] = CRGB::Gold;
  } else if (chase2 < MATRIX_WIDTH * 2 + MATRIX_HEIGHT) {
    matrix_leds[IXY_Corrected(MATRIX_WIDTH - 1 - (chase2 - (MATRIX_WIDTH + MATRIX_HEIGHT)), MATRIX_HEIGHT - 1)] = CRGB::Gold;
  } else {
    matrix_leds[IXY_Corrected(0, MATRIX_HEIGHT - 1 - (chase2 - (MATRIX_WIDTH * 2 + MATRIX_HEIGHT)))] = CRGB::Gold;
  }
}

void patternHollyBerries() {
  fill_solid(matrix_leds, MATRIX_NUM_LEDS, CRGB::DarkGreen);
  for (int i = 0; i < 10; i++) {
    int x = inoise8(i * 20, millis() / 400) % MATRIX_WIDTH;
    int y = inoise8(i * 20, millis() / 400 + 10000) % MATRIX_HEIGHT;
    matrix_leds[IXY_Corrected(x, y)] = CRGB::Red;
  }
}

void patternGreenAndRedWave() {
  uint32_t time = millis() / 10;
  for (int y = 0; y < MATRIX_HEIGHT; y++) {
    for (int x = 0; x < MATRIX_WIDTH; x++) {
      uint8_t value = sin8(x * 10 + y * 5 + time);
      if (value > 127) {
        matrix_leds[IXY_Corrected(x, y)] = CRGB::Red;
      } else {
        matrix_leds[IXY_Corrected(x, y)] = CRGB::Green;
      }
    }
  }
}

void patternHolidayLightsTwinkle() {
  if (random8() < 100) {
    matrix_leds[random16(MATRIX_NUM_LEDS)] = CHSV(random8(), 255, 255);
  }
  fadeToBlackBy(matrix_leds, MATRIX_NUM_LEDS, 10);
}

void patternTwinklingChristmasLights() {
  fadeToBlackBy(matrix_leds, MATRIX_NUM_LEDS, 20);
  for (int i = 0; i < 10; i++) {
    if (random8() < 50) {
      int x = random16(MATRIX_WIDTH);
      int y = (i * (MATRIX_HEIGHT / 10)) % MATRIX_HEIGHT;
      matrix_leds[IXY_Corrected(x, y)] = CHSV(random8(), 255, 255);
    }
  }
}

void patternScrollingCandyCaneStripes() {
  uint32_t time = millis() / 20;
  for (int y = 0; y < MATRIX_HEIGHT; y++) {
    for (int x = 0; x < MATRIX_WIDTH; x++) {
      uint8_t value = (x + y + time) * 10;
      if (value % 40 < 20) {
        matrix_leds[IXY_Corrected(x, y)] = CRGB::Red;
      } else {
        matrix_leds[IXY_Corrected(x, y)] = CRGB::White;
      }
    }
  }
}

// =======================================================
// PATTERN MANAGEMENT
// =======================================================
void runCurrentPattern() {
  gPatterns[gMatrixCurrentPatternNumber].pattern();
}

int getCurrentPatternDelay() {
  return gPatterns[gMatrixCurrentPatternNumber].speed;
}

void setupMatrixPatterns() {
  Serial.println("Matrix Patterns Initialized.");
  FastLED.clearData();
  gMatrixPatternStartTime = millis();
  last_update_time = millis();
}

void loopMatrixPatterns() {
    if (millis() - gMatrixPatternStartTime >= (unsigned long)gPatterns[gMatrixCurrentPatternNumber].duration * 1000) {
        FastLED.clearData();
        gMatrixCurrentPatternNumber = (gMatrixCurrentPatternNumber + 1) % gMatrixNumPatterns;
        gMatrixPatternStartTime = millis();
        Serial.print("Switching to Matrix Pattern: ");
        Serial.println(gPatterns[gMatrixCurrentPatternNumber].name);
    }

    if (millis() - last_update_time >= getCurrentPatternDelay()) {
        last_update_time = millis();
        runCurrentPattern();
    }
}

// --- Status Reporting Functions ---
const char* getMatrixPatternName() {
  return gPatterns[gMatrixCurrentPatternNumber].name;
}

void nextMatrixPatternManual() {
  gMatrixCurrentPatternNumber = (gMatrixCurrentPatternNumber + 1) % gMatrixNumPatterns;
  gMatrixPatternStartTime = millis();
}

void previousMatrixPatternManual() {
  gMatrixCurrentPatternNumber = (gMatrixCurrentPatternNumber - 1 + gMatrixNumPatterns) % gMatrixNumPatterns;
  gMatrixPatternStartTime = millis();
}

int getMatrixRemainingTime() {
  uint32_t elapsed = millis() - gMatrixPatternStartTime;
  uint32_t duration = (uint32_t)gPatterns[gMatrixCurrentPatternNumber].duration * 1000;
  return (duration > elapsed) ? (duration - elapsed) / 1000 : 0;
}
