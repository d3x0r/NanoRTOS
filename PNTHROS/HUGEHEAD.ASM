.model HUGE,C

  extrn main:far

.CODE
  public exit


.DATA

public stackend

.CODE
org 100h
start_point:
  jmp begin

.DATA
public _stackdata
_stackdata dw 512 dup(0)
stackend dw 0
.CODE
public begin
begin:
        mov     ax,seg _stackdata
        mov     ss,ax
        mov     sp,offset stackend
        mov     es,word ptr ds:[44]
        mov     cx,7fffh
        mov     di,0
        mov     al,0
next:
        repnz   scasb
        scasb
        jnz     next
        inc     di
        inc     di
        push    es
        push    di
        push    ds
        push    80h
        mov     ax,@data
        mov     ds,ax
        call    main
        add     sp,8

exit proc far,exit_code:byte
        mov     ah,4ch
        mov     al,exit_code
        int     21h
exit endp

END start_point


