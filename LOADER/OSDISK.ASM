.MODEL HUGE,C
.286
extrn swap_module:proc
extrn Connect_int:proc
extrn Disconnect_int:proc
extrn beep:proc
extrn cprint:proc

.CODE
  public connect_stall
  public disconnect_stall
  public disk_stall
  public getdisk
  public setdisk
  public open
  public create
  public close
  public read
  public write
  public rewind
  public seek
  public sizefind
  public gets


.DATA
  public errorcode

errorcode dw 0

.CODE

getdisk proc far
  mov ah,19h
  int 21h
  xor ah,ah
  ret
getdisk endp

setdisk proc far
;
;
;   6  disk
;   4  far
;   2  near
;   bp-bp
  push bp
  mov bp,sp
  mov ah,0eh
  mov al,[bp+6]
  int 21h
  pop bp
  ret
setdisk endp



;
;  10  attrib
;  8   filename far
;  6   filename near
;  4   far
;  2   near
;  bp- bp

create proc far
          push bp
          mov bp,sp
          push ds
          mov   ah,3ch            ;set create
          mov   cx,[bp+10]        ;set optins
          jmp   do_open_create
open proc far
  push bp
  mov bp,sp
  push ds
  mov al,[bp+10]         ;set options
  mov ah,3dh             ;set open
do_open_create:

  lds dx,[bp+6]          ;set filename
  int 21h
  jnc noopenerror
  push ds
  mov ax,@data
  mov ds,ax
  mov errorcode,ax
  pop ds
  mov ax,-1
noopenerror:
  pop ds
  pop bp
  ret
open endp
create endp

close proc far
  push bp
  mov bp,sp
  mov bx,[bp+6]
  mov ah,3eh
  int 21h
  jnc nocloseerror
  push ds
  mov ax,@data
  mov ds,ax
  mov errorcode,ax
  pop ds
  mov ax,-1
nocloseerror:
  pop bp
  ret
close endp

read proc far
  mov ah,3fh
  jmp do_readwrite
write proc far
  mov ah,40h
;
;
;
;   12  short token
;   10  short length
;   8   buffer far
;   6   buffer near
;   4   far
;   2   near
;   bp- bp
do_readwrite:
  push bp
  mov bp,sp
  push ds
  lds dx,[bp+6]
  mov cx,[bp+10]
  mov bx,[bp+12]
  int 21h
  jnc return_now
  push ds
  mov ax,@data
  mov ds,ax
  mov errorcode,ax
  pop ds
  mov ax,-1
  jmp return_now
return_now:
  pop ds
  pop bp
  ret
write endp
read endp

rewind proc far
  push bp
  mov bp,sp
  mov bx,[bp+6]
  mov cx,0
  mov dx,0
  mov ax,4200h
  int 21h
  pop bp
  ret
rewind endp

seek proc far,handle:word,fileoffset:dword,type:byte
  mov bx,handle
  mov dx,word ptr fileoffset
  mov cx,word ptr fileoffset+2
  mov al,type
  mov ah,42h
  int 21h
  ret
seek endp

sizefind proc far,handle:word
  mov bx,handle
  mov cx,0
  mov dx,0
  mov al,1
  mov ah,42h
  int 21h
  mov bx,handle
  push ax
  push dx
  mov dx,0
  mov cx,0
  mov al,2
  mov ah,42h
  int 21h
  pop cx
  mov bx,dx
  pop dx
  push bx
  push ax
  mov al,0
  mov ah,42h
  mov bx,handle
  int 21h
  pop ax
  pop dx
  ret
sizefind endp

gets proc far
;
;   12  handle
;   10  length
;   8   far buffer
;   6   near buffer
;   4   far
;   2   near
;   bp- bp
  push bp
  mov bp,sp
  push di
  push ds
  mov ax,@data
  mov ds,ax
  mov word ptr errorcode,0
  pop ds
  les bx,[bp+6]
  mov ax,[bp+12]
  xor di,di
  push ax
  push 1
  push es
getsloop:
  push bx
  call read
  pop bx
  push ax
  push ds
  mov ax,@data
  mov ds,ax
  cmp errorcode,0
  pop ds
  pop ax
  jnz leavegets
  cmp ax,1
  jne leavegets
  cmp byte ptr es:[bx],10
  je end1
keep_going:
  inc bx
  inc di
  cmp di,[bp+10]
  jl getsloop
  jmp leavegets
end1:
  cmp byte ptr es:[bx-1],13
  jne keep_going

  mov byte ptr es:[bx-1],10
  mov byte ptr es:[bx],0
leavegets:
  add sp,6
  mov ax,di
  pop di
  pop bp
  ret
gets endp

IO_BUSY dw 0
INT15   dd 0
rel_call dd 0

disk_stall proc
        cmp     ah,91h
        jne     teststall
        mov     cl,al
        xor     ch,ch
;        add     cl,4
        cmp     cl,2
        jb      dev_ok
        clc
        mov     ah,0
        jmp     sreturn
dev_ok:
        mov     bx,1
        shl     bx,cl
        not     bx
        and     CS:IO_BUSY,bx
        mov     ah,0
        stc
        jmp     sreturn
teststall:
        cmp     ah,90h
        jne     chain_next
        mov     cl,al
        xor     ch,ch
;        add     cl,4
        cmp     cl,2
        jnb     sreturn
        mov     bx,1
        shl     bx,cl
        push    ax
        push    bx
        push    cx
        push    dx
        push    es
        or      cs:IO_BUSY,bx
        sti
        push    0
        push    bx
testforstall:
        pop     bx
        pop     cx
        test    cs:IO_BUSY,bx
        jz      return
        inc     cx
        cmp     cx,0
        je      return
        push    bx
        push    cx
        xor     ax,ax
        push    ax
        push    ax
        call    swap_module
        add     sp,4
        jmp     testforstall
return:
        pop     es
        pop     dx
        pop     cx
        pop     bx
        pop     ax
        mov     ah,0
        stc
sreturn:
        iret

chain_next:
        jmp     dword ptr cs:INT15
endp

connect_stall proc

        ret
        push    15h
        push    CS
        push    offset disk_stall
        call    Connect_int
        add     sp,6
        ret
endp

disconnect_stall proc
        ret
        push    15h
        push    CS
        push    offset disk_stall
        call    Disconnect_int
        add     sp,6

        ret
        cli
        mov     ax,0
        mov     es,ax
        mov     ax,word ptr cs:INT15
        mov     es:[15h*4],ax
        mov     ax,word ptr cs:INT15+2
        mov     es:[15h*4+2],ax
        sti
        ret
endp

END




