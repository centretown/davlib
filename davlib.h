#ifndef DAVLIB_H_DEFINED
#define DAVLIB_H_DEFINED

#include <raylib.h>
#include <raymath.h>

typedef struct Pad {
  bool registered;
  int axisCount;
  float axisValues[6];
  int lastButtonPressed;
  double lastButtonTime;
  float lastAxisValue;
  double lastAxisTime;
  double buttonDelay;
  char name[80];
} Pad;

Vector3 KeysToVector(Vector3 vec, Vector3 base, float scale);
Vector3 ButtonsToVector(Vector3 vec, Vector3 base, float scale);
Rectangle ButtonsToRectangle(Rectangle rec, Rectangle recbase, float scale);
Vector3 AxesToVector(Vector3 vec, Vector3 base, float scale);
bool CheckButton(int button);
const char *AxisName(int axis);
const char *ButtonName(int button);

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
  int currentChoice;
  int choiceCount;
  char **choices;
  void (*onChoose)(void *, int, int);
} MenuItem;

typedef struct Menu {
  Position position;
  int valueColumn;
  const char *title;
  int current;
  int itemCount;
  MenuItem **items;
  Theme *theme;
  void *custom; // passed to onChoose in MenuItem
} Menu;

// void DrawModel(Model model, Vector3 position, float scale, Color tint);
typedef enum ShapeTypeID {
  CUBE,
  SPHERE,
  CYLINDER,
  CAPSULE,
} ShapeTypeID;

typedef struct Shape {
  ShapeTypeID typeID;
  double rate;
  Vector3 home;
  // direction bearing
  void *shapePtr;
  void (*draw)(struct Shape *);
  void (*move)(struct Shape *, Vector3 v);
  Vector3 (*position)(struct Shape *);
  // void (*color)(struct Shape *, Color);
} Shape;

typedef struct CubeShape {
  Vector3 position;
  Vector3 size;
  Color color;
} CubeShape;

typedef struct SphereShape {
  Vector3 position;
  float radius;
  Color color;
} SphereShape;

typedef struct CylinderShape {
  Vector3 position;
  float radiusTop;
  float radiusBottom;
  float height;
  int slices;
  Color color;
} CylinderShape;

typedef struct CapsuleShape {
  Vector3 startPos;
  Vector3 endPos;
  float radius;
  int slices;
  int rings;
  Color color;
} CapsuleShape;

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

void DrawShapes(int count, Shape *shapes[]);
void InitShapes(int count, Shape *shapes[]);
void DrawMenu(Menu *menu, Position position);
void NavigateMenu(Menu *menu, double now);

int InputGamepad(int count, const int *buttons, double now);
int InputGamepadNav(double now);
int InputNav(double now);
int InputKeys(int count, const int *keys, double now);
int InputKeyNav(double now);

#define CLAMPNUM(num, lo, hi) ((num >= hi) ? lo : (num < lo) ? hi - 1 : num)
#define VEC3_NULL ((Vector3){0})
#define IS_VEC3_EQUAL(v, u) ((v.x == u.x) && (v.y == u.y) && (v.z == u.z))

///////////////////////////////////////////////////////////////////////////

#ifdef DAVLIB_IMPLEMENTATION
#undef DAVLIB_IMPLEMENTATION

#include <assert.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>

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

// int navigationKeys[] = {KEY_LEFT, KEY_RIGHT, KEY_UP, KEY_PAGE_UP,
//                         KEY_PAGE_DOWN};
// size_t navigationKeysSize = sizeof(navigationKeys) /
// sizeof(navigationKeys[0]);

Vector3 KeysToVector(Vector3 vec, Vector3 base, float scale) {
  float delta = scale;
  if (IsKeyDown(KEY_LEFT)) {
    vec.x -= delta;
    return vec;
  }
  if (IsKeyDown(KEY_RIGHT)) {
    vec.x += delta;
    return vec;
  }
  if (IsKeyDown(KEY_UP)) {
    vec.y += delta;
    return vec;
  }
  if (IsKeyDown(KEY_DOWN)) {
    vec.y -= delta;
    return vec;
  }
  if (IsKeyDown(KEY_PAGE_UP)) {
    vec.z += delta;
    return vec;
  }
  if (IsKeyDown(KEY_PAGE_DOWN)) {
    vec.z -= delta;
    return vec;
  }
  if (IsKeyDown(KEY_HOME)) {
    vec.x = base.x;
    vec.y = base.y;
    vec.z = base.z;
    return vec;
  }
  return vec;
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

void DrawMenu(Menu *menu, Position position) {
  assert(menu);
  assert(menu->items);
  assert(menu->itemCount >= 0);
  Theme *theme = menu->theme;
  assert(theme);

  int fontSize = theme->fontSize;
  int baseX = menu->position.x * fontSize + position.x * fontSize;
  int baseY = menu->position.y * fontSize + position.y * fontSize;
  DrawText(menu->title, baseX, baseY, fontSize, theme->titleColor);

  int y = baseY + fontSize + fontSize / 3;
  for (int i = 0; i < menu->itemCount; i++) {
    MenuItem *item = menu->items[i];
    int x = baseX + 2 * fontSize;
    DrawText(item->label, x, y, fontSize,
             (i == menu->current) ? theme->labelActive : theme->labelColor);
    x = baseX + menu->valueColumn * fontSize;

    if (item->choices) {
      DrawText(item->choices[item->currentChoice], x, y, fontSize,
               (i == menu->current) ? theme->valueActive : theme->valueColor);
    } else {
      DrawText(TextFormat("%d", item->currentChoice), x, y, fontSize,
               (i == menu->current) ? theme->valueActive : theme->valueColor);
    }
    y += fontSize;
  }
}

void NavigateMenu(Menu *menu, double now) {
  int cmd = InputNav(now);
  MenuItem *item = menu->items[menu->current];
  assert(item);

  if (CMD_NONE != cmd) {
    switch (cmd) {
    case NAV_LEFT:
    case NAV_RIGHT:
      item->currentChoice += (cmd == NAV_RIGHT) ? 1 : -1;
      item->currentChoice = CLAMPNUM(item->currentChoice, 0, item->choiceCount);
      if (item->onChoose) {
        item->onChoose(menu, menu->current, item->currentChoice);
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
}

int InputGamepad(int count, const int *buttons, double now) {
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
  return InputGamepad(navButtonsSize, navButtons, now);
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

void DrawCubeShape(Shape *shape) {
  CubeShape *cube = shape->shapePtr;
  DrawCubeV(cube->position, cube->size, cube->color);
}
void DrawSphereShape(Shape *shape) {
  SphereShape *sphere = shape->shapePtr;
  DrawSphere(sphere->position, sphere->radius, sphere->color);
}
void DrawCylinderShape(Shape *shape) {
  CylinderShape *cylinder = shape->shapePtr;
  DrawCylinder(cylinder->position, cylinder->radiusTop, cylinder->radiusBottom,
               cylinder->height, cylinder->slices, cylinder->color);
}
void DrawCapsuleShape(Shape *shape) {
  CapsuleShape *capsule = shape->shapePtr;
  DrawCapsule(capsule->startPos, capsule->endPos, capsule->radius,
              capsule->slices, capsule->rings, capsule->color);
}
void MoveCubeShape(Shape *shape, Vector3 position) {
  CubeShape *cube = shape->shapePtr;
  cube->position = position;
}
Vector3 CubeShapePosition(Shape *shape) {
  CubeShape *cube = shape->shapePtr;
  return cube->position;
}
void MoveSphereShape(Shape *shape, Vector3 position) {
  SphereShape *sphere = shape->shapePtr;
  sphere->position = position;
}
Vector3 SphereShapePosition(Shape *shape) {
  SphereShape *sphere = shape->shapePtr;
  return sphere->position;
}
void MoveCylinderShape(Shape *shape, Vector3 position) {
  CylinderShape *cylinder = shape->shapePtr;
  cylinder->position = position;
}
Vector3 CylinderShapePosition(Shape *shape) {
  CylinderShape *cylinder = shape->shapePtr;
  return cylinder->position;
}

#include <stdarg.h>
void print_vectors(const int count, ...) {
  va_list args;
  va_start(args, count);
  for (int i = 0; i < count; i++) {
    Vector3 v = va_arg(args, Vector3);
    printf("%d: {%.2f, %.2f, %.2f}\n", i + 1, v.x, v.y, v.z);
  }
  va_end(args);
  printf("\n");
}

void MoveCapsuleShape(Shape *shape, Vector3 position) {
  CapsuleShape *capsule = shape->shapePtr;
  Vector3 diff = Vector3Subtract(position, capsule->startPos);
  // print_vectors(2, capsule->startPos, capsule->endPos);
  capsule->startPos = position;
  capsule->endPos = Vector3Add(diff, capsule->endPos);
  // print_vectors(3, diff, capsule->startPos, capsule->endPos);
}
Vector3 CapsuleShapePosition(Shape *shape) {
  CapsuleShape *capsule = shape->shapePtr;
  return capsule->startPos;
}

void InitShape(Shape *shape) {
  switch (shape->typeID) {
  case CUBE:
    shape->draw = DrawCubeShape;
    shape->move = MoveCubeShape;
    shape->position = CubeShapePosition;
    break;

  case SPHERE: {
    shape->draw = DrawSphereShape;
    shape->move = MoveSphereShape;
    shape->position = SphereShapePosition;
    break;
  }
  case CYLINDER: {
    shape->draw = DrawCylinderShape;
    shape->move = MoveCylinderShape;
    shape->position = CylinderShapePosition;
    break;
  }
  case CAPSULE: {
    shape->draw = DrawCapsuleShape;
    shape->move = MoveCapsuleShape;
    shape->position = CapsuleShapePosition;
    break;
  }
  default:
    assert(0);
    return;
  }

  shape->home = shape->position(shape);
}

void InitShapes(int count, Shape *shapes[]) {
  for (int i = 0; i < count; i++) {
    InitShape(shapes[i]);
  }
}

void DrawShapes(int count, Shape *shapes[]) {
  assert(count >= 0);
  assert(shapes);
  for (int i = 0; i < count; i++) {
    Shape *shape = shapes[i];
    shape->draw(shape);
  }
}

#endif // DAVLIB_IMPLEMENTATION

#endif // DAVLIB_H_DEFINED
