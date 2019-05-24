#define opendisplay
#define moddisplay
#define kbhit

#include "mod.h"
#pragma argsused
dynamic( void,plot,(windowptr window,short x,short y,char color));
#pragma argsused
dynamic( void,line,(windowptr window,short x1,short y1,
                                     short x2,short y2,char color));

windowptr output;


main()
{
  char i,j;
  asm int 3;
  output=opendisplay(10,110,50,50,0,9,0,0,0);
  do
    for (j=0;j<100;j++)
      for (i=0;i<50;i++)
      {
/*      moddisplay(output,1,10+i<<1,10+i<<1,0);*/
        line(output,0,i,50,i,j+i);
      }
  while(!kbhit(output));
  line(output,0,50,50,0,14);
  Exit(10);
}


