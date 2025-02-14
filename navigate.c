#include "davlib.h"
#include "raylib.h"

#include <assert.h>
// #include <stdio.h>

const static int navButtons[] = {
    GAMEPAD_BUTTON_LEFT_FACE_LEFT,  GAMEPAD_BUTTON_LEFT_FACE_RIGHT,
    GAMEPAD_BUTTON_LEFT_FACE_UP,    GAMEPAD_BUTTON_LEFT_FACE_DOWN,
    GAMEPAD_BUTTON_RIGHT_FACE_LEFT, GAMEPAD_BUTTON_RIGHT_FACE_RIGHT,
};
const static int navButtonsSize = sizeof(navButtons) / sizeof(navButtons[0]);

const static int navKeys[] = {
    KEY_LEFT, KEY_RIGHT, KEY_UP, KEY_DOWN, KEY_HOME, KEY_BACKSPACE,
};
const static int navKeysCount = sizeof(navKeys) / sizeof(navKeys[0]);

Rectangle arrowRects[ARROW_COUNT] = {0};
void ResetArrows() {
  for (int i = 0; i < ARROW_COUNT; i++) {
    arrowRects[i] = (Rectangle){0};
  }
}

static Menu *menuStack[MENU_STACK_SIZE] = {0};
static int menuStackTop = -1;

Menu *MenuTop() {
  menuStackTop = 0;
  return menuStack[menuStackTop];
}

bool IsTop() { return menuStackTop == 0; }

Menu *CurrentMenu() { return menuStack[menuStackTop]; }

Menu *PopMenu() {
  int i = menuStackTop;
  if (menuStackTop > 0)
    --menuStackTop;
  return menuStack[i];
}

void PushMenu(Menu *menu) {
  if (menuStackTop < MENU_STACK_SIZE - 1)
    ++menuStackTop;
  menuStack[menuStackTop] = menu;
}

void NavigateMenu(Navigator nav, double now) {
  int cmd = nav.cmd;
  if (CMD_NONE == cmd) {
    return;
  }
  Menu *menu = CurrentMenu();
  MenuItem *item = menu->items[menu->current];
  assert(item);

  switch (cmd) {
  case NAV_LEFT:
  case NAV_RIGHT:
    switch (item->itemType) {
    case MENU_SUB: {
      menu = item->menu;
      menu->title = item->label;
      menu->data = item->data;
      PushMenu(item->menu);
      if (item->onPush) {
        item->onPush(menu);
      }
      break;
    }
    case MENU_CHOICE:
      item->choiceCurrent += (cmd == NAV_RIGHT) ? 1 : -1;
      item->choiceCurrent = CLAMPNUM(item->choiceCurrent, 0, item->choiceCount);
      break;
    case MENU_FLOAT:
      item->fvalue += (cmd == NAV_RIGHT) ? item->finc : -item->finc;
      item->fvalue = CLAMPNUM(item->fvalue, item->fmin, item->fmax);
      break;
    case MENU_INT:
      item->ivalue += (cmd == NAV_RIGHT) ? item->iinc : -item->iinc;
      item->ivalue = CLAMPNUM(item->ivalue, item->imin, item->imax);
      break;
    case MENU_DISPLAY:
      break;
    }
    if (item->onChoose) {
      item->onChoose(menu);
    }
    break;
  case NAV_UP:
  case NAV_DOWN: {
    int count = 0;
    int inc = (cmd == NAV_DOWN) ? 1 : -1;
    // skip display only items
    do {
      menu->current += inc;
      menu->current = CLAMPNUM(menu->current, 0, menu->itemCount);
      item = menu->items[menu->current];
      count++;
    } while (item->itemType == MENU_DISPLAY && count < menu->itemCount);
    break;
  }
  case NAV_HOME:
    break;
  case NAV_BACK:
    PopMenu();
    break;
  default:
    break;
  }
}

static TimedInput padIn = {
    .interval = 0.35f,
    .lastTime = 0.0f,
    .nextTime = 0.0f,
    .lastInput = CMD_NONE,
};

Navigator InputGamepad(int count, const int *buttons, double now) {
  assert(buttons);
  assert(count >= 0);

  Navigator nav = {.cmd = CMD_NONE};

  bool found = false;
  for (int i = 0; i < count; i++) {
    found = CheckButton(buttons[i]);
    if (found) {
      padIn.lastInput = i;
      break;
    }
  }

  if (now >= padIn.nextTime && found) {
    int cmd = padIn.lastInput;
    padIn.lastInput = CMD_NONE;
    padIn.nextTime = now + padIn.interval;
    nav.cmd = cmd;
  }
  return nav;
}

Navigator InputGamepadNav(double now) {
  return InputGamepad(navButtonsSize, navButtons, now);
}

static TimedInput keyIn = {
    .interval = 0.35f,
    .nextTime = 0.0f,
    .lastTime = 0.0f,
    .lastInput = 0,
};

Navigator InputKeys(int count, const int *keys, double now) {
  Navigator nav = {.cmd = CMD_NONE};
  assert(keys);
  assert(count >= 0);
  for (int i = 0; i < count; i++) {
    if (IsKeyDown(keys[i]) && now >= keyIn.nextTime) {
      keyIn.nextTime = now + keyIn.interval;
      nav.cmd = i;
      return nav;
    }
  }
  return nav;
}

Navigator InputKeyNav(double now) {
  return InputKeys(navKeysCount, navKeys, now);
}

Navigator InputNav(double now, Vector2 point) {
  Navigator nav = (Navigator){.cmd = CMD_NONE};
  // left arrow is inside item.rect
  InputMouseMenu(point);
  nav = InputMouseNav(now, point);
  if (nav.cmd == CMD_NONE) {
    nav = InputGamepadNav(now);
    if (nav.cmd == CMD_NONE) {
      nav = InputKeyNav(now);
    }
  }

  return nav;
}
