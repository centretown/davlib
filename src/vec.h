#ifndef DAVLIB_VEC_H_DEFINED
#define DAVLIB_VEC_H_DEFINED
#include <raylib.h>

typedef struct Pad {
  bool registered;
  int axisCount;
  float axisValues[6];
  int lastPressed;
  char name[80];
} Pad;

Vector3 KeysToVector(Vector3 vec, Vector3 base, float scale);
Vector3 ButtonsToVector(Vector3 vec, Vector3 base, float scale);
Rectangle ButtonsToRectangle(Rectangle rec, Rectangle recbase, float scale);
Vector3 AxesToVector(Vector3 vec, Vector3 base, float scale);
bool CheckButton(int button);
const char *AxisName(int axis);
const char *ButtonName(int button);
extern Pad GamePads[];

#endif // DAVLIB_VEC_H_DEFINED
