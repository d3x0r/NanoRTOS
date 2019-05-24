
/*typedef struct Node_holder
{
  Node far *Node;
  struct Node_holder far *next;
} Node_holder;*/

typedef struct threads_active
{
  short id;  /*channel number */
  char status;
  struct threads_active far *next;
  Node_holder far *Nodeq;
} threads_active;

#define STORING 1

#ifndef COMMON
  extern threads_active far *first_thread;
  extern void store_node(Node far *what,threads_active far *where);
  extern Node far *get_old_node(short id);
  extern threads_active far *thread_active(short which);
  extern void deactivate(short what);
  extern void check_outstanding(threads_active far *where);
  extern void Slough_work(Node far *what);
#endif

