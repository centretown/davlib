#ifndef DAV_LIB_DEFINED
#define DAV_LIB_DEFINED

#include <raylib.h>

#include "src/gpads.h"
#include "src/kbd.h"

#ifdef DAV_LIB_IMPLEMENTATION
#undef DAV_LIB_IMPLEMENTATION

#include "src/gpads.c"
#include "src/kbd.c"

#endif // DAV_LIB_IMPLEMENTATION

#endif // DAV_LIB_DEFINED
