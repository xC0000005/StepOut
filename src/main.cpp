#include <arduino.h>
#include <pins_arduino.h>

// The coil input pins should be on the same port - this uses D2-5
#define COIL_B  4  // White wire in my setup. - D2 in my setup
#define COIL_A  8  // Red wire in my setup    - D3 in my setup
#define COIL_A2 16 // Green wire in my setup  - D4 on my setup
#define COIL_B2 32 // Black wire in my setup  - D5 on my setup
#define COIL_MASK (COIL_A | COIL_B | COIL_A2 | COIL_B2)

#define SAMPLE_BUFFER_SIZE 1024

byte sample_buffer[SAMPLE_BUFFER_SIZE] = {0};
int next_sample = 0;

void setup() {
  Serial.begin(115200);

  // This is very tied to my exact wiring.
  for (int i = 2; i < 5; i++)
      pinMode(i, INPUT);
}

void loop() {
  byte pin_status, old_pin_status = 0;

  // First run the sampling loop
  do {
    pin_status = PIND & COIL_MASK;
    if (pin_status != old_pin_status) {
        sample_buffer[next_sample++] = pin_status;
        old_pin_status = pin_status;
    }
  } 
  while (next_sample < SAMPLE_BUFFER_SIZE - 1);

  Serial.print("%i samples collected.\r\n");
  Serial.println(SAMPLE_BUFFER_SIZE, HEX);

  delay(2);

  for (int k = 0; k < SAMPLE_BUFFER_SIZE; k++) {
    byte pin_status = sample_buffer[k];

    Serial.print("Port:");
    Serial.println(pin_status, HEX);
    if (pin_status & COIL_A)
    Serial.println("A");
    if (pin_status & COIL_B)
    Serial.println("B");
    if (pin_status & COIL_A2)
    Serial.println("A2");
    if (pin_status & COIL_B2)
    Serial.println("B2");
  }
}