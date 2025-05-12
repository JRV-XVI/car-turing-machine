#include "turingMachine.hpp"

int main() {
  TM tm;
  tm.analyze(40); 
  tm.analyze(20);
  tm.analyze(10);
  tm.analyze(40);
  tm.getOutput();
}
