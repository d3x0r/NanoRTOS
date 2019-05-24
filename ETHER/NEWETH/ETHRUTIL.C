#define extern_def
#define atoi
#include "mod.h"
#include "video.h"
#include "wdregs.h"
#include "ether.h"

extern char inportb();
extern void outportb(short port,char data);

extern unsigned short alloc,dalloc;
#define Free(a) dalloc++;Free(a);
#define Allocate(a) Allocate(a);alloc++;

extern short IO_base;
extern char far *ether_ptr;

extern short max_retrans_delay;

extern char MY_ether[6];

extern void _openether(void);
extern void _sendether(void);
extern void _readether(void);
extern void _disconnect(void);
extern void _close_connection(void);
extern void _Ether_term(void);
extern void _etherflush(void);
extern void Card_int(void);
extern short start_rec,end_rec;

short IRQ_table[8]={ 0xa, 0xb, 0xd, 0xf,0x72,0x73,0x77, 0xc};
short IRQ_NUM  [8]={   2,   3,   5,   7,  10,  11,  15,   4};
short IRQ_port [8]={0x20,0x20,0x20,0x20,0xa0,0xa0,0xa0,0x20};
char reset_port=0x20;
char IRQ_idx=0;
short IRQ_mask [8]={ 0x4, 0x8,0x20,0x80,0x04,0x08,0x80,0x10};

#define moveIP(a,b) { *(long far*)a=*(long far*)b; }
#define moveEther(a,b) { *(long far*)a=*(long far*)b;  \
                         *(((short far*)a)+2)=*(((short far *)b)+2);}

#define intswap(a) (((unsigned)a>>8)|((unsigned)a<<8))

#define longswap(a) ( ((long)intswap((short)a)<<16) |         \
                      (intswap((short)(a>>16))) )

extern unsigned short card_collisions  /* ETHERNET CARD STATISTICS */
                     ,card_allignment
                     ,card_crcerrors
                     ,card_missedcount
                     ,card_send_busy
                     ,card_sent_count
                     ,card_received;

extern window_type far *output;

extern short tcpcheck(short far *pseudo_ptr,
                      short far *tcp_ptr,
                      short length,
                      char far *data,
                      short data_length);

char readwd(char port)
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

char readwdcon(char port)
{
  return(inportb(IO_base+port));
}

void writewdcon(char port,char data)
{  outportb(IO_base+port,data); }

void init_wd(char trans_size,char receive_size)
{
short temp_v;

  /*This routine sets up the initial conditions for the WD1013E card so
  that I can just watch all incoming packets, and display what address they
  are from */

  char t1,t2,irr;
  short Ether_irq;
  short cnt;

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
      perish();
    }
  }

  t1=readwdcon(MSR)&0x3f;
  writewdcon(MSR,t1|MENB);  /*memory | MENB*/

  t2=readwdcon(LAAR)&0x1f;
  writewdcon(LAAR,t2|0xc0);  /*M16EN  L16EN | memory*/

#define near2far(a) (((long)a&0xffff0000L)|(long)((short)a))

  Ether_irq=IRQ_table[temp_v=(readwdcon(ICR)&2)|
                             (((irr=readwdcon(IRR))&0x60)>>5)];
  IRQ_idx=temp_v;
  Connect_Int(Card_int,Ether_irq);
  reset_port=IRQ_port[temp_v];
  outportb(reset_port+1,inportb(reset_port+1)&(~IRQ_mask[temp_v]));
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

  position(output,0,5);

  displayln(output,"PORT:*h IRQ:*d MEM:*H    "
                  ,IO_base,IRQ_NUM[temp_v],(long)ether_ptr);

}

void start_wd()
{
  writewd(COMMANDW,2);
}

void stop_wd()
{
  outportb(reset_port+1,inportb(reset_port+1)|IRQ_mask[IRQ_idx]);
  Disconnect_Int(Card_int,IRQ_table[IRQ_idx]);
  writewd(COMMANDW,1);
  writewdcon(MSR,0x80);
  writewdcon(MSR,0);
}

void send_reset(TCP_prot far *tcpptr)
{
  connection far *temp;
  temp=Allocate(sizeof(connection));

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
  temp->T.window=0;

  temp->T.dest=(tcpptr->T.source);     /*already in the right order!*/
  temp->T.source=(tcpptr->T.dest);     /*already in the right order!*/
  moveIP(temp->I.dest,tcpptr->I.source);
  moveIP(temp->I.source,tcpptr->I.dest);
  moveIP(temp->PI.dest,tcpptr->I.source);
  moveIP(temp->PI.source,tcpptr->I.dest);

  moveEther(temp->Edest,tcpptr->M.source);

  temp->I.length=intswap(sizeof(IP_layer)+sizeof(TCP_layer));
  temp->I.ident=intswap(intswap(temp->I.ident)+1);
  temp->I.checksum=0;
  temp->I.checksum=ipcheck((short far*)&temp->I.version_len);

  temp->T.ack=longswap(temp->T.ack);
  temp->T.seq=longswap(temp->T.seq);
  temp->T.control=TRESET;

  temp->PI.length=intswap(sizeof(TCP_layer));

  temp->T.checksum=0;
  temp->T.checksum=tcpcheck((short far*)&temp->PI,
                            (short far*)&temp->T,
                                sizeof(TCP_layer),
                                0,0);
  while (readwd(COMMANDR)&4)
    {
      Relinquish(0L);
      card_send_busy++;
    }

  send_Ether(temp,0,0);
  Free(temp);
}

void init()
{
   output=opendisplay(20,17,40,8
                    ,NO_CURSOR|BORDER|NEWLINE,0x1f,0x1a,0,"Ethernet");

  IO_base=atoi(Get_environ("Ether_base"));

  max_retrans_delay=atoi(Get_environ("Retrans_delay"));
  init_wd(TRANS_SIZE,RECV_SIZE);
  _disconnect();
  _openether();
  _readether();
  _sendether();
  _close_connection();
  _Ether_term();
  _etherflush();



  start_wd();
}

