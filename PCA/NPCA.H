/*

Internal Node tracking data

  Data Buffer
  Side   - 0 if even modcomp device, 1 if odd modcomp device.
  status - sloughed, etc
  next   - chain of nodes
  prior  - chain of nodes
  source - name of PCA card driver.

Outgoing mail data
  Rsize                short - size of mail to be sent-if 0 will be default min.
  Rstatus              short - return status
  RXstatus             short - Return xtended status
  RByte_count          unsigned long - return byte count
  Return_operation     byte - return operation on the node

Incoming Mail- Current

  Size        unsigned short    11 - words in mail- not stored
  Transport   unsigned short
  Channel     unsigned short
  Node        unsigned long
  Rex         unsigned short
  Options     unsigned short
  XOptions    unsigned short
  Byte Count  unsigned long
  FPI         unsigned long
  union
  {
    Etherdata Ether;
    Commdata Comm;
    Diskdata Disk;
  } Info;



Current Mail Format
  1S    1S   1S   2S   1S  1S  2S   1S   2S  x   - words
ÚÄÄÄÄÂÄÄÄÄÄÂÄÄÄÄÂÄÄÄÄÂÄÄÄÂÄÄÄÂÄÄÄÄÂÄÄÄÄÂÄÄÄÂÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
³Size³Trans³Chan³Node³Rex³Opt³BCnt³Xopt³FPI³Misc                      ³
ÀÄÄÄÄÁÄÄÄÄÄÁÄÄÄÄÁÄÄÄÄÁÄÄÄÁÄÄÄÁÄÄÄÄÁÄÄÄÄÁÄÄÄÁÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ

  1S    .5,.5   2S   1S   2S     1S   2S  x  - words
ÚÄÄÄÄÂÄÄÄÄÄÄÄÄÂÄÄÄÄÂÄÄÄÄÂÄÄÄÄÄÂÄÄÄÄÄÂÄÄÄÂÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
³Size³Rop /Rex³Node³Stat³Rbyte³Xstat³FPI³MISC                         ³
ÀÄÄÄÄÁÄÄÄÄÄÄÄÄÁÄÄÄÄÁÄÄÄÄÁÄÄÄÄÄÁÄÄÄÄÄÁÄÄÄÁÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ
  Note when sending back Returnop/Rex Upper half of reg must be
  Return op so that the modcomp sees it in the lower half of its
  register.*/

/*Structure Definitions*/




typedef struct Data_Track
{
  short offset;  /* start of data to send with mail in words*/
  short filler;
  short length;  /* length of data to send with mail in words */
}Data_Track;

typedef struct Etherdata
{
  unsigned char Control;     /*4/1/1/2(unused)*/
  unsigned char Break_char;
  unsigned char SourceIP[4];
  unsigned char DestIP[4];
  unsigned short SourceTCP;
  unsigned short DestTCP;
  unsigned short Top_of_form;
  unsigned char TCL_data;
  unsigned char TCL_index;

}Etherdata;

typedef struct Commdata
{
/*     control bits
          S H S H    s=software x-on/x-ff   H=hardware signale
          2 2 2 2    2= to
          H H T T    T= Terminal  H=Host
         ÚÄÂÄÂÄÂÄÂÄÄÄÂÄÄÄÂÄÄÄÄÂÄÄÄÄ¿
         ³ flow  ³CFF³NEK³ Avail   ³
         ÀÄÁÄÁÄÁÄÁÄÄÄÁÄÄÄÁÄÄÄÄÁÄÄÄÄÙ */
  unsigned char Control;
  unsigned char Baud;
  unsigned char Frame_Stop_Par;   /*2/2/3/1 (unused)*/
  unsigned char fill1;
  unsigned char Mode;             /*4/4 currentloop 0 /rs232 1/rs422 2*/
  unsigned char Break_char;
  unsigned short fill2;
  unsigned short Top_of_form;     /*lines per page*/

/*
 bits-tcl data
 ÚÄÄÄÂÄÄÂÄÄÄÄÄÄÂÄÄÄÂÄÄÄÂÄÄÄÄÄÄÄÂÄÄÂÄÄ¿
 ³avail ³break ³Rng³Con³Discon ³avail³
 ÀÄÄÄÁÄÄÁÄÄÄÄÄÄÁÄÄÄÁÄÄÄÁÄÄÄÄÄÄÄÁÄÄÁÄÄÙ
*/

  unsigned char TCL_data;
  unsigned char TCL_index;
}Commdata;

typedef struct NodeTrack
{
  char Side;
  long start_time;
  unsigned short Status;
  char far *Source;
  struct Node far *next,far *prior;
}NodeTrack;

typedef struct BreakReturn
{
  char Opcode;
  unsigned char TCL_data;
  unsigned char TCL_idx;
  unsigned short Transport;
  unsigned short SubChannel;
} BreakReturn;

typedef struct ReturnData
{
  char  Opcode;
  unsigned short Status;
  unsigned long  Byte_count;
  unsigned short XStatus;
  unsigned short Extended_Size;
  unsigned short far *Extended_mail;
}ReturnData;

typedef struct NodeData
{
  unsigned short Transport;
  unsigned short Channel;
  unsigned long  Node_Address;
  unsigned short Rex;
  unsigned short Options;
  unsigned long  Byte_count;
  unsigned short XOptions;
  unsigned long  FPI;
  union
  {
    Etherdata Ether;
    Commdata Comm;
  } Info;
}NodeData;

typedef struct Node
{
  ReturnData Return;
  NodeTrack Tracking;
  unsigned short far *Data;
  NodeData Node;
  Data_Track Data_handle;
}Node;

typedef struct Node_holder
{
  Node far *Node;
  struct Node_holder far *next;
} Node_holder;

/*Values for break packet events */
#define BREAK_EVENT 0x20
#define RING_EVENT  0x10
#define CONNECT_EVENT 0x8
#define DISCON_EVENT 0x4

/*Node far *packet;
packet->Tracking.Side=0;
packet->Return.Byte_count=packet->Node.Byte_count;
packet->Node.Info.Ether.SourceTCP;
*/

/*Defined Values*/

/*Return Operation*/
#define SLOUGH     0
#define COMPLETE   1
#define TRICKLE    2
#define TRICKCOMPL 3
#define BREAK      4

/*Tracking Status*/
#define SLOUGHED         0x01   /*packet was sloughed and needs no responce*/
#define USER_BUFFER      0x02   /*Buffer to be alocated by end user, no buffer
                                  is supplied with work packet, do not
                                  deallocate the buffer when x-fer is done.*/
#define WORK_ACK         0x04   /*Work was returned as complete?*/
#define WORK_IN_MAIL     0x08   /*Work packet is in the mail on its way out*/
#define TERM_WHEN_DONE   0x10   /*Generate External SI when DATA x-fer complete,
                                  instruction given by handler*/
#define SAVE_WHEN_DONE   0x20   /*Do not deallocate the work packet becuase
                                  the handler is not yet done with it, so
                                  just forget about it.*/
#define WORK_IN_PROGRESS 0x40   /*work is waiting for the external SI to
                                  disappear.*/
#define RETRANS          0x80   /*work is a retransmitted packet*/

/*Tracking Side*/
#define EVEN 0
#define ODD 1

/*Node Rex */

#define READ            0
#define WRITE           1
#define REWIND          2
#define BACK_FILE       3
#define BACK_RECORD     4
#define ADVANCE_RECORD  5
#define ADVANCE_FILE    6
#define WRITE_EOF       7
#define HOME            8
#define TERMINATE       9

#define OIC        0x2000        /* operation is data chain I/O (Node Rex)*/
#define QRC        0x0080        /* operation is quick return   (Node Rex)*/

#define UFTOPTSR  0x8000   /*system error recovery*/
#define UFTOPTDDO 0x4000   /*device dependant*/
#define UFTOPTSTD 0x2000   /*standard*/
#define UFTOPTBI  0x1000   /*binary*/
#define UFTOPTAUG 0x0800   /*MAX IV IO Op*/
#define UFTOPTRAN 0x0400   /*Random Position*/
#define UFTOPTTBX 0x0200   /*Inhibit Trailing blank trunc,
                             EOF detect, add of NULL terminator*/
#define UFTOPTRNQ 0x0100   /*Return if not queued*/
#define UFTOPTNT  0x0080   /*No terminator byte*/
#define UFTOPTTRM 0x007f   /*terminator byte from UFT */

#define UFTXOPFLU 0x0010   /*flush */
#define UFTXOPDCL 0x0008   /*disconnect Line*/
#define UFTXOPNEC 0x0002   /*Inhibit read echo*/

#define UFTXOPQR  0x8000
#define UFTXOPBBR 0x4000
#define UFTXOPOTF 0x2000
#define UFTXOPIUS 0x1000
#define UFTXOPUPP 0x0800
#define UFTXOPUST 0x0400
#define UFTXOPUDW 0x0200

#define UFTSTAER  0x8000   /*Error*/
#define UFTSTAOVF 0x4000   /*Input Channel overrun*/
#define UFTSTAPAR 0x2000   /*Input channel parity error*/
#define UFTSTAINO 0x1000   /*Device Inop/DNE/Watchdogged*/
#define UFTSTAMPE 0x0800   /*DMP mapping error */
#define UFTSTALOC 0x0400   /*modem malfunctoin-DSR/DCD dropped input,
                                               DSR/CTS dropped output */
                           /*write protect TAPE ect. */

#define UFTSTACIG 0x0200   /*Out- Ring, In- Frame error */
#define UFTSTAOTH 0x0100   /*Can coded reason UFTBCT*/
#define UFTSTASBV 0x0080   /*Standard binary buffer format violation*/
#define UFTSTAEOM 0x0040   /*End of media*/
#define UFTSTAEOF 0x0020   /*End of file*/
#define UFTSTABOM 0x0010   /*Begin of media*/
#define UFTSTANQ  0x0008   /*set RNQ type error*/
#define UFTSTAHOL 0x0004   /*first byte received or device in HOLD condition*/
#define UFTSTABB  0x0002   /*buffer busy*/
#define UFTSTAUB  0x0001   /*UFT busy*/
#define UFTSTASNO 0x0030   /*unknown operations (snow)*/



#ifndef PCAROOT
#pragma argsused
dynamic(Node far *,Create_node,(char side));
#endif

