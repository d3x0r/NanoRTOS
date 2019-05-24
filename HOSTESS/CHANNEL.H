typedef struct channel
{

  char Status;
#define ChanStatOpen 1      /*bit that indicates that a channel open is in
                              progress*/
#define ChanStatEthr 0b0010  /*mask that indicates Ether channel configuration*/
#define ChanStatHost 0b0100  /*mask that indicates Hostess channel configuration*/
#define ChanStatSasy 0b0110  /*mask that indicates standard asynch config.*/
#define ChanStatPara 0b1000  /*mask that indicates parallel configuration*/

  char CurrentOp;
  char TCL_opts;
  char TCL_idx;
  void far *device_options;
  void far *line_struct;
}
channel;
