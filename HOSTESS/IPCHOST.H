short bauds[]=
{
  -1,3070,2046,1394,1140,1022,510,254,126,83,75,62,41,30,19,14,
  -1,-1,6,-1,2,-1,-1,1,0,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};

/* field definitions for field_set/get from control*/

#define SFLOW_TO_TERM 7,1
#define HFLOW_TO_TERM 6,1
#define SFLOW_TO_HOST 5,1
#define HFLOW_TO_HOST 4,1
#define CAN_FF        3,1
#define SETECHO       2,1

#define BAUD          0,8

/* field definitions for field_set/get from Frame_Stop_Par */

#define FRAME         6,2
#define STOP_BITS     4,2
#define PARITY        1,3

#define MODE          0,4

#define Command_soft 0x40
#define Respect_soft 0x20
#define Break_recvd  0x01

typedef struct line_table
{
  line_entry config;
  char control,FrStPa,Mode;
  short baud;
  char open;
}line_table;

typedef struct break_info
{
        char TCL_avail;
        char TCL_idx;
        short Transport;
        short SubChannel;
        char last_sigs;
        char last_break;
}break_info;


#define UNKNOWN 0
#define CONNECT 1
#define DISCONNECT 2
