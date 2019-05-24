.model HUGE,C
.386
extrn irqHandler:far

INCLUDE OS.EQU

public old_irq
public lptintr

.DATA
old_irq dd 0


.CODE


lptintr Proc Near
   ; push some stuff....
   ; read some other stuff...
	PUSH DS
	push es
	push ebp
	PUSH eAX
	push ebx
	push ecx
	push edx
	push esi
	push edi
        mov     ax,@data
        mov     ds,ax
        ;int 3;
		call irqHandler
   pop edi
   pop esi
	pop edx
	pop ecx
	pop ebx
        mov     al,20h
        out     20h,al
	POP eAX
	pop ebp
	pop es
	POP DS
		; free the pic...
	iret

lptintr endp

END