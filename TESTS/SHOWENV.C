#include "mod.h"
#include "video.h"

windowptr output;

main()
{
  char far *temp;
  asm int 3;
  temp=Allocate(-100);

  output=opendisplay(1,1,40,8,NO_CURSOR|BORDER|NEWLINE,0x1f,0x1f,0x2f,"Show Environ");

/*  displayln(output,"*s",Get_environ("e0"));
  displayln(output,"*s",Get_environ("e1"));
  displayln(output,"*s",Get_environ("e2"));
  displayln(output,"*s",Get_environ("e3"));*/
  displayln(output,"*s\n",Get_environ("number"));
  asm int 3;
  displayln(output,"*H",(long)atol(Get_environ("number")));

  Relinquish(1L);
}
