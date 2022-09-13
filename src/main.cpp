#include <arduino.h>
#include <pins_arduino.h>
#include "stepconverter.h"

StepConverter converter;

#define A1_PIN 3
#define B1_PIN 2
#define A2_PIN 4
#define B2_PIN 5
#define STEP_PIN 7
#define DIR_PIN 8
#define ENABLE_PIN 9

void setup() {
  Serial.begin(115200);
  converter.init(A1_PIN, B1_PIN, A2_PIN, B2_PIN, ENABLE_PIN, STEP_PIN, DIR_PIN);
}

void loop() {
    converter.report_pattern();
}
