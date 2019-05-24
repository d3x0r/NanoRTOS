_COM segment byte public 'CODE'
assume cs:_COM,ds:_COM,ss:_COM




;.model large,C

extrn C main:near
;extrn main:far
public C ldiv
public C lmult
public C lmod

public C externals
public C Exit
public C Relinquish
public C Terminate
public C Export
public C Import
public C device_name   ; a far pointer to the text name of the device
public C Wake
public C spawn_batch
public C spawn
public C Get_environ
public C Allocate
public C Free
public C perish
public C fork
public C destory
public C Inquire_begin
public C process_control
public C Connect_Int
public C Disconnect_Int
public C gettime
public C change_priority
public C my_tcbptr
public C swap_to

externals      label dword
Request        label dword
ORG 4
Exit           label dword
ORG 8
Relinquish     label dword
ORG 12
Terminate      label dword
ORG 16
Export         label dword
ORG 20
Import         label dword
ORG 24
device_name    label dword
ORG 28
Wake           label dword
ORG 32
spawn_batch    label dword
ORG 36
spawn          label dword
ORG 40
Get_environ    label dword
ORG 44
Allocate       label dword
ORG 48
Free           label dword
ORG 52
Inquire_begin  label dword
ORG 56
process_control label dword
ORG 60
Connect_Int    label dword
ORG 64
Disconnect_Int label dword
ORG 68
gettime        label dword
ORG 72
change_priority label dword
ORG 76
my_tcbptr      label dword
ORG 80
swap_to        label dword


ORG 100h

start_point:
;        mov     ax,seg externals
;        mov     ds,ax
        call    main
        mov     ax,1
        push    ax
        call    dword ptr Exit
        mov     ah,4Ch
        int     21h
perish proc near
        mov     ah,5
        int     60h
        ret
perish endp
destory proc near
        mov     ah,6
        int     60h
        ret
destory endp
fork proc near
        mov     ah,3
        int     60h
        ret
fork   endp

do_request proc far
;
;
;          far address of name
;          near address of name
;          far return
;     [sp] near return
        push ss:[bx+6]
        push ss:[bx+4]
        call far [Request]
        pop cx
        pop cx
        mov es,ss:[bx+2]
        mov bx,ss:[bx]
        sub bx,4
        mov es:[bx],ax
        mov es:[bx+2],dx
        ret
do_request endp

ldiv proc C near,num1:dword,num2:dword
        .386
        mov     eax,num1
        mov     ebx,num2
        xor     edx,edx
        div     ebx
        .8086
        ret
endp
lmod proc C near,num1:dword,num2:dword
        .386
        mov     eax,num1
        mov     ebx,num2
        xor     edx,edx
        div     ebx
        mov     eax,edx
        .8086
        ret
endp
lmult proc C near,num1:dword,num2:dword
        .386
        mov     eax,num1
        mov     ebx,num2
        mul    ebx
        .8086
        ret
endp
_COM ends


END start_point

