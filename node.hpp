#ifndef NODE_HPP
#define NODE_HPP
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

#endif
