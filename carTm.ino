#include "turingMachine.hpp"

TM tm;

// Distance sensor
#define TRIG_PIN
#define ECHO_PIN

// Hardware logic of car
void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
  digitalWrite(LED_BUILTIN, HIGH);
  delay(500);
  digitalWrite(LED_BUILTIN, LOW);
  delay(500);
}
