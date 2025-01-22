#include "gpads.h"
#include "string.h"

Pad gamepads[4] = {0};
const int max_game_pads = sizeof(gamepads) / sizeof(gamepads[0]);
const char *undefined = "undefined";

const char *button_names[] = {
    "unknown",         "left face up",   "left face right",  "left face down",
    "left face left",  "right face up",  "right face right", "right face down",
    "right face left", "left trigger 1", "left trigger 2",   "right trigger 1",
    "right trigger 2", "middle left",    "middle",           "middle right",
    "left thumb",      "right thumb",
};
size_t max_buttons = sizeof(button_names) / sizeof(button_names[0]);
inline const char *ButtonName(int button) {
  if (button < 0 || button >= max_buttons)
    return undefined;
  return button_names[button];
}

const char *axis_names[] = {
    "left x", "left y", "right x", "right y", "left trigger", "right trigger",
};
size_t max_axes = sizeof(axis_names) / sizeof(axis_names[0]);

inline const char *AxisName(int axis) {
  if (axis < 0 || axis >= max_axes)
    return undefined;
  return axis_names[axis];
}

Cmd CheckGamePad(int pad) {
  if (pad < 0 || pad >= max_game_pads) {
    return CMD_UNDEFINED;
  }
  if (!IsGamepadAvailable(pad)) {
    return CMD_UNDEFINED;
  }

  Pad *gamepad = &gamepads[pad];
  if (!gamepad->found) {
    gamepad->found = true;
    gamepad->axisCount = GetGamepadAxisCount(pad);
    const char *name = GetGamepadName(pad);
    strncpy(gamepad->name, name, sizeof(gamepad->name) - 1);
    printf("%s gamepad:%d axisCount:%d\n", gamepad->name, pad,
           gamepad->axisCount);
  }

  if (IsGamepadButtonDown(pad, GAMEPAD_BUTTON_LEFT_FACE_LEFT))
    return CMD_MOVE_LEFT;
  if (IsGamepadButtonDown(pad, GAMEPAD_BUTTON_LEFT_FACE_RIGHT))
    return CMD_MOVE_RIGHT;
  if (IsGamepadButtonDown(pad, GAMEPAD_BUTTON_LEFT_FACE_UP))
    return CMD_MOVE_UP;
  if (IsGamepadButtonDown(pad, GAMEPAD_BUTTON_LEFT_FACE_DOWN))
    return CMD_MOVE_DOWN;
  if (IsGamepadButtonDown(pad, GAMEPAD_BUTTON_LEFT_TRIGGER_1))
    return CMD_MOVE_IN;
  if (IsGamepadButtonDown(pad, GAMEPAD_BUTTON_RIGHT_TRIGGER_1))
    return CMD_MOVE_OUT;
  if (IsGamepadButtonDown(pad, GAMEPAD_BUTTON_MIDDLE_RIGHT))
    return CMD_MOVE_HOME;
  return CMD_UNDEFINED;
}

Cmd CheckGamePads() {
  for (int i = 0; i < max_game_pads; i++) {
    Cmd cmd = CheckGamePad(i);
    if (cmd != CMD_UNDEFINED) {
      return cmd;
    }
  }
  return CMD_UNDEFINED;
}
