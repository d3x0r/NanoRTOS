#define extern_def
#include <mod.h>
#define ETHER
#include "ether.h"
#define NULL 0L;
#define false 0
#define true (!false)

/* This file contains all the routines that are to be registered.  This
   provides a flexability, and seclusion of outside routines from those
   routines that are done within the ether task.  */

extern connection far *first_con;
extern char shuttingdown;
extern char broad_ether[6];
extern short receive_size,send_size;
extern short max_retrans_delay;


#define alloc_last(start,current,size) {  \
if (start)                                \
  {                                       \
    current=start;                        \
    while (current->next) current=current->next;   \
    current->next=Allocate(size);            \
    current=current->next;                      \
  }                                       \
  else                                    \
  {                                       \
    current=Allocate(size);                  \
    start=current;                           \
  }                                       \
}


public(connection far *,openether,(char far *source_address,
                               char far *dest_address,
                               short t_source,short t_dest,char opts,
                               short rcv_size,short xmt_size,
                               short num_blocks))
{
  connection far *temp=NULL;
  Load_DS;
  if (temp=first_con)
  {
    if (!cmpIP(dest_address,"\0\0\0\0"))
      while (temp)
      {
        /* we have connections and the new one has a specific destination,
           look for a match */
        if (cmpIP(temp->I.source,source_address)&&
            cmpIP(temp->I.dest,dest_address)&&
            temp->T.source==t_source&&
            temp->T.dest==t_dest)
        {
          while (temp->state) Relinquish(0L);
          Restore_DS;

          return((connection far*)temp);
        }
        temp=temp->next;
      }
    temp=Allocate(sizeof(connection));
    temp->next=first_con;
    first_con->prior=temp;
    first_con=temp;
  }
  else
  {
    temp=first_con=Allocate(sizeof(connection));
    temp->next=NULL;
  }
  temp->prior=NULL;
  /* fill in socket info. */
  temp->Dwindow=0;

  temp->status=0;

  if (cmpIP(dest_address,"\0\0\0\0")&&
      t_dest==0)
  {
    temp->status|=SERVER_SOCKET;
  }
/*The following are constants that shouldn't change during normal usage*/
  moveEther(temp->Edest,broad_ether);
  temp->insize=rcv_size;
  temp->inhead=0;
  temp->intail=0;
  temp->inbuf=Allocate(temp->insize);
  temp->outsize=xmt_size;
  temp->outhead=0;
  temp->outtail=0;
  temp->outbound=0;
  temp->outmaxbnd=0;
  temp->outbuf=Allocate(temp->outsize);
  temp->last_block=num_blocks;
  temp->blocks=Allocate(temp->last_block*sizeof(receive_block));
  temp->last_block=num_blocks;
  temp->blocks[0].begin_sequence=0;
  temp->blocks[0].length        =0;
  temp->blocks[0].buf_begin     =NULL;
  {
    short idx;
    for (idx=0;idx<num_blocks;idx++)
      temp->blocks[idx].buf_begin=NULL;
  }
  temp->Dwindow=100;
  temp->MaxDwindow=100;
reinit_socket:

  temp->I.version_len=0x45;
  temp->I.service=0;
  temp->I.ident=0;
  temp->I.frags=0;
  temp->I.time_to_live=64;
  temp->I.protocol=6;
  temp->PI.prot=6;
  temp->PI.zero=0;
  temp->PI.zeros=0L;

  temp->T.hlen=0x50;
  temp->T.urgent=0;
  temp->T.window=intswap(temp->insize);

  temp->T.dest=intswap(t_dest);
  temp->T.source=intswap(t_source);
  moveIP(temp->I.dest,dest_address);
  moveIP(temp->I.source,source_address);
  moveIP(temp->PI.dest,dest_address);
  moveIP(temp->PI.source,source_address);

  gettime((time_struc far *)&temp->T.ack);       /*produce pseudo       */
  temp->T.seq=(temp->T.ack&=0xffff)&0xfff;       /*random staring places*/
  temp->tail_seq=longswap(temp->T.seq);

  temp->current_ident=0;       /*current ident*/
  temp->state=SRESET|SWSYN|SNARP|SNSYN;  /*not connected state*/
  gettime(&temp->connect_timer);
  temp->flags=0;

  temp->transmits=0;               /*  misc. timers/counters and  */
  temp->retransmits=0;             /*  statistics information     */

  temp->smth_dwind=5000;           /*                             */
  temp->dwind_var=500;             /*                             */

  temp->rto  = max_retrans_delay;
  temp->srtt = max_retrans_delay>>2;
  temp->sdev = max_retrans_delay>>3;

  temp->p_received=0;
  temp->rec_duplicats=0;
  temp->over_flows=0;
  temp->users=1; /*added 6/11/93- used for closing connections we are opening it
                   so we must be using it*/

  while (temp->state)
  {
    wake_output();
    Relinquish(0L);
    if (temp->state&SCLOSED)
    {
      goto reinit_socket;
    }
  }
exit_open:
  wake_output();
  temp->users=0;
  Restore_DS;
  return(temp);
}


public (void,flushether,(connection far *ether))
{
  Load_DS;
  Restore_DS;
}


public (short,closeether,(connection far *ether))  /*return changed 6/11/93*/
{
  Load_DS;
  Restore_DS;
  return(false);
}

public(unsigned short,readether,
     (char far *buffer,unsigned short maxlength,connection far *ether))
{
  short length=0,dolen,head,size,avail;
  if (ether->state||!ether)
    return(-1);
  avail=ether->blocks[0].length;
  if (avail<maxlength)
    maxlength=avail;
  head=ether->inhead;
  size=ether->insize;
  for (length=0;length<maxlength;length++)
  {
    buffer[length]=ether->inbuf[head++];
    if (head==size)
      head=0;
  }
  ether->inhead=head;
  ether->blocks[0].begin_sequence+=length;
  ether->blocks[0].length-=length;
  ether->T.window=intswap(ether->insize-ether->blocks[0].length);
  return(length);
}

public(unsigned short,sendether,
       (char far *buffer,unsigned short maxlength,connection far *ether))
{
  short head=ether->outhead,size=ether->outsize,avail,length;

  Load_DS;
  if (ether->state||!ether)
    return(-1);
  avail=ether->outtail-ether->outhead;
  if (avail<=0)
    avail+=size;
  if (avail<maxlength)
    maxlength=avail;
  for (length=0;length<maxlength;length++)
  {
    ether->outbuf[head++]=buffer[length];
    if (head==size)
      head=0;
  }
  ether->outhead=head;
  Restore_DS;
  return(length);
}

