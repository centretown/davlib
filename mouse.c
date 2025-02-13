#include "davlib.h"
#include "raylib.h"
extern Rectangle arrowRects[ARROW_COUNT];

bool PointInRect(Vector2 point, Rectangle rect) {
  return (point.x >= rect.x && point.x < rect.x + rect.width) &&
         (point.y >= rect.y && point.y < rect.y + rect.height);
}

int InputPoint(Vector2 point, int count, const Rectangle *rects) {
  for (int i = 0; i < count; i++) {
    if (PointInRect(point, rects[i])) {
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

bool InputMouseButton(Rectangle rect, double now, Vector2 point) {
  if (PointInRect(point, rect)) {
    if (now >= mouseIn.nextTime && IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
      mouseIn.nextTime = now + mouseIn.interval;
      return true;
    }
  }
  return false;
}

int InputMouse(int count, const Rectangle *rects, double now,
               Vector2 mousePos) {
  int cmd = InputPoint(mousePos, count, rects);
  if (cmd != CMD_NONE) {
    if (now >= mouseIn.nextTime && IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
      mouseIn.nextTime = now + mouseIn.interval;
      return cmd;
    }
  }

  return CMD_NONE;
}

bool InputMouseMenu(Vector2 point) {
  Menu *menu = CurrentMenu();

  for (int i = 0; i < menu->itemCount; i++) {
    MenuItem *item = menu->items[i];
    // contained?
    if (PointInRect(point, item->rect)) {
      if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
        menu->current = i;
      }
      menu->hoverItem = i;
      return true;
    }
  }
  return false;
}

#include <stdio.h>
void printarrows(int down) {
  printf("BUTTON DOWN %d\n", down);
  for (int i = 0; i < ARROW_COUNT; i++) {
    printf("%d: %0.2f %0.2f %0.2f %0.2f\n", i, arrowRects[i].x, arrowRects[i].y,
           arrowRects[i].width, arrowRects[i].height);
  }
}

Navigator InputMouseNav(double now, Vector2 point) {
  Navigator nav = {.cmd = CMD_NONE};
  for (int i = 0; i < NAV_COUNT; i++) {
    if (PointInRect(point, arrowRects[i])) {
      if (now >= mouseIn.nextTime && IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
        mouseIn.nextTime = now + mouseIn.interval;
        nav.cmd = i;
        // printarrows(nav.cmd);
        return nav;
      }
    }
  }
  return nav;
}
