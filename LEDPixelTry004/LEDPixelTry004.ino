#include <WiFiS3.h>
#include <ArduinoOTA.h>
#include <NuSock.h>
#include "arduino_secrets.h"
#include <FastLED.h>

#ifdef PORT
#undef PORT
#endif
#ifdef _PORT
#undef _PORT
#endif

#include "palettes.h"
#include "config.h"

#ifdef ENABLE_ONBOARD_DISPLAY
#include "ArduinoGraphics.h"
#include "Arduino_LED_Matrix.h"
#include "onboard_display.h"
#endif

#include "patterns.h"
#include "web_server.h"
#include "serial_control.h"
#include "matrix_patterns.h"
#include "palettes.h"

const DesignConfig gDesignConfigs[] PROGMEM = {
  { static_color, RED_PAL, 0, 15 },
  { static_color, GREEN_PAL, 0, 15 },
  { static_color, WARM_WHITE_PAL, 0, 20 },
  { pulse_breathe, RED_PAL, 5, 25 },
  { pulse_breathe, WARM_WHITE_PAL, 3, 25 },
  { twinkle_random, RainbowColors_p, 10, 30 },
  { twinkle_random, FOREST_PAL, 8, 30 },
  { wave_scroll, RainbowColors_p, 4, 35 },
  { wave_scroll, HeatColors_p, 3, 35 },
  { glitter_dim_color, RED_PAL, 10, 20 },
  { color_wipe_chase, RainbowColors_p, 11, 40 },
  { candy_cane_chase, CANDY_CANE_PAL, 12, 25 },
  { christmas_sparkle, CHRISTMAS_SPARKLE_PAL, 15, 30 },
  { falling_snow, SOLID_PAL(CRGB::White), 10, 30 },
  { crackling_fire, CRACKLING_FIRE_PAL, 20, 45 },
  { theater_chase, RED_PAL, 10, 25 },
  { sinelon, RainbowColors_p, 15, 20 },
  { confetti, RainbowColors_p, 12, 25 },
  { noise_pattern, OceanColors_p, 5, 40 },
#ifdef ENABLE_FIRE_PATTERN
  { fire_2012_revised, CRACKLING_FIRE_PAL, 15, 50 },
#endif
  { meteor_rain, WARM_WHITE_PAL, 20, 30 },
  { center_burst, RainbowColors_p, 10, 20 },
  { strobe, SOLID_PAL(CRGB::White), 20, 10 },
  { juggles, RainbowColors_p, 10, 25 },
  { police_lights, SOLID_PAL(CRGB::Blue), 15, 20 },
  { rainbow_march, RainbowColors_p, 8, 35 },
  { fade_in_out, GREEN_PAL, 5, 20 },
  { bounce, FOREST_PAL, 10, 20 },
  { plasma, OceanColors_p, 10, 40 },
  { color_waves, PRIDE_PAL, 8, 35 },
  { static_color, SOLID_PAL(CRGB::Blue), 0, 15 },
  { pulse_breathe, SOLID_PAL(CRGB::Purple), 4, 25 },
  { twinkle_random, SOLID_PAL(CRGB::Gold), 12, 30 },
  { wave_scroll, OceanColors_p, 5, 35 },
  { glitter_dim_color, SOLID_PAL(CRGB::Silver), 9, 20 }, 
  { color_wipe_chase, HeatColors_p, 9, 40 },
  { candy_cane_chase, SOLID_PAL(CRGB::Teal), 10, 25 },
  { christmas_sparkle, SOLID_PAL(CRGB::Aqua), 18, 30 },
  { falling_snow, SOLID_PAL(CRGB::LightBlue), 12, 30 },
  { crackling_fire, HeatColors_p, 18, 45 },
  { theater_chase, SOLID_PAL(CRGB::Gold), 12, 25 },
  { sinelon, SOLID_PAL(CRGB::Red), 10, 20 },
  { confetti, PRIDE_PAL, 15, 25 },
  { noise_pattern, RainbowColors_p, 4, 40 },
#ifdef ENABLE_FIRE_PATTERN
  { fire_2012_revised, HeatColors_p, 20, 50 },
#endif
  { meteor_rain, SOLID_PAL(CRGB::Red), 18, 30 },
  { center_burst, SOLID_PAL(CRGB::Green), 12, 20 },
  { strobe, SOLID_PAL(CRGB::Black), 25, 10 },
  { juggles, PRIDE_PAL, 12, 25 },
  { police_lights, SOLID_PAL(CRGB::Red), 20, 20 },
  { rainbow_march, RainbowColors_p, 6, 35 },
  { fade_in_out, SOLID_PAL(CRGB::Orange), 4, 20 },
  { bounce, SOLID_PAL(CRGB::Purple), 12, 20 },
  { plasma, HeatColors_p, 8, 40 },
  { color_waves, RainbowColors_p, 10, 35 },
  { glitter_dim_color, SOLID_PAL(CRGB::Indigo), 12, 25 }, 
  { color_wipe_chase, SOLID_PAL(CRGB::Violet), 10, 30 }
};

const uint8_t gNumPatterns = sizeof(gDesignConfigs) / sizeof(gDesignConfigs[0]);

const char* const kPatternNames[] PROGMEM = {
  "Static Red", "Static Green", "Static Warm White", "Breathe Red", "Breathe Warm White", "Twinkle Rainbow", "Twinkle Forest", "Wave Rainbow", "Wave Heat", "Glitter Red", "Wipe Rainbow", "Candy Cane", "Christmas Sparkle", "Falling Snow", "Crackling Fire", "Theater Chase Red", "Cylon Rainbow", "Confetti Rainbow", "Noise Ocean", 
#ifdef ENABLE_FIRE_PATTERN
  "Fire 2012", 
#endif
  "Meteor White", "Center Burst Rainbow", "Strobe White", "Juggles Rainbow", "Police Lights", "Rainbow March", "Fade Green", "Bounce Forest", "Plasma Ocean", "Color Waves Pride", "Static Blue", "Breathe Purple", "Twinkle Gold", "Wave Ocean", "Glitter Silver", "Wipe Heat", "Candy Cane Teal", "Sparkle Aqua", "Falling Light Blue", "Crackling Heat", "Theater Chase Gold", "Cylon Red", "Confetti Pride", "Noise Rainbow", 
#ifdef ENABLE_FIRE_PATTERN
  "Fire 2012 Heat", 
#endif
  "Meteor Red", "Center Burst Green", "Strobe Black", "Juggles Pride", "Police Lights Red", "Rainbow March Slow", "Fade Orange", "Bounce Purple", "Plasma Heat", "Color Waves Rainbow", "Glitter Indigo", "Wipe Violet"
};

#ifdef ENABLE_ONBOARD_DISPLAY
ArduinoLEDMatrix matrix;
#endif

CRGB leds[NUM_LEDS];
CRGB leds2[NUM_LEDS];
CRGB matrix_leds[MATRIX_NUM_LEDS];
uint8_t gCurrentPatternNumber = 0;
uint8_t gStripBrightness = DEFAULT_STRIP_BRIGHTNESS;
uint8_t gMatrixBrightness = DEFAULT_MATRIX_BRIGHTNESS;
uint32_t gPatternStartTime = 0;
uint8_t gCurrentHue = 0;
bool gHoldPattern = false;

DisplayMode gDisplayMode = SECTIONAL;
uint8_t gPatternCycle = 0;

String gScrollText;
int gScrollTextX = 0;
unsigned long gLastScrollTime = 0;

const unsigned long IP_PRINT_INTERVAL_MS = 30000;
unsigned long lastIpPrintTime = 0;

char ssid[] = SECRET_SSID;
char pass[] = SECRET_PASS;
char OTAPass[] = SECRET_OTA_PASS;

void setup() {
    Serial.begin(115200);
    while (!Serial) {
        ; 
    }
    delay(200);

    if (WiFi.status() == WL_NO_SHIELD) {
        Serial.println("WiFi shield not present!");
        while (true);
    }

    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    WiFi.begin(ssid, pass);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nWiFi connected");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());

    ArduinoOTA.begin(WiFi.localIP(), "HSEED_Cont1", OTAPass, InternalStorage);
    Serial.println("OTA ready. Hostname: HSEED_Cont1");

    initWebServer();
    setupMatrixPatterns();

#ifdef ENABLE_ONBOARD_DISPLAY
    matrix.begin();
    matrix.textFont(Font_5x7);
#endif

    FastLED.addLeds<LED_TYPE_STRIP_PIN6, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
    FastLED.addLeds<LED_TYPE_STRIP_PIN7, DATA_PIN_2, GRB>(leds2, NUM_LEDS).setCorrection(TypicalLEDStrip);
    FastLED.addLeds<MATRIX_LED_TYPE, MATRIX_DATA_PIN, MATRIX_COLOR_ORDER>(matrix_leds, MATRIX_NUM_LEDS).setCorrection(TypicalLEDStrip);
    
    FastLED.setBrightness(255); // Set to max; scaling is handled manually
    FastLED.setMaxPowerInVoltsAndMilliamps(POWER_VOLTAGE, MAX_CURRENT_MILLIAMPS);
    Serial.print("FastLED initialized. Max Power: ");
    Serial.print(MAX_CURRENT_MILLIAMPS / 1000.0);
    Serial.println(" Amps.");

    gPatternStartTime = millis();
#ifdef ENABLE_ONBOARD_DISPLAY
    setScrollText(String(gCurrentPatternNumber + 1));
#endif
    Serial.print("Starting Pattern ");
    Serial.print(gCurrentPatternNumber + 1);
    Serial.print(": ");
    Serial.println(kPatternNames[gCurrentPatternNumber]);
}

void loop() {
    if (millis() - lastIpPrintTime >= IP_PRINT_INTERVAL_MS) {
        Serial.print("IP Address: ");
        Serial.println(WiFi.localIP());
        lastIpPrintTime = millis();
    }

    ArduinoOTA.handle();
    handleWebServer();
    handleSerialInput();

    // WebSocket Data Push
    if (wsServer.clientCount() > 0) {
        const int num_leds_sampled = NUM_LEDS / WEB_PREVIEW_SAMPLING_RATE;
        uint8_t strip_buffer[num_leds_sampled * 3 + 1];
        strip_buffer[0] = 0; // Target: Strip
        for (int i = 0; i < num_leds_sampled; i++) {
            strip_buffer[i * 3 + 1] = leds[i * WEB_PREVIEW_SAMPLING_RATE].r;
            strip_buffer[i * 3 + 2] = leds[i * WEB_PREVIEW_SAMPLING_RATE].g;
            strip_buffer[i * 3 + 3] = leds[i * WEB_PREVIEW_SAMPLING_RATE].b;
        }
        wsServer.send(strip_buffer, sizeof(strip_buffer));

        const int sampled_width = MATRIX_WIDTH / WEB_PREVIEW_SAMPLING_RATE;
        const int sampled_height = MATRIX_HEIGHT / WEB_PREVIEW_SAMPLING_RATE;
        const int num_matrix_sampled = sampled_width * sampled_height;
        uint8_t matrix_buffer[num_matrix_sampled * 3 + 1];
        matrix_buffer[0] = 1; // Target: Matrix
        for (int y = 0; y < sampled_height; y++) {
            for (int x = 0; x < sampled_width; x++) {
                int original_x = x * WEB_PREVIEW_SAMPLING_RATE;
                int original_y = y * WEB_PREVIEW_SAMPLING_RATE;
                int index = XY(original_x, original_y);
                if (index != -1) {
                    int buffer_index = (y * sampled_width + x) * 3 + 1;
                    matrix_buffer[buffer_index] = matrix_leds[index].r;
                    matrix_buffer[buffer_index + 1] = matrix_leds[index].g;
                    matrix_buffer[buffer_index + 2] = matrix_leds[index].b;
                }
            }
        }
        wsServer.send(matrix_buffer, sizeof(matrix_buffer));
    }

#ifdef ENABLE_ONBOARD_DISPLAY
    updateScrollingText();
#endif
    loopMatrixPatterns();

    runPattern();

    // Apply manual brightness scaling
    if (gDisplayMode == WHOLE) {
        for (int i = 0; i < NUM_LEDS; i++) {
            leds[i].nscale8(gStripBrightness / 2);
        }
    } else {
        for (int i = 0; i < NUM_LEDS; i++) {
            leds[i].nscale8(gStripBrightness);
        }
    }
    for (int i = 0; i < MATRIX_NUM_LEDS; i++) {
        matrix_leds[i].nscale8(gMatrixBrightness);
    }

    memcpy(leds2, leds, sizeof(CRGB) * NUM_LEDS);
    FastLED.show();

    uint32_t current_duration_ms = (uint32_t)gDesignConfigs[gCurrentPatternNumber].duration_seconds * 1000;
    if (!gHoldPattern && millis() - gPatternStartTime >= current_duration_ms) {
        nextPattern();
    }
}

void setPattern(uint8_t newPattern) {
    if (newPattern >= 0 && newPattern < gNumPatterns) {
        gCurrentPatternNumber = newPattern;
        gPatternStartTime = millis();
        fill_solid(leds, NUM_LEDS, CRGB::Black);
#ifdef ENABLE_ONBOARD_DISPLAY
        setScrollText(String(gCurrentPatternNumber + 1));
#endif

        Serial.println("---");
        Serial.print("Pattern changed to ");
        Serial.print(gCurrentPatternNumber + 1);
        Serial.print(": ");
        Serial.println(kPatternNames[gCurrentPatternNumber]);
    }
}

void nextPatternManual() {
    setPattern((gCurrentPatternNumber + 1) % gNumPatterns);
}

void previousPatternManual() {
    setPattern((gCurrentPatternNumber - 1 + gNumPatterns) % gNumPatterns);
}

void toggleHold() {
    gHoldPattern = !gHoldPattern;
}

void runPattern() {
    if (gDisplayMode == SECTIONAL) {
        for (int i = 0; i < NUM_SECTIONS; i++) {
            gDesignConfigs[gCurrentPatternNumber].pattern(leds + (i * SECTION_LENGTH), SECTION_LENGTH);
        }
    } else {
        gDesignConfigs[gCurrentPatternNumber].pattern(leds, NUM_LEDS);
    }
}

void nextPattern() {
    gCurrentPatternNumber = (gCurrentPatternNumber + 1) % gNumPatterns;
    gPatternStartTime = millis();
    fill_solid(leds, NUM_LEDS, CRGB::Black);

    if (gCurrentPatternNumber == 0) { // A full cycle of patterns has completed
        if (gDisplayMode == SECTIONAL) {
            gPatternCycle++;
            if (gPatternCycle >= 2) {
                gDisplayMode = WHOLE;
                gPatternCycle = 0; // Reset for the next sectional run
            }
        } else { // gDisplayMode == WHOLE
            gDisplayMode = SECTIONAL;
        }
    }

#ifdef ENABLE_ONBOARD_DISPLAY
    setScrollText(String(gCurrentPatternNumber + 1));
#endif
    
    Serial.println("---");
    Serial.print("Next Pattern (Auto): ");
    Serial.print(gCurrentPatternNumber + 1);
    Serial.print(": ");
    Serial.println(kPatternNames[gCurrentPatternNumber]);
    Serial.print("Display Mode: ");
    Serial.println(gDisplayMode == SECTIONAL ? "Sectional" : "Whole");
}