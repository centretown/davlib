#include "davlib.h"
#include <assert.h>
#include <stddef.h>
#include <stdio.h>
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
const size_t maxAxes = sizeof(axisNames) / sizeof(axisNames[0]);

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

static int axes[] = {GAMEPAD_AXIS_RIGHT_X, GAMEPAD_AXIS_RIGHT_Y,
                     GAMEPAD_AXIS_LEFT_Y};

Vector3 AxesToVectorPro(Vector3 vec, Vector3 base, float scale, Vector3 min,
                        Vector3 max) {
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
          break;
        case AXIS_Z:
          vec.z = value * scale + base.z;
          break;
        case AXIS_Y:
          vec.y = value * scale + base.y;
          break;
        }
      }
    }
  }
  if (!IS_VEC3_EQUAL(min, max)) {
    vec.x = CLAMPNUM(vec.x, min.x, max.x);
    vec.y = CLAMPNUM(vec.y, min.y, max.y);
    vec.z = CLAMPNUM(vec.z, min.z, max.z);
  }
  return vec;
}

Vector3 AxesToVector(Vector3 vec, Vector3 base, float scale) {
  return AxesToVectorPro(vec, base, scale, VEC3_NULL, VEC3_NULL);
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

Vector3 ButtonsToVectorPro(Vector3 vec, Vector3 base, float scale, Vector3 min,
                           Vector3 max) {
  float delta = scale;

  for (int pad = 0; pad < maxGamePads; pad++) {
    if (!IsGamePadValid(pad)) {
      continue;
    }
    if (IsGamepadButtonDown(pad, GAMEPAD_BUTTON_LEFT_FACE_LEFT)) {
      vec.x += delta;
      break;
    }
    if (IsGamepadButtonDown(pad, GAMEPAD_BUTTON_LEFT_FACE_RIGHT)) {
      vec.x -= delta;
      break;
    }
    if (IsGamepadButtonDown(pad, GAMEPAD_BUTTON_LEFT_FACE_UP)) {
      vec.y += delta;
      break;
    }
    if (IsGamepadButtonDown(pad, GAMEPAD_BUTTON_LEFT_FACE_DOWN)) {
      vec.y -= delta;
      break;
    }
    if (IsGamepadButtonDown(pad, GAMEPAD_BUTTON_LEFT_TRIGGER_1)) {
      vec.z += delta;
      break;
    }
    if (IsGamepadButtonDown(pad, GAMEPAD_BUTTON_RIGHT_TRIGGER_1)) {
      vec.z -= delta;
      break;
    }
    if (IsGamepadButtonDown(pad, GAMEPAD_BUTTON_RIGHT_FACE_LEFT)) {
      vec.x = base.x;
      vec.y = base.y;
      vec.z = base.z;
      break;
    }
  }

  if (!IS_VEC3_EQUAL(min, max)) {
    vec.x = CLAMPNUM(vec.x, min.x, max.x);
    vec.y = CLAMPNUM(vec.y, min.y, max.y);
    vec.z = CLAMPNUM(vec.z, min.z, max.z);
  }
  return vec;
}

Vector3 ButtonsToVector(Vector3 vec, Vector3 base, float scale) {
  return ButtonsToVectorPro(vec, base, scale, VEC3_NULL, VEC3_NULL);
}

Rectangle ButtonsToRectangle(Rectangle rec, Rectangle recbase, float scale) {
  Vector3 vec = (Vector3){.x = rec.x, .y = 0.0f, .z = rec.y};
  Vector3 base = (Vector3){.x = recbase.x, .y = 0.0f, .z = recbase.y};
  vec = ButtonsToVector(vec, base, scale);
  rec.x = vec.x;
  rec.y = vec.z;
  return rec;
}
