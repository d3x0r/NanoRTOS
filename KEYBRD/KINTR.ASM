.model small,C
;_COM segment byte public 'CODE'
;assume cs:_COM,ds:_COM,ss:_COM

INCLUDE OS.EQU

public schead
public sctail
public kbdch

public old_key
public beep
public keyintr
public biostrap

.DATA
old_key dd 0
kbdch db 50 dup(0)
schead db 0
sctail db 0
extrn keytask:dword

.CODE
beep proc near
;
;
;
;  value    bx+2
;  near     bx
        mov ax,0b6h
        out 043h,al
        mov ax,051h
        out 042h,al
        mov ax,2
        out 042h,al

        in al,061h
        or al,3
        out 061h,al

        xor cx,cx
pause1:
        loop pause1

        and al,0fch
        out 61h,al

        ret
beep endp

biostrap proc near
;
; Entrance Conditions-
;       ah 0    Read Key
;       ah 1    Scan Key
;       ah 2    get shift status
;
; Exit Conditions-
;
;
biostrap endp

keyintr Proc Near
        push    ax
        push    bx
        push    cx
        push    ds
        in      al,60h
        push    ax

;   this calls old keyboard routine
;        pushf
;        call    dword ptr cs:[_old_key]
;   this clears the keyboard myslef
        in      al,61h
        mov     ah,al
        or      al,80h
        out     61h,al
        mov     al,ah
        out     61h,al

        mov     ax,@data
        mov     ds,ax
        pop     ax;
        mov     bl,schead      ;put head in bl
        inc     bl                 ;add 1
        cmp     bl,50              ;test to see to wrap
        jne     dont_wrap
        mov     bl,0
dont_wrap:
        cmp     bl,sctail      ;compare head vs tail
        jne     do_store           ;if not equal, then store
        push    0
        call    beep
        add     sp,2
        jmp     clear_intr
do_store:
        mov     bh,schead      ;get old head
        mov     schead,bl      ;save new head
        xchg    bh,bl              ;swap old and new
        xor     bh,bh              ;clear new, so we are storing at old
        mov     [kbdch+bx],al
clear_intr:
; this clears the harware interrupt myself
        mov     al,20h
        out     20h,al
; this gets rid of what bios did to me.
;        mov     ah,1         ;clear keybuffer of old keys
;        int     16h
;        jz      leave_kintr
;        mov     ah,0
;        int     16h
;        jmp     clear_intr
leave_kintr:
        lds     bx,keytask
        mov     word ptr ds:[bx].status,0
        mov     word ptr ds:[bx+2].status,0
        pop     ds
        pop     cx
        pop     bx
        pop     ax
        iret
keyintr  endp

END

