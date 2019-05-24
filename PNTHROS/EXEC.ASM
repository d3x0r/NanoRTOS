.MODEL HUGE,C

extrn Request:far
extrn Register:far
extrn check_avail_now:far
extrn swap_module:far
extrn fork:far
extrn perish:far
extrn cdestroy:far
extrn add_hooks:far

.DATA
extrn routines:dword
num_predefs equ 22

.CODE
public Exec_Services
public Connect_Int
public Disconnect_Int


entry_table dw Func00
            dw Func01
            dw Func02
            dw Func03
            dw Func04
            dw Func05
            dw Func06
            dw Func07
            dw Func08
            dw Func09

MAX_FUNCTION equ 10


oldINT                  dd      0
Relinquish_return       dd      0
Exec_Services proc far
;    This is the O/S's executive service handler.  the data needed is
;    in it's registers, so there is no stack setup.
;
;    register ah holds the service number of the call.
;
;
        sti
        mov     bl,ah
        xor     bh,bh
        cmp     bx,MAX_FUNCTION
        jge     no_function
        shl     bx,1
        jmp     cs:[entry_table+bx]
no_function:
        iret
Func00:;  this function is a request for an external routine to be linked
       ;  in.
       ;  Entry Conditions
       ;    ES:DX points to the text name of the routine
       ;    BX    points at the offset of the routine that called here.
       ;  Exit
       ;    Place at CS off the stack:BX is modified to a far jump
       ;this is now an invalid type routine
       iret
       push     cx

       call     Request,dx,es         ;call of external routine to get
                                      ; the address of the routine.  if
                                      ; it is not already in memory, then
                                      ; it loads it from a disk
                                      ; This routine will not return if
                                      ; the requested routine could not be
                                      ; located.
       mov      es,dx
       mov      di,ax
       les      di,es:[di]

       pop      di                    ;restore offset of procedure

       mov      cx,ax                 ;save ax because were going to clobber
                                      ; it

                                      ; continue on and trace
                                      ; back the stack for the correct
                                      ; place in memory to modify.
                                      ;
                                      ;
                                      ;
                                      ;
                                      ;   +8 -far return
                                      ;   +6 -near return = next instruction
                                      ;   +4 -flags
                                      ;   +2 -far call address
                                      ;   sp -near call
                                      ;

                                      ; the 12 subtraction is 5 for the jump
                                      ;  modification and 7 for prior things,
                                      ;  including standard stack setup.
        mov     bx,sp
        mov     word ptr ss:[bx],di   ;back up to go through jump on return
        mov     es,ss:[bx+2]          ; get far return value
                                      ; bx already has the offset of the
                                      ; routine

;        cmp     byte ptr es:[di-3],55h;look to see if we are next to a
;                                      ; push bp
;        jne     write_jmp             ; if it is not, then just change
;                                      ;  the address
;                                      ; otherwise move address on stack to
;        mov     di,sp                 ; go back to the stack
;        mov     bp,ss:[di+6]          ;restore bp
;        mov     ax,ss:[di+4]          ;move flags, and address back 1 notch
;        mov     ss:[di+6],ax
;        mov     ax,ss:[di+2]
;        mov     ss:[di+4],ax
;        mov     ax,ss:[di+0]
;        mov     ss:[di+2],ax
;        add     sp,2
;        sub     word ptr ss:[di+2],3
;        mov     di,ss:[di+2]           ;continue
write_jmp:
        mov     byte ptr es:[di],0eah ; store far jump opcode
        mov     es:[di+1],cx          ; store near address
        mov     es:[di+3],dx          ; store segment
        iret

;this function adds a routine to be used publicly
Func01:
        call    Register,si,es,dx,ds,0       ;call the register routine;
        iret                                    ;simple procedure;

;this function checks to see if a named routine exists already
Func02:
        call    check_avail_now,dx,es     ;check for routine
        iret

;this function spawns a new thread from the currnet place, the
;first one returns a 0 the next returns a 1
;
thread_return   dw      ?
Func03:
        cbw                             ;turn the lower byte into the whole value
        call    fork,ax                 ;dup the module...
        iret                            ;get me outa here.
Func04:
        call    swap_module,ax,dx   ;relinquish
        iret
Func05:
        call    perish         ;remove the current thread.  If this
                               ;was the last thread, then deallocate the
                               ; code, and the stack also.

        iret
Func06:
        call    cdestroy
        iret
Func07:;  this function is a request for a call to an external routine
       ;  in.

       call     Request,dx,es         ;call of external routine to get
                                      ; the address of the routine.  if
                                      ; it is not already in memory, then
                                      ; it loads it from a disk
                                      ; This routine will not return if
                                      ; the requested routine could not be
                                      ; located.
       mov      es,dx
       mov      bx,ax
       call     dword ptr es:[bx]
       iret                           ;return
Func08:;  This function copies the addresses of the preset routines
       ;  into the memory area pointed at by es:di;
       push     cx
       push     ds
       push     si
       mov      cx,num_predefs
       mov      ax,seg routines
       mov      ds,ax
       mov      si,offset routines
mov_routine_data:
       movsw
       movsw
       loop mov_routine_data
       pop      si
       pop      ds
       pop      cx

       iret
Func09:;  this function takes the addresses in si:dx(fork) and cx:bx(die)
       ;  and stores them in the list of hooks for routines to be called
       ;  when a task forks or dies.
       push     si
       push     dx
       push     di
       push     cx
       call     add_hooks
       add      sp,8
       iret

Exec_Services endp

Connect_Int proc far ,routine:dword,int_num:word

        push    di                      ;save di
        push    ds                      ;save ds
        cli                             ;lockout interupts
        mov     bx,int_num              ;get the interupt number
        mov     cl,2                    ;shift it left 2 (Int*4)
        shl     bx,cl
        xor     ax,ax
        mov     es,ax                   ;mov es to 0:
        lds     di,routine              ;get address of routine
        sub     di,8                    ;subtract 8 from routine to store
        mov     ax,es:[bx]              ;  the old IRQ value before the
        mov     [di],ax                 ;  new interrupt routine
        mov     ax,es:[bx+2]
        mov     [di+2],ax
        add     di,4                    ;Add 4 to routine address to store
                                        ;  the addres of the task swapper
                                        ;  for premptive servicing

;****************************
;       Add storage of Pointer to Task Swapper
;****************************


        add     di,4                    ;add 4 to get original address
        mov     es:[bx],di              ;store routine adress in INT table.
        mov     es:[bx+2],ds
        sti                             ;enable interrupts
        pop     ds
        pop     di
        ret
endp

Disconnect_Int proc far ,routine:dword,int_num:word

        push    di                      ;save di
        push    ds                      ;save ds
        cli                             ;lockout interrupts
        mov     bx,int_num              ;get interrupt number
        shl     bx,1
        shl     bx,1
        xor     ax,ax
        mov     es,ax                   ;set ES to 0:
        lds     di,routine              ;get address of routine
        sub     di,8                    ;subtract 8 to get old INT value
        mov     ax,[di]                 ;start test of 0 address
        or      ax,[di+2]               ;or in other part
        jz      not_connected           ;if was zero then don't disconnect
        mov     ax,[di]                 ;store old int back in interrupt table
        mov     es:[bx],ax
        mov     ax,[di+2]
        mov     es:[bx+2],ax
not_connected:
        sti                             ;resume interrupts
        pop     ds
        pop     di
        ret
endp

END


