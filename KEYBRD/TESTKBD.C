#define kbhit
#define getch
#define open
#define read
#define close
#include "mod.h"
#include "video.h"
windowptr output;

char short_buf[64];
packet transmit,far *receive;
char transmitted;
char key_buf[64];
char head=0,tail=0;
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
  asm mov dx,word ptr cs:old_key
  asm mov ds,word ptr cs:old_key+2
  asm mov ax,0x2509;
  asm int 21h
  asm pop ds;
}


void dump_key(short idx)
{
  short cnt,len,key;
  char column=7;
  char op,first,done,tlen,tchar,topt,ofirst,otopt=-1;
  char lindex;
  char text;
  char line[80];
  char quote;
    for (cnt=0;cnt<8;cnt++)
    {
      quote='\'';
      first=true;
      done=false;
      ofirst=true;
      tchar=tempkbd->keydata[idx++];
      if (otopt!=tchar||cnt==0)
      {
        displayln(output," (");
        column+=2;
        for (topt=0;topt<maxopts;topt++)
        {
          if (tchar&(1<<topt))
          {
            if (!ofirst) displayln(output,",");
            if ((column+8)>80)
            {
              displayln(output,"\\\r\n       ");
              column=7;
            }
            column+=8;
            displayln(output,"*s",(char far *)option[topt]);
            ofirst=false;
            if (topt==5)
            {
              skip=true;
            }
          }
        }
        displayln(output,")");
        column+=1;
      }
      otopt=tchar;
      do
      {
        switch(tempkbd->keydata[idx++])
        {
          case 0:displayln(output,"]");
                 done=true;
                 column++;
                 break;
          case 1:if (!first) displayln(output,","); else displayln(output,"[");
                 if ((column+8)>80)
                 {
                   displayln(output,"\\\r\n      ");
                   column=8;
                 }
                 displayln(output,"Shift");
                 column+=8;
                 break;
          case 2:if (!first) displayln(output,","); else displayln(output,"[");
                 if ((column+5)>80)
                 {
                   displayln(output,"\\\r\n      ");
                   column=7;
                 }
                 displayln(output,"Ctrl");
                 column+=5;
                 break;
          case 3:if (!first) displayln(output,","); else displayln(output,"[");
                 if ((column+4)>80)
                 {
                   displayln(output,"\\\r\n      ");
                   column=7;
                 }
                 displayln(output,"Alt");
                 column+=4;
                 break;
          case 4:if (!first) displayln(output,","); else displayln(output,"[");
                 if ((column+8)>80)
                 {
                   displayln(output,"\\\r\n      ");
                   column=7;
                 }
                 displayln(output,"Capslck");
                 column+=8;
                 break;
          case 5:if (!first) displayln(output,","); else displayln(output,"[");
                 if ((column+7)>80)
                 {
                   displayln(output,"\\\r\n      ");
                   column=7;
                 }
                 displayln(output,"Numlck");
                 column+=7;
                 break;
          case 6:if (!first) displayln(output,","); else displayln(output,"[");
                 if ((column+8)>80)
                 {
                   displayln(output,"\\\r\n      ");
                   column=7;
                 }
                 displayln(output,"Scrllck");
                 column+=8;
                 break;
          case 7:if (!first) displayln(output,","); else displayln(output,"[");
                 text=false;
                 tlen=tempkbd->keydata[idx++];
                 for (lindex=tlen;lindex;lindex--)
                 {
                   if (tempkbd->keydata[idx+lindex-1]=='\'')
                     quote='\"';
                 }
                 for (;tlen;tlen--)
                 {
                   tchar=tempkbd->keydata[idx++];
                   if (tchar>=32)
                   {
                     if (!text)  displayln(output,"*c",quote);
                     text=true;
                     displayln(output,"*c",tchar);
                   }
                   else
                   {
                     displayln(output,"#*n",tchar);
                   }

                 }
                 if (text) displayln(output,"*c",quote);
                 break;
          case 8:if (!first) displayln(output,","); else displayln(output,"[");
                 tlen=tempkbd->keydata[idx++];
                 for (;tlen;tlen--)
                   displayln(output,"*c",tempkbd->keydata[idx++]);
                 break;
          case 9:displayln(output,"[]");
                 column+=2;
                 done=true;
                 break;
        }
        first=false;
      }while (!done);
    }
    displayln(output,"\r\n");
  }

void process(short idx,char isbreak)
{
  short cnt,len,key;
  char column=7;
  char op,first,done,tlen,tchar,topt,ofirst,otopt=-1;
  char lindex;
  char text;
  char line[80];
  char quote;
    for (cnt=0;cnt<sft_status;cnt++)
    {
      done=false;
      idx++; /*skip options*/
      do
      {
        switch(tempkbd->keydata[idx++])  /*skip type*/
        {
          case 0:
          case 9:
                 done=true;
          case 1:
          case 2:
          case 3:
          case 4:
          case 5:
          case 6:
                 break;
          case 7:
          case 8:
                 tlen=tempkbd->keydata[idx++];     /*skip length*/
                 for (;tlen;tlen--)
                   idx++;
                 break;
        }
      }while (!done);
    }
    {
      quote='\'';
      first=true;
      done=false;
      ofirst=true;
      tchar=tempkbd->keydata[idx++];
      if ((otopt!=tchar||cnt==0)&&(!isbreak))
      {
        displayln(output," (");
        column+=2;
        for (topt=0;topt<maxopts;topt++)
        {
          if (tchar&(1<<topt))
          {
            if (!ofirst) displayln(output,",");
            if ((column+8)>80)
            {
              displayln(output,"\\\r\n       ");
              column=7;
            }
            column+=8;
            displayln(output,"*s",(char far *)option[topt]);
            if (topt==5)
              skip=true;
            ofirst=false;
          }
        }
        displayln(output,")");
        column+=1;
      }
      otopt=tchar;
      do
      {
        switch(tempkbd->keydata[idx++])
        {
          case 0:
                if (!isbreak)
                {
                   displayln(output,"]");
                   column++;
                }
                 done=true;
                 break;
          case 1:
              if (!isbreak)
              {
                 if (!first) displayln(output,","); else displayln(output,"[");
                 if ((column+8)>80)
                 {
                   displayln(output,"\\\r\n      ");
                   column=8;
                 }
                 displayln(output,"Shift");
                 column+=8;
              }
                 if (!isbreak)
                   sft_status|=1;
                 else
                   sft_status&=~1;
                 break;
          case 2:
              if (!isbreak)
              {
                 if (!first) displayln(output,","); else displayln(output,"[");
                 if ((column+5)>80)
                 {
                   displayln(output,"\\\r\n      ");
                   column=7;
                 }
                 displayln(output,"Ctrl");
                 column+=5;
              }
                 if (!isbreak)
                   sft_status|=2;
                 else
                   sft_status&=~2;
                 break;
          case 3:
              if (!isbreak)
              {
                 if (!first) displayln(output,","); else displayln(output,"[");
                 if ((column+4)>80)
                 {
                   displayln(output,"\\\r\n      ");
                   column=7;
                 }
                 displayln(output,"Alt");
                 column+=4;
              }
                 if (!isbreak)
                   sft_status|=4;
                 else
                   sft_status&=~4;
                 break;
          case 4:
              if (!isbreak)
              {
                 if (!first) displayln(output,","); else displayln(output,"[");
                 if ((column+8)>80)
                 {
                   displayln(output,"\\\r\n      ");
                   column=7;
                 }
                 displayln(output,"Capslck");
                 column+=8;
                 led_status^=1;
              }
                 break;
          case 5:
              if (!isbreak)
              {
                 if (!first) displayln(output,","); else displayln(output,"[");
                 if ((column+7)>80)
                 {
                   displayln(output,"\\\r\n      ");
                   column=7;
                 }
                 displayln(output,"Numlck");
                 column+=7;
                 led_status^=2;
              }
                 break;
          case 6:
              if (!isbreak)
              {
                 if (!first) displayln(output,","); else displayln(output,"[");
                 if ((column+8)>80)
                 {
                   displayln(output,"\\\r\n      ");
                   column=7;
                 }
                 displayln(output,"Scrllck");
                 column+=8;
                 led_status^=4;
              }
                 break;
          case 7:
              if (!isbreak)
              {
                 if (!first) displayln(output,","); else displayln(output,"[");
                 text=false;
                 tlen=tempkbd->keydata[idx++];
                 for (lindex=tlen;lindex;lindex--)
                 {
                   if (tempkbd->keydata[idx+lindex-1]=='\'')
                     quote='\"';
                 }
                 for (;tlen;tlen--)
                 {
                   tchar=tempkbd->keydata[idx++];
                   if (tchar>=32)
                   {
                     if (!text)  displayln(output,"*c",quote);
                     text=true;
                     displayln(output,"*c",tchar);
                   }
                   else
                   {
                     displayln(output,"#*n",tchar);
                   }

                 }
                 if (text) displayln(output,"*c",quote);
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
                 if (!first) displayln(output,","); else displayln(output,"[");
                 tlen=tempkbd->keydata[idx++];
                 for (;tlen;tlen--)
                   displayln(output,"*c",tempkbd->keydata[idx++]);
              }
              else
              {
                 tlen=tempkbd->keydata[idx++];     /*skip length*/
                 for (;tlen;tlen--)
                   idx++;
              }
                 break;
          case 9:
                 displayln(output,"[]");
                 column+=2;
                 done=true;
                 break;
        }
        first=false;
      }while (!done);
    }
    displayln(output,"\r\n");
  }

void dump_defs()
{
  short cnt,len,datalen,key,pkey;
  short idx;
  prefix_data far *tpretable;

  displayln(output,"KeyDefs\r\n\r\n");
  displayln(output,"!     These are the \'Normal\' Keys.\r\n");
  pkey=0;
  for (key=0;key<tempkbd->maxcode;key++)
  {
    idx=tempkbd->table[key];
    if (idx==-1)
    {
      displayln(output,"!   NO KEY\r\n");
      continue;
    }
    displayln(output,"Key*n",pkey++);
    dump_key(idx);
  }
  tpretable=tempkbd->prefix_table;
  while (tpretable->next)
  {

    displayln(output,"\r\n!  These are for Prefix #*n \r\n",tpretable->prefix);
    for (key=0;key<tpretable->length;key++)
    {
      idx=tpretable->table[key];
      if (idx==-1)
      {
        displayln(output,"!   NO KEY\r\n");
        continue;
      }
      displayln(output,"Key*n",pkey++);
      dump_key(idx);
    }
    tpretable=tpretable->next;
  }

}

void dump_scans()
{
  short cnt,len,datalen,key,pkey;
  short idx;
  prefix_data far *tpretable;
  displayln(output,"ScanDefs\r\n\r\n");
  displayln(output,"!     These are the \'Normal\' Keys.\r\n");
  pkey=0;
  for (key=0;key<tempkbd->maxcode;key++)
  {
    idx=tempkbd->table[key];
    if (idx==-1)
    {
      displayln(output,"!   NO KEY\r\n");
      continue;
    }
    displayln(output,"Key*n      [#*n]\r\n",pkey++,key);
  }
  tpretable=tempkbd->prefix_table;
  while (tpretable->next)
  {

    displayln(output,"\r\nPrefix [#*n] \r\n",tpretable->prefix);
    for (key=0;key<tpretable->length;key++)
    {
      idx=tpretable->table[key];
      if (idx==-1)
      {
        displayln(output,"!   NO KEY\r\n");
        continue;
      }
      displayln(output,"Key*n   [#*n]\r\n",pkey++,tpretable->scancodes[key]);
    }
    tpretable=tpretable->next;
  }

}
void read_kbd_data()
{
  short temp;
  short datalen;
  prefix_data far *tpretable;
  char len;
/*  tempkbd=(kbd far *)Get_environ("KEYBOARD");*/

  temp=open("config.i",0);
  if (temp==-1)
  {
    displayln(output,"Error opening Configuration file config.i.");
    exit(0);
  }
  read(&datalen,2,temp);
  tempkbd->keydata=Allocate(datalen);

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

void main()
{
  char ch=0;
  _disconnect_kbd();
  connect_kbd();
  output=opendisplay(1,1,50,15,BORDER,0x1f,0x1f,0x2f,"Keyboard");
  read_kbd_data();
/*  dump_defs();*/
/*  dump_scans();*/
  transmit.sd.buf=short_buf;
  transmit.short_pkt=true;
  transmitted=false;

  while (ch!=1)
  {
/*    if (transmit.sd.status)
      transmitted=false;
    if (kbhit())
    {
      c=getch();
      if (c==0)
      {
        c=getch();
        switch(c)
        {
          case 'D':Exit(0L);
        }
      }
      else
      {
        key_buf[head]=c;
        head++;
        if (head==64) head=0;
        if (head==tail)
        {
          head--;
          if (head<0)
            head=63;
        }
      }
      if ((!transmitted)&&(tail!=head))
      {
        char cnt=0;
        while (head!=tail)
        {
          short_buf[cnt]=key_buf[tail];
          tail++;
          if (tail==64) tail=0;
          cnt++;
        }
        transmit.sd.status=0;
        transmit.sd.length=cnt;
        transmit.sd.opcode=0;
        Export("display",&transmit);
        transmitted=true;
      }
    }
    Relinquish(0L);*/

    if (schead!=sctail)
    {
      ch=kbdch[sctail++];
      if (sctail==50) sctail=0;
/*      displayln(output,"*n ",ch);*/
      if ((ch&0x7f)<tempkbd->maxcode)
      {
        short idx;
        if (ch&0x80)
        {
          displayln(output,"Break Code!\r\n");
        }
        {
          if (skip)
          {
            skip=false;
            continue;
          }
          idx=tempkbd->table[ch&0x7f];
          if (idx==-1)
          {
            displayln(output,"No Definition\n\r");
            beep(1);
          }
          else
            process(idx,ch&0x80);
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
            if (ch&0x80)
              displayln(output,"Prefix Break Code!\r\n");
            {
              for (idx=0;idx<temp->length;idx++)
              {
                if (temp->scancodes[idx]==(ch&0x7f))
                {
                  if (temp->table[idx]==-1)
                  {
                    displayln(output,"No Definition\n\r");
                    beep(1);
                  }
                  else
                    process(temp->table[idx],ch&0x80);
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
          displayln(output,"NO PREFIX!\r\n");
        }
        beep(2);
        /*find prefix*/
      }
    }
  }
  Exit(1);
}
