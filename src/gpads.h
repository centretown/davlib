#ifndef GPADS_COOL_DEFINED
#define GPADS_COOL_DEFINED

#include <stdio.h>
#include "raylib.h"
#include "cmd.h"

typedef struct
{
    bool found;
    int axisCount;
    int lastPressed;
    char name[80];
    CmdQueue cmds;
    int Qsize;
} Pad;

Cmd CheckGamePads(); // detect gamepads

#endif // GPADS_COOL_DEFINED
