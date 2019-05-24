#define atoi
#include "mod.h"


short DPRAM_ADDR;
short IO_ADDR;
short page_size;
short page;

extern short far hiread(char far *buffer,short length,short line);
public(short,hiread,(char far *buffer,short length,short line));

extern short far hiwrite(char far *buffer,short length,short line);
public(short,hiwrite,(char far *buffer,short length,short line));

extern short far hiopen(short line);
public(short,hiopen,(short line));

extern short far hiclose(short line);
public(short,hiclose,(short line));

main()
{
   IO_ADDR=atoi(Get_environ("Port"));
   page=atoi(Get_environ("page"));
   switch(page&0xf)
   {
     case 0x0:page_size=4;
              break;
     case 0x4:page_size=1;
              break;
     case 0x8:page_size=2;
              break;
     case 0xc:page_size=1;
              break;
     default:
       perish();
       break;
   }
   DPRAM_ADDR=page<<8;
  _hiopen();
  _hiread();
  _hiwrite();
  _hiclose();
  Relinquish(1L);
/*  _hiflush();*/
}
