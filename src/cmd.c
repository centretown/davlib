#include "cmd.h"

void PushQ(CmdQueue *q, Cmd cmd)
{
    if (q->Pos < QUEUE_MAX)
    {
        q->Queue[q->Pos++] = cmd;
    }
}

Cmd PopQ(CmdQueue *q)
{
    if (q->Pos > 0)
    {
        return q->Queue[q->Pos--];
    }
    return CMD_UNDEFINED;
}