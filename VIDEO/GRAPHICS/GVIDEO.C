#define atoi
#define itoa
#define movmem
#include "mod.h"
#define false 0
#define true !false
#define NULL 0L
#define NO_CURSOR 4
#define NO_SCROLL 0x10
int x=0,y=0;
typedef struct video_line_type
{
  unsigned short count;        /*number of words to move from window to screen*/
  unsigned char page;
  char far *window_addr;      /*first pos. of window data plane*/
  struct video_line_type far *next_line_seg;
} video_line_type;

char back_scrolled;
short vwidth=80;
short vheight=25;
short Window_addresses[640][2];
char blanks[640];
long alloc=0,Dalloc=0;
video_line_type far *video_lines[480];
short screenseg=0xa000;
long screen_seg=0xa0000000L;
short scanlines[480];
short mode;
short textcolor=0x1700;
char mousex,mousey,mouse=false;

char hex[16]="0123456789ABCDEF";

char Border_Proto[2][6]={{'É','Í','»','º','È','¼'},
                        {'Ú','Ä','¿','³','À','Ù'}
                        };

extern void mov_cursor();

public(extern short,kbhit,(void));
public(extern short,getch,(void));
short screen_ofs;
/*#define Allocate(a) Allocate(a);alloc++;
#define Free(a) {Dalloc++;Free(a);}*/

typedef struct window_type
{
  short ulx,uly;           /*coordinate of the upper left corner of data */
  short width,height;      /*height and width of the data plane*/
  short dwidth,dheight;    /*height and width of display portion*/
  short xofs,yofs;         /*offset of the upper left corner of the display
                             viewport on the data plane*/
  short cursx,cursy;       /*Current Cursor x,y */
  short _cursx,_cursy;     /*Last Cursor x,y*/
  short status;            /*status-
                                0x0080 Window is in use already
                                0x0004 Window is offset on dataplane - x
                                0x0008 Window is offset on dataplane - y
                                0x0100 Filter of at end of line linefeeds
                           */

  short opts;              /*opts-
                               0x0001 display Escape code TXT
                               0x0002      do not process escape codes
                               0x0004      no cursor
                               0x0010      no scroll
                               0x0020      Add Border
                               0x0(00xx)00 Add Shadow depth xx (1-3)
                               0x1000      no dataplane- moving windows
                           */
  char cursattr,attr,borattr,backattr;
  char far *data;
  char far *border;
  char far *shadow;
  struct window_type far *next_window,far *previous_window;
} window_type;

window_type far *First_Window,far *Current_Window;
/*void far display(window_type far *window,unsigned char character);*/

void beep(void)
{}

void show_screen()
{
  short i;
  for (i=0;i<vheight;i++)
    display_line(i);
}

void setmemb(char far *buf,char data,short count)
{
  asm les di,buf
  asm mov al,data
  asm mov cx,count
  asm rep stosb
}

void setmemw(short far *buf,short data,short count)
{
  asm les di,buf
  asm mov ax,data
  asm mov cx,count
  asm rep stosb
}


void gen_address_array(short line)
{
  short i;
  short tempx,tempy,width;
  short add_sides=0;
  short segment,ofs,dseg;
  window_type far *Temp_Window;
  asm mov dseg,ds
  asm mov cx,vwidth
  asm mov bx,dseg
  asm mov ax,offset blanks
  asm mov es,dseg
  asm mov di,offset Window_addresses
init_address:
  asm stosw
  asm xchg  bx,ax
  asm stosw
  asm xchg  bx,ax
  asm inc ax
  asm loop init_address;
/*  for (i=0;i<vwidth;i++)
  {
    Window_addresses[i][1]=segment;
    Window_addresses[i][0]=(short)blanks+i;
  }*/
  Temp_Window=First_Window;
  while (Temp_Window)
  {
    add_sides=false;
    tempy=line-Temp_Window->uly;
    if (tempy>=0)
    {
      if ((tempy-Temp_Window->dheight)<=0)
      {
        tempx=Temp_Window->ulx;
        width=Temp_Window->dwidth;
        segment=(long)Temp_Window->data>>16;
        ofs=(tempy*width)+(short)Temp_Window->data;
        if (tempx+width>vwidth)
          width=vwidth-tempx;
        if (!(Temp_Window->opts&0x1000))
        {
          tempx=tempx*4+(short)Window_addresses;
          asm mov cx,width
          asm mov bx,segment
          asm mov ax,ofs
          asm mov es,dseg
          asm mov di,tempx
        fill_address:
          asm stosw
          asm xchg  bx,ax
          asm stosw
          asm xchg  bx,ax
          asm inc ax
          asm loop fill_address;

/*          for (i=0;i<(width);i++,tempx++)
          {
            if (tempx>=0)
            {
              Window_addresses[tempx][1]=segment;
              Window_addresses[tempx][0]=ofs;
            }
            ofs++;
          }  */
        }
        add_sides=true; /*if we added the window, then we need to indicate
                          to add the side borders, if present so we don't have
                          to again test where we are along the window*/
      }
    }
    Temp_Window=Temp_Window->next_window;
  }
}

void release(video_line_type far *line)
{
  video_line_type far *temp,far *next;

  temp=line->next_line_seg;
  line->next_line_seg=NULL;

  while (temp)
  {
    next=temp->next_line_seg;
    Free(temp);
    temp=next;
  }
}

void display_window(window_type far *window,char present)
{
  /*This function links the specified window into the
    video display line data structure*/
  short i;
  long page;
  short pos;
  short oldseg,count,oldofs;
  char step;
  short start,end;
  video_line_type far *temp_line;
  if (window)
  {
    if (present)
    {
      if (window->uly&&(window->opts&0x20))
        start=-1;
      else
        start=0;
      end=window->height+1;
      if (window->opts&0x20)
        end++;
      step=1;
    }
    else
    {
      if (window->previous_window)
        window->previous_window->next_window=window->next_window;
      else
        First_Window=window->next_window;
      start=window->height;
      if (window->opts&0x20)
        start++;
      if (window->uly&&(window->opts&0x20))
        end=-2;
      else
        end=-1;
      step=-1;
    }
    for (i=start;i!=end;i+=step)
    {
      if ((window->uly+i)<0||
          (window->uly+i)>=vheight)
        continue;
           /*compute page address */
      asm int 3;
      asm les bx,window
      asm mov ax,i
      asm add ax,es:[bx].uly
      asm mov dx,vwidth
      asm mul dx
      asm mov word ptr page,ax
      asm mov word ptr page+2,dx

      gen_address_array(window->uly+i);
      release(video_lines[window->uly+i]);
      temp_line=video_lines[window->uly+i];
      count=0;
      oldofs=Window_addresses[0][0]-1;
      oldseg=Window_addresses[0][1];
      temp_line->window_addr=(char far*)*(long far*)&Window_addresses[0][0];
      for (pos=0;pos<vwidth;pos++,page++)
      {
        if (Window_addresses[pos][1]==oldseg&&(((short)page&0xffff)||!(page)))
        {
          if (Window_addresses[pos][0]==++oldofs)
          {
            count++;
          }
        }
        else
        {
            {
              temp_line->count=count;
              temp_line->page=(page>>16);
              if (count)
              {
                if (temp_line->next_line_seg==NULL)
                {
                  temp_line->next_line_seg=(video_line_type far*)
                            Allocate(sizeof(video_line_type));
                  temp_line->next_line_seg->next_line_seg=NULL;
                }
                temp_line=temp_line->next_line_seg;
                count=0;
              }
              temp_line->window_addr=
                   (char far*)*(long far*)&Window_addresses[pos][0];
              oldseg=Window_addresses[pos][1];
              oldofs=Window_addresses[pos][0];
              count++;
            }
        }
      }

      if (count)
      {
        temp_line->count=count;
        temp_line->page=(short)(page>>16);
        if (temp_line->next_line_seg==NULL)
        {
          temp_line->next_line_seg=(video_line_type far*)
                    Allocate(sizeof(video_line_type));
          temp_line->next_line_seg->next_line_seg=NULL;
        }
        temp_line=temp_line->next_line_seg;
      }
      while (temp_line)
      {
        temp_line->count=0;
        temp_line=temp_line->next_line_seg;
      }
      display_line(window->uly+i);
    }
    if (!present)
      if (window->previous_window)
        window->previous_window->next_window=window;
      else
        First_Window=window;
  }
}

void select_window(window_type far *window)
{
  /*Increment supremacy table, display window*/
  if (Current_Window!=window)
    if (Current_Window==NULL)
    {
      First_Window=window;
      Current_Window=window;
    }
    else
    {
      if (First_Window==window)
      {
        if (window->next_window)
          First_Window=window->next_window;
        First_Window->previous_window=NULL;
      }
      else
      {
        if (window->previous_window)
          window->previous_window->next_window=window->next_window;
        if (window->next_window)
          window->next_window->previous_window=window->previous_window;
      }
      window->previous_window=Current_Window;
      Current_Window->next_window=window;
      window->next_window=NULL;
      Current_Window=window;
    }
  display_window(window,true);
}


void hide_window(window_type far *window)
{
  /*Increment supremacy table, display window*/
  if (First_Window==NULL)
  {
    First_Window=window;
    Current_Window=window;
  }
  else
  {
    if (First_Window==window) return;
    if (Current_Window==window)
    {
      if (window->previous_window)
        Current_Window=window->previous_window;
      Current_Window->next_window=NULL;
    }
    else
    {
      if (window->previous_window)
      {
        if (window->previous_window)
          window->previous_window->next_window=window->next_window;
        if (window->next_window)
          window->next_window->previous_window=window->previous_window;
      }
    }
    window->next_window=First_Window;
    First_Window->previous_window=window;
    window->previous_window=NULL;
    First_Window=window;
  }
  display_window(window,true);
}


public(windowptr,dupdisplay,(window_type far *window,short opts))
{
  /*This routine duplicates a window so that we can either move it, or
    resize it, or whatever */
  window_type far *temp;
  char border_attr;
  short width,height;
  short i;
  asm push ds;
  asm push cs;
  asm pop ds;
  temp=Allocate(sizeof(window_type));
#define set(a) temp->a=window->a
  set(ulx);
  set(uly);
  set(dwidth);
  set(dheight);
  set(width);
  set(height);
  width=temp->dwidth;
  height=temp->dheight;
  set(xofs);
  set(yofs);
  set(cursx);
  set(cursy);
  set(_cursx);
  set(_cursy);
  set(cursattr);
  set(attr);
  set(borattr);
  set(backattr);
  temp->status=0;
  temp->opts=opts;
  if (!(temp->opts&NO_DATA))
  {
    temp->data=(char far *)Allocate(width*(height+1)<<1);
    for (i=0;i<width*height;i++)
      temp->data[i]=window->data[i];
  }
  else
    temp->data=NULL;
  temp->previous_window=NULL;
  temp->next_window=NULL;
  if (temp->opts&0x20)
  {
    short far *b;
    b=(short far *)temp->border=Allocate(2*(2*(width+height+2)));
    for (i=0;i<2*(width+height+2);i++)
    {
      b[i]=window->border[i];
    }
  }
  else
    temp->border=NULL;
  select_window(temp);
  asm pop ds;
  return((windowptr)temp);
}

public(void,moddisplay,(window_type far *window,...))
{
  /*This routine is used to modify what a window's traits are.  Resizing
    and moving the the most important thereof*/
  char done=false;
  short oldy;
  short idx=0;
  asm push ds
  asm push cs
  asm pop ds
  while (!done)
  {
    switch(...[idx])
    {
      case 0:done=true;
             break;
      case 1:/*goto x,y*/
             display_window(window,false);
             window->ulx=...[idx+1];
             window->uly=...[idx+2];
             display_window(window,true);
             idx+=2;
             break;
      case 2:/*modify mouse x,y*/
             mousex=...[idx+1];
             oldy=mousey;
             mousey=...[idx+2];
             if (mouse)
             {
               display_line(oldy);
               display_line(mousey);
              }
             idx+=2;
             break;
      case 3:select_window(window);
             break;
      case 4:window->opts|=0x1000;
             break;
      case 5:window->opts&=0xefff;
             break;
      default:asm pop ds;
              return;
    }
    idx++;
  }
  asm pop ds;
}

public(short,getdisplay,(short far *x,short far *y,window_type far * far *ret))
{
  /*This routine returns a pointer to the window that was selected by the
    (x,y) point.  It will be the most 'ontop' window.
    The window will be return in the pointer to the pointer to the window
      variable in the other program.
    The function then returns more specifically what it found when the
      coordinate was picked.  Currently, this is either Data or Border.
    */
  window_type far *temp;
  window_type far *returnv=NULL;
  short lx=*x,ly=*y;
  short desc=0;
  asm push ds
  asm push cs
  asm pop ds
  temp=First_Window;
  while (temp)
  {
    if (lx>=temp->ulx&&
        ly>=temp->uly&&
        lx<(temp->ulx+temp->dwidth)&&
        ly<=(temp->uly+temp->dheight))
    {
      if (!(temp->opts&NO_DATA))
      {
        returnv=temp;
        desc=1;
      }
    }
    else
    if (temp->opts&0x20)
    {
        if (lx>=(temp->ulx-1)&&
            ly>=(temp->uly-1)&&
            lx<=(temp->ulx+temp->dwidth)&&
            ly<=(temp->uly+temp->dheight+1))
        {
          returnv=temp;
          desc=2;
        }
      }
    temp=temp->next_window;
  }
  *ret=returnv;
  *x-=returnv->ulx;
  *y-=returnv->uly;
  asm pop ds;
  return(desc);
}


public(windowptr,opendisplay,(short ulx,short uly,
                           short width,short height,
                           short opts,
                           char data_attr,char border_attr,char cursor_attr))
{
  window_type far *temp,far *t1;
  short i;
  if (ulx<=0||uly<=0)
    return(NULL);
  asm push ds
  asm push cs
  asm pop ds
  temp=(window_type far *)Allocate(sizeof(window_type));
  temp->ulx=ulx-1;
  temp->uly=uly-1;
  temp->width=width;
  temp->height=height-1;
  temp->dwidth=width;
  temp->dheight=height-1;
  temp->opts=opts;
  temp->cursx=0;
  temp->cursy=0;
  temp->_cursx=0;
  temp->_cursy=0;
  temp->cursattr=cursor_attr;
  temp->attr=data_attr;
  temp->backattr=data_attr;
  temp->status=0;
  if (!(temp->opts&NO_DATA))
  {
    temp->data=(char far *)Allocate(width*(height+1));
    for (i=0;i<width*height;i++)
      temp->data[i]=data_attr;
  }
  else
    temp->data=NULL;
  temp->previous_window=NULL;
  temp->next_window=NULL;
  if (temp->opts&0x20&&(0))
  {
    char far *b;
           /*   1234567
                ÚÄÄÄÄÄ¿1
                ³ÚÄÄÄ¿³2   2*height+
                ³ÀÄÄÄÙ³3   2*(width+ 2{for corners} )
                ÀÄÄÄÄÄÙ4
       c  ------  c  ||||||||  c ------- c
       0  2       2  4         4 6       6   */
    b=(char far *)temp->border=Allocate(2*(2*(width+height+2)));
    b[0]=Border_Proto[0][0];
    b[1]=border_attr;

    b[2+(width<<1)]=Border_Proto[0][2];
    b[3+(width<<1)]=border_attr;

    b[4+(width<<1)+(height<<2)]=Border_Proto[0][4];
    b[5+(width<<1)+(height<<2)]=border_attr;

    b[6+(width<<2)+(height<<2)]=Border_Proto[0][5];
    b[7+(width<<2)+(height<<2)]=border_attr;

    for (i=0;i<width;i++)
    {
      b[2+(i<<1)]=Border_Proto[0][1];
      b[3+(i<<1)]=border_attr;
      b[6+(width<<1)+(height<<2)+(i<<1)]=Border_Proto[0][1];
      b[7+(width<<1)+(height<<2)+(i<<1)]=border_attr;
    }
    for (i=0;i<(height<<1);i++)
    {
      b[4+(2*width)+(i<<1)]=Border_Proto[0][3];
      b[5+(2*width)+(i<<1)]=border_attr;
    }
  }
  else
    temp->border=NULL;
  select_window(temp);
  asm pop ds;
  return((windowptr)temp);
}

public(short,closedisplay,(window_type far *window))
{
  window_type far *temp;
  asm push ds
  asm push cs
  asm pop ds
  temp=First_Window;
  while (temp&&temp!=window)
    temp=temp->next_window;
  if (temp)
  {
    Free(temp->border);
    Free(temp->data);
    display_window(window,false);
    if (temp->previous_window)
      temp->previous_window->next_window=temp->next_window;
    if (temp->next_window)
      temp->next_window->previous_window=temp->previous_window;
    Free(temp);
    asm pop ds;
    return(true);
  }
  asm pop ds;
  return(false);
}

public(void,plot,(window_type far *window,short x,short y,char color))
{
  window->data[(window->width*y)+x]=color;
  display_line(window->uly+y);
}

public(void,line,(window_type far *window,short x1,short y1,
                                          short x2,short y2,char color))
{

  #define sign(x)((x)>0?1:((x)==0?0:(-1)))
  #define abs(x) ((x)>=0?x:-x)
  int dx,dy,dxabs,dyabs,i,px,py,sdx,sdy,x,y;
  int ay;
  char far *ptr;
  asm push ds
  asm push cs
  asm pop ds
  dx=x2-x1;
  dy=y2-y1;
  sdx=sign(dx);
  sdy=sign(dy);
  dxabs=abs(dx);
  dyabs=abs(dy);
  x=0;
  y=0;
  px=x1;
  py=y1;
  ay=py+window->uly;
  ptr=window->data+(window->width*py)+px;
  *ptr=color;
  if (dxabs>=dyabs)
    for (i=0;i<=dxabs;i++)
    {
      y+=dyabs;
      if (y>=dxabs)
      {
        y-=dxabs;
        display_line(ay);
        py+=sdy;
        ay+=sdy;
        ptr+=window->width*sdy;
      }
      if ((px>=0)&&(px<640)&&(py>=0)&&(py<400))
         *ptr=color;
/*        plot(window,px,py,color);*/
      ptr+=sdx;
/*      px+=sdx;*/
    }
  else
    for (i=0;i<=dyabs;i++)
    {
      x+=dxabs;
      if (x>=dyabs)
      {
        x-=dyabs;
/*        px+=sdx;*/
        ptr+=sdx;
      }
      if ((px>=0)&&(px<640)&&(py>=0)&&(py<400))
      {
         *ptr=color;
        /*plot(window,px,py,color);*/
      }
      display_line(ay);

      py+=sdy;
      ay+=sdy;
      ptr+=window->width*sdy;
    }
  display_line(ay);
  asm pop ds
}

void clr_video(void)
{
  unsigned short i;
  unsigned long page;
  for (i=0;i<480;i++)
  {
    video_lines[i]=(video_line_type far *)Allocate(sizeof(video_line_type));
    video_lines[i]->count=vwidth;
    video_lines[i]->window_addr=blanks;
    video_lines[i]->next_line_seg=NULL;
           /*compute page address */
      asm mov ax,i
      asm mov dx,vwidth
      asm mul dx
      asm mov word ptr page,ax
      asm mov word ptr page+2,dx
    video_lines[i]->page=page>>16;
    if ((page&0xffff)>((page+(unsigned)vwidth)&0xffff))
    {
      video_lines[i]->count=vwidth-((page+vwidth)&0xffff);
      video_lines[i]->next_line_seg=Allocate(sizeof(video_line_type));

      video_lines[i]->next_line_seg->count=((page+vwidth)&0xffff);
      video_lines[i]->next_line_seg->page=((page+vwidth)>>16);
      video_lines[i]->next_line_seg->window_addr=blanks;
      video_lines[i]->next_line_seg->next_line_seg=NULL;
    }
  }
}

cleanup(void,bye_bye,(void))
{
  asm mov ax,3
  asm int 10h
}

void main()
{
  short i;
  char far *txt;
  windowptr output;
  txt=Get_environ("GMODE");
  if (txt)
  {
    mode=atoi(txt);
  }
  txt=Get_environ("vheight");
  if (txt)
  {
    vheight=atoi(txt);
  }
  txt=Get_environ("vwidth");
  if (txt)
  {
    vwidth=atoi(txt);
  }
  asm mov ax,mode;
  asm int 10h;
  for (i=0;i<vheight;i++)
    scanlines[i]=i*vwidth;
  for (i=0;i<vwidth;i++)
    blanks[i]=7;
  _kbhit();
  _getch();
  _opendisplay();
  _closedisplay();
  _getdisplay();
  _moddisplay();
  _dupdisplay();
  _plot();
  _line();
  _bye_bye();
  mousex=0;
  mousey=0;
  mouse=true;
  clr_video();
  show_screen();
  Relinquish(1L);
  Exit(1L);
}





