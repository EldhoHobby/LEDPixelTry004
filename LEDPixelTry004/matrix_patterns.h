#ifndef MATRIX_PATTERNS_H
#define MATRIX_PATTERNS_H

#include <FastLED.h>

void setupMatrixPatterns();
void loopMatrixPatterns();

const char* getMatrixPatternName();
int getMatrixRemainingTime();
void nextMatrixPatternManual();
void previousMatrixPatternManual();

typedef void (*Pattern)();

struct PatternInfo {
  Pattern pattern;
  const char* name;
  uint16_t duration;
  uint8_t speed;
};

extern const PatternInfo gPatterns[];
extern const uint8_t gMatrixNumPatterns;
extern uint8_t gMatrixCurrentPatternNumber;
extern unsigned long gMatrixPatternStartTime;

#endif // MATRIX_PATTERNS_H
