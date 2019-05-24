.model small,c
.386

public outportb
public inportb
public WDCard_int

INCLUDE \common.ipc\os.equ

.data

extrn input_TCB:dword
extrn output_TCB:dword
extrn process_control:dword
extrn WDinterupt:byte
extrn WDreset_port:byte
extrn IO_base:word

.code

outportb proc near
;
;    6   data
;    4   port number
;    2   near
;    bp- bp
    push bp
    mov bp,sp
    mov dx,[bp+4]
    mov al,[bp+6]
    out dx,al
    pop bp
    ret
outportb endp

inportb proc near
;
;    4   port number
;    2   near
;    bp- bp
    push bp
    mov bp,sp
    mov dx,[bp+4]
    in al,dx
    pop bp
    ret
inportb endp

oldint dd 0
saveint dd 0

WDCard_int proc far
        push    ds
        push    es
        push    di
        push    dx
        push    ax
        mov     ax,@data
        mov     ds,ax
        les     di,process_control
        or      word ptr es:[di],1
        les     di,input_TCB
        mov     ax,es
        or      ax,di
        jz      not_set
        mov     word ptr es:[di].status,0
        mov     word ptr es:[di+2].status,0
not_set:
        les     di,output_TCB
        mov     ax,es
        or      ax,di
        jz      not_set2
        mov     word ptr es:[di].status,0
        mov     word ptr es:[di+2].status,0
not_set2:
        mov     dx,IO_base
        add     dx,10h                  ;go to command regs
        in      al,dx                   ;get command for the page
        mov     ah,al
        mov     al,0                    ;select page 0
        out     dx,al                   ;go there
        add     dx,7
        in      al,dx
        or      WDinterupt,al
        out     dx,al
        sub     dx,7
        mov     al,ah
        out     dx,al
        mov     dl,WDreset_port
        xor     dh,dh
        mov     al,20h
        out     dx,al
        out     20h,al
        pop     ax
        pop     dx
        pop     di
        pop     es
        pop     ds
        iret
endp


END



