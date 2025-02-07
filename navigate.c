#include "davlib.h"

#include <assert.h>
// #include <stdio.h>

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

bool PointInRect(Vector2 point, Rectangle rect) {
  return (point.x >= rect.x && point.x < rect.x + rect.width) &&
         (point.y >= rect.y && point.y < rect.y + rect.height);
}

void DrawMenu(Menu *menu, Position position) {
  assert(menu);
  assert(menu->items);
  assert(menu->itemCount >= 0);
  Theme *theme = menu->theme;
  assert(theme);

  int fontSize = theme->fontSize;
  int lineHeight = fontSize + fontSize / 2;
  int baseX = position.x + menu->position.x * fontSize;
  int baseY = position.y + menu->position.y * fontSize;

  int y = baseY + fontSize + fontSize / 3;
  float scale = fontSize / 24.0f;
  float arrowWidth = menu->leftArrow.width * scale;
  float arrowHeight = menu->leftArrow.height * scale;

  DrawRectangleRounded(
      (Rectangle){.x = position.x,
                  .y = position.y,
                  .width = 2.5 * menu->valueColumn * fontSize,
                  .height = (menu->itemCount + 2) * lineHeight + 2 * scale},
      .25f, 8, (Color){.a = 96, .r = 11, .g = 127, .b = 127});

  DrawText(menu->title, baseX, baseY, fontSize, theme->titleColor);

  for (int i = 0; i < menu->itemCount; i++, y += lineHeight) {
    MenuItem *item = menu->items[i];
    int x = baseX + 2 * fontSize;

    // draw label
    DrawText(item->label, x, y, fontSize,
             (i == menu->current)     ? theme->labelActive
             : (i == menu->hoverItem) ? theme->labelHover
                                      : theme->labelColor);
    item->rect.x = x;
    item->rect.y = y;
    item->rect.height = fontSize;
    x = baseX + menu->valueColumn * fontSize;
    item->rect.width = x - item->rect.x;

    Color color = theme->valueColor;
    item->rect.width += menu->valueColumn * fontSize;

    // draw arrows for the current item
    if (i == menu->current) {
      color = theme->valueActive;
      menu->arrowRects[ARROW_LEFT].y = menu->arrowRects[ARROW_RIGHT].y =
          item->rect.y - (float)fontSize / 4.0f;
      menu->arrowRects[ARROW_LEFT].width = menu->arrowRects[ARROW_RIGHT].width =
          arrowWidth;
      menu->arrowRects[ARROW_LEFT].height =
          menu->arrowRects[ARROW_RIGHT].height = arrowHeight;
      menu->arrowRects[ARROW_LEFT].x = x - (menu->leftArrow.width * scale + 8);
      menu->arrowRects[ARROW_RIGHT].x = x + menu->valueColumn * fontSize;

      Vector2 mousePos = menu->mousePos;
      bool isHovered = PointInRect(mousePos, menu->arrowRects[ARROW_LEFT]);
      DrawTextureEx(menu->leftArrow,
                    (Vector2){menu->arrowRects[ARROW_LEFT].x,
                              menu->arrowRects[ARROW_LEFT].y},
                    0.0f, scale,
                    (isHovered) ? menu->colorHover : menu->colorDim);

      isHovered = PointInRect(mousePos, menu->arrowRects[ARROW_RIGHT]);
      DrawTextureEx(menu->rightArrow,
                    (Vector2){menu->arrowRects[ARROW_RIGHT].x,
                              menu->arrowRects[ARROW_RIGHT].y},
                    0.0f, scale,
                    (isHovered) ? menu->colorHover : menu->colorDim);
    } else if (i == menu->hoverItem) {
      color = theme->valueHover;
    }

    switch (item->itemType) {
    case MENU_SUB:
      break;
    case MENU_CHOICE: {
      const char *choice = item->choices[item->choiceCurrent];
      if (item->choices) {
        DrawText(choice, x, y, fontSize, color);
      }
      break;
    }
    case MENU_FLOAT:
      DrawText(TextFormat("%0.1f", item->fvalue), x, y, fontSize, color);
      break;
    case MENU_INT:
      DrawText(TextFormat("%d", item->ivalue), x, y, fontSize, color);
      break;
    }
  }
}

void NavigateMenu(int cmd, Menu *menu, double now) {
  MenuItem *item = menu->items[menu->current];
  assert(item);

  if (CMD_NONE == cmd) {
    return;
  }
  switch (cmd) {
  case NAV_LEFT:
  case NAV_RIGHT:
    switch (item->itemType) {
    case MENU_SUB:
      break;
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
    }
    if (item->onChoose) {
      item->onChoose(menu);
    }
    break;
  case NAV_UP:
  case NAV_DOWN:
    menu->current += (cmd == NAV_DOWN) ? 1 : -1;
    menu->current = CLAMPNUM(menu->current, 0, menu->itemCount);
    break;
  case NAV_SELECT:
    break;
  case NAV_ESCAPE:
    break;
  }
}

typedef struct TimedInput {
  double interval;
  double lastTime; // when last input set
  double nextTime; // when last input set
  int lastInput;   // set to 0 after interval
} TimedInput;

static TimedInput padIn = {
    .interval = 0.25f,
    .lastTime = 0.0f,
    .nextTime = 0.0f,
    .lastInput = CMD_NONE,
};

int InputGamepad(int count, const int *buttons, double now) {
  assert(buttons);
  assert(count >= 0);

  bool found = false;
  for (int i = 0; i < count; i++) {
    found = CheckButton(buttons[i]);
    if (found) {
      padIn.lastInput = i;
      break;
    }
  }

  if (now >= padIn.nextTime && found) {
    int in = padIn.lastInput;
    padIn.lastInput = CMD_NONE;
    padIn.nextTime = now + padIn.interval;
    return in;
  }
  return CMD_NONE;
}

int InputGamepadNav(double now) {
  return InputGamepad(navButtonsSize, navButtons, now);
}

int InputPoint(Vector2 point, int count, const Rectangle *rects, double now) {
  for (int i = 0; i < count; i++) {
    const Rectangle rect = rects[i];
    if (PointInRect(point, rect)) {
      return i;
    }
  }
  return CMD_NONE;
}

const int mouseHover = 0x00010000;
static TimedInput mouseIn = {
    .interval = 0.25f,
    .nextTime = 0.0f,
    .lastTime = 0.0f,
    .lastInput = 0,
};

int InputMouse(int count, const Rectangle *rects, double now,
               Vector2 mousePos) {
  if (now >= mouseIn.nextTime && IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
    int cmd = InputPoint(mousePos, count, rects, now);
    if (cmd != CMD_NONE) {
      mouseIn.nextTime = now + mouseIn.interval;
      return cmd;
    }
  }

  return CMD_NONE;
}

void InputMouseMenu(Menu *menu, double now) {
  Vector2 point = menu->mousePos;
  menu->hoverItem = -1;

  for (int i = 0; i < menu->itemCount; i++) {
    MenuItem *item = menu->items[i];
    const Rectangle rect = item->rect;
    // contained?
    if (PointInRect(point, rect)) {
      if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
        menu->current = i;
      }
      menu->hoverItem = i;
    }
  }

  int cmd = InputMouse(2, menu->arrowRects, now, menu->mousePos);
  if (cmd != CMD_NONE) {
    NavigateMenu(cmd, menu, now);
  }

  if (now >= mouseIn.nextTime && IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
    // printf("updating\n");
    bool updated = false;
    MenuItem *item = menu->items[menu->current];
    int choice = item->choiceCurrent;
    if (PointInRect(point, menu->arrowRects[ARROW_LEFT])) {
      ++choice;
      updated = true;
    } else if (PointInRect(point, menu->arrowRects[ARROW_LEFT])) {
      --choice;
      updated = true;
    }
    if (updated) {
      // printf("updated\n");
      mouseIn.nextTime = now + mouseIn.interval;
      item->choiceCurrent = CLAMPNUM(choice, 0, item->choiceCount);
    }
  }
}

static TimedInput keyIn = {
    .interval = 0.5f,
    .nextTime = 0.0f,
    .lastTime = 0.0f,
    .lastInput = 0,
};

int InputKeys(int count, const int *keys, double now) {
  assert(keys);
  assert(count >= 0);
  for (int i = 0; i < count; i++) {
    if (IsKeyDown(keys[i]) && now >= keyIn.nextTime) {
      keyIn.nextTime = now + keyIn.interval;
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
