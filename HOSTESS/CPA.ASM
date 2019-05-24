        page   60,80
.286

;*****************************************************************************
;File:    cpa.asm
;Purpose: Sample assembly language control program for HOSTESS i.
;         Supports open, close, read, and write to any asynchronous line.
;Company: Comtrol Corporation
;Author:  Craig Harrison
;Release: 1.00, 8-23-91 - Original release
;         1.01, 12-3-91, Craig Harrison
;               In enable_line proc added missing "out dx,al" for reset
;               external status interrupts.  Also deleted 2nd insurance reset.
;*****************************************************************************
;Copyright 1991 Comtrol Corporation. All rights reserved. Subject to developers
;license agreement.
;*****************************************************************************

         .xlist
         .list
         include cp.equ

_TEXT    segment para public  'CODE'
         assume  cs:_TEXT, ds:_TEXT

                 org 0h
;The first 80h bytes are the "firmware user area" defined by the HOSTESS i
;firmware

         public  interact_flag
interact_flag    dw      ?             ;processor interaction flag
boot_flag        dw      ?             ;boot/activity flag
cfg_map          dw      ?             ;configuration map
fw_release       db      8 dup (?)     ;firmware release number
sw_release       db      8 dup (?)     ;control program release number
                 dd      ?             ;reserved
dram_map         dd      ?             ;DRAM map
scc_map          dd      ?             ;SCC map
board_id         dd      ?             ;board ID
ii_flag          db      ?             ;invalid interrupt flag
ii_type          db      ?             ;invalid interrupt type
ii_cnt           dw      ?             ;invalid interrupt count
                 db      128-42 dup (?) ;balance of firmware area

                 org     80h             ;firmware jumps here when interrupted
         public  cpmain, start
cpmain   proc

;Initialize segment registers, interrupt vectors, etc.
start:   cli                       ;disable interrupts while initializing them
         mov     ax,cs                   ;establish data segment
         mov     ds,ax

         mov     ss,ax                   ;establish a program stack
         mov     sp,offset tos

         xor     ax,ax
         mov     es,ax
         mov     bx,INT1_type*4          ;setup system interrupt vector
         mov     ax,offset system_isr
         mov     es:[bx],ax
         mov     ax,cs
         mov     es:[bx+2],ax

         mov     bx,TIM1_type*4          ;setup timer 1 intr vector
         mov     ax,offset timer1_isr
         mov     es:[bx],ax
         mov     ax,cs
         mov     es:[bx+2],ax

         mov     dx,TIM_CTL
         mov     al,074h                 ;enable timer1
         out     dx,al
         mov     dx,TIMER1
         mov     al,TIMER1_LSB           ;12/sec LSB
         out     dx,al
         mov     al,TIMER1_MSB           ;12/sec MSB
         out     dx,al

         mov     ax,base_vector          ;AX = base vector type
         call    vector_init             ;initialize vector table

         mov     ax,cs
         mov     es,ax
         mov     di,offset interact_flag ;DI=> interaction flag
         mov     [di],55aah              ;restore interaction flag

         mov     di,offset boot_flag     ;indicate control program active
         mov     [di],0ffffh

         mov     di,offset sw_release    ;move software release number
         mov     si,offset release       ;  to shared memory
         mov     cx,release_len
         rep     movsb

         sti                             ;enable interrupts


;Main processing, is an infinite loop
main_10:
         mov     si,offset line00        ;SI=> 1st line table
main_30:
         mov     ax,[si].line_status     ;get line status
         test    ax,line_active          ;is line active ?
         jnz     short main_40
         jmp     main_70                 ;  no ... move on to next line
main_40:
         test    ax,Tx_active            ;is transmit active ?
         jnz     main_70                 ;  yes ... move on to next line
         call    deq_Tx_data             ;character from xmit queue to send?
         jc      main_70                 ;  no ... continue
main_60:
         cli                             ;protect SCC out from interrupts
         call    send_data               ;send the caracter in ac
         sti
main_70:
         cmp     si,offset line15        ;last table?
         jl      short main_75           ;  no ... continue
         jmp     main_10                 ;otherwise start over ...
main_75:
         add     si,line_entry_len       ;bump to next line table
         jmp     main_30
cpmain   endp



;------------------------------------------------------------------------------
; Name: send_data
; Purpose: send data character in al out the comm channel.
; Entry: AL is the character
;        SI points at the line table entry
; Exit: None
send_data proc
         or      [si].line_status,Tx_active   ; indicate transmit active
         mov     dx,[si].io_base         ;get base I/O address
         add     dx,2                    ;offset to data register
         out     dx,al                   ;move character to SCC
         ret
send_data endp

;------------------------------------------------------------------------------
; Name: Check_transmit_flow
; Purpose: to check the transmit flow control to see if we should send
;          the data or not
; Entry: SI points at line data
; Exit: Carry Reset okay to send
;       Carry Set not okay to send
check_transmit_flow proc
        clc                                        ;set carry-is okay
        test    [si].Flow_control,Outbound_flowed  ;check to see if already
        jz      check_transmit_flow_10             ;  flowed if not,check other
        test    [si].Flow_control,Respect_soft     ;check to see if we should
        jnz     check_transmit_flow_15             ;actually respect it.
        and     [si].Flow_control,NOT Outbound_flowed ;if we shouldn't, then we
                                                   ;clear the condition and
check_transmit_flow_10:
        test    [si].Flow_control,Outbound_hard    ;check to check hardware flow
        jz      check_transmit_flow_20             ;if not, leave
        mov     dx,[si].io_base                    ;get port
        mov     al,RR0                             ;set read register 0
        out     dx,al
        in      al,dx                              ;read register 0
;        mov     [si].RR0_,al                     ;should be int only
        test    al,CTS_                            ;check for CTS
        jnz     check_transmit_flow_20             ;if present exit
check_transmit_flow_15:
        stc
check_transmit_flow_20:
        ret
check_transmit_flow endp

;------------------------------------------------------------------------------
; Name: timer1_isr
; Purpose: Process interrupt from timer 1.  Doesn't do anything useful, just
;          increments a word in dual port RAM to demonstrate that its working.
; Entry: Nothing
; Exit:  Nothing

timer1_isr proc
         push    ax                    ;save registers
         push    dx
         inc     word ptr ds:7ch       ;increment word
         mov     dx,INTCTL             ;end of interrupt to PIC
         mov     al,EOI_VAL
         out     dx,al
         pop     dx                    ;recover registers
         pop     ax
         iret
timer1_isr endp


;------------------------------------------------------------------------------
;Name:    system_isr
;Purpose: Process interrupt from sytem processor
;Entry:   Nothing
;Exit:    Nothing

system_isr proc
         pusha                          ;save registers

system_isr_02:
         call    deq_Com_msg            ;Comm Processor message waiting ?
         jc      system_isr_20          ;  no ... exit
         mov     al,msg_area            ;get command
         xor     ah,ah                  ;clear upper byte
         shl     ax,1                   ;double the command value
         mov     bx,offset command_tbl  ;BX=> command table
         add     bx,ax                  ;offset into table
         cmp     bx,offset command_tble ;valid command ?
         jnc     system_isr_10          ;  no ... continue
         call    word ptr [bx]          ;invoke command processor
system_isr_10:
         jmp     system_isr_02          ;check for another message
system_isr_20:
         mov     dx,INTCTL              ;end of interrupt to PIC
         mov     al,EOI_VAL
         out     dx,al
         popa
         iret
system_isr endp

command_tbl      equ     $
         dw      null_cmd              ;0 - null command
         dw      open                  ;1 - open  a line
         dw      close                 ;2 - close a line
         dw      int_sys               ;3 - generate interrupt to system
command_tble    equ     $


;------------------------------------------------------------------------------
;Name:    null_cmd (0)
;Purpose: Handle unimplemented command
;Entry:   Nothing
;Exit:    Nothing

null_cmd proc
         ret
null_cmd endp


;------------------------------------------------------------------------------
;Name:    open (1)
;Purpose: Open a line for asynchronous communications
;Entry:   msg_area+1 = line number
;         msg_area+2 = WR3 parameters   (Rx character size)
;         msg_area+3 = WR4 parameters   (stop bits, parity)
;         msg_area+4 = WR5 parameters   (Tx character size)
;         msg_area+5 = WR12 parameter   (lower byte of BRGTC)
;         msg_area+6 = WR13 parameter   (upper byte of BRGTC)
;         msg_area+7 = Flow_control parameter
;Exit:    Nothing

open     proc
         call    get_line              ;return SI=> line table entry
         call    configure_line        ;configure line for async communications
         call    enable_line           ;enable line
         ret
open     endp


;------------------------------------------------------------------------------
;Name:    close (2)
;Purpose: Close a serial line
;Entry:   msg_area+1 = line number
;Exit:    Nothing

close    proc
         call    get_line              ;SI=> line table entry
         mov     dx,[si].io_base       ;get base I/O address
         mov     al,WR1                ;point to WR1
         out     dx,al
         xor     al,al                 ;disable all interrupts
         out     dx,al
         mov     [si].line_status,0    ;disable all in line_status

         mov     al,[si].WR5_          ;get write reg 5
         and     al,NOT (RTS OR DTR)   ;turn off RTS and DTR
         mov     bl,al                 ;save value
         mov     al,WR5                ;write select reg 5
         out     dx,al                 ;select register
         mov     al,bl                 ;get value
         out     dx,al                 ;drop dtr/rts

         ret
close    endp


;------------------------------------------------------------------------------
;Name:    int_sys (3)
;Purpose: Generate an interrupt to the system.  This would probably never be
;         executed as a command from the system by is included here to show how
;         to interrupt the system.
;Entry:   Nothing
;Exit:    Nothing

int_sys  proc
         mov     dx,C2Sint_reg         ;address of interrupt reg
         mov     ax,C2Sint_low         ;set interrupt line low
         out     dx,ax                 ;do it

         jmp     short $+2             ;delay 2uS
         jmp     short $+2
         jmp     short $+2

         mov     ax,C2Sint_hi          ;set interrupt line high
         out     dx,ax                 ;do it
         ret
int_sys  endp

;------------------------------------------------------------------------------
;Name:    configure_line
;Purpose: Configure the communications parameters in the line table
;Entry:   SI=> line table entry
;         msg_area+1 = line number
;         msg_area+2 = WR3 parameters   (Rx character size)
;         msg_area+3 = WR4 parameters   (stop bits, parity)
;         msg_area+4 = WR5 parameters   (Tx character size)
;         msg_area+5 = WR12 parameter   (lower byte of BRGTC)
;         msg_area+6 = WR13 parameter   (upper byte of BRGTC)
;         msg_area+7 = Flow_control parameter
;Exit:    Nothing

configure_line  proc
         push    si                    ;save line table start
         add     si,offset WR3_
         mov     di,si                 ;DI=> line table
         mov     si,offset msg_area+2  ;SI=> message area
         lodsb                         ;WR3 value
         and     al,0c0h               ;isolate Rx character size
         stosb                         ;  and move to line table
         lodsb                         ;WR4 value
         and     al,0fh                ;isolate stop bits/parity
         stosb                         ;  and move to line table
         lodsb                         ;WR5 value
         or      al,80h
         and     al,60h                ;isolate Tx character size
         mov     ah,[di]               ;get current value
         and     ah,0ffh-60h           ;  and clear Tx character size
         or      al,ah                 ;combine the two
         stosb                         ;  and move to line table
         mov     cx,2                  ;setup remaining length
         rep     movsb                 ;move BRGTC to line table

         lodsb                         ;get flow control
         add     di,Flow_control-WR15_ ;move to flow control data
         stosb                         ;store flow control data
         pop     si                    ;recover line table start
         call    scc_init              ;update params to SCC
         ret
configure_line endp


;------------------------------------------------------------------------------
;Name:    enable_line
;Purpose: Enable a serial line
;Entry:   SI=> line table entry
;Exit:    Nothing
;Sequence to enable interrupts:
;       1.  WR15: specify external/status interrupts
;       2.   WR0: reset external status
;       3.   WR0: reset external status twice
;       4.   WR1: enable receive, transmit and external status interrupts
;       5.   WR9: enable master interrupt enable

enable_line proc
         mov     ax,[si].Txq_head      ;flush the Tx queue
         mov     [si].Txq_tail,ax

         mov     ax,[si].Rxq_tail      ;flush the Rx queue
         mov     [si].Rxq_head,ax

         mov     [si].line_status,line_active ;indicate line active

         mov     al,[si].WR5_          ;make sure RTS is high
         or      al,RTS
         or      al,DTR                ;DTR temp
         mov     [si].WR5_,al
         mov     dx,[si].io_base       ;get base i/o address
         mov     al,WR5
         out     dx,al
         mov     al,[si].WR5_          ;transmit parameters
         out     dx,al

         ;Enable interrupts
         mov     al,WR15
         out     dx,al
         mov     al,[si].WR15_         ;external/status interrupt control
         out     dx,al

         mov     al,WR0
         out     dx,al                 ;for insurance
         mov     al,reset_ext          ;reset external status interrupts
         out     dx,al

         mov     al,WR1                ;interrupt enables
         out     dx,al
         mov     al,ext_int_enable+Tx_int_enable+parity_special+Rx_int_enable
         out     dx,al

         mov     al,WR9
         out     dx,al
         mov     al,MIE+status_lo+VIS   ;master interrupt enable
         out     dx,al

         mov     [si].Break_char,4
         ret
enable_line endp

;------------------------------------------------------------------------------
;Name:    get_line
;Purpose: Return line table pointer
;Entry:   msg_area+1 = line number
;Exit:    SI=> line table entry

get_line proc
         mov     al,msg_area+1         ;get line number
         xor     ah,ah                 ;clear upper byte
         mov     cx,line_entry_len     ;calculate line table offset
         mul     cl
         add     ax,offset line00
         mov     si,ax
         ret
get_line endp

;-------------------------------------------------------------------------------
;Name:    scc_init
;Purpose: Initialize SCC for asynchronous operation
;Entry:   SI=> line table entry
;Exit:    Nothing
;Sequence:
;       1.  WR9: reset channel
;       2.  WR4: specify clock mode, number of stop bits and parity
;       3.  WR2: specify base interrupt vector type
;       4.  WR3: specify number of bits/character for receive
;       5.  WR5: specify number of bits/character for transmit
;       6.  WR9: specify interrupt control
;       7.  WR11: specify receive and transmit clock source
;       8.  WR12: specify lower byte of Baud Rate Time Constant
;       9.  WR13: specify upper byte of Baud Rate Time Constant
;      10.  WR14: specify Baud Rate Generator source and enable it
;      11.  WR3: enable receive operation
;      12.  WR5: enable transmit operation
;      13.  RR0: read the initial values of control signals.

scc_init proc
         mov     dx,[si].io_base       ;get base I/O address

         mov     al,WR4
         out     dx,al
         mov     al,[si].WR4_          ;stop bits and parity
         or      al,x16_clock          ;add clock mode
         out     dx,al

         mov     al,WR2
         out     dx,al
         mov     al,[si].WR2_          ;base interrupt vector type
         out     dx,al

         mov     al,WR3
         out     dx,al
         mov     al,[si].WR3_          ;Rx character size
         out     dx,al

         mov     al,WR5
         out     dx,al
         mov     al,[si].WR5_          ;Tx character size, modem/break
         or      al,DTR                ;quick assert of DTR
         out     dx,al

         mov     al,WR9
         out     dx,al
         mov     al,MIE+status_lo+VIS      ;interrupt control
         out     dx,al

         mov     al,WR11
         out     dx,al
         mov     al,Rx_clk_eq_BRG+Tx_clk_eq_BRG  ;clock sources
         out     dx,al

         mov     al,WR12
         out     dx,al
         mov     al,[si].WR12_          ;lower byte of BRGTC
         out     dx,al

         mov     al,WR13
         out     dx,al
         mov     al,[si].WR13_          ;upper byte of BRGTC
         out     dx,al

         mov     al,WR14
         out     dx,al
         mov     al,BRG_eq_sys_clk+BRG_enable    ;BRG source
         out     dx,al

         mov     al,WR3
         out     dx,al
         mov     al,[si].WR3_          ;Rx character size
         or      al,Rx_enable          ;enable receive
         mov     [si].WR3_,al
         out     dx,al

         mov     al,WR5
         out     dx,al
         mov     al,[si].WR5_          ;Tx character size
         or      al,Tx_enable          ;enable transmit
         mov     [si].WR5_,al
         out     dx,al

         mov     al,RR0
         out     dx,al
         in      al,dx
         mov     [si].RR0_,al

         ret
scc_init endp


;------------------------------------------------------------------------------
;Name:    deq_Tx_data
;Purpose: Remove character from transmit queue
;Entry:   SI=> line table entry
;Exit:    carry set if queue is empty, else
;         carry clear and AL = character

deq_Tx_data proc
         push    bx
         test    [si].status,Flowed            ;check to see if we tried to
         jz      deq_Tx_data_2                 ;stop the other end
         test    [si].Flow_control,Inbound_soft;check to see if we did an x-off
                                               ; to control the other end
         jz      deq_Tx_data_2
         test    [si].Flow_control,Command_soft;check to see if we should send
                                               ;some kind of flow control
         jnz     deq_Tx_data_05
         and     [si].status,NOT Flowed        ;we shouldn't command flow
                                               ; control so this bit should not
                                               ; have been set bye RCA
         jmp     deq_Tx_data_2
deq_Tx_data_05:
         mov     bx,[si].Rxq_tail              ;compute how much rcv buf is avl
         sub     bx,[si].Rxq_head
         jnc     deq_Tx_data_1
         add     bx,Rxb_size

deq_Tx_data_1:
         cmp     bx,Rxb_undr                   ;check to see if buffer avl is
                                               ;greater than Rcv Buffer lower
                                               ;flow control boundry.
         jng     deq_Tx_data_2                 ;if not, continue as norm
         and     [si].status,NOT Flowed
         jmp     deq_Tx_data_4
deq_Tx_data_2:
         call    check_transmit_flow   ;don't even bother to check buffer if
         jc      deq_Tx_data_10        ; flow control disallows send

         test    [si].Flow_control,Command_soft
         jz      deq_Tx_data_9

         test    [si].status,Send_XOFF ; check to see if we need to send
                                       ; an XOFF character
         jz      deq_Tx_data_3
         clc
         mov     al,XOFFCHAR
         and     [si].status,NOT Send_XOFF
         jmp     deq_Tx_data_10
deq_Tx_data_3:
         test    [si].status,Send_XON  ; check to see if we need to send
                                       ; an XON character
         jz      deq_Tx_data_9
deq_Tx_data_4:
         clc
         mov     al,XONCHAR
         and     [si].status,NOT (Send_XON OR Flowed)
         jmp     deq_Tx_data_10

deq_Tx_data_9:

         mov     bx,[si].Txq_tail      ;get queue tail
         cmp     bx,[si].Txq_head      ;is queue empty ?
         stc                           ;(assume it is)
         jz      deq_Tx_data_10        ;  yes ... exit
         push    di                    ;save register
         mov     di,[si].Txq_offset    ;get queue offset
         mov     al,[di+bx]            ;remove character
         pop     di                    ;recover register
         inc     bx                    ;bump pointer
         and     bx,Txq_mask           ;  and mask it
         mov     [si].Txq_tail,bx      ;update pointer
         clc                           ;return carry clear
deq_Tx_data_10:
         pop     bx
         ret
deq_Tx_data endp


;------------------------------------------------------------------------------
;Name:    enq_Sys_msg
;Purpose: Add message to System Processor queue
;Entry:   Nothing
;Exit:    carry set if queue is full, else
;         carry clear

enq_Sys_msg proc
         push    ax                              ;save registers
         push    bx

         mov     bx,offset Sysq        ;BX=> system queue data
         mov     ax,[bx].msgq_head     ;get queue head
         inc     ax                    ;bump pointer
         and     ax,msgq_mask          ;  and mask it
         cmp     ax,[bx].msgq_tail     ;is queue full ?
         stc                           ;(assume it is)
         jz      enq_Sys_msg_10        ;  yes ... exit
         push    cx                    ;save additional registers
         push    si
         push    di
         mov     ax,[bx].msgq_head     ;get queue head again
         mov     cx,msg_len            ;calculate message offset
         mul     cl
         lea     di,[bx].msgq_area
         add     di,ax                 ;got DI
         mov     si,offset msg_area    ;got SI

         rep     movsb                 ;move message queue area
         mov     ax,[bx].msgq_head     ;get queue head again
         inc     ax                    ;bump pointer
         and     ax,msgq_mask          ;  and mask it
         mov     [bx].msgq_head,ax     ;update queue head
         clc                           ;return carry clear
         pop     di
         pop     si
         pop     cx
enq_Sys_msg_10:
         pop     bx
         pop     ax
         ret
enq_Sys_msg endp

;------------------------------------------------------------------------------
;Name:    deq_Com_msg
;Purpose: Remove message from Communications Processor queue
;Entry:   Nothing
;Exit:    carry set if queue is empty, else
;         carry clear

deq_Com_msg proc
         push    ax                    ;save registers
         push    bx

         mov     bx,offset Comq        ;BX=> comm queue data
         mov     ax,[bx].msgq_tail     ;get queue tail
         cmp     ax,[bx].msgq_head     ;is queue empty ?
         stc                           ;(assume it is)
         jz      deq_Com_msg_10        ;  yes ... exit
         push    cx                    ;save additional registers
         push    si
         push    di
         mov     cx,msg_len            ;calculate message offset
         mul     cl
         lea     si,[bx].msgq_area
         add     si,ax                 ;got SI
         mov     di,offset msg_area    ;got DI

         rep     movsb                 ;move message local area
         mov     ax,[bx].msgq_tail     ;get queue tail again
         inc     ax                    ;bump pointer
         and     ax,msgq_mask          ;  and mask it
         mov     [bx].msgq_tail,ax     ;update queue tail
         pop     di
         pop     si
         pop     cx
deq_Com_msg_10:
         pop     bx
         pop     ax
         ret
deq_Com_msg endp


;------------------------------------------------------------------------------
;Name:    TBE_isr
;Purpose: Common Transmit Buffer Empty Interrupt Service Routine.
;         This clears the Tx_active flag in the line table to indicate that
;         a character is no longer in the process of being transmitted.
;         To keep the time in the ISR short data writes to the SCC are handled
;         in the main loop.
;Entry:   AX = line table entry
;Exit:    Nothing

TBE_isr  proc
         push    dx                      ; save registers etc.
         push    bx
         push    cx
         push    si
         push    di

         mov     si,ax                 ;si = line table entry
         mov     dx,[si].io_base       ;get base I/O address
         push    dx                    ;save I/O address for isr_ret
         and     [si].line_status,NOT Tx_active    ;inactive

TBE_isr_10:
         mov     al,WR0
         out     dx,al
         mov     al,reset_Tx_int       ;reset pending Tx interrupt
         out     dx,al
TBE_isr_99:
         jmp     isr_ret               ;common exit ...
TBE_isr endp

;------------------------------------------------------------------------------
;Name:    ESC_isr
;Purpose: Common External Status Change Interrupt Service Routine.  This
;         doesn't do any real work, just demonstrates how to reset and return.
;Entry:   AX = line table entry
;Exit:    Nothing

ESC_isr  proc
         push    dx                    ;save registers etc.
         push    bx
         push    cx
         push    si
         push    di

         mov     si,ax                 ;si = line table entry
         mov     dx,[si].io_base       ;get base I/O address
         push    dx                    ;save I/O address for isr_ret
         mov     al,RR0
         out     dx,al
         in      al,dx
         mov     [si].RR0_,al

         ;External status change processing goes here

         mov     al,WR0
         out     dx,al
         mov     al,reset_ext           ;reset external/status interrupts
         out     dx,al
         jmp     isr_ret                ;common exit ...
ESC_isr  endp

;------------------------------------------------------------------------------
;Name:    RCA_isr
;Purpose: Common Receive Character Available Interrupt Service Routine
;Entry:   AX = line table entry
;Exit:    Nothing

RCA_isr  proc
         push    dx                    ;save registers etc.
         push    bx
         push    cx
         push    si
         push    di

         mov     si,ax                 ;si = line table entry
         mov     dx,[si].io_base       ;get base I/O address
         push    dx                    ;save I/O address for isr_ret
         add     dx,2                  ;set up to read data register
         in      al,dx                 ;input character from SCC

         mov     bx,[si].Rxq_tail
         sub     bx,[si].Rxq_head
         dec     bx
         cmp     bx,0
         jge     RCA_isr_30
         add     bx,Rxb_size
RCA_isr_30:
;***************** Buffer flow control check
         test    [si].status,Flowed    ;see if we are already flowed
         jz      RCA_isr_32

         cmp     bx,Rxb_undr           ;if so, then check to see if we
                                       ;have removed enough data to unflow
                                       ;the channel.
         jl      RCA_isr_32
         and     [si].status,NOT Flowed    ;we were flowed, and now regardless
                                           ;of type we are now releasing it.
         test    [si].Flow_control,Inbound_soft
         jz      RCA_isr_31
         test    [si].Flow_control,Command_soft
         jz      RCA_isr_31
         or      [si].status,Send_XON      ;mark to send x-on
;***************** hardware command check
RCA_isr_31:
         test    [si].Flow_control,Inbound_hard
         jz      RCA_isr_32
         mov     dx,[si].io_base                    ;get port
         push    ax
         mov     al,WR5                             ;set read register 0
         out     dx,al
         or      byte ptr [si].WR5_,RTS
         mov     al,[si].WR5_
         out     dx,al
         pop     ax

RCA_isr_32:
         cmp     bx,Rxb_ovr         ;see if we exceed our max
         jg      RCA_isr_35
         or      [si].status,Flowed ;turn on flow control bit
;***************** software command check-conditioned by last write
         test    [si].Flow_control,Inbound_soft ;should we send x-on/x-off
         jz      RCA_isr_33
         test    [si].Flow_control,Command_soft ;are we able to send x-on/x-off
         jz      RCA_isr_33
         or      [si].status,Send_XOFF  ;mark to send x-off

;***************** hardware command check
RCA_isr_33:
         test    [si].Flow_control,Inbound_hard
         jz      RCA_isr_35
;drop RTS
         mov     dx,[si].io_base                    ;get port
         push    ax
         mov     al,WR5                             ;set read register 0
         out     dx,al
         and     byte ptr [si].WR5_,NOT RTS
         mov     al,[si].WR5_
         out     dx,al
         pop     ax


RCA_isr_35:
         cmp     bx,1                  ;bx = number of empty spots
         jl      RCA_isr_99            ;if Rx buffer full exit

         mov     bx,[si].Rxq_head      ;get queue head again
         mov     di,[si].Rxq_offset    ;get queue offset
         mov     [di+bx],al            ;add character to queue
         test    [si].Flow_control,Respect_soft
         jz      RCA_isr_70

         test    [si].Flow_control,Outbound_soft
         jz      RCA_isr_50
         cmp     al,XONCHAR
         jne     RCA_isr_40
         and     [si].Flow_control,NOT Outbound_flowed
         jmp     RCA_isr_50
;***************** test for respect x-on/x-off

RCA_isr_40:
         cmp     al,XOFFCHAR
         jne     RCA_isr_50
         or      [si].Flow_control,Outbound_flowed
;***************** test for respect x-on/x-off
RCA_isr_50:
         cmp     [si].Break_char,0
         je      RCA_isr_70
         cmp     [si].Break_char,al
         jne     RCA_isr_70
         or      [si].status,Break_recvd
RCA_isr_70:
         inc     bx                    ;bump head pointer
         and     bx,Rxq_mask           ;  and mask it
         mov     [si].Rxq_head,bx      ;  and update it
RCA_isr_99:
         jmp     isr_ret                ;common exit ...
RCA_isr endp

;------------------------------------------------------------------------------
;Name:    SRC_isr
;Purpose: Common Special Receive Condition Interrupt Service Routine.  This shows
;         how to get the special receive condition status, but doesn't do any
;         processing on it.
;Entry:   AX = line table entry
;Exit:    Nothing

SRC_isr  proc
         push    dx                    ;save registers etc.
         push    bx
         push    cx
         push    si
         push    di

         mov     si,ax                 ;si = line table entry
         mov     dx,[si].io_base       ;get base I/O address
         push    dx                    ;save I/O address for isr_ret

         mov     al,RR1                ;allow access to Read Register 1
         out     dx,al
         in      al,dx                 ;get special receive condition status


         ;Do Special Receive Condition processing here

         mov     al,WR0                ;for insurance
         out     dx,al
         mov     al,error_reset        ;issue error reset command
         out     dx,al

SRC_isr  endp

;------------------------------------------------------------------------------
;Name:    isr_ret
;Purpose: Common Interrupt Service Routine exit processing
;Entry:   DX = SCC base I/O address
;Exit:    To interrupted routine

isr_ret  proc
         pop     dx                    ;get I/O address

         pop     di
         pop     si
         pop     cx
         pop     bx

         mov     al,WR0
         out     dx,al
         mov     al,reset_ius          ;end of interrupt to SCC
         out     dx,al

         mov     dx,INTCTL             ;end of interrupt to PIC
         mov     al,EOI_VAL
         out     dx,al

         pop     dx
         pop     ax
         iret
isr_ret  endp


;------------------------------------------------------------------------------
;Name:    lineXX_TBE
;Purpose: Transmit Buffer Empty Interrupt Service Routine.  There is one of
;         these for each line.  Since each line has identical requirements
;         on TBE, each of these jumps to a common TBE_isr.
;Entry:   Nothing
;Exit:    AX = line table entry

line00_TBE proc                        ;Transmit Buffer Empty
         push    ax                    ;save register
         mov     ax,offset line00      ;   and setup line table offset
         jmp     TBE_isr               ;do common processing ...
line00_TBE endp

line01_TBE proc                        ;Transmit Buffer Empty
         push    ax                    ;save register
         mov     ax,offset line01      ;   and setup line table offset
         jmp     TBE_isr               ;do common processing ...
line01_TBE endp

line02_TBE proc                        ;Transmit Buffer Empty
         push    ax                    ;save register
         mov     ax,offset line02      ;   and setup line table offset
         jmp     TBE_isr               ;do common processing ...
line02_TBE endp

line03_TBE proc                        ;Transmit Buffer Empty
         push    ax                    ;save register
         mov     ax,offset line03      ;   and setup line table offset
         jmp     TBE_isr               ;do common processing ...
line03_TBE endp

line04_TBE proc                        ;Transmit Buffer Empty
         push    ax                    ;save register
         mov     ax,offset line04      ;   and setup line table offset
         jmp     TBE_isr               ;do common processing ...
line04_TBE endp

line05_TBE proc                        ;Transmit Buffer Empty
         push    ax                    ;save register
         mov     ax,offset line05      ;   and setup line table offset
         jmp     TBE_isr               ;do common processing ...
line05_TBE endp

line06_TBE proc                        ;Transmit Buffer Empty
         push    ax                    ;save register
         mov     ax,offset line06      ;   and setup line table offset
         jmp     TBE_isr               ;do common processing ...
line06_TBE endp

line07_TBE proc                        ;Transmit Buffer Empty
         push    ax                    ;save register
         mov     ax,offset line07      ;   and setup line table offset
         jmp     TBE_isr               ;do common processing ...
line07_TBE endp

line08_TBE proc                        ;Transmit Buffer Empty
         push    ax                    ;save register
         mov     ax,offset line08      ;   and setup line table offset
         jmp     TBE_isr               ;do common processing ...
line08_TBE endp

line09_TBE proc                        ;Transmit Buffer Empty
         push    ax                    ;save register
         mov     ax,offset line09      ;   and setup line table offset
         jmp     TBE_isr               ;do common processing ...
line09_TBE endp

line10_TBE proc                        ;Transmit Buffer Empty
         push    ax                    ;save register
         mov     ax,offset line10      ;   and setup line table offset
         jmp     TBE_isr               ;do common processing ...
line10_TBE endp

line11_TBE proc                        ;Transmit Buffer Empty
         push    ax                    ;save register
         mov     ax,offset line11      ;   and setup line table offset
         jmp     TBE_isr               ;do common processing ...
line11_TBE endp

line12_TBE proc                        ;Transmit Buffer Empty
         push    ax                    ;save register
         mov     ax,offset line12      ;   and setup line table offset
         jmp     TBE_isr               ;do common processing ...
line12_TBE endp

line13_TBE proc                        ;Transmit Buffer Empty
         push    ax                    ;save register
         mov     ax,offset line13      ;   and setup line table offset
         jmp     TBE_isr               ;do common processing ...
line13_TBE endp

line14_TBE proc                        ;Transmit Buffer Empty
         push    ax                    ;save register
         mov     ax,offset line14      ;   and setup line table offset
         jmp     TBE_isr               ;do common processing ...
line14_TBE endp

line15_TBE proc                        ;Transmit Buffer Empty
         push    ax                    ;save register
         mov     ax,offset line15      ;   and setup line table offset
         jmp     TBE_isr               ;do common processing ...
line15_TBE endp

;------------------------------------------------------------------------------
;Name:    lineXX_ESC
;Purpose: External Status Change Interrupt Service Routine.  There is one of
;         these for each line.  Since each line has identical requirements
;         on ESC, each of these jumps to a common ESC_isr.
;Entry:   Nothing
;Exit:    AX = line table entry

line00_ESC proc                        ;External/Status Change
         push    ax                    ;save register
         mov     ax,offset line00      ;   and setup line table offset
         jmp     ESC_isr               ;do common processing ...
line00_ESC endp

line01_ESC proc                        ;External/Status Change
         push    ax                    ;save register
         mov     ax,offset line01      ;   and setup line table offset
         jmp     ESC_isr               ;do common processing ...
line01_ESC endp

line02_ESC proc                        ;External/Status Change
         push    ax                    ;save register
         mov     ax,offset line02      ;   and setup line table offset
         jmp     ESC_isr               ;do common processing ...
line02_ESC endp

line03_ESC proc                        ;External/Status Change
         push    ax                    ;save register
         mov     ax,offset line03      ;   and setup line table offset
         jmp     ESC_isr               ;do common processing ...
line03_ESC endp

line04_ESC proc                        ;External/Status Change
         push    ax                    ;save register
         mov     ax,offset line04      ;   and setup line table offset
         jmp     ESC_isr               ;do common processing ...
line04_ESC endp

line05_ESC proc                        ;External/Status Change
         push    ax                    ;save register
         mov     ax,offset line05      ;   and setup line table offset
         jmp     ESC_isr               ;do common processing ...
line05_ESC endp

line06_ESC proc                        ;External/Status Change
         push    ax                    ;save register
         mov     ax,offset line06      ;   and setup line table offset
         jmp     ESC_isr               ;do common processing ...
line06_ESC endp

line07_ESC proc                        ;External/Status Change
         push    ax                    ;save register
         mov     ax,offset line07      ;   and setup line table offset
         jmp     ESC_isr               ;do common processing ...
line07_ESC endp

line08_ESC proc                        ;External/Status Change
         push    ax                    ;save register
         mov     ax,offset line08      ;   and setup line table offset
         jmp     ESC_isr               ;do common processing ...
line08_ESC endp

line09_ESC proc                        ;External/Status Change
         push    ax                    ;save register
         mov     ax,offset line09      ;   and setup line table offset
         jmp     ESC_isr               ;do common processing ...
line09_ESC endp

line10_ESC proc                        ;External/Status Change
         push    ax                    ;save register
         mov     ax,offset line10      ;   and setup line table offset
         jmp     ESC_isr               ;do common processing ...
line10_ESC endp

line11_ESC proc                        ;External/Status Change
         push    ax                    ;save register
         mov     ax,offset line11      ;   and setup line table offset
         jmp     ESC_isr               ;do common processing ...
line11_ESC endp

line12_ESC proc                        ;External/Status Change
         push    ax                    ;save register
         mov     ax,offset line12      ;   and setup line table offset
         jmp     ESC_isr               ;do common processing ...
line12_ESC endp

line13_ESC proc                        ;External/Status Change
         push    ax                    ;save register
         mov     ax,offset line13      ;   and setup line table offset
         jmp     ESC_isr               ;do common processing ...
line13_ESC endp

line14_ESC proc                        ;External/Status Change
         push    ax                    ;save register
         mov     ax,offset line14      ;   and setup line table offset
         jmp     ESC_isr               ;do common processing ...
line14_ESC endp

line15_ESC proc                        ;External/Status Change
         push    ax                    ;save register
         mov     ax,offset line15      ;   and setup line table offset
         jmp     ESC_isr               ;do common processing ...
line15_ESC endp


;------------------------------------------------------------------------------
;Name:    lineXX_RCA
;Purpose: Receive Character Available  Interrupt Service Routine.  There is one
;         of these for each line.  Since each line has identical requirements
;         on RCA, each of these jumps to a common RCA_isr.
;Entry:   Nothing
;Exit:    AX = line table entry

line00_RCA proc                        ;Receive Character Available
         push    ax                    ;save register
         mov     ax,offset line00      ;  and setup interrupt type
         jmp     RCA_isr               ;do common processing ...
line00_RCA endp

line01_RCA proc                        ;Receive Character Available
         push    ax                    ;save register
         mov     ax,offset line01      ;  and setup interrupt type
         jmp     RCA_isr               ;do common processing ...
line01_RCA endp

line02_RCA proc                        ;Receive Character Available
         push    ax                    ;save register
         mov     ax,offset line02      ;  and setup interrupt type
         jmp     RCA_isr               ;do common processing ...
line02_RCA endp

line03_RCA proc                        ;Receive Character Available
         push    ax                    ;save register
         mov     ax,offset line03      ;  and setup interrupt type
         jmp     RCA_isr               ;do common processing ...
line03_RCA endp

line04_RCA proc                        ;Receive Character Available
         push    ax                    ;save register
         mov     ax,offset line04      ;  and setup interrupt type
         jmp     RCA_isr               ;do common processing ...
line04_RCA endp

line05_RCA proc                        ;Receive Character Available
         push    ax                    ;save register
         mov     ax,offset line05      ;  and setup interrupt type
         jmp     RCA_isr               ;do common processing ...
line05_RCA endp

line06_RCA proc                        ;Receive Character Available
         push    ax                    ;save register
         mov     ax,offset line06      ;  and setup interrupt type
         jmp     RCA_isr               ;do common processing ...
line06_RCA endp

line07_RCA proc                        ;Receive Character Available
         push    ax                    ;save register
         mov     ax,offset line07      ;  and setup interrupt type
         jmp     RCA_isr               ;do common processing ...
line07_RCA endp

line08_RCA proc                        ;Receive Character Available
         push    ax                    ;save register
         mov     ax,offset line08      ;  and setup interrupt type
         jmp     RCA_isr               ;do common processing ...
line08_RCA endp

line09_RCA proc                        ;Receive Character Available
         push    ax                    ;save register
         mov     ax,offset line09      ;  and setup interrupt type
         jmp     RCA_isr               ;do common processing ...
line09_RCA endp

line10_RCA proc                        ;Receive Character Available
         push    ax                    ;save register
         mov     ax,offset line10      ;  and setup interrupt type
         jmp     RCA_isr               ;do common processing ...
line10_RCA endp

line11_RCA proc                        ;Receive Character Available
         push    ax                    ;save register
         mov     ax,offset line11      ;  and setup interrupt type
         jmp     RCA_isr               ;do common processing ...
line11_RCA endp

line12_RCA proc                        ;Receive Character Available
         push    ax                    ;save register
         mov     ax,offset line12      ;  and setup interrupt type
         jmp     RCA_isr               ;do common processing ...
line12_RCA endp

line13_RCA proc                        ;Receive Character Available
         push    ax                    ;save register
         mov     ax,offset line13      ;  and setup interrupt type
         jmp     RCA_isr               ;do common processing ...
line13_RCA endp

line14_RCA proc                        ;Receive Character Available
         push    ax                    ;save register
         mov     ax,offset line14      ;  and setup interrupt type
         jmp     RCA_isr               ;do common processing ...
line14_RCA endp

line15_RCA proc                        ;Receive Character Available
         push    ax                    ;save register
         mov     ax,offset line15      ;  and setup interrupt type
         jmp     RCA_isr               ;do common processing ...
line15_RCA endp

;------------------------------------------------------------------------------
;Name:    lineXX_SRC
;Purpose: Special Receive Condition Interrupt Service Routine.  There is one
;         of these for each line.  Since each line has identical requirements
;         on SRC, each of these jumps to a common SRC_isr.
;Entry:   Nothing
;Exit:    AX = line table entry

line00_SRC proc                        ;Special Receive Condition
         push    ax                    ;save register
         mov     ax,offset line00      ;  and setup interrupt type
         jmp     SRC_isr               ;do common processing ...
line00_SRC endp

line01_SRC proc                        ;Special Receive Condition
         push    ax                    ;save register
         mov     ax,offset line01      ;  and setup interrupt type
         jmp     SRC_isr               ;do common processing ...
line01_SRC endp

line02_SRC proc                        ;Special Receive Condition
         push    ax                    ;save register
         mov     ax,offset line02      ;  and setup interrupt type
         jmp     SRC_isr               ;do common processing ...
line02_SRC endp

line03_SRC proc                        ;Special Receive Condition
         push    ax                    ;save register
         mov     ax,offset line03      ;  and setup interrupt type
         jmp     SRC_isr               ;do common processing ...
line03_SRC endp

line04_SRC proc                        ;Special Receive Condition
         push    ax                    ;save register
         mov     ax,offset line04      ;  and setup interrupt type
         jmp     SRC_isr               ;do common processing ...
line04_SRC endp

line05_SRC proc                        ;Special Receive Condition
         push    ax                    ;save register
         mov     ax,offset line05      ;  and setup interrupt type
         jmp     SRC_isr               ;do common processing ...
line05_SRC endp

line06_SRC proc                        ;Special Receive Condition
         push    ax                    ;save register
         mov     ax,offset line06      ;  and setup interrupt type
         jmp     SRC_isr               ;do common processing ...
line06_SRC endp

line07_SRC proc                        ;Special Receive Condition
         push    ax                    ;save register
         mov     ax,offset line07      ;  and setup interrupt type
         jmp     SRC_isr               ;do common processing ...
line07_SRC endp

line08_SRC proc                        ;Special Receive Condition
         push    ax                    ;save register
         mov     ax,offset line08      ;  and setup interrupt type
         jmp     SRC_isr               ;do common processing ...
line08_SRC endp

line09_SRC proc                        ;Special Receive Condition
         push    ax                    ;save register
         mov     ax,offset line09      ;  and setup interrupt type
         jmp     SRC_isr               ;do common processing ...
line09_SRC endp

line10_SRC proc                        ;Special Receive Condition
         push    ax                    ;save register
         mov     ax,offset line10      ;  and setup interrupt type
         jmp     SRC_isr               ;do common processing ...
line10_SRC endp

line11_SRC proc                        ;Special Receive Condition
         push    ax                    ;save register
         mov     ax,offset line11      ;  and setup interrupt type
         jmp     SRC_isr               ;do common processing ...
line11_SRC endp

line12_SRC proc                        ;Special Receive Condition
         push    ax                    ;save register
         mov     ax,offset line12      ;  and setup interrupt type
         jmp     SRC_isr               ;do common processing ...
line12_SRC endp

line13_SRC proc                        ;Special Receive Condition
         push    ax                    ;save register
         mov     ax,offset line13      ;  and setup interrupt type
         jmp     SRC_isr               ;do common processing ...
line13_SRC endp

line14_SRC proc                        ;Special Receive Condition
         push    ax                    ;save register
         mov     ax,offset line14      ;  and setup interrupt type
         jmp     SRC_isr               ;do common processing ...
line14_SRC endp

line15_SRC proc                        ;Special Receive Condition
         push    ax                    ;save register
         mov     ax,offset line15      ;  and setup interrupt type
         jmp     SRC_isr               ;do common processing ...
line15_SRC endp


;------------------------------------------------------------------------------
;Name:    vector_init
;Purpose: Initialize SCC interrupt vectors.  Each vector requires 4 bytes.
;         Since the SCC modifies bits 3, 2, and 1 of the base vector type, but
;         does not modify bit 0, every second vector is unused.  The unused
;         vectors have already been initialized to point to an "invalid
;         interrupt ISR" by the firmware, so they are not altered here.
;Entry:   AX = base vector ype
;Exit:    Nothing
;Registers AX, BX, CX, SI, DI and ES altered

vector_init proc
         shl     ax,1                  ;calculate interrupt vector address
         shl     ax,1
         mov     di,ax
         xor     ax,ax
         mov     es,ax                 ;ES:DI=> destination
         mov     si,offset vector_tbl  ;SI=> vector table
         mov     cx,[si]
         add     si,2
         shr     cx,1                  ;CX = table length (words)
         mov     bx,cs                 ;setup segment address
vector_init_10:
         movsw                         ;move vector offset
         mov     ax,bx
         stosw
         add     di,4                  ;skip unused vector entry
         loop    vector_init_10        ;continue ...
vector_init_99:
         ret
vector_init endp

;------------------------------------------------------------------------------
;SCC Interrupt Vector Table

vector_tbl       equ     $
         dw      vector_tbl_end-$-2    ;table length

         dw      line01_TBE
         dw      line01_ESC
         dw      line01_RCA
         dw      line01_SRC

         dw      line00_TBE
         dw      line00_ESC
         dw      line00_RCA
         dw      line00_SRC

         dw      line03_TBE
         dw      line03_ESC
         dw      line03_RCA
         dw      line03_SRC

         dw      line02_TBE
         dw      line02_ESC
         dw      line02_RCA
         dw      line02_SRC

         dw      line05_TBE
         dw      line05_ESC
         dw      line05_RCA
         dw      line05_SRC

         dw      line04_TBE
         dw      line04_ESC
         dw      line04_RCA
         dw      line04_SRC

line07_entry     label   word
         dw      line07_TBE
         dw      line07_ESC
         dw      line07_RCA
         dw      line07_SRC

         dw      line06_TBE
         dw      line06_ESC
         dw      line06_RCA
         dw      line06_SRC

         dw      line09_TBE
         dw      line09_ESC
         dw      line09_RCA
         dw      line09_SRC

         dw      line08_TBE
         dw      line08_ESC
         dw      line08_RCA
         dw      line08_SRC

         dw      line11_TBE
         dw      line11_ESC
         dw      line11_RCA
         dw      line11_SRC

         dw      line10_TBE
         dw      line10_ESC
         dw      line10_RCA
         dw      line10_SRC

         dw      line13_TBE
         dw      line13_ESC
         dw      line13_RCA
         dw      line13_SRC

         dw      line12_TBE
         dw      line12_ESC
         dw      line12_RCA
         dw      line12_SRC

         dw      line15_TBE
         dw      line15_ESC
         dw      line15_RCA
         dw      line15_SRC

         dw      line14_TBE
         dw      line14_ESC
         dw      line14_RCA
         dw      line14_SRC

vector_tbl_end   equ     $
line07_count     equ     (($-line07_entry)/2)


;------------------------------------------------------------------------------
;Miscellaneous data and stack

         db      'Comtrol HOSTESS i Sample Control Program' ,0
         db      'Copyright (C) 1991 Comtrol Corp.  ',0
         db      'All rights reserved.',0

release equ     $
         db      '1.00   ',0
release_len     equ     $-release

msg_area db      16 dup (?)            ;message area

         public  bos
bos      label   word                  ;bottom of stack
         db      512     dup     (?)   ;stack size = 512 bytes
         public  tos
tos      label   word                  ;top of stack


         org     1000h
;-----------------------------------------------------------------------------
         public  Comq,Sysq
;Comq - Queue for messages from System Processor to Communications Processor
Comq     msgq_entry      <>

;Sysq - Queue for messages from Communications Processor to Systems Processor
Sysq     msgq_entry      <>

         dw      4 dup (?)             ;filler

;-----------------------------------------------------------------------------
;Line table, one entry for each line

         public  line00,line01,line02,line03,line04,line05,line06,line07
         public  line08,line09,line10,line11,line12,line13,line14,line15
statints equ break_ie+CTS_ie+DCD_ie
line00   line_entry      <0e1f4h,0,80h,0c0h,04h,60h,bps9600,bps9600 shr 8,\
                          statints,\
                          0,0,0,offset line00_Txb,0,0,offset line00_Rxb,\
                          0,0,0,0>
line01   line_entry      <0e1f0h,0,80h,0c0h,04h,60h,bps9600,bps9600 shr 8,\
                          statints,\
                          0,0,0,offset line01_Txb,0,0,offset line01_Rxb,\
                          0,0,0,0>
line02   line_entry      <0e3f4h,0,90h,0c0h,04h,60h,bps9600,bps9600 shr 8,\
                          statints,\
                          0,0,0,offset line02_Txb,0,0,offset line02_Rxb,\
                          0,0,0,0>
line03   line_entry      <0e3f0h,0,90h,0c0h,04h,60h,bps9600,bps9600 shr 8,\
                          statints,\
                          0,0,0,offset line03_Txb,0,0,offset line03_Rxb,\
                          0,0,0,0>
line04   line_entry      <0e5f4h,0,0a0h,0c0h,04h,60h,bps9600,bps9600 shr 8,\
                          statints,\
                          0,0,0,offset line04_Txb,0,0,offset line04_Rxb,\
                          0,0,0,0>
line05   line_entry      <0e5f0h,0,0a0h,0c0h,04h,60h,bps9600,bps9600 shr 8,\
                          statints,\
                          0,0,0,offset line05_Txb,0,0,offset line05_Rxb,\
                          0,0,0,0>
line06   line_entry      <0e7f4h,0,0b0h,0c0h,04h,60h,bps9600,bps9600 shr 8,\
                          statints,\
                          0,0,0,offset line06_Txb,0,0,offset line06_Rxb,\
                          0,0,0,0>
line07   line_entry      <0e7f0h,0,0b0h,0c0h,04h,60h,bps9600,bps9600 shr 8,\
                          statints,\
                          0,0,0,offset line07_Txb,0,0,offset line07_Rxb,\
                          0,0,0,0>
line08   line_entry      <0e9f4h,0,0c0h,0c0h,04h,60h,bps9600,bps9600 shr 8,\
                          statints,\
                          0,0,0,offset line08_Txb,0,0,offset line08_Rxb,\
                          0,0,0,0>
line09   line_entry      <0e9f0h,0,0c0h,0c0h,04h,60h,bps9600,bps9600 shr 8,\
                          statints,\
                          0,0,0,offset line09_Txb,0,0,offset line09_Rxb,\
                          0,0,0,0>
line10   line_entry      <0ebf4h,0,0d0h,0c0h,04h,60h,bps9600,bps9600 shr 8,\
                          statints,\
                          0,0,0,offset line10_Txb,0,0,offset line10_Rxb,\
                          0,0,0,0>
line11   line_entry      <0ebf0h,0,0d0h,0c0h,04h,60h,bps9600,bps9600 shr 8,\
                          statints,\
                          0,0,0,offset line11_Txb,0,0,offset line11_Rxb,\
                          0,0,0,0>
line12   line_entry      <0edf4h,0,0e0h,0c0h,04h,60h,bps9600,bps9600 shr 8,\
                          statints,\
                          0,0,0,offset line12_Txb,0,0,offset line12_Rxb,\
                          0,0,0,0>
line13   line_entry      <0edf0h,0,0e0h,0c0h,04h,60h,bps9600,bps9600 shr 8,\
                          statints,\
                          0,0,0,offset line13_Txb,0,0,offset line13_Rxb,\
                          0,0,0,0>
line14   line_entry      <0eff4h,0,0f0h,0c0h,04h,60h,bps9600,bps9600 shr 8,\
                          statints,\
                          0,0,0,offset line14_Txb,0,0,offset line14_Rxb,\
                          0,0,0,0>
line15   line_entry      <0eff0h,0,0f0h,0c0h,04h,60h,bps9600,bps9600 shr 8,\
                          statints,\
                          0,0,0,offset line15_Txb,0,0,offset line15_Rxb,\
                          0,0,0,0>

;-----------------------------------------------------------------------------
;Transmit buffers, one for each line

         public  line00_Txb,line01_Txb,line02_Txb,line03_Txb,line04_Txb
         public  line05_Txb,line06_Txb,line07_Txb,line08_Txb,line09_Txb
         public  line10_Txb,line11_Txb,line12_Txb,line13_Txb,line14_Txb
         public  line15_Txb
line00_Txb       db      Txb_size dup (?)
line01_Txb       db      Txb_size dup (?)
line02_Txb       db      Txb_size dup (?)
line03_Txb       db      Txb_size dup (?)
line04_Txb       db      Txb_size dup (?)
line05_Txb       db      Txb_size dup (?)
line06_Txb       db      Txb_size dup (?)
line07_Txb       db      Txb_size dup (?)
line08_Txb       db      Txb_size dup (?)
line09_Txb       db      Txb_size dup (?)
line10_Txb       db      Txb_size dup (?)
line11_Txb       db      Txb_size dup (?)
line12_Txb       db      Txb_size dup (?)
line13_Txb       db      Txb_size dup (?)
line14_Txb       db      Txb_size dup (?)
line15_Txb       db      Txb_size dup (?)

;-----------------------------------------------------------------------------
;Receive buffers, one for each line

         public  line00_Rxb,line01_Rxb,line02_Rxb,line03_Rxb,line04_Rxb
         public  line05_Rxb,line06_Rxb,line07_Rxb,line08_Rxb,line09_Rxb
         public  line10_Rxb,line11_Rxb,line12_Rxb,line13_Rxb,line14_Rxb
         public  line15_Rxb
line00_Rxb       db      Rxb_size dup (?)
line01_Rxb       db      Rxb_size dup (?)
line02_Rxb       db      Rxb_size dup (?)
line03_Rxb       db      Rxb_size dup (?)
line04_Rxb       db      Rxb_size dup (?)
line05_Rxb       db      Rxb_size dup (?)
line06_Rxb       db      Rxb_size dup (?)
line07_Rxb       db      Rxb_size dup (?)
line08_Rxb       db      Rxb_size dup (?)
line09_Rxb       db      Rxb_size dup (?)
line10_Rxb       db      Rxb_size dup (?)
line11_Rxb       db      Rxb_size dup (?)
line12_Rxb       db      Rxb_size dup (?)
line13_Rxb       db      Rxb_size dup (?)
line14_Rxb       db      Rxb_size dup (?)
line15_Rxb       db      Rxb_size dup (?)

_TEXT    ends
         end     start
