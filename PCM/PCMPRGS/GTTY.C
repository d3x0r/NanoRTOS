#include <mod.h>
#include <comm.h>
#include <video.h>

#define false 0
#define true (!false)
com_socket far *input;
windowptr output;
char done=false;
char idx,_idx;
char ch;
char line[256];

#define REGISTER 0
#define REMOVE   1

typedef char (far *validation_routine)(char charac);

typedef struct form
{
  short head,tail,size;
  char far *buffer;
  validation_routine valid;
}form;

typedef struct package
{
  char operation;
  form far *invoice;
}package;

package parcel;
form invoice;

char far Xyz(char charac);
char far XyZ(char charac);

char far XyZ(char charac)
{
  invoice.valid=Xyz;
  return(true);
}

char far Xyz(char charac)
{
  invoice.valid=XyZ;
  return(false);
}


main()

{
  short comstat;
  char len,show;
  long stime;
  long curtime;

  parcel.operation=REGISTER;
  parcel.invoice=&invoice;
  invoice.head=invoice.tail=0;
  invoice.size=0;
  invoice.buffer=0;
  invoice.valid=Xyz;
  Export("Dispatch",&parcel);
  Relinquish(1L);
}

