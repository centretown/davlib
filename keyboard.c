#include "davlib.h"

// int navigationKeys[] = {KEY_LEFT, KEY_RIGHT, KEY_UP, KEY_PAGE_UP,
//                         KEY_PAGE_DOWN};
// size_t navigationKeysSize = sizeof(navigationKeys) /
// sizeof(navigationKeys[0]);

Vector3 KeysToVector(Vector3 vec, Vector3 base, float scale) {
  float delta = scale;
  if (IsKeyDown(KEY_LEFT)) {
    vec.x -= delta;
    return vec;
  }
  if (IsKeyDown(KEY_RIGHT)) {
    vec.x += delta;
    return vec;
  }
  if (IsKeyDown(KEY_UP)) {
    vec.y += delta;
    return vec;
  }
  if (IsKeyDown(KEY_DOWN)) {
    vec.y -= delta;
    return vec;
  }
  if (IsKeyDown(KEY_PAGE_UP)) {
    vec.z += delta;
    return vec;
  }
  if (IsKeyDown(KEY_PAGE_DOWN)) {
    vec.z -= delta;
    return vec;
  }
  if (IsKeyDown(KEY_HOME)) {
    vec.x = base.x;
    vec.y = base.y;
    vec.z = base.z;
    return vec;
  }
  return vec;
}
