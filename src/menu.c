#include "menu.h"
#include "vec.h"
#include <assert.h>
#include <raylib.h>

static double nextUpdate = 0.0f;
static double nextInterval = 0.5f;

const static int navButtons[] = {
    GAMEPAD_BUTTON_LEFT_FACE_LEFT,  GAMEPAD_BUTTON_LEFT_FACE_RIGHT,
    GAMEPAD_BUTTON_LEFT_FACE_UP,    GAMEPAD_BUTTON_LEFT_FACE_DOWN,
    GAMEPAD_BUTTON_RIGHT_FACE_LEFT, GAMEPAD_BUTTON_RIGHT_FACE_RIGHT,
};
const static int navButtonsSize = sizeof(navButtons) / sizeof(navButtons[0]);

const static int navKeys[] = {
    KEY_LEFT, KEY_RIGHT, KEY_UP, KEY_DOWN, KEY_ENTER, KEY_ESCAPE,
};
const static int navKeysCount = sizeof(navKeys) / sizeof(navKeys[0]);

void DrawMenu(Theme *theme, Menu *menu, Position position, double now) {
  assert(theme);
  assert(menu);
  assert(menu->items);
  assert(menu->itemCount >= 0);

  int fontSize = theme->fontSize;
  int baseX = menu->position.x * fontSize + position.x * fontSize;
  int baseY = menu->position.y * fontSize + position.y * fontSize;
  DrawText(menu->title, baseX, baseY, fontSize, theme->titleColor);

  int y = baseY + fontSize + fontSize / 3;
  for (int i = 0; i < menu->itemCount; i++) {
    MenuItem *item = &menu->items[i];
    int x = baseX + 2 * fontSize;
    DrawText(item->label, x, y, fontSize,
             (i == menu->current) ? theme->labelActive : theme->labelColor);
    x = baseX + menu->valueColumn * fontSize;
    DrawText(item->value, x, y, fontSize,
             (i == menu->current) ? theme->valueActive : theme->valueColor);
    y += fontSize;
  }
  int cmd = InputNav(now);
  if (CMD_NONE != cmd) {
    switch (cmd) {
    case NAV_LEFT:
      break;
    case NAV_RIGHT:
      break;
    case NAV_UP:
      menu->current--;
      break;
    case NAV_DOWN:
      menu->current++;
      break;
    case NAV_SELECT:
      break;
    case NAV_ESCAPE:
      break;
    }
    menu->current = ClampInt(menu->current, 0, menu->itemCount);
  }
}

int InputGamepad(double now, int count, const int *buttons) {
  assert(buttons);
  assert(count >= 0);
  for (int i = 0; i < count; i++) {
    if (CheckButton(buttons[i])) {
      if (now >= nextUpdate) {
        nextUpdate = now + nextInterval;
        return i;
      }
      break;
    }
  }
  return CMD_NONE;
}

int InputGamepadNav(double now) {
  return InputGamepad(now, navButtonsSize, navButtons);
}

int InputKeys(int count, const int *keys, double now) {
  assert(keys);
  assert(count >= 0);
  for (int i = 0; i < count; i++) {
    if (IsKeyDown(keys[i]) && now >= nextUpdate) {
      nextUpdate = now + nextInterval;
      return i;
    }
  }

  return CMD_NONE;
}

int InputKeyNav(double now) { return InputKeys(navKeysCount, navKeys, now); }

int InputNav(double now) {
  int cmd = InputGamepadNav(now);
  if (cmd == CMD_NONE) {
    cmd = InputKeyNav(now);
  }

  return cmd;
}

int ClampInt(int current, int min, int max) {
  if (current >= max) {
    return min;
  }
  if (current < min) {
    return max - 1;
  }
  return current;
}
