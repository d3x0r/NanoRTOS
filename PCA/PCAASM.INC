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
MAX_READ_CLASSES   equ 3
MAX_WRITE_CLASSES  equ 1
READ_RAW      equ 0
WRITE_RAW     equ 0

EVENTC   equ     01h
ODDTC    equ     02h
EVENSEL  equ     04h
ODDSEL   equ     08h
EVENTI   equ     10h
ODDTI    equ     20h
EVENTERM equ     40h
ODDTERM  equ     80h


NodeTrack STRUC
    side    db  ?
    Status  dw  ?
    Source  dd  ?
    Next    dd  ?
    Prior   dd  ?
ENDS

ReturnData STRUC
    Opcode        db  ?
    Status        dw  ?
    Byte_cnt      dd  ?
    XStatus       dw  ?
    Extended_Size dw  ?
    Extended_mail dd  ?
ENDS

Etherdata STRUC
  SourceIP  dd ?
  DestIP    dd ?
  SourceTCP dw ?
  DestTCP   dw ?
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
  Tracking    NodeTrack  ?
  Data        dd         ?
  Return      ReturnData ?
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

