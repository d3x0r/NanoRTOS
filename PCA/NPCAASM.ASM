.model small,C
.386

INCLUDE NPCAASM.INC
INCLUDE \common.ipc\OS.EQU

.data
extrn base:word
extrn control:word
extrn int_work:word
extrn int_queues:word
extrn Allocate:dword
extrn do_transfer:byte
extrn process_control:dword
extrn Relinquish:dword
extrn Connect_Int:dword
extrn Disconnect_Int:dword
extrn Exit:dword
extrn filling:byte
extrn input_task:dword
extrn gettime:dword
extrn eventime:dword
extrn oddtime:dword
extrn first_work:dword

.code
public write_mail
public read_mailbox
public connect_int
public out_port
public read_port
public write_port
public copy_mail
public send_mail
public send_break
public read_all_mail
public disconnect
public outportb
public inportb
public shiftlong;
public shiftrong;

shiftlong proc near,number:dword,bits:byte
   les bx,number
   mov edx,es:[bx]
   mov cl,bits
   shl edx,cl
   mov es:[bx],edx
   ret
shiftlong endp

shiftrong proc near,number:dword,bits:byte
   les bx,number
   mov edx,es:[bx]
   mov cl,bits
   shr edx,cl
   mov es:[bx],edx
   ret
shiftrong endp


read_all_mail proc near,array:dword
  push di
  mov dx,base
  add dx,1ch
  mov ax,0
  out dx,ax
  les di,array
  add dx,2
  mov cx,256
  rep insw
  pop di
  ret
read_all_mail endp

out_port proc near,port_ofs:word,data:word
    mov dx,base
    add dx,port_ofs
    mov ax,data
    out dx,ax
    ret
out_port endp

outportb proc near,port:word,data:byte
    mov dx,port
    mov al,data
    out dx,al
    ret
outportb endp

inportb proc near,port:word
    mov dx,port
    in al,dx
    cbw
    ret
inportb endp


read_port proc near,port_offset:word
    mov dx,base
    add dx,port_offset
    in ax,dx
    ret
read_port endp

write_port proc near,port_offset:word,value:word
    mov dx,base
    add dx,port_offset
    mov ax,value
    out dx,ax
    ret
write_port endp


send_mail proc near,mail_start:word,struct:dword
  push di
  mov dx,base
  add dx,mail_address
  mov ax,mail_start
  out dx,ax
  les di,struct
  add dx,mail_data-mail_address
  mov ax,es:[di].Return.Extended_Size
  add ax,Base_Return_mail
  xchg al,ah
  out dx,ax
  write_data es:[di].Return,Opcode,es:[di].Node,Rex      ;1
  write_data es:[di].Node,Node_Address                   ;2
  write_data es:[di].Return,StatusRD                       ;1
  write_data es:[di].Return,Byte_cnt                     ;2
  write_data es:[di].Return,XStatus                      ;1
  write_data es:[di].Node,FPI                            ;2
;  jmp leave_Write
WriteRaw:
  mov cx,es:[di].Return.Extended_Size
  cmp cx,0
  jne send_extra
  jmp leave_Write
send_extra:
  push ds
  push si
  lds si,es:[di].Return.Extended_mail
send_extras:
  lodsw
  out dx,ax
  loop send_extras
  pop si
  pop ds
  jmp leave_Write

leave_Write:
  pop di
  ret
send_mail endp

send_break proc near,mail_start:word,struct:dword
  push di
  mov dx,base
  add dx,mail_address
  mov ax,mail_start
  out dx,ax
  les di,struct
  add dx,mail_data-mail_address
  mov ax,es:[di].Return.Extended_Size
  add ax,Base_break_mail
  xchg al,ah
  out dx,ax
  write_data es:[di].Return,Opcode,es:[di].Node,Rex      ;1
  write_data es:[di].Return,TCL_idx,es:[di].Return,TCL_data             ;1
  write_data es:[di].Return,BTransport                   ;1
  write_data es:[di].Return,SubChannel                   ;1
  pop di
  ret
send_break endp


copy_mail proc near,mail_start:word,struct:dword
  push di
  les di,struct
  mov   es:[di].Return.Opcode,-1
  xor bx,bx
read_mail_now:
  mov dx,base
  add dx,mail_address
  mov ax,mail_start
  out dx,ax
  add dx,mail_data-mail_address ; we are already on the address so
                                ;       add differnce

  in ax,dx                  ;get mail size
  xchg al,ah                ;reverse it
  test ax,8000h             ;check to see if this is a retransmit packet
  jz not_retrans
  xor ah,ah
  or  es:[di].Tracking.StatusNT,80h
not_retrans:
  cmp ax,0                  ;test to see if there is mail
  jne contin_copy
  mov es:[di].Return.Opcode,-2
  mov ax,false
  jmp leave_copy
contin_copy:
  push dx
  cmp ax,min_mail
  jge alloc_ok
retry_mail:
  cmp bx,-1
  je  exit_with_error
  mov bx,-1
  jmp read_mail_now
exit_with_error:
  mov ax,false
  mov es:[di].Return.Opcode,-4
  jmp leave_copy
alloc_ok:
  cmp ax,max_mail
  ja retry_mail
  mov cx,ax                 ;store count of mail
  pop dx
  read_data es:[di].Node,Transport
  read_data es:[di].Node,Channel
  read_data es:[di].Node,Node_Address
  read_data es:[di].Node,Rex
  read_data es:[di].Node,Options
  read_data es:[di].Node,Byte_Count
  read_data es:[di].Node,XOptions
  read_data es:[di].Node,FPI
  push  es
  add   di,Tracking.Start
  push  di
  call  gettime
  pop   di
  pop   es
  sub   di,Tracking.Start

  sub cx,min_mail
  jbe loop_done
  add di,Node.Info
Read_Raw_loop:
  in  ax,dx
  stosw
  loop Read_Raw_loop;
loop_done:
  test es:[di].Tracking.StatusNT,80h
  jz dont_check
  push ds
  push bx
  lds bx,first_work
test_next:
  mov dx,ds
  or dx,bx
  jz done_testing
  mov dx,es
  mov ax,ds
  cmp ax,dx
  je  get_next
  mov dx,word ptr es:[di].Node.Node_Address
  cmp word ptr ds:[bx].Node.Node_Address,dx
  jne get_next;
  mov dx,word ptr es:[di].Node.Node_Address+2
  cmp word ptr ds:[bx].Node.Node_Address+2,dx
  jne get_next
  mov es:[di].Return.Opcode,-3
  mov ax,false
  pop bx
  pop ds
  jmp leave_copy
get_next:
  lds bx,ds:[bx].Tracking.Next
  jmp test_next;
done_testing:
  pop bx
  pop ds
dont_check:
  mov dx,base              ;indicate that mail has been read
  add dx,mail_address
  mov ax,mail_start
  out dx,ax
  add dx,mail_data-mail_address
  xor ax,ax
  out dx,ax
  mov ax,true
leave_copy:
  pop di
  ret
copy_mail endp


read_mailbox proc near,mailbox:word
  mov dx,base
  add dx,mail_address
  mov ax,mailbox
  out dx,ax
  add dx,mail_data-mail_address
  in ax,dx
  ret
read_mailbox endp

write_mail proc near,mailbox:word,value:word
  mov dx,base
  add dx,mail_address
  mov ax,mailbox
  out dx,ax
  add dx,mail_data-mail_address
  mov ax,value
  out dx,ax
  ret
write_mail endp

;sread_mailbox proc near,value:word
;  mov dx,base
;  add dx,mail_address
;  mov ax,value
;  out dx,ax
;  add dx,mail_data-mail_address
;  in ax,dx
;  xchg al,ah
;  ret
;sread_mailbox endp

;lread_mailbox proc near,which:word
;  mov dx,base
;  add dx,mail_address
;  mov ax,which;
;  out dx,ax
;  add dx,mail_data-mail_address
;  in ax,dx
;  xchg al,ah
;  push ax
;  in ax,dx
;  xchg al,ah
;  pop dx
;  ret
;lread_mailbox endp

IRQ_ack dw 20h

TESTBIT MACRO name,next
LOCAL Jmp1
  test ax,INT_&name
  jz next
  push bx
  push ax
  add dx,REG_&name-REG_INTQ
  in  ax,dx
  mov bx,int_work
  and bx,name
  shr bx,POS_&name
  shl bx,1
  mov int_queues+QUEUE_&name+bx,ax
  shr bx,1
  inc bx
  shl bx,POS_&name
  and int_work,NOT name
  or  int_work,bx
  pop ax
  pop bx
  add dx,REG_INTQ-REG_&name
ENDM

;passes  db 0
last_IRQ dd 0h
_PCA_int proc far
  push dx
  push ax
  push bx
  push cx
  push ds
  mov   ax,@data
  mov   ds,ax
  cmp   filling,1h
  jne   okay
  jmp   return_now
okay:
;  mov   cs:passes,0
  mov   cx,base
  add   cx,REG_CONTROL
  mov   bx,control
  mov dx,base                     ;IQ register
  add dx,REG_INTQ
try_for_more:
  in  ax,dx                       ;get IQ
  and ax,0ffh
  jz restore
;  cmp   cs:passes,1
;  jne   first_pass
;  jmp   clearcurrent
;first_pass:
;  inc   cs:passes


  TESTBIT EVENTERM check_other_term
check_other_term:
  TESTBIT ODDTERM checktcs
checktcs:
  TESTBIT EVENTC nexttc
  xchg  dx,cx
  xchg  ax,bx
  and   ax,0fffeh
  out   dx,ax
  xchg  dx,cx
  xchg  ax,bx
nexttc:
  TESTBIT ODDTC actualselect
  xchg  dx,cx
  xchg  ax,bx
  and   ax,0fffdh
  out   dx,ax
  xchg  dx,cx
  xchg  ax,bx
actualselect:
  TESTBIT EVENSEL checkother
checkother:
  TESTBIT ODDSEL check_ti
check_ti:
  TESTBIT EVENTI check_other_ti
check_other_ti:
  TESTBIT ODDTI clearcurrent
  cmp   do_transfer,0    ;check to see if there is a channel number to
                             ; transfer
  je    no_dma_start
  push  ax
  mov   al,do_transfer   ;get channel number
  mov   cs:do_transfer,0
  out   0d4h,al              ;start dma
  pop   ax
no_dma_start:
  push  ax
  sub   dx,4
  in    ax,dx
  cmp   ax,88h
  jne   not_fill
  mov   filling,1h
not_fill:
  add   dx,4
  pop   ax
  je    restore
clearcurrent:
        out     dx,ax
        jmp     try_for_more
restore:
        push    es
        push    bx
        les     bx,process_control
        or      word ptr es:[bx],1           ;make task list scan from top NOW
        les     bx,input_task
        mov     word ptr es:[bx].status,0    ;zero input task status
        mov     word ptr es:[bx+2].status,0  ;zero input task status
        pop     bx
        pop     es
        mov     control,bx
return_now:
        push    es
        push    di
        test    int_work,EVENTC+EVENSEL+EVENTERM+EVENTI
        jz      test_odd
        push    seg     eventime
        push    offset     eventime
        call    gettime
        add     sp,4
test_odd:
        test    int_work,ODDTC+ODDSEL+ODDTERM+ODDTI
        jz      clear_int
        push    seg     oddtime
        push    offset     oddtime
        call    gettime
        add     sp,4
clear_int:
        pop     di
        pop     es
return_int:
        mov     al,20h
        out     20h,al
        mov     dx,cs:IRQ_ack
        cmp     dx,0a0h
        jne     already_reset
        out     dx,al
already_reset:
        pop     ds
        pop     cx
        pop     bx
        pop     ax
        pop     dx
  iret
_PCA_int endp
IRQ_addr dw 0000h   ;0
         dw 0000h   ;1
         dw 000ah   ;2
         dw 000bh   ;3
         dw 000ch   ;4
         dw 000dh   ;5
         dw 000eh   ;6
         dw 000fh   ;7
         dw 0070h   ;8
         dw 0071h   ;9
         dw 0072h   ;10
         dw 0073h   ;11
         dw 0074h   ;12
         dw 0075h   ;13
         dw 0076h   ;14
         dw 0077h   ;15

irqbase_table   dw 20h  ;0
                dw 20h  ;1
                dw 20h  ;2
                dw 20h  ;3
                dw 20h  ;4
                dw 20h  ;5
                dw 20h  ;6
                dw 20h  ;7
                dw 0a0h  ;8
                dw 0a0h  ;9
                dw 0a0h  ;10
                dw 0a0h  ;11
                dw 0a0h  ;12
                dw 0a0h  ;13
                dw 0a0h  ;14
                dw 0a0h  ;15


connect_int proc near
;   Stack Frame
;
;
;              irq num
;        +2    near
;        bp  - bp
  push bp
  mov bp,sp
  mov bx,[bp+4]                              ;mov irq num to bx
  xor bh,bh
  shl bx,1                                 ;make bx a  word index
  mov di,[IRQ_addr+bx]
  push  bx
  push  di
  push  cs
  push  offset _PCA_int
  call  Connect_Int
  add   sp,6
  pop   bx

  mov ax,[irqbase_table+bx]                ;get ack port
  mov IRQ_ack,ax
  mov dx,ax
  inc dx                                   ;put IRQ mask port in dx
  shr bx,1                                 ;make bx the value
  and bx,7                                 ;mask off high bit, because the
                                           ;high IRQs are the same as low
  mov cx,bx
  mov bx,1
  shl bx,cl                                ;bx will contain the bits mask
                                           ;of the IRQ in question

  in al,dx                                 ;get prior mask
  or al,bl                                 ;turn on bit
  xor al,bl                                ;turn off bit
  out dx,al                                ;save mask
  pop bp
  ret

connect_int endp
disconnect proc near
;   Stack Frame
;
;
;              irq num
;        +2    near
;        bp  - bp
  push bp
  mov bp,sp
  mov bx,[bp+4]                              ;mov irq num to bx
  xor bh,bh
  shl bx,1                                 ;make bx a word index
  mov di,[IRQ_addr+bx]
  push  bx
  push  di
  push  cs
  push  offset _PCA_int
  call  Disconnect_Int
  add   sp,6
  pop   bx

  mov ax,[irqbase_table+bx]                ;get ack port
  mov IRQ_ack,ax
  mov dx,ax
  inc dx                                   ;put IRQ mask port in dx
  shr bx,1                                 ;make bx the value
  and bx,7                                 ;mask off high bit, because the
                                           ;high IRQs are the same as low
  mov cx,bx
  mov bx,1
  shl bx,cl                                ;bx will contain the bits mask
                                           ;of the IRQ in question
  in al,dx                                 ;get prior mask
  or al,bl                                 ;turn on bit
  out dx,al                                ;save mask
  pop bp
  ret

disconnect endp

END
