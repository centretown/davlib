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
  Color titleHover;
  Color labelHover;
  Color valueHover;
  Color backgroundColor;
} Theme;

typedef enum MenuItemType {
  MENU_SUB,
  MENU_CHOICE,
  MENU_FLOAT,
  MENU_INT,
} MenuItemType;

typedef struct MenuItem {
  const char *label;
  MenuItemType itemType;
  union {
    struct {
      struct MenuItem **items;
      int itemCurrent;
      int itemCount;
    };
    struct {
      char **choices;
      int choiceCurrent;
      int choiceCount;
    };
    struct {
      float fvalue;
      float fmin;
      float fmax;
      float finc;
    };
    struct {
      int ivalue;
      int imin;
      int imax;
      int iinc;
    };
  };
  void (*onChoose)(void *);
  Rectangle rect;
} MenuItem;

typedef enum ArrowDirection {
  ARROW_LEFT,
  ARROW_RIGHT,
} ArrowDirection;

typedef struct Menu {
  Vector2 mousePos;
  Position position;
  const char *title;
  MenuItem **items;
  Theme *theme;
  void *custom; // passed to onChoose in MenuItem

  Color colorDim;
  Color colorHover;

  Rectangle arrowRects[2];
  Texture2D leftArrow;
  Texture2D rightArrow;

  int current;
  int hoverItem;
  int itemCount;
  int valueColumn;
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
  MESH_FIRST = MESH_POLY,
} ShapeTypeID;

typedef struct Shape {
  Vector3 rate;
  Vector3 home;
  ShapeTypeID typeID;
  // direction bearing
  void *shapePtr;
  void (*Draw)(struct Shape *);
  void (*Move)(struct Shape *, Vector3 v);
  Vector3 (*Position)(struct Shape *);
  void (*Unload)(struct Shape *);
  // void (*Colour)(struct Shape *, Color);
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
  NAV_LEFT = 0,
  NAV_RIGHT,
  NAV_UP,
  NAV_DOWN,
  NAV_SELECT,
  NAV_ESCAPE,
} NavCmd;

typedef struct MeshShape {
  Mesh mesh;
  Material material;
  Matrix matrix;
  Vector3 position;
  Vector3 rotationAxis;
  float rotationAngle;
  Vector3 scale;
  Color tint;
  void *genPtr;
} MeshShape;

typedef struct MeshPoly {
  int sides;
  float radius;
} MeshPoly;

typedef struct MeshPlane {
  float width;
  float length;
  int resX;
  int resZ;
} MeshPlane;

typedef struct MeshCube {
  float width;
  float height;
  float length;
} MeshCube;

typedef struct MeshSphere {
  float radius;
  int rings;
  int slices;
} MeshSphere;

typedef struct MeshHemisphere {
  float radius;
  int rings;
  int slices;
} MeshHemisphere;

typedef struct MeshCylinder {
  float radius;
  float height;
  int slices;
} MeshCylinder;

typedef struct MeshCone {
  float radius;
  float height;
  int slices;
} MeshCone;

typedef struct MeshTorus {
  float radius;
  float size;
  int radSeg;
  int sides;
} MeshTorus;

typedef struct MeshKnot {
  float radius;
  float size;
  int radSeg;
  int sides;
} MeshKnot;

typedef struct MeshHeightmap {
  Image heightmap;
  Vector3 size;
} MeshHeightmap;

typedef struct MeshCubicmap {
  Image cubicmap;
  Vector3 cubeSize;
} MeshCubicmap;

#define CLAMPNUM(num, lo, hi) ((num >= hi) ? lo : (num < lo) ? hi - 1 : num)
#define VEC3_NULL ((Vector3){0})
#define IS_VEC3_EQUAL(v, u) ((v.x == u.x) && (v.y == u.y) && (v.z == u.z))

int InputNav(double now);

// gamepad
bool IsGamePadValid(int pad);
int InputGamepad(int count, const int *buttons, double now);
int InputGamepadNav(double now);
bool CheckButton(int button);
const char *AxisName(int axis);
const char *ButtonName(int button);
Vector3 ButtonsToVector(Vector3 vec, Vector3 base, float scale);
Vector3 AxesToVector(Vector3 vec, float scale);
Rectangle ButtonsToRectangle(Rectangle rec, Rectangle recbase, float scale);

// keyboard
int InputKeys(int count, const int *keys, double now);
int InputKeyNav(double now);
Vector3 KeysToVector(Vector3 vec, Vector3 base, float scale);

// mouse
int InputMouse(int count, const Rectangle *rects, double now, Vector2 mousePos);
void InputMouseMenu(Menu *menu, double now);

void DrawShapes(int count, Shape *shapes[]);
void InitShapes(int count, Shape *shapes[], Material material);

void DrawMenu(Menu *menu, Position position);
void NavigateMenu(int cmd, Menu *menu, double now);

void print_vectors(const int count, ...);

void InitMeshShape(Shape *shape, Material material);

bool PointInRect(Vector2 point, Rectangle rect);

///////////////////////////////////////////////////////////////////////////

#ifdef DAVLIB_IMPLEMENTATION

#include <stdarg.h>
#include <stdio.h>

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
#include "mesh.c"
#include "navigate.c"
#include "shape.c"

#endif // DAVLIB_IMPLEMENTATION

#endif // DAVLIB_H_DEFINED
