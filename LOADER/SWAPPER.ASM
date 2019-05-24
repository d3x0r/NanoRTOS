.MODEL HUGE,C
INCLUDE os.equ

  extrn exit:far
  extrn print:far
  extrn duplicate_module:far
  extrn cperish:far    ;C routine to kill task
  extrn mem_check:far

.DATA
  extrn current_module:dword
  extrn first_module:dword
  extrn process_control:word
  extrn force_time:word
  loopc dw ?

.CODE



  public swap_module
  public fork
  public perish
  public swap_to

rel_begin MACRO
        push    bp
        mov     bp,sp
        push    ds
        push    si
        push    di
        push    ax
ENDM

rel_end MACRO
        pop     ax
        pop     di
        pop     si
        pop     ds
        pop     bp
ENDM

locked_message db 13,10,"All routines are asleep...",13,10,0
public beep

beep proc far,freq:word
  pushf
  cli;
  mov ax,0b6h;
  out 043h,al;
  mov ax,freq
  out 042h,al;
  xchg ah,al
  out 042h,al;
  in al,061h;
  or al,3;
  out 061h,al;
  sti;
  mov cx,-1
wait1:
  loop wait1
  mov cx,-1
wait2:
  loop wait2
  cli;
  in al,061h;
  and al,0fdh;
  out 061h,al;
  popf
  ret
endp


swap_to proc far
;  8   far of next
;  6   near of next
;  4   far
;  2   near
;  bp- bp
        rel_begin
        mov     ax,@data
        mov     ds,ax
        mov     es,ax
        or      process_control,2
        lds     bx,current_module         ;load ds:bx to be currnet mod
        mov     [bx].stackseg,ss              ;store current stackseg
        mov     [bx].stackofs,sp              ;store current stackofs
        test    es:process_control,1
        jz      continue_skip
;       push 457
;        call    beep
;       add sp,2
        mov     es:force_time,0
        and     es:process_control,0fffeh
        lds     bx,es:first_module
        jmp     check_new_task;
continue_skip:
        lds     bx,[bp+6]
        jmp     check_new_task;
swap_to endp


fork proc far


;   Stack Frame
;
;       +12  flags
;       +10  far iret
;       +8   near iret
;       +6   relation
;       +4   far ret
;       +2   near ret
;       bp-  Old BP
;       -2   DS
;       -4   SI
;       -6   DI
;       -8   AX
;
  rel_begin
  mov ax,[bp+12]
  push ax
  popf
  mov   ax,@data
  mov   ds,ax
  mov   es,ax
  or    process_control,2
  lds bx,current_module
  mov [bx].stackseg,ss
  mov [bx].stackofs,sp
  pop ax    ;get rid of prior ax
  mov ax,1
  push ax   ;store child value for the duplication - next becomes child
  push  ds
  push  es
  push  bx

  mov ax,[bp+6]   ;relation from caller
  push ax         ;
  mov ax,1        ;copies to make
  push ax
  xor   ax,ax
  call duplicate_module
  add   sp,4

  pop   bx
  pop   es
  pop   ds
  pop   ax
  mov   ax,0
  push  ax   ;store parent value after duplication
  mov   cs:loopc,0
  test  byte ptr [bp+6],80h
  jnz no_inc
  jmp continue_current
fork endp



swap_module proc far
;   Stack Frame
;       +6   val
;       +4   far ret
;       +2   near ret
;       bp-  Old BP
        rel_begin
        cli
        mov     ax,@data
        mov     ds,ax
        mov     es,ax
        or      es:process_control,2          ;set scan lockout
        lds     bx,current_module             ;load ds:bx to be currnet mod
        mov     [bx].stackseg,ss              ;store current stackseg
        mov     [bx].stackofs,sp              ;store current stackofs
        mov     ax,[bp+6];                    ;get parameter that was passed
        mov     word ptr [bx].status,ax       ;store status
        mov     ax,[bp+8];                    ;get other half
        mov     word ptr [bx+2].status,ax     ;store it
        test    es:process_control,1          ;check to see if we need to start
                                              ; at top of list
        jz      do_next;                      ;if not continue from here
;        push    657h
;        call    beep
;        add     sp,2
        and     es:process_control,NOT 1      ;turn off force scan bit
        mov     es:force_time,0               ;zero force_scan timer
        lds     bx,es:first_module            ;go to first module;
        jmp     check_new_task;

do_next:
        lds     bx,[bx].next
check_new_task:
        cli
        test    word ptr[bx].status+2,8000h
        jz      no_inc
        add     word ptr[bx].status,1
        adc     word ptr[bx].status+2,0
no_inc:
        mov     si,word ptr [bx].status
        or      si,word ptr [bx].status+2
        sti
        jnz     do_next
continue_current:
        cli
        mov     word ptr es:current_module,bx
        mov     word ptr es:current_module+2,ds
        mov     ss,[bx].stackseg
        mov     sp,[bx].stackofs
        sti
        mov     ax,@data
        mov     ds,ax
        and     process_control,0fffdh
;        call    mem_check
        rel_end
        ret
swap_module endp

int_module proc far
  push ax
  push ds
  mov   ax,@data
  mov   ds,ax
  test  process_control,4      ;test to see if task scanner is locked out
  jnz No_Work;                    ;if it is just return from the interrupt.
  mov   al,20h                    ;clear interupt now and go to the swap routine
                                  ;as task level.
  out   20h,al
  sti
  push es
  push bx
  push dx
  push cx
  push 0
  push 0
  call swap_module
  add sp,2
  pop cx
  pop dx
  pop bx
  pop es

No_Work:
  mov al,20h
  out 20h,al
  pop ds
  pop ax
  iret
endp


perish proc far
;this routine is the end of a thread's life.  It calls me here, and
;I free it's unlink it, and jump to relinquish to go to the next
;thread never to return here again
;
;             falgs
;             far iret
;              near iret
;              far reat
;      bp- sp- near ret
        mov     bp,sp
        mov     ax,[bp+8]
        push    ax
        popf
        call    cperish
        mov     ax,@data
        mov     es,ax
        lds     bx,es:current_module
        mov     es:loopc,0
        jmp     no_inc
perish endp

END

