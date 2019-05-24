_COM segment byte public 'CODE'
assume cs:_COM,ds:_COM,ss:_COM


public _fstrtok

;
;   Copyright (c) Mix Software 1988, 1989
;
; -------------------------------------------------------
;       char *farstrtok(s1, s2)
;       char far *s1, far *s2;
;
;       Purpose: Searches for the beginning of the next token
;                in s1.  A token is a sequence of one or more
;                characters in s1 separated from the next token
;                by a sequence of one or more characters contained
;                in s2.  S1 must point to a string of tokens for
;                the first call to strtok.  Subsequent calls
;                can specify a NULL value for s1 to return the next
;                token in the string.
;
;       Returns: Pointer to next token in s1
;                NULL if there are no more tokens in s1
; -------------------------------------------------------
;
;
;
_fstrtok  proc c near,str1:dword,str2:dword
         push   di
         push   si
         les    di,str2
         cld
         mov  cx,-1
         xor  al,al
         repnz scasb          ; find length of s2
         not  cx
         jcxz nos2            ; return null if s2 is empty
         mov  dx,cx           ; save length of s2
         push ds
         mov    si,word ptr str1
         mov    ax,word ptr str1+2
         test si,si           ; is s1 null?
         jnz  notnull
         test ax,ax
         jnz  notnull
         mov  si,[strptr]
         mov  ax,[strseg]
         test si,si
         jnz  notnull         ; no previous string?
         test ax,ax
         jz   empty
notnull: mov  ds,ax
nextch:  lodsb                ; get s1 character
         test al,al
         jz   empty           ; end of s1
         mov  cx,dx
         mov    di,word ptr str2
         repnz scasb          ; check in delimiter set
         jz   nextch
         mov  bx,si           ; address of first non-delimiter
         dec  bx
tokch:   lodsb
         test al,al
         jz   ends1
         mov  cx,dx
         mov    di,word ptr str2
         repnz scasb
         jnz  tokch           ; not a delimiter
         mov  byte ptr [si-1],0   ; terminate token
         mov  ax,bx
         mov  dx,ds
         pop  cx
         mov  ds,cx
         mov  [strptr],si
         mov  [strseg],dx
         pop    si
         pop    di
         ret
empty:   xor  ax,ax
         mov  [strptr],ax
         xor  dx,dx
         mov  [strseg],ax
         pop  cx
         mov  ds,cx
         pop    si
         pop    di
         ret
nos2:    mov    cx,word ptr str1
         mov  [strptr],cx
         mov    cx,word ptr str1+2
         mov  [strseg],cx
         xor  ax,ax
         xor  dx,dx
         pop    si
         pop    di
         ret
ends1:   mov  ax,bx
         mov  dx,ds
         pop  cx
         mov  ds,cx
         mov  word ptr [strptr],0
         mov  word ptr [strseg],0
         pop    si
         pop    di
         ret
endp

strptr   dw   0-0
strseg   dw   0-0
_COM ends


END
