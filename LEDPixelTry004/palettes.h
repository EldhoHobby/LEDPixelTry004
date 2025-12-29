#ifndef PALETTES_H
#define PALETTES_H

#include <FastLED.h>

// --- Palettes ---
extern const TProgmemRGBPalette16 HeatColors_p;
extern const TProgmemRGBPalette16 RainbowColors_p;
extern const TProgmemRGBPalette16 OceanColors_p;
#define SOLID_PAL(c) (CRGBPalette16(c, c, c, c, c, c, c, c, c, c, c, c, c, c, c, c))
const CRGBPalette16 RED_PAL = SOLID_PAL(CRGB::Red);
const CRGBPalette16 GREEN_PAL = SOLID_PAL(CRGB::Green);
const CRGBPalette16 WARM_WHITE_PAL = SOLID_PAL(CRGB(255, 200, 160));
const CRGBPalette16 CANDY_CANE_PAL = { CRGB::Red, CRGB::Red, CRGB::White, CRGB::White, CRGB::Red, CRGB::Red, CRGB::White, CRGB::White, CRGB::Red, CRGB::Red, CRGB::White, CRGB::White, CRGB::Red, CRGB::Red, CRGB::White, CRGB::White };
const TProgmemRGBPalette16 FOREST_PAL = { CRGB::DarkGreen, CRGB::ForestGreen, CRGB::DarkGreen, CRGB::LimeGreen, CRGB::ForestGreen, CRGB::LimeGreen, CRGB::DarkGreen, CRGB::Green, CRGB::DarkGreen, CRGB::ForestGreen, CRGB::DarkGreen, CRGB::LimeGreen, CRGB::ForestGreen, CRGB::LimeGreen, CRGB::DarkGreen, CRGB::Green };
const TProgmemRGBPalette16 CHRISTMAS_SPARKLE_PAL = { CRGB::Red, CRGB::Green, CRGB::White, CRGB::Red, CRGB::Green, CRGB::White, CRGB::Red, CRGB::Green, CRGB::White, CRGB::Red, CRGB::Green, CRGB::White, CRGB::Red, CRGB::Green, CRGB::White, CRGB::Red };
const TProgmemRGBPalette16 CRACKLING_FIRE_PAL = { CRGB::Red, CRGB::Orange, CRGB::Yellow, CRGB::Orange, CRGB::Red, CRGB::Red, CRGB::Orange, CRGB::Yellow, CRGB::Orange, CRGB::Red, CRGB::Orange, CRGB::Yellow, CRGB::Yellow, CRGB::Orange, CRGB::Red, CRGB::Red };
const TProgmemRGBPalette16 PRIDE_PAL = { CRGB::Red, CRGB::Orange, CRGB::Yellow, CRGB::Green, CRGB::Blue, CRGB::Purple, CRGB::Red, CRGB::Orange, CRGB::Yellow, CRGB::Green, CRGB::Blue, CRGB::Purple, CRGB::Red, CRGB::Orange, CRGB::Yellow, CRGB::Green };

#endif // PALETTES_H