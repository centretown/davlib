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

void DrawItemValue(MenuItem *item, Rectangle rect, int fontSize, Color color) {
  if (item->onDraw) {
    item->onDraw(item, rect, fontSize, color);
    return;
  }

  switch (item->itemType) {
  case MENU_SUB: {
    DrawText(item->label, rect.x, rect.y, fontSize, color);
    break;
  }
  case MENU_CHOICE: {
    const char *choice = item->choices[item->choiceCurrent];
    if (item->choices) {
      DrawText(choice, rect.x, rect.y, fontSize, color);
    }
    break;
  }
  case MENU_FLOAT:
    DrawText(TextFormat("%0.1f", item->fvalue), rect.x, rect.y, fontSize,
             color);
    break;
  case MENU_INT:
    DrawText(TextFormat("%d", item->ivalue), rect.x, rect.y, fontSize, color);
    break;
  }
}

void DrawPanelHeading(Menu *menu, Theme *theme, float baseX, float baseY,
                      float panelWidth, Vector2 point) {
  float fontSize = theme->fontSize;
  float scale = fontSize / 24.0f;
  if (!IsTop()) {
    arrowRects[ARROW_BACK] = (Rectangle){.x = baseX,
                                         .y = baseY,
                                         .width = theme->outArrow.width,
                                         .height = theme->outArrow.height};
    bool isHovered = PointInRect(point, arrowRects[ARROW_BACK]);
    DrawTextureEx(theme->outArrow,
                  (Vector2){arrowRects[ARROW_BACK].x, arrowRects[ARROW_BACK].y},
                  0.0f, scale,
                  (isHovered) ? theme->colorHover : theme->colorDim);
  }

  float titleLength = (float)MeasureText(menu->title, fontSize);
  DrawText(menu->title, baseX + panelWidth / 2 - titleLength / 2, baseY,
           fontSize, theme->titleColor);
}

void DrawArrows(Menu *menu, MenuItem *item, Theme *theme, int col,
                Vector2 point) {
  Color hover = theme->colorHover;
  Color dim = theme->colorDim;
  float fontSize = theme->fontSize;
  float scale = fontSize / 24.0f;
  float arrowWidth = theme->leftArrow.width * scale;
  float arrowHeight = theme->leftArrow.height * scale;
  // arrowRects = {0};
  // calculate size and positon of arrows
  arrowRects[ARROW_LEFT].y = arrowRects[ARROW_RIGHT].y =
      item->rect.y - fontSize / 4.0f;
  arrowRects[ARROW_LEFT].width = arrowRects[ARROW_RIGHT].width = arrowWidth;
  arrowRects[ARROW_LEFT].height = arrowRects[ARROW_RIGHT].height = arrowHeight;
  arrowRects[ARROW_LEFT].x = col - arrowWidth;
  // arrowRects[ARROW_LEFT].x = col - (theme->leftArrow.width * scale + 8);
  arrowRects[ARROW_RIGHT].x = col + theme->valueColumn * fontSize;

  bool leftHovered = PointInRect(point, arrowRects[ARROW_LEFT]);
  bool rightHovered = PointInRect(point, arrowRects[ARROW_RIGHT]);
  if (item->itemType == MENU_SUB) {
    DrawTextureEx(theme->rightArrow,
                  (Vector2){arrowRects[ARROW_LEFT].x, arrowRects[ARROW_LEFT].y},
                  0.0f, scale, (leftHovered) ? hover : dim);
    DrawTextureEx(
        theme->leftArrow,
        (Vector2){arrowRects[ARROW_RIGHT].x, arrowRects[ARROW_RIGHT].y}, 0.0f,
        scale, (rightHovered) ? hover : dim);
  } else {
    DrawTextureEx(theme->leftArrow,
                  (Vector2){arrowRects[ARROW_LEFT].x, arrowRects[ARROW_LEFT].y},
                  0.0f, scale, (leftHovered) ? hover : dim);
    DrawTextureEx(
        theme->rightArrow,
        (Vector2){arrowRects[ARROW_RIGHT].x, arrowRects[ARROW_RIGHT].y}, 0.0f,
        scale, (rightHovered) ? hover : dim);
  }
}

static float roundnessFactor = 0.075f;
static int roundnessSegments = 8;

void DrawMenu(Theme *theme, Vector2 position, Vector2 point) {
  Menu *menu = CurrentMenu();
  assert(theme);

  ResetArrows();
  float fontSize = theme->fontSize;
  float lineHeight = fontSize + fontSize / 2;
  float baseX = position.x + theme->padding;
  float baseY = position.y + theme->padding;

  Rectangle panel = (Rectangle){.x = position.x,
                                .y = position.y,
                                .width = (2 * theme->valueColumn * fontSize) +
                                         (2 * theme->padding) + (2 * fontSize),
                                .height = (menu->itemCount + 1) * lineHeight +
                                          3 * theme->padding};

  // a little trickery to get a consistent roundness
  // see DrawRectangleRounded in raylib/rshapes.c
  float roundness = (panel.width > panel.height) ? (panel.width / panel.height)
                                                 : (panel.height / panel.width);
  DrawRectangleRounded(panel, roundness * roundnessFactor, roundnessSegments,
                       theme->panelColor);

  DrawPanelHeading(menu, theme, baseX, baseY, panel.width, point);

  float row = baseY + lineHeight + theme->padding;
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
      DrawArrows(menu, item, theme, col, point);
    } else if (itemIndex == menu->hoverItem) {
      color = theme->valueHover;
    }

    DrawItemValue(item,
                  (Rectangle){.x = col + theme->valueColumn,
                              .y = row,
                              .width = (theme->valueColumn - 1) * fontSize,
                              .height = fontSize},
                  fontSize, color);
  }
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
