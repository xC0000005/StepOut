#include <arduino.h>
#include <pins_arduino.h>
#include "stepconverter.h"

StepConverter converter;

// On my particular setup:
// D5    D4    D3    D2
// B2    A2    A1    B1

#define A1_PIN 3
#define B1_PIN 2
#define A2_PIN 4
#define B2_PIN 5

#define STEP_PIN A4
#define DIR_PIN A5
#define ENABLE_PIN A3

#define STEP_LED_PIN 10
#define DIR_LED_PIN 11
#define ENABLE_LED_PIN 12

void setup() {
  Serial.begin(115200);
  converter.init(A1_PIN, B1_PIN, A2_PIN, B2_PIN, ENABLE_PIN, STEP_PIN, DIR_PIN);
}

void loop() {
    pinMode(ENABLE_PIN, OUTPUT);
    pinMode(STEP_PIN, OUTPUT);
    pinMode(DIR_PIN, OUTPUT);

    //digitalWrite(ENABLE_PIN, HIGH);
    bool dir = 0;

    for(;;) {
        //digitalWrite(DIR_PIN, dir);
        for (int i = 0; i < 400; i++) {
            digitalWrite(STEP_PIN, HIGH);
            delay(2);
            digitalWrite(STEP_PIN, LOW);
            delay(2);
        }
        if (dir == 0)
            dir = 1;
        else
            dir = 0;
    }
    //converter.monitor_and_step();
}
