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

typedef struct Theme {
  Color titleColor;
  Color panelColor;
  Color labelColor;
  Color labelActive;
  Color labelHover;
  Color valueColor;
  Color valueActive;
  Color valueHover;
  Color backgroundColor;
  Color colorDim;
  Color colorHover;

  Texture2D leftArrow;
  Texture2D rightArrow;
  Texture2D inArrow;
  Texture2D outArrow;
  Texture2D menuInactive;
  Texture2D menuActive;
  Texture2D doubleArrowRight;
  Texture2D doubleArrowLeft;
  int fontSize;
  int padding;
  float valueColumn;
} Theme;

typedef enum MenuItemType {
  MENU_SUB,
  MENU_CHOICE,
  MENU_FLOAT,
  MENU_INT,
} MenuItemType;

struct Menu;

typedef struct MenuItem {
  const char *label;
  MenuItemType itemType;
  void (*onChoose)(struct Menu *);
  void (*onDraw)(struct MenuItem *item, Rectangle rect, int fontSize,
                 Color color);
  Rectangle rect;
  void *data;
  union {
    struct {
      struct Menu *menu;
      void (*onPush)(struct Menu *);
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
} MenuItem;

typedef enum ArrowDirection {
  ARROW_LEFT,
  ARROW_RIGHT,
  ARROW_UP,
  ARROW_DOWN,
  ARROW_HOME,
  ARROW_BACK,
  ARROW_COUNT,
} ArrowDirection;

#define CMD_NONE -1

typedef enum NavCmd {
  NAV_LEFT,
  NAV_RIGHT,
  NAV_UP,
  NAV_DOWN,
  NAV_HOME,
  NAV_BACK,
  NAV_COUNT,
} NavCmd;

typedef struct Navigator {
  NavCmd cmd;
  float ratio;
} Navigator;

#define MENU_STACK_SIZE 8

typedef struct Menu {
  const char *title;
  MenuItem **items;
  void *data;

  // calculated
  int current;
  int hoverItem;
  int itemCount;
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

typedef struct TimedInput {
  double interval;
  double lastTime; // when last input set
  double nextTime; // when last input set
  int lastInput;   // set to 0 after interval
} TimedInput;

#define CLAMPNUM(num, lo, hi) ((num >= hi) ? lo : (num < lo) ? hi - 1 : num)
#define VEC3_NULL ((Vector3){0})
#define IS_VEC3_EQUAL(v, u) ((v.x == u.x) && (v.y == u.y) && (v.z == u.z))

Navigator InputNav(double now, Vector2 point);
int InputMouse(int count, const Rectangle *rects, double now, Vector2 mousePos);

// gamepad
bool IsGamePadValid(int pad);
Navigator InputGamepad(int count, const int *buttons, double now);
Navigator InputGamepadNav(double now);
bool CheckButton(int button);
const char *AxisName(int axis);
const char *ButtonName(int button);
Vector3 ButtonsToVector(Vector3 vec, Vector3 base, float scale);
Vector3 AxesToVector(Vector3 vec, float scale);
Rectangle ButtonsToRectangle(Rectangle rec, Rectangle recbase, float scale);

// keyboard
Navigator InputKeys(int count, const int *keys, double now);
Navigator InputKeyNav(double now);
Vector3 KeysToVector(Vector3 vec, Vector3 base, float scale);

// mouse
int InputMouse(int count, const Rectangle *rects, double now, Vector2 mousePos);
Navigator InputMouseNav(double now, Vector2 point);
bool InputMouseButton(Rectangle rect, double now, Vector2 point);
bool InputMouseMenu(Vector2 point);

void DrawShapes(int count, Shape *shapes[]);
void InitShapes(int count, Shape *shapes[], Material material);

void DrawMenu(Theme *theme, Vector2 position, Vector2 point);
void NavigateMenu(Navigator nav, double now);

void InitializeMenus();
Menu *CurrentMenu();
Menu *MenuTop();
Menu *PopMenu();
void PushMenu(Menu *menu);

void print_vectors(const int count, ...);

void InitMeshShape(Shape *shape, Material material);

bool PointInRect(Vector2 point, Rectangle rect);

// theme handlers
void OnChooseFont(Menu *menuptr);
void OnSetColorRed(Menu *menuptr);
void OnSetColorGreen(Menu *menuptr);
void OnSetColorBlue(Menu *menuptr);
void OnSetColorAlpha(Menu *menuptr);
// void OnPushTitleColor(Menu *menuptr);
// void OnPushPanelColor(Menu *menuptr);
// void OnPushTitleHover(Menu *menuptr);
// void OnPushLabelColor(Menu *menuptr);
// void OnPushLabelActive(Menu *menuptr);
// void OnPushLabelHover(Menu *menuptr);
// void OnPushValueColor(Menu *menuptr);
// void OnPushValueActive(Menu *menuptr);
// void OnPushValueHover(Menu *menuptr);
// void OnPushBackgroundColor(Menu *menuptr);
// void OnPushColorDim(Menu *menuptr);
// void OnPushColorHover(Menu *menuptr);

// OnChoose Handler Macros
#define MENU_ITEM_CHOICE(menuptr)                                              \
  Menu *menu = menuptr;                                                        \
  MenuItem *item = menu->items[menu->current];                                 \
  int choiceCurrent = item->choiceCurrent;

#define MENU_ITEM_FLOAT(menuptr)                                               \
  Menu *menu = menuptr;                                                        \
  MenuItem *item = menu->items[menu->current];                                 \
  float value = item->fvalue;

#define MENU_ITEM_INT(menuptr)                                                 \
  Menu *menu = menuptr;                                                        \
  MenuItem *item = menu->items[menu->current];                                 \
  int value = item->ivalue;

extern Theme *DAV_theme;
extern MenuItem *DAV_colorItems[];
extern Menu *DAV_themeMenu;
///////////////////////////////////////////////////////////////////////////

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
#include "menus.c"
#include "mesh.c"
#include "mouse.c"
#include "navigate.c"
#include "shape.c"

#endif // DAVLIB_IMPLEMENTATION

#endif // DAVLIB_H_DEFINED
