#include "mod.h"
#incldue "ether.h"
#pragma warn -rvl
#ifndef VIDEO
dynamic( short,keypressed,(windowptr window));
dynamic( char,readch,(windowptr window));
dynamic( void,Clr_window,(windowptr window));
dynamic( windowptr,
           opendisplay,
           (short ulx,short uly,short width,short height,short opts,
           char data_attr,char border_attr,char cursor_attr,char far *title));
dynamic( void,get_xy,
           (windowptr window,short far *x,short far *y));
dynamic( void,display,(windowptr window,char charac));
dynamic ( void,displayln,(windowptr window,char far *line,...));
dynamic( void,position,(windowptr window,int x,int y));
dynamic( void,setattr,(windowptr window,char attr));
dynamic( char,getattr,(windowptr window));
dynamic( void,clr_display,(windowptr window,char notes));
dynamic(short,getdisplay,(short far *x,short far *y,windowptr far *window));
dynamic(short,moddisplay,(windowptr window,...));
dynamic(short,closedisplay,(windowptr window));
dynamic(windowptr,dupdisplay,(windowptr window,short opts));
#else
dynamic(char,rawkeypressed,(void));
dynamic(char,rawreadch,(void));
#endif

connection far *output;

public(windowptr,opendisplay,(short ulx,short uly,short width,short height,short opts,
           char data_attr,char border_attr,char cursor_attr,char far *title))
{

}


main()
{
  _opendisplay()
  output=openether(");


}
