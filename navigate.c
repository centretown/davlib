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

static Menu *menuStack[MENU_STACK_SIZE] = {0};
static int menuStackTop = -1;

Menu *MenuTop() {
  menuStackTop = 0;
  return menuStack[menuStackTop];
}

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

bool PointInRect(Vector2 point, Rectangle rect) {
  return (point.x >= rect.x && point.x < rect.x + rect.width) &&
         (point.y >= rect.y && point.y < rect.y + rect.height);
}

void DrawItemValue(MenuItem *item, int x, int y, int fontSize, Color color) {
  switch (item->itemType) {
  case MENU_SUB: {
    DrawText(item->label, x, y, fontSize, color);
    break;
  }
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

void DrawArrows(Menu *menu, Theme *theme, float scale, Vector2 point) {
  Color hover = theme->colorHover;
  Color dim = theme->colorDim;
  bool isHovered = PointInRect(point, menu->arrowRects[ARROW_LEFT]);
  DrawTextureEx(
      theme->leftArrow,
      (Vector2){menu->arrowRects[ARROW_LEFT].x, menu->arrowRects[ARROW_LEFT].y},
      0.0f, scale, (isHovered) ? hover : dim);
  isHovered = PointInRect(point, menu->arrowRects[ARROW_RIGHT]);
  DrawTextureEx(theme->rightArrow,
                (Vector2){menu->arrowRects[ARROW_RIGHT].x,
                          menu->arrowRects[ARROW_RIGHT].y},
                0.0f, scale, (isHovered) ? hover : dim);
}

void DrawMenu(Theme *theme, Vector2 position, Vector2 point) {
  Menu *menu = CurrentMenu();
  assert(theme);

  float fontSize = theme->fontSize;
  int lineHeight = fontSize + fontSize / 2;
  int baseX = position.x + theme->padding;
  int baseY = position.y + theme->padding;

  float scale = fontSize / 24.0f;
  float arrowWidth = theme->leftArrow.width * scale;
  float arrowHeight = theme->leftArrow.height * scale;

  Rectangle panel = (Rectangle){.x = position.x,
                                .y = position.y,
                                .width = (2 * theme->valueColumn * fontSize) +
                                         (2 * theme->padding) + (2 * fontSize),
                                .height = (menu->itemCount + 1) * lineHeight +
                                          2 * theme->padding};

  DrawRectangleRounded(panel, .25f, 8,
                       (Color){.a = 96, .r = 11, .g = 127, .b = 127});

  float titleLength = (float)MeasureText(menu->title, fontSize);
  DrawText(menu->title, panel.width / 2 - titleLength / 2, baseY, fontSize,
           theme->titleColor);

  float row = baseY + fontSize + fontSize / 3;
  for (int itemIndex = 0; itemIndex < menu->itemCount;
       itemIndex++, row += lineHeight) {
    MenuItem *item = menu->items[itemIndex];

    // draw label
    float col = baseX;
    DrawText(item->label, col, row, fontSize,
             (itemIndex == menu->current)     ? theme->labelActive
             : (itemIndex == menu->hoverItem) ? theme->labelHover
                                              : theme->labelColor);
    // calculate item size and position
    item->rect.x = col;
    item->rect.y = row;
    item->rect.height = fontSize;
    col = baseX + theme->valueColumn * fontSize;
    item->rect.width = col - item->rect.x;
    item->rect.width += theme->valueColumn * fontSize;

    // default color when not active or hovered
    Color color = theme->valueColor;
    if (itemIndex == menu->current) {
      color = theme->valueActive;

      // calculate size and positon of arrows
      menu->arrowRects[ARROW_LEFT].y = menu->arrowRects[ARROW_RIGHT].y =
          item->rect.y - fontSize / 4.0f;
      menu->arrowRects[ARROW_LEFT].width = menu->arrowRects[ARROW_RIGHT].width =
          arrowWidth;
      menu->arrowRects[ARROW_LEFT].height =
          menu->arrowRects[ARROW_RIGHT].height = arrowHeight;
      menu->arrowRects[ARROW_LEFT].x =
          col - (theme->leftArrow.width * scale + 8);
      menu->arrowRects[ARROW_RIGHT].x = col + theme->valueColumn * fontSize;

      // draw arrows for the active item
      DrawArrows(menu, theme, scale, point);
    } else if (itemIndex == menu->hoverItem) {
      color = theme->valueHover;
    }

    DrawItemValue(item, col + theme->valueColumn, row, fontSize, color);
  }
}

void NavigateMenu(int cmd, double now) {
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
  case NAV_HOME:
    break;
  case NAV_BACK:
    PopMenu();
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
    .interval = 0.35f,
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
    .interval = 0.35f,
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

void InputMouseMenu(double now, Vector2 point) {
  Menu *menu = CurrentMenu();
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

  int cmd = InputMouse(2, menu->arrowRects, now, point);
  if (cmd != CMD_NONE) {
    NavigateMenu(cmd, now);
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
    .interval = 0.35f,
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
