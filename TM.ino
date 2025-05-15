#define MAX_TRANSITIONS 4  // Maximum number of transitions per state
#define MAX_OUTPUT_SIZE 100 // Maximum output sequence size

// Node class definition
class Node {
public:
  // Transition structure
  struct Transition {
    float minRange;
    float maxRange;
    Node* nextNode;
    bool isValid;
    
    Transition() : minRange(0), maxRange(0), nextNode(nullptr), isValid(false) {}
  };
  
  // Attributes
  Transition transitions[MAX_TRANSITIONS];
  int transitionCount;
  bool isFinal;
  bool isReject;  // Flag to indicate a reject state

  // Constructor
  Node(bool isFinal, bool isReject = false) : transitionCount(0), isFinal(isFinal), isReject(isReject) {
    // Initialize transitions
    for (int i = 0; i < MAX_TRANSITIONS; i++) {
      transitions[i].isValid = false;
    }
  }

  // Adding range distance and reference node for transition
  void addTransition(float minRange, float maxRange, Node* node) {
    if (transitionCount < MAX_TRANSITIONS) {
      transitions[transitionCount].minRange = minRange;
      transitions[transitionCount].maxRange = maxRange;
      transitions[transitionCount].nextNode = node;
      transitions[transitionCount].isValid = true;
      transitionCount++;
    }
  }

  // Getting next node from a distance given, if not returning null
  Node* getNextNode(float distance) {
    for (int i = 0; i < MAX_TRANSITIONS; i++) {
      if (transitions[i].isValid && 
          distance > transitions[i].minRange && 
          distance <= transitions[i].maxRange) {
        return transitions[i].nextNode;
      }
    }
    return nullptr;
  }
};

// Turing Machine class
class TM {
private:
  // Declaring all nodes
  Node* start;
  Node* a;
  Node* b;
  Node* c;
  Node* halt;
  Node* reject;    // New reject state for "abc" detection
  Node* currentNode;

  // Output array
  char output[MAX_OUTPUT_SIZE];
  int outputCount;

public:
  TM() {
    // Create nodes
    start = new Node(false);
    a = new Node(false);
    b = new Node(false);
    c = new Node(false);
    halt = new Node(true, false);  // Final accepted state
    reject = new Node(true, true); // Final rejected state
    currentNode = start;
    outputCount = 0;
    
    // Node start transition
    start->addTransition(30.0, 100.0, a);
    
    // Node a transitions
    a->addTransition(0.0, 10.0, b);    // 'a' symbol
    a->addTransition(10.0, 20.0, a);   // 'b' symbol
    a->addTransition(20.0, 30.0, a);   // 'c' symbol
    a->addTransition(30.0, 100.0, halt);
    
    // Node b transitions
    b->addTransition(0.0, 10.0, b);
    b->addTransition(10.0, 20.0, c);   // 'b' symbol
    b->addTransition(20.0, 30.0, a);   // 'c' symbol
    b->addTransition(30.0, 100.0, halt);
    
    // Node c transitions
    c->addTransition(0.0, 10.0, b);    // 'a' symbol
    c->addTransition(10.0, 20.0, a);   // 'b' symbol
    c->addTransition(20.0, 30.0, c);   // 'c' symbol
    c->addTransition(30.0, 100.0, halt);
  }

  // Returns the current node as a character
  char getCurrentNodeChar() {
    if (currentNode == a) return 'a';
    else if (currentNode == b) return 'b';
    else if (currentNode == c) return 'c';
    else if (currentNode == halt || currentNode == reject) return '_';
    else return '?';
  }
  
  // Returns the current state as a string
  String getCurrentState() {
    if (currentNode == start) return "start";
    else if (currentNode == a) return "a";
    else if (currentNode == b) return "b";
    else if (currentNode == c) return "c";
    else if (currentNode == halt) return "halt";
    else if (currentNode == reject) return "reject";
    else return "unknown";
  }

  bool analyze(float distance) {
    Node* nextNode = currentNode->getNextNode(distance);
    if (nextNode != nullptr) {
      // Store current node before transition
      Node* prevNode = currentNode;
      
      // Make the transition
      currentNode = nextNode;
      char currentSymbol = getCurrentNodeChar();
      
      if (outputCount < MAX_OUTPUT_SIZE) {
        output[outputCount++] = currentSymbol;
      }
      
      // Only reject if:
      // 1. We were in state C
      // 2. We're still in state C (self-loop)
      // 3. The output contains "abc"
      if (prevNode == c && currentNode == c) {
        // Check for "abc" pattern in the entire output
        for (int i = 0; i < outputCount - 2; i++) {
          if (output[i] == 'a' && 
              output[i+1] == 'b' && 
              output[i+2] == 'c') {
            currentNode = reject;
            break;
          }
        }
      }
      
      return true;
    }
    return false;
  }

  // Check if the machine has halted
  bool isHalted() {
    return currentNode->isFinal;
  }
  
  // Check if the machine has rejected the input
  bool isRejected() {
    return currentNode->isReject;
  }
  
  // Get the complete output string so far
  String getOutputString() {
    String result = "";
    for (int i = 0; i < outputCount; i++) {
      result += output[i];
    }
    return result;
  }
};

// Arduino pins configuration
#define TRIG_PIN 6
#define ECHO_PIN 5

#define LED_A A3
#define LED_B A4
#define LED_C A5
#define LED_VALID A1
#define LED_INVALID A2
#define LED_EMPTY A0

// Global flag to track if program has completed
bool programTerminated = false;

// Create our Turing Machine instance
TM tm;

void setup() {
  Serial.begin(9600);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  pinMode(LED_A, OUTPUT);
  pinMode(LED_B, OUTPUT);
  pinMode(LED_C, OUTPUT);
  pinMode(LED_VALID, OUTPUT);
  pinMode(LED_INVALID, OUTPUT);
  pinMode(LED_EMPTY, OUTPUT);

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
  }
  else if (currentState == "a") {
    // State a - LED A
    digitalWrite(LED_A, HIGH);
  }
  else if (currentState == "b") {
    // State b - LED B
    digitalWrite(LED_B, HIGH);
  }
  else if (currentState == "c") {
    // State c - LED C
    digitalWrite(LED_C, HIGH);
  }
  else if (currentState == "halt") {
    // Halt state - VALID LED
    digitalWrite(LED_VALID, HIGH);
  }
  else if (currentState == "reject") {
    // Reject state - INVALID LED
    digitalWrite(LED_INVALID, HIGH);
  }
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