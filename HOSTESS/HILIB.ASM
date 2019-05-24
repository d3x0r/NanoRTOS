;*****************************************************************************
;File:    hilib.asm
;Purpose: A library of sample routines for the HOSTESS i controller
;Company: Comtrol Corporation
;Author:  Lori Slater, Craig Harrison
;Release: 1.00, 8-23-91 - Original release
;*****************************************************************************
;Copyright 1991 Comtrol Corporation. All rights reserved. Subject to developers
;license agreement.
;*****************************************************************************
.model SMALL,c

        .xlist
        include cp.equ
        .list

.data

extrn DPRAM_ADDR:word
;    equ     0d000h      ;System address of HOSTESS i dual port RAM
extrn IO_ADDR:word
;    equ     218h        ;System I/O base address of HOSTESS i

extrn   Relinquish:dword
hiopenmsg       db      1,0,0c0h,44h,60h,0eh,0,0,0,0,0,0,0,0,0,0
hiclosmsg       db      2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
public terminates
terminates      dw      16 dup(0)

.code
;----------------------------------------------------------------------------
;Function:   line_struct far *hiopen(linenum,message)
;Parameters: int linenum;        /* number of line to open (0-15) */
;            char far *message
;Return:     pointer to line struct if successful, NULL if failed
;Purpose:    Open a serial line on the HOSTESS i

         public  hiopen
hiopen   proc    far
         push    bp                      ;prolog, establish stack frame
         mov     bp,sp
         push    ds
         mov     ax,@data
         mov     ds,ax
         assume  ds:_DATA

         assume  es:nothing
         mov     ax,DS:DPRAM_ADDR
         mov     es,ax                   ;es is dpram address

         mov     bx,offset Comq          ;start of comm proc queue
         mov     ax,es:[bx].msgq_head    ;ax = current msg number
         inc     ax                      ;bump pointer
         and     ax,msgq_mask            ;mask it
         cmp     ax,es:[bx].msgq_tail    ;is msg area full?
         mov     ax,0                    ;set up return value for fail
         mov     dx,0                    ;set up return value for fail
         jz      _hiopen_10              ;if it's is full exit now

         push    si                      ;save registers
         push    di

         mov     ax,es:[bx].msgq_head    ;get head again
         mov     cx,msg_len              ;length of a message
         mul     cl                      ;get message offset
         lea     di,[bx].msgq_area       ;get destination
         add     di,ax                   ;es:di = destination
         lds     si,offset [bp+8]        ;ds:si = source
         mov     ax,[bp+6]               ;line number to open
         mov     byte ptr es:[di],1
         inc     di
         dec     cx
         mov     es:[di],al
         inc     di
         dec     cx
         rep     movsb                   ;move the message

         mov     ax,es:[bx].msgq_head    ;get head again
         inc     ax                      ;increment it
         and     ax,msgq_mask            ;mask it
         mov     es:[bx].msgq_head,ax    ;store new head

         mov     ax,@data                ;restore dataseg
         mov     ds,ax

         mov     dx,IO_ADDR              ;interrupt comm proc
         add     dx,2
         out     dx,al

         mov     ax,[bp+6]               ;set up success return value
         call    higet_line
         mov     ax,si
         mov     dx,ds:DPRAM_ADDR
         pop     di
         pop     si

_hiopen_10:
         pop     ds
         pop     bp                      ;epilog
         ret
hiopen  endp

;----------------------------------------------------------------------------
;Function:   int hiclose(linenum)
;Parameters: line_entry far *linenum;        /* number of line to close (0-15) */
;Returns:    1 if successful, 0 if failed
;Purpose:    Close a serial line on the HOSTESS i

         public  hiclose
hiclose  proc    far
         push    bp                      ;prolog, establish stack frame
         mov     bp,sp
         push    ds
         mov    ax,@data
         mov    ds,ax
         assume  ds:_DATA

         mov     ax,ds:DPRAM_ADDR
         mov     es,ax                   ;es = dpram address
         assume  es:nothing

         mov     bx,offset Comq          ;start of comm proc queue
         mov     ax,es:[bx].msgq_head    ;ax = current msg number
         inc     ax                      ;bump pointer
         and     ax,msgq_mask            ;mask it
         cmp     ax,es:[bx].msgq_tail    ;is msg area full?
         mov     ax,0                    ;set up return value for fail
         jz      _hiclose_10             ;if it's is full exit now

         push    si                      ;save registers
         push    di


         mov     ax,es:[bx].msgq_head    ;get head again
         mov     cx,msg_len              ;length of a message
         mul     cl                      ;get message offset
         lea     di,[bx].msgq_area       ;get destination
         add     di,ax                   ;es:di = destination
         mov     si,offset hiclosmsg     ;ds:si = source
         mov     ax,[bp+6]               ;line number to close
         call    higetidx                ;convert pointer to idx
         mov     [si+1],ax               ;2nd byte of msg is line number
         rep     movsb                   ;move the message

         mov     ax,es:[bx].msgq_head    ;get head again
         inc     ax                      ;increment it
         and     ax,msgq_mask            ;mask it
         mov     es:[bx].msgq_head,ax    ;store new head

         mov     dx,IO_ADDR              ;interrupt comm proc
         add     dx,2
         out     dx,al
         pop     di
         pop     si
         mov     ax,1                    ;set up sucess return value

_hiclose_10:
         pop     ds
         pop     bp                      ;epilog
         ret
hiclose endp

;----------------------------------------------------------------------------
;Function:   int hiread(buffer,cnt,linenum)
;Parameters: char *buffer;       /* points to storage buffer */
;            int cnt;            /* maximum count to read */
;            line_entry far * linenum;        /* number (0-7) of serial line */
;Returns:    Integer number of bytes read
;Purpose:    Read bytes from an open serial line on the HOSTESS i

         public  hiread
hiread   proc    far
         push    bp                      ;prolog, establish stack frame
         mov     bp,sp
         sub     sp,2

         push    si                      ;save registers
         push    di
         push    ds
         mov     ax,@data
         mov     ds,ax
         assume  ds:_DATA

         lds     di,[bp+12]
         assume  ds:nothing

         mov     es,[bp+8]               ;es = seg of buffer
         assume  es:nothing

         mov     ax,[bp+10]               ;ax = count
         mov     cx,ds:[di].Rxq_head     ;get head
         mov     bx,ds:[di].Rxq_tail     ;get tail
         sub     cx,bx                   ;cx = head - tail
         jb      _hiread_10              ;if cx < 0 jump
         call     himin                  ;get min of cx and ax
         mov     dx,cx                   ;store min in dx
         jcxz    _hiread_30              ;exit if min = 0 (i.e. empty)

         mov     si,ds:[di].Rxq_offset   ;si = receive buffer
         add     si,bx                   ;si = current loc in buffer
         mov     di,[bp+6]               ;di = local buffer
         rep     movsb                   ;read bytes into buffer

         jmp     short _hiread_20        ;jump
_hiread_10:
         add     cx,Rxb_size             ;adjust cx
         call    himin                   ;now get min of avail and size again
         mov     dx,cx                   ;store min
         jcxz    _hiread_30              ;exit if min = 0

         mov     ax,Rxb_size             ;ax = size
         sub     ax,bx                   ;ax = size - tail
         call    himin                   ;get min of size and data from
                                         ;tail to EOB
         mov     si,ds:[di].Rxq_offset   ;si = receive buffer
         add     si,bx                   ;si = current loc in buffer
         mov     di,[bp+6]               ;di = local buffer
         rep     movsb                   ;read bytes into buffer
         mov     cx,dx                   ;cx = full min count
         sub     cx,ax                   ;subtract what we just read
         jcxz    _hiread_20              ;exit if 0

         mov     bx,word ptr line               ;restore start of line table
         mov     si,ds:[bx].Rxq_offset   ;restore start of buffer
         rep     movsb                   ;read rest into local buffer

_hiread_20:
         mov     di,word ptr line               ;di = line table
         mov     bx,ds:[di].Rxq_offset   ;bx = receive buffer
         sub     si,bx                   ;get new tail
         and     si,Rxq_mask             ;mask it
         mov     ds:[di].Rxq_tail,si     ;store it

_hiread_30:
         mov     ax,dx                   ;return value = number of bytes read
         pop     ds                      ;epilog
         pop     di
         pop     si
         mov     sp,bp
         pop     bp
         ret
hiread   endp

;----------------------------------------------------------------------------
;Function:   int hiwrite(buffer,cnt,linenum)
;Parameters: char *buffer;       /* points to buffer to write */
;            int cnt;            /* number of bytes to write */
;            line_entry far * linenum;        /* number (0-7) of serial line */
;Returns:    Integer number of bytes written
;Purpose:    Write bytes to an open serial line on the HOSTESS i

        public   hiwrite
hiwrite  proc    far,buffer:dword,length:word,line:dword
         push    si                      ;save registers
         push    di
         push    ds
         push    es
         les     si,line

         assume  es:nothing

         mov     ds,word ptr buffer+2    ;ds = seg of buffer
         assume  ds:nothing

_hiwrite_5:
         mov     ax,length               ;ax = count
         mov     cx,es:[si].Txq_tail     ;get tail
         mov     bx,es:[si].Txq_head     ;get head
         sub     cx,bx                   ;get difference
         jbe     _hiwrite_10             ;branch if tail <= head

         dec     cx                      ;adjust tail - head
         call    himin                   ;return minimum of ax and cx
         mov     dx,cx                   ;save minimum in dx
         jcxz    _hiwrite_30             ;exit if min = 0 (i.e. full)

         mov     di,es:[si].Txq_offset   ;get offset of buffer
         add     di,bx                   ;get starting loc in buffer
         mov     si,word ptr buffer      ;si = start of write buffer
         rep     movsb                   ;write buffer to dpm
         jmp     short _hiwrite_20       ;done

_hiwrite_10:
         add     cx,Txq_mask             ;adjust tail - head
         call    himin
         mov     dx,cx                   ;store minimum in dx
         jcxz    _hiwrite_30             ;exit if min = 0

         mov     ax,Txq_mask             ;ax = mask
         sub     ax,bx                   ;ax -= head
         inc     ax                      ;# we can write at end of buffer
         call    himin
         mov     di,es:[si].Txq_offset   ;get buffer offset
         add     di,bx                   ;get starting loc in buffer
         mov     si,word ptr buffer      ;si = start of write buffer
         rep     movsb                   ;write buffer to dpm

         mov     cx,dx                   ;cx = count
         sub     cx,ax                   ;subtract what we just wrote
         jcxz    _hiwrite_20             ;exit if done

         mov     bx,word ptr line               ;get line table pointer
         mov     di,es:[bx].Txq_offset   ;reset buffer pointer to start
         rep     movsb                   ;write rest of buffer
_hiwrite_20:
         mov     si,word ptr line               ;si = line table
         mov     bx,es:[si].Txq_offset   ;get offset of buffer
         sub     di,bx                   ;get new head pointer
         and     di,Txq_mask             ;mask it
         mov     es:[si].Txq_head,di     ;store new head
_hiwrite_30:
         mov     ax,dx                   ;return = number of bytes written
         pop     es
         pop     ds
         pop     di                      ;epilog
         pop     si
         ret
hiwrite  endp

;----------------------------------------------------------------------------

higet_line proc
         mov     cl,line_entry_len       ;cl = length of a line table
         mul     cl                      ;mult linenum by length
         add     ax,offset line00        ;add start of line tables
         mov     si,ax                   ;return in si
         ret
higet_line endp

;----------------------------------------------------------------------------

higetidx proc
         sub     ax,offset line00        ;add start of line tables
         xor     dx,dx
         mov     cl,line_entry_len       ;cl = length of a line table
         div     cl                      ;mult linenum by length
         ret
higetidx endp

;----------------------------------------------------------------------------

himin    proc
         cmp     ax,cx                   ;compare the two values
         jnc     himin_10                       ; branch if ax >= cx
         mov     cx,ax                   ;make cx = ax
         jmp     short himin_99
himin_10:
         mov     ax,cx                   ;make ax = cx
himin_99:
         ret
himin    endp


;----------------------------------------------------------------------------
;This segment forms a template for dual port RAM so that offsets can be
;computed.  It does not actually reserve any memory.

DPRAM   segment para AT 0
                 org 0h
;The first 80h bytes are defined by the firmware
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

         org     1000h
;-----------------------------------------------------------------------------
;Comq - Queue for messages from System Processor to Communications Processor
Comq     msgq_entry      <>

;Sysq - Queue for messages from Communications Processor to Systems Processor
Sysq     msgq_entry      <>

         dw      4 dup (?)             ;filler

;-----------------------------------------------------------------------------
;Line table, one entry for each line

line00   line_entry      <>
line01   line_entry      <>
line02   line_entry      <>
line03   line_entry      <>
line04   line_entry      <>
line05   line_entry      <>
line06   line_entry      <>
line07   line_entry      <>
line08   line_entry      <>
line09   line_entry      <>
line10   line_entry      <>
line11   line_entry      <>
line12   line_entry      <>
line13   line_entry      <>
line14   line_entry      <>
line15   line_entry      <>

;-----------------------------------------------------------------------------
;Transmit buffers, one for each line

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

DPRAM   ends

        end
