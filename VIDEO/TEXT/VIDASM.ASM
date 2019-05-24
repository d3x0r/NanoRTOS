.model small,c

;_COM segment byte public 'CODE' USE16
;assume cs:_COM,ds:_COM,ss:_COM

video STRUC
        count           dw  0
        window_addr     dd  0
        next_line_seg   dd  0
        video_addr      dd  0
video ENDS

window STRUC
        ulx             dw  ?
        uly             dw  ?    ;upper left corner of window
        width           dw  ?
        height          dw  ?    ;height and width of dataplane of window
        dwidth          dw  ?
        dheight         dw  ?    ;height and width of display portion
        owidth          dw  ?
        oheight         dw  ?    ;save of dwidth,dheight for deiconing
        xofs            dw  ?
        yofs            dw  ?    ;offset of upper left corner of data plane
        cursx           dw  ?
        cursy           dw  ?    ;coords of cursor in the window
        _cursx          dw  ?
        _cursy          dw  ?    ;last coords of cursor
        status          dw  ?    ;current status of window ...??????
        opts            dw  ?    ;options for the window.
        key_buffer      dd  ?    ;keyboard buffer structure
        cursattr        db  ?
        borattr         db  ?
        backattr        db  ?    ;current attributes of various items.
        data            dd  ?    ;address of the dataplane
        border          dd  ?    ;address of border data
        shadow          dd  ?    ;address of shadow data
        title           db  16 dup(?) ; title of the window
        next_window     dd  ?
        prev_window     dd  ?    ;pointers to the next and prior windows in list
        mouse_handle    dd  ?    ;pointer to mouse handling procedure
        lines           dd  ?    ;array of pointers to lines to show on screen
window ENDS

extrn fork_hook:far
extrn destroy_hook:far

.data

extrn x:byte
extrn y:byte
extrn Relinquish:dword
extrn scanlines:dword
extrn video_lines:dword
extrn screenseg:word
extrn vheight:word
extrn vwidth:word
extrn mousex:word
extrn mousey:word
extrn mouse:byte
extrn Current_Window:dword

.code

public getch;
public kbhit;
public display_line
public hook_vectors

hook_vectors proc far
        mov di,seg destroy_hook
        mov cx,offset destroy_hook
        mov si,seg fork_hook
        mov dx,offset fork_hook
        mov ah,9
        int 60h
        ret
endp


kbhit proc far
;   8  win far
;   6  win near
;   4  far
;   2  near
;  bp- bp
;
        push    bp
        mov     bp,sp
        cmp     cs:hold,0
        jne     gotchar
        mov     ah,1
        int     16h
        jnz     gotchar
nochar:
        xor     ax,ax
        jmp     endproc
gotchar:
        mov     ax,0ffffh
endproc:
        pop     bp
        ret
kbhit endp

hold db  0

getch proc far
;
;  8  far win
;  6  near win
;  4  far ret
;  2  near ret
; bp- bp
        push    bp
        mov     bp,sp
        cmp     cs:hold,0
        je      wait1
        mov     al,cs:hold
        mov     cs:hold,0
        jmp     leave_get
wait1:
        mov     ah,1
        int     16h
        jnz     continget
        xor     ax,ax
        push    ax
        push    ax
        call    Relinquish
        add     sp,4d
        jmp     wait1
continget:
        mov     ah,0
        int     16h
        cmp     al,0
        mov     cs:hold,0
        jnz     leave_get
        mov     cs:hold,ah
leave_get:
        mov     ah,0
        pop     bp
        ret
getch endp

color_table     db      70h
                db      60h
                db      40h
                db      50h
                db      20h
                db      30h
                db      10h
                db      00h
                db      70h
                db      60h
                db      40h
                db      50h
                db      20h
                db      30h
                db      10h
                db      00h

mono_table      db      70h
                db      70h
                db      70h
                db      70h
                db      70h
                db      70h
                db      70h
                db      00h
                db      70h
                db      70h
                db      70h
                db      70h
                db      70h
                db      70h
                db      70h
                db      00h

display_line proc,line:word
; +6,+8 Window pointer
; +4    line
; +2    near
; -0    bp
;
        push    ds
        push    di
        mov     ax,seg screenseg
        mov     ds,ax
        mov     bx,line
        cmp     bx,0
        jnl     testheight;
        jmp     contin_exit
testheight:
        cmp     bx,vheight
        jnge    do_line;
        jmp     contin_exit
do_line:
        shl     bx,1;
        les     di,scanlines
        mov     di,es:[bx+di]
        shl     bx,1
        push    di
        les     di,video_lines
        les     bx,es:[di+bx]
        pop     di
        jmp     test_line
display_line_seg:

        mov     cl,byte ptr es:[bx].count
        xor     ch,ch;
        push    es
        push    ds
        mov     ax,screenseg
        lds     si,dword ptr es:[bx].window_addr
        mov     es,ax
        rep     movsw
        pop     ds
        pop     es
        les     bx,es:[bx].next_line_seg

test_line:
        mov     ax,es
        or      ax,bx
        jz      exit1
        jmp     display_line_seg                ;else go to display;
exit1:
        cmp     mouse,0
        je      contin_exit
        mov     bx,line
        cmp     bx,mousey
        jne     contin_exit
        mov     ax,vwidth
        mul     bx
        add     ax,mousex
        shl     ax,1
        inc     ax
        mov     di,ax
        mov     es,screenseg
        mov     bl,es:[di]
        mov     al,bl
        and     al,0fh
        xor     bh,bh
        mov     cl,4
        shr     bx,cl
        cmp     screenseg,0b000h
        je      mono
        mov     bl,cs:color_table+bx
        jmp     contin
mono:
        mov     bl,cs:mono_table+bx
contin:
        or      al,bl
        mov     es:[di],al
contin_exit:
        pop     di
        pop     ds
        ret
display_line endp




END

