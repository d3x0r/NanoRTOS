#include <stdio.h>
#include <stdlib.h>
#define false 0
#define true (!false)
#define Allocate malloc
#define Free free
typedef Xltr(struct pipe far *pipe);

typedef struct Device
{
  short Device_type;
  void far *device;
}Device;

typedef struct pipe
{
  Device far *InDevice;
  Device far *OutDevice;
  Xltr far *Trans;
  short Trans_seq;
  struct pipe far *next,far *prior;

} pipe;

pipe far *first_pipe=NULL;

char Devgetc(Device far *device)
{
  char ch;
  switch(device->Device_type)
  {
    case 0:
    case 1:
      ch=fgetc(device->device);
      break;
  }
  return(ch);
}

void Devputc(char ch,Device far *device)
{
  switch(device->Device_type)
  {
    case 0:
    case 1:
      fputc(ch,device->device);
      break;
  }
}

Device far *Devopen(short type)
{
  Device far *temp;
  temp=Allocate(sizeof(Device));
  temp->Device_type=type;
  switch(type)
  {
    case 0:
      temp->device=stdin;
      break;
    case 1:
      temp->device=stdout;
      break;
  }
  return(temp);
}

Xltr Glass_Trans
{
  char ch;
  switch(pipe->Trans_seq)
  {
    case 0:
      ch=Devgetc(pipe->InDevice);
      Devputc(ch,pipe->OutDevice);
      break;
  }
}

pipe far *make_pipe(Xltr far *Trans,Device far *In,Device far *Out)
{
  pipe far *current=first_pipe,far *prior=NULL;
  if (!first_pipe)
    first_pipe=current=Allocate(sizeof(pipe));
  else
  {
    while (current->next)
      current=current->next;
    current->next=Allocate(sizeof(pipe));
    prior=current;
    current=current->next;
  }
  current->next=NULL;
  current->prior=prior;
  current->Trans_seq=0;
  current->Trans=Trans;
  current->InDevice=In;
  current->OutDevice=Out;
}



main()
{
  pipe far *current;
  Device far *In,far *Out;

  In=Devopen(0);
  Out=Devopen(1);


  current=make_pipe(Glass_Trans,In,Out);

  while (true)
  {
    current=first_pipe;
    while (current)
    {
      current->Trans(current);
      current=current->next;
    }
  }
}




