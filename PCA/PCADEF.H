#define NORM_BUF(buffer) (((short)(buffer))+((((long)(buffer))&0xffff0000)>>12))
#define PTR_BUF(buf) (((((long)(buf))&0xf0000)<<12)+((unsigned short)(buf)))

#define false 0
#define true (!false)
#define READ_MEM false
#define WRITE_MEM true

#define DEBUG_DEFAULT false

#define DMAIL_DEFAULT true
#define DWORK_DEFAULT false

#define OUT_CHANNEL 0x00
#define OUT_TERM    0x02
#define OUT_TI      0x04
#define OUT_SELECT  0x06
#define IN_CHANNEL  0x08
#define IN_TERM     0x0a
#define IN_TI       0x0c
#define IN_SELECT   0x0e
#define INTR_QUEUE  0x10
#define DATA        0x12
#define CONTROL     0x14
#define STATUS      0x16
#define CONFIGURATION 0x1A
#define MAIL_ADDRESS 0x1C
#define MAIL_DATA    0x1E
#define MAIL_MAX     0x40
#define Mod_mail_in  0x00
#define Mod_mail_out 0x80
#define PC_mail_in   0x40
#define PC_mail_out  0xC0
    /*bits   -
      xxxx xxx1 08 Even Channel TC
      xxxx xx1x 80 Odd  Channel TC
      xxxx x1xx 04 Even Channel Select
      xxxx 1xxx 40 Odd  Channel Select
      xxx1 xxxx 01 Even Channel Trans Init
      xx1x xxxx 10 Odd  Channel Trans Init
      x1xx xxxx 02 Even Channel Terminate
      1xxx xxxx 20 Odd  Channel Terminate
    */
#define TRMDOG   25

#define EVENTC   0x0003
#define ODDTC    0x000c
#define EVENSEL  0x0030
#define ODDSEL   0x00c0
#define EVENTI   0x0300
#define ODDTI    0x0c00
#define EVENTERM 0x3000
#define ODDTERM  0xc000
#define INT_EVENTC   0x08
#define INT_ODDTC    0x80
#define INT_EVENSEL  0x04
#define INT_ODDSEL   0x40
#define INT_EVENTI   0x01
#define INT_ODDTI    0x10
#define INT_EVENTERM 0x02
#define INT_ODDTERM  0x20
#define POS_EVENTC   0x00
#define POS_ODDTC    0x02
#define POS_EVENSEL  0x04
#define POS_ODDSEL   0x06
#define POS_EVENTI   0x08
#define POS_ODDTI    0x0a
#define POS_EVENTERM 0x0c
#define POS_ODDTERM  0x0e
#define QUEUE_EVENTC   0
#define QUEUE_ODDTC    1
#define QUEUE_EVENSEL  2
#define QUEUE_ODDSEL   3
#define QUEUE_EVENTI   4
#define QUEUE_ODDTI    5
#define QUEUE_EVENTERM 6
#define QUEUE_ODDTERM  7
#define EVENBUSY 1
#define ODDBUSY  2
#define EXTERNAL_ODD  0x40
#define EXTERNAL_EVEN 0x80

#define ABRT_TRM   1
#define HAND_BSY   2

#define EVEN_FUL   0x0001
#define CARD_ARMD  0x0002
#define EVEN_BSY   0x0004
#define EVEN_XSI   0x0008
#define EVEN_ISI   0x0010
#define EVEN_MPE   0x0020
#define EVEN_N     0x0040
#define EVEN_ERR   0x0080

#define ODD_EMPT   0x0100
#define MCCLK      0x0200
#define ODD_BSY    0x0400
#define ODD_XSI    0x0800
#define ODD_ISI    0x1000
#define ODD_MPE    0x2000
#define ODD_N      0x4000
#define ODD_ERR    0x8000



#define EVENERROR 1
#define ODDERROR 2
#define DISARMERROR 4
#define EMPTYINPUT 1

#define MAX_DEVICES 256

