#define extern_def
#include <mod.h>
#define ETHER
#include "ether.h"
#include "wdregs.h"
#include <video.h>

#define false 0
#define true (!false)


static short IRQ_table[8]={ 0xa, 0xb, 0xd, 0xf,0x72,0x73,0x77, 0xc};
static short IRQ_NUM  [8]={   2,   3,   5,   7,  10,  11,  15,   4};
static short IRQ_port [8]={0x20,0x20,0x20,0x20,0xa0,0xa0,0xa0,0x20};
char WDreset_port=0x20;
char IRQ_idx=0;
static short IRQ_mask [8]={ 0x4, 0x8,0x20,0x80,0x04,0x08,0x80,0x10};
char WDinterupt;


static char far *send_buf;
static char far *receive_buf;
static start_rec,end_rec;
char far *ether_ptr;
short IO_base;

extern char MY_ether[6];
extern short card_collisions;
extern short card_allignment;
extern short card_crcerrors;
extern short card_missedcount;
extern short card_packets_sent;
extern short card_received;
extern short card_busyxmt;

extern char inportb();
extern void outportb(short port,char data);
extern void WDCard_int(void);


unsigned char readwd(char port)
{
  char temp;
  temp=(port&0xf0)<<2;
  outportb(IO_base+0x10,temp);
  return((short)0|inportb(IO_base+0x10+(port&0xf)));
}

void writewd(char port,char data)
{
  char temp;
  temp=(port&0xf0)<<2;
  outportb(IO_base+0x10,temp);
  outportb(IO_base+0x10+(port&0xf),data);
}

unsigned char readwdcon(char port)
{
  return(inportb(IO_base+port));
}

void writewdcon(char port,char data)
{  outportb(IO_base+port,data); }

void start_wd()
{
  writewd(COMMANDW,2);
}

void stopWD()
{
  outportb(WDreset_port+1,inportb(WDreset_port+1)|IRQ_mask[IRQ_idx]);
  Disconnect_Int(WDCard_int,IRQ_table[IRQ_idx]);
  writewd(COMMANDW,1);
  writewdcon(MSR,0x80);
  writewdcon(MSR,0);
}

short initWD(windowptr output,short port)
{
short temp_v;

  /*This routine sets up the initial conditions for the WD1013E card so
  that I can just watch all incoming packets, and display what address they
  are from */

  char t1,t2,irr;
  short Ether_irq;
  short cnt;
  short trans_size=TRANS_SIZE,receive_size=RECV_SIZE;

  IO_base=port;

  writewd(COMMANDW,1);  /*stop wd card in case it was already working*/
  writewdcon(ICR,0x71);  /*reset the Card */

  cnt=0;
  while (readwdcon(ICR)&0x70) /*read config */
  {
    if (cnt++>5000)  /*check the card 5000 times to see if it is there*/
    {
      displayln(output,"There is no Ether "\
                       "card Installed or "\
                       "the Port address is invalid");
      return(false);
    }
    Relinquish(0L);
  }

  t1=readwdcon(MSR)&0x3f;
  writewdcon(MSR,t1|MENB);  /*memory | MENB*/

  t2=readwdcon(LAAR)&0x1f;
  writewdcon(LAAR,t2|0xc0);  /*M16EN  L16EN | memory*/

#define near2far(a) (((long)a&0xffff0000L)|(long)((short)a))

  Ether_irq=IRQ_table[temp_v=(readwdcon(ICR)&2)|
                             (((irr=readwdcon(IRR))&0x60)>>5)];
  IRQ_idx=temp_v;
  Connect_Int(WDCard_int,Ether_irq);
  WDreset_port=IRQ_port[temp_v];
  outportb(WDreset_port+1,inportb(WDreset_port+1)&(~IRQ_mask[temp_v]));
  writewdcon(IRR,irr|0x80);   /*enable the cards interrupts */
  writewd(INTMASKW,3);
  while (readwd(INTSTATUSR)&0x7f)
    writewd(INTSTATUSW,readwd(INTSTATUSR));

  ether_ptr=(char far*)((long)((t1<<9)|(t2<<15))<<16);

/*  IO_base=(short)(readwdcon(IAR)<<5);*/

  for (t1=0;t1<6;t1++)
  {
    MY_ether[t1]=readwdcon(LAR1+t1);   /*get my address*/
    writewd(STA0W+t1,MY_ether[t1]);    /*as I'm getting my address make sure
                                          the card knows who it is*/
  }
  if (trans_size+receive_size>0x40)
    receive_size=0x40-trans_size;

  writewd(TSTARTW,0);
  writewd(RSTARTW,trans_size);

  start_rec=trans_size;
  writewd(RSTOPW,trans_size+receive_size);

  end_rec=trans_size+receive_size;
  writewd(BOUNDW,trans_size);
  writewd(CURRW,trans_size);
  writewd(RCONW,RGROUP|RBROAD);
  writewd(DCONW,0x41);
  writewd(TCONW,0);
  start_wd();
  position(output,0,5);

  displayln(output,"WD8013 PORT:*h IRQ:*d MEM:*H    "
                  ,IO_base,IRQ_NUM[temp_v],(long)ether_ptr);

  return(true);
}



void sendWD(char far *Edest,short type,char buffers,buftype far *bufs)
{
  char far *tcpptr;

  short buf;
  short size;
  while (readwd(COMMANDR)&4)
  {
    card_busyxmt++;
    Relinquish(0L);
  }
  tcpptr=ether_ptr;

  movmem(Edest,tcpptr,6);                   /*ETHER*/
  tcpptr+=6;
  movmem(MY_ether,tcpptr,6);
  tcpptr+=6;
  *(short far *)tcpptr=type;
  tcpptr+=2;
  for (buf=0;buf<buffers;buf++)
  {
    movmem(bufs[buf].data,
           tcpptr,
           bufs[buf].length);
    tcpptr+=bufs[buf].length;
  }
  writewd(TSTARTW,0);
  size=tcpptr-ether_ptr;
  if (size<64)
    size=64;
  writewd(TCNTHW,(size)>>8);
  writewd(TCNTLW,(size)&0xff);

  card_collisions +=readwd(COLCNTR);
  card_allignment +=readwd(ALICNTR);
  card_crcerrors  +=readwd(CRCCNTR);
  card_missedcount+=readwd( MPCNTR);
  writewd(COMMANDW,4);
  card_packets_sent++;
}



short readWD(windowptr output,char far * far *buffer)
{
  /* This procedure goes out the the WD8013 card and checks to see
     if there are any datapackets availiable.  If there are, then it
     returns then length of the packet, and fills in the address
     of the packet into the **buffer variable.  If the card did not
     have memory of it's own, then this routine could allocate
     a buffer, and return the data in the buffer it allocated. It also
     stores the blocks that have been read from the card, so that the
     release procedure may account for every block.  There are a limited
     number of blocks that may be outstanding at this point, so we must
     wait to see if there is an availiable block.*/
  char i,trys_left=3;
  Ether_header far *etherhead;
  char far *tempbuf;
  short curr,from;
try_again:
  from=readwd(BOUNDR);
  if (from==(curr=readwd(CURRR)))
  {
    return(0);
  }

  if (from<start_rec||curr<start_rec)
  {
    if (trys_left)
    {
      trys_left--;
      goto try_again;
    }
    displayln(output,"PANIC!PANIC!PANIC!");
    if (!initWD(output,IO_base))
      perish();
    return(0);
  }
  etherhead=(Ether_header far*)((long)ether_ptr|((short)from<<8));

  if (etherhead->next<from&&etherhead->next!=start_rec)
  {
    short first_length=(end_rec-from)<<8;
    tempbuf=Allocate(etherhead->count);
    movmem(tempbuf,
            etherhead->data,
           (end_rec-from)<<8);
    movmem(tempbuf+first_length,
           ether_ptr+(start_rec<<8),
           etherhead->count-first_length);
    *buffer=tempbuf;
    writewd(BOUNDW,etherhead->next);
  }
  else
    *buffer=etherhead->data;

  card_received++;
  return(etherhead->count);
}

void releaseWD(windowptr output,char far *buffer)
{
  /* this routine frees the "ethermemory" that was returned from a readWD.
     this routine checks the block to see if it was actually returned from
     a WD read so that we don't remove something that isn't ours.  This
     was provided for those controllers that might not have their own memories.
     and a mechanism for releaseing buffers was provided.*/
  Ether_header far *etherhead;
  if (((long)buffer&0xffff0000)==(long)ether_ptr)
  {
    etherhead=(Ether_header far *)(buffer-sizeof(etherhead));
    if (etherhead->next<start_rec)
    {
      displayln(output,"TROUBLED WATERS AHEAD!");
      if (!initWD(output,IO_base))
        perish();
      return;
    }
    writewd(BOUNDW,etherhead->next);
  }
  else
  {
    Free(buffer);
  }
}





