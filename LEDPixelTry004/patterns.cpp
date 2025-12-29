#include "patterns.h"
#include "config.h"
#include <FastLED.h>

extern CRGB leds[NUM_LEDS];
extern uint8_t gCurrentPatternNumber;
extern uint8_t gCurrentHue;
extern uint32_t gPatternStartTime;
extern DisplayMode gDisplayMode;
extern uint8_t gPatternCycle;

void static_color(CRGB* leds, int num_leds) {
  fill_solid(leds, num_leds, ColorFromPalette(gDesignConfigs[gCurrentPatternNumber].palette, 0));
}

void pulse_breathe(CRGB* leds, int num_leds) {
  uint8_t beat = beatsin8(gDesignConfigs[gCurrentPatternNumber].speed, 50, 255);
  CRGB color = ColorFromPalette(gDesignConfigs[gCurrentPatternNumber].palette, 0);
  fill_solid(leds, num_leds, color.nscale8(beat));
}

void twinkle_random(CRGB* leds, int num_leds) {
  fadeToBlackBy(leds, num_leds, 10);
  if (random8() < gDesignConfigs[gCurrentPatternNumber].speed) {
    leds[random16(num_leds)] = ColorFromPalette(gDesignConfigs[gCurrentPatternNumber].palette, random8());
  }
}

void wave_scroll(CRGB* leds, int num_leds) {
  uint8_t off = millis() / (256 - gDesignConfigs[gCurrentPatternNumber].speed);
  for (int i = 0; i < num_leds; i++) {
    leds[i] = ColorFromPalette(gDesignConfigs[gCurrentPatternNumber].palette, (i + off) % 256);
  }
}

void glitter_dim_color(CRGB* leds, int num_leds) {
  fadeToBlackBy(leds, num_leds, 20);
  if (random8() < 80) {
    int pos = random16(num_leds);
    leds[pos] += CRGB::White;
    if (pos > 0) {
        leds[pos-1] += CRGB(150,150,150);
    }
  }
}

void color_wipe_chase(CRGB* leds, int num_leds) {
  fadeToBlackBy(leds, num_leds, 50);
  uint32_t elapsed = millis() - gPatternStartTime;
  uint16_t idx = (elapsed / (30 - gDesignConfigs[gCurrentPatternNumber].speed)) % num_leds;
  leds[idx] = ColorFromPalette(gDesignConfigs[gCurrentPatternNumber].palette, gCurrentHue++);
}

void candy_cane_chase(CRGB* leds, int num_leds) {
  uint8_t off = millis() / (256 - gDesignConfigs[gCurrentPatternNumber].speed);
  for (int i = 0; i < num_leds; i++) {
    leds[i] = ColorFromPalette(gDesignConfigs[gCurrentPatternNumber].palette, (i * 8 + off) % 256);
  }
}

void christmas_sparkle(CRGB* leds, int num_leds) {
  fadeToBlackBy(leds, num_leds, 20);
  if (random8() < gDesignConfigs[gCurrentPatternNumber].speed * 2) {
    leds[random16(num_leds)] = ColorFromPalette(gDesignConfigs[gCurrentPatternNumber].palette, random8());
  }
}

void falling_snow(CRGB* leds, int num_leds) {
  fadeToBlackBy(leds, num_leds, 10);
  if (random8() < gDesignConfigs[gCurrentPatternNumber].speed) {
    leds[random16(num_leds)] = ColorFromPalette(gDesignConfigs[gCurrentPatternNumber].palette, random8());
  }
}

void crackling_fire(CRGB* leds, int num_leds) {
  fadeToBlackBy(leds, num_leds, 55);
  if (random8() < 100) {
    int pos = random16(num_leds);
    CRGB color = ColorFromPalette(gDesignConfigs[gCurrentPatternNumber].palette, random8());
    leds[pos] = color.nscale8(random8(150, 255));
  }
}

void theater_chase(CRGB* leds, int num_leds) {
    uint32_t elapsed = millis() - gPatternStartTime;
    uint8_t f = (elapsed / (256 - gDesignConfigs[gCurrentPatternNumber].speed)) % 3;
    
    for (int i = 0; i < num_leds; i = i + 3) {
        leds[i + f] = ColorFromPalette(gDesignConfigs[gCurrentPatternNumber].palette, 0);
    }
    fadeToBlackBy(leds, num_leds, 20);
}

void sinelon(CRGB* leds, int num_leds) {
  fadeToBlackBy(leds, num_leds, 20);
  int pos = beatsin16(gDesignConfigs[gCurrentPatternNumber].speed, 0, num_leds - 1);
  leds[pos] += ColorFromPalette(gDesignConfigs[gCurrentPatternNumber].palette, gCurrentHue++);
}

void confetti(CRGB* leds, int num_leds) {
  fadeToBlackBy(leds, num_leds, 10);
  leds[random16(num_leds)] += ColorFromPalette(gDesignConfigs[gCurrentPatternNumber].palette, random8(255));
}

void noise_pattern(CRGB* leds, int num_leds) {
  for (int i = 0; i < num_leds; i++) {
    leds[i] = ColorFromPalette(gDesignConfigs[gCurrentPatternNumber].palette, inoise8(i * 30, millis() * gDesignConfigs[gCurrentPatternNumber].speed / 4));
  }
}

#ifdef ENABLE_FIRE_PATTERN
void fire_2012_revised(CRGB* leds, int num_leds) {
  static byte heat[NUM_LEDS];
  int offset = leds - ::leds;

  for (int i = 0; i < num_leds; i++) {
    int c = random8(0, (((gDesignConfigs[gCurrentPatternNumber].speed) * 10) / num_leds) + 2);
    heat[offset + i] = qsub8(heat[offset + i], c);
  }
  for (int k = num_leds - 1; k >= 2; k--) {
    heat[offset + k] = (heat[offset + k - 1] + heat[offset + k - 2] + heat[offset + k - 2]) / 3;
  }
  if (random8() < 120) {
    heat[offset + random16(num_leds)] = qadd8(heat[offset + random16(num_leds)], random8(160, 255));
  }
  for (int j = 0; j < num_leds; j++) {
    leds[j] = HeatColor(heat[offset + j]);
  }
}
#endif

#define METEOR_SIZE 10

void meteor_rain(CRGB* leds, int num_leds) {
  for (int i = 0; i < num_leds; i++) {
    leds[i].nscale8(255 - 10);
  }
  if (random8() < gDesignConfigs[gCurrentPatternNumber].speed) {
    int pos = random16(num_leds);
    CRGB color = ColorFromPalette(gDesignConfigs[gCurrentPatternNumber].palette, 0);
    leds[pos] = color;
    for(int j = 1; j <= METEOR_SIZE; j++) {
      if(pos - j >= 0) {
        leds[pos - j].nscale8(255 - (255 / (j+1)));
      }
    }
  }
}

void center_burst(CRGB* leds, int num_leds) {
  int center = num_leds / 2;
  int step = 0;
  
  fadeToBlackBy(leds, num_leds, 20);
  step = beatsin8(gDesignConfigs[gCurrentPatternNumber].speed, 0, center);
  CRGB color = ColorFromPalette(gDesignConfigs[gCurrentPatternNumber].palette, gCurrentHue + step * 2);

  if (center + step < num_leds) {
    leds[center + step] = color;
  }
  if (center - step >= 0) {
    leds[center - step] = color;
  }
  
  gCurrentHue++;
}

void strobe(CRGB* leds, int num_leds) {
  uint32_t elapsed = millis() - gPatternStartTime;
  unsigned long strobe_delay = 25 + (255 - gDesignConfigs[gCurrentPatternNumber].speed) * 4;
  bool on = (elapsed / strobe_delay) % 2 == 0;
  fill_solid(leds, num_leds, on ? CRGB::White : CRGB::Black);
}

void juggles(CRGB* leds, int num_leds) {
  fadeToBlackBy(leds, num_leds, 20);
  uint32_t elapsed = millis() - gPatternStartTime;
  byte h = 0;
  for (int i = 0; i < 8; i++) {
    leds[beatsin16(i + 7, 0, num_leds - 1, elapsed)] |= CHSV(h, 200, 255);
    h += 32;
  }
}

void police_lights(CRGB* leds, int num_leds) {
    int half = num_leds / 2;
    uint32_t elapsed = millis() - gPatternStartTime;
    unsigned long interval = (255 - gDesignConfigs[gCurrentPatternNumber].speed) * 2;
    bool s = (elapsed / interval) % 2 == 0;

    fill_solid(leds, half, s ? CRGB::Red : CRGB::Blue);
    fill_solid(leds + half, num_leds - half, s ? CRGB::Blue : CRGB::Red);
}

void rainbow_march(CRGB* leds, int num_leds) {
  gCurrentHue += gDesignConfigs[gCurrentPatternNumber].speed / 10;
  fill_rainbow(leds, num_leds, gCurrentHue, 7);
}

void fade_in_out(CRGB* leds, int num_leds) {
  uint8_t beat = beatsin8(gDesignConfigs[gCurrentPatternNumber].speed, 0, 255);
  CRGB color = ColorFromPalette(gDesignConfigs[gCurrentPatternNumber].palette, 0);
  fill_solid(leds, num_leds, color.nscale8(beat));
}

void bounce(CRGB* leds, int num_leds) {
  fadeToBlackBy(leds, num_leds, 10);
  int pos = beatsin16(gDesignConfigs[gCurrentPatternNumber].speed, 0, num_leds - 1, 0, 32767);
  leds[pos] = ColorFromPalette(gDesignConfigs[gCurrentPatternNumber].palette, 0);
}

void plasma(CRGB* leds, int num_leds) {
  int p1 = beatsin8(6, -64, 64);
  int p2 = beatsin8(7, -64, 64);
  for (int k = 0; k < num_leds; k++) {
    int c = cubicwave8((k * 23) + p1) / 2 + cos8((k * 15) + p2) / 2;
    leds[k] = ColorFromPalette(gDesignConfigs[gCurrentPatternNumber].palette, c, qsub8(c, beatsin8(7, 0, 96)), LINEARBLEND);
  }
}

void color_waves(CRGB* leds, int num_leds) {
  uint16_t ms = millis();
  for (int i = 0; i < num_leds; i++) {
    uint8_t w1 = beatsin8(9 + gDesignConfigs[gCurrentPatternNumber].speed / 10, 0, 255, ms / 10, i * 2);
    uint8_t w2 = beatsin8(11 + gDesignConfigs[gCurrentPatternNumber].speed / 10, 0, 255, ms / 10, i * 3);
    uint8_t w3 = beatsin8(13 + gDesignConfigs[gCurrentPatternNumber].speed / 10, 0, 255, ms / 10, i * 4);
    leds[i] = ColorFromPalette(gDesignConfigs[gCurrentPatternNumber].palette, (w1 + w2 + w3) / 3);
  }
}

void toggleDisplayMode() {
    if (gDisplayMode == SECTIONAL) {
        gDisplayMode = WHOLE;
    } else {
        gDisplayMode = SECTIONAL;
    }
    gPatternCycle = 0;
}

