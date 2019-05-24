#include "mod.h"
#include "video.h"
#define NULL 0L
#define false 0
#define true !false

char shuttingdown=false;
short alloc=0,dalloc=0;


typedef struct data_holder
{
  char far *data;
  short length;
  struct data_holder far *next;
}data_holder;

typedef struct connection
{
  data_holder far *data;
} connection;

connection far *first_connection=0L;

windowptr output;


public(connection far *,openether,(void /*char far *source_address,
                               char far *dest_address,
                               short t_source,short t_dest,char opts*/))
{
  /*this routine opens an IP connection and returns the pointer to the
    Ether control block
    Options- 1=record socket*/
  connection far *temp;
  temp=Allocate(sizeof(connection));
  temp->data=NULL;
  return(temp);
}


public (void,etherflush,(connection far *ether))
{
  data_holder far *temp,far *next=NULL;
  temp=ether->data;
  while (temp)
  {
    next=temp->next;
    Free(temp);
    temp=next;
  }
}

public (void,close_connection,(connection far *ether))
{
  data_holder far *temp,far *next;
  short cnt,loop;

  Load_DS;

  temp=ether->data;
  while (temp)
  {
    next=temp->next;
    Free(temp);
    temp=next;
  }
  Free(ether);

  Restore_DS;
}


public(void,Ether_term,
       (connection far *ether,short read))
{
}


public(short,readether,
     (connection far *ether,char far *buffer,short maxlength))
{
  short idx;
  short cnt;
  data_holder far *temp;
  char far *data;

  Load_DS;

  for (cnt=0;cnt<10;cnt++)
    Relinquish(0L);
  while  (!ether->data)
    Relinquish(0L);
  temp=ether->data;
  data=temp->data;
  for (idx=0;idx<maxlength&&idx<temp->length;idx++)
  {
    buffer[idx]=data[idx];
  }
  ether->data=ether->data->next;
  displayln(output,"r");
  Free(data);
  Free(temp);

  Restore_DS;

  return(idx);
}

public(short,sendether,
       (connection far *ether,char far *buffer,unsigned short length))
{
  data_holder far *temp;
  short cnt;
  char far *data;
  short idx;

  Load_DS;

  for (cnt=0;cnt<10;cnt++)
    Relinquish(0L);
  temp=ether->data;
  if (!temp)
    temp=ether->data=Allocate(sizeof(data_holder));
  else
  {
    while (temp->next)  temp=temp->next;
    temp->next=Allocate(sizeof(data_holder));
    temp=temp->next;
  }
  temp->next=NULL;
  displayln(output,"w");
  data=temp->data=Allocate(length);
  temp->length=length;
  for (idx=0;idx<length;idx++)
    data[idx]=buffer[idx];

  Restore_DS;

  return(length);
}

void init()
{
  _openether();
  _readether();
  _sendether();
  _Ether_term();
  _etherflush();
  output=opendisplay(28,21,30,5,NO_CURSOR|BORDER,0x1f,0x1a,0,"Ether_Null");
}

void main(void)
{
  short i;
  init();
  Relinquish(1);
}




