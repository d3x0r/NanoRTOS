LOCALS
.MODEL HUGE,C

extrn Exit_routine:far
extrn Request:far

.DATA
extrn errorcode:word
extrn process_control:word
extrn current_module:dword

.CODE
times_null  dw 0
public first_used
public Allocate_mem
public Free_mem            ;must be last so that endmem will be at the
                           ; end of the program.
public times_null


IFNDEF memdebug

Enter_debug MACRO op,size,where
ENDM

save_unused MACRO number
ENDM

ELSE

save_unused MACRO number
  push ax

  mov ax,prior_unused
  mov prior_unused1,ax

  mov ax,first_unused
  mov prior_unused,ax

  mov ax,mark_move
  mov mark_move1,ax
  mov word ptr mark_move,number
  pop ax
ENDM


Enter_debug MACRO op,size,where
LOCAL @@next_trace
  push  bx
  push  ax
  push  dx
  push  es
  push  di

  mov   bx,trace_place                   ;get next entry to do
  cmp   bx,last_trace                    ;test to see if at last
  jne   @@next_trace                     ;if not, then inc next
  mov   trace_place,-1                   ;store -1 so it gets incremented to 0
@@next_trace:
  inc   trace_place                      ;inc next palce
  mov   ax,debugentry                    ;mov ax,sizeof entry
  mul   bx                               ;multiply by entry indec (destroys dx)
  mov   bx,ax                            ;move result back to bx
  mov   Allocate_Trace[bx].op_id,op      ;store operation
  mov   Allocate_Trace[bx].Size,size     ;store size
  mov   Allocate_Trace[bx].Address,where ;store location of block

  mov   ax,40h                            ;seg of timer
  mov   es,ax                             ;set seg
  mov   ax,es:[6ch]                       ;get time
  mov   Allocate_Trace[bx].Time,ax        ;store time
  les   di,current_module                 ;get current
  mov   ax,es:[di]                        ;get begin of name
  mov   word ptr Allocate_Trace[bx].Taskname,ax    ;store name
  mov   ax,es:[di+2]                               ;get next of name
  mov   word ptr Allocate_Trace[bx].Taskname+2,ax  ;store name
  pop   di
  pop   es
  pop   dx
  pop   ax
  pop   bx
ENDM

.DATA

public Allocate_Trace

ENTRY STRUC
  op_id         db 0          ;1
  Size          dw 0          ;2
  Address       dw 0          ;2
  Time          dw 0          ;2
  Taskname      dd 20202020h  ;4
ENTRY ends

debugentry      equ 11
last_trace      dw 4095
trace_place     dw 0
Allocate_Trace  ENTRY 4096 dup(?)

ENDIF

.DATA

label first_used word
first dw 0
first_unused dw 0
prior_unused dw 0
mark_move    dw 0
prior_unused1 dw 0
mark_move1    dw 0

BLOCK STRUC
  priorsegment  dw ?
  blocksize     dw ?
  owner         dw ?
  nextfree      dw ?
  priorfree     dw ?
  freed_by      dw ?
  checksum      dw ?
BLOCK ends

MEM_DUMP db     'Dump_blocks'

dumproutine dd      ?

.CODE

IFDEF memdebug
header_size     equ     2

Check_headers proc,segment:word
        push    es
        push    ds
        push    ax
        push    bx
        push    cx
        push    dx
        push    di
        push    si
        mov     es,segment
        mov     cx,es:[0].priorsegment
        add     cx,es:[0].blocksize
        add     cx,es:[0].owner
        add     cx,es:[0].nextfree
        add     cx,es:[0].priorfree
        add     cx,es:[0].freed_by
        cmp     cx,es:[0].checksum
        jne     bad_check
        mov     di,0
        mov     si,0
        mov     cx,segment
        mov     ds,cx
        inc     cx
        mov     es,cx
        mov     cx,TYPE BLOCK
        shr     cx,1
        repz    cmpsw
        jz      good_check
bad_check:
        push    @data
        push    offset MEM_DUMP
        call    Request
        add     sp,4
        int     3
        mov     cx,ax
        or      cx,dx
        jz      good_check   ;not actually a good check, but we don't ahve
                             ;  a routine to dump this junk
        mov     es,dx
        mov     di,ax
        mov     cx,segment
        push    cx
        call    dword ptr es:[di]
        add     sp,2
good_check:
        pop     si
        pop     di
        pop     dx
        pop     cx
        pop     bx
        pop     ax
        pop     ds
        pop     es
        ret
Check_headers endp

Dup_Checksum proc,segment:word
        push    es
        push    ds
        push    ax
        push    bx
        push    cx
        push    di
        push    si
        mov     bx,segment
        mov     ds,bx
        inc     bx
        mov     es,bx
        mov     si,0
        mov     di,0
        mov     bx,0
        mov     cx,6
do_move:
        lodsw
        stosw
        add     bx,ax
        loop    do_move
        mov     es:[0].checksum,bx
        mov     ds:[0].checksum,bx
        pop     si
        pop     di
        pop     cx
        pop     bx
        pop     ax
        pop     ds
        pop     es
        ret
Dup_Checksum endp
ELSE

header_size equ 1
Dup_Checksum proc
  ret
Dup_Checksum endp
Check_headers proc
  ret
Check_headers endp

ENDIF

Allocate_mem proc far,size:word
  push  ds
  push  di
  mov   ax,@data
  mov   ds,ax
  or    process_control,4
  cmp   first,0                 ;test to see if this is the first
                                 ;block ever allocated
  jne   continue_alloc
  mov   ax,seg endmem              ;if not, then compute the address
  mov   bx,offset endmem           ;of the first full segment after my
  mov   cl,4                       ;program
  shr   bx,cl
  inc   bx
  add   ax,bx
  mov   first,ax                ;save computated block segment
  mov   es,ax
  mov   word ptr es:[0].priorsegment,0
  mov cx,0a000h                  ;last part of usable memory
  mov bx,es                      ;save end segment
  sub cx,bx                      ;find length of block to end of memory
  sub cx,header_size
  mov word ptr es:[0].blocksize,cx
  mov word ptr es:[0].owner,0
  mov word ptr es:[0].nextfree,0
  mov word ptr es:[0].priorfree,0

IFDEF memdebug
  push  es
  call  Dup_Checksum
  pop   es
ENDIF
  save_unused 1
  mov first_unused,bx
continue_alloc:
  mov  cx,size
  mov  dx,0
  test cx,0fh
  jz   shift
  mov  dx,1                       ;had a partial number, so we need to
                                  ;allocate another paragraph so that
                                  ;the fraction is accounted for
shift:
  shr cx,1                        ;convert to paragraphs
  shr cx,1
  shr cx,1
  shr cx,1
  mov ax,first_unused             ;get segment of first free block  **************************
  cmp ax,0                        ;check to see if there is a first free
  jne go_on_allocate              ;if so, allocate
  jmp error
go_on_allocate:
  mov es,ax                       ;move seg of first free to es
  add cx,dx                       ;add fraction of paragraph to paragraph if
                                  ; there was one, dx is 1 else dx is 0
test_again:
  cmp es:[0].blocksize,cx         ;see if current block is big enough
  jnb doalloc                     ;if so, allocate it
  cmp es:[0].nextfree,0           ;check to see if there is a next free
  mov es,es:[0].nextfree          ;move to it regardless
  je  error_start                 ;if no next, not block found, leave as error
  jmp test_again                  ;test that block for allocation if one exists
error_start:
  mov word ptr errorcode,10h
  jmp error                       ;leave with a NULL pointer returned
doalloc:
  mov ax,es:[0].blocksize         ;get size of block
  mov bx,[bp+4]                   ;move owner address to bx
  sub ax,cx                       ;figure out how much leftover there
                                  ;is for the remainder of the block.

IFDEF memdebug
  push es                         ;header for block to be allocated exists
  call Check_headers              ;so check for validity
  pop es
ENDIF

  mov es:[0].blocksize,cx         ;store blocksize
  mov es:[0].owner,bx             ;store owner
  mov dx,es:[0].nextfree
  mov es:[0].nextfree,0           ;clear next free pointer
  mov di,es:[0].priorfree
  mov es:[0].priorfree,0          ;clear prior free pointer

IFDEF memdebug
  push es                         ;duplicate the information just entered
  call Dup_Checksum               ;for allocated block and checksum.
  pop es
ENDIF

  mov bx,es                       ;move es to bx to either return to user
                                  ;or to compute next block
  add cx,bx                       ;cx was formerly size, now it is the
  add cx,header_size              ;address of the next block
                                  ;add header size to compensate that the
                                  ;block header occupies space

  cmp ax,header_size              ;test to see if there was a remainter
  jae split                       ;if there is, then set up next block

  add es:[0].blocksize,ax         ;if there was an amount left that was
                                  ;smaller than the allocation header size
                                  ;then add that size to this block.
IFDEF memdebug
  push es
  call Dup_Checksum
  pop es
ENDIF
  cmp dx,0                        ;check to see if there is a next free
                                  ;block
  je no_next_free
  mov es,dx                       ;mov next free to segment

IFDEF memdebug
  push es                         ;check next free block's header
  call Check_headers
  pop es
ENDIF

  mov es:[0].priorfree,di         ;allocated block was completely used
                                  ;so next free's prior=allocated block's
                                  ;prior

IFDEF memdebug
  push es                         ;duplicate and checksum changed info.
  call Dup_Checksum
  pop es
ENDIF


no_next_free:
  cmp di,0                        ;check to see if allocated block was the
                                  ;first free one
  jne not_first_free              ;if not then jump
  save_unused 2
  mov first_unused,dx             ;otherwise, just move first unused to
                                  ;be the next avail block
  jmp return_addr                 ;return the block allocated.
not_first_free:
  mov es,di                       ;go to prior free block

IFDEF memdebug
  push es                         ;check the prior free block's header
  call Check_headers
  pop es
ENDIF

  mov es:[0].nextfree,dx          ;store allocated block's next free in
                                  ;allocated block's prior free block.

IFDEF memdebug
  push es                         ;duplicate and checksum changed info in
  call Dup_Checksum               ;prior free block's header
  pop es
ENDIF

  jmp return_addr

split:
                                  ;this block does not exist yet because
                                  ;we are splitting one free block into
                                  ;two blocks, first one has been allocated
                                  ;and the second one will be the remainder
                                  ;of the free memory
  sub ax,header_size
  mov es,cx                       ;go to the next block to become a memory
                                  ;block
  mov es:[0].priorsegment,bx      ;set up next block params
  mov es:[0].blocksize,ax         ;
  mov es:[0].owner,0              ;
  mov es:[0].nextfree,dx          ;
  mov es:[0].priorfree,di         ;

IFDEF memdebug
  push es                         ;checksum and duplicate the info in the
  call Dup_Checksum               ;newly created header.
  pop es
ENDIF

  cmp dx,0                        ;check to see if there is a next free block
  je no_next_free2
  mov es,dx

IFDEF memdebug
  push es
  call Check_headers              ;there was a next free block alread, so
  pop es                          ;check its validity
ENDIF

  mov es:[0].priorfree,cx

IFDEF memdebug
  push es                         ;duplicate and checksum changed info
  call Dup_Checksum
  pop es
ENDIF

no_next_free2:
  cmp di,0                        ;check to see if prior block exists
  jne not_first_again
  save_unused 3                   ;if not, then mark the move of first_unused
  mov first_unused,cx             ;move first unused to newly created block
  jmp was_first                   ;skip update of prior block becuase there
                                  ;isn't one.
not_first_again:
  mov es,di                       ;go to prior block.

IFDEF memdebug
  push es                         ;prior block existed before, so validate it.
  call Check_headers
  pop es
ENDIF

  mov es:[0].nextfree,cx

IFDEF memdebug
  push es                         ;dup and checksum changed info
  call Dup_Checksum
  pop es
ENDIF

was_first:
  mov es,cx                       ;set segment to newly created block.
  add cx,ax                       ;compute next block after this free block
  add cx,header_size
  cmp cx,0a000h                   ;see if next block is end of memory
  jae return_addr
  mov ax,es                       ;if not save this segment

  mov es,cx                       ;mov es to next block

IFDEF memdebug
  push es                         ;next physical block exists so check it.
  call Check_headers
  pop es
ENDIF

  mov es:[0].priorsegment,ax      ;point next at proir segment return_addr:

IFDEF memdebug
  push es                         ;copy and checksum updated info.
  call Dup_Checksum
  pop es
ENDIF

return_addr:
IFDEF memdebug
  push bx
  call Dup_Checksum
  pop bx
ENDIF
  add bx,header_size
  mov dx,bx                       ;return segment of header
  mov ax,0                        ;with an offset of 0
  jmp leave_alloc
error:
  xor ax,ax;
  xor dx,dx;
  inc cs:times_null
leave_alloc:
  mov   bx,@data
  mov   ds,bx
  and   process_control,0fffbh   ;okay to prempt now

  push  si
  mov   si,size
  mov   cx,dx
  Enter_debug 0,si,cx    ; 1 is op for free, and -1 is the size,
                         ;   dx is seg of block
  cmp   dx,first         ;check to see if we ended up allocating a block
                         ;prior to the first one ever allocated
  jbe   allocate_error
continue_after:
  pop   si               ;if okay, continue

  pop   di

  mov cx,first_unused    ;get first unused block
  cmp cx,0               ;check to see if there is one
  je  block_is_okay      ;if not, is an okay condition
  mov es,cx              ;if so, see if that block is unowned as expected
  cmp es:[0].owner,0
  jne misallocated       ;if not, we have an expected free block owned
block_is_okay:
  pop   ds

  ret
allocate_error:
  cmp   dx,0
  je    continue_after
  int 3
  mov   ax,101
  push  ax
  call  Exit_routine
misallocated:
  int 3
  mov   ax,102
  push  ax
  call  Exit_routine
Allocate_mem endp

bubble proc near
;    while (beginfree && beginfree->nextfree>(short)((long)beginfree>>16))
;    {
;      beginfree=(BLOCK far *)((long)beginfree->nextfree<<16);
;    }
;             if  beginfree=first_block, first_block=begin->next
;
;                                   temp=begin->next;
;
;                               1   begin->next=temp->next;
;                               2   begin->next->prior=begin;
;                               3   temp->prior=begin->prior;
;                               4   temp->prior->next=temp;
;                               5   temp->next=begin;
;                               6   begin->prior=temp;
;
        mov     ax,@data
        mov     ds,ax
        mov     ds,first_unused
        mov     ax,ds                 ;get paragraph of first free
test_order:
        or      ax,ax                 ;test to see if at end of chain
        jne     test_order_5          ;if so, leave w/ no mods
        jmp     leave_bubble
test_order_5:
        mov     es,ax                 ;make a usageble seg
        mov     dx,es:[0].nextfree    ;get the next unallocated block
        or      dx,dx                 ;test to see if this is end of chain
        jne     test_order_10
        jmp      leave_bubble          ;if so, leave
test_order_10:
        cmp     dx,ax                 ;test to see if next is lower then
                                      ; current
        mov     ax,dx                 ;update the current;
        ja      test_order            ;if not, keep checking order


                                      ;now ES is pointing at seg that
                                      ;is out of order. AX,DX are the
                                      ;next header that should be before
                                      ;this one, so we follow steps 1-6.
        mov     ds,ax                 ;make next a usable segment

        mov     dx,ds:[0].nextfree    ; begin(es)->next=temp(ds)->next
        mov     es:[0].nextfree,dx
IFDEF memdebug
        push es
        call Dup_Checksum
        pop es
ENDIF

        or      dx,dx                 ;if temp->next==NULL skip next
        je      skip1
                                      ; begin->next (dx)->prior=begin(es)
        mov     ds,dx                 ;
        mov     ds:[0].priorfree,es
IFDEF memdebug
        push ds
        call Dup_Checksum
        pop ds
ENDIF
        mov     ds,ax                 ;   (recover ds(temp) from ax);
skip1:

        mov     dx,es:[0].priorfree   ; temp(ds)->prior=begin(es)->prior
        mov     ds:[0].priorfree,dx
IFDEF memdebug
        push ds
        call Dup_Checksum
        pop ds
ENDIF

        or      dx,dx                 ;if temp->prior==NULL skip next
        je      skip2
        mov     cx,es                 ; temp->prior(dx)->next=temp(ds)
        mov     es,dx                 ;    (save es(begin) in cx)
        mov     es:[0].nextfree,ds
IFDEF memdebug
        push es
        call Dup_Checksum
        pop es
ENDIF
        mov     es,cx                 ;   (recover es(begin) from cx)
skip2:

        mov     ds:[0].nextfree,es    ; temp->next=begin
        mov     es:[0].priorfree,ds   ; begin->prior=temp;
IFDEF memdebug
        push es
        call Dup_Checksum
        pop es
        push ds
        call Dup_Checksum
        pop ds
ENDIF

        cmp     dx,0                  ;check begin to see if it was first
        jne     leave_bubble          ;if not, leave
        mov     dx,ds
        mov     ax,@data
        mov     ds,ax
        save_unused 4
        mov     first_unused,dx    ;otherwise temp(ds) is now first
        ret
leave_bubble:
        ret

bubble endp

block_check proc near
;
;   ax=current seg
;   es=current seg
;   coming into here, es points at the block that we are going to try to
;   free.  All other registers are not setup, so we can use them.
;   Given this information, I need to check the prior block to see if
;   it points at me.
;   then I need to check the next block to see if it points at me.
;   if either of these are false, then someone is trying to mess with my
;   mind.
        mov     cx,ds                   ;save data seg
        mov     bx,es:[0].priorsegment
        cmp     bx,0
        je      block_okay1
        mov     ds,bx

IFDEF memdebug
        push    ds                     ;check prior block header
        call    Check_headers
        pop     ds
ENDIF

        sub     ax,ds:[0].blocksize
        sub     ax,header_size
        sub     ax,bx
        jz      block_okay1
        int     3                       ;prior block was not valid
        mov     ax,100
        push    ax
        call    Exit_routine
block_okay1:

IFDEF memdebug
        push    es                     ;check current block header
        call    Check_headers
        pop     es
ENDIF

        mov     ax,es
        mov     bx,ax
        add     ax,es:[0].blocksize
        add     ax,header_size
        cmp     ax,0a000h
        je      block_okay2
IFDEF memdebug
        push    ax                     ;check next block header
        call    Check_headers
        pop     ax
ENDIF
        mov     ds,ax
        cmp     bx,ds:[0].priorsegment
        je      block_okay2
        int     3                       ;next block was not valid
        mov     ax,105
        push    ax
        call    Exit_routine
block_okay2:
        mov     ax,es                   ;restore ax
        mov     ds,cx                   ;restore dataseg
        ret


block_check endp

Free_mem proc far,block_addr:dword
;
; 8    far ptr to block
; 6    near ptr to block
; 4    far
; 2    near
; bp - bp
  push  ds
  mov   ax,@data
  mov   ds,ax
  or    process_control,4

  mov ax,word ptr block_addr
  cmp ax,0
  jne go_to_panic

  mov ax,word ptr block_addr+2

  cmp ax,0
  je  go_to_panic

  sub ax,header_size                    ;make ax point at block header
  cmp ax,first
  ja continue_free

go_to_panic:

  int 3
  jmp panic_leave
continue_free:
  mov es,ax                 ;es is current block
  call  block_check
  cmp es:[0].owner,0
  jne do_free
  mov ax,103
  push ax
  call Exit_routine
  jmp leave_free_now
do_free:
  mov es:[0].owner,0
  mov ds,es:[0].priorsegment;ds is prior block in overall chain
  mov cx,es:[0].blocksize   ;cx will always be the size of the block
  mov bx,ax
  add bx,cx
  add bx,header_size        ;bx is next block in overall chain
  xor dx,dx
  cmp ds:[0].owner,0        ;check to see if prior block is free(already header
                            ;checked)
  jne skip_prior_concat     ;if not, don't combine

  mov dx,1                  ;mark dx as concatenation took place
  add cx,ds:[0].blocksize   ;add prior block size to current size
  add cx,header_size        ;inc 1 for the current header paragraph space
  mov ds:[0].blocksize,cx   ;store size
IFDEF memdebug
  push ds                   ;prior physical, already free headers
  call Dup_Checksum
  pop ds
ENDIF
  mov es,bx                 ;move to next block
  mov es:[0].priorsegment,ds    ;next's priorseg is the prior block.
IFDEF memdebug
  push es                       ;next physicals header
  call Dup_Checksum
  pop es
ENDIF
  mov es,es:[0].priorsegment    ;es is now current block again
  mov ax,es                     ;keep ax current as pointer to current
skip_prior_concat:
  mov ds,bx                 ;ds points at the next block
  cmp ds:[0].owner,0
  je next_free_okay
  jmp leave_free3
next_free_okay:
  add cx,ds:[0].blocksize   ;add nexts block size to the total free space
  add cx,header_size        ;addjust size
  mov es:[0].blocksize,cx   ;current block size is total
IFDEF memdebug
  push es
  call Dup_Checksum
  pop es
ENDIF
  add bx,ds:[0].blocksize   ;start computing next next
  add bx,header_size        ;compute address of next block
  cmp bx,0a000h             ;check for EOM
  je no_next_to_change_prior    ;if so, then there is not a block after this
                                ; that needs to be updated
  mov es,bx                     ;es is now the next next block
IFDEF memdebug
  push es                       ;block after next hasn't been checked yet
  call Check_headers
  pop es
ENDIF
  mov es:[0].priorsegment,ax    ;go there and make prior address= currnet block
IFDEF memdebug
  push es                       ;duplicate and checksum updated info
  call Dup_Checksum             ;next next block's header
  pop es
ENDIF

  mov es,ax                     ;restore es to currnet block
no_next_to_change_prior:  ;(es=current block ds=next block*/
  cmp dx,0                  ;check to see if we concated to prior
  je prior_wasnt_concated   ; if not, then update accordingly
  cmp ds:[0].nextfree,0     ;check to see if we are at end of free blocks
  je no_next_to_upd_prior   ;if so, no next
  mov bx,ds                 ;save ds(point to next physical block)
  mov dx,ds:[0].priorfree   ;  next->prior=prior
  mov ds,ds:[0].nextfree    ;
IFDEF memdebug
  push ds                   ;check the next free block
  call Check_headers
  pop ds
ENDIF
  mov ds:[0].priorfree,dx   ;
IFDEF memdebug
  push ds                   ;dup and checksum updated info
  call Dup_Checksum         ;next free's header
  pop ds
ENDIF
  mov ds,bx
no_next_to_upd_prior:
  cmp ds:[0].priorfree,0
  mov dx,ds:[0].nextfree
  jne c1
  jmp update_first_free
c1:
  mov   ds,ds:[0].priorfree
IFDEF memdebug
  push ds                   ;check the prior free block
  call Check_headers
  pop ds
ENDIF
  mov   ds:[0].nextfree,dx
IFDEF memdebug
  push ds                   ;dup and checksum updated info
  call Dup_Checksum         ;prior free's header
  pop ds
ENDIF
  jmp   leave_free2

prior_wasnt_concated:
  mov   dx,ds:[0].nextfree   ;get next free
  mov   es:[0].nextfree,dx   ;move next free to current ->nextfree
  mov   bx,ds:[0].priorfree  ;get priorfree
  mov   es:[0].priorfree,bx  ;move prior free to current ->priorfree
  cmp   dx,0                 ;check to see if next was null
  je no_next_to_upd_prior_1
  mov   bx,ds                ;if not, then save ds
  mov   ds,dx                ;go to next
IFDEF memdebug
  push  ds                   ;check next free block's headers
  call  Check_headers
  pop   ds
ENDIF
  mov   ds:[0].priorfree,es  ;update next->prior=current
IFDEF memdebug
  push  ds                   ;duplicate and checksum changed info
  call  Dup_Checksum         ;next free's header
  pop   ds
ENDIF
  mov   ds,bx                ;restore ds
no_next_to_upd_prior_1:
  cmp   ds:[0].priorfree,0   ;check prior
  jne   c2
  mov   dx,es                ;save es to update first free
  jmp   update_first_free
c2:
  mov   bx,ds                ;save ds
  mov   ds,ds:[0].priorfree  ;go to prior
IFDEF memdebug
  push ds                    ;check prior free's headers
  call Check_headers
  pop ds
ENDIF
  mov   ds:[0].nextfree,es   ;move prior -> next to be current
IFDEF memdebug
  push  ds                   ;dup and checksum changed info
  call  Dup_Checksum         ;prior free's header
  pop   ds
ENDIF
  jmp   leave_free2
update_first_free:
  mov   ax,@data
  mov   ds,ax
  save_unused 5
  mov   first_unused,dx
leave_free2:
  mov   bx,[bp+4]             ;get caller's segment
  mov   es:[0].freed_by,bx    ;es is current block
IFDEF memdebug
  push  es
  call  Dup_Checksum     ;current block's header
  pop   es
ENDIF
  call  bubble
panic_leave:
  mov   bx,@data
  mov   ds,bx
  and   process_control,0fffbh
  mov   cx,word ptr block_addr+2
  Enter_debug 1,-1,cx  ; 1 is op for free, and -1 is the size
leave_free_now:
  pop   ds
  ret
leave_free3:
;   at this point dx indicates whether we did concatenation or not
;   es=current block
;
;
  cmp dx,0                  ;check to see if we did any concatenation
  je leave_free3_1
  jmp leave_free2           ;if so, get outa here
leave_free3_1:
  mov   ax,@data
  mov   ds,ax
  mov bx,first_unused       ;get first free block
  mov es:[0].nextfree,bx    ;move last first to new first's next
  mov es:[0].priorfree,0    ;no prior
  cmp bx,0
  je save_unused6           ;if no first free block
  mov ds,bx                 ;go to prior first
IFDEF memdebug
  push ds
  call Check_headers
  pop ds
ENDIF
  mov ds:[0].priorfree,es   ;prior first's prior is not current block
IFDEF memdebug
  push ds
  call Dup_Checksum         ;first free's header
  pop ds
ENDIF
save_unused6:
  mov ax,@data
  mov ds,ax
  save_unused 6
  mov first_unused,es    ;first block is now the current block
  jmp leave_free2
Free_mem endp

.FARDATA
endmem dw 0


;_COM ends

END
