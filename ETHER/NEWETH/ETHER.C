#define atoi
#define movmem
#include "mod.h"

#include "video.h"
#include "wdregs.h"

#define ETHER
#include "ether.h"

#define sockwid 40
#define sockhei 16
#define socketwindow 5000
#define max_records  20
#define max_retrans 10
#define MAX_USERS 10

#define connect_delay 500

short max_retrans_delay=1000;
short faked_ints=0;

#define NULL 0L

#define false 0
#define true !false
time_struc time=0,save_time=-500;
module far *input_TCB=0L,far *output_TCB=0L;

unsigned short card_collisions       /* ETHERNET CARD STATISTICS */
              ,card_allignment
              ,card_crcerrors
              ,card_missedcount
              ,card_sent_count
              ,card_send_busy
              ,card_received
              ,int_count;

Ether_addr MY_ether;
Ether_addr broad_ether={0xff,0xff,0xff,0xff,0xff,0xff};
short start_rec,end_rec;   /*first and last block of the receive buffers.
                              One per card, ok DATA */

/* Status
0x80   record socket
0x10   stop write NOW
   8   EOR
   4   Broken EOR
   2   stop read NOW
   1   Closing
*/

char shuttingdown=false,interupt=0;;

unsigned short alloc=0,dalloc=0;
#define Free(a) dalloc++;Free(a);
#define Allocate(a) Allocate(a);alloc++;

#define min(a,b)      ((a)<(b)?(a):(b))
#define cmpIP(a,b)    (*(long far*)a==*(long far*)b)
#define cmpEther(a,b) ((*(long far*)a==*(long far *)b)&& \
                      (*(((short far*)a)+2)==*(((short far*)b)+2)))

#define tcpptr ((TCP_prot far*)General_ptr)
#define ipptr  ((IP_prot far*)General_ptr)
#define pktptr ((Ether_layer far*)General_ptr)
#define arpptr ((ARP_prot far*)General_ptr)

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


extern void send_reset(TCP_prot far *tcp);
extern void init(void);

extern char readwd(char port);
extern void writewd(char port,char data);
extern char readwdcon(char port);
extern void writewdcon(char port,char data);

extern void init_wd(char trans_size,char recv_size);

extern short ipcheck(short far *pkt_ptr);
extern short tcpcheck(short far *pseudo_ptr,
                      short far *tcp_ptr,
                      short length,
                      char far *data,
                      short data_length);

#define moveIP(a,b) { *(long far*)a=*(long far*)b; }

#define moveEther(a,b) { *(long far*)a=*(long far*)b;  \
                         *(((short far*)a)+2)=*(((short far *)b)+2);}

#define intswap(a) (((unsigned)a>>8)|((unsigned)a<<8))

#define longswap(a) ( ((long)intswap((short)a)<<16) |         \
                      (intswap((short)(a>>16))) )


static short update;            /* variables for process_display */
static char how_many;
static connection far   *disp_con;
static connection far *l_disp_con;

window_type far *tcpwindow;            /* opened by us                  */
window_type far *output;               /* opened by others for us       */

connection far *first_connection=0L;

short IO_base=0x300;
char far *ether_ptr;  /*address of Ether Memory */


void complete(ARP_prot far *arp_reply)
{
  /*This routines tries to complete the connection which it thinks
    I started... I got a reply after all */

  connection far *temp;
  temp=first_connection;
  while (temp)
  {
    if (cmpIP(temp->I.source,arp_reply->A.target_prot_addr)&&
        cmpIP(temp->I.dest,arp_reply->A.source_prot_addr)&&
        temp->state&SNARP)
    {
      moveEther(temp->Edest,arp_reply->A.source_hard_addr);
      temp->state&=~SNARP;
      if (!(temp->state&(SNARP|SWSYN|SNSYN))) temp->state&=~SRESET;
    }
    temp=temp->next;
  }
}

void respond(ARP_prot far *request)
{
  char i;
  ARP_prot far *arp_responce;
  connection far *temp;
  temp=first_connection;
  while (temp)
  {
    if (cmpIP(temp->I.source,request->A.target_prot_addr) &&
        cmpIP(temp->I.dest,request->A.source_prot_addr)      ) break;

    temp=temp->next;
  }
  if (!temp)
  {
    temp=first_connection;
    while (temp)
    {
      if (cmpIP(temp->I.source,request->A.target_prot_addr) &&
          cmpIP(temp->I.dest,"\0\0\0\0")      ) break;

      temp=temp->next;
    }
  }

  if (temp)
  {
    while (readwd(COMMANDR)&4)
    {
      Relinquish(0L);
      card_send_busy++;
    }
    arp_responce=(ARP_prot far*)ether_ptr;

    if (temp->state&SNARP)
    {
      temp->state&=~SNARP;
      moveEther(temp->Edest,request->M.source);
      if (!(temp->state&(SNARP|SNSYN|SWSYN))) temp->state&=~SRESET;
    }
    for (i=0;i < 6;i++)
    {
      arp_responce->M.dest[i]=arp_responce->A.target_hard_addr[i]=request->M.source[i];
      arp_responce->M.source[i]=arp_responce->A.source_hard_addr[i]=MY_ether[i];
    }
    *(long far*)arp_responce->A.target_prot_addr=*(long far *)request->A.source_prot_addr;
    *(long far*)arp_responce->A.source_prot_addr=*(long far *)request->A.target_prot_addr;
    arp_responce->A.hardlen=6;
    arp_responce->A.protlen=4;
    arp_responce->A.opcode=0x200;   /*reply*/   /*1=arp_responce RARP 3,4*/
    arp_responce->A.hardware=0x100;
    arp_responce->A.protocol=8;
    arp_responce->M.type=0x608;
    writewd(TSTARTW,0);
    writewd(TCNTHW,0);
    writewd(TCNTLW,sizeof(ARP_prot)+18);

    writewd(COMMANDW,4);
    card_sent_count++;
  }
}

void send_Ether(connection far *temp,char far *data,short length)
{
  short far *General_ptr;

  short i;

  tcpptr=(TCP_prot far*)ether_ptr;

  moveEther(tcpptr->M.dest,temp->Edest);                   /*ETHER*/
  moveEther(tcpptr->M.source,MY_ether);
  tcpptr->M.type=IP_type;

  for (i=0;i < (temp->I.version_len&0xf);i++)                 /*IP*/
    *((long far *)&tcpptr->I.version_len+i)=
      *((long far*)&temp->I.version_len+i);

  for (i=0;i < (temp->T.hlen>>4);i++)                         /*TCP*/
    *((long far *)&tcpptr->T.source+i)=
      *((long far*)&temp->T.source+i);

  for (i=0;i < length;i++)                                    /*DATA*/
  {
    tcpptr->data[i]=data[i];
  }
  writewd(TSTARTW,0);
  writewd(TCNTHW,(sizeof(TCP_prot)+6+length)>>8);
  writewd(TCNTLW,(sizeof(TCP_prot)+6+length)&0xff);

  card_collisions +=readwd(COLCNTR);
  card_allignment +=readwd(ALICNTR);
  card_crcerrors  +=readwd(CRCCNTR);
  card_missedcount+=readwd( MPCNTR);

  writewd(COMMANDW,4);
  card_sent_count++;
}

void release(seq_data far *what,connection far *where)
{
  seq_data far *seqtemp=where->outstanding_recs,far *prior=0L;

  if (what->sent&RELEASING) return;
  what->sent|=RELEASING;

  while (what->sent&SENDING) Relinquish(0L);
  while (seqtemp&&seqtemp!=what)
  {
    prior=seqtemp;
    seqtemp=seqtemp->next;
  }
  if (seqtemp)
  {
    if (prior)
      prior->next=seqtemp->next;
    else
    {
      where->outstanding_recs=seqtemp->next;
      if (!where->outstanding_recs)
        where->packets=0;
    }
    Free(seqtemp);
  }
  if (where->packets)
    where->packets--;
}

void remove_acked(long ack,connection far *socket)
{
  seq_data far *seqtemp, far *prior;

restart:

  seqtemp=socket->outstanding_recs;
  prior=0L;

  while (seqtemp)
  {
    if ((ack >= seqtemp->seq) && prior)  /* if block is acked upto or beyond,*/
    {                                 /*  and there was a prior block, then */
      release(prior,socket);          /* release it, because it is obviously*/
      goto restart;                   /* acked                              */
    }
    prior=seqtemp;
    seqtemp=seqtemp->next;
  }

  if (prior)                          /* see if we can release the last one */

    if (ack == socket->highest_seq)
    {
      release(prior,socket);
    }
}

seq_data far *allocate_seq(char far *data,short length,
                           char flags,connection far *ether)
{
   /*This routine computes the current sequence number for this
     block of data, and fills in the block appropriately*/
  seq_data far *seqtemp;
  seqtemp=ether->outstanding_recs;
  if (seqtemp)
  {
    while (seqtemp->next) seqtemp=seqtemp->next;
    seqtemp=seqtemp->next=Allocate(sizeof(seq_data));
  }
  else
  {
    seqtemp=ether->outstanding_recs=Allocate(sizeof(seq_data));
  }
  seqtemp->next=NULL;
  seqtemp->data=data;
  seqtemp->seq=ether->highest_seq;
  seqtemp->length=length;
/*  seqtemp->ident=ether->current_ident; */
  seqtemp->flags=flags;
  seqtemp->sent=0;
  seqtemp->times_sent=0;  /*added 6/11/93- used for finding invalid connections*/
  seqtemp->time_sent=0;

  ether->highest_seq+=length;
/*  ether->current_ident++;              */
  if (ether->packets==1)
    ether->mult_packets++;
  ether->packets++;

  return(seqtemp);
}

short send_tcp(connection far *tmpcon,seq_data far *block)
{
  short temp_win=0,temp_rec;

  time_struc longtime;

  if    (block->length&&                     /*if data to transmit*/
         tmpcon->Dwindow < block->length&&   /*and it will not fit in window*/
         !(block->sent&SENT))                /*and it was not previously sent*/
  {
    return(false);
  }

/* this was commented out 6/11/93 because we do not wanna abort the sending
   of data on a transmit anymore */
//  if (tmpcon->status&WRITE_TERM)   /* write terminated  */
//    return(false);

  block->sent|=SENDING;
  if (block->times_sent++>max_retrans)  /*added 6/11/93- used for finding invalid connections*/
  {
    tmpcon->state|=SCLOSED;
  }


  if (block->sent&SENT)
  {              /*if block has     been send */

    tmpcon->retransmits++;
    if(!(block->sent&SENTAGAIN))
    {     /* INCREMENT RTO IF FIRST RETRANSMIT */
      tmpcon->rto += tmpcon->srtt + 2 * tmpcon->sdev;
      if (tmpcon->rto>max_retrans_delay) tmpcon->rto = max_retrans_delay;
    }
  }
  else
  {              /*if block has not been sent */
    tmpcon->Dwindow-=block->length;
    tmpcon->lowest_not_sent_seq=block->seq+block->length;
    tmpcon->transmits++;
  }

  while (readwd(COMMANDR)&4)
    {
      Relinquish(0L);
      card_send_busy++;
    }

  gettime(&longtime);
  tmpcon->last_packet_sent=block->time_sent=longtime & 0x3fff; /* remember when first sent */

  {
    tmpcon->I.length=intswap(sizeof(IP_layer)+sizeof(TCP_layer)+block->length);

    tmpcon->I.ident=tmpcon->current_ident++;

    tmpcon->I.checksum=0;
    tmpcon->I.checksum=ipcheck((short far*)&tmpcon->I.version_len);

    tmpcon->T.control=block->flags|tmpcon->flags;
    tmpcon->flags=0;
    tmpcon->T.seq=longswap(block->seq);
    tmpcon->T.ack=longswap(tmpcon->T.ack);

    tmpcon->PI.length=intswap(sizeof(TCP_layer)+(block->length) );

    tmpcon->T.checksum=0;
    tmpcon->T.checksum=tcpcheck((short far*)&tmpcon->PI,
                              (short far*)&tmpcon->T,
                              sizeof(TCP_layer),
                              block->data,
                              block->length);
    send_Ether(tmpcon,block->data,block->length);
    tmpcon->last_ad=intswap(tmpcon->T.window);
    tmpcon->T.ack=longswap(tmpcon->T.ack);     /*put current ack field back*/
    if (temp_win) tmpcon->T.window=temp_win; /*restore window size if we lied*/
  }
  if (block->sent&SENT) block->sent|=SENTAGAIN;
  block->sent|=SENT;
  block->sent&=~SENDING;

  return(true);
}


void send_ARP(connection far *tmpcon)
{
  char i;
  short far *General_ptr;

  arpptr=(ARP_prot far*)ether_ptr;
  while (readwd(COMMANDR)&4)
    {
      Relinquish(0L);
      card_send_busy++;
    }
  for (i=0;i < 6;i++)
  {
    arpptr->M.dest[i]=arpptr->A.target_hard_addr[i]=tmpcon->Edest[i];
    arpptr->M.source[i]=arpptr->A.source_hard_addr[i]=MY_ether[i];
  }
  arpptr->M.type=ARP_type;
  moveIP(arpptr->A.target_prot_addr,tmpcon->I.dest);
  moveIP(arpptr->A.source_prot_addr,tmpcon->I.source);
  arpptr->A.hardlen=6;
  arpptr->A.protlen=4;
  arpptr->A.opcode=0x100;   /*request*/   /*2=reply RARP 3,4*/
  arpptr->A.hardware=0x100;
  arpptr->A.protocol=8;
  writewd(TSTARTW,0);
  writewd(TCNTHW,0);
  writewd(TCNTLW,sizeof(ARP_prot)+18);

  writewd(COMMANDW,4);
  card_sent_count++;

  displayln(output," S-ARP");
}

void wake_output()
{
  output_TCB->status=0;
}

public(connection far *,openether,(char far *source_address,
                               char far *dest_address,
                               short t_source,short t_dest,char opts))
{
  /*this routine opens an IP connection and returns the pointer to the
    Ether control block
    Options- xxxx xxx1=record socket
             xxxx xx1x=server socet (No Open)*/
  connection far *temp;
  seq_data far *current;
  short cnt;
  Load_DS;
  if (shuttingdown)
  {
    temp=0L;
    goto exit_open;
  }
  temp=first_connection;
  while(temp)
  {
    if (cmpIP(temp->I.source,source_address)&&
        cmpIP(temp->I.dest,dest_address)&&
        !cmpIP(temp->I.dest,"\0\0\0\0")&&
        temp->T.source==t_source&&
        temp->T.dest==t_dest)
    {
      while (temp->state) Relinquish(0L);
      Restore_DS;

      return((connection far*)temp);
    }
    temp=temp->next;
  }
  alloc_last(first_connection,temp,sizeof(connection));
  if (opts&RECORD_SOC)
  {
    temp->rec_list=Allocate(sizeof(record)*max_records);
    temp->last_rec=max_records;
    temp->first_rec=0;
    temp->cur_rec=0;
  }
  temp->next=NULL;
  temp->Dwindow=0;
  temp->retransmitting=false;
  temp->status=0;
  temp->status|=(opts&QUICK_IO);
  if (cmpIP(dest_address,"\0\0\0\0")&&
      t_dest==0)
  {
    temp->status|=SERVER_SOCKET;
    temp->status&=~RECORD_SOCKET;
  }
/*The following are constants that shouldn't change during normal usage*/
  moveEther(temp->Edest,broad_ether);
  temp->inbuf=Allocate(socketwindow);
reinit_socket:
  temp->ihead=0;
  temp->itail=0;
  temp->base=0;
  temp->hend=0;
  temp->I.version_len=0x45;
  temp->I.service=0;
  temp->I.ident=intswap(1);
  temp->I.frags=0;
  temp->I.time_to_live=64;
  temp->I.protocol=6;
  temp->PI.prot=6;
  temp->PI.zero=0;
  temp->PI.zeros=0L;

  temp->T.hlen=0x50;
  temp->T.urgent=0;
  temp->T.window=intswap(socketwindow);

  temp->T.dest=intswap(t_dest);
  temp->T.source=intswap(t_source);
  moveIP(temp->I.dest,dest_address);
  moveIP(temp->I.source,source_address);
  moveIP(temp->PI.dest,dest_address);
  moveIP(temp->PI.source,source_address);

  gettime((time_struc far *)&temp->T.ack);       /*produce pseudo       */
  temp->T.seq=(temp->T.ack&=0xffff)&0xfff;       /*random staring places*/

  temp->highest_seq=temp->highest_seq;  /*same as above*/
  temp->current_ident=0;       /*current ident*/
  temp->outstanding_recs=0L;   /*nothing outstanding */
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
  temp->packets=0;
  temp->mult_packets=0;
  temp->users=1; /*added 6/11/93- used for closing connections we are opening it
                   so we must be using it*/
  temp->bytes_outstanding=0;
  temp->last_write=NULL;

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


public (void,etherflush,(connection far *ether))
{
  ether->ihead=ether->itail=ether->base=ether->hend=0;
  ether->T.window=intswap(socketwindow);
  Load_DS;
  wake_output();
  Restore_DS;
}


public (short,close_connection,(connection far *ether))  /*return changed 6/11/93*/
{
  connection far *temp;
  short cnt,loop;
  seq_data far *current;
  Load_DS;

  temp=first_connection;
  while (temp)
  {
    if (temp==ether) break;
    temp=temp->next;
  }
  if (temp==0L||(temp->status&CLOSING)||temp->users) /*added 6/11/93- used for closing connections*/
  {
    Restore_DS;
    return(false);
  }
  temp->status|=CLOSING;           /* temp == ether from here on */

  displayln(output," closing...");

  /*Free the buffers*/
  loop=0;
  ether->status&=~(WRITE_TERM|READ_TERM);
  current=allocate_seq(0,0,TFIN,ether);
loop_top:
  if (!(ether->status&ENDRECORD))
    while (!send_tcp(ether,current))
    {
      release(current,ether);
      Relinquish(0L);
      current=allocate_seq(0,0,TFIN,ether);
    }

  cnt=0;
  release(current,ether);
  current=allocate_seq(0,0,TRESET,ether);
  while (!(ether->state&SCLOSED))
  {
    cnt++;
    if (cnt > 10000)
    {
      loop++;
      if (loop > 4)
      {
        while (!send_tcp(ether,current))
        {
          release(current,ether);
          Relinquish(0L);
          current=allocate_seq(0,0,TRESET,ether);
        }
        break;
      }
    }
    Relinquish(0L);
  }

  if(temp==disp_con)disp_con=0L; /*if closeing current don't display it! */
  l_disp_con=0L;                 /*   and update everything              */

  release(current,ether);
  remove_acked(ether->highest_seq,ether);
  Free(ether->inbuf);
  /*unlink from queue and deallocate connection structure */
  temp=first_connection;
  if (first_connection==ether)
    first_connection=first_connection->next;
  else
  {
    while ((temp->next!=ether)&&(temp->next))
    {
      temp=temp->next;
    }
    temp->next=ether->next;
  }
  Free(ether);
  displayln(output," done...");

  Restore_DS;
  return(true);
}


void close_connections()
{
  connection far *temp;
  temp=first_connection;
  while (temp)
  {
    if (!(temp->status&CLOSING)&&!(temp->state))
      close_connection(temp);
    temp=temp->next;
  }
}

cleanup(void,disconnect,(void))
{
  Load_DS;
  shuttingdown=true;
  displayln(output," Closing Connections!");
  close_connections();
  stop_wd();
  Restore_DS;
}

public(void,Ether_term,
       (connection far *ether,short read))
{
  if (read)
    ether->status|=READ_TERM;
  else
    ether->status|=WRITE_TERM;
}

public(unsigned short,readether,
     (char far *buffer,unsigned short maxlength,connection far *ether))
{
  unsigned short c=0;
  unsigned short cnt;
  unsigned short tail;
  unsigned short head;
  unsigned short first_rec,cur_rec;

  Load_DS;

  if (ether->state||shuttingdown||!ether)
  {
     Restore_DS;
     return(-1);
  }
  while (ether->users>MAX_USERS) /*added 6/11/93- used for closing connections*/
    Relinquish(0L);
  ether->users++;
  ether->status&=~READ_TERM; /*make sure we don't have a terminate left over*/
  {
    tail=ether->itail;
    head=ether->base;
    while ((tail!=head)&&(c < maxlength))
    {
      buffer[c++]=ether->inbuf[tail++];
      if (tail==socketwindow) tail=0;
    }
    ether->itail=tail;
    ether->T.window=intswap(intswap(ether->T.window)+c);

    if ((ether->last_ad + 1000) <= intswap(ether->T.window) ||
        (ether->last_ad < socketwindow &&
         intswap(ether->T.window)==socketwindow))
             /*make sure enough window advertised*/
      ether->flags|=TACK;  /*say that we should send ack*/

  }
  ether->users--; /*added 6/11/93- used for closing connections*/
  Restore_DS;
  if (ether->state)
  {
                      /*6/11/93  If there is a state flag, then the
                        connection is invalid and we need to return the
                        error to the caller.*/
    return(-1);
  }
  else
    return(c);         /*we can return the actual count read*/
}

public(unsigned short,sendether,
       (char far *buffer,unsigned short length,connection far *ether))
{
  unsigned short temp_length;
  unsigned short block;
  seq_data far *current;
  seq_data far *seqtemp;
  char done;
  unsigned short pos;

  Load_DS;

  ether->status&=~WRITE_TERM;  /*make sure we don't have a terminate laying about*/
  if (ether->status&QUICK_IO&&ether->bytes_outstanding)
    goto check_write_done;
  if (ether->state||shuttingdown||!ether)
  {
    Restore_DS;
    return(-1);
  }

  while (ether->users>MAX_USERS)/*added 6/11/93- used for closing connections*/
    Relinquish(0L);
  ether->users++;

  if (length)
  {
    block=length/1500;
    temp_length=length%1500;
    pos=0;
    for (;block > 0;block--)
    {
      current=allocate_seq(buffer+pos,1500,
                 (block==1&&!temp_length)?TPUSH:0,ether);
      pos+=1500;
    }
    if (temp_length)
      current=allocate_seq(buffer+pos,temp_length,
                              TPUSH,ether);
  }
  else
    current=allocate_seq(0,1,0x80|TPUSH,ether);


  do
  {
    {
check_write_done:
      if (ether->status&QUICK_IO)
      {
        if (ether->bytes_outstanding)
          current=ether->last_write;
      }
      else
      {
        ether->last_write=current;
        ether->bytes_outstanding=length;
      }
      seqtemp=ether->outstanding_recs;
      done=true;
      while (seqtemp)
      {
        if (seqtemp==current)
        {
          done=false;
          break;
        }
        seqtemp=seqtemp->next;
      }
      if (!done)  /*added 6/11/93 to add efficiency when in fact we are done.*/
      {
        if (ether->status&QUICK_IO)
          return(0);
        else
        {
          wake_output();
          Relinquish(0L);
        }
      }
      else
      {
        length=ether->bytes_outstanding;
        ether->bytes_outstanding=0;
        ether->last_write=NULL;
      }
    }
  }
  while (!done&&!ether->state&&!(ether->status&WRITE_TERM));
  if (ether->state||(ether->status&WRITE_TERM))   /*someone damaged the socket return that we did
                          not in fact send any data... though we may have*/
  {
    length=-1;
  }

  ether->users--;     /*added 6/11/93- used for closing connections*/

  Restore_DS;

  return(length);
}


void process_display()
{
  char cnt=0;
  short keyboard_input;
  seq_data far *current;


  if(keypressed(tcpwindow))              /* check our keyboard input */
  {
    position(output,10,6);

    keyboard_input=readch(tcpwindow);

    if(keyboard_input==0) /* extended key */
       keyboard_input=readch(tcpwindow)|0x0100;

    if(keyboard_input==0x004e)
    {                             /* "N" go to next  connection */
      disp_con=disp_con->next;
      l_disp_con=0L;
      goto mine;
    }
    if(keyboard_input==0x0046)
    {                             /* "F" go to first connection */
      disp_con=first_connection;
      l_disp_con=0L;
      goto mine;
    }
  }
mine:
/*  gettime(&time);
  if (time-save_time>500)
  {
    save_time=time;
  }
  else
    return;
  if(!(update&=0xfff))*/
    l_disp_con=0L; /* update periodically */
  update++;

  position(output,0,0);

  {
    char ints=readwd(INTSTATUSR);
    displayln(output," Sockets:*5d   *n  Collisions:*5d  \n"
                    ,how_many,faked_ints,card_collisions);
    if (ints)
    {
       faked_ints++;
       writewd(INTSTATUSR,ints);
       output_TCB->status=0;
       input_TCB->status=0;
    }
  }

  displayln(output,"Received:*5d       Allignment:*5d  \n"
                  ,card_received,card_allignment);

  displayln(output,"CRC errs:*5d   Missed packets:*5d  \n"
                  ,card_crcerrors,card_missedcount);

  displayln(output,"    Sent:*5d       Found busy:*5d  \n"
                  ,card_sent_count,card_send_busy);

  displayln(output,"   Alloc:*5d  *5d    Dalloc:*5d  \n"
                  ,alloc,int_count,dalloc);

  if(!disp_con)       /* if we are not displaying already */
  {                   /*   start up  tests */

    disp_con=first_connection;
    if(!disp_con)return;

    if(!tcpwindow)tcpwindow=opendisplay(20,1,sockwid,sockhei
                          ,NO_CURSOR|BORDER|NEWLINE,0x1f,0x1a,0,"Socket");
  }

  position(tcpwindow,0,0);
  if(l_disp_con != disp_con)clr_display(tcpwindow,3);

  displayln(tcpwindow,"State:*s *s *s *s *s *s *s *s\n",
               (char far*)"",(char far*)"",
               (char far*)((disp_con->state&DLYWSYN  )?"DLYW":"    "),
               (char far*)((disp_con->state&SCLOSED  )?"CLOS":"    "),
               (char far*)((disp_con->state&SRESET   )?"RST" :"   " ),
               (char far*)((disp_con->state&SWSYN    )?"WSYN":"    "),
               (char far*)((disp_con->state&SNSYN    )?"NSYN":"    "),
               (char far*)((disp_con->state&SNARP    )?"NARP":"    "));

  if(l_disp_con != disp_con)
  {
    displayln(tcpwindow,"D IP:*n.*n.*n.*n TCP:*d\n",
                        disp_con->I.dest[0],disp_con->I.dest[1],
                        disp_con->I.dest[2],disp_con->I.dest[3],
                        intswap(disp_con->T.dest));

    displayln(tcpwindow,"S IP:*n.*n.*n.*n TCP:*d\n",
                        disp_con->I.source[0],disp_con->I.source[1],
                        disp_con->I.source[2],disp_con->I.source[3],
                        intswap(disp_con->T.source));
  }
  else
    position(tcpwindow,0,3);

  displayln(tcpwindow,"D window:*5d  Flags:*n\n"
                          ,disp_con->Dwindow,disp_con->flags);

  displayln(tcpwindow,"S window:*5d   SRTT:*4d *4d *4d\n"
                          ,intswap(disp_con->T.window)
                          ,disp_con->srtt,disp_con->sdev
                          ,disp_con->rto);



  if(l_disp_con != disp_con)
  {
    displayln(tcpwindow,"First:*H  Next:*H  \n"
                            ,(long)first_connection
                            ,(long)disp_con->next);
    displayln(tcpwindow,"  IAm:*H\n",(long)disp_con);
  }
  else
    position(tcpwindow,0,7);

  displayln(tcpwindow,"Seq :*R  Ack:*H\n"
                          ,(long)disp_con->T.seq,(long)disp_con->T.ack);

  displayln(tcpwindow,"HSeq:*H  LNS:*H\n"
                          ,(long)disp_con->highest_seq
                          ,disp_con->lowest_not_sent_seq);

  current=disp_con->outstanding_recs;
  while (current && (cnt < 6) )
  {
     cnt++;
     displayln(tcpwindow,"Seq :*H len:*4d sent:*h\n",
                 current->seq,current->length,current->sent);
     current=current->next;
  }

  clr_display(tcpwindow,3);
  l_disp_con=disp_con;

/*-v-v-v-v-v-v-v-v-v-v-v-v-v-v- fix some day -v-v-v-v-v-v-v-v-v-v-*/
/*                       these need headings                      */

  position(tcpwindow,31, 1);
  displayln(tcpwindow,"*5d:XMT", disp_con->transmits);

  position(tcpwindow,31, 2);
  displayln(tcpwindow,"*5d:RXT", disp_con->retransmits);

  position(tcpwindow,31, 3);
  displayln(tcpwindow,"*5d:MPK", disp_con->mult_packets);

  position(tcpwindow,31, 5);
  displayln(tcpwindow,"*5d:RCV", disp_con->p_received);

  position(tcpwindow,31, 6);
  displayln(tcpwindow,"*5d:DUP", disp_con->rec_duplicats );

  position(tcpwindow,31, 7);
  displayln(tcpwindow,"*5d:OVF", disp_con->over_flows );

/*-^-^-^-^-^-^-^-^-^-^-^-^-^-^- fix some day -^-^-^-^-^-^-^-^-^-^-*/


}


static char force_transmit;
void process_card()
{
  short far *General_ptr;
  Ether_header far *etherhead;
  char  from;
  short lngth;
  short dif;
  connection far *tmpcon;
  long temp_ack;



  while ((from=readwd(BOUNDR))!=readwd(CURRR))
  {
    char i;
    if (from<start_rec)
    {
      displayln(output,"PANIC!PANIC!PANIC!");
      init_wd(TRANS_SIZE,RECV_SIZE);
      return;
    }
    etherhead=(Ether_header far*)((long)ether_ptr|((short)from<<8));
    pktptr=(Ether_layer far *)(etherhead+1);

    card_received++;

    if (pktptr->type==ARP_type)   /*ARP transaction of some sort*/
    {

      displayln(output," R-ARP=*h",arpptr->A.opcode);

      if (arpptr->A.opcode==0x100)
        respond(arpptr);
      if (arpptr->A.opcode==0x200)
        complete(arpptr);
    }
    else
    if (pktptr->type==IP_type)
    {
      switch(ipptr->I.protocol)
      {
        case 1:displayln(output," [ICMP]");
               break;
        case 6:
          tmpcon=first_connection;
          while (tmpcon)
          {
            if (cmpIP(tcpptr->I.source,tmpcon->I.dest)&&
                cmpIP(tcpptr->I.dest,tmpcon->I.source)&&
                tcpptr->T.source==tmpcon->T.dest&&
                tcpptr->T.dest==tmpcon->T.source)
              break;
            tmpcon=tmpcon->next;
          }
          if (!tmpcon)
          {
            /*look for blank ports if there wasn't an actaully known
              connection*/
            tmpcon=first_connection;
            while (tmpcon)
            {
              if (cmpIP("\0\0\0\0",tmpcon->I.dest)&&
                  cmpIP(tcpptr->I.dest,tmpcon->I.source)&&
                  tmpcon->T.dest==0&&
                  tcpptr->T.dest==tmpcon->T.source)
                break;
              tmpcon=tmpcon->next;
            }
            if (tmpcon)
            {
              /*fill in the blanks for the connection addresses.  And, all
                is filled for a complete socket from now on.*/
              moveIP(tmpcon->I.dest,tcpptr->I.source);
              moveIP(tmpcon->PI.dest,tcpptr->I.source);
              tmpcon->T.dest=tcpptr->T.source;
            }
          }

           /*If there wasn't a socket that matched, or the one that was
             found was closed, then send a reset so we don't connect */
          if (!tmpcon||(tmpcon->state&SCLOSED))
          {
            displayln(output," [S-RSET]");
            /*Check for a blank Socket later*/
            asm int 3;
            send_reset(tcpptr);
            break;
          }

          tmpcon->p_received++;

          /* if we have gotten a reset, and we are not already working
                on coming back from a reset */

          if (tcpptr->T.control&TRESET)
          {

            displayln(output," R-RSET");

            tmpcon->state|=SNSYN|SWSYN; /*reset, and Not acked SYN, and Not sent SEQ */
            tmpcon->flags=0;
          }

          if ((tcpptr->T.control&TFIN) && !(tmpcon->state&SCLOSED))
          {

            displayln(output," R-FIN");

            tmpcon->state|=SCLOSED;
            if (!(tmpcon->status&CLOSING))
              tmpcon->flags|=TFIN;
          }

          if (tcpptr->T.control&TACK)
          {
            char okay=true;
            temp_ack=longswap(tcpptr->T.ack);
            if (tmpcon->state&SNSYN)
            {
              if ((tmpcon->highest_seq+1)==temp_ack)
              {
                tmpcon->state&=~(SNSYN);
                if (!(tmpcon->state&(SNARP|SNSYN|SWSYN)))
                  tmpcon->state&=~SRESET;
                displayln(output," R-ACK->SYN");
                tmpcon->highest_seq++;
              }
              else
              {
                displayln(output," {S-RSET}");
                send_reset(tcpptr);
                okay=false;
              }
            }
            if (okay)
            {

              if(    (tmpcon->outstanding_recs)
                  && ((tmpcon->outstanding_recs->sent
                       &(SENT|SENDING|SENTAGAIN)) == SENT)
                  && (temp_ack > tmpcon->outstanding_recs->seq              )
                )
              {      /* we have a valid acknowledge to our first record */

               time_struc longtime;
               short rtt;

               gettime(&longtime);

               rtt = (longtime & 0x3fff)
                    - tmpcon->outstanding_recs->time_sent;
               if(rtt < 0) rtt+=0x4000;

               if(rtt > (max_retrans_delay << 1))rtt=tmpcon->srtt;

               tmpcon->srtt = ldiv(lmult(srtt_weight,tmpcon->srtt)
                         + lmult(100-srtt_weight,rtt),100) ;

               if(tmpcon->srtt < 10)
                  tmpcon->srtt = 10;

               if(tmpcon->srtt > (max_retrans_delay>>1))
                  tmpcon->srtt = (max_retrans_delay>>1);

               if ((dif=(tmpcon->srtt-rtt)) < 0)
                 tmpcon->sdev =ldiv(lmult(sdev_weight,tmpcon->sdev)
                         +  lmult(100 - sdev_weight,-dif),100) ;
               else
                 tmpcon->sdev =ldiv(lmult(sdev_weight,tmpcon->sdev)
                         +  lmult(100 - sdev_weight, dif),100) ;

               if( tmpcon->sdev < (dif = (tmpcon->srtt >> 2) )
                 ) tmpcon->sdev = dif;

               if((tmpcon->rto = tmpcon->srtt + (2 * (tmpcon->sdev+=2)))
                   > max_retrans_delay)
                  tmpcon->rto = max_retrans_delay;

              }
              remove_acked(temp_ack,tmpcon);
            }
          }

          tmpcon->Dwindow = (intswap(tcpptr->T.window))
                          - (tmpcon->lowest_not_sent_seq
                             -longswap(tcpptr->T.ack)      );

          if (tcpptr->T.control&TSYN)
          {
            temp_ack=longswap(tcpptr->T.seq)+1;

            if (!(tmpcon->state&(SRESET|SWSYN))
              && (temp_ack != tmpcon->T.ack) )
            {
                    /*Not reset, and have sent ack*/
                    /*got unresonable second SYN */

/*              send_reset(tcpptr);  */
              tmpcon->state|=(SRESET|SWSYN|SNSYN);
              displayln(output," U-SYN");
            }
            else
            {        /*this is a resonable syn */

              displayln(output," R-SYN");
            }

            tmpcon->T.ack=temp_ack;
            tmpcon->flags|=TACK;
            tmpcon->state&=~SWSYN;
            if (!(tmpcon->state&(SNARP|SNSYN|SWSYN))) tmpcon->state&=~SRESET;
            if (tmpcon->state&SNSYN)
            {
                                /*we had a connection, and apparently the
                                  other side got amnisia.  Remind him of
                                  where we are at. Embed with Ack so because
                                  he is at state 3 he will not remind us
                                  of who he is.  */
              tmpcon->flags|=TSYN;
              displayln(output," s-SYN");
            }
          }

          lngth=intswap(tcpptr->I.length)-
                     sizeof(IP_layer)-
                     ( (tcpptr->T.hlen&0xf0)>>2);
          if (!tmpcon->state&&lngth&&(lngth <= intswap(tmpcon->T.window)) )
          {
            short idx,cnt;
            idx=(long)tmpcon->base+longswap(tcpptr->T.seq)-tmpcon->T.ack;
            if (idx >= socketwindow)
            {
              idx-=socketwindow;
              if (idx >= socketwindow)
                break;  /*exit switch*/
            }


            if (idx==tmpcon->base)
            {
              /*Store data in buffer, because this is where I expect to be*/
              if (!(tcpptr->T.control&0x80))
              {
                /* no hole, this is also where the end of the data is*/
                /*compute current byte position, which is all of the
                  protocol headers to get to where the data is*/
                short remainder;
                char far *data=tcpptr->data,
                     far *buffer=tmpcon->inbuf+idx;
                  cnt=0;
                  do
                  {
                    remainder=min((end_rec<<8)-((short)data),
                                  min(socketwindow-(unsigned short)buffer,
                                      lngth));
                    movmem(data,buffer,remainder);
                    cnt+=remainder;
                    data+=remainder;
                    if ((short)data==(end_rec<<8))
                      data=(char far *)((long)ether_ptr+(start_rec<<8));
                    buffer+=remainder;
                    if ((short)buffer==socketwindow)
                      buffer=tmpcon->inbuf;
                    lngth-=remainder;
                  }
                  while (lngth);

                tmpcon->base=(short)buffer;
                tmpcon->ihead=(short)buffer;
                tmpcon->hend=(short)buffer;
                tmpcon->T.ack+=cnt;
              }
              else
              {
                /* EOF, no data, just say it was ok*/
                tmpcon->T.ack+=lngth;
                cnt=0;
              }
              tmpcon->flags|=TACK;
              tmpcon->T.window=intswap(intswap(tmpcon->T.window)-cnt);
            }
            else
            {

              tmpcon->rec_duplicats++;
              tmpcon->p_received--;      /* if a duplicate don't count here*/

              if (longswap(tcpptr->T.seq) <= tmpcon->T.ack)
              {

                tmpcon->flags|=TACK; /*if the packet was
                                          earlier, then ack it*/
              }
            }
          }
          else
overflow:
            if (lngth)
            {
              tmpcon->over_flows++;
            }


          if (tmpcon->flags) force_transmit=true;

          break;

        case 11:
          displayln(output,"[UDP]");
          break;

        default: displayln(output,"[??]");
      }
    }
exit:
    if (etherhead->next<start_rec)
    {
      displayln(output,"TROUBLED WATERS AHEAD!");
      init_wd(TRANS_SIZE,RECV_SIZE);
      return;
    }
    writewd(BOUNDW,etherhead->next);
  }
}


void process_connections()
{
  connection far *tmpcon;
  seq_data far *current;
  time_struc current_time;
  tmpcon=first_connection;


  how_many=0;                   /*count for display */

/*  while (tmpcon&&!(tmpcon->status&CLOSING)) <--old line.... is invalid to not
      count/process closing nodes*/
  while (tmpcon)
  {
    if (tmpcon->status&CLOSING)  /*though this is valid ... 6/11/93*/
    {
      how_many++;
      tmpcon=tmpcon->next;
      continue;
    }
    tmpcon->users++;             /* added 6/11/93 we need to stop the
                                    socket from closing just yet if
                                    it relinquishes during send*/
    gettime(&current_time);
    if (tmpcon->state&&(current_time>tmpcon->connect_timer)&&
        !(tmpcon->status&SERVER_SOCKET))
    {
      if (tmpcon->state&SNARP)
      {
        send_ARP(tmpcon);
        tmpcon->connect_timer+=connect_delay;
      }
      else
        if (tmpcon->state&SNSYN)
        {
          tmpcon->flags|=TSYN;
          displayln(output," S-SYN");
          tmpcon->connect_timer+=connect_delay;
        }
        else
        if (tmpcon->state&SWSYN)
          {
            displayln(output," WSYN");
            if (tmpcon->state&DLYWSYN)
            {
              tmpcon->flags|=TSYN;
              tmpcon->state&=~DLYWSYN;
            }
            else
            {
              tmpcon->state|=DLYWSYN;
            }
            tmpcon->connect_timer+=connect_delay;
          }
      if (!(tmpcon->state&(SNARP|SNSYN|SWSYN)))
        tmpcon->state&=~SRESET;
    }

    if ((current=tmpcon->outstanding_recs) != NULL) /* do we have any work ? */
    {

      if(current->sent&SENT) /* has the first record timed out? */
      {                      /* or we are in recovery mode      */
        time_struc longtime;
        short elapsedtime;

        gettime(&longtime);

        elapsedtime = (longtime & 0x3fff) - current->time_sent;
        if(elapsedtime < 0 ) elapsedtime+=0x4000;


        if(elapsedtime > tmpcon->rto)
           /*         (tmpcon->packets)?tmpcon->packets:1)) ) *//* 1.0 seconds? */
        {
          send_tcp(tmpcon,current);    /* we should have ack   */
          tmpcon->retransmitting=true;
          if(tmpcon == disp_con)
          {
            position(tcpwindow,22,6);
            displayln(tcpwindow,"*5d",elapsedtime);
          }
        }

        if(tmpcon->retransmitting)
        {
          if(tmpcon->flags)
          {
            short savelength=current->length;
            char  saveflags=current->flags;
            char  savesent=current->sent;

            current->flags=current->length=0;

            send_tcp(tmpcon,current);

            current->length=savelength;
            current->flags =saveflags;
            current->sent=savesent;
          }
          tmpcon->users--;             /*6/14/93 */
          how_many++;
          tmpcon=tmpcon->next;         /* get it out of here */
          continue;                    /* correctly before continuing */
        }


        if (current->flags==0   && current->length==0
            && (current->sent&SENT)&& current->next==0L)
          release(current,tmpcon);

      }


                     /*  look for & process new work   */


      tmpcon->retransmitting=false;

      current=tmpcon->outstanding_recs;


      while (current&&current->sent&SENT)   /* find first record not sent*/
        current=current->next;

      while (current)
      {
        tmpcon->lowest_not_sent_seq=current->seq;

        if (send_tcp(tmpcon,current))
        {
          current=current->next;  /*while we can still send, keep
                                    sending packets*/
        }
        else
          break;
      }

      if(tmpcon->flags)
      {
        if(current)
        {               /* we didn't send all records */
          short savelength=current->length;
          char  saveflags=current->flags;

          current->flags=current->length=0;

          send_tcp(tmpcon,current);


          current->sent=0;
          current->length=savelength;
          current->flags =saveflags;
        }
        else
        {               /* there are no more records to send */
          goto sendflags;
        }
      }
    }
    else                              /*if there are no records  */
    {                                 /*create one and send the  */
      tmpcon->retransmitting=false;   /*ack or whatever          */
      if (tmpcon->flags)              /*and we can't be retransmitting */
      {

sendflags:
       current=allocate_seq(0,0,0,tmpcon); /*this will be first, because
                                            there is NO prior data.  so,
                                            it will remain here until
                                            released by an ack coming back*/
        if (send_tcp(tmpcon,current))
          release(current,tmpcon);
      }
    }

    tmpcon->users--;     /*6/11/93 we are done using the socket... ok to close*/
    how_many++;                   /*count for display */
    tmpcon=tmpcon->next;
  }
}

void main(void)
{
  init();
  int_count=0;
  if (fork(INDEPENDANT))
  {                  /*runs second */
    input_TCB=my_TCB;
    while (true)
    {
      interupt&=~1;
      process_card();
      if(force_transmit)
      {
        swap_to(output_TCB);
        force_transmit=false;
      }
      else
      {
        process_display();
        asm cli;                 /*Relinquish re enables interupts */
        if (!(interupt&1))
          Relinquish(-5000L);
      }
    }
  }
  else
  {                  /*runs first */
    output_TCB=my_TCB;
    while (true)
    {
      interupt&=~2;
      process_connections();
      if (!(interupt&2))
        Relinquish(-5000L);
    }
  }
}

