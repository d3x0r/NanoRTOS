#define RTS 2
#define DTR 1
#define DCD 0x80
#define DSR 0x20
#define CTS 0x10

typedef struct buffer
{
  short Head,Tail,Size;
  char far *Buffer;
} buffer;


typedef struct com_socket
{
  buffer In,Out;
  short IOPort;
  unsigned short Baud_Divisor;
  char Line_Control;
  char Modem_Control;
  char Line_Status;
  char Modem_Status;
  char control;
  struct com_socket far *next;
} com_socket;

#ifndef COMM
#pragma warn -rvl
#pragma argsused
dynamic(short,comopen,
       (short port,short baud,char bits,char stop,char parity,
        short insize,short outsize,char irq,char opts,
        void far *returnsocket));
#pragma argsused
dynamic(short,comread,(com_socket far *socket,char far *buffer,short maxlen));
#pragma argsused
dynamic(short,comwrite,(com_socket far *socket,char far *buffer,short length));
#pragma argsused
dynamic(short,comsetmodem,(com_socket far *socket,short params));
#endif

#define COM_PARAMS 8
#define SOCKET_OK    0
#define IO_ALLOCATED 1
#define IRQ_INVALID  2
#define PARITY_INVALID 3
#define BITS_INVALID 4
#define STOP_INVALID 5


