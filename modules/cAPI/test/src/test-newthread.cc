#include <HPGE.h>


int main () {
  for (int i = 0; i < 1000; ++i) {
    HPGE::initialize(-1,1.0,1.0);
    HPGE::start();
  }

  return 0;
}
