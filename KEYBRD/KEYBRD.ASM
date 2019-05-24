	.286p
	ifndef	??version
?debug	macro
	endm
publicdll macro	name
	public	name
	endm
$comm	macro	name,dist,size,count
	comm	dist name:BYTE:count*size
	endm
	else
$comm	macro	name,dist,size,count
	comm	dist name[size]:BYTE:count
	endm
	endif
	?debug	V 300h
	?debug	S "keybrd.c"
	?debug	C E90E47541B086B65796272642E63
	?debug	C E92548541B136D3A5C636F6D6D6F6E2E6970635C6D6F642E68
	?debug	C E97668481B126D3A5C636F6D6D6F6E2E6970635C6F732E68
_TEXT	segment byte public 'CODE'
_TEXT	ends
DGROUP	group	_DATA,_BSS
	assume	cs:_TEXT,ds:DGROUP
_DATA	segment word public 'DATA'
d@	label	byte
d@w	label	word
_DATA	ends
_BSS	segment word public 'BSS'
b@	label	byte
b@w	label	word
_BSS	ends
_TEXT	segment byte public 'CODE'
   ;	
   ;	short Locate(char far *string)
   ;	
	assume	cs:_TEXT
_Locate	proc	near
	enter	2,0
   ;	
   ;	{
   ;	  short temp;
   ;	  asm mov dx,word ptr string;
   ;	
 	mov	 dx,word ptr [bp+4]
   ;	
   ;	  asm mov es,word ptr string+2;
   ;	
 	mov	 es,word ptr [bp+4]+2
   ;	
   ;	  asm mov ah,2;
   ;	
 	mov	 ah,2
   ;	
   ;	  asm int 60h;
   ;	
 	int	 60h
   ;	
   ;	  asm mov temp,ax;
   ;	
 	mov	 [bp-2],ax
   ;	
   ;	  return(temp);
   ;	
	mov	ax,word ptr [bp-2]
	jmp	short @1@198
@1@198:
   ;	
   ;	}
   ;	
	leave	
	ret	
_Locate	endp
	?debug	C E9D076591A146D3A5C636F6D6D6F6E2E6970635C636F6D6D2E68
	?debug	C E9615B581A134D3A5C6E6970635C636F6D6D5C636F6D6D2E68
	?debug	C E9A04C771A156D3A5C636F6D6D6F6E2E6970635C766964656F2E68
	?debug	C E94765921A1A4D3A5C6E6970635C766964656F5C746578745C7669+
	?debug	C 64656F2E68
_TEXT	ends
_DATA	segment word public 'DATA'
_output	label	dword
	db	0
	db	0
	db	0
	db	0
_keyhead	label	byte
	db	0
_keytail	label	byte
	db	0
_led_status	label	byte
	db	0
_sft_status	label	byte
	db	0
_option	label	word
	dw	DGROUP:s@
	dw	DGROUP:s@+7
	dw	DGROUP:s@+12
	dw	DGROUP:s@+16
	dw	DGROUP:s@+20
	dw	DGROUP:s@+27
_DATA	ends
_TEXT	segment byte public 'CODE'
   ;	
   ;	void connect_kbd(void)
   ;	
	assume	cs:_TEXT
_connect_kbd	proc	near
	enter	4,0
   ;	
   ;	{
   ;	  long temp_ptr;
   ;	  schead=sctail;
   ;	
	mov	al,byte ptr DGROUP:_sctail
	mov	byte ptr DGROUP:_schead,al
   ;	
   ;	  asm mov ax,0x3509;
   ;	
 	mov	 ax,03509H
   ;	
   ;	  asm int 21h
   ;	
 	int	 21h
   ;	
   ;	  asm mov word ptr old_key,bx;
   ;	
 	mov	 word ptr _old_key,bx
   ;	
   ;	  asm mov word ptr old_key+2,es;
   ;	
 	mov	 word ptr _old_key+2,es
   ;	
   ;	  asm mov ax,0x2509;
   ;	
 	mov	 ax,02509H
   ;	
   ;	  temp_ptr=(long)&keyintr;
   ;	
	mov	word ptr [bp-2],cs
	mov	word ptr [bp-4],offset _keyintr
   ;	
   ;	  asm push ds
   ;	
 	push	 ds
   ;	
   ;	  asm mov dx,word ptr temp_ptr
   ;	
 	mov	 dx,word ptr [bp-4]
   ;	
   ;	  asm mov ds,word ptr temp_ptr+2
   ;	
 	mov	 ds,word ptr [bp-4]+2
   ;	
   ;	  asm int 21h
   ;	
 	int	 21h
   ;	
   ;	  asm pop ds;
   ;	
 	pop	 ds
   ;	
   ;	}
   ;	
	leave	
	ret	
_connect_kbd	endp
_TEXT	ends
_DATA	segment word public 'DATA'
_disconnect_kbdname	label	byte
	db	33
	db	100
	db	105
	db	115
	db	99
	db	111
	db	110
	db	110
	db	101
	db	99
	db	116
	db	95
	db	107
	db	98
	db	100
	db	0
_DATA	ends
_TEXT	segment byte public 'CODE'
   ;	
   ;	cleanup(void,disconnect_kbd,(void))
   ;	
	assume	cs:_TEXT
__disconnect_kbd	proc	near
	push	bp
	mov	bp,sp
	push	si
 	push	 ds
 	mov	 dx,offset _disconnect_kbd
 	mov	 si,offset DGROUP:_disconnect_kbdname
 	mov	 ax,seg DGROUP:_disconnect_kbdname
 	mov	 es,ax
 	mov	 ax,seg _disconnect_kbd
 	mov	 ds,ax
 	mov	 ah,1
 	int	 060H
 	pop	 ds
	pop	si
	pop	bp
	ret	
__disconnect_kbd	endp
   ;	
   ;	cleanup(void,disconnect_kbd,(void))
   ;	
	assume	cs:_TEXT
_disconnect_kbd	proc	far
	push	bp
	mov	bp,sp
   ;	
   ;	{
   ;	  asm push ds;
   ;	
 	push	 ds
   ;	
   ;	  asm mov ax,seg old_key
   ;	
 	mov	 ax,seg _old_key
   ;	
   ;	  asm mov ds,ax
   ;	
 	mov	 ds,ax
   ;	
   ;	  asm mov dx,word ptr old_key
   ;	
 	mov	 dx,word ptr _old_key
   ;	
   ;	  asm mov ds,word ptr old_key+2
   ;	
 	mov	 ds,word ptr _old_key+2
   ;	
   ;	  asm mov ax,0x2509;
   ;	
 	mov	 ax,02509H
   ;	
   ;	  asm int 21h
   ;	
 	int	 21h
   ;	
   ;	  asm pop ds;
   ;	
 	pop	 ds
   ;	
   ;	}
   ;	
	pop	bp
	ret	
_disconnect_kbd	endp
   ;	
   ;	void call(char far *line)
   ;	
	assume	cs:_TEXT
_call	proc	near
	push	bp
	mov	bp,sp
   ;	
   ;	{
   ;	  asm mov es,word ptr line+2
   ;	
 	mov	 es,word ptr [bp+4]+2
   ;	
   ;	  asm mov dx,word ptr line
   ;	
 	mov	 dx,word ptr [bp+4]
   ;	
   ;	  asm mov ah,7
   ;	
 	mov	 ah,7
   ;	
   ;	  asm int 0x60
   ;	
 	int	 060H
   ;	
   ;	}
   ;	
	pop	bp
	ret	
_call	endp
_TEXT	ends
_DATA	segment word public 'DATA'
_functions	label	word
	dw	DGROUP:s@+32
	dw	DGROUP:s@+43
	dw	DGROUP:s@+55
	dw	DGROUP:s@+64
	dw	DGROUP:s@+75
	dw	DGROUP:s@+86
	dw	DGROUP:s@+98
	dw	DGROUP:s@+107
	dw	DGROUP:s@+118
	dw	DGROUP:s@+129
	dw	DGROUP:s@+142
	dw	DGROUP:s@+152
	dw	DGROUP:s@+163
	dw	DGROUP:s@+171
	dw	DGROUP:s@+181
	dw	DGROUP:s@+192
	dw	DGROUP:s@+204
	dw	DGROUP:s@+213
_DATA	ends
_TEXT	segment byte public 'CODE'
   ;	
   ;	void do_local(short function)
   ;	
	assume	cs:_TEXT
_do_local	proc	near
	enter	8,0
   ;	
   ;	{
   ;	  windowptr twindow;
   ;	  short x=-1,y=-1;
   ;	
	mov	word ptr [bp-6],-1
	mov	word ptr [bp-8],-1
   ;	
   ;	  switch(function)
   ;	
	mov	bx,word ptr [bp+4]
	cmp	bx,17
	jbe	@@0
	jmp	@6@1066
@@0:
	shl	bx,1
	jmp	word ptr cs:@6@C850[bx]
@6@142:
   ;	
   ;	  {
   ;	    case 0:
   ;	      getdisplay(&x,&y,&twindow);
   ;	
	push	ss
	lea	ax,word ptr [bp-4]
	push	ax
	push	ss
	lea	ax,word ptr [bp-8]
	push	ax
	push	ss
	lea	ax,word ptr [bp-6]
	push	ax
	call	far ptr _getdisplay
	add	sp,12
   ;	
   ;	      moddisplay(twindow,DELTA_MOVE_W(-1,0),END_MOD);
   ;	
	push	0
	push	0
	push	-1
@6@170:
	push	7
@6@198:
	push	word ptr [bp-2]
	push	word ptr [bp-4]
	call	far ptr _moddisplay
	add	sp,12
   ;	
   ;	      break;
   ;	
	jmp	@6@1066
@6@226:
   ;	
   ;	    case 1:
   ;	      getdisplay(&x,&y,&twindow);
   ;	
	push	ss
	lea	ax,word ptr [bp-4]
	push	ax
	push	ss
	lea	ax,word ptr [bp-8]
	push	ax
	push	ss
	lea	ax,word ptr [bp-6]
	push	ax
	call	far ptr _getdisplay
	add	sp,12
   ;	
   ;	      moddisplay(twindow,DELTA_MOVE_W(1,0),END_MOD);
   ;	
	push	0
	push	0
	push	1
	jmp	short @6@170
@6@254:
   ;	
   ;	      break;
   ;	    case 2:
   ;	      getdisplay(&x,&y,&twindow);
   ;	
	push	ss
	lea	ax,word ptr [bp-4]
	push	ax
	push	ss
	lea	ax,word ptr [bp-8]
	push	ax
	push	ss
	lea	ax,word ptr [bp-6]
	push	ax
	call	far ptr _getdisplay
	add	sp,12
   ;	
   ;	      moddisplay(twindow,DELTA_MOVE_W(0,-1),END_MOD);
   ;	
	push	0
	push	-1
	jmp	short @6@310
@6@282:
   ;	
   ;	      break;
   ;	    case 3:
   ;	      getdisplay(&x,&y,&twindow);
   ;	
	push	ss
	lea	ax,word ptr [bp-4]
	push	ax
	push	ss
	lea	ax,word ptr [bp-8]
	push	ax
	push	ss
	lea	ax,word ptr [bp-6]
	push	ax
	call	far ptr _getdisplay
	add	sp,12
   ;	
   ;	      moddisplay(twindow,DELTA_MOVE_W(0,1),END_MOD);
   ;	
	push	0
	push	1
@6@310:
	push	0
	jmp	short @6@170
@6@338:
   ;	
   ;	      break;
   ;	    case 4:
   ;	      getdisplay(&x,&y,&twindow);
   ;	
	push	ss
	lea	ax,word ptr [bp-4]
	push	ax
	push	ss
	lea	ax,word ptr [bp-8]
	push	ax
	push	ss
	lea	ax,word ptr [bp-6]
	push	ax
	call	far ptr _getdisplay
	add	sp,12
   ;	
   ;	      moddisplay(twindow,RESIZEX(-1),END_MOD);
   ;	
	push	0
	push	-1
@6@366:
	push	10
	jmp	@6@1038
@6@394:
   ;	
   ;	      break;
   ;	    case 5:
   ;	      getdisplay(&x,&y,&twindow);
   ;	
	push	ss
	lea	ax,word ptr [bp-4]
	push	ax
	push	ss
	lea	ax,word ptr [bp-8]
	push	ax
	push	ss
	lea	ax,word ptr [bp-6]
	push	ax
	call	far ptr _getdisplay
	add	sp,12
   ;	
   ;	      moddisplay(twindow,RESIZEX(1),END_MOD);
   ;	
	push	0
	push	1
	jmp	short @6@366
@6@422:
   ;	
   ;	      break;
   ;	    case 6:
   ;	      getdisplay(&x,&y,&twindow);
   ;	
	push	ss
	lea	ax,word ptr [bp-4]
	push	ax
	push	ss
	lea	ax,word ptr [bp-8]
	push	ax
	push	ss
	lea	ax,word ptr [bp-6]
	push	ax
	call	far ptr _getdisplay
	add	sp,12
   ;	
   ;	      moddisplay(twindow,RESIZEY(-1),END_MOD);
   ;	
	push	0
	push	-1
	jmp	short @6@478
@6@450:
   ;	
   ;	      break;
   ;	    case 7:
   ;	      getdisplay(&x,&y,&twindow);
   ;	
	push	ss
	lea	ax,word ptr [bp-4]
	push	ax
	push	ss
	lea	ax,word ptr [bp-8]
	push	ax
	push	ss
	lea	ax,word ptr [bp-6]
	push	ax
	call	far ptr _getdisplay
	add	sp,12
   ;	
   ;	      moddisplay(twindow,RESIZEY(1),END_MOD);
   ;	
	push	0
	push	1
@6@478:
	push	12
	jmp	@6@1038
@6@506:
   ;	
   ;	      break;
   ;	    case 8:
   ;	      getdisplay(&x,&y,&twindow);
   ;	
	push	ss
	lea	ax,word ptr [bp-4]
	push	ax
	push	ss
	lea	ax,word ptr [bp-8]
	push	ax
	push	ss
	lea	ax,word ptr [bp-6]
	push	ax
	call	far ptr _getdisplay
	add	sp,12
   ;	
   ;	      if (twindow)
   ;	
	mov	ax,word ptr [bp-4]
	or	ax,word ptr [bp-2]
	je	short @6@646
   ;	
   ;	        if (twindow->opts&ICON)
   ;	
	les	bx,dword ptr [bp-4]
	test	word ptr es:[bx+30],16384
	je	short @6@590
   ;	
   ;	          moddisplay(twindow,RESTORE_WINDOW,END_MOD);
   ;	
	push	0
	push	16
	jmp	short @6@618
@6@590:
   ;	
   ;	        else
   ;	          moddisplay(twindow,ICON_WINDOW,END_MOD);
   ;	
	push	0
	push	6
@6@618:
	push	word ptr [bp-2]
	push	word ptr [bp-4]
	call	far ptr _moddisplay
	add	sp,8
@6@646:
   ;	
   ;	      break;
   ;	
	jmp	@6@1066
@6@674:
   ;	
   ;	    case 9:
   ;	      getdisplay(&x,&y,&twindow);
   ;	
	push	ss
	lea	ax,word ptr [bp-4]
	push	ax
	push	ss
	lea	ax,word ptr [bp-8]
	push	ax
	push	ss
	lea	ax,word ptr [bp-6]
	push	ax
	call	far ptr _getdisplay
	add	sp,12
   ;	
   ;	      getdisplay(&x,&y,&twindow);
   ;	
	push	ss
	lea	ax,word ptr [bp-4]
	push	ax
	push	ss
	lea	ax,word ptr [bp-8]
	push	ax
	push	ss
	lea	ax,word ptr [bp-6]
	push	ax
	call	far ptr _getdisplay
	add	sp,12
   ;	
   ;	      moddisplay(twindow,SELECT_WINDOW,END_MOD);
   ;	
	push	0
	push	3
	push	word ptr [bp-2]
	push	word ptr [bp-4]
	call	far ptr _moddisplay
	add	sp,8
   ;	
   ;	      break;
   ;	
	jmp	@6@1066
@6@702:
   ;	
   ;	    case 10:
   ;	      getdisplay(&x,&y,&twindow);
   ;	
	push	ss
	lea	ax,word ptr [bp-4]
	push	ax
	push	ss
	lea	ax,word ptr [bp-8]
	push	ax
	push	ss
	lea	ax,word ptr [bp-6]
	push	ax
	call	far ptr _getdisplay
	add	sp,12
   ;	
   ;	      moddisplay(twindow,DELTA_MOVE_M(-1,0),END_MOD);
   ;	
	push	0
	push	0
	push	-1
	jmp	short @6@842
@6@730:
   ;	
   ;	      break;
   ;	    case 11:
   ;	      getdisplay(&x,&y,&twindow);
   ;	
	push	ss
	lea	ax,word ptr [bp-4]
	push	ax
	push	ss
	lea	ax,word ptr [bp-8]
	push	ax
	push	ss
	lea	ax,word ptr [bp-6]
	push	ax
	call	far ptr _getdisplay
	add	sp,12
   ;	
   ;	      moddisplay(twindow,DELTA_MOVE_M(1,0),END_MOD);
   ;	
	push	0
	push	0
	push	1
	jmp	short @6@842
@6@758:
   ;	
   ;	      break;
   ;	    case 12:
   ;	      getdisplay(&x,&y,&twindow);
   ;	
	push	ss
	lea	ax,word ptr [bp-4]
	push	ax
	push	ss
	lea	ax,word ptr [bp-8]
	push	ax
	push	ss
	lea	ax,word ptr [bp-6]
	push	ax
	call	far ptr _getdisplay
	add	sp,12
   ;	
   ;	      moddisplay(twindow,DELTA_MOVE_M(0,-1),END_MOD);
   ;	
	push	0
	push	-1
	jmp	short @6@814
@6@786:
   ;	
   ;	      break;
   ;	    case 13:
   ;	      getdisplay(&x,&y,&twindow);
   ;	
	push	ss
	lea	ax,word ptr [bp-4]
	push	ax
	push	ss
	lea	ax,word ptr [bp-8]
	push	ax
	push	ss
	lea	ax,word ptr [bp-6]
	push	ax
	call	far ptr _getdisplay
	add	sp,12
   ;	
   ;	      moddisplay(twindow,DELTA_MOVE_M(0,1),END_MOD);
   ;	
	push	0
	push	1
@6@814:
	push	0
@6@842:
	push	8
	jmp	@6@198
@6@870:
   ;	
   ;	      break;
   ;	    case 14:
   ;	      getdisplay(&x,&y,&twindow);
   ;	
	push	ss
	lea	ax,word ptr [bp-4]
	push	ax
	push	ss
	lea	ax,word ptr [bp-8]
	push	ax
	push	ss
	lea	ax,word ptr [bp-6]
	push	ax
	call	far ptr _getdisplay
	add	sp,12
   ;	
   ;	      moddisplay(twindow,SCROLL_LEFT,END_MOD);
   ;	
	push	0
	push	0
@6@898:
	push	13
	jmp	short @6@1038
@6@926:
   ;	
   ;	      break;
   ;	    case 15:
   ;	      getdisplay(&x,&y,&twindow);
   ;	
	push	ss
	lea	ax,word ptr [bp-4]
	push	ax
	push	ss
	lea	ax,word ptr [bp-8]
	push	ax
	push	ss
	lea	ax,word ptr [bp-6]
	push	ax
	call	far ptr _getdisplay
	add	sp,12
   ;	
   ;	      moddisplay(twindow,SCROLL_RIGHT,END_MOD);
   ;	
	push	0
	push	1
	jmp	short @6@898
@6@954:
   ;	
   ;	      break;
   ;	    case 16:
   ;	      getdisplay(&x,&y,&twindow);
   ;	
	push	ss
	lea	ax,word ptr [bp-4]
	push	ax
	push	ss
	lea	ax,word ptr [bp-8]
	push	ax
	push	ss
	lea	ax,word ptr [bp-6]
	push	ax
	call	far ptr _getdisplay
	add	sp,12
   ;	
   ;	      moddisplay(twindow,SCROLL_UP,END_MOD);
   ;	
	push	0
	push	0
	jmp	short @6@1010
@6@982:
   ;	
   ;	      break;
   ;	    case 17:
   ;	      getdisplay(&x,&y,&twindow);
   ;	
	push	ss
	lea	ax,word ptr [bp-4]
	push	ax
	push	ss
	lea	ax,word ptr [bp-8]
	push	ax
	push	ss
	lea	ax,word ptr [bp-6]
	push	ax
	call	far ptr _getdisplay
	add	sp,12
   ;	
   ;	      moddisplay(twindow,SCROLL_DOWN,END_MOD);
   ;	
	push	0
	push	1
@6@1010:
	push	11
@6@1038:
	push	word ptr [bp-2]
	push	word ptr [bp-4]
	call	far ptr _moddisplay
	add	sp,10
   ;	
   ;	      break;
   ;	
	jmp	short @6@1066
@6@1066:
   ;	
   ;	  }
   ;	}
   ;	
	leave	
	ret	
_do_local	endp
@6@C850	label	word
	dw	@6@142
	dw	@6@226
	dw	@6@254
	dw	@6@282
	dw	@6@338
	dw	@6@394
	dw	@6@422
	dw	@6@450
	dw	@6@506
	dw	@6@674
	dw	@6@702
	dw	@6@730
	dw	@6@758
	dw	@6@786
	dw	@6@870
	dw	@6@926
	dw	@6@954
	dw	@6@982
   ;	
   ;	void process(short idx,char isbreak)
   ;	
	assume	cs:_TEXT
_process	proc	near
	enter	92,0
   ;	
   ;	{
   ;	  short key,save_idx=idx;
   ;	
	mov	ax,word ptr [bp+4]
	mov	word ptr [bp-2],ax
   ;	
   ;	  char op,done,tlen,topt,cnt;
   ;	  char temp_shift=sft_status;
   ;	
	mov	al,byte ptr DGROUP:_sft_status
	mov	byte ptr [bp-7],al
   ;	
   ;	  char find_actual;
   ;	  char line[80],place;
   ;	#ifdef DEBUG
   ;	/*    displayln(output,"kbd *H",(long)tempkbd);
   ;	    asm int 3;*/
   ;	#endif
   ;	    skip_keys;
   ;	
	mov	byte ptr [bp-6],0
	jmp	short @7@450
@7@58:
	mov	byte ptr [bp-3],0
	inc	word ptr [bp+4]
@7@86:
	les	bx,dword ptr DGROUP:_tempkbd
	les	bx,dword ptr es:[bx]
	mov	ax,word ptr [bp+4]
	inc	word ptr [bp+4]
	add	bx,ax
	mov	al,byte ptr es:[bx]
	cbw	
	mov	bx,ax
	cmp	bx,9
	ja	short @7@366
	shl	bx,1
	jmp	word ptr cs:@7@C3266[bx]
@7@170:
	mov	byte ptr [bp-3],1
@7@198:
	jmp	short @7@394
@7@226:
	les	bx,dword ptr DGROUP:_tempkbd
	les	bx,dword ptr es:[bx]
	add	bx,word ptr [bp+4]
	mov	al,byte ptr es:[bx]
	mov	byte ptr [bp-4],al
	inc	word ptr [bp+4]
	jmp	short @7@310
@7@254:
	inc	word ptr [bp+4]
	dec	byte ptr [bp-4]
@7@310:
	cmp	byte ptr [bp-4],0
	jne	short @7@254
	jmp	short @7@394
@7@366:
	push	1
	call	dword ptr DGROUP:_Exit
	pop	cx
@7@394:
	mov	al,byte ptr [bp-3]
	cbw	
	or	ax,ax
	je	short @7@86
	inc	byte ptr [bp-6]
@7@450:
	mov	al,byte ptr [bp-6]
	cmp	al,byte ptr [bp-7]
	jl	short @7@58
   ;	
   ;	    topt=tempkbd->keydata[idx++];  /*get options for found key*/
   ;	
	les	bx,dword ptr DGROUP:_tempkbd
	les	bx,dword ptr es:[bx]
	add	bx,word ptr [bp+4]
	mov	al,byte ptr es:[bx]
	mov	byte ptr [bp-5],al
	inc	word ptr [bp+4]
   ;	
   ;	    if ((topt&2)&&led_status&1)  /*if key has option of caps, and
   ;	
	test	byte ptr [bp-5],2
	je	short @7@562
	test	byte ptr DGROUP:_led_status,1
	je	short @7@562
   ;	
   ;	                                   caps lock is pressed, then toggle
   ;	                                   the actual shift bit*/
   ;	    {
   ;	      temp_shift^=1;
   ;	
	xor	byte ptr [bp-7],1
   ;	
   ;	      find_actual=true;          /*if status is changed, we need actual
   ;	
	mov	byte ptr [bp-8],1
@7@562:
   ;	
   ;	                                    key becaseu we found the wrong one*/
   ;	    }
   ;	    if ((topt&4)&&led_status&2)
   ;	
	test	byte ptr [bp-5],4
	je	short @7@646
	test	byte ptr DGROUP:_led_status,2
	je	short @7@646
   ;	
   ;	    {
   ;	      temp_shift^=1;             /*if key has option of num, and
   ;	
	xor	byte ptr [bp-7],1
   ;	
   ;	                                   num lock is pressed, then toggle
   ;	                                   the actual shift bit*/
   ;	      find_actual=true;          /*if status is changed, we need actual
   ;	
	mov	byte ptr [bp-8],1
@7@646:
   ;	
   ;	                                    key becaseu we found the wrong one*/
   ;	    }
   ;	    if (find_actual)
   ;	
	cmp	byte ptr [bp-8],0
	jne	@@1
	jmp	@7@1150
@@1:
   ;	
   ;	    {
   ;	      idx=save_idx;  /*go back to start of key defs*/
   ;	
	mov	ax,word ptr [bp-2]
	mov	word ptr [bp+4],ax
   ;	
   ;	      skip_keys;
   ;	
	mov	byte ptr [bp-6],0
	jmp	short @7@1094
@7@702:
	mov	byte ptr [bp-3],0
	inc	word ptr [bp+4]
@7@730:
	les	bx,dword ptr DGROUP:_tempkbd
	les	bx,dword ptr es:[bx]
	mov	ax,word ptr [bp+4]
	inc	word ptr [bp+4]
	add	bx,ax
	mov	al,byte ptr es:[bx]
	cbw	
	mov	bx,ax
	cmp	bx,9
	ja	short @7@1010
	shl	bx,1
	jmp	word ptr cs:@7@C3314[bx]
@7@814:
	mov	byte ptr [bp-3],1
@7@842:
	jmp	short @7@1038
@7@870:
	les	bx,dword ptr DGROUP:_tempkbd
	les	bx,dword ptr es:[bx]
	add	bx,word ptr [bp+4]
	mov	al,byte ptr es:[bx]
	mov	byte ptr [bp-4],al
	inc	word ptr [bp+4]
	jmp	short @7@954
@7@898:
	inc	word ptr [bp+4]
	dec	byte ptr [bp-4]
@7@954:
	cmp	byte ptr [bp-4],0
	jne	short @7@898
	jmp	short @7@1038
@7@1010:
	push	1
	call	dword ptr DGROUP:_Exit
	pop	cx
@7@1038:
	mov	al,byte ptr [bp-3]
	cbw	
	or	ax,ax
	je	short @7@730
	inc	byte ptr [bp-6]
@7@1094:
	mov	al,byte ptr [bp-6]
	cmp	al,byte ptr [bp-7]
	jl	short @7@702
   ;	
   ;	      idx++;  /*skip options for the actually found key*/
   ;	
	inc	word ptr [bp+4]
@7@1150:
   ;	
   ;	    }
   ;	    {
   ;	      done=false;
   ;	
	mov	byte ptr [bp-3],0
@7@1178:
   ;	
   ;	      do
   ;	      {
   ;	        switch(tempkbd->keydata[idx++])
   ;	
	les	bx,dword ptr DGROUP:_tempkbd
	les	bx,dword ptr es:[bx]
	mov	ax,word ptr [bp+4]
	inc	word ptr [bp+4]
	add	bx,ax
	mov	al,byte ptr es:[bx]
	cbw	
	mov	bx,ax
	cmp	bx,9
	jbe	@@2
	jmp	@7@2774
@@2:
	shl	bx,1
	jmp	word ptr cs:@7@C3362[bx]
@7@1262:
   ;	
   ;	        {
   ;	          case 0:/*this is the terminator following a string of commands*/
   ;	          case 9:/*This is the terminator if there were NO commands*/
   ;	                 done=true;
   ;	
	mov	byte ptr [bp-3],1
   ;	
   ;	                 break;
   ;	
	jmp	@7@2802
@7@1290:
   ;	
   ;	          case 1:
   ;	                 if (!isbreak)
   ;	
	mov	al,byte ptr [bp+6]
	cbw	
	or	ax,ax
	jne	short @7@1346
   ;	
   ;	                   sft_status|=1;
   ;	
	or	byte ptr DGROUP:_sft_status,1
	jmp	short @7@1374
@7@1346:
   ;	
   ;	                 else
   ;	                   sft_status&=~1;
   ;	
	and	byte ptr DGROUP:_sft_status,254
@7@1374:
   ;	
   ;	#ifdef DEBUG
   ;	                 displayln(output,"Shift: *b\n",sft_status);
   ;	#endif
   ;	                 break;
   ;	
	jmp	@7@2802
@7@1402:
   ;	
   ;	          case 2:
   ;	                 if (!isbreak)
   ;	
	mov	al,byte ptr [bp+6]
	cbw	
	or	ax,ax
	jne	short @7@1458
   ;	
   ;	                   sft_status|=2;
   ;	
	or	byte ptr DGROUP:_sft_status,2
	jmp	short @7@1486
@7@1458:
   ;	
   ;	                 else
   ;	                   sft_status&=~2;
   ;	
	and	byte ptr DGROUP:_sft_status,253
@7@1486:
   ;	
   ;	#ifdef DEBUG
   ;	                 displayln(output,"Shift: *b\n",sft_status);
   ;	#endif
   ;	                 break;
   ;	
	jmp	@7@2802
@7@1514:
   ;	
   ;	          case 3:
   ;	                 if (!isbreak)
   ;	
	mov	al,byte ptr [bp+6]
	cbw	
	or	ax,ax
	jne	short @7@1570
   ;	
   ;	                   sft_status|=4;
   ;	
	or	byte ptr DGROUP:_sft_status,4
	jmp	short @7@1598
@7@1570:
   ;	
   ;	                 else
   ;	                   sft_status&=~4;
   ;	
	and	byte ptr DGROUP:_sft_status,251
@7@1598:
   ;	
   ;	#ifdef DEBUG
   ;	                 displayln(output,"Shift: *b\n",sft_status);
   ;	#endif
   ;	                 break;
   ;	
	jmp	@7@2802
@7@1626:
   ;	
   ;	          case 4:
   ;	                 if (!isbreak)
   ;	
	mov	al,byte ptr [bp+6]
	cbw	
	or	ax,ax
	jne	short @7@1682
   ;	
   ;	                   led_status^=1;
   ;	
	xor	byte ptr DGROUP:_led_status,1
@7@1682:
   ;	
   ;	#ifdef DEBUG
   ;	                 displayln(output,"LED: *b\n",led_status);
   ;	#endif
   ;	                 break;
   ;	
	jmp	@7@2802
@7@1710:
   ;	
   ;	          case 5:
   ;	                 if (!isbreak)
   ;	
	mov	al,byte ptr [bp+6]
	cbw	
	or	ax,ax
	jne	short @7@1766
   ;	
   ;	                   led_status^=2;
   ;	
	xor	byte ptr DGROUP:_led_status,2
@7@1766:
   ;	
   ;	#ifdef DEBUG
   ;	                 displayln(output,"LED: *b\n",led_status);
   ;	#endif
   ;	                 break;
   ;	
	jmp	@7@2802
@7@1794:
   ;	
   ;	          case 6:
   ;	                 if (!isbreak)
   ;	
	mov	al,byte ptr [bp+6]
	cbw	
	or	ax,ax
	jne	short @7@1850
   ;	
   ;	                   led_status^=4;
   ;	
	xor	byte ptr DGROUP:_led_status,4
@7@1850:
   ;	
   ;	#ifdef DEBUG
   ;	                 displayln(output,"LED: *b\n",led_status);
   ;	#endif
   ;	                 break;
   ;	
	jmp	@7@2802
@7@1878:
   ;	
   ;	          case 7:
   ;	              if (!isbreak)
   ;	
	mov	al,byte ptr [bp+6]
	cbw	
	or	ax,ax
	jne	short @7@2074
   ;	
   ;	              {
   ;	                 tlen=tempkbd->keydata[idx++];
   ;	
	les	bx,dword ptr DGROUP:_tempkbd
	les	bx,dword ptr es:[bx]
	add	bx,word ptr [bp+4]
	mov	al,byte ptr es:[bx]
	mov	byte ptr [bp-4],al
	inc	word ptr [bp+4]
	jmp	short @7@2018
@7@1934:
   ;	
   ;	                 for (;tlen;tlen--)
   ;	                 {
   ;	                   /*Store Character(s) in key buffer*/
   ;	                   key_buf[keyhead]=tempkbd->keydata[idx++];
   ;	
	mov	al,byte ptr DGROUP:_keyhead
	cbw	
	les	bx,dword ptr DGROUP:_tempkbd
	les	bx,dword ptr es:[bx]
	add	bx,word ptr [bp+4]
	mov	dl,byte ptr es:[bx]
	mov	bx,ax
	mov	byte ptr DGROUP:_key_buf[bx],dl
	inc	word ptr [bp+4]
   ;	
   ;	                   keyhead++;
   ;	
	inc	byte ptr DGROUP:_keyhead
   ;	
   ;	                   keyhead&=(KEYBUFSIZE-1);
   ;	
	and	byte ptr DGROUP:_keyhead,127
   ;	
   ;	                   if (keyhead==keytail)
   ;	
	mov	al,byte ptr DGROUP:_keyhead
	cmp	al,byte ptr DGROUP:_keytail
	jne	short @7@1990
   ;	
   ;	                   {
   ;	                     keyhead--;
   ;	
	dec	byte ptr DGROUP:_keyhead
   ;	
   ;	                     keyhead&=(KEYBUFSIZE-1);
   ;	
	and	byte ptr DGROUP:_keyhead,127
@7@1990:
	dec	byte ptr [bp-4]
@7@2018:
	cmp	byte ptr [bp-4],0
	jne	short @7@1934
   ;	
   ;	                   }
   ;	                 }
   ;	              }
   ;	
	jmp	short @7@2186
@7@2074:
   ;	
   ;	              else
   ;	              {
   ;	                 tlen=tempkbd->keydata[idx++];     /*skip length*/
   ;	
	les	bx,dword ptr DGROUP:_tempkbd
	les	bx,dword ptr es:[bx]
	add	bx,word ptr [bp+4]
	mov	al,byte ptr es:[bx]
	mov	byte ptr [bp-4],al
	inc	word ptr [bp+4]
	jmp	short @7@2158
@7@2102:
   ;	
   ;	                 for (;tlen;tlen--)
   ;	                   idx++;
   ;	
	inc	word ptr [bp+4]
	dec	byte ptr [bp-4]
@7@2158:
	cmp	byte ptr [bp-4],0
	jne	short @7@2102
@7@2186:
   ;	
   ;	              }
   ;	              break;
   ;	
	jmp	@7@2802
@7@2214:
   ;	
   ;	          case 8:
   ;	              if (!isbreak)
   ;	
	mov	al,byte ptr [bp+6]
	cbw	
	or	ax,ax
	je	@@3
	jmp	@7@2634
@@3:
   ;	
   ;	              {
   ;	                 short i;
   ;	                 for (place=0,tlen=tempkbd->keydata[idx++];
   ;	
	mov	byte ptr [bp-9],0
	les	bx,dword ptr DGROUP:_tempkbd
	les	bx,dword ptr es:[bx]
	add	bx,word ptr [bp+4]
	mov	al,byte ptr es:[bx]
	mov	byte ptr [bp-4],al
	inc	word ptr [bp+4]
	jmp	short @7@2326
@7@2270:
   ;	
   ;	                      tlen;
   ;	                      tlen--,place++)
   ;	                   line[place]=tempkbd->keydata[idx++];
   ;	
	mov	al,byte ptr [bp-9]
	cbw	
	lea	dx,word ptr [bp-92]
	add	ax,dx
	les	bx,dword ptr DGROUP:_tempkbd
	les	bx,dword ptr es:[bx]
	add	bx,word ptr [bp+4]
	mov	dl,byte ptr es:[bx]
	mov	bx,ax
	mov	byte ptr ss:[bx],dl
	inc	word ptr [bp+4]
	dec	byte ptr [bp-4]
	inc	byte ptr [bp-9]
@7@2326:
	cmp	byte ptr [bp-4],0
	jne	short @7@2270
   ;	
   ;	                 line[place]=0;
   ;	
	mov	al,byte ptr [bp-9]
	cbw	
	lea	dx,word ptr [bp-92]
	add	ax,dx
	mov	bx,ax
	mov	byte ptr ss:[bx],0
   ;	
   ;	                 for (i=0;i<NUM_LOCAL;i++)
   ;	
	mov	word ptr [bp-12],0
	jmp	short @7@2466
@7@2382:
   ;	
   ;	                 {
   ;	                   if (stricmp(line,(char far *)functions[i])==0)
   ;	
	push	ds
	mov	bx,word ptr [bp-12]
	shl	bx,1
	push	word ptr DGROUP:_functions[bx]
	push	ss
	lea	ax,word ptr [bp-92]
	push	ax
	call	far ptr _stricmp
	add	sp,8
	or	ax,ax
	jne	short @7@2438
   ;	
   ;	                   {
   ;	                     do_local(i);
   ;	
	push	word ptr [bp-12]
	call	near ptr _do_local
	pop	cx
   ;	
   ;	                     break;
   ;	
	jmp	short @7@2494
@7@2438:
	inc	word ptr [bp-12]
@7@2466:
	cmp	word ptr [bp-12],18
	jl	short @7@2382
@7@2494:
   ;	
   ;	                   }
   ;	                 }
   ;	                 if (i<NUM_LOCAL)
   ;	
	cmp	word ptr [bp-12],18
	jge	short @7@2550
   ;	
   ;	                   break;  /*exit switch*/
   ;	
	jmp	short @7@2802
@7@2550:
   ;	
   ;	                 if (Locate(line))
   ;	
	push	ss
	lea	ax,word ptr [bp-92]
	push	ax
	call	near ptr _Locate
	add	sp,4
	or	ax,ax
	je	short @7@2606
   ;	
   ;	                 {
   ;	#ifdef DEBUG
   ;	                   displayln(output,"routine");
   ;	#endif
   ;	                   call(line);
   ;	
	push	ss
	lea	ax,word ptr [bp-92]
	push	ax
	call	near ptr _call
	add	sp,4
   ;	
   ;	
   ;	#ifdef DEBUG
   ;	                   displayln(output," found\n");
   ;	#endif
   ;	                 }
   ;	
	jmp	short @7@2606
@7@2606:
   ;	
   ;	                 else
   ;	                 {
   ;	#ifdef DEBUG
   ;	                   displayln(output,"Function *s not found\n",(char far *)line);
   ;	#endif
   ;	                 }
   ;	              }
   ;	
	jmp	short @7@2746
@7@2634:
   ;	
   ;	              else
   ;	              {
   ;	                 tlen=tempkbd->keydata[idx++];     /*skip length*/
   ;	
	les	bx,dword ptr DGROUP:_tempkbd
	les	bx,dword ptr es:[bx]
	add	bx,word ptr [bp+4]
	mov	al,byte ptr es:[bx]
	mov	byte ptr [bp-4],al
	inc	word ptr [bp+4]
	jmp	short @7@2718
@7@2662:
   ;	
   ;	                 for (;tlen;tlen--)
   ;	                   idx++;
   ;	
	inc	word ptr [bp+4]
	dec	byte ptr [bp-4]
@7@2718:
	cmp	byte ptr [bp-4],0
	jne	short @7@2662
@7@2746:
   ;	
   ;	              }
   ;	                 break;
   ;	
	jmp	short @7@2802
@7@2774:
   ;	
   ;	          default:
   ;	#ifdef DEBUG
   ;	                  displayln(output,"invalid opcode\n");
   ;	#endif
   ;	                  Exit(1);
   ;	
	push	1
	call	dword ptr DGROUP:_Exit
	pop	cx
@7@2802:
   ;	
   ;	        }
   ;	      }while (!done);
   ;	
	mov	al,byte ptr [bp-3]
	cbw	
	or	ax,ax
	jne	@@4
	jmp	@7@1178
@@4:
   ;	
   ;	    }
   ;	  }
   ;	
	leave	
	ret	
_process	endp
@7@C3362	label	word
	dw	@7@1262
	dw	@7@1290
	dw	@7@1402
	dw	@7@1514
	dw	@7@1626
	dw	@7@1710
	dw	@7@1794
	dw	@7@1878
	dw	@7@2214
	dw	@7@1262
@7@C3314	label	word
	dw	@7@814
	dw	@7@842
	dw	@7@842
	dw	@7@842
	dw	@7@842
	dw	@7@842
	dw	@7@842
	dw	@7@870
	dw	@7@870
	dw	@7@814
@7@C3266	label	word
	dw	@7@170
	dw	@7@198
	dw	@7@198
	dw	@7@198
	dw	@7@198
	dw	@7@198
	dw	@7@198
	dw	@7@226
	dw	@7@226
	dw	@7@170
   ;	
   ;	void read_kbd_data()
   ;	
	assume	cs:_TEXT
_read_kbd_data	proc	near
	enter	90,0
   ;	
   ;	{
   ;	  short temp;
   ;	  short datalen;
   ;	  char fname[80];
   ;	  prefix_data far *tpretable;
   ;	  char len;
   ;	/*  tempkbd=(kbd far *)Get_environ("KEYBOARD");*/
   ;	  tempkbd=Allocate(sizeof(kbd));
   ;	
	push	13
	call	dword ptr DGROUP:_Allocate
	pop	cx
	mov	word ptr DGROUP:_tempkbd+2,dx
	mov	word ptr DGROUP:_tempkbd,ax
   ;	
   ;	  strncpy(fname,current_path,80);
   ;	
	push	80
	push	word ptr DGROUP:_current_path+2
	push	word ptr DGROUP:_current_path
	push	ss
	lea	ax,word ptr [bp-90]
	push	ax
	call	far ptr _strncpy
	add	sp,10
   ;	
   ;	  strcat(fname,"config.i");
   ;	
	push	ds
	push	offset DGROUP:s@+224
	push	ss
	lea	ax,word ptr [bp-90]
	push	ax
	call	far ptr _strcat
	add	sp,8
   ;	
   ;	  temp=open(fname,0);
   ;	
	push	0
	push	ss
	lea	ax,word ptr [bp-90]
	push	ax
	call	far ptr _open
	add	sp,6
	mov	word ptr [bp-2],ax
   ;	
   ;	  if (temp==-1)
   ;	
	cmp	word ptr [bp-2],-1
	jne	short @8@86
   ;	
   ;	  {
   ;	#ifdef DEBUG
   ;	    displayln(output,"Error opening Configuration file config.i.");
   ;	#endif
   ;	    perish();
   ;	
	call	near ptr _perish
@8@86:
   ;	
   ;	  }
   ;	  read(&datalen,2,temp);
   ;	
	push	word ptr [bp-2]
	push	2
	push	ss
	lea	ax,word ptr [bp-4]
	push	ax
	call	far ptr _read
	add	sp,8
   ;	
   ;	  tempkbd->keydata=Allocate(datalen);
   ;	
	push	word ptr [bp-4]
	call	dword ptr DGROUP:_Allocate
	pop	cx
	les	bx,dword ptr DGROUP:_tempkbd
	mov	word ptr es:[bx+2],dx
	mov	word ptr es:[bx],ax
   ;	
   ;	  if (!tempkbd->keydata)
   ;	
	les	bx,dword ptr DGROUP:_tempkbd
	mov	ax,word ptr es:[bx]
	or	ax,word ptr es:[bx+2]
	jne	short @8@142
   ;	
   ;	  {
   ;	#ifdef DEBUG
   ;	    displayln(output,"Error allocating a buffer *d bytes long.",datalen);
   ;	#endif
   ;	    perish();
   ;	
	call	near ptr _perish
@8@142:
   ;	
   ;	  }
   ;	  read(&tempkbd->maxcode,1,temp);
   ;	
	push	word ptr [bp-2]
	push	1
	mov	ax,word ptr DGROUP:_tempkbd
	add	ax,8
	push	word ptr DGROUP:_tempkbd+2
	push	ax
	call	far ptr _read
	add	sp,8
   ;	
   ;	
   ;	  tempkbd->table=Allocate(tempkbd->maxcode*2);
   ;	
	les	bx,dword ptr DGROUP:_tempkbd
	mov	al,byte ptr es:[bx+8]
	mov	ah,0
	shl	ax,1
	push	ax
	call	dword ptr DGROUP:_Allocate
	pop	cx
	les	bx,dword ptr DGROUP:_tempkbd
	mov	word ptr es:[bx+6],dx
	mov	word ptr es:[bx+4],ax
   ;	
   ;	
   ;	  read(tempkbd->table,tempkbd->maxcode*2,temp);
   ;	
	push	word ptr [bp-2]
	les	bx,dword ptr DGROUP:_tempkbd
	mov	al,byte ptr es:[bx+8]
	mov	ah,0
	shl	ax,1
	push	ax
	les	bx,dword ptr DGROUP:_tempkbd
	push	word ptr es:[bx+6]
	push	word ptr es:[bx+4]
	call	far ptr _read
	add	sp,8
   ;	
   ;	
   ;	  tpretable=tempkbd->prefix_table=Allocate(sizeof(prefix_data));
   ;	
	push	14
	call	dword ptr DGROUP:_Allocate
	pop	cx
	les	bx,dword ptr DGROUP:_tempkbd
	mov	word ptr es:[bx+11],dx
	mov	word ptr es:[bx+9],ax
	mov	word ptr [bp-6],dx
	mov	word ptr [bp-8],ax
	jmp	@8@198
@8@170:
   ;	
   ;	  while (read(&len,1,temp)&&len)
   ;	  {
   ;	    read(&tpretable->prefix,1,temp);
   ;	
	push	word ptr [bp-2]
	push	1
	push	word ptr [bp-6]
	push	word ptr [bp-8]
	call	far ptr _read
	add	sp,8
   ;	
   ;	    tpretable->length=len;
   ;	
	les	bx,dword ptr [bp-8]
	mov	al,byte ptr [bp-9]
	mov	byte ptr es:[bx+1],al
   ;	
   ;	    tpretable->scancodes=Allocate(len);
   ;	
	mov	al,byte ptr [bp-9]
	cbw	
	push	ax
	call	dword ptr DGROUP:_Allocate
	pop	cx
	les	bx,dword ptr [bp-8]
	mov	word ptr es:[bx+4],dx
	mov	word ptr es:[bx+2],ax
   ;	
   ;	    read(tpretable->scancodes,len,temp);
   ;	
	push	word ptr [bp-2]
	mov	al,byte ptr [bp-9]
	cbw	
	push	ax
	les	bx,dword ptr [bp-8]
	push	word ptr es:[bx+4]
	push	word ptr es:[bx+2]
	call	far ptr _read
	add	sp,8
   ;	
   ;	    tpretable->table=Allocate(len*2);
   ;	
	mov	al,byte ptr [bp-9]
	cbw	
	shl	ax,1
	push	ax
	call	dword ptr DGROUP:_Allocate
	pop	cx
	les	bx,dword ptr [bp-8]
	mov	word ptr es:[bx+8],dx
	mov	word ptr es:[bx+6],ax
   ;	
   ;	    read(tpretable->table,len*2,temp);
   ;	
	push	word ptr [bp-2]
	mov	al,byte ptr [bp-9]
	cbw	
	shl	ax,1
	push	ax
	les	bx,dword ptr [bp-8]
	push	word ptr es:[bx+8]
	push	word ptr es:[bx+6]
	call	far ptr _read
	add	sp,8
   ;	
   ;	    tpretable->next=Allocate(sizeof(prefix_data));
   ;	
	push	14
	call	dword ptr DGROUP:_Allocate
	pop	cx
	les	bx,dword ptr [bp-8]
	mov	word ptr es:[bx+12],dx
	mov	word ptr es:[bx+10],ax
   ;	
   ;	    tpretable=tpretable->next;
   ;	
	les	bx,dword ptr [bp-8]
	mov	ax,word ptr es:[bx+12]
	mov	dx,word ptr es:[bx+10]
	mov	word ptr [bp-6],ax
	mov	word ptr [bp-8],dx
@8@198:
	push	word ptr [bp-2]
	push	1
	push	ss
	lea	ax,word ptr [bp-9]
	push	ax
	call	far ptr _read
	add	sp,8
	or	ax,ax
	je	short @8@254
	cmp	byte ptr [bp-9],0
	je	@@5
	jmp	@8@170
@@5:
@8@254:
   ;	
   ;	  }
   ;	  tpretable->length=len;
   ;	
	les	bx,dword ptr [bp-8]
	mov	al,byte ptr [bp-9]
	mov	byte ptr es:[bx+1],al
   ;	
   ;	  tpretable->next=0L;
   ;	
	les	bx,dword ptr [bp-8]
	mov	word ptr es:[bx+12],0
	mov	word ptr es:[bx+10],0
   ;	
   ;	  read(tempkbd->keydata,datalen,temp);
   ;	
	push	word ptr [bp-2]
	push	word ptr [bp-4]
	les	bx,dword ptr DGROUP:_tempkbd
	push	word ptr es:[bx+2]
	push	word ptr es:[bx]
	call	far ptr _read
	add	sp,8
   ;	
   ;	  close(temp);
   ;	
	push	word ptr [bp-2]
	call	far ptr _close
	pop	cx
   ;	
   ;	}
   ;	
	leave	
	ret	
_read_kbd_data	endp
_TEXT	ends
_DATA	segment word public 'DATA'
_rawkeypressedname	label	byte
	db	114
	db	97
	db	119
	db	107
	db	101
	db	121
	db	112
	db	114
	db	101
	db	115
	db	115
	db	101
	db	100
	db	0
_DATA	ends
_TEXT	segment byte public 'CODE'
   ;	
   ;	public(char,rawkeypressed,(void))
   ;	
	assume	cs:_TEXT
__rawkeypressed	proc	near
	push	bp
	mov	bp,sp
	push	si
 	push	 ds
 	mov	 dx,offset _rawkeypressed
 	mov	 si,offset DGROUP:_rawkeypressedname
 	mov	 ax,seg DGROUP:_rawkeypressedname
 	mov	 es,ax
 	mov	 ax,seg _rawkeypressed
 	mov	 ds,ax
 	mov	 ah,1
 	int	 060H
 	pop	 ds
	pop	si
	pop	bp
	ret	
__rawkeypressed	endp
   ;	
   ;	public(char,rawkeypressed,(void))
   ;	
	assume	cs:_TEXT
_rawkeypressed	proc	far
	push	bp
	mov	bp,sp
   ;	
   ;	{
   ;	  Load_DS;
   ;	
 	push	 ds
 	mov	 ax,seg _Relinquish
 	mov	 ds,ax
   ;	
   ;	  if (keyhead!=keytail)
   ;	
	mov	al,byte ptr DGROUP:_keyhead
	cmp	al,byte ptr DGROUP:_keytail
	je	short @10@254
   ;	
   ;	  {
   ;	    asm pop ds;
   ;	
 	pop	 ds
   ;	
   ;	    return(true);
   ;	
	mov	al,1
@10@198:
	jmp	short @10@310
   ;	
   ;	  }
   ;	
	jmp	short @10@310
@10@254:
   ;	
   ;	  else
   ;	  {
   ;	    asm pop ds;
   ;	
 	pop	 ds
   ;	
   ;	    return(false);
   ;	
	mov	al,0
	jmp	short @10@198
@10@310:
   ;	
   ;	  }
   ;	}
   ;	
	pop	bp
	ret	
_rawkeypressed	endp
_TEXT	ends
_DATA	segment word public 'DATA'
_rawreadchname	label	byte
	db	114
	db	97
	db	119
	db	114
	db	101
	db	97
	db	100
	db	99
	db	104
	db	0
_DATA	ends
_TEXT	segment byte public 'CODE'
   ;	
   ;	public(char,rawreadch,(void))
   ;	
	assume	cs:_TEXT
__rawreadch	proc	near
	push	bp
	mov	bp,sp
	push	si
 	push	 ds
 	mov	 dx,offset _rawreadch
 	mov	 si,offset DGROUP:_rawreadchname
 	mov	 ax,seg DGROUP:_rawreadchname
 	mov	 es,ax
 	mov	 ax,seg _rawreadch
 	mov	 ds,ax
 	mov	 ah,1
 	int	 060H
 	pop	 ds
	pop	si
	pop	bp
	ret	
__rawreadch	endp
   ;	
   ;	public(char,rawreadch,(void))
   ;	
	assume	cs:_TEXT
_rawreadch	proc	far
	enter	2,0
   ;	
   ;	{
   ;	  char temp;
   ;	  Load_DS;
   ;	
 	push	 ds
 	mov	 ax,seg _Relinquish
 	mov	 ds,ax
	jmp	short @12@170
@12@142:
   ;	
   ;	  while (keyhead==keytail)
   ;	  {
   ;	    Relinquish(0L);
   ;	
	push	0
	push	0
	call	dword ptr DGROUP:_Relinquish
	add	sp,4
@12@170:
	mov	al,byte ptr DGROUP:_keyhead
	cmp	al,byte ptr DGROUP:_keytail
	je	short @12@142
   ;	
   ;	  }
   ;	  temp=key_buf[keytail];
   ;	
	mov	al,byte ptr DGROUP:_keytail
	cbw	
	mov	bx,ax
	mov	al,byte ptr DGROUP:_key_buf[bx]
	mov	byte ptr [bp-1],al
   ;	
   ;	  keytail++;
   ;	
	inc	byte ptr DGROUP:_keytail
   ;	
   ;	  keytail&=(KEYBUFSIZE-1);
   ;	
	and	byte ptr DGROUP:_keytail,127
   ;	
   ;	  asm pop ds;
   ;	
 	pop	 ds
   ;	
   ;	  return(temp);
   ;	
	mov	al,byte ptr [bp-1]
	jmp	short @12@282
@12@282:
   ;	
   ;	}
   ;	
	leave	
	ret	
_rawreadch	endp
   ;	
   ;	void main(char argc,char far *path)
   ;	
	assume	cs:_TEXT
_main	proc	near
	enter	8,0
   ;	
   ;	{
   ;	  char ch=0,cnt,len;
   ;	
	mov	byte ptr [bp-1],0
   ;	
   ;	  char temp;
   ;	  char far *line;
   ;	  current_path=path;
   ;	
	mov	ax,word ptr [bp+8]
	mov	dx,word ptr [bp+6]
	mov	word ptr DGROUP:_current_path+2,ax
	mov	word ptr DGROUP:_current_path,dx
   ;	
   ;	  read_kbd_data();
   ;	
	call	near ptr _read_kbd_data
   ;	
   ;	  _disconnect_kbd();
   ;	
	call	near ptr __disconnect_kbd
   ;	
   ;	  connect_kbd();
   ;	
	call	near ptr _connect_kbd
   ;	
   ;	#ifdef DEBUG
   ;	  output=opendisplay(30,2,20,15,BORDER|NEWLINE,0x1f,0x1f,0x2f,"Keyboard");
   ;	  moddisplay(output,ICON_WINDOW,END_MOD);
   ;	#endif
   ;	  _rawkeypressed();
   ;	
	call	near ptr __rawkeypressed
   ;	
   ;	  _rawreadch();
   ;	
	call	near ptr __rawreadch
   ;	
   ;	  keytask=my_TCB;
   ;	
	les	bx,dword ptr DGROUP:_my_tcbptr
	mov	ax,word ptr es:[bx+2]
	mov	dx,word ptr es:[bx]
	mov	word ptr DGROUP:_keytask+2,ax
	mov	word ptr DGROUP:_keytask,dx
@13@58:
   ;	
   ;	  while (1)
   ;	  {
   ;	    if (schead!=sctail)
   ;	
	mov	al,byte ptr DGROUP:_schead
	cmp	al,byte ptr DGROUP:_sctail
	jne	@@6
	jmp	@13@898
@@6:
   ;	
   ;	    {
   ;	      ch=kbdch[sctail++];
   ;	
	mov	al,byte ptr DGROUP:_sctail
	cbw	
	mov	bx,ax
	mov	al,byte ptr DGROUP:_kbdch[bx]
	mov	byte ptr [bp-1],al
	inc	byte ptr DGROUP:_sctail
   ;	
   ;	      if (sctail==50) sctail=0;
   ;	
	cmp	byte ptr DGROUP:_sctail,50
	jne	short @13@142
	mov	byte ptr DGROUP:_sctail,0
@13@142:
   ;	
   ;	      if ((ch&0x7f)<tempkbd->maxcode)
   ;	
	mov	al,byte ptr [bp-1]
	cbw	
	and	ax,127
	les	bx,dword ptr DGROUP:_tempkbd
	mov	dl,byte ptr es:[bx+8]
	mov	dh,0
	cmp	ax,dx
	jge	short @13@338
   ;	
   ;	      {
   ;	        short idx;
   ;	        {
   ;	          if (skip)
   ;	
	cmp	byte ptr DGROUP:_skip,0
	je	short @13@226
   ;	
   ;	          {
   ;	            skip=false;
   ;	
	mov	byte ptr DGROUP:_skip,0
   ;	
   ;	            continue;
   ;	
	jmp	@13@1010
@13@226:
   ;	
   ;	          }
   ;	          idx=tempkbd->table[ch&0x7f];
   ;	
	mov	al,byte ptr [bp-1]
	cbw	
	and	ax,127
	shl	ax,1
	les	bx,dword ptr DGROUP:_tempkbd
	les	bx,dword ptr es:[bx+4]
	add	bx,ax
	mov	ax,word ptr es:[bx]
	mov	word ptr [bp-4],ax
   ;	
   ;	          if (idx==-1)
   ;	
	cmp	word ptr [bp-4],-1
	jne	short @13@282
   ;	
   ;	          {
   ;	#ifdef DEBUG
   ;	            displayln(output,"No Definition\n");
   ;	#endif
   ;	            beep(1);
   ;	
	push	1
	call	near ptr _beep
	pop	cx
   ;	
   ;	          }
   ;	
	jmp	short @13@310
@13@282:
   ;	
   ;	          else
   ;	          {
   ;	            process(idx,ch&0x80);
   ;	
	mov	al,byte ptr [bp-1]
	and	al,128
	push	ax
	push	word ptr [bp-4]
	call	near ptr _process
	add	sp,4
@13@310:
   ;	
   ;	          }
   ;	        }
   ;	      }
   ;	
	jmp	@13@898
@13@338:
   ;	
   ;	      else
   ;	      {
   ;	        prefix_data far *temp=tempkbd->prefix_table;
   ;	
	les	bx,dword ptr DGROUP:_tempkbd
	mov	ax,word ptr es:[bx+11]
	mov	dx,word ptr es:[bx+9]
	jmp	@13@814
@13@366:
   ;	
   ;	        while (temp)
   ;	        {
   ;	          if (temp->prefix==ch)
   ;	
	les	bx,dword ptr [bp-6]
	mov	al,byte ptr es:[bx]
	cmp	al,byte ptr [bp-1]
	je	@@7
	jmp	@13@786
@@7:
	jmp	short @13@450
@13@422:
   ;	
   ;	          {
   ;	            char idx;
   ;	            while (sctail==schead) Relinquish(0L);
   ;	
	push	0
	push	0
	call	dword ptr DGROUP:_Relinquish
	add	sp,4
@13@450:
	mov	al,byte ptr DGROUP:_sctail
	cmp	al,byte ptr DGROUP:_schead
	je	short @13@422
   ;	
   ;	            ch=kbdch[sctail++];
   ;	
	mov	al,byte ptr DGROUP:_sctail
	cbw	
	mov	bx,ax
	mov	al,byte ptr DGROUP:_kbdch[bx]
	mov	byte ptr [bp-1],al
	inc	byte ptr DGROUP:_sctail
   ;	
   ;	            if (sctail==50) sctail=0;
   ;	
	cmp	byte ptr DGROUP:_sctail,50
	jne	short @13@534
	mov	byte ptr DGROUP:_sctail,0
@13@534:
   ;	
   ;	            {
   ;	              for (idx=0;idx<temp->length;idx++)
   ;	
	mov	byte ptr [bp-7],0
	jmp	short @13@730
@13@562:
   ;	
   ;	              {
   ;	                if (temp->scancodes[idx]==(ch&0x7f))
   ;	
	mov	al,byte ptr [bp-7]
	cbw	
	les	bx,dword ptr [bp-6]
	les	bx,dword ptr es:[bx+2]
	add	bx,ax
	mov	al,byte ptr es:[bx]
	cbw	
	push	ax
	mov	al,byte ptr [bp-1]
	cbw	
	and	ax,127
	pop	dx
	cmp	dx,ax
	jne	short @13@702
   ;	
   ;	                {
   ;	                  if (temp->table[idx]==-1)
   ;	
	mov	al,byte ptr [bp-7]
	cbw	
	shl	ax,1
	les	bx,dword ptr [bp-6]
	les	bx,dword ptr es:[bx+6]
	add	bx,ax
	cmp	word ptr es:[bx],-1
	jne	short @13@646
   ;	
   ;	                  {
   ;	#ifdef DEBUG
   ;	                    displayln(output,"No Definition\n");
   ;	#endif
   ;	                    beep(1);
   ;	
	push	1
	call	near ptr _beep
	pop	cx
   ;	
   ;	                  }
   ;	
	jmp	short @13@674
@13@646:
   ;	
   ;	                  else
   ;	                  {
   ;	                    process(temp->table[idx],ch&0x80);
   ;	
	mov	al,byte ptr [bp-1]
	and	al,128
	push	ax
	mov	al,byte ptr [bp-7]
	cbw	
	shl	ax,1
	les	bx,dword ptr [bp-6]
	les	bx,dword ptr es:[bx+6]
	add	bx,ax
	push	word ptr es:[bx]
	call	near ptr _process
	add	sp,4
@13@674:
   ;	
   ;	                  }
   ;	                  break;
   ;	
	jmp	short @13@758
@13@702:
	inc	byte ptr [bp-7]
@13@730:
	les	bx,dword ptr [bp-6]
	mov	al,byte ptr es:[bx+1]
	cmp	al,byte ptr [bp-7]
	jg	short @13@562
@13@758:
   ;	
   ;	                }
   ;	              }
   ;	            }
   ;	            break;
   ;	
	jmp	short @13@870
@13@786:
   ;	
   ;	          }
   ;	          temp=temp->next;
   ;	
	les	bx,dword ptr [bp-6]
	mov	ax,word ptr es:[bx+12]
	mov	dx,word ptr es:[bx+10]
@13@814:
	mov	word ptr [bp-4],ax
	mov	word ptr [bp-6],dx
	mov	ax,word ptr [bp-6]
	or	ax,word ptr [bp-4]
	je	@@8
	jmp	@13@366
@@8:
@13@870:
   ;	
   ;	        }
   ;	        if (!temp)
   ;	
	mov	ax,word ptr [bp-6]
	or	ax,word ptr [bp-4]
	jne	short @13@898
@13@898:
   ;	
   ;	        {
   ;	#ifdef DEBUG
   ;	          displayln(output,"NO PREFIX!\n");
   ;	#endif
   ;	        }
   ;	/*        beep(2);*/
   ;	      }
   ;	    }
   ;	    if (schead==sctail)
   ;	
	mov	al,byte ptr DGROUP:_schead
	cmp	al,byte ptr DGROUP:_sctail
	jne	short @13@954
   ;	
   ;	      Relinquish(-10000L);
   ;	
	push	-1
	push	-10000
	jmp	short @13@982
@13@954:
   ;	
   ;	    else
   ;	      Relinquish(0L);
   ;	
	push	0
	push	0
@13@982:
	call	dword ptr DGROUP:_Relinquish
	add	sp,4
@13@1010:
	jmp	@13@58
   ;	
   ;	  }
   ;	}
   ;	
	leave	
	ret	
_main	endp
_TEXT	ends
_BSS	segment word public 'BSS'
_tempkbd	label	dword
	db	4 dup (?)
_skip	label	byte
	db	1 dup (?)
_c	label	byte
	db	1 dup (?)
_key_buf	label	byte
	db	128 dup (?)
_current_path	label	dword
	db	4 dup (?)
_keytask	label	dword
	db	4 dup (?)
	?debug	C E9
	?debug	C FA00000000
_BSS	ends
_DATA	segment word public 'DATA'
s@	label	byte
	db	'Toggle'
	db	0
	db	'Caps'
	db	0
	db	'Num'
	db	0
	db	'LED'
	db	0
	db	'Scroll'
	db	0
	db	'Skip'
	db	0
	db	'windowleft'
	db	0
	db	'windowright'
	db	0
	db	'windowup'
	db	0
	db	'windowdown'
	db	0
	db	'resizeleft'
	db	0
	db	'resizeright'
	db	0
	db	'resizeup'
	db	0
	db	'resizedown'
	db	0
	db	'windowicon'
	db	0
	db	'windowselect'
	db	0
	db	'mouseleft'
	db	0
	db	'mouseright'
	db	0
	db	'mouseup'
	db	0
	db	'mousedown'
	db	0
	db	'scrollleft'
	db	0
	db	'scrollright'
	db	0
	db	'scrollup'
	db	0
	db	'scrolldown'
	db	0
	db	'config.i'
	db	0
_DATA	ends
_TEXT	segment byte public 'CODE'
_TEXT	ends
	extrn	_beep:near
	public	_main
	public	__rawreadch
	public	_rawreadch
	public	_rawreadchname
	public	__rawkeypressed
	public	_rawkeypressed
	public	_rawkeypressedname
	public	_read_kbd_data
	public	_process
	public	_do_local
	public	_functions
	public	_call
	public	__disconnect_kbd
	public	_disconnect_kbd
	public	_disconnect_kbdname
	public	_connect_kbd
	public	_tempkbd
	public	_option
	extrn	_kbdch:byte
	extrn	_sctail:byte
	extrn	_schead:byte
	extrn	_keyintr:near
	public	_sft_status
	public	_led_status
	extrn	_old_key:dword
	public	_skip
	public	_c
	public	_keytail
	public	_keyhead
	public	_key_buf
	public	_current_path
	public	_keytask
	public	_output
	extrn	_moddisplay:far
	extrn	_getdisplay:far
	extrn	_strncpy:far
	extrn	_strcat:far
	extrn	_stricmp:far
	extrn	_read:far
	extrn	_close:far
	extrn	_open:far
	public	_Locate
	extrn	_perish:near
	extrn	_Allocate:dword
	extrn	_Relinquish:dword
	extrn	_Exit:dword
	extrn	_my_tcbptr:dword
_s@	equ	s@
	end
