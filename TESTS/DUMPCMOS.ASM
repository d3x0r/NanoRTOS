.model tiny,C

.code
ORG 100h
start:
        push    cs
        pop     ax
        mov     ds,ax
        mov     es,ax
        mov     di,offset buffer
        mov     ax,0
        mov     cx,128
        cld
read_cmos:
        cli                ;disable interupts
        xchg    ah,al      ;ah wass register, al was old data
        out     70h,al     ;select register
        xchg    ah,al      ;al was register, ah was data
        in      al,71h     ;get data, ah saves register still
        sti                ;enable interupts
        stosb
        inc     ah         ;select next register
        loop    read_cmos  ;read some more

        mov     cx,128           ;set up count for loop
        mov     dx,offset output ;dx is offset of message, ds is already there
        mov     si,offset buffer ;si is offset of buffer
        mov     bx,offset hex
print_values:
        lodsb                    ;get first value
        mov     ah,al            ;copy byte to upper part of reg
        shr     ah,1             ;shift right 4 to remove low nibble
        shr     ah,1
        shr     ah,1
        shr     ah,1

        and     ax,0f0fh         ;mask off all but low nibbles of the two
                                 ; halves-  ax is now the two nibbles of
                                 ; al in each half of itself
        xlat
        xchg    ah,al
        xlat
        mov     word ptr output,ax
        mov     ah,9h
        int     21h
        mov     ax,cx
        and     ax,0fh
        cmp     ax,01h
        jne     no_carriage
        mov     dx,offset carriage
        mov     ah,9h
        int     21h
        mov     dx,offset output
no_carriage:
        loop    print_values

        mov     ax,4c00h
        int     21h

        buffer  db      256 dup(0)
        hex     db      '0123456789ABCDEF'
        output  db      '   $'
        carriage db     13,10,'$'


end start
