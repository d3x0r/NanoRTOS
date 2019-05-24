.model small,c

;_COM segment byte public 'CODE'
;assume cs:_COM,ds:_COM,ss:_COM

extrn display_line:proc
extrn show_cursor:far
extrn beep:proc
extrn scroll_data:proc

INCLUDE WINDOW.INC

.code
;public(void,display,(window_type far *window,unsigned char character))
;{
;  /*This routine puts the character at current cursor location
;    within specified window, then updates the cursor for that window*/
;  unsigned char tempx,tempy;
;  char liney;
;  char i;
;  asm push ds
;  asm push cs
;  asm pop ds
;  if (window->data&&window)
;  {
;    tempx=window->cursx;
;    tempy=window->cursy;
;    liney=window->uly+tempy;
;    if (character<32)
;    {
;      if ((window->opts&2)==0)
;        switch (character)
;        {
;          case  0:
;            if (window->cursattr)
;              show_cursor(window);
;            display_line(window->uly+tempy);
;            break;
;          case  7:
;            beep();
;            break;
;          case  8:
;            window->status&=0xfeff;
;            {
;              window->cursx--;
;              back_scrolled=false;
;              if (window->cursx>window->dwidth)
;              {
;                if (window->cursy)
;                {
;                  window->cursx=window->dwidth-1;
;                  back_scrolled=true;
;                  window->cursy--;
;                }
;                else
;                  window->cursx=0;
;              }
;
;              if (window->cursattr)
;                show_cursor(window);
;            }
;            break;
;          case 9:                /*Tab*/
;            break;
;          case 10:
;            if (window->status&0x100)
;            {
;              window->status&=0xfeff;
;              break;
;            }
;            display_line(liney);
;            window->cursy++;
;            if (window->cursy>window->dheight)
;            {
;              if (window->opts&NO_SCROLL)
;              {
;                display_line(window->cursy+window->uly-1);
;                window->cursy=0;
;              }
;              else
;              {
;                scroll_data(window);
;                window->cursy--;
;                if (window->cursattr&&window->_cursy)
;                  window->_cursy--;
;              }
;              tempy=window->uly;
;              for (i=0;i<window->height;i++)
;                display_line(tempy+i);
;            }
;            break;
;          case 13:
;            window->cursx=0;
;            break;
;          default:
;            goto show_character;
;        }
;    }
;    else
;    {
;      window->status&=0xfeff;
;  show_character:
;      tempx=window->cursx;
;      tempy=window->cursy;
;      {
;        *(window->data+(tempx)+(tempy*window->dwidth))=
;             (short)(window->attr<<8)+character;
;        window->backattr=window->attr;
;      }
;      window->cursx++;
;      if (window->cursx>=window->dwidth)
;      {
;        window->status|=0x100;
;        window->cursx=0;
;        window->cursy++;
;        if (window->cursy>window->dheight)
;        {
;          if ( window->opts&NO_SCROLL)
;          {
;            display_line(window->cursy+window->uly-1);
;            window->cursy=0;
;          }
;          else
;          {
;            scroll_data(window);
;            window->cursy--;
;            if (window->cursattr&&window->_cursy)
;               window->_cursy--;
;          }
;          tempy=window->uly;
;          for (i=0;i<window->height;i++)
;            display_line(tempy+i);
;        }
;        else
;          display_line(window->uly+window->cursy-1);
;      }
;    }
;  }
;  asm pop ds
;}
;

escape_entry_table      dw      code00
                        dw      code01
                        dw      code02
                        dw      code03
                        dw      code04
                        dw      code05
                        dw      code06
                        dw      code07
                        dw      code08
                        dw      code09
                        dw      code0a
                        dw      code0b
                        dw      code0c
                        dw      code0d
                        dw      code0e
                        dw      code0f
                        dw      code10
                        dw      code11
                        dw      code12
                        dw      code13
                        dw      code14
                        dw      code15
                        dw      code16
                        dw      code17
                        dw      code18
                        dw      code19
                        dw      code1a
                        dw      code1b
                        dw      code1c
                        dw      code1d
                        dw      code1e
                        dw      code1f

extrn back_scrolled:byte

public display

display proc far,window:dword,char:byte
;
;   10   character
;   8    win far
;   6    win near
;   4    far
;   2    near
;   bp - bp
        les     bx,dword ptr window             ;get pointer to window
        mov     ax,es
        or      ax,bx
        jnz     continue1;                       ;if NULL return
        ret
continue1:
        push    ds
        push    di
        lds     di,dword ptr es:[bx].data       ;get pointer to data
        mov     ax,ds
        or      ax,di
        jnz     continue2;                       ;if NULL return
return_now1:
        pop     di
        pop     ds
        ret
continue2:
        mov     ax,es:[bx].opts                 ;get the options
        test    ax,8000h                        ;check to see if closed
        jnz     return_now1                     ;get out if so.
        mov     al,char                      ;set up character in ax
        mov     ah,es:[bx].attr                 ;  with attribute

        cmp     al,20h                          ;check to see if it is
        jb      escape_code                     ;  a escape code or not
        jmp     normal_ascii
escape_code:
        test    es:[bx].opts,2                  ;test to see if we are supposed
        jz      do_code                         ;to act on escape codes or not
        jmp     skip_do
do_code:
        push    si
        mov     cl,al
        xor     ch,ch
        shl     cx,1
        add     cx,offset escape_entry_table
        mov     si,cx
        jmp     cs:[si]
code00:
        test    es:[bx].opts,4
        jne     no_cursor1
        push    es
        push    bx
        call    show_cursor
        pop     bx
        pop     es
        jmp     skip_do
no_cursor1:
        mov     dx,es:[bx].uly
        add     dx,es:[bx].cursy
        mov     ax,es:[bx].dheight
        add     ax,es:[bx].yofs
        cmp     ax,es:[bx].cursy
        jl      NO_LINE1
        push    dx
        call    display_line
        pop     dx
NO_LINE1:
        jmp     skip_do;
code07:
        call    beep
        jmp     skip_do;
code08:
        and     es:[bx].status,0feffh   ;clear fact that is wrapped by line
                                        ;overrun
        mov     dx,es:[bx].cursx        ;get    x cursor
        dec     dx                      ;subtract one
        cmp     es:[bx].width,dx        ;check to see if above width
        jnb     x_okay                  ;if not then didn't wrap, x okay
        mov     cs:back_scrolled,0     ;otherwise, we go to prior line
        cmp     es:[bx].cursy,0         ;check to see if prior line avail
        je      y_at_top
        dec     es:[bx].cursy           ;if there is, back up
        mov     dx,es:[bx].width;       ;get new x
        dec     dx                      ;subtract one to fit
        mov     cs:back_scrolled,1     ;mark that we backed up one line
y_at_top:
        mov     dx,0                    ;otherwise x sits at left margin
x_okay:
        mov     es:[bx].cursx,dx        ;store x
        test    es:[bx].opts,4          ;check to see if there is a cursor
        jne     no_cursor2
        push    es                      ;if so, show it
        push    bx
        call    show_cursor
        add     sp,4
no_cursor2:
        jmp     skip_do
code0a:
        test    es:[bx].status,100h     ;check to see if overrun
        jz      do_linefeed             ;if not, do normal linefeed
        and     es:[bx].status,0feffh   ;otherwise clear overrun
        jmp     skip_do                 ;we're done!
do_linefeed:
        mov     dx,es:[bx].uly          ;get window y
        mov     cx,es:[bx].cursy        ;get cursor y
        add     dx,cx                   ;compute absolute y
        push    dx                      ;push absolute y
        inc     cx                      ;add one to the current y
        test    es:[bx].opts,0400h      ;check to see if newline is supposed
                                        ;       to do a carriage return too.
        jz      no_carriage
        mov     es:[bx].cursx,0         ;if it does, then zero the x.
no_carriage:
        cmp     cx,es:[bx].height       ;check to see if it is valid
        mov     es:[bx].cursy,cx        ;store y
        jng     not_scrolled            ;jump if valid
        test    es:[bx].opts,NO_SCROLL  ;test for NO_SCROLL window
        jz      do_scroll               ;if !NO_SCROLL scroll
        mov     es:[bx].cursy,0         ;otherwize go to top of window
        jmp     not_scrolled            ;get me outa here
do_scroll:
        dec     es:[bx].cursy           ;get rid of auto increment
        cmp     es:[bx]._cursy,0        ;check to see if lasy y exists
        je      dont_upd_lasty
        dec     es:[bx]._cursy          ;if so, decrement it
dont_upd_lasty:
        push    es                      ;scroll the data
        push    bx                      ; NOTE: at this point, we should not
        call    scroll_data            ; have stored y and it should not have
        pop     bx                      ; moved either becuase it is at the
        pop     es                      ; bottom of the data window

        mov     dx,es:[bx].uly          ;get the window y
        mov     cx,es:[bx].dheight      ;get the window height
        cmp     cx,0
        jge     do_show
        pop     dx                      ;get rid of last line to show from stack
        jmp     skip_do
do_show:
        push    cx                      ;save our counter, becuase display_line
                                        ;wont
        push    dx                      ;push absolute y
        call    display_line           ;show this line
        pop     dx                      ;clean stack
        pop     cx                      ;retore counter
        inc     dx                      ;go to next line
        loop    do_show                 ;show the window after it was
                                        ;scrolled
        pop     dx                      ;get rid of line to show
        jmp     skip_do                 ; and leave, (already done)
not_scrolled:
        sub     cx,es:[bx].yofs
        cmp     cx,es:[bx].dheight      ;check to see if we can show line
        jge     clean_stack1            ;if not, then leave
        call    display_line           ;show the currnet line
clean_stack1:
        pop     dx                      ;clean stack
        jmp     skip_do                 ;leave

code0d:
        mov     es:[bx].cursx,0;
        jmp     skip_do

code01:
code02:
code03:
code04:
code05:
code06:
code09:
code0b:
code0c:
code0e:
code0f:
code10:
code11:
code12:
code13:
code14:
code15:
code16:
code17:
code18:
code19:
code1a:
code1b:
code1c:
code1d:
code1e:
code1f:
        pop     si
        jmp     show_normally
skip_do:
        pop     si
        jmp     q_return                ;redunant, I know, but provides future
                                        ;expansion

normal_ascii:
        and     es:[bx].status,0feffh   ;clear overrun
show_normally:
        mov     cx,ax                   ;save character
        mov     ax,es:[bx].cursy        ;get y
        mov     dx,es:[bx].width        ;get width
        mul     dx                      ;compute y*width
        mov     dx,es:[bx].cursx        ;get x pos
        add     ax,dx                   ;compute y*width + x
        add     di,ax                   ;add computation to begin of data
        shl     di,1                    ;shift for word address
        mov     ds:[di],cx              ;store character on data
        mov     al,es:[bx].attr;        ;update attrib behind cursor
        mov     es:[bx].backattr,al
        inc     dx                      ;update x
        cmp     es:[bx].width ,dx       ;test x for EOL
        jle     overflow
        mov     es:[bx].cursx,dx        ;if on line, update x, return
        jmp     q_return
overflow:
        or      es:[bx].status,0100h    ;mark that we overflowed
        mov     es:[bx].cursx,0         ;zero x
        mov     dx,es:[bx].cursy        ;get y
        inc     dx                      ;add 1 to y
        cmp     es:[bx].height ,dx      ;check y with height
        jl      test_scroll             ;if greater than scroll data
        mov     es:[bx].cursy,dx        ;otherwise update y and show last line
                                        ; and return;
        dec     dx                      ;go back to last line
        sub     dx,es:[bx].yofs         ;move region
        cmp     dx,es:[bx].dheight      ;test to see if line is visable
        jg      dont_show
        add     dx,es:[bx].yofs         ;restore cursor line value
        add     dx,es:[bx].uly          ;add window y to get absolute y
        push    dx
        call    display_line           ;display last line
        pop     dx
dont_show:
        jmp     q_return
test_scroll:
        test    es:[bx].opts,NO_SCROLL  ;check to see if we are supposed to
                                        ;scroll
        jz      scroll_window
        dec     dx                      ;dx is y, now go back to last line
        sub     dx,es:[bx].yofs
        cmp     dx,es:[bx].dheight
        jg      no_display
        add     dx,es:[bx].yofs
        add     dx,es:[bx].uly          ;compute absolute y
        push    dx
        call    display_line           ;show last line
        pop     dx
no_display:
        mov     es:[bx].cursy,0         ;zero cursor pos;
        jmp     q_return
scroll_window:
        cmp     es:[bx]._cursy,0
        je      dont_update_lasty
        dec     es:[bx]._cursy
dont_update_lasty:
        push    es
        push    bx
        call    scroll_data
        pop     bx
        pop     es
        mov     dx,es:[bx].uly
        mov     cx,es:[bx].dheight
        cmp     cx,0
        jl      q_return;
show_window:
        push    cx
        push    dx
        call    display_line
        pop     dx
        pop     cx
        inc     dx
        loop    show_window
q_return:
        pop     di
        pop     ds
        ret
display endp


;_COM ends


END








