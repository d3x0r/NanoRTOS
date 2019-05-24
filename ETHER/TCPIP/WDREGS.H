#define MSR        0x00     /*Memory select Register*/
#define ICR        0x01     /*Interface Configuration Register*/
#define IAR        0x02     /*I/O Address Register*/
#define EAR        0x03     /*Bios Rom Address Register*/
#define BIO        0x03     /*EEROM Address Register*/
#define IRR        0x04     /*Interrupt Requst Register*/
#define LAAR       0x05     /*LA Address Register*/
#define IJR        0x06     /*Initialize Jumper Register*/
#define GP2        0x07     /*General Purpose Register 2*/
#define LAR1       0x08     /*Lan Address Registers*/
#define LAR2       0x09
#define LAR3       0x0A
#define LAR4       0x0B
#define LAR5       0x0C
#define LAR6       0x0D
#define LAR7       0x0E
#define LAR8       0x0F
#define ALICNTR    0x0D    /*Alignment Error Counter*/
#define COLCNTR    0X05 /*Collision counters-cleared at start of transmission*/
#define CRCCNTR    0X0E    /*CRC error counter*/
#define MPCNTR     0X0F    /*Missed packet error count*/
#define TESTR      0X31    /*test enables- do not write*/
#define TESTW      0X31
#define ENHR       0X27    /*Enhanced features- unique to WD83c690*/
#define ENHW       0X27
#define COMMANDR   0X00    /*Command Register*/
#define COMMANDW   0X00
#define DCONR      0X2E    /*Data config register - for memory*/
#define DCONW      0X0E
#define RCONR      0X2C    /*Receive configuration*/
#define RCONW      0X0C
#define TCONR      0X2D    /*Transmit configuration data*/
#define TCONW      0X0D
#define INTMASKR   0X2F    /*IRQ Mask*/
#define INTMASKW   0X0F
#define INTSTATUSR 0X07    /*IRQ Status - pending interrupts*/
#define INTSTATUSW 0X07
#define BLOCKR     0X26    /*Upper 8 bits of Memory address         A23-16*/
#define BLOCKW     0X26
#define BOUNDR     0X03    /*Pointer to the oldes used receive buffer  A15-8*/
#define BOUNDW     0X03
#define CURRR      0X17 /*Pointer to first buffer to store current frame A15-8*/
#define CURRW      0X17
#define NEXTR      0X25  /*Pointer to next buffer to be opened by TRDMA A15-8*/
#define NEXTW      0X25
#define RSTARTR    0X21    /*Pointer to start of receive buffer ring*/
#define RSTARTW    0X01
#define RSTOPR     0X22    /*Pointer to end of ring buffer*/
#define RSTOPW     0X02
#define RSTATR     0X0C    /*Receive status register */
#define STA0R      0X11    /*My address registers*/
#define STA0W      0X11
#define STA1R      0X12
#define STA1W      0X12
#define STA2R      0X13
#define STA2W      0X13
#define STA3R      0X14
#define STA3W      0X14
#define STA4R      0X15
#define STA4W      0X15
#define STA5R      0X16
#define STA5W      0X16
#define TSTARTR    0X24    /*Pointer to packet to transfer*/
#define TSTARTW    0X04
#define TCNTHW     0X06
#define TCNTLW     0X05
#define TRINCRHR   0X02    /*transfer increment registers.  */
#define TRINCRHW   0X22
#define TRINCRLR   0X01
#define TRINCRLW   0X21
#define TSTATR     0X04    /*Status bits that occured while transmitting.*/

#define RPROM  0x10
#define RGROUP 0x8
#define RBROAD 0x4
#define RUNTS  0x2
#define MENB   0x40
typedef struct Ether_header    /*Head specific to WD cards, contains info
                                  about this packet. IN PC ORDER!!!!*/
{
  unsigned char status;
  unsigned char next;
  short count;
  char data[];
}Ether_header;

