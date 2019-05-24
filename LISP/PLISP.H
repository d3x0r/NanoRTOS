#define false 0
#define true (!false)
#define NULL 0

#define NIL 0
#define LIST 1

typedef struct atom
{
  char type;
  char far *name;
  void far *data;
  struct atom far *next,far *prior,far *child,far *parent;
}atom;

#define LINE_SEG 50
typedef struct line_seg
{
  short idx;
  char data[LINE_SEG];
  struct line_seg far *next,far *prior;
}line_seg;
