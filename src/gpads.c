
#include "gpads.h"

#include <assert.h>
#include <raylib.h>
// #include <stdio.h>
#include <string.h>

Pad GamePads[4] = {0};
const size_t maxGamePads = sizeof(GamePads) / sizeof(GamePads[0]);

const char *noneDefined = "undefined";
const PadButtonResult padButtonResultNone = {
    .pad = -1,
    .button = -1,
    .command = -1,
};
const PadAxisResult padAxisResultNone = {
    .pad = -1,
    .axis = -1,
    .command = -1,
    .value = 0.0f,
};

const char *buttonNames[] = {
    "unknown",         "left face up",   "left face right",  "left face down",
    "left face left",  "right face up",  "right face right", "right face down",
    "right face left", "left trigger 1", "left trigger 2",   "right trigger 1",
    "right trigger 2", "middle left",    "middle",           "middle right",
    "left thumb",      "right thumb",
};
const size_t maxButtons = sizeof(buttonNames) / sizeof(buttonNames[0]);
inline const char *ButtonName(int button) {
  if (button < 0 || button >= maxButtons)
    return noneDefined;
  return buttonNames[button];
}

const char *axisNames[] = {
    "left x", "left y", "right x", "right y", "left trigger", "right trigger",
};
size_t maxAxes = sizeof(axisNames) / sizeof(axisNames[0]);

inline const char *AxisName(int axis) {
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

PadAxisResult CheckGamePadAxis(int pad, int axis, int command) {
  if (!IsGamePadValid(pad)) {
    return padAxisResultNone;
  }
  float current = GamePads[pad].axisValues[axis];
  float value = GetGamepadAxisMovement(pad, axis);
  if (value == current) {
    return padAxisResultNone;
  }

  GamePads[pad].axisValues[axis] = value;
  return (PadAxisResult){
      .pad = pad,
      .axis = axis,
      .command = command,
      .value = value,
  };
}

PadAxisResult CheckGamePadAxes(int count, const int *list) {
  assert(count > 0);
  for (int pad = 0; pad < maxGamePads; pad++) {
    if (!IsGamePadValid(pad)) {
      continue;
    }

    for (int command = 0; command < count; command++) {
      PadAxisResult result = CheckGamePadAxis(pad, list[command], command);
      if (result.command >= 0) {
        // printf("pad=%d axis=%d command=%d value=%f\n", result.pad, result.axis,
        //        result.command, result.value);
        return result;
      }
    }
  }
  return padAxisResultNone;
}

inline PadButtonResult CheckGamePadButton(int pad, int button, int index) {
  if (IsGamepadButtonDown(pad, button)) {
    return (PadButtonResult){
        .pad = pad,
        .button = button,
        .command = index,
    };
  }
  return padButtonResultNone;
}

PadButtonResult CheckAllGamePadButtons(int count, const int *list) {
  assert(list);
  assert(count > 0);
  for (int pad = 0; pad < maxGamePads; pad++) {
    if (!IsGamePadValid(pad))
      continue;

    for (int index = 0; index < count; index++) {
      int button = list[index];
      PadButtonResult result = CheckGamePadButton(pad, button, index);
      if (result.command >= 0) {
        return result;
      }
    }
  }
  return padButtonResultNone;
}

// PadButtonResult CheckAllGamePadButtons(int count, ...) {
//   assert(count > 0);
//   va_list valist;
//   va_start(valist, count);

//   for (int pad = 0; pad < maxGamePads; pad++) {
//     if (!IsGamePadValid(pad))
//       continue;

//     for (int index = 0; index < count; index++) {
//       int button = va_arg(valist, int);
//       PadButtonResult result = CheckGamePadButton(pad, button, index);
//       if (result.command >= 0) {
//         va_end(valist);
//         return result;
//       }
//     }
//   }
//   va_end(valist);
//   return padButtonResultNone;
// }
