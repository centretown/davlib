#include "vec.h"

#include <assert.h>
#include <raylib.h>
#include <stdbool.h>
#include <string.h>

Pad GamePads[4] = {0};
const size_t maxGamePads = sizeof(GamePads) / sizeof(GamePads[0]);

const char *noneDefined = "undefined";
const char *buttonNames[] = {
    "unknown",         "left face up",   "left face right",  "left face down",
    "left face left",  "right face up",  "right face right", "right face down",
    "right face left", "left trigger 1", "left trigger 2",   "right trigger 1",
    "right trigger 2", "middle left",    "middle",           "middle right",
    "left thumb",      "right thumb",
};
const size_t maxButtons = sizeof(buttonNames) / sizeof(buttonNames[0]);
const char *ButtonName(int button) {
  if (button < 0 || button >= maxButtons)
    return noneDefined;
  return buttonNames[button];
}

const char *axisNames[] = {
    "left x", "left y", "right x", "right y", "left trigger", "right trigger",
};
size_t maxAxes = sizeof(axisNames) / sizeof(axisNames[0]);

const char *AxisName(int axis) {
  if (axis < 0 || axis >= maxAxes)
    return noneDefined;
  return axisNames[axis];
}

void RegisterGamePad(int pad) {
  Pad *gamepad = &GamePads[pad];
  gamepad->registered = true;
  gamepad->axisCount = GetGamepadAxisCount(pad);
  const char *name = GetGamepadName(pad);
  strncpy(gamepad->name, name, sizeof(gamepad->name) - 1);
}

inline bool IsGamePadValid(int pad) {
  if (pad < 0 || pad >= maxGamePads || !IsGamepadAvailable(pad)) {
    return false;
  }
  if (!GamePads[pad].registered) {
    RegisterGamePad(pad);
  }
  return true;
}

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
    vec.z += delta;
    return vec;
  }
  if (IsKeyDown(KEY_DOWN)) {
    vec.z -= delta;
    return vec;
  }
  if (IsKeyDown(KEY_PAGE_UP)) {
    vec.y += delta;
    return vec;
  }
  if (IsKeyDown(KEY_PAGE_DOWN)) {
    vec.y -= delta;
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

bool CheckButton(int button) {
  for (int pad = 0; pad < maxGamePads; pad++) {
    if (!IsGamePadValid(pad)) {
      continue;
    }
    if (IsGamepadButtonDown(pad, button)) {
      return true;
    }
  }
  return false;
}

Vector3 ButtonsToVector(Vector3 vec, Vector3 base, float scale) {
  float delta = scale;
  for (int pad = 0; pad < maxGamePads; pad++) {
    if (!IsGamePadValid(pad)) {
      continue;
    }
    if (IsGamepadButtonDown(pad, GAMEPAD_BUTTON_LEFT_FACE_LEFT)) {
      vec.z += delta;
      return vec;
    }
    if (IsGamepadButtonDown(pad, GAMEPAD_BUTTON_LEFT_FACE_RIGHT)) {
      vec.z -= delta;
      return vec;
    }
    if (IsGamepadButtonDown(pad, GAMEPAD_BUTTON_LEFT_FACE_UP)) {
      vec.x -= delta;
      return vec;
    }
    if (IsGamepadButtonDown(pad, GAMEPAD_BUTTON_LEFT_FACE_DOWN)) {
      vec.x += delta;
      return vec;
    }
    if (IsGamepadButtonDown(pad, GAMEPAD_BUTTON_LEFT_TRIGGER_1)) {
      vec.y += delta;
      return vec;
    }
    if (IsGamepadButtonDown(pad, GAMEPAD_BUTTON_RIGHT_TRIGGER_1)) {
      vec.y -= delta;
      return vec;
    }
    if (IsGamepadButtonDown(pad, GAMEPAD_BUTTON_RIGHT_FACE_LEFT)) {
      vec.x = base.x;
      vec.y = base.y;
      vec.z = base.z;
      return vec;
    }
  }
  return vec;
}

static int axes[] = {GAMEPAD_AXIS_RIGHT_X, GAMEPAD_AXIS_RIGHT_Y,
                     GAMEPAD_AXIS_LEFT_Y};

Vector3 AxesToVector(Vector3 vec, Vector3 base, float scale) {
  enum { AXIS_X, AXIS_Z, AXIS_Y };
  for (int pad = 0; pad < maxGamePads; pad++) {
    if (!IsGamePadValid(pad)) {
      continue;
    }
    for (int i = 0; i < 3; i++) {
      int axis = axes[i];
      float current = GamePads[pad].axisValues[axis];
      float value = GetGamepadAxisMovement(pad, axis);
      if (value != current) {
        GamePads[pad].axisValues[axis] = value;
        switch (i) {
        case AXIS_X:
          vec.x = value * scale + base.x;
          return vec;
        case AXIS_Z:
          vec.z = value * scale + base.z;
          return vec;
        case AXIS_Y:
          vec.y = value * scale + base.y;
          return vec;
        }
      }
    }
  }
  return vec;
}
