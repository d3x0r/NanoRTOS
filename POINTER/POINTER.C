/*#define DEBUG*/
#include "mod.h"
#include "video.h"

#ifdef DEBUG
#define premod(Opcode) asm mov ax,0xf000; asm mov es,ax;asm mov ax,es:[Opcode];
#define postmod asm mov ax,0xf000;asm mov es,ax;asm mov ax,es:[0xfffe];
#else
#undef premod
#define premod(a)
#undef postmod
#define postmod
#endif

#define true (!false)
#define false 0
windowptr output;
windowptr moving;

short newx,newy;
short buttons;
short resizing;
short vheight,vwidth;
short _buttons=0;
short _x,_y;
short okay_move=false;
short upd_coord=true;
short upd_buttons=true;
short present;
short wx,wy;
time_struc curtime;


main()

{
  asm mov ax,0

  asm int 33h;
  asm mov present,ax
  if (!present)
    perish();
#ifdef DEBUG
  output=opendisplay(40,5,20,10,BORDER,0x30,0x30,0x2f,"Mouse");
/*  moddisplay(output,ICON_WINDOW,END_MOD);*/
#endif
  vwidth=*(unsigned short far *)0x0000044aL;
  if (vheight==0)
  {
    vheight=(((*(unsigned short far *)0x0000044cL))/(vwidth*2));
    if (vheight>25) vheight--;
    vheight*=8;
  }
  if (vwidth>132)
    vwidth=132;
  asm mov ax,8
  asm mov cx,0;
  asm mov dx,vheight;
  asm int 33h
  vwidth*=8;
  asm mov ax,7
  asm mov cx,0
  asm mov dx,vwidth;
  asm int 33h
  while (1)
  {
    asm mov ax,3
    asm int 33h;
    asm mov newx,cx
    asm mov newy,dx
    asm mov buttons,bx
    if (_x!=newx||_y!=newy)
      upd_coord=true;
    if (_buttons!=buttons)
      upd_buttons=true;
    if (upd_coord)
    {
      if (okay_move)
      {
        premod(1);
        if (moddisplay(moving,MOVE_WINDOW((newx/8)-wx,(newy/8)-wy),END_MOD))
        {
          postmod;
          premod(2);
          moddisplay(0,MOVE_MOUSE(newx/8,newy/8),END_MOD);
          postmod;
          _x=newx;
          _y=newy;
        }
        else
        {
          postmod;
          asm mov ax,4
          asm mov cx,_x;
          asm mov dx,_y;
          asm int 33h;
        }
      }
      else
      if (resizing)
      {
        char restore=false;
        premod(10);
        if (moddisplay(moving,RESIZEX((newx-_x)/8),END_MOD))
        {
          postmod;
          premod(2);
          moddisplay(0,MOVE_MOUSE(newx/8,_y/8),END_MOD);
          postmod;
          _x=newx;
        }
        else
          restore=true;
        premod(12);
        if (moddisplay(moving,RESIZEY((newy-_y)/8),END_MOD))
        {
          postmod;
          premod(2);
          moddisplay(0,MOVE_MOUSE(_x/8,newy/8),END_MOD);
          postmod;
          _y=newy;
        }
        else
        {
          restore=true;
        }
        if (restore)
        {
          asm mov ax,4
          asm mov cx,_x;
          asm mov dx,_y;
          asm int 33h;
        }
     }
     else
     {
          premod(2);
          moddisplay(0,MOVE_MOUSE(newx/8,newy/8),END_MOD);
          postmod;
          _x=newx;
          _y=newy;
     }
     upd_coord=false;
    }
    if (upd_buttons)
    {
      short test=(buttons^_buttons)&buttons;  /*generates up edged bits*/
      if (test&1)
      {
        short spot;
#ifdef DEBUG
        displayln(output,"left ");
#endif
        wx=newx/8;
        wy=newy/8;
        spot=getdisplay(&wx,&wy,&moving);
        if (moving)
        {
          switch(spot)
          {
            case 1:  /*data field*/
              premod(3);
              if (moving->next_window)
                moddisplay(moving,SELECT_WINDOW,END_MOD);
              else
                moving->mouse_handle(wx,wy,upd_buttons);
              postmod;
              break;
            case 2:  /*anywhere on the border, not corner or scroll bar*/
              okay_move=true;
#ifdef DEBUG
              displayln(output,"Grabbed ");
#endif
              premod(4);
              moddisplay(moving,REMOVE_DATA,SELECT_WINDOW,END_MOD);
              postmod;
              break;
            case 3:  /*upper left corner for close*/
              premod(9);
              moddisplay(moving,CLOSE_WINDOW,END_MOD);
              postmod;
              break;
            case 4:  /*upper right corner for iconification */
              premod(6);
              moddisplay(moving,ICON_WINDOW,END_MOD);
              postmod;
              break;
            case 5:  /*lower right corner for resizification*/
              premod(4);
              moddisplay(moving,REMOVE_DATA,SELECT_WINDOW,END_MOD);
              postmod;
              resizing=true;
              break;
            case 6:
              premod(11);
              moddisplay(moving,SCROLL_UP,END_MOD);
              postmod;
              break;
            case 7:
              premod(11);
              moddisplay(moving,SCROLL_DOWN,END_MOD);
              postmod;
              break;
            case 8:
              premod(13);
              moddisplay(moving,SCROLL_LEFT,END_MOD);
              postmod;
              break;
            case 9:
              premod(13);
              moddisplay(moving,SCROLL_RIGHT,END_MOD);
              postmod;
              break;
            case 10:
              premod(14);
              moddisplay(moving,PAGE_UP,END_MOD);
              postmod;
              break;
            case 11:
              premod(14);
              moddisplay(moving,PAGE_DOWN,END_MOD);
              postmod;
              break;
            case 12:
              premod(15);
              moddisplay(moving,PAGE_LEFT,END_MOD);
              postmod;
              break;
            case 13:
              premod(15);
              moddisplay(moving,PAGE_RIGHT,END_MOD);
              postmod;
              break;
            case 14:
              premod(5);
              moddisplay(moving,RESTORE_WINDOW,END_MOD);
              postmod;
              break;
            default:
#ifdef DEBUG
              displayln(output,"Unknow Position in the Window!!!!");
#endif
              break;
          }
        }
      }
      if (test&2)
      {
        short spot;
        wx=newx/8;
        wy=newy/8;
        spot=getdisplay(&wx,&wy,&moving);
        moddisplay(moving,HIDE_WINDOW,END_MOD);
#ifdef DEBUG
        displayln(output,"Right ");
#endif
      }
      if (test&4)
      {
#ifdef DEBUG
        displayln(output,"Middle ");
#endif
      }
      test=(buttons^_buttons)&_buttons;  /*generates down edged bits*/
      if (test&1)
      {
        if (okay_move)
        {
          okay_move=false;
          premod(5);
          moddisplay(moving,RESTORE_DATA,SELECT_WINDOW,END_MOD);
          postmod;
        }
        if (resizing)
        {
          resizing=false;
          premod(5);
          moddisplay(moving,RESTORE_DATA,SELECT_WINDOW,END_MOD);
          postmod;
        }
#ifdef DEBUG
        displayln(output,"Upleft ");
#endif
      }
      if (test&2)
      {
#ifdef DEBUG
        displayln(output,"UpRight ");
#endif
      }
      if (test&4)
      {
#ifdef DEBUG
        displayln(output,"UpMiddle ");
#endif
      }
      _buttons=buttons;
      upd_buttons=false;
    }
    Relinquish(0L);
  }
}

