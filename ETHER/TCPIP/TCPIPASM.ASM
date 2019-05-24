.model small,c
.386

public shiftlong;
public shiftrong;
public ipcheck;
public tcpcheck;

INCLUDE \common.ipc\os.equ

.data

.code

shiftlong proc near

;   Stack Frame
;
;           +6  bits
;           +4  address of long
;           +2  return
;          bp---bp
   push bp;
   mov bp,sp;
   mov bx,[bp+4]
   mov edx, [bx]
   mov cl,[bp+6]
   shl edx,cl
   mov [bx],edx
   pop bp
   ret
shiftlong endp

shiftrong proc near

;   Stack Frame
;
;           +6  bits
;           +4  address of long
;           +2  return
;          bp---bp
   push bp;
   mov bp,sp;
   mov bx,[bp+4]
   mov edx, [bx]
   mov cl,[bp+6]
   shr edx,cl
   mov [bx],edx
   pop bp
   ret
shiftrong endp

;*************************************************************************
;  Internet header checksum
;    header checksum is calculated for a higher level program to verify
;
;  USAGE:  ipcheck(ptr)
;   short far *ptr;
;
;  this proc knows that the IP header length is found in the first byte
;
;  6  far ptr
;  4  near ptr
;  2  near
;  0  bp

ipcheck PROC    NEAR
        PUSH    BP
        MOV     BP,SP
        PUSH    DS
        MOV     AX,[BP+6]       ; ds for input data
        MOV     DS,AX
        MOV     SI,[BP+4]       ; pointer to data
        MOV     Cl,[si]         ; count of words to test
        and     cx,0fh          ; mask off the header length
        shl     cx,1            ;  (length in 4 byte granularity. if
                                ;   multiplied by 2 gets to be 2 byte
                                ;   granularity or # of shorts)

        XOR     BX,BX
        CLC
CHKSUM:
        LODSW                   ; get next word
        ADC     BX,AX           ; keep adding
        LOOP    CHKSUM          ; til' done

        ADC     BX,0            ; adds the carry bit in
                                ;
        NOT     BX              ; take one more 1-complement

        MOV     AX,BX
        POP     DS
        POP     BP
        RET
ipcheck ENDP

;
;  TCP checksum, has two parts, including support for a pseudo-header
;
;  usage:   tcpcheck(psptr,tcpptr,tcplen)
;            char *psptr,*tcpptr;  pointers to pseudo header and real header
;            int tcplen            length of tcp packet in checksum
;
;    18 short data length
;    16 far data
;    14 near data
;    12 short length
;    10 far tcpptr
;    8  near tcpptr
;    6  far psptr
;    4  near psptr
;    2  return
;    0  bp
tcpcheck        PROC    NEAR
        PUSH    BP
        MOV     BP,SP
        PUSH    DS
        MOV     AX,[BP+6]       ; ds for input data for pseudo-hdr
        MOV     DS,AX
        MOV     SI,[BP+4]       ; pointer to data
        MOV     CX,6            ; length of p-hdr in words

        XOR     BX,BX           ; clear to begin
        CLC
PCHKSUM:
        LODSW                   ; get next word
        ADC     BX,AX           ; keep adding
        LOOP    PCHKSUM         ; til' done

        ADC     BX,0            ; adds the carry bit in
;
; NOW THE TCP HEAD THING
;
        MOV     AX,[BP+10]      ; ds of real stuff
        MOV     DS,AX
        MOV     SI,[BP+08]      ; pointer

        MOV     CX,[BP+12]      ; count of bytes to test
        MOV     DX,CX           ; keep a copy
        SHR     CX,1            ; divide by two, round down

        CLC
RCHKSUM:
        LODSW
        ADC     BX,AX           ; add to previous running sum
        LOOP    RCHKSUM
        ADC     BX,0            ; add the last carry in again

        AND     DX,1            ; odd # of bytes?
        JZ      NOTODD
        LODSB                   ; get that last byte
        XOR     AH,AH           ; clear the high portion
        ADD     BX,AX           ; add the last one in
        ADC     BX,0            ; add the carry in, too

NOTODD:
;
; NOW THE DATA
;
        MOV     AX,[BP+16]      ; ds of real stuff
        MOV     DS,AX
        MOV     SI,[BP+14]      ; pointer

        MOV     CX,[BP+18]      ; count of bytes to test
        MOV     DX,CX           ; keep a copy
        SHR     CX,1            ; divide by two, round down
        cmp     cx,0            ; test to see  if no data
        jne     morethanone     ; if there is data go to morethan1
        CLC
        jmp     ONEBYTE         ;otherwise clear carry, and jmp only1
morethanone:
        CLC
DCHKSUM:
        LODSW
        ADC     BX,AX           ; add to previous running sum
        LOOP    DCHKSUM
ONEBYTE:
        ADC     BX,0            ; add the last carry in again

        AND     DX,1            ; odd # of bytes?
        JZ      NOTODDD
        LODSB                   ; get that last byte
        XOR     AH,AH           ; clear the high portion
        ADD     BX,AX           ; add the last one in
        ADC     BX,0            ; add the carry in, too

NOTODDD:
        NOT     BX              ; take one more 1-complement

        MOV     AX,BX
        POP     DS
        POP     BP
        RET
tcpcheck        ENDP


END


