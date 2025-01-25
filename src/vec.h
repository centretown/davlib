#ifndef VEC_COOL_DEFINED
#define VEC_COOL_DEFINED
#include <raylib.h>
typedef struct {
  bool registered;
  int axisCount;
  float axisValues[6];
  int lastPressed;
  char name[80];
} Pad;

Vector3 KeysToVector(Vector3 vec, float scale);
Vector3 ButtonsToVector(Vector3 vec, float scale);
Vector3 AxesToVector(Vector3 vec, float scale);
inline const char *AxisName(int axis);
inline const char *ButtonName(int button);
extern Pad GamePads[4];
#endif // VEC_COOL_DEFINED
