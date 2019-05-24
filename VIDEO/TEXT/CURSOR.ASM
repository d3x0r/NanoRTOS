_COM segment byte public 'CODE'
assume cs:_COM,ds:_COM,ss:_COM

extrn _display_line:near
public  _show_cursor

;void show_cursor(window_type far *window)
;{
;      char temp_attr;
;      unsigned char tempx,tempy;
;      short temp_offset;
;      short far *temp_ptr;
;      char oldy;
;      if (!window||!window->data) return;
;      tempx=window->_cursx;
;      tempy=window->_cursy;
;      oldy=tempy+window->uly;
;      temp_offset=tempx+(tempy*window->dwidth);
;      temp_ptr=window->data+temp_offset;
;      if (*temp_ptr>>8==window->cursattr)
;      {
;        *(temp_ptr)=
;             (*(temp_ptr)&0xff)+
;             ((short)(window->backattr)<<8);
;        display_line(window->uly+tempy);
;      }
;      tempx=window->cursx;
;      tempy=window->cursy;
;      temp_offset=tempx+(tempy*window->dwidth);
;      temp_ptr=window->data+temp_offset;
;      window->backattr=*((char far *)(temp_ptr)+1);
;      if ((window->backattr&0x7)==
;         ((window->backattr&0x70)>>4))
;      {
;        temp_attr=window->cursattr&0x70|((window->cursattr&0x70)>>4);
;        *(temp_ptr)=
;             (*(temp_ptr)&0xff)+
;                  ((short)(temp_attr)<<8);
;      }
;      else
;        *(temp_ptr)=
;             (*(temp_ptr)&0xff)+
;                  ((short)(window->cursattr)<<8);
;      if (back_scrolled)
;        display_line(oldy);
;      display_line(window->uly+tempy);
;      window->_cursx=window->cursx;
;      window->_cursy=window->cursy;
;}

INCLUDE WINDOW.INC

_show_cursor proc near
;
;   10
;   6    win far
;   4    win near
;   2    near
;   bp - bp
        push    bp
        mov     bp,sp
        les     bx,dword ptr [bp+4]             ;get pointer to window
        mov     ax,es
        or      ax,bx
        jnz     continue1;                       ;if NULL return
        pop     bp
        ret
continue1:
        push    ds
        push    di
        lds     di,dword ptr es:[bx].data       ;get pointer to data
        mov     ax,ds
        or      ax,di
        jnz     continue2;                       ;if NULL return
        pop     di
        pop     ds
        pop     bp
        ret
continue2:
        int     3
        mov     ax,es:[bx].dwidth
        mov     dx,es:[bx]._cursy
        mul     dx
        add     ax,es:[bx]._cursx
        add     ax,di
        shl     ax,1
        inc     ax
        xchg    ax,di
        mov     cl,ds:[di]
        cmp     cl,es:[bx].cursattr
        jne     no_previous_cursor
        mov     cl,es:[bx].backattr
        mov     ds:[di],cl
no_previous_cursor:
        xchg    ax,di
        mov     ax,es:[bx].dwidth
        mov     dx,es:[bx].cursy
        mul     dx
        add     ax,es:[bx].cursx
        add     di,ax
        shl     di,1
        inc     di
        mov     cl,ds:[di]
        mov     es:[bx].backattr,cl
        mov     cl,es:[bx].cursattr
        mov     ds:[di],cl
        mov     dx,es:[bx]._cursy
        cmp     dx,es:[bx].cursy
        mov     cx,1
        je      one_only
        inc     cx
        add     dx,es:[bx].uly
        push    dx
one_only:
        mov     dx,es:[bx].cursy
        add     dx,es:[bx].uly
        push    dx
do_lines:
        pop     dx
        push    cx
        push    dx
        call    _display_line
        pop     dx
        pop     cx
        loop    do_lines

        pop     di
        pop     ds
        pop     bp
        ret
_show_cursor endp

_COM ends


END


