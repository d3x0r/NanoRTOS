LOCALS
.model large

.data

_HD_Port dw 320h
_FD_Port dw 0
extrn _buffer:word

Drive   STRUC
        head    db 0
        track   db 0
        sector  db 0
ENDS

.code

public _Read
public _Write
;public  _Flop_Read_Track
;public  _Flop_Config
;public  _Flop_Status
;public  _Flop_Write
;public  _Flop_Read
;public  _Flop_Calib
;public  _Flop_Interrupt
;public  _Flop_Write_Deleted
;public  _Flop_ID
;public  _Flop_Read_Deleted
;public  _Flop_Format
;public  _Flop_Seek
;public  _Flop_Scan_Eq
;public  _Flop_Scan_LEq
;public  _Flop_Scan_HEq
;
public  _Hard_Test_Rdy               ;00
;public  _Hard_Calib                  ;01
;public  _Hard_Status                 ;03
;public  _Hard_Format_Disk            ;04
;public  _Hard_Ready_Verify           ;05
;public  _Hard_Format_Track           ;06
;public  _Hard_Format_Bad_Track       ;07
;public  _Hard_Read                   ;08
;public  _Hard_Write                  ;0a
;public  _Hard_Seek                   ;0b
;public  _Hard_Init                   ;0c
;public  _Hard_Read_ECC               ;0d
;public  _Hard_Read_Buf               ;0e
;public  _Hard_Write_Buf              ;0f
;public  _Hard_RAM                    ;70


_Hard_Test_Rdy proc far
;   6   drive #
;   4   far
;   2   near
;   bp- bp
        push    bp
        mov     bp,sp
        mov     al,[bp+6]
        cmp     ax,1                    ;test drive number
        mov     ah,0
        ja      @@error
        mov     cl,5
        shl     ax,cl
        mov     dx,_HD_Port
        xchg    ah,al
        out     dx,al                   ;command
        xchg    ah,al
        out     dx,al                   ;drive
        out     dx,al                   ;? 2
        out     dx,al                   ;? 3
        out     dx,al                   ;? 4
        out     dx,al                   ;? 5
        inc     dx
        in      al,dx                   ;get status
        shr     al,1                    ;put error status in low bit
        and     al,1                    ;mask off just error bit
        not     al                      ;not it: 1=fffe 0=ffff
        inc     al                      ;add 1 : 1=ffff 0=0000
        cbw
        jmp     @@exit
@@error:
        mov     ax,-1
@@exit:
        pop     bp
        ret
_Hard_Test_Rdy endp


_Read proc far
;   18  buffer high
;   16  buffer low
;   14  count
;   12  sector
;   10  track
;   8   head
;   6   drive
;   4   far
;   2   near
;   bp- bp
        push    bp
        mov     bp,sp
        mov     ah,2
        mov     es,[bp+18]
        mov     bx,[bp+16]
        mov     al,[bp+14]
        mov     cx,[bp+10]
        shr     cx,1
        shr     cx,1
        and     cl,0c0h
        or      cl,[bp+12]
        mov     ch,[bp+10]
        mov     dh,[bp+8]
        mov     dl,[bp+6]
        int     13h
        pop     bp
        ret
_Read endp

_Write proc far
;   18  buffer high
;   16  buffer low
;   14  count
;   12  sector
;   10  track
;   8   head
;   6   drive
;   4   far
;   2   near
;   bp- bp
        push    bp
        mov     bp,sp
        mov     ah,3
        mov     es,[bp+18]
        mov     bx,[bp+16]
        mov     al,[bp+14]
        mov     cx,[bp+10]
        shr     cx,1
        shr     cx,1
        and     cl,0c0h
        or      cl,[bp+12]
        mov     ch,[bp+10]
        mov     dh,[bp+8]
        mov     dl,[bp+6]
        int     13h
        pop     bp
        ret
_Write endp

END






