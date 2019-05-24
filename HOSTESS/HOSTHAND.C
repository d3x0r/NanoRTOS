#define strncpy
#define movmem
#define strncmp
#include "mod.h"
#include "npca.h"
#include "multthrd.h"
#include "hostess.h"
#include "ipchost.h"

#define DEBUG
#ifdef DEBUG
#include "video.h"
windowptr output,nodestuff;
#endif

#define write(buffer,count,unit) {hiwrite(buffer,count,unit)}
#define readascii(charac,unit) { hiread(&charac,1,unit);}
#define readbin(buffer,count,unit) {hiread(buffer,count,unit);}
#define openchan hiopen
#define closechan hiclose
#define remoteabt hiterm
#define ECHO(node) (!(                                    \
                        (node->Node.XOptions&UFTXOPNEC)|  \
                      (!(node->Node.Info.Comm.Control&4)) \
                     )                                    \
                   )


/* we need to convert everything to
     1> close
     2> open
     3> getch
     4> getblk
     5> flush.
  To these functions, we have to pass a pointer to the thing to operate on.
  Then this becomes an object.  We also need to define two structures
  that can be passed to the common open routine in order that we may give
  the appropriate parameters to the open.  Types of things that the first
  cut will handle... ethernet, Hostess.  */

line_table lines[16];
card_entry far *card_data[16];
break_info breaks[16];
char curline=0;
short trickle_delay=100;

char linefeeds[21]={'\r','\n','\n','\n','\n','\n','\n','\n','\n','\n','\n'
                        ,'\n','\n','\n','\n','\n','\n','\n','\n','\n','\n'};

#define TRMGETC    0
#define TRMWRITE   2

#define intswap(a) (((unsigned)a>>8)|((unsigned)a<<8))

#define false 0
#define true (!false)

/*this is the device driver front end to interfact the PCA driver with
  the ether DLL. Buffering is done completely by the ether driver.  The
  only real problem I have to worry about is packet integrety for CNI.*/


void terminate_watch(Node far *what,char far *operation
          /*add the parameter so that the work can be found when
            it is in a foriegn body */)
{
  threads_active far *temp=first_thread;
  char cnt=0;
            /*locate our thread entry*/

/*the following was commented out becuase we don't really need to do it.
  We have for the most part found that we should be running at the point
  that this thread would be started  6/18/93*/
  while (temp)
  {
    if (temp->id==what->Node.Channel)
      break;
    temp=temp->next;
  }
  if (!temp)
  {
    asm int 3;
    Exit(50);     /*we have NO right to be running */
  }
  while (1)
  {
    if (!what->Node.Node_Address)
    {
      /*insert specfic code to destroy the outstanding read */
      remoteabt(what->Node.Channel>>1,*operation);
      Relinquish(16L);
    }
    if (!(cnt&0xf))
      check_outstanding(temp);
    if (cnt==1)              /*if we have looped once and the operation*/
    {
      Slough_work(what);     /*is still active, then slough it*/
    }
    Relinquish(0L);
    cnt++;
  }
}

abort_node(Node far *what,short can_code)
{
  what->Return.Status=UFTSTAER|UFTSTAINO|UFTSTAOTH;
  what->Return.Byte_count=can_code;
  if (what->Data)
  {
    Free(what->Data);
    what->Data=0;
  }
  what->Return.XStatus=0;
  what->Return.Opcode=COMPLETE;
  what->Tracking.Status|=WORK_IN_MAIL|WORK_ACK;
  Export(what->Tracking.Source,what);
}

void write_formfeed(Node far *what,short line)
{
   if (what->Node.Info.Comm.Control&0x8)
     hiwrite("\f",1,line);
   else
     if (what->Node.Info.Comm.Top_of_form)
     {
       unsigned char i,total,remainder;
       total=what->Node.Info.Comm.Top_of_form-lines[line].CLP;
       remainder=total%20;
       total=total/20;
       for (i=0;i<total;i++)
         hiwrite(linefeeds,21,line);
       if (what->Node.Node_Address)
         hiwrite(linefeeds,remainder+1,line);
     }
     else
       hiwrite(linefeeds,5,line);
     lines[line].CLP=0;
}

flushline(char line)
{
  card_data[line]->Rxq_tail=card_data[line]->Rxq_head;
}

void process_rex(node far *what)
{
  if (what->Node.Node_Address)
  {
    data_buffer=(unsigned char far *)what->Data;
    switch(what->Node.Rex&0xf)
    {
      case READ:/*read*/
             if (what->Tracking.Side==EVEN)
             {
               Slough_work(what);
              }
/*At this point we could have a terminated node on our hands */

             if (what->Node.XOptions&UFTXOPFLU)
             {
              /*flush*/
               flushline(line);
             }
             opptr=&extrnop;
             if (fork(CHILD))
             {
               terminate_watch(what,opptr);
             }
             else
             {
              unsigned char data_read;
              unsigned short bufferidx=0;
              if (what->Node.Options&UFTOPTBI)
              {
                card_data[line]->Flow_control&=~(Respect_soft);
                if (!(what->Node.Options&UFTOPTDDO))
                {
                 /*read into the buffer until a non null character is
                   read.*/
                  do
                  {
                    extrnop=TRMGETC;
                    if (readascii(line,&data_read)&&ECHO(what))
                    {
                      extrnop=TRMWRITE;
                      hiwrite(&data_read,1,line);
                    }
                    if (!what->Node.Node_Address) goto return_now;
                  }
                  while (!data_read);
                  data_buffer[bufferidx++]=data_read;
                }
                if (what->Node.Options&UFTOPTSTD)
                { /*standard binary*/
                  if (!(what->Node.Options&UFTOPTTBX))
                  {
                    while (bufferidx<2)
                    {
                      extrnop=TRMGETC;
                      if (readascii(line,data_buffer+bufferidx++)&&ECHO(what))
                      {
                        extrnop=TRMWRITE;
//                        hiwrite(data_buffer+bufferidx,1,line);
                      }
                      if (!what->Node.Node_Address) goto return_now;
                    }
                    if (*((short far *)data_buffer)==0x2424)
                    {
                      what->Return.Status=UFTSTAEOF;
                      break;
                    }
                  }
                  if (!bufferidx)
                  {
                    extrnop=TRMGETC;
                    if (readascii(line,data_buffer+bufferidx++)&&ECHO(what))
                    {
                      extrnop=TRMWRITE;
//                      hiwrite(data_buffer+bufferidx,1,line);
                    }
                    if (!what->Node.Node_Address) goto return_now;
                  }
                  if (data_buffer[0]==3||data_buffer[0]==7)
                  {
                    short block_size;
standard_binary:
                    while (bufferidx<4)
                    {
                      extrnop=TRMGETC;
                      if (readascii(line,data_buffer+bufferidx++)&&ECHO(what))
                      {
                        extrnop=TRMWRITE;
//                        hiwrite(data_buffer+bufferidx,1,line);
                      }
                      if (!what->Node.Node_Address) goto return_now;
                    }
                    block_size=intswap(*(short far*)(data_buffer+2));
                    while (bufferidx<block_size&&
                           what->Node.Node_Address)
                    {
                      short tsize;
                      bufferidx+=(tsize=readbin(data_buffer+bufferidx,
                             block_size-bufferidx,
                             line));
                      extrnop=TRMWRITE;
                      if (tsize&&ECHO(what))
                      {
                        hiwrite(data_buffer+bufferidx-tsize,
                                tsize,
                                line);
                      }

                      if (bufferidx<block_size)
                        Relinquish(0L);
                    }
                    if (!what->Node.Node_Address) goto return_now;
                  }
                }
                else
                {  /*non standard binary*/
                  while (bufferidx<what->Node.Byte_count&&
                         what->Node.Node_Address)
                  {
                    short tsize;
                    bufferidx+= (tsize=readbin(data_buffer+bufferidx,
                                       what->Node.Byte_count-bufferidx,
                                       line));
                    extrnop=TRMWRITE;
                    if (tsize&&ECHO(what))
                    {
                      hiwrite(data_buffer+bufferidx-tsize,
                              tsize,
                              line);
                    }

                    if (bufferidx<what->Node.Byte_count)
                      Relinquish(0L);
                  }

                  if (!what->Node.Node_Address) goto return_now;
                }
              }
              else
              {
                char tchar,gotone=false;
                card_data[line]->Flow_control|=(Respect_soft);
                if (!(what->Node.Options&UFTOPTDDO))
                {
                  extrnop=TRMWRITE;
                  hiwrite("\r\n",2,line);
                  lines[line].CLP++;
                  if (!what->Node.Node_Address) goto return_now;
                }
                else
                {
                  do
                  {
                    extrnop=TRMGETC;
                    if (readascii(line,&tchar)&&ECHO(what))
                    {
                      extrnop=TRMWRITE;
                      hiwrite(&tchar,1,line);
                    }
                    if (!what->Node.Node_Address) goto return_now;
                  }
                  while (!tchar);
                  gotone=true;
                }
                if (what->Node.Options&UFTOPTSTD)
                { /*standard ascii*/
                  char done=false;
                  while (!done&&bufferidx<what->Node.Byte_count)
                  {
                    if (bufferidx)
                    {
                      if (data_buffer[0]==3||data_buffer[0]==7)
                        goto standard_binary;
                    }
                    if (!gotone)
                    {
                      extrnop=TRMGETC;
                      if (readascii(line,&tchar)&&ECHO(what))
                      {
                        extrnop=TRMWRITE;
                        hiwrite(&tchar,1,line);
                      }
                      if (!what->Node.Node_Address) goto return_now;
                    }
                    else
                      gotone=false;
                    switch(tchar)
                    {
                      case '\r':
                      case 0   :
                                if (!(what->Node.Options&UFTOPTTBX)&&bufferidx)
                                {
                                  bufferidx--;
                                  while (data_buffer[bufferidx]==' ')
                                    bufferidx--;
                                  bufferidx++;
                                }
                                if ((bufferidx&1)&&
                                    (bufferidx+1<what->Node.Byte_count))
                                {
                                  data_buffer[bufferidx++]=' ';
                                  what->Return.XStatus=-1;
                                }
                                if (!(what->Node.Options&UFTOPTTBX)&&
                                    (bufferidx+2<what->Node.Byte_count))
                                {
                                  data_buffer[bufferidx++]=0;
                                  data_buffer[bufferidx++]=0;
                                  what->Return.XStatus-=2;
                                }
                                what->Return.XStatus&=0x1ff;
                                done=true;
                      case '\n':break;
                      case '\b':if (bufferidx)
                                  bufferidx--;
                                break;
                      case 127:bufferidx=0;
                               extrnop=TRMWRITE;
                               hiwrite("\r\n",2,line);
                               lines[line].CLP++;
                               if (!what->Node.Node_Address) goto return_now;
                               break;
                      default:data_buffer[bufferidx++]=tchar;
                    }
                  }
                  if ((bufferidx>2)&&!(what->Node.Options&UFTOPTTBX))
                    if (*(short far *)data_buffer==0x2424)
                      what->Return.Status|=UFTSTAEOF;
                }
                else
                { /*non standard ascii */
                  while (bufferidx<what->Node.Byte_count)
                  {
                    if (!gotone)
                    {
                      extrnop=TRMGETC;
                      if (readascii(line,&tchar)&&ECHO(what))
                      {
                        extrnop=TRMWRITE;
                        hiwrite(&tchar,1,line);
                      }
                      if (!what->Node.Node_Address) goto return_now;
                    }
                    else
                      gotone=false;
                    data_buffer[bufferidx++]=tchar;
                    if (!(what->Node.Options&UFTOPTNT))
                    {
                      if (tchar==(what->Node.Options&UFTOPTTRM))
                      {
                        what->Return.XStatus=(-1)&0x1ff;
                        break;
                      }
                    }
                  }
                }
                what->Return.Byte_count=bufferidx;
              }
             }
return_now:
             destory();
             while (what->Tracking.Status&(WORK_IN_MAIL|WORK_IN_PROGRESS))
               Relinquish(0L);

             what->Tracking.Side=ODD;
             what->Node.FPI++;
             what->Return.Opcode=COMPLETE;
             break;
      case WRITE:
            {
              unsigned short count_sent=1;
             extrnop=TRMWRITE;
             if (fork(CHILD))
             {
               terminate_watch(what,&extrnop);
             }
             else
             {
              if (what->Node.Options&UFTOPTBI)
              {
                card_data[line]->Flow_control&=~(Command_soft);
                if (what->Node.Options&UFTOPTSTD)
                {
                  if (data_buffer[0]==3||data_buffer[0]==7)
                  {
                    unsigned short output_count=*((short *)(data_buffer+2));
                    if (output_count>what->Node.Byte_count)
                      output_count=what->Node.Byte_count;
                    hiwrite(data_buffer,output_count,line);
                    if (!what->Node.Node_Address) goto return_now;
                    what->Return.Byte_count=output_count;
                  }
                  else
                  {
                    what->Return.Status=UFTSTAER|UFTSTASBV;
                    what->Return.Byte_count=0;
                  }
                }
                else
                {
                  hiwrite(data_buffer,
                          (short)what->Node.Byte_count,
                          line);
                  if (!what->Node.Node_Address) goto return_now;
                  what->Return.Byte_count=what->Node.Byte_count;
                }
              }
              else
              {
                unsigned short idx,bias=0;
                card_data[line]->Flow_control|=(Command_soft);
                if (what->Node.Options&UFTOPTSTD)
                {
                  switch(data_buffer[0])
                  {
                    case '0':
                             hiwrite("\r\n\n",3,line);
                             lines[line].CLP+=2;
                             if (!what->Node.Node_Address) goto return_now;
                             break;
                    case '1':
                    case '-':
                             write_formfeed(what,line);
                             if (!what->Node.Node_Address) goto return_now;
                             break;
                    case '+':
                             hiwrite("\r",1,line);
                             if (!what->Node.Node_Address) goto return_now;
                             break;
                    default:
                             hiwrite("\r\n",2,line);
                             lines[line].CLP++;
                             if (!what->Node.Node_Address) goto return_now;
                             break;
                  }
                  data_buffer++;
                  bias++;
                  what->Node.Byte_count--;
                  if (data_buffer[1]==2)
                  {
                    data_buffer++;
                    bias++;
                    what->Node.Byte_count--;
                  }
                  for (idx=0;
                       (idx<what->Node.Byte_count)&&data_buffer[idx];
                       idx++);
                }
                else
                {
                  unsigned short terminator;

                  if (what->Node.Options&UFTOPTNT)
                    terminator=256;
                  else
                    terminator=what->Node.Options&UFTOPTTRM;

                  for (idx=0;
                      (idx<what->Node.Byte_count)&&
                       data_buffer[idx]!=terminator;
                       idx++);
                }
                hiwrite(data_buffer,idx,line);
                if (!what->Node.Node_Address) goto return_now;
                what->Return.Byte_count=idx+bias;
              }
              destory();
             }
             while (what->Tracking.Status&(WORK_IN_MAIL|WORK_IN_PROGRESS))
               Relinquish(0L);
             what->Return.Opcode=COMPLETE;
             what->Return.Byte_count=count_sent;
             what->Node.FPI++;
            }
            break;
      case REWIND:
             write_formfeed(what,line);
             while (what->Tracking.Status&(WORK_IN_MAIL|WORK_IN_PROGRESS))
               Relinquish(0L);
             what->Return.Opcode=COMPLETE;
             what->Return.Byte_count=0;
             what->Return.Status=UFTSTAEOF|UFTSTABOM;
             what->Node.FPI=0;
             break;
      case BACK_RECORD:
             while (what->Tracking.Status&(WORK_IN_MAIL|WORK_IN_PROGRESS))
               Relinquish(0L);
             what->Return.Opcode=COMPLETE;
             what->Return.Byte_count=0;
             what->Return.Status=UFTSTAEOF|UFTSTABOM;

             break;
      case BACK_FILE:
             while (what->Tracking.Status&(WORK_IN_MAIL|WORK_IN_PROGRESS))
               Relinquish(0L);
             what->Return.Opcode=COMPLETE;
             what->Return.Byte_count=0;
             what->Return.Status=UFTSTAEOF|UFTSTABOM;
             break;
      case ADVANCE_FILE:
             write_formfeed(what,line);
             while (what->Tracking.Status&(WORK_IN_MAIL|WORK_IN_PROGRESS))
               Relinquish(0L);
             what->Return.Opcode=COMPLETE;
             what->Return.Byte_count=0;
             what->Return.Status=UFTSTAEOF;
             what->Node.FPI++;
             break;
      case ADVANCE_RECORD:
             hiwrite("\r\n",2,line);
             lines[line].CLP++;
             while (what->Tracking.Status&(WORK_IN_MAIL|WORK_IN_PROGRESS))
               Relinquish(0L);
             what->Return.Opcode=COMPLETE;
             what->Return.Byte_count=0;
             what->Node.FPI++;
             break;
      case WRITE_EOF:
             hiwrite("\r\n\n$$",5,line);
             lines[line].CLP=0;
             while (what->Tracking.Status&(WORK_IN_MAIL|WORK_IN_PROGRESS))
               Relinquish(0L);
             what->Return.Opcode=COMPLETE;
             what->Return.Byte_count=0;
             what->Node.FPI++;
             break;
      case HOME:
             if (what->Node.XOptions&UFTXOPDCL)
             {
               closechan(line);
               lines[line].open=false;
             }
             what->Return.Byte_count=0;
             what->Node.FPI++;
             what->Return.Opcode=COMPLETE;
             break;
    }
    if (!lines[line].CLP&&!(what->Return.Status&UFTSTAER))
      what->Return.Status|=UFTSTAEOF;

    if (!(what->Tracking.Status&WORK_ACK))
    {
      what->Tracking.Status|=WORK_ACK;
      while (what->Tracking.Status&WORK_IN_PROGRESS)
      {
        Relinquish(0L);
      }
      if (!(what->Tracking.Status&WORK_IN_MAIL))  /*if not in mail */
      {
        what->Tracking.Status|=WORK_IN_MAIL;
        Export(what->Tracking.Source,what);
      }
    }
  }
  else
  {
    while (what->Tracking.Status&(WORK_IN_MAIL|WORK_IN_PROGRESS))
      Relinquish(0L);
    what->Return.Opcode=COMPLETE;
    if (!(what->Tracking.Status&WORK_IN_MAIL))
      Export(what->Tracking.Source,what);
  }


}

void process_node(Node far *what,short me,channel far *config)
{
#define I what->Node.Info.Comm
  short temp;
  char far *opptr;
  char extrnop;
  char line=what->Node.Channel>>1;
  unsigned char far *data_buffer;
#undef DEBUG
#ifdef DEBUG
  displayln(nodestuff,"Rex:*n TCLdat:*n TCLidx:*n Xop:*h ct:*n BC:*h\n",
                      what->Node.Rex,
                      what->Node.Info.Comm.TCL_data,
                      what->Node.Info.Comm.TCL_index,
                      what->Node.XOptions,
                      what->Node.Info.Comm.Control,
                      (short)what->Node.Byte_count);
#endif
#define DEBUG
if (()config->Status&ChanStatEthr)==ChanStatEthr)
{
  if (!output[me>>1])
  {
    (long)output[me>>1]=1L;  /*flag it, that someone is nodeing on the problem*/

    if (fork(CHILD))
      open_terminate_watch(my_node);
    output[me>>1]
        =openether(my_node->Node.Info.Ether.SourceIP,
                   my_node->Node.Info.Ether.DestIP,
                   intswap(my_node->Node.Info.Ether.SourceTCP),
                   intswap(my_node->Node.Info.Ether.DestTCP),1);
    asm int 3;
    destory();

  }
  else
  if ((long)output[me>>1]==1)
  {

    if (fork(CHILD))
      open_terminate_watch(my_node);
    while (((long)output[me>>1]==1)&&my_node->Node.Node_Address)
      Relinquish(0L);
    destory();

  }

/*------ normal status set ups   done by node originator ------------

      my_node->Return.Status =0;
      my_node->Return.XStatus=0;
      my_node->Return.Byte_count=my_node->Node.Byte_count;

-------------------------------------------------------------------*/
}
if (()config->Status&ChanStatHost)==ChanStatHost)
{
  breaks[line].Transport=what->Node.Transport;
  breaks[line].SubChannel=what->Node.Channel;
  breaks[line].TCL_idx=what->Node.Info.Comm.TCL_index;
  breaks[line].TCL_avail=what->Node.Info.Comm.TCL_data;
  if (!lines[line].open||
       (lines[line].control&0xf0)!=(I.Control&0xf0)||
       lines[line].FrStPa!=I.Frame_Stop_Par||
       lines[line].Mode!=I.Mode||
       lines[line].baud!=I.Baud)
  {
    lines[line].LPP=I.Top_of_form;
    lines[line].CLP=0;
    lines[line].FrStPa=I.Frame_Stop_Par;
    lines[line].Mode=I.Mode;
    lines[line].baud=I.Baud;
    if (bauds[I.Baud]>0)
    {
      lines[line].config.baud=bauds[I.Baud];
    }
    else
    {
      abort_node(what,0xcbd);
      goto get_next;
    }
    lines[line].config.rx_size=I.Frame_Stop_Par&0xc0;
    lines[line].config.tx_size=(I.Frame_Stop_Par&0xc0)>>1;
    lines[line].config.stop_par=((I.Frame_Stop_Par&0x30)>>2)|
                                ((I.Frame_Stop_Par&0x06)>>1);
    lines[line].config.flow=Respect_soft|Command_soft|
                           (what->Node.Info.Comm.Control&0xf0)>>4;;
    lines[line].open=openchan(line,&lines[line].config);
  }
  lines[line].control=I.Control;
  if (!lines[line].open)
  {
    abort_node(what,0x596e);  /*NLN*/
    goto get_next;
  }
}

proc_again:
  process_rex(what);
get_next:
  what=get_old_node(me);
  if (!what)
    deactivate(me);

  goto proc_again;
}

BreakReturn make_break(short mask,BreakReturn far *packet)
{
  Node far *breakp=(Node far *)packet
  if (!breakp)
    breakp=Create_node(0);

  breakp->Node.Node_Address=-1;
  packet=(BreakReturn far *)breakp;
  packet->Opcode=BREAK;

  packet->TCL_data=breaks[curline].TCL_avail&mask;
  packet->TCL_idx=breaks[curline].TCL_idx;
  packet->Transport=breaks[curline].Transport;
  packet->SubChannel=breaks[curline].SubChannel;

}

void init_local_structs(void)
{
  char line;
  output=opendisplay(1,1,50,32,NEWLINE|BORDER,0x1f,0x1f,0x2e,"HostHandDbg");
//  nodestuff=opendisplay(1,1,50,32,NEWLINE|BORDER,0x1f,0x1f,0x2e,"Host Nodes");
  hold_IO();
  for (line=0;line<16;line++)
  {
    lines[line].config.rx_size=0xc0;
    lines[line].config.stop_par=0x44;
    lines[line].config.tx_size=0x60;
    lines[line].config.baud=0xe;
    lines[line].config.flow=Inbound_soft|Outbound_soft|
                     Outbound_hard|Respect_soft|Command_soft;
    lines[line].open=false;
    breaks[line].last_sigs=UNKNOWN;
    breaks[line].last_break=0;
    card_data[line]=higetline(line);
  }
  if (fork(BROTHER))
  {
    char line=15;
    while (1)
    {
      char ch;
      char cnt;
      Node far *breakp;
      BreakReturn far *packet;
      Relinquish(0L);
      if (!cnt++)
      {
        do
        {
          short idx;
          line=(++line)&0xf;
          position(output,1,line*2);
          if (line==curline)
          {
            setattr(output,0x71);
          }
          else
            setattr(output,0x1f);
          displayln(output,"*n:*n/*n/*n/*n/*n/*n(*h|*h)(*h|*h)*n *n\n",line,
                                          card_data[line]->line_status,
                                          card_data[line]->WR2,
                                          card_data[line]->WR3,
                                          card_data[line]->WR5,
                                          card_data[line]->WR12,
                                          card_data[line]->RR0,

                                          card_data[line]->Txq_head,
                                          card_data[line]->Txq_tail,
                                          card_data[line]->Rxq_head,
                                          card_data[line]->Rxq_tail,
                                          card_data[line]->Flow_control,
                                          card_data[line]->status);
          for (idx=0;idx<16;idx++)
          {
            short index;
            index=card_data[line]->Txq_head-16+idx;
            if (index>=512)
              idx-=512;
            displayln(output,"*n/",*
                            ((char far *)( ((long)card_data[line]&0xffff0000)|
                                card_data[line]->Txq_offset)+index));
          }
        }
        while (line);
      }
      do
      {
        line=(++line)&0xf;
        breakp=packet=0L;
        if (lines[line].open&&((card_data[line]->status&Break_recvd)||
                               (card_data[line]->RR0&BREAK_COND)))
        {
          if ((card_data[line]->RR0&BREAK_COND)||
              (card_data[line]->status&Break_recvd))
          {
            breaks[line].last_break=1;
            card_data[line]->RR0&=~BREAK_COND;
            card_data[line]->status&=~Break_recvd;
            breakp=make_break(BREAK_EVENT,breakp);
            flushline(line);
          }
        }
        else
        {
          breaks[line].last_break=0;
        }

        if (lines[line].open&&((card_data[line]->RR0&(DCD|CTS))==(DCD|CTS)))
        {
          switch(breaks[line].last_sigs)
          {
            case DISCONNECT:
              breakp=make_break(RING_EVENT|CONNECT_EVENT,breakp);
            case UNKNOWN:
              breaks[line].last_sigs=CONNECT;
            case CONNECT:
              break;
          }
        }

        if (lines[line].open&&(!(card_data[line]->RR0&(DCD))))
        {
          switch(breaks[line].last_sigs)
          {
            case CONNECT:
              breakp=make_break(DISCON_EVENT,breakp);
            case UNKNOWN:
              breaks[line].last_sigs=DISCONNECT;
            case DISCONNECT:
              break;
            /*disconnect*/
          }
        }
        if (breakp)
        {
          if (!packet->TCL_data)
            breakp->Node.Node_Address=0;
          Export(breakp->Tracking.Source,packet);
        }
      }
      while (line);

      {
        if (keypressed(output))
        {
          ch=readch(output);
          switch(ch)
          {
            case 'U':            case 'u':
              --curline;
              curline&=0xf;
              break;
            case 'N':            case 'n':
              ++curline;
              curline&=0xf;
              break;
            case 'B':            case 'b':
              if (!lines[curline].open)
                break;
              make_break(BREAK_EVENT);
              Export(breakp->Tracking.Source,packet);
              break;
            case 'R':            case 'r':
              if (!lines[curline].open)
                break;
              make_break(RING_EVENT);
              Export(breakp->Tracking.Source,packet);
              break;
            case 'C':            case 'c':
              if (!lines[curline].open)
                break;
              packet=make_break(CONNECT_EVENT);
              Export(breakp->Tracking.Source,packet);
              break;
            case 'D':            case 'd':
              if (!lines[curline].open)
                break;
              packet=make_break(DISCON_EVENT);
              Export(breakp->Tracking.Source,packet);
              break;
          }
        }
      }
    }
  }
}
void init_local_structs(void)
{
  unsigned short temp,i;

  temp=atoi(Get_environ("Sockets"));
  if (temp)
  {
    output=(connection far * far *)Allocate(temp*sizeof(connection far *));
    for (i=0;i<temp;i++)
      output[i]=0;
  }
  else
    Exit(30);
}
