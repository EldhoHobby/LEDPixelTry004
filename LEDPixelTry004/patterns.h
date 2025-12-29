#ifndef PATTERNS_H
#define PATTERNS_H

#include <FastLED.h>

// --- Animation Functions ---
void static_color(CRGB* leds, int num_leds);
void pulse_breathe(CRGB* leds, int num_leds);
void twinkle_random(CRGB* leds, int num_leds);
void wave_scroll(CRGB* leds, int num_leds);
void glitter_dim_color(CRGB* leds, int num_leds);
void color_wipe_chase(CRGB* leds, int num_leds);
void candy_cane_chase(CRGB* leds, int num_leds);
void christmas_sparkle(CRGB* leds, int num_leds);
void falling_snow(CRGB* leds, int num_leds);
void crackling_fire(CRGB* leds, int num_leds);
void theater_chase(CRGB* leds, int num_leds);
void sinelon(CRGB* leds, int num_leds);
void confetti(CRGB* leds, int num_leds);
void noise_pattern(CRGB* leds, int num_leds);
#ifdef ENABLE_FIRE_PATTERN
void fire_2012_revised(CRGB* leds, int num_leds);
#endif
void meteor_rain(CRGB* leds, int num_leds);
void center_burst(CRGB* leds, int num_leds);
void strobe(CRGB* leds, int num_leds);
void juggles(CRGB* leds, int num_leds);
void police_lights(CRGB* leds, int num_leds);
void rainbow_march(CRGB* leds, int num_leds);
void fade_in_out(CRGB* leds, int num_leds);
void bounce(CRGB* leds, int num_leds);
void plasma(CRGB* leds, int num_leds);
void color_waves(CRGB* leds, int num_leds);

void toggleDisplayMode();

#endif // PATTERNS_H

