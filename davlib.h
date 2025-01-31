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
  MESH_POLY,
  MESH_PLANE,
  MESH_CUBE,
  MESH_SPHERE,
  MESH_HEMISPHERE,
  MESH_CYLINDER,
  MESH_CONE,
  MESH_TORUS,
  MESH_KNOT,
  MESH_HEIGHTMAP,
  MESH_CUBICMAP,
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
  void (*unload)(struct Shape *);
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

#define CLAMPNUM(num, lo, hi) ((num >= hi) ? lo : (num < lo) ? hi - 1 : num)
#define VEC3_NULL ((Vector3){0})
#define IS_VEC3_EQUAL(v, u) ((v.x == u.x) && (v.y == u.y) && (v.z == u.z))

int InputNav(double now);

// gamepad
int InputGamepad(int count, const int *buttons, double now);
int InputGamepadNav(double now);
bool CheckButton(int button);
const char *AxisName(int axis);
const char *ButtonName(int button);
Vector3 ButtonsToVector(Vector3 vec, Vector3 base, float scale);
Vector3 AxesToVector(Vector3 vec, Vector3 base, float scale);
Rectangle ButtonsToRectangle(Rectangle rec, Rectangle recbase, float scale);

// keyboard
int InputKeys(int count, const int *keys, double now);
int InputKeyNav(double now);
Vector3 KeysToVector(Vector3 vec, Vector3 base, float scale);

void DrawShapes(int count, Shape *shapes[]);
void InitShapes(int count, Shape *shapes[]);
void DrawMenu(Menu *menu, Position position);
void NavigateMenu(Menu *menu, double now);

void print_vectors(const int count, ...);

///////////////////////////////////////////////////////////////////////////

#ifdef DAVLIB_IMPLEMENTATION
#undef DAVLIB_IMPLEMENTATION

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

#include "gamepad.c"
#include "keyboard.c"
#include "shape.c"
#include "vector.c"

#endif // DAVLIB_IMPLEMENTATION

#endif // DAVLIB_H_DEFINED
