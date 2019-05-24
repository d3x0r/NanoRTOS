#include "os.h"
char data_item;
#define DATA seg data_item
#define pre_def_routs 22
#define false 0
#define true !false
#define name_length 10  /*LOADER.EXE*/
#define near2far(a) (((long)routines&0xffff0000L)|(long)((short)a))
#undef near2far
#define near2far(a)  (a)
#define config_comment_char '!'

#ifdef memdebug
typedef struct node_action
{
  char I_O;
  char far *node_holder;
  char far *node_data;
  char queue[4];
}node_action;


node_action node_list[1000];
short node_idx;
#endif
char load_name[15];
char timed=false;

char far *path;
extern short errorcode;
typedef void (near nproc)();

void main(char far *params,char far *inpath);
void Shell(){};
void (far Exit_routine)(short code);

extern far swap_module(long parms);
/*extern near routine_not_avail();*/
typedef struct memory_struc
{
  char op_id;
  unsigned short size;
  unsigned short address;
  unsigned short time;
  char Taskname[4];
}memory_struc;
#ifdef memdebug
extern memory_struc Allocate_Trace[4095];
#endif
extern unsigned long ticks;
extern short stackend;
extern short (far compute_CCITT_asm)(short length,char far *line,short accum);
extern short (far compute_CRC16_asm)(short length,char far *line,short accum);
extern void (far swap_to)(module far *next_to_run);
extern void (far time_interupt)(void);
extern void (far seek)(short handle,long position,char type);
extern long (far ldiv)(long num,long divisor);
extern long (far lmod)(long num,long divisor);
extern long (far lmult)(long num1,long num2);
extern void (far gettime)(time_struc far *time);
extern void (far Exec_Services)(void);
extern void (far Connect_Int)(void far *routine,short int_num);
extern void (far Disconnect_Int)(void far *routine,short int_num);
extern short  (far atoi)(char far *line);
extern long   (far atol)(char far *line);
extern char (far itoa)(short number,char far *s);
extern short  (far strlen)(char far *line);
extern char far *(far strchr)(char far *line,char character);
extern void   (far strcpy)(char far *dest,char far *source);
extern void   (far strcat)(char far *dest,char far *source);
extern short  (far strncpy)(char far *source,
                            char far *dest,
                            short length); /*returns number
                                             of characters copied*/
extern void   (far strncat)(char far *dest,char far *source,short length);
extern short  (far strnicmp)(char far *s1,char far *s2,short count);
//extern short  (far strcmp)(char far *s1,char far *s2);
extern short  (far stricmp)(char far *s1,char far *s2);
extern void   (far movmem)(void far *from,void far *to,short count);
extern void   (far memset)(void far *from,short value,short count);
extern void   (far print)(char far *line);
extern void   (far cprint)(char charac);
extern void   (far connect_stall)(void);
extern void   (far disconnect_stall)(void);
extern short  (far open)(char far *filename,short options);
extern short  (far create)(char far *filename,short options);
extern        (far close)(short handle);
extern short  (far sizefind)(short handle);
extern short  (far read)(void far *buffer,unsigned short length,short token);
extern short  (far write)(char far *buffer,unsigned short length,short token);
extern short  (far gets)(char far *buffer,short length,short token);
extern void   (far Free_mem)(void far *block);
extern void far *(far Allocate_mem)(short size);
extern short first_used;
extern short times_null;

long routines[pre_def_routs];

#define NULL 0L
module OS_module={"PNTHR/OS",
                  NULL,
                  NULL,
                  NULL,
                  &OS_module,
                  &OS_module,
                  NULL,            /* stack */
                  (unsigned char far *)&main,  /*code*/
                  0,512,0,  /*size,stack_size,max_stack*/
                  0L,   /*status*/
                  0,0,  /*stackpos*/
                  0,    /*time*/
                  0,    /*priority*/
                  0,    /*options*/
                  NULL, /*environ*/
                  NULL};/*device_name*/

routine_ptr far *first_routine;
routine_ptr far *routine_list;
hook_ptr far *hooks=NULL;  /*provide external routines for when a task is
                             created/forked, and destroyed.*/
module far *current_module=&OS_module,far *first_module=&OS_module;

node_entry far *nodes,far *temp_entry;
short max_node_used;

device_tree far *device_root=0L;

char loop_count;
short force_time=0;
short force_interval=50;
short process_control;

char far *final_dot(char far *string)
{
  char far *priordot=0,far *currentdot=string;
  while (currentdot=strchr(currentdot,'.'))
    priordot=currentdot++;
  return(priordot);
}

void add_hooks(destroyhook dproc,forkhook fproc)
{
  hook_ptr far *current;
  if (!hooks)
  {
    current=hooks=Allocate_mem(sizeof(hook_ptr));
    current->next=NULL;
  }
  else
  {
    current=hooks;
    while (current->next)
      current=current->next;
    current->next=Allocate_mem(sizeof(hook_ptr));
    current=current->next;
    current->next=NULL;
  }
  current->destroyproc=dproc;
  current->forkproc=fproc;
}

void far *(far Inquire_begin)(short what)
{
  void far *temp;
  asm push ds
  asm mov ax,DATA
  asm mov ds,ax
  switch(what)
  {
    case MODULE:
          temp=first_module;
          break;
    case ENVIRON:
          temp=current_module->environ;
          break;
    case NODE:
          temp=0L;
          break;
    case DEVICE:
          temp=device_root;
          break;
    case BLOCK:
          temp=(void far *)((long)first_used<<16);
          break;
    case ROUTINE:
          temp=first_routine;
          break;
    case NULLS:
          temp=&times_null;
          break;
    default:
          temp=0L;
          break;
  }
  asm pop ds;
  return(temp);
}

void (far delay)(unsigned short milliseconds)
{
  unsigned long end=ticks+milliseconds;
  if (end<milliseconds)
    while (end<ticks)
      swap_module(0);
  while (end>ticks)
    swap_module(0);
}


void (far wake_task)(short mask,short spec)
{
  module far *temp;
  asm push ds
  asm mov ax,DATA
  asm mov ds,ax
  temp=first_module;
  if (temp==0L) return;
  if (spec)
    do
    {
      if (((short)temp->status&mask)&&
          (((long)temp->status>>16)==spec))
        temp->status=0;
      temp=temp->next;
    }
    while (temp!=first_module);
  else
    do
    {
      if ((short)temp->status&mask)
        temp->status=0;
      temp=temp->next;
    }
    while (temp!=first_module);
  asm pop ds
}

void far change_priority(unsigned char priority)
{
  /*changes the current module to a new priority.  It gets added at the end
    of the tasks at that priority.*/
  unsigned char SPriority;
  module far *temp,far *next_run;
  asm push ds
  asm mov ax,DATA
  asm mov ds,ax
  SPriority=current_module->Priority;
  LOCK_SCAN;
  SET_MANIP;
  current_module->Priority=priority;
  temp=current_module;
  next_run=temp->next;

  temp->prior->next=temp->next;   /*unlink the current module block to move it*/
  temp->next->prior=temp->prior;

  if (current_module==first_module)
    current_module=current_module->next;
  while (current_module->Priority<=priority&&
         current_module!=first_module)  /*go up to the next_priority*/
    current_module=current_module->next;
  if (current_module==first_module)
    current_module=current_module->prior;
  while (current_module->Priority>priority&&
         current_module!=first_module)   /*go back one or more*/
    current_module=current_module->prior;

  temp->next=current_module->next;
  temp->prior=current_module;

  temp->next->prior=temp;
  temp->prior->next=temp;
  current_module=temp;
  FREE_SCAN;
  CLEAR_MANIP;
  if (SPriority<priority) swap_to(next_run);
  asm pop ds;
}

device_tree far *find_device_node(char far *devicename)
{
 device_tree far *temp=device_root;
 char d;
 char length=strlen(devicename);
 do
 {
   if ((d=strnicmp(temp->dest,devicename,length))==0)
     return(temp);
   if (d<0)
     temp=temp->more;
   else
     temp=temp->less;
  }while (temp);
  return(temp);
}

char far *add_device_node(char far *devicename)
{
  device_tree far *temp=device_root;
  device_tree far *prior;
  char length=strlen(devicename);
  char direction;
  if (length)
  {
    LOCK_SCAN;
    if (temp)
    {
      while (temp)
      {
         prior=temp;
         direction=strnicmp(devicename,temp->dest,length);
         if (direction>0)
           temp=temp->more;
         else
           if (direction<0)
             temp=temp->less;
           else
           {
             print("Device Duplicately Defined.\n");
             print(devicename);
             Exit_routine(1);
           }
      }
      if (strnicmp(devicename,prior->dest,length)>0)
      {
        prior->more=Allocate_mem(sizeof(device_tree));
        temp=prior->more;
      }
      else
      {
        prior->less=Allocate_mem(sizeof(device_tree));
        temp=prior->less;
      }
      goto fill_node;
    }
    else
    {
      temp=Allocate_mem(sizeof(device_tree));
      device_root=temp;
  fill_node:
      temp->more=0L;
      temp->less=0L;
      temp->Destination_Task=0L;
      strcpy(temp->dest,devicename);
      temp->node=0L;
    }
    FREE_SCAN
    return(temp->dest);
  }
  else
    return(0L);
}

short (far Export)(char far *destination,
               unsigned char far *packet)
{
  /*This routine takes the data passed to it and queues up some work.
    If the options indicate to do so, it copies the data buffer to another
    scratch buffer, so that the calling routine does not have to pause in
    order to save the data within it's buffer.  This is only aplicable on
    writes. */
  device_tree far *temp;
  node_entry far *node;
  char length;
  asm push ds
  asm mov ax,DATA
  asm mov ds,ax
try_again:
  while(!(temp_entry=Allocate_mem(sizeof(node_entry))))
      swap_module(0L);
#ifdef memdebug
  node_list[node_idx].I_O=0;
  node_list[node_idx].node_holder=temp_entry;
  node_list[node_idx].node_data=packet;
  *(long far *)&node_list[node_idx].queue[0]=*(long far *)destination;
  node_idx++;
  if (node_idx==1000) node_idx=0;
#endif
  length=strlen(destination);
  if (length>12) destination[12]=0;
  temp=find_device_node(destination);
  if (temp)
  {
    node=temp->node;
    if (node)
    {
      node=(node->last->next=temp_entry);
    }
    else
    {
      node=temp->node=temp_entry;
    }
    node->next=0L;
    temp->node->last=node;
    node->data=packet;
    node->status=0;
    if (temp->Destination_Task)
    {
      if (temp->Destination_Task->status&8)
        temp->Destination_Task->status=0;
      if (temp->Destination_Task->Priority<current_module->Priority)
        swap_to(temp->Destination_Task);
    }
    else
    {
      process_control|=1;
      wake_task(8,0);
    }
    asm pop ds
    return(true);
  }
  else
  {
    Free_mem(temp_entry);
  }
  asm pop ds
  return(false);
}

void far *(far Terminate(char far *whose,char position,
                         char bytes,short far *inprogress,...))
{
  /*This first located the node path for the particual device, whose
    node we are going to destroy.  Then it goes to the (position) of the
    data to check to see if it is the appropriate node, it then compares
    the (bytes) number of values with extra parameters (...[]), if it
    compares, it looks to see if it is already being processed, if so, then
    it just returns the address of the data, and does nothing to the node.
    If it was not found then it returns a NULL.
    if it was found, and not processed, it marks it as having been read
      once and returns a the data ptr.
         (we are never going to get the node,
          so, we need to send our own terminate ack)*/
  device_tree far *temp;
  node_entry far *node;
  char cnt;
  char length;
  asm push ds
  asm mov ax,DATA
  asm mov ds,ax
length=strlen(whose);
  if (length>12) whose[12]=0;
  temp=find_device_node(whose);
  if (temp)
  {
    node=temp->node;
    while (node)
    {
      for (cnt=0;cnt<bytes;cnt++)          /*comare the bytes*/
        if (node->data[position+cnt]!=(char)...[cnt])
          continue;
      if (cnt==bytes)                      /*if all matched */
      {
        *inprogress=node->status&1;
        node->status|=1;         /*mark as read so the
                                    device doesn't get it*/
        asm pop ds;
        return(node->data);
      }
      node=node->next;
    }
    asm pop ds;
    return(0L);
  }
  asm pop ds;
  return(0L);
}

void far *(far Import)(char far *WhoIAm)
{
  /*This is the function that checks the nodes and returns the status of
  the work to the person who is asking for it.  There are options in the
  work for multiple people to access it, so this routine also decrements the
  multiple access counter.*/
  device_tree far *temp;
  node_entry far *node,far *last;
  char far *data=0;
  char length;
  asm push ds
  asm mov ax,DATA
  asm mov ds,ax
length=strlen(WhoIAm);
  if (length>12) WhoIAm[12]=0;
  temp=find_device_node(WhoIAm);
  if (temp)
  {
    do
    {
      temp->Destination_Task=current_module;
      node=temp->node;
      if (node)
      {
        if ((unsigned long)node>0xa0000000L)
          asm int 3;
        while (node->status&1)
        {
          last=temp->node->last;
          temp->node=node->next;
          Free_mem(node);
          node=temp->node;
          if (node)
            node->last=last;
          else goto null_return;
        }
#ifdef memdebug
  node_list[node_idx].I_O=1;
  node_list[node_idx].node_holder=node;
  node_list[node_idx].node_data=node->data;
  *(long far *)&node_list[node_idx].queue[0]=*(long far *)&temp->dest;
  node_idx++;
  if (node_idx==1000) node_idx=0;
#endif
        data=node->data;
        node->status|=1;
        break;
      }
      else
null_return:
        swap_module(8L);
    }
    while (1==1);
  }
  else
  {
    asm int 3;
    print("Error, This module is having an identity crisis!: ");
    print(WhoIAm);
    Exit_routine(1);
  }
  asm pop ds
  return(data);
}
void slow_clock(void);

char HEX[]="0123456789ABCDEF";
  char tstr[2]={0,0};

void (far Exit_routine)(short code)
{
  routine_ptr far *temp;
  short i;
  char codet[10];
  asm push ds
  asm mov ax,DATA
  asm mov ds,ax
#ifndef DEBUG
  slow_clock();
#endif
  disconnect_stall();
  if (code)
  {
#define Longshift(value,bits)  (bits>=16)?                           \
                                 ((short)(value>>16)>>bits-16):     \
                                 ((short)value>>bits)
#define code_print(what) cprint(HEX[Longshift(((long)what&0xf0000000),28)]); \
                         cprint(HEX[Longshift(((long)what&0x0f000000),24)]); \
                         cprint(HEX[Longshift(((long)what&0x00f00000),20)]); \
                         cprint(HEX[Longshift(((long)what&0x000f0000),16)]); \
                         cprint(HEX[Longshift(((long)what&0x0000f000),12)]); \
                         cprint(HEX[Longshift(((long)what&0x00000f00),8)]);  \
                         cprint(HEX[Longshift(((long)what&0x000000f0),4)]);  \
                         cprint(HEX[Longshift(((long)what&0x0000000f),0)]);
#define pname(what)  for(i=0;i<8,what->name[i];i++) cprint(what->name[i]); \
                     for(;i<8;i++) cprint(' ');   /*space fill the name if nulled*/
    temp=first_routine;
    while (temp)
    {
      if (temp->name[0]=='!')
      {
        temp->procedure();
      }
      temp=temp->next;
    }
#ifdef memdebug
    {
      short handle;
      char far *tname;
      tname=final_dot(load_name);
      if (tname)
        strcpy(tname,".MEM");
      else
        strcat(load_name,".MEM");
      handle=create(load_name,0);
      if (handle!=-1)
      {
        /*the minus 4 is to include the number of entries and the current entry*/
        write(((char far *)Allocate_Trace)-4,11*4096+2,handle);
        close(handle);
      }
    }
#endif
#define data_print(where) {               \
    code_print(where->code);              \
    print(" ");                           \
    code_print(where->child);             \
    print(" ");                           \
    code_print(where->parent);            \
    print(" ");                           \
    code_print(where->elder_sib);  }

    print("\r\nPosition      Name      Code     Child    Parent   Elder ");
    print("\r\nPrior Task:   ");
    pname(current_module->prior);
    print("  ");
    data_print(current_module->prior);
    print("\r\nCurrent_task: ");
    pname(current_module);
    print("  ");
    data_print(current_module);
    print("\r\nNext Task:    ");
    pname(current_module->next);
    print("  ");
    data_print(current_module->next);
    print("\r\nExiting via Loader...Code:");
    itoa(code,codet);
    print(codet);
    print("\r\n");
    print("Stack Dump:");
    for (i=0;i<30;i++)
    {
      tstr[0]=HEX[(*((short far *)&code-2+i)&0xf000)>>12];
      print(tstr);
      tstr[0]=HEX[(*((short far *)&code-2+i)&0x0f00)>>8];
      print(tstr);
      tstr[0]=HEX[(*((short far *)&code-2+i)&0x00f0)>>4];
      print(tstr);
      tstr[0]=HEX[(*((short far *)&code-2+i)&0x000f)>>0];
      print(tstr);
      tstr[0]=' ';
      print(tstr);
    }
  }
  else
    print("Error Terminate");
  Disconnect_Int(Exec_Services,0x60);
  exit(code);
  asm pop ds;
}

short (far check_avail_now)(unsigned char far *routine_name)
{
  routine_ptr far *temp;
  char  length;
  asm push  ds;
  asm mov ax,DATA;
  asm mov ds,ax;
  temp=first_routine;
  length=strlen(routine_name);
  if (length>12) length=12;
  while (temp)
  {
    if (strnicmp(temp->name,routine_name,length)==0)
    {
      asm pop ds;
      return(true);
    }
    temp=temp->next;
  }
  asm pop ds;
  return(false);
}


void void_routine()
{
  print("There is no routine here...\r\n");
}

void far *(far Request)(char far *routine_name)
{
  routine_ptr far *temp;
  char count;
  asm push ds
  asm mov ax,DATA
  asm mov ds,ax
  count=strlen(routine_name);
  if (count>12) count=12;
  do
  {
    temp=first_routine;
    while (temp)

    {
      char s;
      if ((strnicmp(temp->name,routine_name,count)==0)&&
           (((s=strlen(temp->name))>12?12:s)==count))
      {
        (proc)temp=(proc)&(temp->procedure);
        break;
      }
      temp=temp->next;
    }
    if (temp==0L)
      swap_module(2);
  }
  while (temp==0L);
  asm pop ds
  return((proc)temp);
}

mem_check()
{
  asm push ds
  asm mov ax,DATA
  asm mov ds,ax
  if (check_avail_now("MEMDIAG"))
  {
    ((proc)(Request("MEMDIAG")))(1);
  }
  asm pop ds;
}


void (far Register)(char far *name,proc address,short Dseg)
{
  routine_ptr far *temp;
  routine_ptr far *prior=0L;
  char count=strlen(name);
  asm push ds
  asm mov ax,DATA
  asm mov ds,ax
  temp=first_routine;
  if (count>=ROUTINEMAX)
  {
    count=ROUTINEMAX;
  }
  else
    count++;  /*add 1 for the null to copy*/
  while (temp)
  {
    if ((strnicmp(temp->name,name,count)==0)&&
        (strlen(temp->name)==count))
    {
      temp->procedure=address;
      break;
    }
    else
      if (strnicmp(temp->name,name,count)>0)
      {
        if (prior)
        {
new:
          prior->next=Allocate_mem(sizeof(routine_ptr));
          prior=prior->next;
        }
        else
        {
          prior=Allocate_mem(sizeof(routine_ptr));
          first_routine=prior;
        }
        prior->next=temp;
        prior->procedure=address;
        prior->data_seg=Dseg;
        strncpy(prior->name,name,count);
        temp=prior;
        break;
      }
    prior=temp;
    temp=temp->next;
  }
  if (temp==0L)
  {
    goto new;
  }
  wake_task(2,0);
  asm pop ds;
}

void process_entry(char far *line,environ_entry far **queue)
{
  environ_entry far *temp;
  char i;
  temp=(environ_entry far*)Allocate_mem(sizeof(environ_entry));
  temp->next=*queue;
  *queue=temp;
  while (line[0]==' '||line[0]=='\t'||line[0]=='\n') line++;
  if (!line[0])
    return;
  for (i=0;(i<12)&&(line[i]!='=');i++)
    temp->name[i]=line[i];
  while ((i)&&((temp->name[i-1]==' ')||(temp->name[i-1]=='\t')))
    i--;
  temp->name[i]=0;
  while (line[0]!='=') line++;
  line++;
  while (line[0]==' '||line[0]=='\t') line++;
  for (i=0;(i<30)&&(line[i]!=10)&&(line[i]);i++)
    temp->parameter[i]=line[i];
  while ((i)&&((temp->parameter[i-1]==' ')||(temp->parameter[i-1]=='\t')))
    i--;
  temp->parameter[i]=0;
}

char far *(far get_environ)(char far *name)
{
  char i;
  environ_entry far *temp;
  asm push ds
  asm mov ax,DATA
  asm mov ds,ax
  temp=current_module->environ;
  while (temp)
  {
    if (strnicmp(name,temp->name,12)==0)
    {
      asm pop ds
      return(temp->parameter);
    }
    temp=temp->next;
  }
  asm pop ds;
  return(0L);
}

void far cdestroy(void)
{
  module far *temp,far *code_check,far *next,far *prior;
  unsigned char far *code;
  hook_ptr far *hook=hooks;
  char code_used=false;
  asm push ds
  asm mov ax,DATA
  asm mov ds,ax
  /*stage 1 unlink all children from task queue, and free stack/
         thread control block, and possibly the code */
  SET_MANIP;
  temp=current_module->child;
  current_module->child=0;  /*NO children */
  if (temp)
  {
    while (temp!=current_module)
    {
      if (temp->child)
      {
        next=temp->child;
        temp->child=0L;
      }
      else
      {
        prior=temp->prior;

        prior->next=temp->next;  /*The module is now unlinked*/
        temp->next->prior=prior;
        code=temp->code;
        code_check=first_module;
        do
        {
          if (code==code_check->code)
          {
            code_used=true;
            break;
          }
          code_check=code_check->next;
        }while (code_check!=first_module);

        if (!code_used)
        {
          Free_mem(temp->code);
        }
        Free_mem(temp->stack);
        if (temp->elder_sib)
          next=temp->elder_sib;
        else
          next=temp->parent;
        if (temp->parent)
          temp->parent->Acumulated_time+=temp->Acumulated_time;
        while (hook)
        {
          hook->destroyproc(temp);
          hook=hook->next;
        }
        Free_mem(temp);
      }
      temp=next;
    }
  }
  CLEAR_MANIP;
  asm pop ds;
}


void far cperish(void)
{
  module far *temp,far *code_check,far *next;
  unsigned char far *code;
  hook_ptr far *hook=hooks;
  char code_used=false;
  asm push ds
  asm mov ax,DATA
  asm mov ds,ax
  /*stage 1 unlink all children from task queue, and free stack/
         thread control block, and possibly the code */
  if (current_module->max_size>current_module->stack_size)
    swap_module(1L);
  cdestroy();
  SET_MANIP;
    /*stage 2 unlink current block from task queue and free stack */
  if ((temp=current_module->parent)!=0)
  {
    if (temp->child!=current_module)
    {
      temp=temp->child;
      while (temp->elder_sib)
      {
        if (temp->elder_sib==current_module)
        {
          temp->elder_sib=current_module->elder_sib;
          break;
        }
        temp=temp->elder_sib;
      }
    }
    else
      temp->child=current_module->elder_sib;
  }
  temp=current_module->prior;

  temp->next=current_module->next;  /*The module is now unlinked*/
  current_module->next->prior=temp;
  code=current_module->code;
  code_check=first_module;
  do
  {
    if (code==code_check->code)
    {
      code_used=true;
      break;
    }
    code_check=code_check->next;
  }while (code_check!=first_module);
  if (!code_used)
  {
    Free_mem(current_module->code);
  }
  Free_mem(current_module->stack);
  if (current_module->parent)
          current_module->parent->Acumulated_time+=
                current_module->Acumulated_time;
  while (hook)
  {
    hook->destroyproc(current_module);
    hook=hook->next;
  }
  Free_mem(current_module);
  current_module=temp;
  CLEAR_MANIP;
  asm pop ds;
}


void far duplicate_module(char copies,char relation)
{
/*relation- 0=child  - if caller has a child, then put it in the
                        list of his children, otherwise make it
                        his child.
            1=Brother- put it in the list of callers generation
            2=Independent- No children, not a child, and no siblings
            3=Parent-  Make new one the Parent of the one who called it.*/
  module far *temp, far *newchild;
  module far *orig;
  module far *relmark;
  hook_ptr far *hook;
  short far *stack;
  short far *origstack;
  short cnt;
  short abs_stack_ofs;
  relation&=0xf;                   /*get only the lowest nibble becuase the
                                     rest is used elsewhere */
  asm push ds
  asm mov ax,DATA
  asm mov ds,ax
if (current_module->options&1)
  {
    asm pop ds;
    return;
  }
  SET_MANIP;
  temp=current_module->next;       /*save next*/
  orig=current_module;
  origstack=current_module->stack;
  abs_stack_ofs=orig->stackofs-(short)orig->stack;
  while (copies)
  {

    while( (stack=Allocate_mem(orig->stack_size) )==0)
    {

/* asm int 3 */

      swap_module(0L);
    }

    while((newchild=(module far *)Allocate_mem(sizeof(module)))==0)
    {

/* asm int 3 */

      swap_module(0L);
    }
    hook=hooks;
    while (hook)
    {
      hook->forkproc(current_module,newchild);
      hook=hook->next;
    }
    current_module->next=newchild;

    current_module->next->prior=current_module;
    current_module=current_module->next;
    strncpy(current_module->name,orig->name,8);
    current_module->Acumulated_time=0;
    current_module->status    =orig->status;
    current_module->stack_size=orig->stack_size;
    current_module->code      =orig->code;
    current_module->Priority  =orig->Priority;
    current_module->options   =orig->options;
    current_module->environ   =orig->environ;
    current_module->load_path =orig->load_path;
    current_module->max_size=orig->stack_size-orig->stackofs;
    current_module->device_name=orig->device_name;

    current_module->stack     =stack;
    current_module->stackseg  =(short)((long)stack>>16);
    current_module->stackofs  =(short)stack+          /*add the offset
                                                        equivalant to old
                                                        SP*/
                             abs_stack_ofs;
    /*duplicate the stack*/
    movmem(origstack+(abs_stack_ofs>>1),
           stack+(abs_stack_ofs>>1),
           orig->stack_size-abs_stack_ofs);
    if (relation==0)
    {
      if (orig->child)
      {
        current_module->elder_sib=orig->child;     /*Elder is person that
                                                 was youngest*/
        orig->child=current_module;            /*move to youngest */
        current_module->child=0L;              /*I have no Children*/
        current_module->parent=orig;
      }
      else
      {
        orig->child=current_module;
        current_module->elder_sib=0L;
        current_module->child=0L;
        current_module->parent=orig;
      }
    }
    else
    if (relation==1)
    {
      relmark=orig->parent;
      if (relmark)
      {
        current_module->elder_sib=relmark->child;
        relmark->child=current_module;
        current_module->parent=relmark;
        current_module->child=0L;
      }
      else
        goto independant;
    }
    else
    if (relation==2)
    {
independant:
      current_module->child=0L;
      current_module->parent=0L;
      current_module->elder_sib=0L;
    }
    else
    if (relation==3)
    {
      /*If this is a child of another thread, then his parents child is the
        new parent*/
      if (orig->parent)
        orig->parent->child=current_module;
      /*The New parents parent is the parent of the original child */
      current_module->parent=orig->parent;
      /*The new parent is now a parent */
      orig->parent=current_module;
      /*The parent has 1 child */
      current_module->child=orig;
      /*The parent has not brothers */
      current_module->elder_sib=0L;
    }

    copies--;
  }
  current_module->next=temp;    /*keep the chain intact*/
  temp->prior=current_module;
  current_module=orig;
  CLEAR_MANIP;
  asm pop ds;
}

char far *Load_Com(char far *filename,module far *Task_block)
{

  short input;
  unsigned short length;
  input=open(filename,0);
  if (input==-1)
  {
    return(0L);
  }
  length=sizefind(input)+0x100;
  Task_block->code=Allocate_mem(length);
  read(Task_block->code+0x100,length,input);
  close(input);
  return(Task_block->code+0x100);
}

char far *Load_Dos_Exec(char far *filename,module far *Task_block)
{
  /*
    Exec Header
    0  Exec Signature 4d5a
    2  Length mod 512 including header
    4  Length div 512 including header
    6  Number of relocate Items
    8  paragraphs of header
    10 Min number needed above program
    12 max number needed above program
    14 Stack Displacemnet (segment)
    16 stack pointer (offset)
    18 Checksum
    20 IP
    22 Code Displacement (segment)
    24 Offset of first relaction item
    26 Overlay Number
    28- Relocation Table
        Program and data segments
        Stack Segments
  */
  struct
  {
  short head_check;
  short Length_Remainder;
  short Length_pages;
  short num_relocate;
  short header_size;  /*in Paragraphs*/
  short Min_Excess;
  short Max_Excess;
  short Stack_Seg_Add;
  short Stack_Pointer;
  short checksum;
  short Code_offset;
  short Code_Seg_Add;
  short first_item;
  }header_info;
  long  total_code;
  short Fixup;
  short Program_seg;
  short far *Code;
  long far *Relocate_table;
  short input;
  input=open(filename,0);
  if (input==-1)
  {
    print("No such File!\n");
    print(filename);
    print("\r\n");
    Exit_routine(errorcode);
  }
  read(&header_info,26,input);
  if (header_info.head_check!=0x5a4d)
  {
    print("File is not a DOS .EXE");
    Exit_routine(errorcode);
  }
  if (header_info.Length_Remainder)
    header_info.Length_pages--;
  if (header_info.Stack_Pointer||header_info.Stack_Seg_Add)
  {
    Task_block->stack=(short far *)(
                        (long)(header_info.Stack_Seg_Add+Program_seg)<<16|
                         header_info.Stack_Pointer);
/*    print("Unable to load this EXE.  There is a stack already predefined.\r\n");
    print("Sorry...");
    Exit_routine(1);*/
  }
  total_code=(long)lmult(header_info.Length_pages,512)+
                  header_info.Length_Remainder-
                  lmult(header_info.header_size,16)+
                  lmult(header_info.Min_Excess,16);
  seek(input,header_info.first_item,0);
  if (total_code>65535L)
  {
    print("Error Allocateing Space for :");
    print(filename);
    print("\r\n");
    Exit_routine(errorcode);
  }
        /*allocate space for the code*/
  Code=(short far *)Task_block->code=Allocate_mem(total_code);
  if (!Code)
  {
    print("Error Allocating space for code of Exe:");
    print(filename);
    print("\r\n");
    Exit_routine(errorcode);
  }
  Program_seg=(short)((long)Code>>16);
  Relocate_table=Allocate_mem(4*header_info.num_relocate);
  if (!Relocate_table)
  {
    print("Error Allocating space for relocate table of Exe:");
    print(filename);
    print("\r\n");
    Exit_routine(errorcode);
  }
  if ((header_info.num_relocate*4)!=
         read(Relocate_table,
              4*header_info.num_relocate,
              input))  /*read relocate info*/
  {
    print("Error reading Relocate table.\r\n");
    Exit_routine(errorcode);
  }
  seek(input,header_info.header_size*16,0);
  if (!read(Code,(short)total_code,input))          /*read code info*/
  {
    print("Error reading Code.\r\n");
    Exit_routine(errorcode);
  }
  {
    short temp=header_info.num_relocate;
    asm push ds;
    asm mov cx,temp
    asm les di,Relocate_table

continue_fix:
    asm lds bx,Code
    asm add bx,es:[di]
    asm mov dx,ds
    asm add dx,es:[di+2]
    asm mov ds,dx
    asm mov ax,Program_seg
    asm add ds:[bx],ax
    asm add di,4
    asm loop continue_fix
    asm pop ds;
  }
/*  for (Fixup=0;Fixup<header_info.num_relocate;Fixup++)
  {
    *(short far *)((char far *)Code+Relocate_table[Fixup])+=Program_seg;
  }*/
  Free_mem(Relocate_table);
  close(input);
  header_info.Code_Seg_Add+=Program_seg;  /*get new relative cs*/
  return((void far *)(((long)header_info.Code_Seg_Add<<16)|
                             header_info.Code_offset));
}



module far *(far load_module)(char far *name,short stack_len,
                        char far *device_name,char priority)
{
  /*This routines job is to load the names module, with the stack of
    appropriate length, and also give it the corrent device number*/
  short tseg,tofs,codeseg;
  char far *pos;
  char far *program_begin;
  short far *stack;
  module far *temp;
  module far *orig,far *work_module;
  char far *filename;
  asm push  ds
  asm mov ax,DATA;
  asm mov ds,ax;
  SET_MANIP;
  orig=current_module;
  if (current_module==first_module)
    current_module=current_module->next;
  while (current_module->Priority<=priority&&
         current_module!=first_module)  /*go up to the next_priority*/
    current_module=current_module->next;
  if (current_module==first_module)
    current_module=current_module->prior;
  while (current_module->Priority>priority&&
         current_module!=first_module)   /*go back one or more*/
    current_module=current_module->prior;
  temp=current_module->next;
  current_module->next=(module far *)Allocate_mem(sizeof(module));
  if (current_module->next==0L)
  {
    print("No more Memory for more modules.\r\n");
    CLEAR_MANIP;
    Exit_routine(1);
  }
  current_module->next->prior=current_module;
  current_module=current_module->next;
  work_module=current_module;
  strncpy(current_module->name,name,8);
  current_module->Acumulated_time=0;
  current_module->next=temp;
  current_module->options=0;
  current_module->stack=0L;
  current_module->load_path=path;
  current_module->environ=OS_module.environ;
  temp->prior=current_module;
  filename    =  (char far*)Allocate_mem(strlen(path)+12);
  strcpy(filename,path);
  strcat(filename,name);
  if (final_dot(name)==0)
  {
    strcat(filename,".COM");
    program_begin=Load_Com(filename,current_module);
    if (!program_begin)
    {
      strcpy(filename,path);
      strcat(filename,name);
      strcat(filename,".EXE");
      program_begin=Load_Dos_Exec(filename,current_module);
    }
  }
  else
  {
    program_begin=Load_Com(filename,current_module);
    if (!program_begin)
      program_begin=Load_Dos_Exec(filename,current_module);
  }
  if (!program_begin)
  {
    print("Unable to load: ");
    print(name);
    print(" on path ");
    print(path);
    print("\r\n");
    CLEAR_MANIP;
    Exit_routine(1L);
  }
  if (!current_module->stack)
  {
    current_module->stack_size=stack_len;
    current_module->stack=stack=Allocate_mem(stack_len);
    stack+=(stack_len>>1)-1;
    current_module->options&=~1;
  }
  else
  {
    current_module->options|=1;
    stack=current_module->stack;
  }
  device_name=add_device_node(device_name);
  current_module->device_name=device_name;


  current_module->status=0;
  /*set up the stack for the first 'return'*/
  codeseg=(short)((long)current_module->code>>16);
  tseg=(short)((long)stack>>16);
  tofs=(short)stack;
    /*Stack bottom- Segment of routine*/
  stack[0]=(short)((long)program_begin>>16);
    /*Next word down- Offset of routine*/
  stack[-1]=(short)program_begin;
    /*Next word down- BP*/
  stack[-2]=0;
    /*Next word down- DS*/
  stack[-3]=codeseg;
    /*Next word down- SI*/
  stack[-4]=0;
    /*Next word down- DI*/
  stack[-5]=0;
    /*next word down- AX*/
  stack[-6]=0;
  current_module->max_size=0;
  current_module->stackseg=tseg;
  current_module->stackofs=tofs-(2*6);
  current_module->parent=0L;
  current_module->child=0L;
  current_module->elder_sib=0L;
  current_module->Priority=priority;
  current_module=orig;
  Free_mem(filename);
  CLEAR_MANIP;
  asm pop ds;
  return(work_module);
}

void far load_modules(char far *load_name)
{
  short handle,stack_size;
  short threads;
  unsigned char priority;
  char marker,endmark,param;
  char far *line,far *filename,far *tpointer;
  module far *last_loaded=&OS_module;
  char devicename[12];
  char number[10],params_valid;
  asm push ds
  asm mov ax,DATA
  asm mov ds,ax
  line        =  (char far*)Allocate_mem(80);
  filename    =  (char far*)Allocate_mem(strlen(path)+12);
  strcpy(filename,path);
  strcat(filename,load_name);
  handle=open(filename,0);
  if (handle==-1)
  {
    asm int 3;
    print("Error opening configuration File.  Please report to the\r\n");
    print("system manager.\a\a\r\n");
    print(" File: ");
    print(filename);
    Exit_routine(errorcode);
  }
  Free_mem(filename); /*we no longer need its own allocated space, becuase
                       it just points to line, and we build it again
                       in load_module()*/
  priority=0;
  while (gets(line,80,handle))
  {
    tpointer=strchr(line,config_comment_char);
    if (tpointer)
      tpointer[0]=0;
    if (strchr(line,'='))
    {
      process_entry(line,&(last_loaded->environ));
      continue;
    }
    marker=0;
    while (line[marker]==' '||line[marker]=='\t') marker++;
    endmark=marker;
    param=0;
    stack_size=MINSTACK;
    devicename[0]=0;
    threads=1;
    if (priority<255) priority++;
    params_valid=false;
    while (line[marker])
    {
      /*search for the first tab/space/,/\n/NULL after the name
        which deliminates the name*/
      while ((line[endmark]!='\t')&&
             (line[endmark]!=' ') &&
             (line[endmark]!=10)  &&
             (line[endmark]!=',') &&
             (line[endmark]!='\0'))
        endmark++;
      if (endmark!=marker)
      {
        params_valid=true;
        switch(param)
        {
          case 0:/*this is the name of the module to load */
                 line[endmark]=0;
                 filename=line+marker;
                 endmark++;
                 break;
          case 1:/*This is the stack length- if NULL default=512*/
                 strncpy(number,line+marker,endmark-marker);
                 stack_size=atoi(number);
                 if (stack_size==0)
                   stack_size=MINSTACK;
                 break;
          case 2:/*This is the module's devicename*/
                 strncpy(devicename,line+marker,
                     ((endmark-marker)>12)?12:(endmark-marker));
                 if (endmark-marker<12)
                   devicename[endmark-marker]=0;
                 break;
          case 3:/*This is the number of threads*/
                 strncpy(number,line+marker,endmark-marker);
                 number[endmark-marker]=0;
                 threads=atoi(number);
                 if (threads==0)
                 {
                   print("Error in Thread def in line(0 not allowed):\r\n");
                   print(line);
                   Exit_routine(1);
                 }
                 break;
          case 4:/* This is the priority Number */
                 strncpy(number,line+marker,endmark-marker);
                 number[endmark-marker]=0;
                 priority=atoi(number);
                 if (priority==0)
                 {
                   print("Error in Priority def in line(0 not allowed:\r\n");
                   print(line);
                   Exit_routine(1);
                 }
                 break;
        }
      }
      do
      {
        param++;
        /*if the prior delimiter was a ',' then we need to move to the
        next character regardless so that we don't pick up the same comma.
        Then we go through and while there are spaces, no commas, and
        no end of lines, we go till we find the first text. If there is
        nothing here but another ',' then we do the search again,
        incrementing the parameter counter*/
        if (line[endmark]==',') endmark++;
        while ((line[endmark]==' ')&&
               (line[endmark]!=',')&&
               (line[endmark]!=10) &&
               (line[endmark]!='\0')) endmark++;
      }while (line[endmark]==',');
      if ((line[endmark]==10)||
          (line[endmark]==0)) break;
      marker=endmark;
    }
    if (params_valid)
      if (!(last_loaded=
              load_module(filename,stack_size,devicename,priority)))
      {
        char number[10];
        print("Error loading File ");
        print(filename);
        print(" with error:");
        itoa(errorcode,number);
        print(number);
        Exit_routine(0);
      }
    /*Actually load module, and set up stack, and set up module number*/
  }
  close(handle);
  Free_mem(line);
  asm pop ds;
}

void make_lists()
{
  /*This routine builds the basic list for all routines that will
    be availiable from the base program.  This list will be added to by
    register_routine()*/

  first_routine=(routine_ptr far *)Allocate_mem(sizeof(routine_ptr));
  first_routine->procedure=(proc)void_routine;
  strcpy(first_routine->name,"VOID");
  first_routine->next=0L;


  Register("Shell",(proc)Shell,0);
  Register("atol",(proc)near2far(atol),0);
  Register("atoi",(proc)near2far(atoi),0);
  Register("open",(proc)near2far(open),0);
  Register("create",(proc)near2far(create),0);
  Register("close",(proc)near2far(close),0);
  Register("read",(proc)near2far(read),0);
  Register("write",(proc)near2far(write),0);
  Register("strnicmp",(proc)near2far(strnicmp),0);
  Register("stricmp",(proc)near2far(stricmp),0);
//  Register("strcmp",(proc)near2far(strcmp),0);
  Register("itoa",(proc)near2far(itoa),0);
  Register("strlen",(proc)near2far(strlen),0);
  Register("movmem",(proc)near2far(movmem),0);
  Register("strncpy",(proc)near2far(strncpy),0);
  Register("strcat",(proc)near2far(strcat),0);
  Register("memset",(proc)near2far(memset),0);
  routines[0]=(long)near2far(Request);
  routines[1]=(long)near2far(Exit_routine);
  routines[2]=(long)near2far(swap_module);
  routines[3]=(long)near2far(Terminate);
  routines[4]=(long)near2far(Export);
  routines[5]=(long)near2far(Import);
  routines[6]=(long)0;    /*room for the name*/
  routines[7]=(long)near2far(wake_task);

  routines[8]=(long)near2far(load_modules);
  routines[9]=(long)near2far(load_module);
  routines[10]=(long)near2far(get_environ);
  routines[11]=(long)near2far(Allocate_mem);
  routines[12]=(long)near2far(Free_mem);
  routines[13]=(long)near2far(Inquire_begin);
  routines[14]=(long)near2far(&process_control);
  routines[15]=(long)near2far(Connect_Int);
  routines[16]=(long)near2far(Disconnect_Int);
  routines[17]=(long)near2far(gettime);
  routines[18]=(long)near2far(change_priority);
  routines[19]=(long)near2far(&current_module);
  routines[20]=(long)near2far(swap_to);
  routines[21]=(long)near2far(delay);

}

void fast_clock(void)
{
  ticks=*(long far *)0x0000046cL;
  Connect_Int(time_interupt,0x8);
  if (timed)
  {
    asm cli
    asm mov al,0x36
    asm out 0x43,al
    asm mov al,0xa9;
    asm out 0x40,al
    asm mov al,0x04;
    asm out 0x40,al;
    asm sti
    ticks=lmult(ticks,55);
  }
}

void slow_clock(void)
{
  if (timed)
  {
    asm cli
    asm mov al,0x36
    asm out 0x43,al
    asm mov al,0;
    asm out 0x40,al
    asm out 0x40,al;
    asm sti
  }
  Disconnect_Int(time_interupt,0x8);
}

void process_environ(void)
{
  char far *param;
  char number[10];
  param=get_environ("timed");
  if (param)
  {
    if (param[0]=='t'||param[0]=='T')
      timed=true;
  }
  param=get_environ("scan_force");
  if (param)
  {
    force_interval=atoi(param);
  }
  print("Clock Res.:");
  if (timed)
    print("High  ");
  else
    print("Low   ");
  print("Interval: ");
  itoa(force_interval,number);
  print(number);
}

void main(char far *params,char far *inpath)
{
  char i;
  char far *EOPath;
  char name_char;
  connect_stall();
  print("Intelligent Peripheral Controller\r\n");
  print("Copyright (c) 1992,1993 Logical Data Corporation\r\n");
  print("All Rights Reserved.\r\n");
  EOPath=final_dot(inpath);     /*get where the dot extension is*/
  EOPath[0]=0;                  /*get rid of prior extention*/
  while (EOPath[0]!='\\'&&       /*back up to the first slash or begin of
                                  name*/
         EOPath>inpath&&
         EOPath[0]!=':')
    EOPath--;
  EOPath++;
  strcpy(load_name,EOPath);     /*move the loadname.cfg to default name*/
  strcat(load_name,".cfg");        /*append .cfg to load name*/
  name_char=*EOPath;
  *EOPath=0;                  /*remove filename from inpath*/
  path=Allocate_mem(strlen(inpath));
  strcpy(path,inpath);
  *EOPath=name_char;
  print(path);
  print("\r\n");
  params[0]++;
  for (i=1;i<=params[0];i++)
    if (params[i]==13) params[i]=0;
  print(params+1);
  print("\r\n");
  for (i=1;i<params[0];i++)
  {

    if (params[i]=='/')
    {
      switch(params[i+1])
      {
        case 'C':
        case 'c':
        case 'L':
        case 'l':i+=strncpy(load_name,params+i+2,15);
                 break;
        default: print("Invalid Parameters.  Usage:");
                 print(inpath);
                 print(" /Cconfig.fil\r\n");
                 Exit_routine(1);
      }
    }
  }

  make_lists();
  Connect_Int((void far *)near2far(Exec_Services),0x60);
  load_modules(load_name);
  process_environ();
  fast_clock();
  swap_module(1L);
}
