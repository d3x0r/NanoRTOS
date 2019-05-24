.model small,C
INCLUDE OS.EQU

.code
public C ldiv
public C lmult
public C lmod

.data
public C externals
public C Request
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
public C delay

ORG 0
externals       dd 0
ORG 0
Request         dd 0
Exit            dd 0
Relinquish      dd 0
Terminate       dd 0
Export          dd 0
Import          dd 0
device_name     dd 0
Wake            dd 0
spawn_batch     dd 0
spawn           dd 0
Get_environ     dd 0
Allocate        dd 0
Free            dd 0
Inquire_begin   dd 0
process_control dd 0
Connect_Int     dd 0
Disconnect_Int  dd 0
gettime         dd 0
change_priority dd 0
my_tcbptr       dd 0
swap_to         dd 0
delay           dd 0

.code
public C dataseg
dataseg         dw 0

extrn C main:proc

start_point:
        mov     di,offset externals
        mov     ax,seg externals
        mov     es,ax
        mov     ah,8
        int     60h
        mov     ax,seg externals
        mov     ds,ax
        mov     cs:dataseg,ds
        les     bx,my_tcbptr  ; load pointer to current_module(*TCB)
        les     bx,es:[bx]    ; load pointer to TCB
        push    word ptr es:[bx+2].load_path
        push    word ptr es:[bx].load_path
        push    1
        call    main
        mov     ah,5     ;do a perish... not a exit...
        int     60h
        call    perish
        mov     ax,1     ;if perish failed, exit
        push    ax
        call    dword ptr Exit
        mov     ah,4Ch   ;if exit failed, really exit!
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
fork proc C near,relation:byte
        mov     ah,3
        mov     al,relation

        int     60h
        ret
fork   endp

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


END start_point

