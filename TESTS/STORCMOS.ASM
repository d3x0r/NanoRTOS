.model tiny,C

.code
ORG 100h
start:
        push    cs
        pop     ax
        mov     ds,ax
        mov     es,ax
        cld

        mov     di,offset input
        mov     cx,2*128

read_input:
        mov     ah,08h
        int     21h
        cmp     al,0dh
        je      read_input
        cmp     al,0ah
        je      read_input
        cmp     al,20h
        je      read_input
        stosb
        loop    read_input



        mov     si,offset input
        mov     di,offset buffer
        mov     cx,128
convert_byte:
        lodsw
        xchg    al,ah            ;otherwise value will be byte reversed
        mov     bx,offset hex
convert_lower:
        cmp     al,[bx]
        je      found_char1
        inc     bx
        jmp     convert_lower
found_char1:
        sub     bx,offset hex
        mov     al,bl
        mov     bx,offset hex
convert_upper:
        cmp     ah,[bx]
        je      found_char2
        inc     bx
        jmp     convert_upper
found_char2:
        sub     bx,offset hex
        mov     ah,bl
        shl     ah,1
        shl     ah,1
        shl     ah,1
        shl     ah,1
        and     ax,0f00fh
        or      al,ah
        stosb
        loop    convert_byte



        int     3

        mov     ax,0
        mov     cx,128
        mov     si,offset buffer
write_cmos:
        lodsb
        cli                ;disable interupts
        xchg    ah,al      ;ah wass register, al was old data
        out     70h,al     ;select register
        xchg    ah,al      ;al was register, ah was data
        out     71h,al     ;get data, ah saves register still
        sti                ;enable interupts
        inc     ah         ;select next register
        loop    write_cmos  ;read some more



        mov     ax,4c00h
        int     21h

        buffer   db      256 dup(0)
        hex      db      '0123456789ABCDEF'
        input    db      128*3 dup (0)
        carriage db      13,10,'$'


end start
