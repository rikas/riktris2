#include "utils.h"

void must_init(bool test, const char *description) {
  if (test) {
    return;
  }

  printf("Couldn't initialize %s\n", description);
  exit(1);
}
