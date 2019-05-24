#include "mod.h"
#define VIDEO
#include "video.h"
module far *Video_TCB;
#define false 0
#define true !false
void far hook_vectors(void);
char far Getch_check(void);
char far keypressed_check(void);
char (far *_rawkeypressed)(void)=keypressed_check;
char (far *_rawreadch)(void)=Getch_check;
#define NULL 0L
int x=0,y=0;
char data;
short remote_keybrd=false;
char back_scrolled;
char mono;
short vwidth=80;
short vheight=0;
short Window_addresses[134][2];
long alloc=0,Dalloc=0;
short blanks[134];
video_line_type far * far*video_lines;
short far *scanlines;
long screen_seg=0xb8000000L;
short screenseg;
short textcolor=0x1700;
short mousex,mousey;
char mouse=false;

char hex[16]="0123456789ABCDEF";

/*char *escape_codes[32]={"<Nul>","<Soh>","<Stx>","<Etx>",
                                    "<Eot>","<Enq>","<Ack>","<Bel>",
                                    "<Bs>","<Ht>","<Lf>","<Vt>",
                                    "<Ff>","<Cr>","<So>","<Si>",
                                    "<Dle>","<D1>","<D2>","<D3>",
                                    "<D4>","<Nak>","<Syn>","<Etb>",
                                    "<Can>","<Em>","<Sub>","<Esc>",
                                    "<Fs>","<Gs>","<Rs>","<Us>"};*/

#define MAX_BORDER_TYPES 2
#define BORDER_CHARS 15
char Border_Proto[MAX_BORDER_TYPES][BORDER_CHARS]=
                                  {
                               /* Attentive Window */
                                    {'\x08','Í','\n','º','È','^',     /*normal border chars*/
                                     '\033','\032','°','Û',           /*bottom scroll bar */
                                     '\030','\031','°','Û',           /*right Scroll Bar */
                                     '¼'},
                               /* Non Attentive Window. */
                                    {'Ú','Ä','\n','³','À','Ù',    /*normal border chars*/
                                     'Ä','Ä','Ä','Ä',                   /*bottom scroll bar */
                                     '³','³','³','³',                   /*right Scroll Bar */
                                     'Ù'}
                                    };
#define ACTIVE    0
#define INACTIVE  1
extern void mov_cursor();

extern char far kbhit(void);
extern char far getch(void);
short screen_ofs;
/*#define Allocate(a) Allocate(a);alloc++;
#define Free(a) {Dalloc++;Free(a);}*/

window_type far *First_Window,far *Current_Window;

#ifdef DEBUG
window_type far *output;
#endif
extern void display_line(short line);
public (extern void,display,(window_type far *window,unsigned char character));
/*void far display(window_type far *window,unsigned char character);*/
void far displayln(window_type far *window,unsigned char far *str,...);
char cursor;

void kill_cursor(void)
{
   if (mono)
   {
      asm mov dx,0x3b4;
      asm mov al,10;
      asm out dx,al;
      asm inc dx
      asm in al,dx;
      asm mov cursor,al;
      asm mov dx,0x3b4;
      asm mov al,10;
      asm out dx,al;
      asm inc dx;
      asm mov al,0xff;
      asm out dx,al;
   }
   else
   {
      asm mov dx,0x3d4;
      asm mov al,10;
      asm out dx,al;
      asm inc dx
      asm in al,dx;
      asm mov cursor,al;
      asm mov dx,0x3d4;
      asm mov al,10;
      asm out dx,al;
      asm inc dx;
      asm mov al,0xff;
      asm out dx,al;
   }
}

void restore_cursor(void)
{
   if (mono)
   {
      asm mov dx,0x3b4;
      asm mov al,10;
      asm out dx,al;
      asm inc dx
      asm mov al,cursor;
      asm out dx,al;
   }
   else
   {
      asm mov dx,0x3d4;
      asm mov al,10;
      asm out dx,al;
      asm inc dx
      asm mov al,cursor;
      asm out dx,al;
   }
}


void update_Scroll_bars(window_type far *window)
{
   if (window==Current_Window)
   {
      if (window->dheight!=window->height)
      {
            window->border[10+(window->dwidth<<1)+
                      ((((window->yofs)*(window->dheight-2)+
                                    (window->height-window->dheight)/2)/
                               (window->height-window->dheight))<<2)]=
               Border_Proto[0][13];
      }

      if (window->dwidth!=window->width)
      {
            window->border[12+(window->dheight<<2)+(window->dwidth<<1)+
                      ((((window->xofs)*(window->dwidth-3)+
                                    (window->width-window->dwidth)/2)/
                               (window->width-window->dwidth))<<1)]=
               Border_Proto[0][9];
      }
   }
}


void draw_border(char value,
                         window_type far *window)
{
                /*      1234567
                        ÚÄÄÄÄÄ¿1
                        ³ÚÄÄÄ¿³2  2*height+
                        ³ÀÄÄÄÙ³3  2*(width+ 2{for corners} )
                        ÀÄÄÄÄÄÙ4
          c ------   c   ||||||||    c ------- c
          0 2           2   4           4 6         6  */
      short i,
               width=window->dwidth,
               height=window->dheight,
               tlen=strlen(window->title)-1,
               tpos=(width-tlen)>>1,
               attr=window->borattr,
               notitle=window->opts&NO_TITLE;
      char far *buffer=window->border,
             far *title=window->title;
      height++;
      if ((window->opts&CLOSED)||
            !window||
            !buffer||
            !(window->opts&BORDER)) return;
      for (i=1;i<=(7+(width<<2)+(height<<2));i+=2)
         buffer[i]=attr;
      buffer[0]=Border_Proto[value][0];
      buffer[2+(width<<1)]=Border_Proto[value][2];
      buffer[4+(width<<1)+(height<<2)]=Border_Proto[value][4];
      if (window->opts&ICON)
         buffer[6+(width<<2)+(height<<2)]=Border_Proto[value][14];
      else
         buffer[6+(width<<2)+(height<<2)]=Border_Proto[value][5];

      for (i=0;i<width;i++)
      {
         if ((notitle)||(i<tpos)||(i>tpos+tlen))
            buffer[2+(i<<1)]=Border_Proto[value][1];
         else
         {
            buffer[2+(i<<1)]=title[i-tpos];
         }
         if (window->dwidth==window->width||window->opts&ICON)
         {
            buffer[6+(width<<1)+(height<<2)+(i<<1)]=Border_Proto[value][1];
         }
         else
         {
            if (i==0)
               buffer[6+(width<<1)+(height<<2)+(i<<1)]=Border_Proto[value][6];
            else
            if (i==width-1)
               buffer[6+(width<<1)+(height<<2)+(i<<1)]=Border_Proto[value][7];
            else
               buffer[6+(width<<1)+(height<<2)+(i<<1)]=Border_Proto[value][8];
         }
      }
      for (i=0;i<height;i++)
      {
         buffer[4+(2*width)+(i<<2)]=Border_Proto[value][3];
         if (window->dheight==window->height||window->opts&ICON)
         {
            buffer[6+(2*width)+(i<<2)]=Border_Proto[value][3];
         }
         else
         if (i==0)
            buffer[6+(2*width)+(i<<2)]=Border_Proto[value][10];
         else
         if (i==height-1)
            buffer[6+(2*width)+(i<<2)]=Border_Proto[value][11];
         else
            buffer[6+(2*width)+(i<<2)]=Border_Proto[value][12];
      }
   if (value==ACTIVE&&!(window->opts&ICON))
      update_Scroll_bars(window);
}

void beep(void)
{}

void display_window(window_type far *window,char present);

void scroll_data(window_type far *window)
{
   /*this function scrolls the dataplane of a window up one place*/
   asm les bx,window
   asm mov dx,es:[bx].(struct window_type)width
   asm mov ax,es:[bx].(struct window_type)height;
   asm push dx
   asm mul dx
   asm pop dx
   asm mov cx,ax
   asm mov al,es:[bx].(struct window_type)attr
   asm xor ah,ah
   asm les bx,es:[bx].(struct window_type)data
   asm mov si,dx
   asm shl si,1
   asm add si,bx
   asm mov di,bx
   asm push ds
   asm push ax
   asm mov ax,es
   asm mov ds,ax
   asm pop ax
   asm xchg al,ah
   asm rep movsw
   asm pop ds
   asm mov cx,dx
   asm rep stosw
}


#define displayhexrev(window,number) {\
   displaybytehex(window,number);         \
   displaybytehex(window,number>>8);}

#define displayhex(window,number) { \
   displaybytehex(window,number>>8); \
   displaybytehex(window,number); }

#define displaybytehex(window,number){\
   display(window,hex[(number>>4)&0x000f]);   \
   display(window,hex[(number)&0x000f]); }

#define displaybin(window,number,length) { \
   char i;                                                   \
   for (i=length-1;i>=0;i--)                        \
      display(window,0x30+((number>>i)&1)); }


public (void,
            displayln,
            (window_type far *window,unsigned char far *str,...))
{
   short i=0,j=0;
   char param_count=0,c1,c2;
   char do_update=true;
   char islong=false;
   char number[12];
   char far *ts;
   char width, zfill = 0;
   short value;
   long lvalue;
   char size;
   Load_DS;
	
   if (!window||
         !window->data||
         (window->opts&CLOSED))
   {
      asm pop ds;
      return;
   }
   while ((c1=str[i])!=0)
   {
      if (c1=='%')
      {
         width = 0; // display width minimum accumulator
         zfill = 0; // zero fill 
do_more:
         i++;
         switch(str[i])
         {
         case '%':display(window,'%');
                  break;
         case 'd':
            value=...[param_count];
            itoa(value,number);
            size=strlen(number);
            for (;width>size;width--)
               display(window,' ');
            displayln(window,"%s",(char far *)number);
            param_count++;
            break;
         case 'D':
            lvalue=...[param_count];
            ltoa(lvalue,number);
            size=1;
            while (value/=10)
               size++;
            for (;width>size;width--)
               display(window,' ');
            displayln(window,"%s",(char far *)number);
            param_count++;
            break;
			case 'x':
				if( width < 3 ) // byte display intended...
				{
	            displaybytehex(window,...[param_count]);
	            param_count++;
					break;
				}
         case 'h':
         	displayhex(window,...[param_count]);
            param_count++;
            break;
         case 'r':
         	displayhexrev(window,...[param_count]);
            param_count++;
            break;
			case 'X':
         case 'H':
         	displayhex(window,...[param_count+1]);
            displayhex(window,...[param_count]);
            param_count+=2;
            break;
         case 'n':displaybytehex(window,...[param_count]);
                      param_count++;
                      break;
         case 'R':displayhexrev(window,...[param_count]);
                      displayhexrev(window,...[param_count+1]);
                      param_count+=2;
                      break;
         case 'b':displaybin(window,...[param_count],8);
                      param_count++;
                      break;
         case 'B':displaybin(window,...[param_count],16);
                      param_count++;
                      break;
         case 's':j=0;
                      ts=(char far *)((unsigned long)...[param_count+1]<<16|
                                              (unsigned long)...[param_count]);
                      while ((c2=ts[j])!=0)
                      {
                         display(window,c2);
                         j++;
                      }
                      for(;j<width;j++)
                         display(window,' ');
                      param_count+=2;
                      break;
         case 'c':display(window,...[param_count]);
                      param_count+=1;
                      break;
         case 'u':do_update=false;
                      break;
         case '0':
         	if( !width )
         	{
         		zfill = 1;
         		break;
         	}
         case '1':
         case '2':
         case '3':
         case '4':
         case '5':
         case '6':
         case '7':
         case '8':
         case '9':
            width*=10;
            width+=str[i]-0x30;
            goto do_more;
         default:
         	display(window, '%%' );
         	if( zfill )
         		display( window, '0' );
            if (width)
               displayln(window,"%d",width);
            display(window,str[i]);
         }
      }
      else
         display(window,str[i]);
      i++;
   }
   if (do_update)
      display(window,0);
   asm pop ds;
}

public(void,setattr,(window_type far *window,int c))
{
   Load_DS;
   if (window&&!(window->opts&CLOSED))
      window->attr=c;
   asm pop ds;
}

void scroll(window_type far *window,char notes)
{
   /*This routine scrolls the window down from the cursor to EOS*/
   /* 1==
         2==
         3== Scroll data up*/
   short pos,length;
   if (!window||!window->data||(window->opts&CLOSED)) return;
   if (notes==1)
   {
      pos=window->cursy*window->width;
      pos+=window->cursx;
      length=(window->height+1)*window->width;
      length-=pos;
      length=length<<1;
      movmem(window->data+pos,window->data+pos-window->width,length);
   }
   if (notes==2)
   {
      pos=(window->cursy)*window->width;
      pos+=window->cursx;
      length=(window->height+1)*window->width;
      length-=pos;
      length-=window->width;
      length=length<<1;
      movmem(window->data+pos,window->data+pos+window->width,length);
   }
   if (notes==3)
   {
/*    movmem(window->data+(window->width),
                window->data,
                window->width*(window->height)<<1);*/
      asm les bx,window;
      asm mov cl,es:[bx].width;
      asm xor ch,ch;
      asm mov al,0;
      asm mov ah,es:[bx].(struct window_type)attr;
      asm les di,es:[bx].(struct window_type)data
      asm rep stosw;
   }
}

void far show_cursor(window_type far *window)
{
         char temp_attr;
         unsigned char tempx,tempy;
         short temp_offset;
         short far *temp_ptr;
         char oldy;
         if (!window||!window->data||(window->opts&CLOSED)) return;
         tempx=window->_cursx;
         tempy=window->_cursy;
         oldy=tempy+window->uly;
         temp_offset=tempx+(tempy*window->width);
         temp_ptr=window->data+temp_offset;
         if (*temp_ptr>>8==window->cursattr)
         {
            *(temp_ptr)=
                   (*(temp_ptr)&0xff)+
                   ((short)(window->backattr)<<8);
            if (tempy<=(window->dheight+window->yofs)&&
                  tempy>=window->yofs)
               display_line(window->uly+tempy);
         }
         tempx=window->cursx;
         tempy=window->cursy;
         temp_offset=tempx+(tempy*window->width);
         temp_ptr=window->data+temp_offset;
         window->backattr=*((char far *)(temp_ptr)+1);
         if ((window->backattr&0x7)==
             ((window->backattr&0x70)>>4))
         {
            temp_attr=window->cursattr&0x70|((window->cursattr&0x70)>>4);
            *(temp_ptr)=
                   (*(temp_ptr)&0xff)+
                           ((short)(temp_attr)<<8);
         }
         else
            *(temp_ptr)=
                   (*(temp_ptr)&0xff)+
                           ((short)(window->cursattr)<<8);
         if (back_scrolled&&
               oldy<=(window->dheight+window->yofs)&&
               oldy>=window->yofs)
            display_line(oldy);
         if (tempy<=(window->dheight+window->yofs)&&
               tempy>=window->yofs)
            display_line(window->uly+tempy);
         window->_cursx=window->cursx;
         window->_cursy=window->cursy;
}

public(void,position,(window_type far *window,
                               unsigned char x,unsigned char y))
{
   /*This routine puts the cursor of specified window at
      x,y specified. If the X,Y are beyound the displayed
      boundries, the scroll bars are moved, if possible to accomidate
      otherwise the cursor is left where it was*/
   Load_DS;
   if (window->data&&window&&!(window->opts&CLOSED))
   {
      while (window->status&0x80)
         Relinquish(0L);
/*    if ((x>=window->x_ofs)&&
            (x<=(window->x_ofs+window->width)))
         window->cursx=x;
      else
         if (window->status&4)
            if (x<window->x_ofs)
            {
               window->cursx=x;
               window->x_ofs=x;
            }
            else
            {
               window->cursx=x;
               window->x_ofs=x-window->width;
            }
      if ((y>=window->y_ofs)&&
            (y<=(window->y_ofs+window->height)))
         window->cursy=y;
      else
         if (window->status&8)
            if (y<window->y_ofs)
            {
               window->cursy=y;
               window->y_ofs=y;
            }
            else
            {
               window->cursy=y;
               window->y_ofs=y-window->height;
            }*/
      window->status&=0xfeff;
      if (y<=window->height)
         window->cursy=y;
      if (x<window->width)
         window->cursx=x;
      if (!(window->opts&NO_CURSOR))
      {
/*get Move the previous position to tempx, and tempy.  then put what
   was behind the cursor out on the screen*/
         show_cursor(window);
/*if Page mode is currently selected, then move the attribute that
   was on the screen to be the current attribute.  This is to permit
   the cursor to jump fields and not destroy the current attributes that
   are on the screen*/
      }
   }
   asm pop ds;
}

void update_window(window_type far *window)
{
   short i;
   short endline=(window->uly+window->dheight+2);
   if (window->opts&CLOSED)
      return;
   if (endline>vheight) endline=vheight;
      for (i=(window->uly-1)<0?0:(window->uly-1);
               i<endline;i++)
         display_line(i);
}

public(void,clr_display,(window_type far *window,char notes))
{
   /*This routine clears the specified window according to the passed notes-
         1> Erase entire Screen
         2> Erase to EOL
         3> Erase to EOS
         4> Erase from TOS
   */
   char i;
   if (window->data&&window&&!(window->opts&CLOSED))
   {
      while (window->status&0x80)
         Relinquish(0L);
      switch(notes)
      {
         case 1:asm les bx,window
                   asm mov dx,es:[bx].(struct window_type)width
                   asm mov ax,es:[bx].(struct window_type)height
                   asm inc ax;
                   asm xor dh,dh
                   asm mul dx;
                   asm mov cx,ax;
                   asm mov al,0x0;
                   asm mov ah,es:[bx].(struct window_type)attr
                   asm les di,es:[bx].(struct window_type)data
                   asm rep stosw
                   break;
         case 2:
                   asm les bx,window
                   asm mov ax,es:[bx].(struct window_type)width
                   asm push ax;
                   asm mov dx,es:[bx].(struct window_type)cursy
                   asm mov ax,es:[bx].(struct window_type)width
                   asm mul dx
                   asm add ax,es:[bx].(struct window_type)cursx
                   asm pop dx
                   asm sub dx,es:[bx].(struct window_type)cursx
                   asm mov cx,dx;
                   asm push ax
                   asm mov al,0x0;
                   asm mov ah,es:[bx].(struct window_type)attr
                   asm les di,es:[bx].(struct window_type)data;
                   asm pop dx;
                   asm shl dx,1;
                   asm add di,dx;
                   asm rep stosw;
                   break;
         case 3:
                   asm les bx,window
                   asm mov dx,es:[bx].(struct window_type)width
                   asm mov ax,es:[bx].(struct window_type)height
                   asm inc ax;
                   asm mul dx
                   asm push ax;
                   asm mov dx,es:[bx].(struct window_type)cursy
                   asm mov ax,es:[bx].(struct window_type)width
                   asm mul dx
                   asm add ax,es:[bx].(struct window_type)cursx
                   asm pop dx
                   asm push ax
                   asm sub ax,dx
                   asm neg ax
                   asm mov cx,ax;
                   asm mov al,0x0;
                   asm mov ah,es:[bx].(struct window_type)attr
                   asm pop dx;
                   asm les di,es:[bx].(struct window_type)data;
                   asm shl dx,1;
                   asm add di,dx;
                   asm rep stosw;
                   break;
         case 4:asm les bx,window
                   asm mov dx,es:[bx].(struct window_type)cursy
                   asm mov ax,es:[bx].(struct window_type)width
                   asm mul dx
                   asm add ax,es:[bx].(struct window_type)cursx
                   asm mov cx,ax;
                   asm mov al,0x0;
                   asm mov ah,es:[bx].(struct window_type)attr
                   asm les di,es:[bx].(struct window_type)data;
                   asm rep stosw;
                   break;
      }
      window->backattr=*((char far*)(window->data)+1);
      position(window,window->cursx,window->cursy);
      Load_DS;
      update_window(window);
      asm pop ds
   }
}

public(void,get_xy,(window_type far *window,
                  short far *x,short far *y))
{
   if (window->data&&window&&!(window->opts&CLOSED))
   {
      *x=window->cursx;
      *y=window->cursy;
   }
   else
   {
      *x=-1;
      *y=-1;
   }
}


char getcursorchar(window_type far *window)
{
   short i;
   if (window&&window->data&&!(window->opts&CLOSED))
   {
      i=window->width*window->cursy;
      i+=window->cursx;
      return(*(char far*)(window->data+i));
   }
   return(0);
}

void show_screen()
{
   char i;
   for (i=0;i<vheight;i++)
      display_line(i);
}


void gen_address_array(char line/*,char present*/)
{
   short i;
   short tempx,tempy,width;
   short add_sides=0;
   short segment,ofs;
   window_type far *Temp_Window;
   asm push ds
   asm pop segment
   for (i=0;i<vwidth;i++)
   {
      Window_addresses[i][1]=segment;
      Window_addresses[i][0]=(short)blanks+(i<<1);
   }
   Temp_Window=First_Window;
   while (Temp_Window)
   {
      add_sides=false;
      tempy=line-Temp_Window->uly;
      if (tempy>=0)
         if ((tempy-Temp_Window->dheight)<=0)
         {
            tempx=Temp_Window->ulx;
            width=Temp_Window->dwidth;
            segment=(long)Temp_Window->data>>16;
            ofs=((Temp_Window->xofs+
                     (tempy+Temp_Window->yofs)*Temp_Window->width)<<1)+
                   (short)Temp_Window->data;
            if (tempx+width>vwidth)
               width=vwidth-tempx;
            if (!(Temp_Window->opts&NO_DATA)||(Temp_Window->opts&ICON))
            {
               for (i=0;i<(width);i++)
               {
                  if (tempx+i>=0)
                  {
                     Window_addresses[tempx+i][1]=segment;
                     Window_addresses[tempx+i][0]=ofs;
                  }
                  ofs+=2;
               }
            }
            add_sides=true; /*if we added the window, then we need to indicate
                                       to add the side borders, if present so we don't have
                                       to again test where we are along the window*/
         }

      if (Temp_Window->opts&BORDER)  /*if there was a border, then we need
                                                       to find out what portion of the border
                                                       buffer we need to put out */
      {
         if (add_sides)
         {
            segment=(long)Temp_Window->border>>16;
            width=Temp_Window->dwidth;
            ofs=4+(width<<1)+(4*tempy)+(short)Temp_Window->border;
            /*we know we need to add the sides of the window */
            tempx--;
            if (tempx>=0&&tempx<vwidth)
            {
               Window_addresses[tempx][1]=segment;
               Window_addresses[tempx][0]=ofs;
            }
            if ((tempx+width+1)>=0&&(tempx+width+1)<vwidth)
            {
               Window_addresses[tempx+width+1][1]=segment;
               Window_addresses[tempx+width+1][0]=ofs+2;
            }
         }
         else
         if (line==(Temp_Window->uly-1))
         {
            /*This is the top border of the window */

            width=Temp_Window->dwidth;
            tempx=Temp_Window->ulx;
            segment=(long)Temp_Window->border>>16;
            ofs=(short)Temp_Window->border;
            if (tempx-1>=0)
            {
               Window_addresses[tempx-1][1]=segment;
               Window_addresses[tempx-1][0]=ofs;
            }
            ofs+=2;
            if (width>vwidth) width=vwidth-width;
            for (i=0;i<(width+1);i++)
            {
               if (tempx+i>=0)
               {
                  Window_addresses[tempx+i][1]=segment;
                  Window_addresses[tempx+i][0]=ofs;
               }
               ofs+=2;
            }
         }
         else
         if (line==(Temp_Window->uly+Temp_Window->dheight+1))
         {
            /*This is the bottom Border of the window */
            width=Temp_Window->dwidth;
            tempx=Temp_Window->ulx;
            segment=(long)Temp_Window->border>>16;
            ofs=4+(width<<1)+(4*(Temp_Window->dheight+1))+(short)Temp_Window->border;
            if (tempx>=0)
            {
               Window_addresses[tempx-1][1]=segment;
               Window_addresses[tempx-1][0]=ofs;
            }
            ofs+=2;
            if (width>vwidth) width=vwidth-width;
            for (i=0;i<(width+1);i++)
            {
               if ((tempx+i)>=0)
               {
                  Window_addresses[tempx+i][1]=segment;
                  Window_addresses[tempx+i][0]=ofs;
               }
               ofs+=2;
            }
         }
      }
      Temp_Window=Temp_Window->next_window;
   }
}

void release(video_line_type far *line)
{
   video_line_type far *temp,far *next;
   if (!line)
      return;
   temp=line->next_line_seg;
   line->next_line_seg=NULL;
   while (temp)
   {
      next=temp->next_line_seg;
      temp->next_line_seg=NULL;
      Free(temp);
      temp=next;
   }
}

void display_window(window_type far *window,char present)
{
   /*This function links the specified window into the
      video display line data structure*/
   short i;
   short pos;
   short oldseg,count,oldofs;
   char step;
   char start,end;
   video_line_type far *temp_line;
   if (window&&!(window->opts&CLOSED))
   {
      if (present)
      {
         if (window->uly&&(window->opts&BORDER))
            start=-1;
         else
            start=0;
         end=window->dheight+1;
         if (window->opts&BORDER)
            end++;
         step=1;
      }
      else
      {
         if (window->previous_window)
            window->previous_window->next_window=window->next_window;
         else
            First_Window=window->next_window;
         start=window->dheight;
         if (window->opts&BORDER)
            start++;
         if (window->uly&&(window->opts&BORDER))
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
         gen_address_array(window->uly+i);
         release(video_lines[window->uly+i]);
         temp_line=video_lines[window->uly+i];
         count=0;
         oldofs=Window_addresses[0][0]-2;
         oldseg=Window_addresses[0][1];
         temp_line->window_addr=(short far*)*(long far*)&Window_addresses[0][0];
         for (pos=0;pos<vwidth;pos++)
         {
            if (Window_addresses[pos][1]==oldseg)
            {
               if (Window_addresses[pos][1])
                  if (Window_addresses[pos][0]==(oldofs+2))
                  {
                     oldofs++;oldofs++;count++;
                  }
                  else {}
               else
               {
                  oldseg=0;
               }
            }
            else
            {
                  {
                     temp_line->count=count;
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
                            (short far*)*(long far*)&Window_addresses[pos][0];
                     oldseg=Window_addresses[pos][1];
                     if (oldseg)
                     {
                        count++;
                     }
                     oldofs=Window_addresses[pos][0];
                  }
            }
         }
         if (count)
         {
            temp_line->count=count;
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
         if (*process_control&0x1)
             Relinquish(0L);

      }
      if (!present)
         if (window->previous_window)
            window->previous_window->next_window=window;
         else
            First_Window=window;
   }
   update_window(window);
}

void select_window(window_type far *window)
{
   /*Increment supremacy table, display window*/
   if (!window||(window->opts&CLOSED))
      return;
   if (Current_Window!=window)
   {
      draw_border(INACTIVE,Current_Window);
      update_window(Current_Window);
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
   }
   draw_border(ACTIVE,window);                   /*for iconification*/
   display_window(window,true);
}

void hide_window(window_type far *window)
{
   if (!window||(window->opts&CLOSED))
      return;
   if (First_Window==NULL)
   {
      First_Window=window;
      Current_Window=window;
   }
   else
   {
      if (First_Window==window)
      {
         draw_border(INACTIVE,window);        /*for iconification*/
      }
      else
      {
         if (Current_Window==window)
         {
            if (window->previous_window)
               Current_Window=window->previous_window;
            Current_Window->next_window=NULL;
            draw_border(INACTIVE,window);          /*for iconification*/
            draw_border(ACTIVE,Current_Window);
         }
         else
         {
            window->previous_window->next_window=window->next_window;
            window->next_window->previous_window=window->previous_window;
            draw_border(INACTIVE,window);        /*for iconification*/
         }
         window->next_window=First_Window;
         First_Window->previous_window=window;
         window->previous_window=NULL;
         First_Window=window;
         update_window(Current_Window);
      }
   }
   display_window(window,true);
}

public(void,bury_window,(void))
{
   Load_DS;
   if (First_Window!=Current_Window)
      hide_window(Current_Window);
   Restore_DS;
}

public(char,getattr,(window_type far *window))
{
   if (window&&!(window->opts&CLOSED))
      return(window->attr);
   else
      return(0);
}

void far void_mouse(short mousex,short mousey,char buttons)
{
}


public(window_type far *,dupdisplay,(window_type far *window,short opts))
{
   /*This routine duplicates a window so that we can either move it, or
      resize it, or whatever */
   window_type far *temp;
   char border_attr;
   short width,height;
   short i;
   Load_DS;
   if (!window||(window->opts&CLOSED))
   {
      asm pop ds;
      return(NULL);
   }
   temp=Allocate(sizeof(window_type));
   movmem(temp,window,sizeof(window_type));
   temp->status=0;
   temp->opts=opts;
   temp->mouse_handle=void_mouse;
   temp->previous_window=NULL;
   temp->next_window=NULL;

   if (!(temp->opts&NO_DATA))
   {
      temp->data=(short far *)Allocate(width*(height+1)<<1);
      for (i=0;i<width*height;i++)
         temp->data[i]=window->data[i];
   }
   else
      temp->data=NULL;

   if (temp->opts&BORDER)
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
   return(temp);
}

short icon_window(window_type far *window,short restore)
{
   if (!window||(window->opts&CLOSED)) return(false);
   if (restore)
   {
      if (!(window->opts&ICON))
         return(true);
      display_window(window,false);
      window->opts&=~ICON;
      window->dheight=window->oheight;
      window->dwidth=window->owidth;
      select_window(window);
   }
   else
   {
      if (window->opts&ICON)
         return(true);
      display_window(window,false);
      window->opts|=ICON;
      window->oheight=window->dheight;
      window->owidth=window->dwidth;
      window->dheight=-1;
      window->dwidth=10;
      window->uly=window->uly>=0?window->uly:0;
      window->ulx=window->ulx>=-10?window->ulx:-10;
      select_window(window);
   }
   return(true);
}

public(short,moddisplay,(window_type far *window,...))
{
   /*This routine is used to modify what a window's traits are.   Resizing
      and moving the the most important thereof*/
   char done=false;
   char status=true;
   short oldy;
   short savex,savey;
   short idx=0;
   Load_DS;
   while (!done&&status)
   {
      switch(...[idx++])
      {
         case WEND_MOD:
            done=true;
            break;
         case WMOVE_WINDOW:/*goto x,y*/
                   display_window(window,false);
                   savex=window->ulx;
                   savey=window->uly;
                   window->ulx=...[idx++];
                   window->uly=...[idx++];
                   if (window->ulx<-window->dwidth||window->ulx>vwidth||
                         window->uly<(-window->dheight-1)||window->uly>vheight)
                   {
                      window->ulx=savex;
                      window->uly=savey;
                      status=false;
                   }
                   display_window(window,true);
                   break;
         case WMOVE_MOUSE:/*modify mouse x,y*/
                   mouse=true;
                   mousex=...[idx++];
                   oldy=mousey;
                   mousey=...[idx++];
                   if (mousex<0)
                      mousex=0;
                   if (mousex>=vwidth)
                      mousex=vwidth-1;
                   if (mousey<0)
                      mousey=0;
                   if (mousey>=vheight)
                      mousey=vheight-1;
                   display_line(oldy);
                   display_line(mousey);
                   break;
         case WSELECT_WINDOW:select_window(window);
                   break;
         case WREMOVE_DATA:window->opts|=NO_DATA;
                   break;
         case WRESTORE_DATA:window->opts&=~NO_DATA;
                   break;
         case WICON_WINDOW:icon_window(window,0);
                   break;
         case WDELTA_MOVE_W:
                   display_window(window,false);
                   savex=window->ulx;
                   savey=window->uly;
                   window->ulx+=...[idx++];
                   window->uly+=...[idx++];
                   if (window->ulx<-window->dwidth||window->ulx>vwidth||
                         window->uly<(-window->dheight-1)||window->uly>vheight)
                   {
                      window->ulx=savex;
                      window->uly=savey;
                      status=false;
                   }
                   display_window(window,true);
                   break;

         case WDELTA_MOVE_M:/*modify mouse x,y*/
                   mouse=true;
                   savex=mousex;
                   savey=mousey;
                   mousex+=...[idx++];
                   oldy=mousey;
                   mousey+=...[idx++];
                   if (mousex<0||mousex>=vwidth||
                         mousey<0||mousey>=vheight)
                   {
                      mousex=savex;
                      mousey=savey;
                      status=false;
                   }
                   else
                   if (mouse)
                   {
                      display_line(oldy);
                      display_line(mousey);
                   }
                   break;
         case WCLOSE_WINDOW:display_window(window,false);
                   window->opts|=CLOSED;
                   Free(window->border);
                   Free(window->data);
                   window->border=0;
                   window->data=0;
                   if (window->next_window)
                      window->next_window->previous_window=window->previous_window;
                   else
                   {
                      Current_Window=window->previous_window;
                      draw_border(ACTIVE,Current_Window);
                      display_window(Current_Window,true);
                   }
                   if (window->previous_window)
                      window->previous_window->next_window=window->next_window;
                   else
                      First_Window=window->next_window;
                   break;
         case WRESIZEX:
                     display_window(window,false);
                     savex=window->dwidth;
                     window->dwidth+=...[idx++];
                     if ((window->dwidth<10)||
                           (window->dwidth>window->width))
                     {
                        window->dwidth=savex;
                        status=false;
                     }
                     if (window->xofs>(window->width-window->dwidth))
                        window->xofs=window->width-window->dwidth;
                     select_window(window); /*show the new window              */
                     break;
         case WRESIZEY:
                     display_window(window,false);
                     savey=window->dheight;
                     window->dheight+=...[idx++];
                     if ((window->dheight<4)||
                           (window->dheight>window->height))
                     {
                        window->dheight=savey;
                        status=false;
                     }
                     if (window->yofs>(window->height-window->dheight))
                        window->yofs=window->height-window->dheight;
                     select_window(window); /*show the new window              */
                     break;
         case WSCROLL_UD:/*change vertical scroll by a line*/
                     if (...[idx++])
                     {
                        window->yofs++;
                        if (window->yofs>window->height-window->dheight)
                        {
                           window->yofs--;
                           status=false;
                        }
                     }
                     else
                     {
                        if (window->yofs)
                           window->yofs--;
                        else
                           status=false;
                     }
                     if (status)
                        select_window(window);
                     break;
         case WSCROLL_LR:/*change Horizontal scroll by a line*/
                     if (...[idx++])
                     {
                        window->xofs++;
                        if (window->xofs>(window->width-window->dwidth))
                        {
                           window->xofs--;
                           status=false;
                        }
                     }
                     else
                     {
                        if (window->xofs)
                           window->xofs--;
                        else
                           status=false;
                     }
                     if (status)
                        select_window(window);
                     break;
         case WSCROLL_PUD:/*change vertical scroll by a page*/
                     if (...[idx++])
                     {
                        window->yofs+=window->dheight;
                        if (window->yofs>window->height-window->dheight)
                           window->yofs=window->height-window->dheight;
                     }
                     else
                     {
                        window->yofs-=window->dheight-1;
                        if (window->yofs<0)
                           window->yofs=0;
                     }
                     if (status)
                        select_window(window);
                     break;
         case WSCROLL_PLR:/*change Horizontal scroll by a page*/
                     if (...[idx++])
                     {
                        window->xofs+=window->dwidth-1;
                        if (window->xofs>(window->width-window->dwidth))
                           window->xofs=window->width-window->dwidth;
                     }
                     else
                     {
                        window->xofs-=window->dwidth;
                        if (window->xofs<0)
                           window->xofs=0;
                     }
                     if (status)
                        select_window(window);
                     break;
         case WRESTORE_WINDOW:icon_window(window,1);
                     break;
         case WHIDE_WINDOW:hide_window(window);
                     break;
         case WCHANGE_MHAND:
                     window->mouse_handle=(mhandproc)(*(long *)(...+idx));
                     idx+=2;
         default:
                     status=false;
                     break;
      }
   }
   asm pop ds;
   return(status);
}


public(short,getdisplay,(short far *x,short far *y,window_type far * far *ret))
{
   /*This routine returns a pointer to the window that was selected by the
      (x,y) point.   It will be the most 'ontop' window.
      The window will be return in the pointer to the pointer to the window
         variable in the other program.
      The function then returns more specifically what it found when the
         coordinate was picked.  Currently, this is either Data or Border.
      */
   window_type far *temp;
   window_type far *returnv=NULL;
   short lx=*x,ly=*y;
   short desc=0;
   Load_DS;
   if (lx==-1||ly==-1)
   {
      *x=mousex;
      *y=mousey;
      *ret=Current_Window;
   }
   else
   {
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
         if (temp->opts&BORDER)
         {
               if ((temp==Current_Window)&&
                     (lx==temp->ulx-1)&&                     /*return upper left corner,
                                                                            if active*/
                     (ly==temp->uly-1))
               {
                  returnv=temp;
                  desc=3;
               }
               else
               if ((lx==temp->ulx+temp->dwidth)&&   /*return upper right corner*/
                     (ly==temp->uly-1))
               {
                  returnv=temp;
                  if (temp->opts&ICON)
                     desc=14;
                  else
                     desc=4;
               }
               else
               if ((temp==Current_Window)&&
                     (lx==temp->ulx+temp->dwidth)&&    /*return lower right corner,
                                                                            if active*/
                     (ly==(temp->uly+temp->dheight+1)&&
                     !(temp->opts&ICON)))
               {
                  returnv=temp;
                  desc=5;
               }
               else
               if ((temp==Current_Window)&&            /*return on scroll bar, if active*/
                     (temp->dheight!=temp->height)&&
                     (lx== (temp->ulx+temp->dwidth)&&
                     !(temp->opts&ICON)))
               {
                  short testv;
                  if (ly==temp->uly)
                  {
                     /*we are on the top arrow*/
                     returnv=temp;
                        desc=6;
                  }
                  else
                  if (ly==(temp->uly+temp->dheight))
                  {
                     /*we are on the bottom arrow*/
                     returnv=temp;
                     desc=7;
                  }
                  else
                  {
   #ifdef DEBUG
                     displayln(output,"Y:%d  UL:%d  MARK: %d",ly,temp->uly,
                      (1+temp->uly+( temp->yofs *
                                                    (temp->dheight-2)+
                                                    (temp->height-temp->dheight)/2
                                                ) /
                                                    (temp->height-temp->dheight)
                               ));
   #endif
                  if ((ly>temp->uly)   &&
                        (ly< (testv=(1+temp->uly+( temp->yofs *
                                                    (temp->dheight-2)+
                                                    (temp->height-temp->dheight)/2) /
                                                    (temp->height-temp->dheight))) )
                      )
                  {
                      /*figure out if we are on the actual bar, but not the position
                         marker*/
                      returnv=temp;
                      desc=10;
                  }
                  else
                  if (ly>testv&&
                        ly<temp->uly+temp->dheight)
                  {
                     returnv=temp;
                     desc=11;
                  }
                  else
                  {
                     returnv=temp;
                     desc=14;
                  }
                  }
               }
               else
               if ((temp==Current_Window)&&
                     (temp->dwidth!=temp->width)&&
                     (ly==(temp->uly+temp->dheight+1)&&
                     !(temp->opts&ICON)))
               {
                  short testv;
   #ifdef DEBUG
                  displayln(output,"On Bar. %d %d %d Mark:%d",
                           lx,
                           temp->ulx,
                           (temp->ulx+temp->dwidth),
                           (temp->ulx+(temp->xofs*(temp->dwidth-2)+
                                                (temp->width-temp->dwidth)/2)/
                                              (temp->width-temp->dwidth)));
   #endif
                  if (lx==temp->ulx)
                  {
                     /*we are on the top arrow*/
                     returnv=temp;
                     desc=8;
                  }
                  else
                  if (lx==(temp->ulx+temp->dwidth-1))
                  {
                     /*we are on the bottom arrow*/
                     returnv=temp;
                     desc=9;
                  }
                  else
                  {
                  if (lx>temp->ulx&&
                        lx<(testv=(temp->ulx+(temp->xofs*(temp->dwidth-2)+
                                                (temp->width-temp->dwidth)/2)/
                                              (temp->width-temp->dwidth))))
                  {
                      /*figure out if we are on the actual bar, but not the position
                         marker*/
   #ifdef DEBUG
                      displayln(output,"left");
   #endif
                      returnv=temp;
                      desc=12;
                  }
                  else
                  if (lx>testv&&
                        lx<temp->ulx+temp->dwidth)
                  {
   #ifdef DEBUG
                      displayln(output,"Right");
   #endif
                     returnv=temp;
                     desc=13;
                  }
                  else
                  {
   #ifdef DEBUG
                      displayln(output,"OnH");
   #endif
                     returnv=temp;
                     desc=15;
                  }
                  }
               }
               else
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
   }
   asm pop ds;
   return(desc);
}

short add_link(module far *task,window_type far *window)
{
   task_link far *current;
   if (!window->links)
   {
      current=window->links=Allocate(sizeof(task_link));
      current->links_used=0;
      current->next=NULL;
   }
   else
      current=window->links;
   while (current->links_used==6&&current->next)
      current=current->next;
   if (current->links_used==6)
   {
      current->next=Allocate(sizeof(task_link));
      current=current->next;
      current->links_used=0;
      current->next=NULL;
   }
   current->TCB[current->links_used++]=task;
}

short remove_link(module far *task,window_type far *window)
{
   char cidx,pidx=-1;
   task_link far *current,far *prior;
   current=window->links;
   do
   {
      for (cidx=0;cidx<current->links_used;cidx++)
      {
         if (pidx>0)
         {
            prior->TCB[pidx++]=current->TCB[cidx];
            if (pidx>=6)
            {
               prior=prior->next;
               pidx=0;
            }
         }
         if (current->TCB[cidx]==task)
         {
            if (pidx<0)
            {
               pidx=cidx;
               prior=current;
            }
            else
               Exit(145);
         }
      }
      if (!current->next)
      {
         current->links_used--;
         if (!current->links_used)
         {
            if (window->links!=current)
            {
               prior=window->links;
               while (prior->next!=current) prior=prior->next;
               prior->next=NULL;
               Free(current);
            }
            else
            {
               Free(current);
               window->links=NULL;
            }
         }
      }
      current=current->next;
   }
   while (current);
   if (pidx<0)          /*we never moved the pidx, so we never had a match of
                                 TCB addresses.*/
      return(false);
   else
      return(true);
}

public(window_type far *,opendisplay,(short ulx,short uly,
                                        short width,short height,
                                        short opts,
                                        char data_attr,char border_attr,char cursor_attr,
                                        char far *title))
{
   window_type far *temp,far *t1;
   short i;
   short tlen,tpos;
   if (ulx<=0||uly<=0||(opts&CLOSED))
      return(NULL);
   Load_DS;
   temp=(window_type far *)Allocate(sizeof(window_type));
   if (!temp)
      return(NULL);
   temp->ulx=ulx-1;
   temp->uly=uly;
   temp->width=width;
   temp->height=height-1;
   temp->dwidth=width;
   temp->dheight=height-1;
   temp->owidth=width;
   temp->oheight=height-1;
   temp->xofs=0;
   temp->yofs=0;
   temp->opts=opts;
   temp->cursx=0;
   temp->cursy=0;
   temp->_cursx=0;
   temp->_cursy=0;
   temp->cursattr=cursor_attr;
   temp->attr=data_attr;
   temp->backattr=data_attr;
   temp->borattr=border_attr;
   temp->status=0;
   temp->mouse_handle=void_mouse;
   for (i=0;title[i]&&i<15;i++)
      temp->title[i]=title[i];
   temp->title[i]=0;

   if (!(temp->opts&NO_DATA))
   {
      temp->data=(short far *)Allocate(width*(height+1)<<1);
      for (i=0;i<width*height;i++)
         temp->data[i]=(short)temp->attr<<8;
   }
   else
      temp->data=NULL;
   if (!(temp->opts&NO_KEYS))
   {
      temp->keys=Allocate(sizeof(key_buffer)+KEY_BUF_SIZE);
      temp->keys->size=KEY_BUF_SIZE;
      temp->keys->head=0;
      temp->keys->tail=0;
   }

   if (temp->opts&BORDER)
   {
      temp->border=Allocate(2*(2*(width+height+2)));
   }
   else
      temp->border=0L;
   temp->previous_window=NULL;
   temp->next_window=NULL;
   temp->lines=NULL;
   temp->links=NULL;
   add_link(my_TCB,temp);

   select_window(temp);
   asm pop ds;
   return(temp);
}

public(short,closedisplay,(window_type far *window))
{
   window_type far *temp;
   Load_DS;
   temp=First_Window;
   while (temp&&temp!=window)
      temp=temp->next_window;
   if (temp)
   {
      if (temp->border)
         Free(temp->border);
      Free(temp->data);
      if (temp->lines)
         Free(temp->lines);
      display_window(window,false);
      if (window==Current_Window)
         Current_Window=window->previous_window;
      if (window==First_Window)
         First_Window=window->next_window;

      if (temp->previous_window)
         temp->previous_window->next_window=temp->next_window;

      if (temp->next_window)
         temp->next_window->previous_window=temp->previous_window;

      Free(temp);
   }
   else
      if (window&&(window->opts&CLOSED))
         Free(window);
      else
      {
         asm pop ds
         return(false);
      }
   asm pop ds;
   return(true);
}

void clr_video(void)
{
   char i;
   for (i=0;i<vheight;i++)
   {
      video_lines[i]=(video_line_type far *)Allocate(sizeof(video_line_type));
      video_lines[i]->count=vwidth;
      video_lines[i]->window_addr=blanks;
      video_lines[i]->next_line_seg=NULL;
   }
}

public(short,keypressed,(window_type far *window))
{
   if (!window) return(false);
   if (window->keys->head==window->keys->tail)
      return(false);
   else
      return(true);
}

public(char,readch,(window_type far *window))
{
   char return_v;
   key_buffer far *keys=window->keys;
   Load_DS;
   if (!window) return(0);
   while (keys->head==keys->tail)
      Relinquish(0L);
   return_v=keys->buffer[keys->tail];
   keys->tail++;
   if (keys->tail>=keys->size)
      keys->tail=0;
   Restore_DS;
   return(return_v);
}

cleanup(void,video_exit,(void))
{
   Load_DS;
   restore_cursor();
   Restore_DS;
}

short check_Kpres=1000;
short check_Gch=1000;

char far keypressed_check(void)
{
   if (Locate("rawkeypressed"))
   {
      remote_keybrd=true;
      _rawkeypressed=rawkeypressed;
      return(_rawkeypressed());
   }
   else
   {
      check_Kpres--;
      if (!check_Kpres)
         _rawkeypressed=kbhit;
      return(kbhit());
   }
}


char far Getch_check(void)
{
   while (!_rawkeypressed()) Relinquish(0L);
   if (Locate("rawreadch"))
   {
      remote_keybrd=true;
      _rawreadch=rawreadch;
      return(_rawreadch());
   }
   else
   {
      check_Gch--;
      if (!check_Gch)
         _rawreadch=getch;
      return(getch());
   }
}


/*fork hook... this is a hook into the OS so that when a task forks,
   all windows that it owns are linked to the TCB of his child/brother/whatever.
*/
void far fork_hook(module far *current_TCB,module far *new_TCB)
{
   window_type far *current;
   task_link far *link;
   short idx;
   asm mov ax,0xCAFE;
   Load_DS;
   current=First_Window;
   while (current)
   {
      link=current->links;
      do
      {
         for (idx=0;idx<link->links_used;idx++)
            if (link->TCB[idx]==current_TCB)
               add_link(new_TCB,current);
         link=link->next;
      }
      while (link);
      current=current->next_window;
   }
   Restore_DS;
}

/*destroy hook... this is a hook so that all children tasks will disown the
   windows they claim to have. if all tasks owning the window are no longer,
   then the window is closed.*/
void far destroy_hook(module far *current_TCB)
{
   window_type far *current;
   task_link far *link;
   short idx;
   asm mov ax,0xCAFE;
   Load_DS;
   current=First_Window;
   while (current)
   {
      link=current->links;
      do
      {
         for (idx=0;idx<link->links_used;idx++)
            if (link->TCB[idx]==current_TCB)
            {
               remove_link(current_TCB,current);
               if (!current->links)
                  closedisplay(current);
            }
         link=link->next;
      }
      while (link);
      current=current->next_window;
   }
   Restore_DS;
}


/*disown... this procedure will take a specified and let the task disown it...
   then when he dies, his window won't die, because he doesn't own it.   Owner-
   ship is transferred to the video module.*/


public(short,disowndisplay,(window_type far *window))
{
   Load_DS;
   if (remove_link(my_TCB,window))
      add_link(Video_TCB,window);
   Restore_DS;
}

void main()
{
   short i;
   char far *txt;
   txt=Get_environ("VIDEO");
   if (txt)
   {
      screenseg=atoi(txt);
      if (screenseg==0) goto load_null_video;
      switch(screenseg)
      {
         case 0xb000:mono=true;
                           break;
         default:mono=false;
                     break;
      }
      screen_seg=(long)screenseg<<16;
      kill_cursor();
   }
   else
   {
load_null_video:
      spawn("nullvid.com",50,"\0",1,my_TCB->Priority);
      perish();
   }
   vwidth=*(unsigned short far *)0x0000044aL;
   if (vheight==0)
   {
      vheight=(((*(unsigned short far *)0x0000044cL))/(vwidth*2));
      if (vheight>25) vheight--;
   }
   if (vwidth>132)
      vwidth=132;
   Video_TCB=my_TCB;
   hook_vectors();
   _disowndisplay();
   _video_exit();
   _keypressed();
   _readch();
   _displayln();
   _display();
   _position();
   _clr_display();
   _setattr();
   _getattr();
   _opendisplay();
   _closedisplay();
   _getdisplay();
   _moddisplay();
   _dupdisplay();
   _get_xy();
   _bury_window();
   video_lines=Allocate(vheight<<2);
   scanlines=Allocate(vheight*2);
   for (i=0;i<vheight;i++)
      scanlines[i]=(vwidth<<1)*i;
   for (i=0;i<vwidth;i++)
      blanks[i]=0x1100+'°';
   mousex=0;
   mousey=0;
   mouse=false;
   clr_video();
   show_screen();
/* displayln(opendisplay(1,1,10,10,0,0x1f,0x1f,0x2f,"Status"),"%d %d",vheight,vwidth);*/
#ifdef DEBUG
   output=opendisplay(50,3,28,10,BORDER|NEWLINE|NO_KEYS,2,3,0x14,"Video Test");
#endif
   while (1)
   {
      char character;

      character=_rawreadch();
      if (Current_Window&&!(Current_Window->opts&NO_KEYS))
         {
            key_buffer far *keys;
            short room;
            keys=Current_Window->keys;                   /*get closer pointer*/
            keys->buffer[keys->head]=character;    /*get the keys*/
            if (!remote_keybrd&&!keys->buffer[keys->head])
            {
alt_key:
               switch(character=_rawreadch())
               {
                  case 49:bury_window();
                              break;
                  case 45:Exit(10);
                              break;
                  default:
                     room=keys->tail-keys->head;                     /*compute how much buffer
                                                                                    space is left*/
                     if (room<=0)                                          /*if there is a wrap*/
                        room+=keys->size;                               /*then add size*/
                     if (room>1)                                           /*if there is room*/
                     {
                        keys->head++;                                      /*then update the head pointer*/
                        if (keys->head>=keys->size) keys->head=0;
                     }
                     keys->buffer[keys->head]=character;
                     goto add_head;
               }
            }
            else
            {
add_head:
               room=keys->tail-keys->head;                     /*compute how much buffer
                                                                              space is left*/
               if (room<=0)                                          /*if there is a wrap*/
                  room+=keys->size;                               /*then add size*/
               if (room>1)                                           /*if there is room*/
               {
                  keys->head++;                                      /*then update the head pointer*/
                  if (keys->head>=keys->size) keys->head=0;
               }
            }
         }
      else
         if (!character)
            goto alt_key;
      Relinquish(0L);
   }

}


