.model SMALL,c

public  inport
public  outport
public  connect_irqs
public  disconnect_irqs

.data

extrn   IRQ4   :dword
extrn   IRQ3   :dword
extrn   Exit   :dword

.code

;typedef struct buffer
;{
;  short Head,Tail,Size;
;  char far *Buffer;
;} buffer;
;
;typedef struct com_socket
;{
;  buffer In,Out;
;  short IOPort;
;  unsigned short Baud_Divisor;
;  char Line_Control;
;  char Modem_Control;
;  char Line_Status;
;  char Modem_Status;
;  char control;
;  struct com_socket far *next;


comsocket STRUC
        InHead          dw      ?
        InTail          dw      ?
        InSize          dw      ?
        InBuffer        dd      ?
        OutHead         dw      ?
        OutTail         dw      ?
        OutSize         dw      ?
        OutBuffer       dd      ?
        Port            dw      ?
        Baud            dw      ?
        Line_Control    db      ?
        Modem_Control   db      ?
        Line_Status     db      ?
        Modem_Status    db      ?
        control         db      ?
        Next            dd      ?
comsocket ENDS



outport proc near
;
;    6   data
;    4   port number
;    2   near
;    bp- bp
        push    bp
        mov     bp,sp
        mov     dx,[bp+4]
        mov     al,[bp+6]
        out     dx,al
        pop     bp
        ret
outport endp

inport proc near
;
;    4   port number
;    2   near
;    bp- bp
        push    bp
        mov     bp,sp
        mov     dx,[bp+4]
        in      al,dx
        pop     bp
        ret
inport endp

handle  dw      Modem_Change
        dw      None
        dw      Transmit_Empty
        dw      Dunno
        dw      Data_Avail
        dw      Dunno
        dw      Line_Change
        dw      Dunno

oldIRQ3 dd      ?
oldIRQ4 dd      ?

connect_irqs proc near
        mov     ax,0
        mov     es,ax

        mov     di,2ch

        cli
        mov     ax,es:[di]
        mov     cs:word ptr oldIRQ3,ax
        mov     ax,es:[di+2]
        mov     cs:word ptr oldIRQ3+2,ax

        mov     di,30h
        mov     ax,es:[di]
        mov     cs:word ptr oldIRQ4,ax
        mov     ax,es:[di+2]
        mov     cs:word ptr oldIRQ4+2,ax

        mov     di,2ch
        mov     ax,offset handle_irq3
        mov     es:[di],ax
        mov     ax,cs
        mov     es:[di+2],ax
        mov     di,30h
        mov     ax,offset handle_irq4
        mov     es:[di],ax
        mov     ax,cs
        mov     es:[di+2],ax
        in      al,21h
        and     al,0e7h;
        out     21h,al
        sti
        ret

;        mov     ax,250bh
;        int     21h
;        mov     cs:word ptr oldIRQ3,dx
;        mov     cs:word ptr oldIRQ3+2,es
;
;        mov     ax,250ch
;        int     21h
;        mov     cs:word ptr oldIRQ4,dx
;        mov     cs:word ptr oldIRQ4+2,es
;
;        mov     ax,350bh
;        mov     dx,offset handle_irq3
;        mov     bx,cs
;        mov     ds,bx
;        int     21h
;
;        mov     ax,350ch
;        mov     dx,offset handle_irq4
;        mov     bx,cs
;        mov     ds,bx
;        int     21h

connect_irqs endp

disconnect_irqs proc far
        mov     ax,0
        mov     es,ax
        mov     di,2ch

        cli
        mov     ax,word ptr cs:oldIRQ3
        mov     es:[di],ax
        mov     ax,word ptr cs:oldIRQ3+2
        mov     es:[di+2],ax
        mov     di,30h
        mov     ax,word ptr cs:oldIRQ4
        mov     es:[di],ax
        mov     ax,word ptr cs:oldIRQ4+2
        mov     es:[di+2],ax
        sti
        ret
disconnect_irqs endp


handle_ints proc near
handle_irq3 label word
        push    di
        push    es
        push    ax
        push    bx
        push    ds
        mov     ax,@data
        mov     ds,ax
        mov     bx,0
        les     di,IRQ3
        mov     ax,es
        cmp     ax,0
        jne     common
        pop     ds
        pop     bx
        pop     ax
        pop     es
        pop     di
        jmp     cs:oldIRQ3
handle_irq4 label word
        push    di
        push    es
        push    ax
        push    bx
        push    ds
        mov     ax,@data
        mov     ds,ax
        mov     bx,1
        les     di,IRQ4
        mov     ax,es
        cmp     ax,0
        jne     common
        pop     ds
        pop     bx
        pop     ax
        pop     es
        pop     di
        jmp     cs:oldIRQ4
common:
        push    cx
        push    dx
        push    si
        push    bx
test_pending:
        mov     dx,es:[di].Port         ;get the base port
        add     dx,2                    ;go to the IRQ ident register
        in      al,dx                   ;get the IRQ status
        mov     bl,al
        xor     bh,bh
        shl     bx,1
        jmp     cs:handle[bx]

Modem_Change:
        add     dx,4
        in      al,dx
        mov     es:[di].Modem_Status,al
        jmp     test_pending
Transmit_Empty:
        lds     bx,es:[di].OutBuffer
        mov     ax,es:[di].OutTail
        cmp     ax,es:[di].OutHead
        je      test_pending
        add     bx,ax
        mov     al,[bx]
        sub     dx,2
        out     dx,al
        mov     ax,es:[di].OutTail
        inc     ax
        cmp     ax,es:[di].OutSize
        jl      StoreOutTail
        mov     ax,0
StoreOutTail:
        mov     es:[di].OutTail,ax
        jmp     test_pending
Data_Avail:
        sub     dx,2
        in      al,dx
        lds     bx,es:[di].InBuffer
        add     bx,es:[di].InHead
        mov     [bx],al
        mov     ax,es:[di].InHead
        inc     ax
        cmp     ax,es:[di].InSize
        jl      StoreInHead
        mov     ax,0
StoreInHead:
        mov     es:[di].InHead,ax
        jmp     test_pending
Line_Change:
        add     dx,3
        in      al,dx
        mov     es:[di].Line_Status,al
        jmp     test_pending
None:
        les     di,es:[di].Next
        mov     ax,es
        cmp     ax,0
        je      EOI
        jmp     test_pending
Dunno:
        mov     al,20h
        out     20h,al
        push    500
        call    Exit
EOI:

;        pop     bx
;        pop     si
;        pop     dx
;        pop     cx
;        pop     bx
;        pop     ax
;        pop     es
;        pop     di
;        iret

        pop     bx
        cmp     bx,1
        je      call_irq4

        pop     si
        pop     dx
        pop     cx
        pop     ds
        pop     bx
        mov     al,20h
        out     20h,al
        pop     ax
        pop     es
        pop     di
;        jmp     cs:oldIRQ3
        iret
call_irq4:
        pop     si
        pop     dx
        pop     cx
        pop     ds
        pop     bx
        mov     al,20h
        out     20h,al
        pop     ax
        pop     es
        pop     di
;        jmp     cs:oldIRQ4
        iret
handle_ints endp

END


