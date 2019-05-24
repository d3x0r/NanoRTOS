#include <mod.h>
#include <npca.h>
#include "..\hostess\hostess.h"
#include "..\hostess\ipchost.h"
#include <ether.h>

#define DEFAULT_DEVICE ETHER

#define DEBUG
#ifdef DEBUG
#include <video.h>
windowptr debugout;
#endif

#define false 0
#define true (!false)

#define HOSTESS 0
#define ETHER   1
#define ASYNCH  2

char breakchar=4;

#define readascii(dev,buf) ( readbin(buf,1,dev)?(((*(buf)==19)||     \
                                                   (*(buf)==17)||    \
                                                   (*(buf)==breakchar))?0:1):0)
#define readbinchar(dev,buf) ( readbin(buf,1,dev) )
/*#define readbin(buf,cnt,dev) hiread(buf,cnt,dev)
#define write(buf,cnt,dev) hiwrite(buf,cnt,dev)*/
#define intswap(a) (((unsigned)a>>8)|((unsigned)a<<8))

#define ECHO(node) (!(  (node->Node.XOptions&UFTXOPNEC)|| \
                      (!(node->Node.Info.Comm.Control&4)) \
                     )                                    \
                   )

typedef short (far *iofunc)(char far *buf,short cnt,void far *dev);
typedef short (far *closefunc)(void far *dev);

closefunc closechan[2]={(closefunc)hiclose,(closefunc)closeether};

iofunc readbin[2]={(iofunc)hiread,(iofunc)readether};
iofunc iowrite[2]={(iofunc)hiwrite,(iofunc)sendether};

#define readbin readbin[currentC->type]
#define iowrite iowrite[currentC->type]
#define closechan closechan[currentC->type]

#define Process_Side(side) {                                                \
        if (currentC->side##state!=-1)                                      \
        {                                                                   \
          if (!currentC->side##cur_node->Node.Node_Address)                 \
          {                                                                 \
            complete_Node(currentC->side##cur_node);                        \
            currentC->side##state=-1;                                       \
          }                                                                 \
          else                                                              \
            currentC->side##state=Process_node(currentC->side##state,       \
                                               currentC->type,              \
                                               currentC->side##cur_node);   \
        }                                                                   \
        if (currentC->side##state==-1)                                      \
          side##dequeue(currentC);                                                \
        else                                                                \
          Slough_work(currentC->side##cur_node);                            \
        }


#define write_cons(string,length) switch(state)                 \
      {                                                         \
        case 0:                                                 \
          currentC->idx=0;                                      \
        case 1:                                                 \
          currentC->idx+=iowrite(string+currentC->idx,            \
                                length-currentC->idx,           \
                                currentC->output);              \
          if (currentC->idx<length)                             \
            return(1);                                          \
          currentC->CLP++;                                      \
        case 2:                                                 \
          if (Node->Tracking.Status&(WORK_IN_MAIL|WORK_IN_PROGRESS))  \
            return(2);                                                \
          Node->Return.Opcode=COMPLETE;                               \
          Node->Return.Byte_count=0;                                  \
          Node->Return.Status=0;                                      \
          Node->Node.FPI++;                                           \
      }                                                               \


char types[256];

char linefeeds[21]={'\r','\n','\n','\n','\n','\n','\n','\n','\n','\n','\n'
                        ,'\n','\n','\n','\n','\n','\n','\n','\n','\n','\n'};


typedef struct NodeHolder
{
  Node far *Node;
  struct NodeHolder far *next;
}NodeHolder;

typedef struct channel
{
  char Ostate,Estate;/* what state the node is at in process_node*/
  char type;         /* what type of device it is.  hostess, ether, etc.*/
  char Transport;    /* Unique Identifier 1*/
  char Channel;      /* Unique Identifier 2*/
  Node far *Ocur_node; /*what we are currenly working on on odd side.*/
  Node far *Ecur_node; /*what we are currently working on on even side.*/
  void far *output;   /*pointer to the output structure, whatever it may be*/
  NodeHolder far *ONext_node;  /*outstanding operations*/
  NodeHolder far *ENext_node;
  struct channel far *nextTrans,far *priorTrans,
                 far *priorChan,far *nextChan; /*links to other channels*/
  long trickle_time;
  short idx,start;
  char far *Data;
  short CLP,LPP;
  char TCL_avail;
  char TCL_idx;
  line_table hostess;
}channel;

#define NULL 0L

channel far *first_chan,far *currentC,far *breakC;

module far *channel_processor;

void configure_handler()
{
  /*this routine goes out to the environment statements to find
    out how to configure the transports that it handles... ie.
    what routines it will call.*/
#ifdef DEBUG
  debugout=opendisplay(2,1,40,20,BORDER|NO_SCROLL|NO_CURSOR|NEWLINE,
                       0x4f,0x7c,0x0,"channels");
#endif
}

void display_chans(char clear)
{
  channel far *temp1,far *temp2;
  short line=0,column=0;
  temp1=first_chan;
  if (clear)
    clr_display(debugout,1);
  while (temp1)
  {
    temp2=temp1;
    column=0;
    while (temp2)
    {
      if (clear)
      {
        position(debugout,column,line);
        displayln(debugout,"ÚÄÄÁÄÄÄ¿");
        position(debugout,column,line+2);
        displayln(debugout,"ÀÄÄÂÄÄÄÙ");
      }
      position(debugout,column,line+1);
      if (breakC==temp2)
        setattr(debugout,WHITE|ON_BLUE);
      else
        setattr(debugout,WHITE|ON_RED);
      displayln(debugout,"´*n*n*c*cÃ",temp2->Transport,temp2->Channel,
         temp2->Estate>=0?temp2->Estate+0x41:' ',
         temp2->Ostate>=0?temp2->Ostate+0x41:' ');
      column+=8;
      temp2=temp2->nextChan;
    }
    line+=3;
    temp1=temp1->nextTrans;
  }
}

void Slough_work(Node far *what)
{
  if (!(what->Tracking.Status&(SLOUGHED|WORK_ACK|WORK_IN_MAIL)))
  {
    what->Return.Opcode=SLOUGH;
    what->Tracking.Status|=SLOUGHED|WORK_IN_MAIL;       /*stored*/
    Export(what->Tracking.Source,what);
  }
  while (what->Tracking.Status&(WORK_IN_MAIL|WORK_IN_PROGRESS))
    Relinquish(0L);
}

void abort_node(Node far *what,short can_code)
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

void complete_Node(Node far *what)
{
  what->Return.Opcode=COMPLETE;
  what->Tracking.Status|=WORK_IN_MAIL|WORK_ACK;
  Export(what->Tracking.Source,what);
}

void write_formfeed(Node far *what,void far *line)
{
   if (what->Node.Info.Comm.Control&0x8)
     hiwrite("\f",1,line);
   else
     if (what->Node.Info.Comm.Top_of_form)
     {
       unsigned char i,total,remainder;
       total=what->Node.Info.Comm.Top_of_form-currentC->CLP;
       remainder=total%20;
       total=total/20;
       for (i=0;i<total;i++)
         hiwrite(linefeeds,21,line);
       if (what->Node.Node_Address)
         hiwrite(linefeeds,remainder+1,line);
     }
     else
       hiwrite(linefeeds,5,line);
     currentC->CLP=0;
}

char far *format_formfeed(Node far *what)
{
  char far *temp,tmp;
  if (what->Node.Info.Comm.Control&0x8)
  {
    temp=Allocate(2);
    temp[0]='\f';
    temp[1]=0;
  }
  else
    if (what->Node.Info.Comm.Top_of_form)
    {
      unsigned char i,total,remainder;
      total=what->Node.Info.Comm.Top_of_form-currentC->CLP;
      temp=Allocate(total+1);
      remainder=total%20;
      total=total/20;
      for (i=0;i<total;i++)
        for (tmp=0;tmp<21;tmp++)
          temp[i*20+tmp]=linefeeds[i*20+tmp];
      for (tmp=0;tmp<=remainder;tmp++)
        temp[i*20+tmp]=linefeeds[i*20+tmp];
      temp[i*20+tmp]=0;
    }
    else
    {
      temp=Allocate(6);
      for (tmp=0;tmp<5;tmp++)
        temp[tmp]=linefeeds[tmp];
      temp[tmp]=0;
    }
  currentC->CLP=0;
  return(temp);
}

void flushline()
{
  switch(currentC->type)
  {
    case HOSTESS:
      ((card_entry far *)currentC->output)->Rxq_tail=
          ((card_entry far *)currentC->output)->Rxq_head;
      break;
    case ETHER:
      flushether(currentC->output);
      break;
  }
}


void far *ioopen(char type,Node far *Node)
#define I Node->Node.Info.Comm
{
  void far *outdev=currentC->output;
  switch(type)
  {
    case HOSTESS:
      currentC->TCL_avail=I.TCL_data;
      currentC->TCL_idx=I.TCL_index;
      if (!outdev||
          (currentC->hostess.control&0xf0)!=(I.Control&0xf0)||
           currentC->hostess.FrStPa!=I.Frame_Stop_Par||
           currentC->hostess.Mode!=I.Mode||
           currentC->hostess.baud!=I.Baud)
      {
        currentC->LPP=I.Top_of_form;
        currentC->CLP=0;
        currentC->hostess.FrStPa=I.Frame_Stop_Par;
        currentC->hostess.Mode=I.Mode;
        currentC->hostess.baud=I.Baud;
        if (bauds[I.Baud]>0)
        {
          currentC->hostess.config.baud=bauds[I.Baud];
        }
        else
        {
          abort_node(Node,0xcbd);
        }
        currentC->hostess.config.rx_size=I.Frame_Stop_Par&0xc0;
        currentC->hostess.config.tx_size=(I.Frame_Stop_Par&0xc0)>>1;
        currentC->hostess.config.stop_par=((I.Frame_Stop_Par&0x30)>>2)|
                                    ((I.Frame_Stop_Par&0x06)>>1);
        currentC->hostess.config.flow=Respect_soft|Command_soft|
                               (Node->Node.Info.Comm.Control&0xf0)>>4;;
        outdev=hiopen(Node->Node.Channel>>1,&currentC->hostess.config);
      }
      currentC->hostess.control=I.Control;
      if (!outdev)
      {
        abort_node(Node,0x596e);  /*NLN*/
       }
      break;
    case ETHER:
      {
        channel far *savedC=currentC;
        if (!outdev)
        {
          if (fork(CHILD))
          {
            savedC->output=openether(Node->Node.Info.Ether.SourceIP,
                    Node->Node.Info.Ether.DestIP,
                    intswap(Node->Node.Info.Ether.SourceTCP),
                    intswap(Node->Node.Info.Ether.DestTCP),1,5000,5000,50);
            asm int 3;
            perish();
          }
          else
          {
            (long)outdev=1;
          }
        }
      }
      break;
  }
  return(outdev);
}

char Process_node(char state,char type,Node far *Node)
{
  char data_read,gotone=false;
  if ((long)currentC->output==1)  /*don't process node until it is open... this
                              open is delayed for some reason, and is
                              happening external to this task.  Should
                              the open complete, then the output device
                              will not be one, and we can continue from
                              here.*/
    return(state);
  if (!currentC->output||type==HOSTESS)
  {
    currentC->output=ioopen(type,Node);
    if (!currentC->output||(long)currentC->output==1)
      return(state);
  }
  switch(Node->Node.Rex&0xf)
  {
    case READ:/*read*/
      switch(state)
      {
        case 0:
          if (Node->Tracking.Side==EVEN)
          {
            Slough_work(Node);
          }
          /*at this point we could have a terminated node on our hands */
           if (Node->Node.XOptions&UFTXOPFLU)
          {
           /*flush*/
            flushline();
          }
          currentC->Data=(char far *)Node->Data;
          currentC->idx=0;
          currentC->start=0;
          currentC->trickle_time=0;
          if (Node->Node.Options&UFTOPTBI)
          {
            if (type==HOSTESS)
              ((card_entry far *)currentC->output)->Flow_control&=
                                              ~(Respect_soft);
            if (!(Node->Node.Options&UFTOPTDDO))
            {
             /*read into the buffer until a non null character is
               read.*/
        case 1:
              if (readbinchar(currentC->output,&data_read)>0)
              {
                if (ECHO(Node))
                  iowrite(&data_read,1,currentC->output);
                if (!data_read)
                  return(1);
              }
              else
                return(1);

              currentC->Data[currentC->idx++]=data_read;
              gettime(&currentC->trickle_time);
            }

            if (Node->Node.Options&UFTOPTSTD)
            { /*standard binary*/
              if (!(Node->Node.Options&UFTOPTTBX))
              {
        case 2:
                if (readbinchar(currentC->output,currentC->Data+currentC->idx)>0)
                {
                  if (ECHO(Node))
                    iowrite(currentC->Data+currentC->idx,1,currentC->output);
                  currentC->idx++;
                  if (currentC->idx<2)
                    return(2);
                }
                else
                  return(2);

                if (*((short far *)currentC->Data)==0x2424)
                {
                  Node->Return.Status=UFTSTAEOF;
                  complete_Node(Node);
                  return(-1);
                }
              }
              if (!currentC->idx)
              {
        case 3:
                if (readbinchar(currentC->output,currentC->Data+currentC->idx)>0)
                {
                  if (ECHO(Node))
                    iowrite(currentC->Data+currentC->idx,1,currentC->output);
                  currentC->idx++;
                }
                else
                  return(3);
              }
              if (currentC->Data[0]==3||currentC->Data[0]==7)
              {
                short block_size;
                short tsize;
        case 4:
                if (readbinchar(currentC->output,currentC->Data+currentC->idx++)>0)
                {
                  if (ECHO(Node))
                    iowrite(currentC->Data+currentC->idx,1,currentC->output);
                  if (currentC->idx<4)
                    return(4);
                }
                else
                  return(4);
        case 5:
                block_size=intswap(*(short far*)(currentC->Data+2));
                currentC->idx+=(tsize=readbin(currentC->Data+currentC->idx,
                       block_size-currentC->idx,
                       currentC->output));
                if (tsize>0)
                {
                  if (ECHO(Node))
                  iowrite(currentC->Data+currentC->idx-tsize,
                          tsize,
                          currentC->output);
                }
                else
                {
                  if (tsize==-1)
                  {
                    abort_node(Node,0);
                    return(-1);
                  }

                }
                if (currentC->idx<block_size)
                  return(5);
              }
              else
              {
                abort_node(Node,0x7726);  /*can SBV*/
              }
            }
            else
            {  /*non standard binary*/
              short tsize;
        case 6:
              currentC->idx+= (tsize=readbin(currentC->Data+currentC->idx,
                                 Node->Node.Byte_count-currentC->idx,
                                 currentC->output));
              if ((tsize>0)&&ECHO(Node))
              {
                iowrite(currentC->Data+currentC->idx-tsize,
                        tsize,
                        currentC->output);
              }
              else
                if (tsize==-1)
                {
                  abort_node(Node,0);
                  return(-1);
                }

              if (currentC->idx<Node->Node.Byte_count)
                return(6);
            }
          }
          else
          {
            char tchar;
            if (type==HOSTESS)
              ((card_entry far *)currentC->output)->Flow_control|=(Respect_soft);
            if (!(Node->Node.Options&UFTOPTDDO))
            {
              iowrite("\r\n",2,currentC->output);
              currentC->CLP++;
            }
            else
            {
        case 7:
              if (readascii(currentC->output,&tchar)>0)
              {
                if (ECHO(Node))
                  iowrite(&tchar,1,currentC->output);
                if (!tchar)
                  return(7);
              }
              else
                return(7);
              gotone=true;
            }
            if (Node->Node.Options&UFTOPTSTD)
            { /*standard ascii*/
        case 8:
              if (currentC->idx)
              {
                if (currentC->Data[0]==3||currentC->Data[0]==7)
                  return(4);
              }
              if (!gotone)
              {
                if (readascii(currentC->output,&tchar)>0)
                {
                  if (ECHO(Node))
                    iowrite(&tchar,1,currentC->output);
                }
                else
                  return(8);
              }
              else
                gotone=false;
              switch(tchar)
              {
                case '\r':
                case 0   :
                          if (!(Node->Node.Options&UFTOPTTBX)&&currentC->idx)
                          {
                            currentC->idx--;
                            while (currentC->Data[currentC->idx]==' ')
                              currentC->idx--;
                            currentC->idx++;
                          }
                          if ((currentC->idx&1)&&
                              (currentC->idx+1<Node->Node.Byte_count))
                          {
                            currentC->Data[currentC->idx++]=' ';
                            Node->Return.XStatus=-1;
                          }
                          if (!(Node->Node.Options&UFTOPTTBX)&&
                              (currentC->idx+2<Node->Node.Byte_count))
                          {
                            currentC->Data[currentC->idx++]=0;
                            currentC->Data[currentC->idx++]=0;
                            Node->Return.XStatus-=2;
                          }
                          Node->Return.XStatus&=0x1ff;
                          return(9);
                case '\n':break;
                case '\b':if (currentC->idx)
                            currentC->idx--;
                          break;
                case 127:currentC->idx=0;
                         iowrite("\r\n",2,currentC->output);
                         currentC->CLP++;
                         break;
                default:currentC->Data[currentC->idx++]=tchar;
              }
              if (currentC->idx<Node->Node.Byte_count)
                return(8);
        case 9:
              if ((currentC->idx>2)&&!(Node->Node.Options&UFTOPTTBX))
                if (*(short far *)currentC->Data==0x2424)
                  Node->Return.Status|=UFTSTAEOF;
            }
            else
            { /*non standard ascii */
        case 10:
              if (!gotone)
                if (readascii(currentC->output,&tchar)>0)
                {
                  if (ECHO(Node))
                    iowrite(&tchar,1,currentC->output);
                }
                else
                  return(10);
              else
                gotone=false;
              currentC->Data[currentC->idx++]=tchar;
              if (!(Node->Node.Options&UFTOPTNT))
              {
                if (tchar==(Node->Node.Options&UFTOPTTRM))
                {
                  Node->Return.XStatus=(-1)&0x1ff;
                  break;
                }
              }
              if (currentC->idx<Node->Node.Byte_count)
                return(10);
            }
            Node->Return.Byte_count=currentC->idx;
          }
        case 11:
           if (Node->Tracking.Status&(WORK_IN_MAIL|WORK_IN_PROGRESS))
             return(11);

           Node->Tracking.Side=ODD;
           Node->Node.FPI++;
           Node->Return.Opcode=COMPLETE;
      }
      break;
    case WRITE:
      switch(state)
      {
        case 0:
          currentC->Data=(char far *)Node->Data;
          currentC->idx=0;
          currentC->start=0;
          currentC->trickle_time=0;
          if (Node->Node.Options&UFTOPTBI)
          {
            if (type==HOSTESS)
              ((card_entry far *)currentC->output)->Flow_control&=~(Command_soft);
            if (Node->Node.Options&UFTOPTSTD)
            {
              if (currentC->Data[0]==3||currentC->Data[0]==7)
        case 1:
              {
                unsigned short output_count=*((short *)(currentC->Data+2));
                if (output_count>Node->Node.Byte_count)
                  output_count=Node->Node.Byte_count;
                currentC->idx+=iowrite(currentC->Data+currentC->idx,
                                     output_count-currentC->idx,
                                     currentC->output);
                if (currentC->idx<output_count)
                  return(1);
                Node->Return.Byte_count=output_count;
              }
              else
              {
                Node->Return.Status=UFTSTAER|UFTSTASBV;
                Node->Return.Byte_count=0;
              }
            }
            else
            {
        case 2:
              currentC->idx+=iowrite(currentC->Data+currentC->idx,
                      (short)Node->Node.Byte_count-currentC->idx,
                      currentC->output);
              if (currentC->idx<Node->Node.Byte_count)
                return(2);
              Node->Return.Byte_count=Node->Node.Byte_count;
            }
          }
          else
          {
            unsigned short idx,bias=0;
            if (type==HOSTESS)
              ((card_entry far *)currentC->output)->Flow_control|=(Command_soft);
            if (Node->Node.Options&UFTOPTSTD)
            {
              switch(currentC->Data[0])
              {
                case '0':
                         iowrite("\r\n\n",3,currentC->output);
                         currentC->CLP+=2;
                         break;
                case '1':
                case '-':
                        if (currentC->type!=ETHER)
                           write_formfeed(Node,currentC->output);
                         break;
                case '+':
                         iowrite("\r",1,currentC->output);
                         break;
                default:
                         iowrite("\r\n",2,currentC->output);
                         currentC->CLP++;
                         break;
              }
              currentC->Data++;
              bias++;
              Node->Node.Byte_count--;
              if (currentC->Data[0]==2)
              {
                currentC->Data++;
                bias++;
                Node->Node.Byte_count--;
              }
              for (idx=0;
                   (idx<Node->Node.Byte_count)&&currentC->Data[idx];
                   idx++);
            }
            else
            {
              unsigned short terminator;

              if (Node->Node.Options&UFTOPTNT)
                terminator=256;
              else
                terminator=Node->Node.Options&UFTOPTTRM;

              for (idx=0;
                  (idx<Node->Node.Byte_count)&&
                   currentC->Data[idx]!=terminator;
                   idx++);
            }
            currentC->idx=idx;
        case 3:
            idx=iowrite(currentC->Data,currentC->idx,currentC->output);
            currentC->Data+=idx;
            currentC->idx-=idx;
            if (currentC->idx)
              return(3);
            currentC->idx=(short)currentC->Data;
          }
          while (Node->Tracking.Status&(WORK_IN_MAIL|WORK_IN_PROGRESS))
            Relinquish(0L);
          Node->Return.Opcode=COMPLETE;
          Node->Return.Byte_count=currentC->idx;
          Node->Node.FPI++;
      }
          break;
    case REWIND:
    case ADVANCE_FILE:
      switch(state)
      {
        case 0:
        if (currentC->type!=ETHER)
        {
          currentC->Data=format_formfeed(Node);
          currentC->idx=strlen(currentC->Data);
        case 1:  
          currentC->idx-=iowrite(currentC->Data,currentC->idx,currentC->output);
          if (currentC->idx)
            return(1);
           write_formfeed(Node,currentC->output);
        }
        case 2:
          if (Node->Tracking.Status&(WORK_IN_MAIL|WORK_IN_PROGRESS))
            return(2);
          Node->Return.Opcode=COMPLETE;
          Node->Return.Byte_count=0;
          if ((Node->Node.Rex&0xf)==REWIND)
          {
            Node->Return.Status=UFTSTAEOF|UFTSTABOM;
            Node->Node.FPI=0;
          }
          else
          {
            Node->Return.Status=UFTSTAEOF;
            Node->Node.FPI++;
          }
      }
      break;
    case BACK_RECORD:
    case BACK_FILE:
      switch(state)
      {
        case 0:
           if (Node->Tracking.Status&(WORK_IN_MAIL|WORK_IN_PROGRESS))
             return(0);
           Node->Return.Opcode=COMPLETE;
           Node->Return.Byte_count=0;
           Node->Return.Status=UFTSTAEOF|UFTSTABOM;
      }
      break;
    case ADVANCE_RECORD:
      write_cons("\r\n",2);
      break;
    case WRITE_EOF:
      write_cons("\r\n\n$$",5);
      break;
    case HOME:
      if (Node->Node.XOptions&UFTXOPDCL)
      {
        closechan(currentC->output);
        currentC->output=NULL;
      }
      Node->Return.Byte_count=0;
      Node->Node.FPI++;
      Node->Return.Opcode=COMPLETE;
      break;
  }
  if (!currentC->CLP&&!(Node->Return.Status&UFTSTAER))
    Node->Return.Status|=UFTSTAEOF;

  if (!(Node->Tracking.Status&WORK_ACK))
  {
    Node->Tracking.Status|=WORK_ACK;
    while (Node->Tracking.Status&WORK_IN_PROGRESS)
    {
      Relinquish(0L);
    }
    if (!(Node->Tracking.Status&WORK_IN_MAIL))  /*if not in mail */
    {
      Node->Tracking.Status|=WORK_IN_MAIL;
      Export(Node->Tracking.Source,Node);
    }
  }
  return(-1);

}

void Edequeue(channel far *Channel)
{
  /*this dequeues any pending work so that the channel may continue
    to process*/
  NodeHolder far *next;
  if (Channel->ENext_node)
  {
    Channel->Ecur_node=Channel->ENext_node->Node;
    next=Channel->ENext_node->next;
    Free(Channel->ENext_node);
    Channel->ENext_node=next;
    Channel->Estate=0;
  }
  else
    Channel->Ecur_node=NULL;
}

void Odequeue(channel far *Channel)
{
  /*this dequeues any pending work so that the channel may continue
    to process*/
  NodeHolder far *next;
  if (Channel->ONext_node)
  {
    Channel->Ocur_node=Channel->ONext_node->Node;
    next=Channel->ONext_node->next;
    Free(Channel->ONext_node);
    Channel->ONext_node=next;
    Channel->Ostate=0;
  }
  else
    Channel->Ocur_node=NULL;
}

char test_break(void)
{
  char status=false;
  card_entry far *channel;
  if (currentC->type==HOSTESS)
  {
    channel=(card_entry far *)currentC->output;
    if ((channel->status&Break_recvd)||
        (channel->RR0&BREAK_COND))
    {
      channel->RR0&=~BREAK_COND;
      channel->status&=~Break_recvd;
      channel->Rxq_tail=channel->Rxq_head;  /*flush data*/
      status=true;
    }
  }
  if (currentC->type==ETHER)
  {
    status=false;
  }
  return(status);
}

BreakReturn far * make_break(channel far *Channel,
                             short mask,
                             BreakReturn far *packet)
{
  Node far *breakp=(Node far *)packet;
  if (!breakp)
    breakp=Create_node(0);

  breakp->Node.Node_Address=-1;
  packet=(BreakReturn far *)breakp;
  packet->Opcode=BREAK;

  packet->TCL_data=Channel->TCL_avail&mask;
  packet->TCL_idx=Channel->TCL_idx;
  packet->Transport=Channel->Transport;
  packet->SubChannel=Channel->Channel;
  return(packet);
}

void Process_nodes()
{
  channel far *currentT,far *next;
  long distime,_distime;
  BreakReturn far *breakp;
  channel_processor=my_TCB;

  while (first_chan)
  {
    currentT=first_chan;
    while (currentT)
    {
      currentC=currentT;
      while (currentC)
      {

        breakp=NULL;
        if (test_break())
          breakp=make_break(currentC,BREAK_EVENT,breakp);
        if (breakp)
        {
          if (!breakp->TCL_data)
            ((Node far *)breakp)->Node.Node_Address=0;
          Export(((Node far *)breakp)->Tracking.Source,breakp);
        }
        Process_Side(O);
        Process_Side(E);
        currentC=currentC->nextChan;
      }
      currentT=currentT->nextTrans;
    }
    gettime(&distime);
    if ((distime-_distime)>500)
    {
      display_chans(false);
      _distime=distime;
    }
    if (keypressed(debugout))
    {
      char ch;
      ch=readch(debugout);
      switch(ch)
      {
        case 'B':
        case 'b':
          breakp=make_break(breakC,BREAK_EVENT,breakp);
          if (breakp)
          {
            if (!breakp->TCL_data)
              ((Node far *)breakp)->Node.Node_Address=0;
            Export(((Node far *)breakp)->Tracking.Source,breakp);
          }
          break;
        case 'N':
        case 'n':
          if (!breakC)
            breakC=first_chan;
          else
            if (breakC->nextChan)
              breakC=breakC->nextChan;
          display_chans(false);
          break;
        case 'P':
        case 'p':
          if (!breakC)
            breakC=first_chan;
          else
            if (breakC->priorChan)
              breakC=breakC->priorChan;
          display_chans(false);
          break;
      }
    }
    Relinquish(0L);
  }
  perish();
}

#define NT 0
#define TNA 1
#define CF 2
#define CNF 3
#define CNA 4
#define TNF 5

void activate(char Transport,char Channel,Node far *Node)
{
  channel far *curchan=first_chan,far *tempchan,far *priorchan;
  char lost=NT;
  Channel>>=1;
  while (curchan)
  {
    if (Transport<curchan->Transport)
    {
      lost=TNA;
      break;
    }
    else
      if (Transport==curchan->Transport)
      {
        if (Channel<curchan->Channel)
        {
          lost=CNA;
          break;
        }
        else
          if (Channel==curchan->Channel)
          {
            /*we ahve equal transport and equal channels... the
              channel is already to be processed*/
            lost=CF;
            break;
          }
          else
          {
            /*we have equal transports, and a channel greater than what
              we were looking for in curchan*/
            priorchan=curchan;
            curchan=curchan->nextChan;
            lost=CNF;
          }
      }
      else
      {
        /* we have unequal transports... it was not found at all,
           the transport does not even exist*/
        priorchan=curchan;
        curchan=curchan->nextTrans;
        lost=TNF;
      }
  }
  while (!(tempchan=Allocate(sizeof(channel))))
  {
    Relinquish(0L);
  }
  switch(lost)
  {
    case NT: /* No tranports*/
      currentC=first_chan=tempchan;
      first_chan->priorTrans=first_chan->nextTrans=NULL;
      first_chan->priorChan=first_chan->nextChan=NULL;
      break;
    case TNA: /* curchan=next transport */
      tempchan->nextChan=tempchan->priorChan=NULL;
      tempchan->nextTrans=curchan;
      tempchan->priorTrans=curchan->priorTrans;
      if (curchan->priorTrans)
        curchan->priorTrans->nextTrans=tempchan;
      else
        first_chan=tempchan;
      curchan->priorTrans=tempchan;
      currentC=tempchan;
      break;
    case CF: /* curchan=correct channel*/
      Free(tempchan);
      tempchan=NULL;
      if (Node->Node.Channel&1)
      {
        if (curchan->Ostate==-1)
        {
          curchan->Ostate=0;
          curchan->Ocur_node=Node;
        }
        else
        {
          NodeHolder far *nodehold;
          Slough_work(Node);
          nodehold=Allocate(sizeof(NodeHolder));
          nodehold->Node=Node;
          nodehold->next=NULL;
          if (curchan->ONext_node)
          {
            NodeHolder far *curhold;
            curhold=curchan->ONext_node;
            while (curhold->next)
              curhold=curhold->next;
            curhold->next=nodehold;
          }
          else
            curchan->ONext_node=nodehold;
        }
      }
      else
      {
        if (curchan->Estate==-1)
        {
          curchan->Estate=0;
          curchan->Ecur_node=Node;
        }
        else
        {
          NodeHolder far *nodehold;
          Slough_work(Node);
          nodehold=Allocate(sizeof(NodeHolder));
          nodehold->Node=Node;
          nodehold->next=NULL;
          if (curchan->ENext_node)
          {
            NodeHolder far *curhold;
            curhold=curchan->ENext_node;
            while (curhold->next)
              curhold=curhold->next;
            curhold->next=nodehold;
          }
          else
            curchan->ENext_node=nodehold;
        }
      }
      break;
    case CNF:  /* prior=last channel on transport, cur=null*/
      priorchan->nextChan=tempchan;
      tempchan->priorChan=priorchan;
      tempchan->nextChan=NULL;
      tempchan->priorTrans=tempchan->nextTrans=NULL;
      currentC=tempchan;
      break;
    case CNA:  /* cur=next channel*/
      tempchan->priorTrans=curchan->priorTrans;
      if (curchan->priorTrans)
        curchan->priorTrans->nextTrans=tempchan;
      else
        if (!curchan->priorChan)
          first_chan=tempchan;
      curchan->priorTrans=NULL;

      tempchan->nextTrans=curchan->nextTrans;
      if (curchan->nextTrans)
        curchan->nextTrans->priorTrans=tempchan;
      curchan->nextTrans=NULL;
      tempchan->nextChan=curchan;
      tempchan->priorChan=curchan->priorChan;
      if (curchan->priorChan)
        curchan->priorChan->nextChan=tempchan;
      curchan->priorChan=tempchan;
      currentC=tempchan;
      break;
    case TNF:  /* prior=last transport, cur=NULL*/
      priorchan->nextTrans=tempchan;
      tempchan->priorTrans=priorchan;
      tempchan->nextTrans=NULL;
      tempchan->nextChan=tempchan->priorChan=NULL;
      currentC=tempchan;
      break;
  }
  if (tempchan)
  {
    if (Node->Node.Channel&1)
    {
      tempchan->Ostate=0;
      tempchan->Ocur_node=Node;
      tempchan->Estate=-1;
      tempchan->Ecur_node=NULL;
    }
    else
    {
      tempchan->Estate=0;
      tempchan->Ecur_node=Node;
      tempchan->Ostate=-1;
      tempchan->Ocur_node=NULL;
    }
/*    tempchan->type=types[Transport];*/
    tempchan->type=DEFAULT_DEVICE;
    tempchan->Transport=Transport;
    tempchan->Channel=Channel;
    tempchan->ONext_node=NULL;
    tempchan->ENext_node=NULL;
    tempchan->output=NULL;
    tempchan->trickle_time=0;
    tempchan->idx=tempchan->start=0;
    display_chans(true);
  }
  if (!channel_processor)
  {
    if (!fork(BROTHER))
      Process_nodes();
  }
}

void main()
{
  Node far *received;
  configure_handler();
  while (1)
  {
    received=Import(device_name);
    activate(received->Node.Transport,received->Node.Channel,received);
  }
}
