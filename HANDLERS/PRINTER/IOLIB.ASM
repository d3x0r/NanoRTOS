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

LINK Get_Data_SA
LINK Get_Data_SB
LINK Get_Data_NSA
LINK Get_Data_NSB
LINK Get_Rewind
LINK Get_Back_File
LINK Get_Back_Record
LINK Get_Adv_File
LINK Get_Adv_Record
LINK Get_EOF
LINK Get_Home

END


