
/* definitions for process control variable */
#define LOCK_SCAN  process_control|=SCAN_LOCKOUT;
#define FREE_SCAN  process_control&=~SCAN_LOCKOUT;
#define SET_SCAN   process_control|=CONTEXT_SCAN;
#define CLEAR_SCAN process_control&=~CONTEXT_SCAN;
#define SET_MANIP  process_control|=TCB_MANIP;
#define CLEAR_MANIP process_control&=~TCB_MANIP;
#define   SET_RESRUN process_control|=RES_RUN;
#define CLEAR_RESRUN process_control&=~RES_RUN;
#define CONTEXT_SCAN 1
#define TCB_MANIP    2
#define SCAN_LOCKOUT 4
#define RES_RUN      8

#define ROUTINEMAX 16
#define NAMEMAX 8
#define ENV_VAR_MAX 12
#define ENV_PARAM_MAX 30
#define DEVICE_MAX 12
#define MODULE   0
#define ENVIRON  1
#define NODE     2
#define DEVICE   3
#define BLOCK    4
#define ROUTINE  5
#define NULLS    6

#define CHILD 0
#define BROTHER 1
#define INDEPENDANT 2
#define PARENT 3
#define AUTOREL 0x80
#define MINSTACK 512


typedef (far *proc)();

typedef long time_struc;
typedef struct routine_ptr
{
  char name[ROUTINEMAX];
  proc procedure;
  short data_seg;
  struct routine_ptr far *next;
}routine_ptr;

typedef struct environ_entry
{
  char name[ENV_VAR_MAX];
  char parameter[ENV_PARAM_MAX];
  struct environ_entry far *next;
}environ_entry;

typedef struct module
{
  char name[NAMEMAX];                 /* 0*/
  struct module far *child;           /* 8*/
  struct module far *elder_sib;       /* c*/
  struct module far *parent;          /*10*/
  struct module far *next;            /*14*/
  struct module far *prior;           /*18*/
  short far *stack;                   /*1c*/
  unsigned char far *code;            /*20*/
  unsigned short size;                /*24*/
  unsigned short stack_size;          /*26*/
  unsigned short max_size;            /*28*/
  unsigned long status;               /*2a*/
  unsigned short stackseg,stackofs;   /*2e*/
  unsigned long Acumulated_time;      /*34*/
  unsigned char Priority;             /*35*/
  unsigned char options;              /*36*/
  environ_entry far *environ;         /*40*/
  char far *device_name;              /*44*/
  char far *load_path;                /*48*/
                                      /*xx*/
} module;

typedef (far *forkhook)(module far *original,module far *new);
typedef (far *destroyhook)(module far *current);

typedef struct hook_ptr
{
  forkhook forkproc;
  destroyhook destroyproc;
  struct hook_ptr far *next;
}hook_ptr;


typedef struct node_entry
{
  short status;
  unsigned char far *data;
  module far *source;  /*this is the address of the module that put up
                         the request.  This is used for priviledge checks
                         when a node is terminated, and also in the case
                         of reads, when the read is complete, then this
                         is used to wake up the module without haveing to
                         search all people.*/
  struct node_entry far *next,far *last;
} node_entry;

typedef struct device_tree
{
  struct device_tree far *less,far *more;
  module far *Destination_Task;
  char dest[DEVICE_MAX];
  node_entry far *node;
} device_tree;
