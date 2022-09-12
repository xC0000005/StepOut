#include <arduino.h>
#include <pins_arduino.h>

// The coil input pins should be on the same port - this uses D2-5
#define COIL_B  4  // White wire in my setup. - D2 in my setup
#define COIL_A  8  // Red wire in my setup    - D3 in my setup
#define COIL_A2 16 // Green wire in my setup  - D4 on my setup
#define COIL_B2 32 // Black wire in my setup  - D5 on my setup
#define COIL_MASK (COIL_A | COIL_B | COIL_A2 | COIL_B2)

byte old_pin_status = 0;

void setup() {
  Serial.begin(115200);
  for (int i = 2; i < 8; i++)
      pinMode(i, INPUT);
}

void loop() {
  byte pin_status = PIND & COIL_MASK;
  if (pin_status != old_pin_status) {
    Serial.print("Port Status:");
    Serial.println(pin_status, HEX);
    if (pin_status & COIL_A)
       Serial.println("A");
    if (pin_status & COIL_B)
       Serial.println("B");
    if (pin_status & COIL_A2)
       Serial.println("A2");
    if (pin_status & COIL_B2)
       Serial.println("B2");

    old_pin_status = pin_status;
  }

  delay(2);
}