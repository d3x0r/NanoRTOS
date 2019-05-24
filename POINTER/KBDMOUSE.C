#define moddisplay
#define getdisplay
#define opendisplay
#define displayln
#define closedisplay
#define dupdisplay
#define atoi
#include "mod.h"
#include "video.h"
#define true !false
#define false 0
windowptr output;
windowptr moving;

short newx,newy;
short buttons;
short resizing;
short height,width;
short _buttons=0;
short _x,_y;
short okay_move=false;
short upd_coord=true;
short upd_buttons=true;
short present;
short wx,wy;
time_struc curtime;

process_changes()
{
  if (_x!=newx||_y!=newy)
    upd_coord=true;
  if (_buttons!=buttons)
    upd_buttons=true;
  if (upd_coord)
  {
    if (okay_move)
    {
      if (moddisplay(moving,MOVE_WINDOW((newx)-wx,(newy)-wy),END_MOD))
      {
        _x=newx;
        _y=newy;
      }
    }
    else
    if (resizing)
    {
      if (moddisplay(moving,RESIZEX((newx-_x)),END_MOD))
        _x=newx;
      if (moddisplay(moving,RESIZEY((newy-_y)),END_MOD))
        _y=newy;
    }
    else
    {
       _x=newx;
       _y=newy;
    }
    moddisplay(0,MOVE_MOUSE(_x,_y),END_MOD);
    upd_coord=false;
  }
  if (upd_buttons)
  {
    short test=(buttons^_buttons)&buttons;  /*generates up edged bits*/
    if (test&1)
    {
      short spot;
      displayln(output,"left ");
      wx=newx;
      wy=newy;
      spot=getdisplay(&wx,&wy,&moving);
      if (moving)
      {
        switch(spot)
        {
          case 1:  /*data field*/
            moddisplay(moving,SELECT_WINDOW,END_MOD);
            break;
          case 2:  /*anywhere on the border, not corner or scroll bar*/
            okay_move=true;
            displayln(output,"Grabbed ");
            moddisplay(moving,REMOVE_DATA,SELECT_WINDOW,END_MOD);
            break;
          case 3:  /*upper left corner for close*/
            moddisplay(moving,CLOSE_WINDOW,END_MOD);
            break;
          case 4:  /*upper right corner for iconification */
            moddisplay(moving,ICON_WINDOW,END_MOD);
            break;
          case 5:  /*lower right corner for resizification*/
            moddisplay(moving,REMOVE_DATA,SELECT_WINDOW,END_MOD);
            resizing=true;
            break;
          case 6:
            moddisplay(moving,SCROLL_UP,END_MOD);
            break;
          case 7:
            moddisplay(moving,SCROLL_DOWN,END_MOD);
            break;
          case 8:
            moddisplay(moving,SCROLL_LEFT,END_MOD);
            break;
          case 9:
            moddisplay(moving,SCROLL_RIGHT,END_MOD);
            break;
          case 10:
            moddisplay(moving,PAGE_UP,END_MOD);
            break;
          case 11:
            moddisplay(moving,PAGE_DOWN,END_MOD);
            break;
          case 12:
            moddisplay(moving,PAGE_LEFT,END_MOD);
            break;
          case 13:
            moddisplay(moving,PAGE_RIGHT,END_MOD);
            break;
          case 14:
            moddisplay(moving,RESTORE_WINDOW,END_MOD);
            break;
          default:
            displayln(output,"Unknow Position in the Window!!!!");
            break;
        }
      }
    }
    if (test&2)
    {
      displayln(output,"Right ");
      Wake(4,0);  /*wake up the memory dumper*/
    }
    if (test&4)
    {
      displayln(output,"Middle ");
    }
    test=(buttons^_buttons)&_buttons;  /*generates down edged bits*/
    if (test&1)
    {
      if (okay_move)
      {
        okay_move=false;
        moddisplay(moving,RESTORE_DATA,SELECT_WINDOW,END_MOD);
      }
      if (resizing)
      {
        resizing=false;
        moddisplay(moving,RESTORE_DATA,SELECT_WINDOW,END_MOD);
      }
      displayln(output,"Upleft ");
    }
    if (test&2)
      displayln(output,"UpRight ");
    if (test&4)
      displayln(output,"UpMiddle ");

    _buttons=buttons;
    upd_buttons=false;
  }
}

public(void,movemouseup,(void))
{
  asm push ds;
  asm push cs;
  asm pop ds;
  if (newy) newy--;
  process_changes();
  asm pop ds;
}
public(void,movemousedown,(void))
{
  asm push ds;
  asm push cs;
  asm pop ds;
  newy++;
  if (newy>height)
    newy=height;
  process_changes();
  asm pop ds;
}
public(void,movemouseleft,(void))
{
  asm push ds;
  asm push cs;
  asm pop ds;
  if (newx) newx--;
  process_changes();
  asm pop ds;
}
public(void,movemouseright,(void))
{
  asm push ds;
  asm push cs;
  asm pop ds;
  newx++;
  if (newx>width)
    newx=width;
  process_changes();
  asm pop ds;
}

public(void,left_mouse,(void))
{
  asm push ds;
  asm push cs;
  asm pop ds;
  buttons^=1;
  process_changes();
  asm pop ds;
}

public (void,right_mouse,(void))
{
  asm push ds
  asm push cs
  asm pop ds
  buttons^=2;
  process_changes();
  asm pop ds
}

public (void,middle_mouse,(void))
{
  asm push ds
  asm push cs
  asm pop ds
  buttons^=4;
  process_changes();
  asm pop ds
}

main()
{
  char far *txt;
  output=opendisplay(40,5,20,10,BORDER,0x30,0x30,0x2f,"Key_Mouse");
  txt=Get_environ("vheight");
  if (txt)
    height=atoi(txt)-1;
  width=80;
  _left_mouse();
  _right_mouse();
  _middle_mouse();
  _movemouseup();
  _movemousedown();
  _movemouseleft();
  _movemouseright();
  Relinquish(1L);
}


