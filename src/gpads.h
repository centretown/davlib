#ifndef GPADS_COOL_DEFINED
#define GPADS_COOL_DEFINED

#include <stdbool.h>

typedef struct {
  bool registered;
  int axisCount;
  float axisValues[6];
  int lastPressed;
  char name[80];
} Pad;

typedef struct {
  int pad;
  int button;
  int command;
} PadButtonResult;

PadButtonResult CheckAllGamePadButtons(int count, const int *list);

typedef struct {
  int pad;
  int axis;
  int command;
  float value;
} PadAxisResult;

PadAxisResult CheckGamePadAxes(int count, const int *list);
#endif // GPADS_COOL_DEFINED
