#include "kbd.h"
#include <assert.h>
#include <raylib.h>

int CheckKeyboard(int count, const int *list) {
  assert(count > 0);

  for (int cmd = 0; cmd < count; cmd++) {
    if (IsKeyDown(list[cmd])) {
      return cmd;
    }
  }

  return -1;
}
