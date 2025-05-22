#ifndef TM_HPP
#define TM_HPP

#include "node.hpp"
#include <Arduino.h>
// Turing Machine class
class TM {
private:
  // Declaring all nodes
  Node *w;
  Node *g;
  Node *y;
  Node *r;
  Node *b;
  Node *o; // New reject state for "abc" detection
  Node *currentNode;

  // Output array
  char output[MAX_OUTPUT_SIZE];
  int outputCount;

public:
  TM() {
    // Create nodes
    w = new Node(false);
    g = new Node(false);
    y = new Node(false);
    r = new Node(false);
    b = new Node(true, false); // Final accepted state
    o = new Node(true, true);  // Final rejected state
    currentNode = w;
    outputCount = 0;

    // Transition range distance
    float aTransition[2] = {0.0, 20.0};
    float bTransition[2] = {20.0, 40.0};
    float cTransition[2] = {40.0, 60.0};
    float spaceTransition[2] = {60.0, 200.0};

    // Node start transition
    w->addTransition(spaceTransition, g);

    // Node a transitions
    g->addTransition(aTransition, y);
    g->addTransition(bTransition, g); 
    g->addTransition(cTransition, g); 
    g->addTransition(spaceTransition, b);

    // Node b transitions
    y->addTransition(aTransition, y);
    y->addTransition(bTransition, r); 
    y->addTransition(cTransition, g); 
    y->addTransition(spaceTransition, b);

    // Node c transitions
    r->addTransition(aTransition, y); 
    r->addTransition(bTransition, g); 
    r->addTransition(cTransition, o); 
    r->addTransition(spaceTransition, b);
  }

  // Returns the current node as a character
  char getCurrentNodeChar() {
    if (currentNode == g)
      return 'g';
    else if (currentNode == y)
      return 'y';
    else if (currentNode == r)
      return 'r';
    else if (currentNode == b || currentNode == o)
      return '_';
    else
      return '?';
  }

  // Returns the current state as a string
  String getCurrentState() {
    if (currentNode == w)
      return "white";
    else if (currentNode == g)
      return "green";
    else if (currentNode == y)
      return "yellow";
    else if (currentNode == r)
      return "red";
    else if (currentNode == b)
      return "blue";
    else if (currentNode == o)
      return "orange";
    else
      return "unknown";
  }

  bool analyze(float distance) {
    Node *nextNode = currentNode->getNextNode(distance);
    if (nextNode != nullptr) {
      currentNode = nextNode;
      char currentSymbol = getCurrentNodeChar();

      if (outputCount < MAX_OUTPUT_SIZE) {
        output[outputCount++] = currentSymbol;
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
