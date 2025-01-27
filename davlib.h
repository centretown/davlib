#ifndef DAVLIB_H_DEFINED
#define DAVLIB_H_DEFINED

#include <raylib.h>

#include "src/vec.h"
#include "src/menu.h"

#ifdef DAVLIB_IMPLEMENTATION
#undef DAVLIB_IMPLEMENTATION

#include "src/vec.c"
#include "src/menu.c"

#endif // DAVLIB_IMPLEMENTATION

#endif // DAVLIB_H_DEFINED
