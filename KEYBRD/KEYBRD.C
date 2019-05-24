#include "mod.h"
#include "comm.h"
#include "video.h"
window_type far *output=0;
module far *keytask;
/*#define DEBUG*/

char far *current_path;
dynamic(short,getdisplay,(short far *x,short far *y,windowptr far *window));
dynamic(short,moddisplay,(windowptr window,...));

#define KEYPOWER 7
#define KEYBUFSIZE (1<<KEYPOWER)

char key_buf[KEYBUFSIZE];
char keyhead=0,keytail=0;
char c;
char skip;

extern char far *old_key;
char led_status=0,sft_status=0;

extern void keyintr(void);
extern char schead,sctail,kbdch[];

#define true !false
#define false 0

/*FILE *temp;*/

typedef struct prefix_data
{
  unsigned char prefix;
  char length;
  char far *scancodes;
  short far *table;
  struct prefix_data far *next;
} prefix_data;

typedef struct kbd
{
  char far *keydata;  /* The data that describes all the keys */
  short far *table;   /* table of indexes into the data table for scancodes
                     below max*/
  unsigned char maxcode;   /* highest 'normal' scancode */

  prefix_data far *prefix_table;
}kbd;

#define maxopts 6
char *option[maxopts]={"Toggle","Caps","Num","LED","Scroll","Skip"};
kbd far *tempkbd;


void connect_kbd(void)
{
  long temp_ptr;
  schead=sctail;
  asm mov ax,0x3509;
  asm int 21h
  asm mov word ptr old_key,bx;
  asm mov word ptr old_key+2,es;
  asm mov ax,0x2509;
  temp_ptr=(long)&keyintr;
  asm push ds
  asm mov dx,word ptr temp_ptr
  asm mov ds,word ptr temp_ptr+2
  asm int 21h
  asm pop ds;
}

cleanup(void,disconnect_kbd,(void))
{
  asm push ds;
  asm mov ax,seg old_key
  asm mov ds,ax
  asm mov dx,word ptr old_key
  asm mov ds,word ptr old_key+2
  asm mov ax,0x2509;
  asm int 21h
  asm pop ds;
}



void call(char far *line)
{
  asm mov es,word ptr line+2
  asm mov dx,word ptr line
  asm mov ah,7
  asm int 0x60
}

#define skip_keys  for (cnt=0;cnt<temp_shift;cnt++) /*skip over character till we get\
                                                      to the correct state*/         \
    {                                                                                \
      done=false;                                                                    \
      idx++;                                                                         \
      do                                                                             \
      {                                                                              \
        switch(tempkbd->keydata[idx++])  /*get and skip type*/                       \
        {                                                                            \
          case 0:                                                                    \
          case 9:                                                                    \
                 done=true;                                                          \
          case 1:                                                                    \
          case 2:                                                                    \
          case 3:                                                                    \
          case 4:                                                                    \
          case 5:                                                                    \
          case 6:                                                                    \
                 break;                                                              \
          case 7:                                                                    \
          case 8:                                                                    \
                 tlen=tempkbd->keydata[idx++];     /*skip length*/                   \
                 for (;tlen;tlen--)                                                  \
                   idx++;                                                            \
                 break;                                                              \
          default:                                                                   \
/*                  displayln(output,"invalid opcode\n");                          */\
                  Exit(1);                                                           \
        }                                                                            \
      }while (!done);                                                                \
    }

char *functions[]={"windowleft",
                   "windowright",
                   "windowup",
                   "windowdown",
                   "resizeleft",
                   "resizeright",
                   "resizeup",
                   "resizedown",
                   "windowicon",
                   "windowselect",
                   "mouseleft",
                   "mouseright",
                   "mouseup",
                   "mousedown",
                   "scrollleft",
                   "scrollright",
                   "scrollup",
                   "scrolldown"};

#define NUM_LOCAL (sizeof(functions)/sizeof(char *))

void do_local(short function)
{
  windowptr twindow;
  short x=-1,y=-1;
  switch(function)
  {
    case 0:
      getdisplay(&x,&y,&twindow);
      moddisplay(twindow,DELTA_MOVE_W(-1,0),END_MOD);
      break;
    case 1:
      getdisplay(&x,&y,&twindow);
      moddisplay(twindow,DELTA_MOVE_W(1,0),END_MOD);
      break;
    case 2:
      getdisplay(&x,&y,&twindow);
      moddisplay(twindow,DELTA_MOVE_W(0,-1),END_MOD);
      break;
    case 3:
      getdisplay(&x,&y,&twindow);
      moddisplay(twindow,DELTA_MOVE_W(0,1),END_MOD);
      break;
    case 4:
      getdisplay(&x,&y,&twindow);
      moddisplay(twindow,RESIZEX(-1),END_MOD);
      break;
    case 5:
      getdisplay(&x,&y,&twindow);
      moddisplay(twindow,RESIZEX(1),END_MOD);
      break;
    case 6:
      getdisplay(&x,&y,&twindow);
      moddisplay(twindow,RESIZEY(-1),END_MOD);
      break;
    case 7:
      getdisplay(&x,&y,&twindow);
      moddisplay(twindow,RESIZEY(1),END_MOD);
      break;
    case 8:
      getdisplay(&x,&y,&twindow);
      if (twindow)
        if (twindow->opts&ICON)
          moddisplay(twindow,RESTORE_WINDOW,END_MOD);
        else
          moddisplay(twindow,ICON_WINDOW,END_MOD);
      break;
    case 9:
      getdisplay(&x,&y,&twindow);
      getdisplay(&x,&y,&twindow);
      moddisplay(twindow,SELECT_WINDOW,END_MOD);
      break;
    case 10:
      getdisplay(&x,&y,&twindow);
      moddisplay(twindow,DELTA_MOVE_M(-1,0),END_MOD);
      break;
    case 11:
      getdisplay(&x,&y,&twindow);
      moddisplay(twindow,DELTA_MOVE_M(1,0),END_MOD);
      break;
    case 12:
      getdisplay(&x,&y,&twindow);
      moddisplay(twindow,DELTA_MOVE_M(0,-1),END_MOD);
      break;
    case 13:
      getdisplay(&x,&y,&twindow);
      moddisplay(twindow,DELTA_MOVE_M(0,1),END_MOD);
      break;
    case 14:
      getdisplay(&x,&y,&twindow);
      moddisplay(twindow,SCROLL_LEFT,END_MOD);
      break;
    case 15:
      getdisplay(&x,&y,&twindow);
      moddisplay(twindow,SCROLL_RIGHT,END_MOD);
      break;
    case 16:
      getdisplay(&x,&y,&twindow);
      moddisplay(twindow,SCROLL_UP,END_MOD);
      break;
    case 17:
      getdisplay(&x,&y,&twindow);
      moddisplay(twindow,SCROLL_DOWN,END_MOD);
      break;
  }
}


void process(short idx,char isbreak)
{
  short key,save_idx=idx;
  char op,done,tlen,topt,cnt;
  char temp_shift=sft_status;
  char find_actual;
  char line[80],place;
#ifdef DEBUG
/*    displayln(output,"kbd *H",(long)tempkbd);
    asm int 3;*/
#endif
    skip_keys;
    topt=tempkbd->keydata[idx++];  /*get options for found key*/
    if ((topt&2)&&led_status&1)  /*if key has option of caps, and
                                   caps lock is pressed, then toggle
                                   the actual shift bit*/
    {
      temp_shift^=1;
      find_actual=true;          /*if status is changed, we need actual
                                    key becaseu we found the wrong one*/
    }
    if ((topt&4)&&led_status&2)
    {
      temp_shift^=1;             /*if key has option of num, and
                                   num lock is pressed, then toggle
                                   the actual shift bit*/
      find_actual=true;          /*if status is changed, we need actual
                                    key becaseu we found the wrong one*/
    }
    if (find_actual)
    {
      idx=save_idx;  /*go back to start of key defs*/
      skip_keys;
      idx++;  /*skip options for the actually found key*/
    }
    {
      done=false;
      do
      {
        switch(tempkbd->keydata[idx++])
        {
          case 0:/*this is the terminator following a string of commands*/
          case 9:/*This is the terminator if there were NO commands*/
                 done=true;
                 break;
          case 1:
                 if (!isbreak)
                   sft_status|=1;
                 else
                   sft_status&=~1;
#ifdef DEBUG
                 displayln(output,"Shift: *b\n",sft_status);
#endif
                 break;
          case 2:
                 if (!isbreak)
                   sft_status|=2;
                 else
                   sft_status&=~2;
#ifdef DEBUG
                 displayln(output,"Shift: *b\n",sft_status);
#endif
                 break;
          case 3:
                 if (!isbreak)
                   sft_status|=4;
                 else
                   sft_status&=~4;
#ifdef DEBUG
                 displayln(output,"Shift: *b\n",sft_status);
#endif
                 break;
          case 4:
                 if (!isbreak)
                   led_status^=1;
#ifdef DEBUG
                 displayln(output,"LED: *b\n",led_status);
#endif
                 break;
          case 5:
                 if (!isbreak)
                   led_status^=2;
#ifdef DEBUG
                 displayln(output,"LED: *b\n",led_status);
#endif
                 break;
          case 6:
                 if (!isbreak)
                   led_status^=4;
#ifdef DEBUG
                 displayln(output,"LED: *b\n",led_status);
#endif
                 break;
          case 7:
              if (!isbreak)
              {
                 tlen=tempkbd->keydata[idx++];
                 for (;tlen;tlen--)
                 {
                   /*Store Character(s) in key buffer*/
                   key_buf[keyhead]=tempkbd->keydata[idx++];
                   keyhead++;
                   keyhead&=(KEYBUFSIZE-1);
                   if (keyhead==keytail)
                   {
                     keyhead--;
                     keyhead&=(KEYBUFSIZE-1);
                   }
                 }
              }
              else
              {
                 tlen=tempkbd->keydata[idx++];     /*skip length*/
                 for (;tlen;tlen--)
                   idx++;
              }
              break;
          case 8:
              if (!isbreak)
              {
                 short i;
                 for (place=0,tlen=tempkbd->keydata[idx++];
                      tlen;
                      tlen--,place++)
                   line[place]=tempkbd->keydata[idx++];
                 line[place]=0;
                 for (i=0;i<NUM_LOCAL;i++)
                 {
                   if (stricmp(line,(char far *)functions[i])==0)
                   {
                     do_local(i);
                     break;
                   }
                 }
                 if (i<NUM_LOCAL)
                   break;  /*exit switch*/
                 if (Locate(line))
                 {
#ifdef DEBUG
                   displayln(output,"routine");
#endif
                   call(line);

#ifdef DEBUG
                   displayln(output," found\n");
#endif
                 }
                 else
                 {
#ifdef DEBUG
                   displayln(output,"Function *s not found\n",(char far *)line);
#endif
                 }
              }
              else
              {
                 tlen=tempkbd->keydata[idx++];     /*skip length*/
                 for (;tlen;tlen--)
                   idx++;
              }
                 break;
          default:
#ifdef DEBUG
                  displayln(output,"invalid opcode\n");
#endif
                  Exit(1);
        }
      }while (!done);
    }
  }

void read_kbd_data()
{
  short temp;
  short datalen;
  char fname[80];
  prefix_data far *tpretable;
  char len;
/*  tempkbd=(kbd far *)Get_environ("KEYBOARD");*/
  tempkbd=Allocate(sizeof(kbd));
  strncpy(fname,current_path,80);
  strcat(fname,"config.i");
  temp=open(fname,0);
  if (temp==-1)
  {
#ifdef DEBUG
    displayln(output,"Error opening Configuration file config.i.");
#endif
    perish();
  }
  read(&datalen,2,temp);
  tempkbd->keydata=Allocate(datalen);
  if (!tempkbd->keydata)
  {
#ifdef DEBUG
    displayln(output,"Error allocating a buffer *d bytes long.",datalen);
#endif
    perish();
  }
  read(&tempkbd->maxcode,1,temp);

  tempkbd->table=Allocate(tempkbd->maxcode*2);

  read(tempkbd->table,tempkbd->maxcode*2,temp);

  tpretable=tempkbd->prefix_table=Allocate(sizeof(prefix_data));
  while (read(&len,1,temp)&&len)
  {
    read(&tpretable->prefix,1,temp);
    tpretable->length=len;
    tpretable->scancodes=Allocate(len);
    read(tpretable->scancodes,len,temp);
    tpretable->table=Allocate(len*2);
    read(tpretable->table,len*2,temp);
    tpretable->next=Allocate(sizeof(prefix_data));
    tpretable=tpretable->next;
  }
  tpretable->length=len;
  tpretable->next=0L;
  read(tempkbd->keydata,datalen,temp);
  close(temp);
}

public(char,rawkeypressed,(void))
{
  Load_DS;
  if (keyhead!=keytail)
  {
    asm pop ds;
    return(true);
  }
  else
  {
    asm pop ds;
    return(false);
  }
}

public(char,rawreadch,(void))
{
  char temp;
  Load_DS;
  while (keyhead==keytail)
  {
    Relinquish(0L);
  }
  temp=key_buf[keytail];
  keytail++;
  keytail&=(KEYBUFSIZE-1);
  asm pop ds;
  return(temp);
}

void main(char argc,char far *path)
{
  char ch=0,cnt,len;
  char temp;
  char far *line;
  current_path=path;
  read_kbd_data();
  _disconnect_kbd();
  connect_kbd();
#ifdef DEBUG
  output=opendisplay(30,2,20,15,BORDER|NEWLINE,0x1f,0x1f,0x2f,"Keyboard");
  moddisplay(output,ICON_WINDOW,END_MOD);
#endif
  _rawkeypressed();
  _rawreadch();
  keytask=my_TCB;
  while (1)
  {
    if (schead!=sctail)
    {
      ch=kbdch[sctail++];
      if (sctail==50) sctail=0;
      if ((ch&0x7f)<tempkbd->maxcode)
      {
        short idx;
        {
          if (skip)
          {
            skip=false;
            continue;
          }
          idx=tempkbd->table[ch&0x7f];
          if (idx==-1)
          {
#ifdef DEBUG
            displayln(output,"No Definition\n");
#endif
            beep(1);
          }
          else
          {
            process(idx,ch&0x80);
          }
        }
      }
      else
      {
        prefix_data far *temp=tempkbd->prefix_table;
        while (temp)
        {
          if (temp->prefix==ch)
          {
            char idx;
            while (sctail==schead) Relinquish(0L);
            ch=kbdch[sctail++];
            if (sctail==50) sctail=0;
            {
              for (idx=0;idx<temp->length;idx++)
              {
                if (temp->scancodes[idx]==(ch&0x7f))
                {
                  if (temp->table[idx]==-1)
                  {
#ifdef DEBUG
                    displayln(output,"No Definition\n");
#endif
                    beep(1);
                  }
                  else
                  {
                    process(temp->table[idx],ch&0x80);
                  }
                  break;
                }
              }
            }
            break;
          }
          temp=temp->next;
        }
        if (!temp)
        {
#ifdef DEBUG
          displayln(output,"NO PREFIX!\n");
#endif
        }
/*        beep(2);*/
      }
    }
    if (schead==sctail)
      Relinquish(-10000L);
    else
      Relinquish(0L);
  }
}
