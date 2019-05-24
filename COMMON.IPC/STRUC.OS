Definitions of OS structures.

Name: Module
Use : Controls task linkage, and retains a pointer to a programs code,
      and stack.
ID #: 0
Explanation:    Name is the eight character name that the program was
                  loaded from disk with.
                size is how big the program's code segment is
                status indicates whether a routine is asleep or not.
                     If most significant bit is on, then then the status
                     indicates a negative count of the number of cycles
                     that the task will relinquish until woken.
                stack contains a pointer to the beginning (end) of the task's
                     stack;
                stackseg contains the value of the program stack segment  at
                     the time it last relinquished.
                stackofs contains the value of the program stack pointer at
                     the time it last relinquished.
                code contains a pointer to the tasks code segment.
                child contains a pointer to the tasks children processes
                     if any.
                elder_sib contains a pointer to the tasks priorly forked brother
                     if any.
                parent contains a pointer to the childs parent.
                next contains a pointer to the next task to run, and is
                     independant of relation
                prior contains a pointer to the task that has just run, and is
                     independant of relation

 typedef struct module
 {
   char name[8];
   unsigned short size;
   unsigned short stack_size;
   unsigned long status;
   short far *stack;
   unsigned short stackseg,stackofs;
   unsigned char far *code;
   struct module far *child;
   struct module far *elder_sib;
   struct module far *parent;
   struct module far *next;
   struct module far *prior;
 } module;

Name: environ_entry
Use : contains the environment parameters from the configuration file.
ID #: 1
Explation:  name is the text environment variable name to look for.
            parameter is the text that was associated with the given variable.
            next is a pointer to the next environment varialbe.

typedef struct environ_entry
{
  char name[12];
  char parameter[30];
  struct environ_entry far *next;
}environ_entry;

Name: node_entry
Use : holds messages that are being passed from one task to another via
          Import and Export.
ID #: 2
Explanation:  status indicates whether the node has already been requested.
              data is a pointer to the message that is being passed.
              souce contians a pointer to the sender. NOTE: I do not
                 know if this works.
              next is a pointer to the next node in the queue;
              last is a pointer to the last node in the queue.  It is
                   maintained so that adding to the tail of the queue
                   is simplified.

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

Name: device_tree
Use : holds a tree of the devices that have been loaded. It is used to hang
        nodes on.
ID #: 3
Explanation: less is a pointer to the next device who's name is alphabetically
                less than the current one.
             more is a pointer ot the next device who's name is alphabetically
                more than the current one.
             dest is the name of the device.
             node is a pointer to a list of nodes that are destine for this
                device.

typedef struct device_tree
{
  struct device_tree far *less,far *more;
  char dest[12];
  node_entry far *node;
} device_tree;

Name: BLOCK
Use : manages memory blocks.
ID #: 4
NOTE:  SNO = Segment No Offset   (indicates a zero justified seg address)
Explanation:  priorsegment is the SNO of the block prior the this one.  This
                  is related to the physical order, and includes all blocks,
                  free and allocated.
              blocksize is the number of paragraphs that have been alloted
                  for this memory block.  It does not inclue the memory manage-
                  ment block.  So, add 1 to account for the management block.
              owner is the SNO of the Call to Allocate that allocated
                  this block, or 0 if the block is free.
              nextfree is only aplicable to free blocks, and it indicates the
                  next free block to check.  It only affects free blocks.
              priorfree is only aplicable to free blocks, and it indicates the
                  prior free blcok to check.  It only affects free blocks.
              freed_by is the SNO of the Call to Free that deallocated this
                  block.

BLOCK STRUC
  priorsegment  dw ?
  blocksize     dw ?
  owner         dw ?
  nextfree      dw ?
  priorfree     dw ?
  freed_by      dw ?
BLOCK ends

Name: routine_ptr
Use : keeps tract of dynamic routines
ID #: 5
Explanation: name is the name of the routine that this is pointing to.
             procedure is a pointer to the beginning of the routine that
                 has been registerd, and is being kept track of by this block.
             next is a pointer to the next routine to check.

typedef struct routine_ptr
{
  char name[ROUTINEMAX];
  proc procedure;
  struct routine_ptr far *next;
}routine_ptr;

