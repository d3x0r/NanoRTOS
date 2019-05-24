DEBUG equ 1

if DEBUG
  BREAK MACRO
    int 3
  ENDM
ELSE
  BREAK MACRO
  ENDM
ENDIF



min_mail      equ 11
max_mail      equ 64
mail_address  equ 1ch
mail_data     equ 1eh
true          equ 1
false         equ 0
Base_Return_mail equ 9
Base_break_mail equ 4
MAX_READ_CLASSES   equ 3
MAX_WRITE_CLASSES  equ 1
READ_RAW      equ 0
WRITE_RAW     equ 0


EVENTC   equ 00003h
ODDTC    equ 0000ch
EVENSEL  equ 00030h
ODDSEL   equ 000c0h
EVENTI   equ 00300h
ODDTI    equ 00c00h
EVENTERM equ 03000h
ODDTERM  equ 0c000h
INT_EVENTC   equ 008h
INT_ODDTC    equ 080h
INT_EVENSEL  equ 004h
INT_ODDSEL   equ 040h
INT_EVENTI   equ 001h
INT_ODDTI    equ 010h
INT_EVENTERM equ 002h
INT_ODDTERM  equ 020h
QUEUE_EVENTC   equ 0*10
QUEUE_ODDTC    equ 1*10
QUEUE_EVENSEL  equ 2*10
QUEUE_ODDSEL   equ 3*10
QUEUE_EVENTI   equ 4*10
QUEUE_ODDTI    equ 5*10
QUEUE_EVENTERM equ 6*10
QUEUE_ODDTERM  equ 7*10
POS_EVENTC   equ 0
POS_ODDTC    equ 2
POS_EVENSEL  equ 4
POS_ODDSEL   equ 6
POS_EVENTI   equ 8
POS_ODDTI    equ 10
POS_EVENTERM equ 12
POS_ODDTERM  equ 14
REG_EVENTC   equ 002h
REG_ODDTC    equ 002h
REG_EVENTERM equ 002h
REG_EVENTI   equ 004h
REG_EVENSEL  equ 006h
REG_ODDTERM  equ 00ah
REG_ODDTI    equ 00ch
REG_ODDSEL   equ 00eh
REG_CONTROL  equ 014h
REG_INTQ     equ 010h

NodeTrack STRUC
    side    db  ?
    Start   dd  ?
    StatusNT  dw  ?
    Source  dd  ?
    Next    dd  ?
    Prior   dd  ?
ENDS

ReturnData STRUC
    Opcode        db  ?
    StatusRD      dw  ?
    Byte_cnt      dd  ?
    XStatus       dw  ?
    Extended_Size dw  ?
    Extended_mail dd  ?
ENDS

BreakReturn STRUC
    Opcode        db  ?
    TCL_data      db  ?
    TCL_idx       db  ?
    BTransport     dw  ?
    SubChannel    dw  ?
ENDS


Etherdata STRUC
  Control   dw ?
  SourceIP  dd ?
  DestIP    dd ?
  SourceTCP dw ?
  DestTCP   dw ?
  fil1      dw ?
  fil2      dw ?
  fil3      dw ?
ENDS

Datatrack STRUC
  Preamble dd ?
  index    dw ?
ENDS


Commdata STRUC
ENDS

Infodata Union
    Ether Etherdata ?
    Comm Commdata ?
ENDS

NodeData STRUC
    Transport    dw  ?
    Channel      dw  ?
    Node_Address dd  ?
    Rex          dw  ?
    Options      dw  ?
    Byte_Count   dd  ?
    XOptions     dw  ?
    FPI          dd  ?
    Info         Infodata ?
ENDS

NodeStruc STRUC
  Return      ReturnData ?
  Tracking    NodeTrack  ?
  Data        dd         ?
  Node        NodeData   ?
  DataHandle  Datatrack  ?
ENDS


read_data MACRO Pointer,variable
  IFE TYPE variable-1
    in ax,dx
    mov byte ptr &Pointer&.&variable&,ah
  ELSE
    IFE TYPE variable-2
      in ax,dx
      xchg ah,al
      mov word ptr &Pointer&.&variable&,ax
    ELSE
      IFE TYPE variable-4
        in ax,dx
        xchg ah,al
        mov word ptr &Pointer&.&variable&+2,ax
        in ax,dx
        xchg ah,al
        mov word ptr &Pointer&.&variable&,ax
      ENDIF
    ENDIF
  ENDIF
ENDM

read_data_straight MACRO Pointer,variable
  IFE TYPE variable-1
    in ax,dx
    mov byte ptr &Pointer&.&variable&,al
  ELSE
    IFE TYPE variable-2
      in ax,dx
      mov word ptr &Pointer&.&variable&,ax
    ELSE
      IFE TYPE variable-4
        in ax,dx
        mov word ptr &Pointer&.&variable&,ax
        in ax,dx
        mov word ptr &Pointer&.&variable&+2,ax
      ENDIF
    ENDIF
  ENDIF
ENDM

write_data MACRO Pointer,variable,Pointer2,variable2
  IFE TYPE variable-1
    mov ah,byte ptr &Pointer&.&variable&
    mov al,byte ptr &Pointer2&.&variable2&
    out dx,ax
  ELSE
    IFE TYPE variable-2
      mov ax,word ptr &Pointer&.&variable&
      xchg ah,al
      out dx,ax
    ELSE
      IFE TYPE variable-4
        mov ax,word ptr &Pointer&.&variable&+2
        xchg ah,al
        out dx,ax
        mov ax,word ptr &Pointer&.&variable&
        xchg ah,al
        out dx,ax
      ENDIF
    ENDIF
  ENDIF
ENDM

