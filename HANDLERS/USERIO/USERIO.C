
/* This is another solution to the undening problem of buffer data
   translation.
   putch(getcharroutine(Node));
*/



/*this is a generic handler for all userio systems-
   Printer
   Asynch
   Ether Terminals
   etc.

This program is mostly configuration file driven.  It will act as
many handlers.  It determines configuation passing based upon the
environment paramters given to it.

userio
device1=COMM
COMMchannels=16 index
COMMopen=
COMMwrite=buffer length index
COMMread=buffer length index
COMMflush=index
COMMdiscon=index

device2=Ether
ETHERchannels=16 pointer
ETHERwrite=pointer buffer length
ETHERread=pointer buffer length
ETHERflush=pointer
ETHERdiscon=pointer

device3=Display
displaychannels=16
displaywrite=

*******
  Need to define Ring/Break packets

*/


