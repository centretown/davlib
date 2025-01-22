#ifndef CMD_COOL_DEFINED
#define CMD_COOL_DEFINED

#define QUEUE_MAX 16

typedef enum {
  CMD_UNDEFINED = -1,
  CMD_NONE = 0,
  CMD_MOVE_LEFT,
  CMD_MOVE_RIGHT,
  CMD_MOVE_UP,
  CMD_MOVE_DOWN,
  CMD_MOVE_IN,
  CMD_MOVE_OUT,
  CMD_MOVE_HOME,
} Cmd;

typedef struct {
  Cmd Queue[QUEUE_MAX];
  int Pos;
} CmdQueue;

#endif // CMD_COOL_DEFINED
