
#define CONNECT_DELAY 1000
#define REQUEST 0x100
#define REPLY 0x200

#define CLOSING          1
#define SERVER_SOCKET 0x20
#define SERVER_INIT   0x10

#define SNARP    0x01
#define SNSYN    0x02
#define SWSYN    0x04
#define SRESET   0x08
#define SCLOSED  0x10
#define DLYWSYN  0x20

typedef unsigned char IP_addr[4];
typedef unsigned char Ether_addr[6];

#define telnet 23
#define IP_type 0x008
#define ARP_type 0x608
#define TURG   0x20
#define TACK   0x10
#define TPUSH  0x08
#define TRESET 0x04
#define TSYN   0x02
#define TFIN   0x01

#define TRANS_SIZE 10
#define RECV_SIZE 0x40

typedef struct buftype
{
  void far *data;
  short length;
}buftype;

typedef struct Ether_layer    /*Ethernet header, is present on all ethernet
                                devices. */
{
  Ether_addr dest;
  Ether_addr source;
  short type;
  char data[];
} Ether_layer;

typedef struct IP_layer      /*Definition for IP header */
{
  unsigned char version_len; /*  Word is masked as VVVV LLLL where
                                 V is the Version  (must be 4)
                                 L is the length of the header in 4-octets
                                      (5)  */
  unsigned char service;     /*  AKA TOS*/
  unsigned short length;     /*  (Must byte swap) total length of packet*/
  unsigned short ident;      /*  (must byte swap)
                                 used as a sequence number-  This number
                                 is used to recontruct fragments from a
                                 given source.  But, if multiple messages
                                 are sent from a source, then this dis-
                                 tinquishes one from another*/

  unsigned short frags;      /*  Word is masked as FFFF OOOO OOOO OOOO where
                                 F are flags
                                 O is offset of fragment.
                                 If MF or Offset set then datagram must be
                                 linked with siblings*/
  unsigned char time_to_live;/*  used to squelch echoing datagrams*/
  unsigned char protocol;    /*  100? */
  short checksum;            /*  checksum of header.  Add all words then ~*/
  IP_addr source;            /*  if transmitting, this is me.
                                 if receiving, this is where it was from.*/
  IP_addr dest;              /*  if transmitting, this is where it is going.
                                 if receiving, this is me.*/
  char data[];
} IP_layer;


/*The following header is prefixed to a TCP packet for purposed of
  Checksuming plus 1 long of NULS. Zero is a zero, and prot=6(TCP)*/

typedef struct Pseudo_IP
{
  IP_addr source;
  IP_addr dest;
  unsigned char zero;
  unsigned char prot;
  unsigned short length;
  long zeros;
} Pseudo_IP;


typedef struct TCP_layer     /* structure for TCP header */
{
  short source;              /*  (byte swap)
                                 if transmitting, this is me.
                                 if receiving, this is where it was from.*/
  short dest;                /*  (byte swap)
                                 if transmitting, this is where it is going
                                 if receiving, this is me.*/
  unsigned long seq;         /*  (byte swap)*/
  unsigned long ack;         /*  (byte swap)*/
  char hlen;                 /*  masked as HHHH RRRR
                                 H=Header Length
                                 R=Reserved
                                 header length give in 4-octet units */
  char control;              /*  control flags*/
  short window;              /*  number of bytes availiable to receive */
  short checksum;            /*  All words added and notted like IP*/
  short urgent;
  char data[];
}TCP_layer;

typedef struct ARP_layer   /* structure an ARP request/responce*/
{
  short hardware,protocol;      /* hardware is 0x100, prot is 0x200 */
  char hardlen;                 /* hardware address length = 6*/
  char protlen;                 /* protocol address length = 4*/
  short opcode;                 /* Operation, 0x100=request, 0x200= responce,
                                              0x300=RARP request, 0x400= RARP responce*/
  Ether_addr source_hard_addr;  /* address of the source of the request*/
  IP_addr    source_prot_addr;  /* address of the source of the request*/
  Ether_addr target_hard_addr;  /* address of the targer of the reqeust*/
  IP_addr    target_prot_addr;  /* address of the target of the request*/
} ARP_layer;

typedef struct ARP_prot     /* Arp protocol, built on ether */
{
  Ether_layer M;
  ARP_layer A;
} ARP_prot;

typedef struct TCP_prot    /* TCP protocol structure, build on ether and IP,
                              and indicates where the data begins in the
                              packet*/
{
  Ether_layer M;
  IP_layer I;
  TCP_layer T;
} TCP_prot;

typedef struct IP_prot     /* IP protocol structure, build on ether, and
                              indicates where the data begins in the packet*/
{
    Ether_layer M;
    IP_layer I;
} IP_prot;

typedef struct receive_block
{
  char far *buf_begin;    /*beginning of the data in the receive_buffer*/
  long begin_sequence;    /*the beginning sequence number of this packet*/
  short length;           /*length of this block of data*/
}receive_block;

typedef struct connection
{
  Ether_addr Edest;      /*Etherneat Address of destination*/

  char state;            /*State of connection.
                               xxxx xxx1  need to arp
                               xxxx xx1x  need to syn
                               xxxx x1xx  need to ack
                               xxxx 1xxx  was reset
                               xxx1 xxxx  was closed */

  IP_layer I;            /*IP data holder.  Since much of this doesn't
                           change, I keep hold of it forever*/
  TCP_layer T;           /*Same as above*/
  Pseudo_IP PI;          /*MORE same as above*/

  char far *inbuf;       /*buffer space for incoming transactions*/
  short inhead,intail,insize;

  receive_block
     far *blocks;        /*structure to manage received blocks of data.
                           used to hold transactions that were received
                           before interveneing blocks arrived.  Used
                           to say which data in the buffer is valid.*/
  short last_block;      /*last block of received data.*/

  char far *outbuf;      /*buffer space for outgoing transactions*/
  short outhead,         /*head of the output buffer to put data at.*/
        outtail,         /*tail of the buffer to take data from*/
        outbound,        /*last position of the buffer having been sent*/
        outmaxbnd,       /*maximum boundry that I have sent so far...*/
        outsize;         /*size of the buffer*/
  unsigned long tail_seq;/*sequence number of the tail pointer */

  char status;           /*status bits -
                                0x40   quick return IO
                                0x20   Server Socket
                                   1   Closing*/

  char flags;             /*outstanding operations to put on outgoing packets,
                           If there are any*/

  short last_packet_sent; /*time at last transmition        */
  short Dwindow;          /*his estimated/real window size  */
  short MaxDwindow;       /*his total window size... updated whenever
                            he advertises a window larger than this value.*/
  short last_ad;          /*our last advertized window size */

#define  dwind_weight 90
#define dw_var_weight 90

  unsigned short smth_dwind;       /* approx 128 sample average  */
  unsigned short dwind_var;         /*window statistics variable */

#define srtt_weight 85
#define sdev_weight 90

  unsigned short srtt,sdev,rto;  /* approx .1 second response */
  unsigned short a_size;         /* response time statistics  */

  unsigned short p_received      /* TCP LAYER STATISTICS      */
                ,rec_duplicats
                ,over_flows 
                ,transmits       /* transmition counts */
                ,retransmits;

  struct connection far *next,far *prior;
  unsigned long connect_timer;
  unsigned long last_time_sent;

  short current_ident;             /*This is the highest ident that was
                                     computed.  This is the next ident number
                                     to be used.*/
  unsigned char users;             /*this is the number of people currently
                                     doing reads or writes to this socket added
                                     6/11/93*/
} connection;




#ifndef ETHER
#pragma warn -rvl
#pragma argsused
dynamic( connection far *,openether,
       (char far *source_address,char far *dest_addr,
        short tsouce,short tdest,char opts,short rcv_size,short xmt_size,
        short blocks));
#pragma argsused
dynamic( unsigned short,sendether,
         (char far *buffer,unsigned short length,connection far * ether));
#pragma argsused
dynamic( unsigned short,readether,
      (char far *buffer,unsigned short maxlength,connection far * ether));
#pragma argsused
dynamic( short,closeether,(connection far * ether));  /*return value changed 6/11/93*/
#pragma argsused
dynamic( void,Ether_term,(connection far * ether,short read));
#pragma argsused
dynamic( void,flushether,(connection far * ether));
#pragma warn .rvl
#endif

#define cmpIP(a,b)    (*(long far*)a==*(long far*)b)
#define cmpEther(a,b) ((*(long far*)a==*(long far *)b)&& \
                      (*(((short far*)a)+2)==*(((short far*)b)+2)))
#define moveIP(a,b) { *(long far*)a=*(long far*)b; }

#define moveEther(a,b) { *(long far*)a=*(long far*)b;  \
                         *(((short far*)a)+2)=*(((short far *)b)+2);}
#define intswap(a) (((unsigned)a>>8)|((unsigned)a<<8))

#define longswap(a) ( ((long)intswap((short)a)<<16) |         \
                      (intswap((short)(a>>16))) )

