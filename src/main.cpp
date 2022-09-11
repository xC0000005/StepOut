#include <arduino.h>
#include <pins_arduino.h>

byte old_pin_status = 0;

void setup() {
  Serial.begin(115200);
  for (int i = 1; i < 8; i++)
      pinMode(i, INPUT);
}

void loop() {
  byte pin_status = PIND;
  if (pin_status != old_pin_status) {
    Serial.print("Port Status:");
    Serial.println(pin_status, HEX);
    old_pin_status = pin_status;
  }

  delay(2);
}