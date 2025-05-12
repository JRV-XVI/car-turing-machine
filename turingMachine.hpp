#ifndef TM_HPP
#define TM_HPP

#include "node.hpp"
#include <algorithm>
#include <iostream>
#include <vector>

class TM {
private:
  // Declaring all node
  Node *start = new Node(false);
  Node *a = new Node(false);
  Node *b = new Node(false);
  Node *c = new Node(false);
  Node *halt = new Node(true);
  Node *currentNode = start;

  // Declaring ranges from each transition
  std::pair<float, float> aRange = {0.0, 10.0};
  std::pair<float, float> bRange = {10.0, 20.0};
  std::pair<float, float> cRange = {20.0, 30.0};
  std::pair<float, float> spaceRange = {30.0, 100.0};

  // Path taken for each lecture
  std::vector<char> output;

public:
  TM() {
    // Node start transition
    start->addTransition(spaceRange, a);
    // Node a transitions
    a->addTransition(aRange, b);
    a->addTransition(bRange, a);
    a->addTransition(cRange, a);
    a->addTransition(spaceRange, halt);
    // Node b transitions
    b->addTransition(aRange, b);
    b->addTransition(bRange, c);
    b->addTransition(cRange, a);
    b->addTransition(spaceRange, halt);
    // Node c transitions
    c->addTransition(aRange, b);
    c->addTransition(bRange, a);
    c->addTransition(cRange, c);
    c->addTransition(spaceRange, halt);
  }

  std::vector<char> analyze(float distance) {
    Node *nextNode = currentNode->getNextNode(distance);
    if (nextNode != nullptr) {
      currentNode = nextNode;
      if (currentNode == a) {
        output.push_back('a');
      } else if (currentNode == b) {
        output.push_back('b');
      } else if (currentNode == c) {
        output.push_back('c');
      } else if (currentNode == halt) {
        output.push_back('_');
      }
    }
    return output;
  }

  void getOutput() {
    for (char state : output) {
      std::cout << state << std::endl;
    }
  }
};

#endif
