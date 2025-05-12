#ifndef NODE_HPP
#define NODE_HPP

#include <utility>
#include <vector>

class Node {
public:
  // Atributes
  std::vector<std::pair<std::pair<float, float>, Node *>> transitions;
  bool isFinal;

  // Constructor
  Node(bool isFinal) : isFinal(isFinal) {}

  // Adding range distance and reference node for transition
  void addTransition(std::pair<float, float> range, Node *node) {
    transitions.push_back({range, node});
  }

  // Getting next node from an distance given, if not returning null
  Node *getNextNode(float distance) {
    for (auto &transition : transitions) {
      float minRange = transition.first.first;
      float maxRange = transition.first.second;
      if (distance > minRange && distance <= maxRange) {
        return transition.second;
      }
    }
    return nullptr;
  }
};

#endif
