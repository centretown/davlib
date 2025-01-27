#ifndef DAVLIB_MENU_H_DEFINED
#define DAVLIB_MENU_H_DEFINED

#include <raylib.h>
typedef struct Position {
  int x;
  int y;
} Position;

typedef struct Theme {
  int fontSize;
  Color titleColor;
  Color labelColor;
  Color valueColor;
  Color titleActive;
  Color labelActive;
  Color valueActive;
  Color backgroundColor;
} Theme;

typedef struct MenuItem {
  const char *label;
  char *value;
  struct MenuItem *items;
  int current;
  int itemCount;
} MenuItem;

typedef struct Menu {
  Position position;
  int valueColumn;
  const char *title;
  int current;
  int itemCount;
  MenuItem *items;
} Menu;

void DrawMenu(Theme *theme, Menu *menu, Position position, double now);
#define CMD_NONE -1
typedef enum NavCmd {
  NAV_NONE = CMD_NONE,
  NAV_LEFT = 0,
  NAV_RIGHT,
  NAV_UP,
  NAV_DOWN,
  NAV_SELECT,
  NAV_ESCAPE,
} NavCmd;

int InputGamepad(double now, int count, const int *buttons);
int InputNav(double now);
int InputGamepadNav(double now);
int ClampInt(int current, int min, int max);
int InputKeyNav(double now);
int InputKeys(int count, const int *keys, double now);

#endif // DAVLIB_MENU_H_DEFINED
