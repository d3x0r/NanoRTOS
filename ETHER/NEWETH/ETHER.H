
/*options for the open call*/
#define RECORD_SOC       1      /*record orientated socket*/
#define QUICK_IO      0x40      /*read and write return as soon as
                                  possible...returning how much they
                                  have processed.  They do not relinquish
                                  and hold on. */


#define CLOSING          1
#define READ_TERM        2
#define ENDRECORD        8
#define WRITE_TERM    0x10
#define SERVER_SOCKET 0x20
#define QUICK_IO      0x40
#define RECORD_SOCKET 0x80

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

typedef struct Ether_header    /*Head specific to WD cards, contains info
                                  about this packet. IN PC ORDER!!!!*/
{
  unsigned char status;
  unsigned char next;
  short count;
}Ether_header;

typedef struct Ether_layer
{
  Ether_addr dest;
  Ether_addr source;
  short type;
} Ether_layer;

typedef struct IP_layer
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


typedef struct TCP_layer
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
}TCP_layer;

typedef struct ARP_layer
{
  short hardware,protocol;
  char hardlen;
  char protlen;
  short opcode;
  Ether_addr source_hard_addr;
  IP_addr    source_prot_addr;
  Ether_addr target_hard_addr;
  IP_addr    target_prot_addr;
} ARP_layer;

typedef struct ARP_prot
{
  Ether_layer M;
  ARP_layer A;
} ARP_prot;

typedef struct TCP_prot
{
  Ether_layer M;
  IP_layer I;
  TCP_layer T;
  unsigned char data[];
} TCP_prot;

typedef struct IP_prot
{
    Ether_layer M;
    IP_layer I;
    unsigned char data[];
} IP_prot;

typedef struct record
{
  short base;      /*base index of the record*/
  short length;    /*length of the buffer*/
  char flags;      /*options involved with the record*/
}record;

typedef struct seq_data
{
        short ident;
        unsigned long seq;
        short length;
        char flags;
        char sent;
        unsigned char times_sent;     /*added 6/11/93 to store retrans.
                                        to detect bad connections*/
        short time_sent;
        char far *data;
        struct seq_data far *next;
} seq_data;

#define SENT            1
#define SENDING         2
#define SENTAGAIN       4
#define RELEASING       8

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
  short ihead,itail;     /*head and tail for in buffer*/
  short base,hend;       /*base is the first place to store data, not
                            neccesarily equal to the head. hend is the
                            end of the hole*/

  char status;           /*status bits -
                                0x80   record socket
                                0x40   quick return IO
                                0x20   Server Socket
                                0x10   stop write NOW
                                   8   EOR
                                   4   Broken EOR
                                   2   stop read NOW
                                   1   Closing*/

  char flags;             /*outstanding operations to put on outgoing packets,
                           If there are any*/
  char retransmitting;

  short last_packet_sent; /*time at last transmition        */
  short Dwindow;          /*his estimated/real window size  */
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

  short cur_rec,first_rec,last_rec;  /*Current record, updated by putting
                                       a new recrod into the buffer.  First
                                       record is an index to the first
                                       untaken record, updated by reading
                                       records.  last_rec is to control the
                                       wrapping of first/curent */
  record far *rec_list;
  struct connection far *next;
  unsigned long connect_timer;
  seq_data far *outstanding_recs;  /* This is a list of all outstanding
                                      work to be done on this socket  */
  unsigned long  highest_seq;      /*This is the highest seq number that
                                     was computed.  This is the first
                                     byte of the current block */
  unsigned long lowest_not_sent_seq;

  short current_ident;             /*This is the highest ident that was
                                     computed.  This is the next ident number
                                     to be used.*/
  short packets;                   /*this is the number of outstanding packets
                                     on the socket*/
  short mult_packets;              /*this is the number of times that multiple
                                     packets were stacked on a socket for
                                     output.*/
  unsigned char users;             /*this is the number of people currently
                                     doing reads or writes to this socket added
                                     6/11/93*/
  unsigned short bytes_outstanding;/*this is how much of the write was out
                                     standing when the last write to the
                                     socket was called.  The amount transfered
                                     is deducted from this each time a
                                     subsequent write is called.  When this
                                     is zero, it will start a new record,
                                     while it isn't it ignores the buffer
                                     and count passed.  this is the only way
                                     that an accurate amount of how much
                                     was sent can be returned.  Sorry Design
                                     flaw.    8/24/93*/
  seq_data far *last_write;        /*this saves the pointer to the last block
                                     that should have been transmitted.*/
} connection;




#ifndef ETHER
#pragma warn -rvl
#pragma argsused
dynamic( connection far *,openether,
       (char far *source_address,char far *dest_addr,
        short tsouce,short tdest,char opts));
#pragma argsused
dynamic( unsigned short,sendether,
         (char far *buffer,unsigned short length,connection far * ether));
#pragma argsused
dynamic( unsigned short,readether,
      (char far *buffer,unsigned short maxlength,connection far * ether));
#pragma argsused
dynamic( short,close_connection,(connection far * ether));  /*return value changed 6/11/93*/
#pragma argsused
dynamic( void,Ether_term,(connection far * ether,short read));
#pragma argsused
dynamic( void,etherflush,(connection far * ether));
#pragma warn .rvl
#endif
