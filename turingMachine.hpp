#ifndef TM_HPP
#define TM_HPP

#include "node.hpp"
#include <Arduino.h>
// Turing Machine class
class TM {
private:
  // Declaring all nodes
  Node *start;
  Node *a;
  Node *b;
  Node *c;
  Node *halt;
  Node *reject; // New reject state for "abc" detection
  Node *currentNode;

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
    a->addTransition(0.0, 20.0, b);  // 'a' symbol
    a->addTransition(20, 40.0, a); // 'b' symbol
    a->addTransition(40.0, 60.0, a); // 'c' symbol
    a->addTransition(60.0, 200.0, halt);

    // Node b transitions
    b->addTransition(0.0, 20.0, b);
    b->addTransition(20, 40.0, c); // 'b' symbol
    b->addTransition(40.0, 60.0, a); // 'c' symbol
    b->addTransition(60.0, 200.0, halt);

    // Node c transitions
    c->addTransition(0.0, 20.0, b);  // 'a' symbol
    c->addTransition(20, 40.0, a); // 'b' symbol
    c->addTransition(40.0, 60.0, c); // 'c' symbol
    c->addTransition(60.0, 200.0, halt);
  }

  // Returns the current node as a character
  char getCurrentNodeChar() {
    if (currentNode == a)
      return 'a';
    else if (currentNode == b)
      return 'b';
    else if (currentNode == c)
      return 'c';
    else if (currentNode == halt || currentNode == reject)
      return '_';
    else
      return '?';
  }

  // Returns the current state as a string
  String getCurrentState() {
    if (currentNode == start)
      return "start";
    else if (currentNode == a)
      return "a";
    else if (currentNode == b)
      return "b";
    else if (currentNode == c)
      return "c";
    else if (currentNode == halt)
      return "halt";
    else if (currentNode == reject)
      return "reject";
    else
      return "unknown";
  }

  bool analyze(float distance) {
    Node *nextNode = currentNode->getNextNode(distance);
    if (nextNode != nullptr) {
      // Store current node before transition
      Node *prevNode = currentNode;

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
          if (output[i] == 'a' && output[i + 1] == 'b' &&
              output[i + 2] == 'c') {
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
  bool isHalted() { return currentNode->isFinal; }

  // Check if the machine has rejected the input
  bool isRejected() { return currentNode->isReject; }

  // Get the complete output string so far
  String getOutputString() {
    String result = "";
    for (int i = 0; i < outputCount; i++) {
      result += output[i];
    }
    return result;
  }
};

#endif
