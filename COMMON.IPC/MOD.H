#include "os.h"
#define __far far
#pragma warn -rvl
extern long lmult(long num1,long num2);
extern long ldiv(long number,long divisor);
extern long lmod(long number,long divisor);
extern long Request;
#define Load_DS  { asm push ds;           \
                   asm mov ax,seg Relinquish; \
                   asm mov ds,ax; }

#define Restore_DS {asm pop ds;}

/*extern proc (far *Request)(routine);
                      Replace by Dynamic declarations

extern void (far *Register_t)(char far *name,short procedure);
#define Register(a,b) {asm push cs;        \
                       Register_t(a,(short)b); \
                       asm pop cx;}
         Replace by Public  Declarations, and Call to _routine()  */

extern module far *far *my_tcbptr;
#define my_TCB (*my_tcbptr)

extern void (far *swap_to)(module far *next_to_run);
extern void (far *delay)(unsigned short delaytime);
extern void (far *change_priority)(char new_priority);
extern void (far *Exit)(short code);
extern void (far *Relinquish)(long param);
extern short (far *Export)(char far*destination,
                            void far *data);
extern void far *(far *Import)(char far*WhoIAm);
extern void far *(far *Terminate)(char far *whose,char pos,
                                  char cnt,short far *inprogress,...);
extern void (far *Wake)(short mask,short spec);
extern module far *(far *spawn_batch)(char far *batch_name);
extern module far *(far *spawn)(char far *name,short stack,char far *dev_name,
                             short threads,char priority);
extern char far *(far *Get_environ)(char far *environ_name);
extern void far *(far *Allocate)(short length);
extern void (far *Free)(void far *pointer);
extern void (far *Connect_Int)(void far *routine,short Int_Num);
extern void (far *Disconnect_Int)(void far *routine,short Int_Num);
extern void (far *gettime)(time_struc far *time);
extern short far *process_control;
extern short dataseg;

#define device_name (my_TCB->device_name)
/*extern char far *device_name;*/
extern void perish(void);
extern short fork(char relation);
extern void destory(void);
extern void far *(far *Inquire_begin)(short what);

#define FIELD_SET(word,pos,width,value)  (word&=                        \
                                       ~(((1<<width)-1)<<pos),            \
                                           word|=            \
                                                (value&((1<<width)-1))\
                                                       <<pos   )

#define FIELD_GET(word,pos,width) ((word&(((1<<width)-1)<<pos))       \
                                                 >>pos)



#ifdef relocatable
#define public_code(routine) { asm push ds;                    \
                               asm mov dx,offset routine;      \
                               asm mov si,offset routine##name;\
                               asm mov ax,seg routine##name;   \
                               asm mov es,ax;                  \
                               asm mov ax,seg routine;         \
                               asm mov ds,ax;                  \
                               asm mov ah,1;                   \
                               asm int 0x60;                   \
                               asm pop ds;}
#else
#define public_code(routine) { asm push ds;\
                               asm mov dx,offset routine;\
                               asm mov si,offset routine##name;\
                               asm mov ax,cs;\
                               asm mov es,ax;\
                               asm mov ds,ax;\
                               asm mov ah,1;\
                               asm int 0x60;\
                               asm pop ds;}
#endif

#define public(return,routine,args)  char routine##name[]=#routine; \
                           return (far routine) args;         \
                           void _##routine(void) public_code(routine);\
                           return (far routine) args

#define cleanup(return,routine,args)  char routine##name[]="!"#routine; \
                           return (far routine) args;         \
                           void _##routine(void) public_code(routine);\
                           return (far routine) args


#ifdef relocatable
#define Link_Code(routine) {             \
   asm  push    ds;                      \
do_jmp:                                  \
   asm  mov     ax,seg  Request;         \
   asm  mov     ds,ax;                   \
jmp_adr2:                                \
   asm  mov     bx,offset  Request;      \
jmp_adr3:                                \
   asm  jmp     call_request;            \
   asm  jmp     exit_request;            \
call_request:                            \
   asm  push    routine##name+2;         \
   asm  push    routine##name;           \
   asm  call    far [bx];                \
   asm  add     sp,4;                    \
   asm  mov     word ptr do_jmp+1,dx;    \
   asm  mov     word ptr jmp_adr2+1,ax;  \
   asm  mov     word ptr jmp_adr3,2fffh; \
exit_request:                            \
   asm  pop     ds;                      \
}

#define Link_Code1(routine)  {\
  asm pop ds;                     \
  asm pop bp;                     \
  asm mov cx,offset routine;      \
  asm mov si,seg routine;         \
  asm mov dx,offset routine##name;\
  asm mov ax,seg routine##name;   \
  asm mov es,ax;                  \
  asm mov ax,0;\
  asm int 60h;\
}
#else
#define Link_Code1(routine)  {\
  asm pop bp;                     \
  asm mov cx,offset routine;      \
  asm mov dx,offset routine##name;\
  asm mov ax,cs;                  \
  asm mov es,ax;                  \
  asm mov ax,0;\
  asm int 60h;\
}
#endif

#ifdef extern_def
#define dynamic(return,routine,paramlist) /*extern char routine##name[];*/ \
                               extern return (far routine)paramlist;
#else
#define dynamic(return,routine,paramlist)/* char routine##name[]=#routine; */\
                                          return (far routine)paramlist; \
                                         /* Link_Code(routine)*/
#endif

#ifdef relocatable
#define Find_Code(routine)  asm mov dx,offset routine##name,\
  asm mov ax,seg routine##name;   \
  asm mov es,ax \
  asm mov ah,2\
  asm int 60h  \
  asm mov temp,ax
#else
#define Find_Code(routine)  asm mov dx,offset routine##name,\
  asm mov ax,cs,\
  asm mov es,ax,\
  asm mov ah,2,\
  asm int 60h,\
  asm mov temp,ax
#endif

#ifdef extern_def
extern short Locate(char far *string);
#else
short Locate(char far *string)
{
  short temp;
  asm mov dx,word ptr string;
  asm mov es,word ptr string+2;
  asm mov ah,2;
  asm int 60h;
  asm mov temp,ax;
  return(temp);
}
#endif

typedef short ether_type;
typedef ether_type far *etherptr;

dynamic( void,shell,(char far *line));
dynamic( short, atoi,(char far *line));
dynamic( long, atol,(char far *line));
dynamic( short, itoa,(short number,char far *line));
dynamic( short,open,(char far *name,short options));
dynamic( short,create,(char far *name,short options));
dynamic( void,close,(short handle));
dynamic( short,read,(void far *buffer,short length,short handle));
dynamic( short,write,(void far *buffer,short length,short handle));
dynamic( short,strnicmp,(char far *s1,char far *s2,short length));
dynamic( short,stricmp,(char far *s1,char far *s2));
//dynamic( short,strcmp,(char far *s1,char far *s2));
dynamic( void,strcat,(char far *s1,char far *s2));
dynamic( void,strncpy,(char far *s1,char far *s2,short length));
dynamic(short,strlen,(char far *line));
dynamic(short,movmem,(void far *src,void far *dest,short length));
dynamic(short,memset,(void far *src,short value,short length));

#pragma warn .rvl

