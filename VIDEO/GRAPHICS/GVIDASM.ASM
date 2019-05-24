
_COM segment byte public 'CODE' USE16
assume cs:_COM,ds:_COM,ss:_COM

video STRUC
        count           dw  0
        gpage           db  0
        window_addr     dd  0
        next_line_seg   dd  0
video ENDS

extrn _x:byte
extrn _y:byte
extrn _displayln:near
extrn _scanlines:word
extrn _video_lines:word
extrn _screenseg:word
extrn _vheight:word
extrn _mousex:byte
extrn _mousey:byte
extrn _mouse:byte

public _getch;
public _kbhit;
public _display_line


_kbhit proc near
  mov ah,1
  int 16h
  jnz nochar
  xor ax,ax
  jmp endproc
nochar:
  mov ax,0ffffh
endproc:
  retf
_kbhit endp

hold db  0

_getch proc near
        cmp     cs:hold,0
        je      wait1
        mov     al,cs:hold
        mov     cs:hold,0
        jmp     leave_get
wait1:
        mov     ah,1
        int     16h
        jz      wait1
        mov     ah,0
        int     16h
        cmp     al,0
        mov     cs:hold,0
        jnz     leave_get
        mov     cs:hold,ah
leave_get:
    retf
_getch endp

color_table     db      70h
                db      60h
                db      40h
                db      50h
                db      20h
                db      30h
                db      10h
                db      00h

opage db -1

achange_page MACRO tpage
      mov    cs:opage,tpage
      mov    ah,tpage
      shl    ah,1
      and    ah,1Eh
      mov    dx,01CEh
      cli
      mov    al,0b2h
      out    dx,al
      inc    dx
      in     al,dx
      and    al,0e1h
      or     ah,al
      mov    al,0b2h
      dec    dx
      out    dx,ax
      sti
ENDM


change_page MACRO tpage
        mov     cs:opage,tpage
        mov     al,tpage
        xor     ah,ah
        mov     cl,4
        ror     ax,cl
        mov     al,9
        mov     dx,3ceh
        out     dx,ax
ENDM


_display_line proc near
;
; +4    line
; +2    near
; -0    bp
;
        int     3
        push    bp
        mov     bp,sp
        push    di
show_line:
        mov     bx,[bp+4]
        cmp     bx,0
        jl      contin_exit;
        cmp     bx,cs:_vheight
        jge     contin_exit;
        mov     dx,bx           ;mov y to dx
        shl     bx,1;
        mov     di,cs:[bx+_scanlines]
;        mov     di,0
        shl     bx,1
        les     bx,dword ptr cs:[_video_lines+bx]
        jmp     test_line
display_line_seg:
;        mov     dl,es:[bx].gpage
;        cmp     cs:opage,dl
;        je      do_line
;        change_page     dl
do_line:
        mov     cx,es:[bx].count
        cmp     cx,0
        je      contin_exit
;        mov     ah,0ch;
        push    es
        push    ds

        mov     ax,cs:_screenseg
        lds     si,dword ptr es:[bx].window_addr
do_points:
;        lodsb
;        xchg    di,cx
;        int     10h
;        xchg    di,cx
;        inc     di
;        loop    do_points
;        test     cl,1
;        jz      not_odd
;        movsb
;not_odd:
;        shr     cl,1
        mov     es,ax
        rep     movsb
        pop     ds
        pop     es
        les     bx,es:[bx].next_line_seg

test_line:
        mov     ax,es
        or      ax,bx
        jz      exit1
        jmp     display_line_seg                ;else go to display;
exit1:
contin_exit:
        pop     di
        pop     bp
        ret
_display_line endp


_COM ends


END

