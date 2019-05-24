
typedef struct line_entry
{
   char rx_size;
   char stop_par;
   char tx_size;
   short baud;
   char flow;
} line_entry;

typedef struct card_entry
{
  short io_base,line_status;
  unsigned char WR2,WR3,WR4,WR5,WR12,WR13,WR15,RR0;
  short Txq_head,Txq_tail,Txq_offset;
  short Rxq_head,Rxq_tail,Rxq_offset;
  char Flow_control,status,Break_char;
} card_entry;

#define Inbound_soft     0x08
#define Inbound_hard     0x04
#define Inbound_flowed   0x80
#define Outbound_soft    0x02
#define Outbound_hard    0x01
#define Outbound_flowed  0x10
#define Respect_soft     0x20
#define Command_soft     0x40

#define Break_recvd      0x01
#define Flowed           0x02
#define Send_XON         0x04
#define Send_XOFF        0x08

#define BREAK_COND       0x80
#define DCD              0x08
#define CTS              0x20


#pragma argsused
dynamic(short,hiread,(char far *buffer,short length,line_entry far * line));
#pragma argsused
dynamic(short,hiwrite,(char far *buffer,short length,line_entry far * line));
#pragma argsused
dynamic(line_entry far *,hiopen,(short line,line_entry far *message));
#pragma argsused
dynamic(short,hiclose,(line_entry far *line));
#pragma argsused
dynamic(short,higetblk,(char far *buffer,short length,line_entry far * line));


#pragma argsused
dynamic(void,hold_IO,(void));
