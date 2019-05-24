.MODEL HUGE,C
INCLUDE  OS.EQU
extrn swap_module

.DATA
  public ticks
  ticks dd  0
  extrn current_module:dword
  extrn timed:byte
  extrn process_control:word
  extrn OS_module:dword
  extrn force_interval:word
  extrn force_time:word

.CODE
  public atoi
  public itoa
;  public ltoa
  public atol
  public strlen
  public strcpy
  public strcat
  public strncpy
  public strncat
  public strchr
  public strnicmp
  public stricmp
  public movmem
  public memset
  public print
  public cprint
  public nextparam
  public gettime;
  public ldiv
  public lmod
  public lmult
  public time_interupt
  public compute_CRC16_asm
  public compute_CCITT_asm

.DATA
can_table db ' ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789:.$'
hex_table db '0123456789ABCDEF'



.CODE

lmult proc far,num1:dword,num2:dword
  .386
  mov eax,num
  mov ebx,divisor
  xor edx,edx
  mul ebx
  mov edx,eax
  shr edx,16
  ret

  .8086
endp

ldiv proc far,num:dword,divisor:dword
  .386
  mov eax,num
  mov ebx,divisor
  xor edx,edx
  div ebx
  mov edx,eax
  shr edx,16
  ret
  .8086
endp

lmod proc far,num:dword,divisor:dword
  .386
  mov eax,num
  mov ebx,divisor
  xor edx,edx
  div ebx
  mov eax,edx
  shr edx,16
  ret
  .8086
endp

gettime proc far,time_struc:dword
        push    ds
        mov     ax,@data
        mov     ds,ax
        les     di,time_struc
        mov     bx,word ptr [ticks]
        mov     es:[di],bx
        mov     bx,word ptr [ticks+2]
        mov     es:[di+2],bx
        pop     ds
        ret
gettime endp

INT_count dw 0

Old_time        dd      0
Swap_address    dd      0
time_interupt proc far
        push    ds
        push    ax
        push    es                              ;save regs
        push    di
        mov     ax,@data
        mov     ds,ax
        cmp     byte ptr timed,0
        jne     norm_inc
        jmp     large_inc
norm_inc:
        add     word ptr ticks,1
        adc     word ptr ticks+2,0
        add     word ptr force_time,1
        mov     ax,force_interval;
        cmp     force_time,ax;
        jl      no_force1;
        mov     force_time,0
        or      process_control,1
no_force1:

        test    process_control,2
        jz      inc_tasktime
        mov     di,offset OS_module
        add     word ptr [di].Acum_Time  ,1
        adc     word ptr [di].Acum_Time+2,0
        jmp     check_int
inc_tasktime:

        les     di,current_module               ;get who is now running
        add     word ptr es:[di].Acum_Time  ,1  ;add 1 to time run
        adc     word ptr es:[di].Acum_Time+2,0
        mov     ax,ss                           ;get current stack seg
        cmp     word ptr es:[di+2].pstack,ax    ;compare vs allocated stack
        jne     restore_pointer                 ;if not same, don't bother check
        mov     ax,es:[di].ssize                ;get stack's size
        sub     ax,sp                           ;subtrace the current pointer
        cmp     es:[di].maxsize,ax              ;test to see if larger than
        jae     restore_pointer                 ;  previous size
        mov     es:[di].maxsize,ax              ;store size of stack
        cmp     es:[di].ssize,ax
        jnb     restore_pointer
        int 3
        mov     word ptr es:[di].status,1
        mov     word ptr es:[di+2].status,0
        mov     di,sp
        add     di,8
        mov     word ptr ss:[di],offset swap_module
        mov     word ptr ss:[di+2],seg swap_module
restore_pointer:
        jmp     check_int
large_inc:
        add     word ptr ticks,54
        adc     word ptr ticks+2,0
        add     word ptr force_time,54
        mov     ax,force_interval;
        cmp     force_time,ax;
        jl      no_force2;
        or      process_control,1
        mov     force_time,0
no_force2:

        test    process_control,2
        jz      lginc_tasktime
        mov     di,offset OS_module
        add     word ptr [di].Acum_Time  ,54
        adc     word ptr [di].Acum_Time+2,0
        jmp     do_int

lginc_tasktime:

        les     di,current_module               ;get who is now running
        add     word ptr es:[di].Acum_Time  ,54 ;add 1 to time run
        adc     word ptr es:[di].Acum_Time+2,0
        mov     ax,ss                           ;get current stack seg
        cmp     word ptr es:[di+2].pstack,ax    ;compare vs allocated stack
        jne     lgrestore_pointer               ;if not same, don't bother check
        mov     ax,es:[di].ssize                ;get stack's size
        sub     ax,sp                           ;subtrace the current pointer
        cmp     es:[di].maxsize,ax              ;test to see if larger than
        jae     lgrestore_pointer                 ;  previous size
        mov     es:[di].maxsize,ax              ;store size of stack
        cmp     es:[di].ssize,ax
        jnb     lgrestore_pointer
        int 3
        mov     word ptr es:[di].status,1
        mov     word ptr es:[di+2].status,0
        mov     di,sp
        add     di,8
        mov     word ptr ss:[di],offset swap_module
        mov     word ptr ss:[di+2],seg swap_module
lgrestore_pointer:
        jmp     do_int
check_int:
        inc     cs:INT_count
        cmp     cs:INT_count,55
        jl      return_now
do_int:
        mov     cs:INT_count,0
        pop     di
        pop     es
        pop     ax
        pop     ds
        jmp     cs:Old_time
return_now:
        mov     al,20h
        out     20h,al
        pop     di
        pop     es
        pop     ax
        pop     ds
        iret
endp


nextparam proc far,str:dword
  push ds
  push di
  mov bx,@data
  mov ds,bx
  mov bx,offset can_table
  xor ax,ax
  les di,str

  pop di
  pop ds
  ret
nextparam endp

atoi proc far
atol proc far
;
;   8   far ptr
;   6   near ptr
;   4   far
;   2   near
;   bp- bp
  push bp
  mov bp,sp
  push ds
  push di
  push si
  lds si,[bp+6]
  xor ax,ax
  xor dx,dx
  mov bx,@data
  mov es,bx
  cmp byte ptr [si],'#'
  jne test_can
  inc si
  jmp base16
test_can:
  cmp byte ptr [si],'@'
  jne base10
  inc si
  mov bp,40
  mov cx,3
  mov bx,offset can_table
  jmp start_loop
base16:
  mov bp,16
  mov bx,offset hex_table
  mov cx,100
  jmp start_loop
base10:
  mov bp,10
  mov bx,offset hex_table
  mov cx,100
  jmp start_loop
start_loop:
testnext:
  push cx
  mov cx,bp
  mov di,bx
  push bx
  push dx
  push ax
  mov al,[si]
  cmp al,60h
  jl dotest
  xor al,20h
dotest:
  repnz scasb
  jnz exitatoi
  sub di,bx
  dec di
  mov cx,bp
  pop ax
  mul cx
  mov bx,dx
  pop dx
  push bx
  push ax
  mov  ax,dx
  mul  cx
  mov  dx,ax
  pop  ax
  pop  bx
  add  dx,bx
  add ax,di
  adc dx,0
  inc si
  pop bx
  pop cx
  loop testnext
  jmp restore_extern
exitatoi:
  pop ax
  pop dx
  pop bx
  pop cx
restore_extern:
  pop si
  pop di
  pop ds
  pop bp
  ret
atol endp
atoi endp

strlen proc far
;
;   8   far ptr
;   6   near ptr
;   4   far
;   2   near
;   bp- bp
;
  push bp
  mov bp,sp
  push di
  push es
  mov al,0
  mov cx,7fffh
  les di,[bp+6]
  repnz scasb
  or cx,8000h
  neg cx
  sub cx,2
  mov ax,cx
  pop es
  pop di
  pop bp
  ret
strlen endp

strchr proc far
;
;   10 character
;   8  far ptr
;   6  near ptr
;   4  far
;   2  near
;   bp-bp
  push bp
  mov bp,sp
  push di
  les di,[bp+6]
  push es
  push di
  call strlen
  pop di
  pop es
  mov al,[bp+10]
  repnz scasb
  dec di
  cmp byte ptr es:[di],al
  jne null
  mov dx,es
  mov ax,di
  jmp exit1
null:
  xor ax,ax
  xor dx,dx
exit1:
  pop di
  pop bp
  ret
strchr endp

strcpy proc far
;
;  12 source far ptr
;  10 source near ptr
;  8  dest far ptr
;  6  dest near ptr
;  4  far
;  2  near
;  bp-bp
  push bp
  mov bp,sp
  push ds
  push si
  push di
  lds si,[bp+10]
  les di,[bp+6]
  push ds
  push si
  call strlen
  pop si
  pop ds
  inc cx
  rep movsb
  pop di
  pop si
  pop ds
  pop bp
  ret
strcpy endp

strcat proc far
;
;  12 source far ptr
;  10 source near ptr
;  8  dest far ptr
;  6  dest near ptr
;  4  far
;  2  near
;  bp-bp
  push bp
  mov bp,sp
  push ds
  push si
  push di
  lds si,[bp+10]
  les di,[bp+6]
  push es
  push di
  call strlen
  pop di
  pop es
  add di,ax
  push ds
  push si
  call strlen
  pop si
  pop ds
  inc cx
  rep movsb
  pop di
  pop si
  pop ds
  pop bp
  ret
strcat endp

strncpy proc far
;  14 maxlen
;  12 source far ptr
;  10 source near ptr
;  8  dest far ptr
;  6  dest near ptr
;  4  far
;  2  near
;  bp-bp
  push bp
  mov bp,sp
  push ds
  push si
  push di
  lds si,[bp+10]
  les di,[bp+6]
  push ds
  push si
  call strlen
  pop si
  pop ds
  inc cx
  cmp cx,[bp+14]
  jle domove1
  mov cx,[bp+14]
domove1:
  mov ax,cx
  rep movsb
  pop di
  pop si
  pop ds
  pop bp
  ret
strncpy endp

strncat proc far
;  14 max length
;  12 source far ptr
;  10 source near ptr
;  8  dest far ptr
;  6  dest near ptr
;  4  far
;  2  near
;  bp-bp
  push bp
  mov bp,sp
  push ds
  push si
  push di
  lds si,[bp+10]
  les di,[bp+6]
  push es
  push di
  call strlen
  pop di
  pop es
  add di,ax
  push ds
  push si
  call strlen
  pop si
  pop ds
  inc cx
  cmp cx,[bp+14]
  jle domove
  mov cx,[bp+14]
domove:
  rep movsb
  pop di
  pop si
  pop ds
  pop bp
  ret
strncat endp

strnicmp proc far
;  14  count
;  12  S2 far ptr
;  10  S2 near ptr
;  8   S1 far ptr
;  6   S1 near ptr
;  4   far
;  2   near
;  bp- bp
  push bp
  mov bp,sp
  push ds
  push si
  push di
  lds si,[bp+6]
  les di,[bp+10]
  mov cx,[bp+14]
  xor bh,bh
  xor ah,ah
top_compare:
  lodsb
  cmp al,0
  jne testlower
  mov bl,es:[di]
  jmp exitstrn
testlower:
  cmp al,'a'
  jl tests2foralpha
  cmp al,'z'
  jg tests2foralpha
  xor al,20h
tests2foralpha:
  mov bl,es:[di]
  inc di
  cmp bl,0
  je exitstrn
  cmp bl,'a'
  jl cmpasis
  cmp bl,'z'
  jg cmpasis
  xor bl,20h
cmpasis:
  cmp al,bl
  loopz top_compare
exitstrn:
  sub ax,bx
  pop di
  pop si
  pop ds
  pop bp
  ret
strnicmp endp

stricmp proc far,str1:dword,str2:dword
;  12  S2 far ptr
;  10  S2 near ptr
;  8   S1 far ptr
;  6   S1 near ptr
;  4   far
;  2   near
;  bp- bp
  push ds
  push si
  push di
  lds si,str1
  les di,str2
  xor bh,bh
  xor ah,ah
@@top_compare:
  lodsb
  cmp al,0
  jne @@testlower
  mov bl,es:[di]
  jmp @@exitstrn
@@testlower:
  cmp al,'a'
  jl @@tests2foralpha
  cmp al,'z'
  jg @@tests2foralpha
  xor al,20h
@@tests2foralpha:
  mov bl,es:[di]
  inc di
  cmp bl,0
  je @@exitstrn
  cmp bl,'a'
  jl @@cmpasis
  cmp bl,'z'
  jg @@cmpasis
  xor bl,20h
@@cmpasis:
  cmp al,bl
  jz @@top_compare
@@exitstrn:
  sub ax,bx
  pop di
  pop si
  pop ds
  ret
stricmp endp

movmem proc far,source:dword,dest:dword,count:word
  push ds
  push es
  push si
  push di
  mov cx,count
  mov bx,cx
  lds si,source
  les di,dest
  and bl,1
  jz testwordodd
  movsb
testwordodd:
;  shr bx,1
;  and bl,1
;  jz dodword
;  movsw
;  shr cx,1
dodword:
  shr cx,1
  rep movsw
  pop di
  pop si
  pop es
  pop ds
  ret
movmem endp

memset proc far,source:dword,value:word,length:word
  push es
  push di
  mov cx,length
  mov ax,value
  les di,source
  test cl,1
  jz testwordoddset
  stosb
testwordoddset:
  shr cx,1
  rep stosw
  pop di
  pop es
  ret
memset endp

print proc far,string:dword
  push ds
  push si
  lds si,string
  mov ah,14
  mov bx,1fh
top:
  lodsb
  cmp al,0
  je exitprint
  int 10h
  jmp top
exitprint:
  pop si
  pop ds
  ret
print endp

cprint proc far,char:byte
  mov al,char
  mov ah,14
  mov bx,1fh
  int 10h
  ret
cprint endp

itoa proc far
;
; +10  far text
; +8   near text
; +6   num
; +4   far
; +2   near
;  bp- bp
        push    bp                         ;save stack
        mov     bp,sp
        push    si
        les     bx,dword ptr [bp+8]        ;get the pointer to string
        mov     ax,[bp+6]                  ;get the number to convert
        cmp     ax,0
        jnz     convert_normal
        mov     word ptr es:[bx],30h     ;say '0' is string
        jmp     end_itoa
convert_normal:
        mov     cx,10                      ;move base to cx
        xor     si,si                      ;zero character counter
div_to_end:
        xor     dx,dx                      ;clear dx for no div by zero
        inc     bx                         ;add 1 to address
        inc     si
        idiv    cx
        or      dx,ax
        cmp     dx,0
        jnz     div_to_end
        dec     bx
        mov     byte ptr es:[bx],0
        mov     ax,[bp+6]
do_number:
        xor     dx,dx
        idiv    cx
        dec     bx
        dec     si
        cmp     si,0
        jz      end_itoa
        mov     byte ptr es:[bx],30h
        add     byte ptr es:[bx],dl
        jmp     do_number
end_itoa:
        pop     si
        pop     bp
        ret
itoa endp


underflow_table dw 00000h    ;0
                dw 01999h    ;1
                dw 03332h    ;2
                dw 04ccbh    ;3
                dw 06664h    ;4
                dw 07ffdh    ;5
                dw 09996h    ;6
                dw 0b32fh    ;7
                dw 0ccc8h    ;8
                dw 0e661h    ;9
                dw 0fffah

CRC_16_tab     dw  00000h,0c0c1h,0c181h,00140h,0c301h,003c0h,00280h,0c241h
               dw  0c601h,006c0h,00780h,0c741h,00500h,0c5c1h,0c481h,00440h
               dw  0cc01h,00cc0h,00d80h,0cd41h,00f00h,0cfc1h,0ce81h,00e40h
               dw  00a00h,0cac1h,0cb81h,00b40h,0c901h,009c0h,00880h,0c841h
               dw  0d801h,018c0h,01980h,0d941h,01b00h,0dbc1h,0da81h,01a40h
               dw  01e00h,0dec1h,0df81h,01f40h,0dd01h,01dc0h,01c80h,0dc41h
               dw  01400h,0d4c1h,0d581h,01540h,0d701h,017c0h,01680h,0d641h
               dw  0d201h,012c0h,01380h,0d341h,01100h,0d1c1h,0d081h,01040h
               dw  0f001h,030c0h,03180h,0f141h,03300h,0f3c1h,0f281h,03240h
               dw  03600h,0f6c1h,0f781h,03740h,0f501h,035c0h,03480h,0f441h
               dw  03c00h,0fcc1h,0fd81h,03d40h,0ff01h,03fc0h,03e80h,0fe41h
               dw  0fa01h,03ac0h,03b80h,0fb41h,03900h,0f9c1h,0f881h,03840h
               dw  02800h,0e8c1h,0e981h,02940h,0eb01h,02bc0h,02a80h,0ea41h
               dw  0ee01h,02ec0h,02f80h,0ef41h,02d00h,0edc1h,0ec81h,02c40h
               dw  0e401h,024c0h,02580h,0e541h,02700h,0e7c1h,0e681h,02640h
               dw  02200h,0e2c1h,0e381h,02340h,0e101h,021c0h,02080h,0e041h
               dw  0a001h,060c0h,06180h,0a141h,06300h,0a3c1h,0a281h,06240h
               dw  06600h,0a6c1h,0a781h,06740h,0a501h,065c0h,06480h,0a441h
               dw  06c00h,0acc1h,0ad81h,06d40h,0af01h,06fc0h,06e80h,0ae41h
               dw  0aa01h,06ac0h,06b80h,0ab41h,06900h,0a9c1h,0a881h,06840h
               dw  07800h,0b8c1h,0b981h,07940h,0bb01h,07bc0h,07a80h,0ba41h
               dw  0be01h,07ec0h,07f80h,0bf41h,07d00h,0bdc1h,0bc81h,07c40h
               dw  0b401h,074c0h,07580h,0b541h,07700h,0b7c1h,0b681h,07640h
               dw  07200h,0b2c1h,0b381h,07340h,0b101h,071c0h,07080h,0b041h
               dw  05000h,090c1h,09181h,05140h,09301h,053c0h,05280h,09241h
               dw  09601h,056c0h,05780h,09741h,05500h,095c1h,09481h,05440h
               dw  09c01h,05cc0h,05d80h,09d41h,05f00h,09fc1h,09e81h,05e40h
               dw  05a00h,09ac1h,09b81h,05b40h,09901h,059c0h,05880h,09841h
               dw  08801h,048c0h,04980h,08941h,04b00h,08bc1h,08a81h,04a40h
               dw  04e00h,08ec1h,08f81h,04f40h,08d01h,04dc0h,04c80h,08c41h
               dw  04400h,084c1h,08581h,04540h,08701h,047c0h,04680h,08641h
               dw  08201h,042c0h,04380h,08341h,04100h,081c1h,08081h,04040h

CCITT_tab      dw  00000h,01021h,02042h,03063h,04084h,050a5h,060c6h,070e7h
               dw  08108h,09129h,0a14ah,0b16bh,0c18ch,0d1adh,0e1ceh,0f1efh
               dw  01231h,00210h,03273h,02252h,052b5h,04294h,072f7h,062d6h
               dw  09339h,08318h,0b37bh,0a35ah,0d3bdh,0c39ch,0f3ffh,0e3deh
               dw  02462h,03443h,00420h,01401h,064e6h,074c7h,044a4h,05485h
               dw  0a56ah,0b54bh,08528h,09509h,0e5eeh,0f5cfh,0c5ach,0d58dh
               dw  03653h,02672h,01611h,00630h,076d7h,066f6h,05695h,046b4h
               dw  0b75bh,0a77ah,09719h,08738h,0f7dfh,0e7feh,0d79dh,0c7bch
               dw  048c4h,058e5h,06886h,078a7h,00840h,01861h,02802h,03823h
               dw  0c9cch,0d9edh,0e98eh,0f9afh,08948h,09969h,0a90ah,0b92bh
               dw  05af5h,04ad4h,07ab7h,06a96h,01a71h,00a50h,03a33h,02a12h
               dw  0dbfdh,0cbdch,0fbbfh,0eb9eh,09b79h,08b58h,0bb3bh,0ab1ah
               dw  06ca6h,07c87h,04ce4h,05cc5h,02c22h,03c03h,00c60h,01c41h
               dw  0edaeh,0fd8fh,0cdech,0ddcdh,0ad2ah,0bd0bh,08d68h,09d49h
               dw  07e97h,06eb6h,05ed5h,04ef4h,03e13h,02e32h,01e51h,00e70h
               dw  0ff9fh,0efbeh,0dfddh,0cffch,0bf1bh,0af3ah,09f59h,08f78h
               dw  09188h,081a9h,0b1cah,0a1ebh,0d10ch,0c12dh,0f14eh,0e16fh
               dw  01080h,000a1h,030c2h,020e3h,05004h,04025h,07046h,06067h
               dw  083b9h,09398h,0a3fbh,0b3dah,0c33dh,0d31ch,0e37fh,0f35eh
               dw  002b1h,01290h,022f3h,032d2h,04235h,05214h,06277h,07256h
               dw  0b5eah,0a5cbh,095a8h,08589h,0f56eh,0e54fh,0d52ch,0c50dh
               dw  034e2h,024c3h,014a0h,00481h,07466h,06447h,05424h,04405h
               dw  0a7dbh,0b7fah,08799h,097b8h,0e75fh,0f77eh,0c71dh,0d73ch
               dw  026d3h,036f2h,00691h,016b0h,06657h,07676h,04615h,05634h
               dw  0d94ch,0c96dh,0f90eh,0e92fh,099c8h,089e9h,0b98ah,0a9abh
               dw  05844h,04865h,07806h,06827h,018c0h,008e1h,03882h,028a3h
               dw  0cb7dh,0db5ch,0eb3fh,0fb1eh,08bf9h,09bd8h,0abbbh,0bb9ah
               dw  04a75h,05a54h,06a37h,07a16h,00af1h,01ad0h,02ab3h,03a92h
               dw  0fd2eh,0ed0fh,0dd6ch,0cd4dh,0bdaah,0ad8bh,09de8h,08dc9h
               dw  07c26h,06c07h,05c64h,04c45h,03ca2h,02c83h,01ce0h,00cc1h
               dw  0ef1fh,0ff3eh,0cf5dh,0df7ch,0af9bh,0bfbah,08fd9h,09ff8h
               dw  06e17h,07e36h,04e55h,05e74h,02e93h,03eb2h,00ed1h,01ef0h

proc compute_CRC16_asm far,length:word,line:dword,base:word
  mov ax,base;
  mov cx,length;
  les bx,line;
  xor di,di;
topcrc16:
  push ax;
  xor al,es:[di+bx]
  xor ah,ah;
  shl ax,1;
  mov si,ax;
  pop ax;
  xchg ah,al
  xor ah,ah;
  xor ax,cs:[CRC_16_tab+si]
  inc di
  cmp di,cx
  jl topcrc16
  ret
endp

proc compute_CCITT_asm,length:word,line:dword,base:word
  mov ax,base;
  mov cx,length;
  les bx,line;
  xor di,di;
topccitt:
  push ax;
  xchg al,ah;
  xor al,es:[di+bx]
  xor ah,ah;
  shl ax,1;
  mov si,ax;
  pop ax;
  xchg ah,al
  xor al,al;
  xor ax,cs:[CCITT_tab+si]
  inc di
  cmp di,cx
  jl topccitt
  ret
endp



END

