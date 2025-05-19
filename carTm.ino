// Import headers
#include "turingMachine.hpp"

/**
*
* Arduino pins configuration
* 
**/

// Distance echo pins
#define TRIG_PIN 6
#define ECHO_PIN 5

// Led pins
#define LED_A A3
#define LED_B A4
#define LED_C A5
#define LED_VALID A1
#define LED_INVALID A2
#define LED_EMPTY A0

// Engine pins
#define IN_1 10
#define IN_2 11
#define EN_A 9
#define IN_3 12
#define IN_4 13
#define EN_B 3

/**
*
* Variables
*  
**/

// Debuging analog informacion
int val = 0;
int outputValue = 0;

// Global flag to track if program has completed
bool programTerminated = false;

// Create our Turing Machine instance
TM tm;

void setup() {
  Serial.begin(9600);
  //Distance sensor
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  // Leds
  pinMode(LED_A, OUTPUT);
  pinMode(LED_B, OUTPUT);
  pinMode(LED_C, OUTPUT);
  pinMode(LED_VALID, OUTPUT);
  pinMode(LED_INVALID, OUTPUT);
  pinMode(LED_EMPTY, OUTPUT);

  // Engines
  pinMode(IN_1, OUTPUT);
  pinMode(IN_2, OUTPUT);
  pinMode(EN_A, OUTPUT);
  pinMode(IN_3, OUTPUT);
  pinMode(IN_4, OUTPUT);
  pinMode(EN_B, OUTPUT);

  // Turn off all LEDs at start
  digitalWrite(LED_A, LOW);
  digitalWrite(LED_B, LOW);
  digitalWrite(LED_C, LOW);
  digitalWrite(LED_VALID, LOW);
  digitalWrite(LED_INVALID, LOW);
  digitalWrite(LED_EMPTY, LOW);

  Serial.println("Turing Machine starting...");

  // Initial state is start - light up EMPTY LED
  digitalWrite(LED_EMPTY, HIGH);
}

// Get distance reading from ultrasonic sensor
float readDistance() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH, 30000);  // 30ms timeout
  float cm = (duration / 2.0) / 29.1;
  return cm;
}

// Update LEDs based on the current STATE, not symbol
void updateLEDs(String currentState) {
  // Turn off all LEDs first
  digitalWrite(LED_A, LOW);
  digitalWrite(LED_B, LOW);
  digitalWrite(LED_C, LOW);
  digitalWrite(LED_VALID, LOW);
  digitalWrite(LED_INVALID, LOW);
  digitalWrite(LED_EMPTY, LOW);

  // Turn on appropriate LED based on state
  if (currentState == "start") {
    // Start state - empty LED
    digitalWrite(LED_EMPTY, HIGH);
  } else if (currentState == "a") {
    // State a - LED A
    digitalWrite(LED_A, HIGH);
  } else if (currentState == "b") {
    // State b - LED B
    digitalWrite(LED_B, HIGH);
  } else if (currentState == "c") {
    // State c - LED C
    digitalWrite(LED_C, HIGH);
  } else if (currentState == "halt") {
    // Halt state - VALID LED
    digitalWrite(LED_VALID, HIGH);
  } else if (currentState == "reject") {
    // Reject state - INVALID LED
    digitalWrite(LED_INVALID, HIGH);
  }
}

void moveCar(int seconds) {
  // Move direction
  digitalWrite(IN_1, LOW);
  digitalWrite(IN_2, HIGH);
  digitalWrite(IN_3, HIGH);
  digitalWrite(IN_4, LOW);

  // Velocity
  analogWrite(EN_A, 200);
  analogWrite(EN_B, 193.3);

  // Start timer of movement
  unsigned long startTime = millis();

  // Continuar moviendo hasta que se cumpla el tiempo
  while (millis() - startTime < seconds * 1000) {}

  // Stop movement
  analogWrite(EN_A, 0);
  analogWrite(EN_B, 0);

  Serial.println("Carro detenido");
}

void loop() {
  // If already terminated, don't do anything
  if (programTerminated) {
    return;
  }

  // If already halted, show the state and stop processing
  if (tm.isHalted()) {
    // Check if rejected due to 'abc' detection
    if (tm.isRejected()) {
      // Update LEDs for reject state
      updateLEDs("reject");
      Serial.println("Machine halted - sequence rejected (abc found)");

      // Mark program as terminated to prevent further execution
      programTerminated = true;
    } else {
      // Update LEDs for halt state
      updateLEDs("halt");
      Serial.println("Machine halted - sequence accepted");

      // Mark program as terminated to prevent further execution
      programTerminated = true;
    }
    return;
  }

  // Read distance from sensor
  float cm = readDistance();

  // Log the distance for debugging
  Serial.print("Distance: ");
  Serial.print(cm);
  Serial.println(" cm");

  // Process the distance in TM
  bool transitioned = tm.analyze(cm);

  if (transitioned) {
    String currentState = tm.getCurrentState();
    char currentSymbol = tm.getCurrentNodeChar();

    // Print current state and symbol
    Serial.print("Current state: ");
    Serial.println(currentState);
    Serial.print("Symbol read: ");
    Serial.println(currentSymbol);

    // Print the entire output sequence so far
    Serial.print("Output sequence: ");
    Serial.println(tm.getOutputString());

    // Update LEDs based on current STATE, not symbol
    updateLEDs(currentState);

    moveCar(1);

    // Check if we've halted
    if (tm.isHalted()) {
      // Handle termination in the next loop iteration
      if (tm.isRejected()) {
        Serial.println("ABC pattern detected! Terminating...");
      } else {
        Serial.println("Valid sequence completed. Terminating...");
      }
    }
  }

  // Add a delay between readings
  delay(2000);
}
