#include "../inc/emulator.hpp"
int main(int argc, char* argv[]) {
  string ulazniFajl = argv[1];
  Emulator emulator(ulazniFajl);
  emulator.pokreni();
  return 0;
}