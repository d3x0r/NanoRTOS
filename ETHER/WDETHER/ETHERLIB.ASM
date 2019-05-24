.model medium,c

.data
extrn   Request         :dword
.code

LINK    macro   name
LOCAL   @@jmpadr1,@@jmpadr2,@@jmpcall,@@proc_name
public  name
proc    name    far
@@jmpadr1:
        mov     ax,seg Request
        mov     es,ax
@@jmpadr2:
        mov     bx,offset Request
@@jmpcall:
        push    cs
        push    offset @@proc_name
        call    dword ptr es:[bx]
        add     sp,4
        mov     word ptr cs:[@@jmpadr1+1],dx
        mov     word ptr cs:[@@jmpadr2+1],ax
        mov     byte ptr cs:[@@jmpcall],26h
        mov     word ptr cs:[@@jmpcall+1],2fffh
        jmp     @@jmpadr1
@@proc_name       db      '&name',0
endp
ENDM

LINK openether
LINK sendether
LINK readether
LINK closeether
LINK flushether

END

