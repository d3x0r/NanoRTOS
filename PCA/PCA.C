#define TESTCODE
#define PCAROOT
#include "npca.h"
#include "pcadef.h"
#include "mod.h"
#include "video.h"
#define NULL 0L

char sabotage_next=false;
short opcount=0;                 /*operations before shutdown of work disp.*/
char display_mail=DMAIL_DEFAULT;
char display_work=DWORK_DEFAULT;
char far *devices[MAX_DEVICES];
char far *my_device=NULL;

#define GET_INTQ(INTNAME)  (int_queues[QUEUE_##INTNAME][0])

#define DEC_INTQ(INTNAME)  {short tvalue;                     \
        asm cli;                                              \
        tvalue=FIELD_GET(int_work,POS_##INTNAME,2)-1;         \
        FIELD_SET(int_work,POS_##INTNAME,2,tvalue);           \
        *((long*)int_queues[QUEUE_##INTNAME])=*((long*)(int_queues[QUEUE_##INTNAME]+1));   \
        *((long*)(int_queues[QUEUE_##INTNAME]+2))=*((long*)(int_queues[QUEUE_##INTNAME]+3)); \
        asm sti;                                              \
  }

/*This module contains all routines needed to talk to the PCA card.
  Such as get value from mailbox,send and receive DMA, kick watchdog,
*/

Node_holder far *evenq=NULL,far *oddq=NULL;

unsigned char dma_pages[4]={0,0x8b,0x89,0x8a};
unsigned char dma_base[4]={0,0xc4,0xc8,0xcc};
short mail[256],_mail[256];
short packet_cnt=0;
short got_error=false;
char error_latch=0;
char destroy_next=false;
window_type far *output=0L,far *nodes=0L,far *error=0L;
module far *input_task,far *evenTCB,far *oddTCB;

Node far *first_work=0L,far *error_node=0L;
short base;
char filling=0;
short thread=0;
long eventime,oddtime;

short IRQ,DMP,priority;
unsigned short int_work;          /*this marks queues that are active.*/
unsigned short int_queues[8][5];  /*this stores the value of the register
                                    associated with the interupt.  TC's will
                                    just store a 1 so that the value is non
                                    zero*/
short input_DMA,output_DMA;
short control,config,PCA_num;
short status;
char far *fill_name;
char do_transfer=false; /*flag for iRQ routine to initiate DMA send on
                          input TI interrupt*/

extern void connect_int(short IRQ);
extern void disconnect(short IRQ);
extern short read_mailbox(int number); /*returns actual value in mailbox*/


extern short read_port(short offset); /*returns the value of a port*/
extern void write_port(short offset,short value); /*sets value of a port*/
extern void write_mail(int number,short data); /*write a value into a mailbox*/
extern short out_port(short offset,short value); /*set the value of a port*/
extern void outportb(short port,char value); /*put out a byte to the
                                               specified port*/
extern char inportb(short port);
extern void read_all_mail(short far *mail_array); /*read all 256 mailboxes
                                                    into the specified
                                                    array*/

extern short copy_mail(short start,Node far *where);   /*copy data from
                                                         the mailbox to
                                                         the appropriate
                                                         queue_enry*/

extern void send_mail(short start,void far *what);


extern void shiftrong(unsigned long far * value,char places);
extern void shiftlong(unsigned long far * value,char places);



#define Check_shutdown { if (opcount) \
                           if (!(--opcount)) display_work=false;}

Empty_evenbuf()
{
  if ((!(got_error&EMPTYINPUT)) && fork(CHILD))
  {
    got_error|=EMPTYINPUT;
    while ((read_port(STATUS)&EVEN_FUL))
    {
      Relinquish(0L);
      read_port(DATA);
    }
    got_error&=~EMPTYINPUT;
    perish();
  }
}

char page(void far*address)
{
  unsigned long temp=(unsigned long)0|(unsigned short)address;
  (long)address^=(unsigned short)address;
  shiftrong((unsigned long far *)&address,12);
  temp+=(unsigned long)address;
  shiftrong((unsigned long far *)&temp,16);
  return ((unsigned char)temp);
}

short ofs(void far*address)
{
  unsigned long temp=(unsigned long)0|(unsigned short)address;
  (unsigned long)address^=(unsigned short)address;
  shiftrong((unsigned long far *)&address,12);
  temp+=(unsigned long)address;
  return ((unsigned short)temp);
}

void mask_DMA(void)
{
//  outport(0xd4,0xff);  /*mask all dmas*/
}

short Initiate_DMA(char channel,
                   unsigned short far *buffer,
                   unsigned short count,
                   char write_mem,char start_now)
{
  unsigned char pg=page(buffer);
  unsigned short offset=ofs(buffer);
  if ((offset&1)||   /*if the buffer starts on an odd address we cannot*/
      (channel<5)||  /*send it... therefore return a false...or if the */
      (channel>8))   /*DMA channel specified was an unsupported one.   */
    return(false);

  offset>>=1;        /*convert byte offset into word offset*/

  if (pg&1)          /*if the page is odd then we need to carry the bit into*/
    offset|=0x8000;  /* The offset*/

  channel-=4;

  if (display_work)
  {
    displayln(nodes,"Ct:*h Ofs:*h Pg:*n Ch:*d w:*d\n",
                    count,offset,pg,channel,write_mem);
  }
  count--;
  outportb(dma_pages[channel],pg);
  outportb(0xd8,0xff);           /*clear byte flip-flop*/
  outportb(dma_base[channel],(char)offset);
  outportb(dma_base[channel],offset>>8);
  outportb(dma_base[channel]+2,(char)count);
  outportb(dma_base[channel]+2,count>>8);

//  outportb(0xd0,0x00);    /*command out*/

  if (write_mem)
  {
    outportb(0xd6,0x44|channel);
    control|=1;
    outportb(0xd4,channel);
  }
  else
  {
    outportb(0xd6,0x48|channel);
    control|=2;
    if (start_now)
      outportb(0xd4,channel);
    else
      do_transfer=channel;
  }
  return(true);
}

short process_ints(void)
{
#define SetInt(a) if (IntQ&INT_##a) { short tvalue;       \
                   asm cli;                               \
                   tvalue=FIELD_GET(int_work,POS_##a,2)+1;\
                   FIELD_SET(int_work,POS_##a,2,tvalue);  \
                   asm sti;                               }
  char IntQ=read_port(INTR_QUEUE);
  if (!IntQ)
    return(false);
  SetInt(EVENTC);
  SetInt(ODDTC);
  SetInt(EVENTI);
  SetInt(ODDTI);
  SetInt(EVENSEL);
  SetInt(ODDSEL);
  SetInt(EVENTERM);
  SetInt(ODDTERM);
  write_port(INTR_QUEUE,IntQ);
  return(true);
}

void show_error(window_type far *window,char *line,short term)
{
  char attr=getattr(window);
  setattr(window,RED|ON_GREY);
  displayln(window,line);
  if (term)
  {
    setattr(window,YELLOW|ON_BLUE);
    displayln(window,"Stat:*h Term:*h",read_port(STATUS),term);
  }
  setattr(window,attr);
  if (window->next_window)
    moddisplay(window,SELECT_WINDOW,END_MOD);
  if (!opcount)
    opcount=5;
}

void show_warn(window_type far *window,char *line)
{
  char attr=getattr(window);
  setattr(window,BLACK|ON_GREEN);
  displayln(window,line);
  setattr(window,attr);
}

#define Watch_Dog(what,time,status) { time_struc End,Current; \
    gettime(&End);                                            \
    End+=time;                                                \
    while (!(what))                                           \
    {                                                         \
      gettime(&Current);                                      \
      if (Current>=End)                                       \
      {                                                       \
        status=false;                                         \
        break;                                                \
      }                                                       \
      Relinquish(0L);                                         \
    }                                                         \
  }


short Send(unsigned short far *buffer,unsigned long count,char short_expected)
{
  short term=0,tvalue;
  short xfr_status=true;
  time_struc endtime,curtime;
  unsigned long total_count=(count>>1)+(count&1);
  unsigned short work_count=0;
  unsigned short offset;
  unsigned short got_busy=0;
  char first=true;
  long buf_address=NORM_BUF(buffer);
  if (count)
  do
  {
    if (work_count)
    {
      buf_address+=(work_count<<1);
    }
    (long)buffer=PTR_BUF(buf_address);
    offset=(short)buffer;
    if (offset&1)
      return(false);
    work_count=0x8000-(offset>>1);
    if (work_count<=total_count)
    {
      total_count-=work_count;
    }
    else
    {
      work_count=total_count;
      total_count=0;
    }
    Initiate_DMA(output_DMA,
                 buffer,
                 work_count,
                 READ_MEM,(first?false:true));

    first=false;
    out_port(CONTROL,control);
    gettime(&endtime);
    endtime+=(100+(work_count>>6));
    /*this is different from receive because the transfer is not
      neccesarily started from the modcomp at this point, so we do not
      know if the card is busy to begin with or not.  So, we track the
      state of busy, so that when we do get it, we latch it up, so
      if it dissappears then we know that there was an unexpected cir-
      cumstance that arrose.*/
    while ((!(int_work&(ODDTC|ODDTERM)))&&
               ((read_port(STATUS)&(CARD_ARMD|got_busy))==(CARD_ARMD|got_busy)))
    {
      if (read_port(STATUS)&ODD_BSY)
      {
        got_busy=ODD_BSY;
        if (do_transfer)  /*If Controller is busy,
                                                     but, it hasn't been started
                                                     then start the DMA*/
        {
          outportb(0xd4,do_transfer);
          do_transfer=0;
        }
      }
      Relinquish(0L);
      gettime(&curtime);
      if (curtime>endtime)
        break;
    }
    if ((read_port(STATUS)&CARD_ARMD)!=CARD_ARMD)
    {
      show_error(nodes,"MC on P->M\n",0);
      xfr_status=false;
      total_count=0;
    }
    if (!(int_work&(ODDTC|ODDTERM)))
    {
      if (got_busy)
        show_error(nodes,"(WDG) ON P->M\n",0);
      else
        show_error(nodes,"(NBUSY) ON P->M\n",0);
      xfr_status=false;
      total_count=0;
    }
    do_transfer=0;                /*this is after we get Terminate / TC */
    if (int_work&ODDTERM)
    {
      term=GET_INTQ(ODDTERM);
      if (term&ABRT_TRM)
      {
        DEC_INTQ(ODDTERM);
        if (term==0x345)
          show_warn(nodes,"Data Rejected\n");
        else
          show_error(nodes,"Abort DMP P->M.\n",term);
        total_count=0;
        xfr_status=false;
      }
      else
      {
        /*if it wasn't an abort, check to see if it would be acceptable
          to have a terminate... ie the transfer is done. */
        if (total_count)
        {
          show_error(nodes,"Unexpected Xfer Terminate P->M.",term);
          total_count=0;
          xfr_status=false;
        }
      }
    }
    if (xfr_status)
    {
      char error_type=0;
      short count=(((unsigned char)inportb(dma_base[output_DMA]+2)<<8)|
           (unsigned char)inportb(dma_base[output_DMA]+2))+1;
      if (!count)
        error_type=1;
      Watch_Dog(int_work&ODDTC,TRMDOG,xfr_status);
      control&=~2;
      out_port(CONTROL,control);
      if (xfr_status)
      {
        DEC_INTQ(ODDTC);
      }
      else
      {
        if (error_type)
        {
          show_error(nodes,"Lost TC P->M\n",-1);
        }
        else
        {
          show_error(nodes,"P->M Xfer Incomplete\n",-1);
          displayln(nodes,"*h(*h) wds mising\n",count,count+total_count);
        }
        DEC_INTQ(ODDTERM);  /*we had a terminate, and no TC*/
        total_count=0;
      }
    }
    else
    {
      if (int_work&ODDTC)
        DEC_INTQ(ODDTC);
    }
  }
  while (total_count);

/*if this is a known shorter transfer than modcomp is expecting then
  generate an external interupt to short circuit the xfer */
  if (short_expected)
    out_port(CONTROL,control|EXTERNAL_ODD);

  if (xfr_status)
  {
    Watch_Dog(int_work&ODDTERM,TRMDOG,xfr_status);
    if (xfr_status)
    {
      term=read_port(IN_TERM);
      if (term&ABRT_TRM)
      {
        show_error(nodes,"Abort DMP P->M",term);
        xfr_status=false;
      }
      DEC_INTQ(ODDTERM);
    }
    else
    {
      show_error(nodes,"Lost MT P->M\n",-1);
    }
  }
  else
  {
    if (int_work&ODDTERM)
      DEC_INTQ(ODDTERM);
  }
  control&=~2;
  return(xfr_status);
}


short Receive(unsigned short far *buffer,unsigned long count)
{
  short term=0,tvalue;
  short xfr_status=true;
  char statuses[12];
  time_struc endtime,curtime;
  unsigned long total_count=(count>>1)+(count&1);
  unsigned short work_count=0;
  unsigned short offset;
  long buf_address=NORM_BUF(buffer);
  if (got_error&EMPTYINPUT)
    return(false);
  if (count)
  do
  {
    if (work_count)
    {
      buf_address+=(work_count<<1);
    }
    (long)buffer=PTR_BUF(buf_address);
    offset=(short)buffer;
    if (offset&1)
      return(false);
    work_count=0x8000-(offset>>1);
    if (work_count<=total_count)
    {
      total_count-=work_count;
    }
    else
    {
      work_count=total_count;
      total_count=0;
    }
    Initiate_DMA(input_DMA,
                 buffer,
                 work_count,
                 WRITE_MEM,true);

    out_port(CONTROL,control);
    gettime(&endtime);
    endtime+=(100+(work_count>>6));
    while ((!(int_work&(EVENTERM|EVENTC)))&&
            ((read_port(STATUS)&(EVEN_BSY|CARD_ARMD))==(EVEN_BSY|CARD_ARMD))
           )
    {
      Relinquish(0L);
      gettime(&curtime);
      if (curtime>endtime)
        break;
    }
    if ((read_port(STATUS)&CARD_ARMD)!=CARD_ARMD)
    {
      show_error(nodes,"MC on M->P\n",0);
      xfr_status=false;
      total_count=0;
    }
    if (!(int_work&(EVENTC|EVENTERM)))
    {
      if (curtime>endtime)
        show_error(nodes,"(WDG) ON M->P\n",0);
      else
        show_error(nodes,"(NBUSY) ON M->P\n",0);
      xfr_status=false;
      total_count=0;
    }


    if (int_work&EVENTERM)
    {
      term=GET_INTQ(EVENTERM);
      if (term&ABRT_TRM)
      {
        DEC_INTQ(EVENTERM);
        show_error(nodes,"Abort DMP M->P.",term);
        total_count=0;
        xfr_status=false;
      }
      else
      {
        /*if it wasn't an abort, check to see if it would be acceptable
          to have a terminate... ie the transfer is done. */
        if (total_count)
        {
          show_error(nodes,"Unexpected Xfer Terminate M->P.",term);
          total_count=0;
          xfr_status=false;
        }
      }
      Empty_evenbuf();
    }
    if (xfr_status)
    {
      Watch_Dog(int_work&EVENTC,TRMDOG,xfr_status)
      control&=~1;
      out_port(CONTROL,control);
      if (xfr_status)
      {
        DEC_INTQ(EVENTC);
      }
      else
      {
        short count=(((unsigned char)inportb(dma_base[output_DMA]+2)<<8)|
             (unsigned char)inportb(dma_base[output_DMA]+2))+1;
        DEC_INTQ(EVENTERM);  /*we got a terminate, but no TC */
        if (!count)
          show_error(nodes,"Lost TC M->P\n",-1);
        else
        {
          show_error(nodes,"M->P Xfer Incomplete\n",-1);
          displayln(nodes,"*h(*h) wds mising\n",count,count+total_count);
        }
        total_count=0;
      }
    }
    else
    {
      if (int_work&EVENTC)
        DEC_INTQ(EVENTC);
    }
  }
  while (total_count);
  if (xfr_status)
  {
    Watch_Dog(int_work&EVENTERM,TRMDOG,xfr_status);
    if (xfr_status)
    {
      term=read_port(OUT_TERM);
      if (term&ABRT_TRM)
      {
        show_error(nodes,"Abort DMP M->P\n",term);
        total_count=0;
        xfr_status=false;
      }
      DEC_INTQ(EVENTERM);
    }
    else
    {
      show_error(nodes,"Lost MT M->P\n",0);
    }
  }
  else
  {
    if (int_work&EVENTERM)
      DEC_INTQ(EVENTERM);
  }
  control&=~1;
  return(xfr_status);
}


void do_fill(char far *filename)
{
  /*this routine is called with the name of the file in which the fill data
    is stored... the file must contain the count of the words to transfer,
    which is stored in the first word.*/
  unsigned short far *fill_data;
  short handle;
  unsigned short size;
/*  asm int 3;*/
  handle=open(filename,0); /*open for read access only */
  if (handle==-1)
    show_error(nodes,"Couldn't open Fill File!\n",0);
  else
  {
    if (!read(&size,2,handle))
      show_error(nodes,"Couldn't read Fill File!\n",0);
    else
    {
      size=((size<<8)|(size>>8))*2;
      if (!(fill_data=Allocate(size)))
        show_error(nodes,"Couldn't allocate Fill Space!\n",0);
      else
      {
        if (read(fill_data,size,handle)!=size)
          show_error(nodes,"Fill file shorter than expected!\n",0);
        else
        {
          /*now we have the data and the count of the transfer, and
            all we need to do is call send to give the data to the
            modcomp!*/
          Send(fill_data,size,false);
          int_work&=~(ODDTI|ODDTC|ODDTERM);
        }
        Free(fill_data);
      }
    }
    close(handle);
  }
}

short unlink(Node far *node)
{
  Node far *temp;
  temp=first_work;
  while (temp)
  {
    if (node==temp)
      break;
    temp=temp->Tracking.next;
  }
  if (!temp)
  {
    show_error(nodes,"Node not linked!\n",0);
    displayln(nodes,"*h *h (*h,*n)\n",*(((short far *)&node)-1),
                             *(((short far *)&node)+1),
                             (short)node->Node.Node_Address,
                             node->Return.Opcode);
  }

  if (node->Tracking.Status&SAVE_WHEN_DONE)
    return(false);
  if (node->Tracking.prior)
    node->Tracking.prior->Tracking.next=node->Tracking.next;
  else
    first_work=node->Tracking.next;
  if (node->Tracking.next)
    node->Tracking.next->Tracking.prior=node->Tracking.prior;
  node->Tracking.next=NULL;
  node->Tracking.prior=NULL;
  return(true);
}

void destroynode(Node far *node)
{
  if (unlink(node))
  {
    if (!(node->Tracking.Status&USER_BUFFER)&&node->Data)
      Free(node->Data);
    if (node->Return.Extended_mail)
      Free(node->Return.Extended_mail);
    Free(node);
  }
}

void time_stamp(Node far *temp,char far *mark)
{
  long temp_time;
  if (display_work)
  {
    gettime(&temp_time);
    displayln(nodes,"*h *s Time:*h\n",
                    (short)temp->Node.Node_Address,
                    mark,
                    (short)(temp_time-temp->Tracking.start_time));
  }
}

void odd_channel(void)
{
  Node far *temp;
  Node_holder far *tqueue;
  oddTCB=my_TCB;
  while (true)
  {
check_nodes:
    if (oddq)
    {
      temp=oddq->Node;
      tqueue=oddq;
      oddq=oddq->next;
      Free(tqueue);
    }
    else
    {
      Relinquish(-10000L);
      goto check_nodes;
    }
    time_stamp(temp,"Odd");
    {
      short mail_retry=0;
send_mail_again:
      do
      {
        while ((read_port(STATUS)&(ODD_BSY|ODD_ISI|ODD_XSI))||
               (read_port(IN_SELECT)&HAND_BSY))
                  /*while Interal/Busy/external
                   or inprocess of being made busy*/
        {
          Relinquish(0L);
          if (!temp->Node.Node_Address)
            goto end_node;
        }
        if (!temp->Node.Node_Address)
          goto end_node;
        if (temp->Return.Opcode==BREAK)
          send_break(PC_mail_out,temp);
        else
          send_mail(PC_mail_out,temp);
      }
      while ((read_port(STATUS)&(ODD_BSY|ODD_ISI|ODD_XSI))||
             (read_port(IN_SELECT)&HAND_BSY));
                                                          /*check status
                                                           to make sure that
                                                           it really is
                                                           safe. */
      out_port(CONTROL,control|EXTERNAL_ODD);  /*give him the external*/
      time_stamp(temp,"OSI");
end_node:
      switch(temp->Return.Opcode)
      {
        case SLOUGH:
          if (!temp->Node.Node_Address)
            show_error(nodes,"Termination of a sloughed node.... CAN Happen HUH?",0);
          temp->Tracking.Status&=~WORK_IN_MAIL;
          temp->Tracking.Status|=WORK_IN_PROGRESS;
          while (read_port(STATUS)&ODD_XSI)
            Relinquish(0L);
          temp->Tracking.Status&=~WORK_IN_PROGRESS;
          break;
        case COMPLETE:
          if (temp->Data&&temp->Node.Node_Address&&temp->Return.Byte_count)
          {
            time_stamp(temp,"SND");
            if (!(Send(temp->Data,temp->Return.Byte_count,
                       (temp->Tracking.Status&TERM_WHEN_DONE))))
            {
              time_stamp(temp,"SFL");
              if (mail_retry++<2)
                goto send_mail_again;
              show_error(nodes,"WDG",0);
              temp->Return.XStatus=0;
              temp->Return.Status=UFTSTAER|UFTSTAINO|UFTSTAOTH;
              temp->Return.Byte_count=0x9067; /*CAN of 'WDG'*/
              /*because we free the data, we will not return this way.
                There is no way out but to send the mail and free
                the node. */
              Free(temp->Data);
              temp->Data=NULL;
              goto send_mail_again;
            }
          }
        case BREAK:
          if (temp->Return.Opcode==BREAK)
          {
            Node far *term=first_work;
            BreakReturn far *breakp=(BreakReturn far *)temp;
            while (read_port(STATUS)&ODD_XSI)
              Relinquish(0L);
            if (*process_control&1)
              Relinquish(0L);
            while (term)
            {
              if ((term!=temp)&&
                  (term->Node.Transport==breakp->Transport)&&
                  ((term->Node.Channel&0xfe)==(breakp->SubChannel&0xfe))&&
                  ((term->Node.Rex&0xf)==READ))
                term->Node.Node_Address=0;
              term=term->Tracking.next;
            }
          }
          time_stamp(temp,"DST");
          destroynode(temp);
          break;

      }
    }
  }
}

void even_channel(void)
{
  Node far *temp;
  Node_holder far *tqueue;
  evenTCB=my_TCB;
  while(true)
  {
check_nodes:
    if (evenq)
    {
      temp=evenq->Node;
      tqueue=evenq;
      evenq=evenq->next;
      Free(tqueue);
    }
    else
    {
      Relinquish(-10000L);
      goto check_nodes;
    }
    time_stamp(temp,"Evn");
    {
      do
      {
        while (((read_port(STATUS)&(EVEN_BSY|EVEN_ISI|EVEN_XSI))||
                (read_port(OUT_SELECT)&HAND_BSY))
                &&temp->Node.Node_Address)
                  /*while Interal/Busy/external
                   or inprocess of being made busy*/
          Relinquish(0L);
        if (!temp->Node.Node_Address)
          goto end_node;
        if (temp->Return.Opcode==4)
          send_break(PC_mail_in,temp);
        else
          send_mail(PC_mail_in,temp);
      }
      while (((read_port(STATUS)&(EVEN_BSY|EVEN_ISI|EVEN_XSI))||
              (read_port(OUT_SELECT)&HAND_BSY))
              &&temp->Node.Node_Address);
           /*check status to make sure that it really is safe. */

      if (!temp->Node.Node_Address)
        goto end_node;
      out_port(CONTROL,control|EXTERNAL_EVEN);  /*give him the external*/
      time_stamp(temp,"ESI");
end_node:
      switch(temp->Return.Opcode)
      {
        case SLOUGH:
          temp->Tracking.Status|=WORK_IN_PROGRESS;
          temp->Tracking.Status&=~WORK_IN_MAIL;
          while (read_port(STATUS)&EVEN_XSI)
            Relinquish(0L);
          temp->Tracking.Status&=~WORK_IN_PROGRESS;
          time_stamp(temp,"ACK");
          break;
        case COMPLETE:
          time_stamp(temp,"DST");
          destroynode(temp);
          break;
        case BREAK:
        {
          Node far *term=first_work;
          BreakReturn far *breakp=(BreakReturn far *)temp;
          while (read_port(STATUS)&ODD_XSI)
            Relinquish(0L);
          if (*process_control&1)
            Relinquish(0L);
          while (term)
          {
            if ((term!=temp)&&
                (term->Node.Transport==breakp->Transport)&&
                ((term->Node.Channel&0xfe)==(breakp->SubChannel&0xfe))&&
                ((term->Node.Rex&0xf)==READ))
              term->Node.Node_Address=0;
            term=term->Tracking.next;
          }
          time_stamp(temp,"DST");
          destroynode(temp);
          break;
        }
      }
    }
  }
}


void do_output()
{
  Node far *temp;
  Node_holder far *queue;
  while (true)
  {
    temp=Import(device_name);
    if (temp->Node.Node_Address)
    {
      if(display_work)
      {
/*        displayln(nodes,"-*h *n RC:*n *n\n S:*h *h *h\n",*/
        displayln(nodes,"-*h *n RC:*n *n\n",
                                      (short)temp->Node.Node_Address,
                                      temp->Node.Rex,
                                      temp->Return.Opcode,
                                      temp->Node.Channel,
                                      temp->Return.Status,
                                      temp->Return.Byte_count,
                                      temp->Return.XStatus);
        Check_shutdown;
      }
      if (temp->Tracking.Side)
      {
        time_stamp(temp,"P>M");
        if (oddq)
        {
          queue=oddq;
          while (queue->next) queue=queue->next;
          while (!(queue->next=Allocate(sizeof(Node_holder))))
            Relinquish(0L);
          queue=queue->next;
        }
        else
        {
          while (!(oddq=queue=Allocate(sizeof(Node_holder))))
            Relinquish(0L);
        }
        oddTCB->status=0;
      }
      else
      {
        time_stamp(temp,"M>P");
        if (evenq)
        {
          queue=evenq;
          while (queue->next) queue=queue->next;
          while (!(queue->next=Allocate(sizeof(Node_holder))))
            Relinquish(0L);
          queue=queue->next;
        }
        else
        {
          while (!(evenq=queue=Allocate(sizeof(Node_holder))))
            Relinquish(0L);
        }
        evenTCB->status=0;
      }
      queue->Node=temp;
      queue->next=NULL;
    }
    else
    {
      /*Node address was zero, so just deallocate it.
        If returning complete. */
      if (display_work)
        displayln(nodes,"Dead Node.");
      if (temp->Return.Opcode==COMPLETE||temp->Return.Opcode==BREAK)
        destroynode(temp);
    }
  }
}

public(Node far *,Create_node,(char side))
{
  Node far *temp;
  Load_DS;
  while (!(temp=Allocate(sizeof(Node))))
  {
    Relinquish(0L);
  }
  temp->Tracking.prior=NULL;
  temp->Tracking.next=first_work;
  if (first_work)
    first_work->Tracking.prior=temp;
  first_work=temp;
  temp->Tracking.Side=side;
  if (side)
    temp->Tracking.start_time=oddtime;
  else
    temp->Tracking.start_time=eventime;
  temp->Tracking.Source=my_device;
  temp->Tracking.Side=side;
  temp->Tracking.Status=0;
  temp->Return.Status=0;
  temp->Return.XStatus=0;
  temp->Return.Extended_Size=0;
  temp->Return.Extended_mail=NULL;

  /*temp->Return.Opcode=-1;  not valid opcode - set in copy_mail()*/
/*  temp->Data_handle.Preamble=NULL;
  temp->Data_handle.index=0;*/
  temp->Data=NULL;
  Restore_DS;
  return(temp);
}

void proc_in_mail(short from,short dma,char side)
{
  Node far *temp;
  Node far *victim;
  if (destroy_next)
  {
    show_error(nodes,"Dumping Node...\n",0);
    destroy_next=false;
    return;
  }
  /*read mailboxes into a work queue entry - in, and create the
     work packet (allocated here because it knows how many extra words
     there are) */
  asm push ss
  temp=Create_node(side);
  if (display_work)
    displayln(nodes,"Side:*c Dma:*c\n",(side?'O':'E'),(dma?'Y':'N'));
  if (!copy_mail(from,temp))
  {
    asm add sp,2

    if (temp->Return.Opcode==-3)
      show_error(nodes,"Duplicate Node\n",0);

    if (temp->Return.Opcode==-2)
      show_error(nodes,"Mail Size Expected\n",0);

    if (temp->Return.Opcode==-4)
      show_error(nodes,"Mail Size Unknown\n",0);

    destroynode(temp);
    return;           /*if there wasn't actually mail*/
  }
  asm add sp,2

  /*if there is a data transfer expexted, allocate the buffer*/
  if (temp->Node.Byte_count)
  {
    while (!(temp->Data=Allocate(temp->Node.Byte_count)))
      Relinquish(0L);
  }
  else
    temp->Data=NULL;
  temp->Return.Byte_count=temp->Node.Byte_count;

  /* get the data if indicated to DMA it in*/
  if (dma&&temp->Node.Byte_count)
  {
    if (sabotage_next)
    {
      temp->Node.Byte_count++;
      sabotage_next=false;
    }
    if (!Receive(temp->Data,temp->Node.Byte_count))
    {
      /*we need to hold this node, to compare it next time we come thru
        here... next time should also be the same node, unless of course
        the modcomp decideds for reasons unknown to us that it will
        not try to retransmit the work*/
      if (error_node)
      {
        show_warn(nodes,"Error Replace");
        Free(error_node->Data);
        Free(error_node);
      }
      unlink(temp);
      error_node=temp;
      return;
    }
    else
    {

      /*if we have a previous node that was in error, then check to
        see if the current work is the same as the other one.*/
      if (temp->Tracking.Status&RETRANS)
      {
        if (error_node)
        {
          if (temp->Node.Node_Address==error_node->Node.Node_Address)
          {
            if (error_node->Node.FPI!=temp->Node.FPI)
              show_error(nodes,"[RETRANS FPI]",0);
            if (error_node->Node.Rex!=temp->Node.Rex)
              show_error(nodes,"[RETRANS Rex]",0);
            if (error_node->Node.Byte_count!=temp->Node.Byte_count)
              show_error(nodes,"[RETRANS BC]",0);
            if (error_node->Node.Options!=temp->Node.Options)
              show_error(nodes,"[RETRANS OPT]",0);
            if (error_node->Node.XOptions!=temp->Node.XOptions)
              show_error(nodes,"[RETRANS XOPT]",0);
            {
              short bufidx,erroridx,showx=0;
              error_node->Node.Byte_count++;
              error_node->Node.Byte_count>>=1;
              for (bufidx=0;bufidx<error_node->Node.Byte_count&&
                            bufidx<((temp->Node.Byte_count+1)>>1);bufidx++)
              {
                if (error_node->Data[bufidx]!=temp->Data[bufidx])
                {
                  show_error(nodes,"[RETRANS CMP]",0);
                  if (!error)
                    error=opendisplay(5,1,50,6,NO_CURSOR|BORDER|NEWLINE,
                                      WHITE|ON_BLUE,LT_BLUE|ON_BLUE,0,"Data Error");
                  showx=0;
                  for (erroridx=bufidx-10;erroridx<bufidx;erroridx++)
                  {
                    position(error,showx,0);
                    if (erroridx<0)
                      displayln(error,"     ");
                    else
                      displayln(error,"*h ",error_node->Data[erroridx]);
                    position(error,showx,2);
                    if (erroridx<0)
                      displayln(error,"     ");
                    else
                      displayln(error,"*h ",temp->Data[erroridx]);
                    showx+=5;
                  }
                  showx=0;
                  for (erroridx=bufidx;erroridx<(bufidx+10);erroridx++)
                  {
                    position(error,showx,1);
                    if (erroridx>error_node->Node.Byte_count)
                      displayln(error,"     ");
                    else
                      displayln(error,"*h ",error_node->Data[erroridx]);
                    position(error,showx,3);
                    if (erroridx>((temp->Node.Byte_count+1)>>1))
                      displayln(error,"     ");
                    else
                      displayln(error,"*h ",temp->Data[erroridx]);
                    showx+=5;
                  }
                  break;
                }
              }
              position(error,0,4);
              displayln(error,"Errpos: *5d ErrBufLen: *5d BufLen: *5d",
                                 bufidx,
                                 (short)error_node->Node.Byte_count,
                                 (short)((temp->Node.Byte_count+1)>>1));
              position(error,0,5);
              displayln(error,"ErBufPtr: *H BufPtr: *H [*h]",
                              (long)error_node->Data,
                              (long)temp->Data,
                              bufidx<<1);
            }
            show_warn(nodes,"Error Remove");
free_error:
            Free(error_node->Data);
            Free(error_node);
            error_node=NULL;

          }
          else
            goto check_dup;
        }
        else
        {
          Node far *current;
check_dup:
          current=first_work;
          while (current)
          {
            if (current!=temp&&temp->Node.Node_Address==current->Node.Node_Address)
            {
              show_error(nodes,"[DUPLICATE]",0);
              destroynode(temp);
              return;
            }
            current=current->Tracking.next;
          }
          /*check for actual duplicate*/
        }
      }
      else
        if (error_node)
        {
          if (error_node->Node.Node_Address==temp->Node.Node_Address)
          {
            show_warn(nodes,"Non-Dup Error Match");
            goto free_error;
          }

        }
    }
    if (display_work)
       displayln(nodes,"**");
  }
  else
    if (display_work)
      displayln(nodes,"+");

  if (display_work)
  {
/*    displayln(nodes,"*h *n to:*n *n\n",*/
    displayln(nodes,"*h *n to:*n *n\n*h *h *h\n",
                     (short)temp->Node.Node_Address,
                     temp->Node.Rex,
                     temp->Node.Transport,
                     temp->Node.Channel,
                     temp->Node.Options,
                     (short)temp->Node.Byte_count,
                     temp->Node.XOptions);
    Check_shutdown;
  }
  if (stricmp(devices[temp->Node.Transport],device_name))
        /*if anything other than a message to me, ship it
                           to specified device.*/
  {
    if (!Export(devices[temp->Node.Transport],(unsigned char far *)temp)||
         temp->Node.Transport>MAX_DEVICES)
    {
      temp->Return.Opcode=COMPLETE;
      temp->Return.Status=UFTSTAEOF|UFTSTABOM;
      temp->Return.Byte_count=0;
      temp->Return.XStatus=0;
      if (temp->Data)
      {
        Free(temp->Data);
        temp->Data=0;
      }
      Export(device_name,temp);
    }
  }
  else
  {
          /*do the Node.Rex myself*/
    switch(temp->Node.Rex&0xf)
    {
      case TERMINATE:  /*Terminate operation*/
             {
               Node far *victim=first_work;
               if (display_work)
                 displayln(nodes,"Term/");
               while (victim)
               {
                if (victim->Node.Node_Address==temp->Node.Node_Address&&
                    victim!=temp)
                {
                  if (display_work)
                    displayln(nodes,"Found/");
                  victim->Node.Node_Address=0L;
                  temp->Tracking.Side=victim->Tracking.Side;
                  break;
                }
                victim=victim->Tracking.next;
               }
               if (!victim)
                 if (display_work)
                    displayln(nodes,"Failed/");
               if (display_work)
                 displayln(nodes,"return\n");
               temp->Return.Opcode=1;
               if (temp->Data)
               {
                 Free(temp->Data);
                 temp->Data=0L;
               }
               temp->Return.Status=0;
               temp->Return.Byte_count=0;
               temp->Return.XStatus=0;
               Export(device_name,temp);
             }
             break;
      default:
               temp->Return.Opcode=1;
               if (temp->Data)
               {
                 Free(temp->Data);
                 temp->Data=0L;
               }
               temp->Return.Status=UFTSTAEOF|UFTSTABOM;
               temp->Return.Byte_count=0;
               temp->Return.XStatus=0;
               Export(device_name,temp);
    }
  }
  packet_cnt++;
}

void do_input()
{
  char charac;
  short tvalue;
  input_task=my_TCB;
top:

  while (!keypressed(output)&&!keypressed(nodes))
  {
    out_port(CONTROL,control);
    if (int_work&(EVENSEL|EVENTI))
    {
      if (display_work)
        displayln(nodes,"INTQ=*h ",int_work);
      proc_in_mail(Mod_mail_in,int_work&EVENTI,0);
      if (int_work&(EVENSEL))
      {
        if (display_work)
          displayln(nodes,"SEL=*h ",GET_INTQ(EVENSEL));
        DEC_INTQ(EVENSEL);
      }
      if (int_work&(EVENTI))
      {
        if (display_work)
          displayln(nodes,"TI=*h ",GET_INTQ(EVENTI));
        DEC_INTQ(EVENTI);
      }
    }
    if (int_work&ODDSEL)
    {
      if (display_work)
        displayln(nodes,"INTQ=*h ",int_work);
      proc_in_mail(Mod_mail_out,0,1);
      if (display_work)
        displayln(nodes,"SEL=*h ",GET_INTQ(ODDSEL));
      DEC_INTQ(ODDSEL);
    }
    if (int_work&ODDTI)
    {
      if (filling)
      {
        do_fill(fill_name);
        filling=false;
        while (process_ints());
      }
      DEC_INTQ(ODDTI);
    }
    asm cli;   /* Relinquish's turn on interupts again. */
    if (!int_work&&
        !read_mailbox(Mod_mail_in)&&
        !read_mailbox(Mod_mail_out))
      Relinquish(-1000L);
    else
      Relinquish(0L);

    if ((read_port(STATUS)&(EVEN_ERR|ODD_ERR|CARD_ARMD))!=
                           (EVEN_ERR|ODD_ERR|CARD_ARMD))
    {
      Node far *temp=first_work;
      if (!(read_port(STATUS)&ODD_ERR)&&!(error_latch&ODDERROR))
      {
        show_error(nodes,"Error on Odd Channel.",GET_INTQ(ODDTERM));
        if (int_work&ODDTERM)
          DEC_INTQ(ODDTERM);
        error_latch|=ODDERROR;
      }
      if (!(read_port(STATUS)&EVEN_ERR)&&!(error_latch&EVENERROR))
      {
        show_error(nodes,"Error on Even Channel",GET_INTQ(EVENTERM));
        if (int_work&EVENTERM)
          DEC_INTQ(EVENTERM);
        Empty_evenbuf();
        error_latch|=EVENERROR;
      }
      if (!(read_port(STATUS)&CARD_ARMD))
      {
        show_error(nodes,"Card Disarmed.",0);
        if (error_latch&DISARMERROR)
        {
          Relinquish(-500L);
        }
        error_latch|=DISARMERROR;
        /*remove all prior terminates*/
        write_port(MAIL_ADDRESS,PC_mail_out);
        write_port(MAIL_DATA,0);    /*make sure outgoing mail is marked read*/
        write_port(MAIL_ADDRESS,PC_mail_in);
        write_port(MAIL_DATA,0);    /*make sure outgoing mail is marked read*/
        outportb(0xa0,0x20);  /*send EOI's to the interupts chips.  5/5/93*/
        outportb(0x20,0x20);
        write_port(INTR_QUEUE,read_port(INTR_QUEUE));
        /*Kill all outstanding nodes */
        while (temp)
        {
          temp->Node.Node_Address=0;
          temp=temp->Tracking.next;
        }
        evenTCB->status=0;
        oddTCB->status=0;
      }
      filling=0;
      int_work=0;
      do_transfer=0;
      out_port(CONFIGURATION,config);
      out_port(OUT_CHANNEL,0);
      out_port(IN_CHANNEL,0);
      mask_DMA();
      control=4;
    }
    else
      error_latch=0;
  }
  if (keypressed(output))
    charac=readch(output);
  if (keypressed(nodes))
    charac=readch(nodes);

  if (charac=='o' || charac=='O')
  {
    asm int 3;
    goto top;
  }

  if (charac==27) Exit(2);

  if (charac==0)                 /* process extended characters */
  {
    if (keypressed(output))
      charac=readch(output);
    if (keypressed(nodes))
      charac=readch(nodes);

    if (charac=='D')
    {
      opcount=0;
      display_work=!display_work;
      if (display_work)
      {
        clr_display(nodes,1);
        position(nodes,0,0);
      }
      goto top;
    }

    if (charac=='C')
    {
      display_mail=!display_mail;
      goto top;
    }
#ifdef TESTCODE
    if (charac=='B')
    {
      sabotage_next=true;
/*      destroy_next=true;*/
      goto top;
    }
#endif
    charac=0;
  }

  Relinquish(0);              /*let others look at it */
  goto top;
}

cleanup(void,dualpca,(void))
{
  Load_DS;
  disconnect(IRQ);
  Restore_DS;
}

short r1,r2,r3,r4,r5,r6,r7,r8,r9;
short _r1,_r2,_r3,_r4,_r5,_r6,_r7,_r8,_r9,_int_work;

void do_diags()
{
  short i;
  short cnt;
  r1=~_r1;       r4=~_r4;    r7=~_r7;
  r2=~_r2;       r5=~_r5;    r8=~_r8;
  r3=~_r3;       r6=~_r6;    r9=~_r9;

  change_priority(255);

  while (true)
  {
    do
      Relinquish(-255);     /*relinquish for 255 passes of the swapper*/
    while(!display_mail);
    {
      read_all_mail(mail);
      for (i=0;i<4;i++)
        for (cnt=0;cnt<16;cnt++)
          if (mail[(i*MAIL_MAX)+cnt]!=_mail[(i*MAIL_MAX)+cnt])
          {
            position(output,(cnt&7)*5,(i<<1)+(cnt/8) );
            displayln(output,"*r",mail[(i*MAIL_MAX)+cnt]);
            _mail[(i*MAIL_MAX)+cnt]=mail[(i*MAIL_MAX)+cnt];
          }
      position(output,0,8);
#define displayi(rnum,port,msg) { if (int_work!=_int_work||(r##rnum=read_port(port))!=_r##rnum) \
      {                                                                  \
         position(output,0,rnum+9);                                        \
         displayln(output,msg"= *h/*n",int_work,r##rnum);              \
         _int_work=int_work;                                             \
        _r##rnum=r##rnum;                                                  \
      } }

#define displayr(rnum,port,msg) { if ((r##rnum=read_port(port))!=_r##rnum) \
      {                                                                    \
         position(output,0,rnum+9);                                        \
         displayln(output,msg"= *h",r##rnum);                              \
        _r##rnum=r##rnum;                                                  \
      }}
#define displayt(rnum,msg) { if ((r##rnum=*(short far *)0x0000046cL)!=_r##rnum)\
      { position(output,0,rnum+9);                                       \
        displayln(output,msg"= *h",r##rnum);                \
        _r##rnum=r##rnum;                                                  \
      } }
      displayln(output,"DMA OUT CNT= *n*n DMA IN CNT= *n*n\n",
             inportb(0xca),inportb(0xca),
             inportb(0xce),inportb(0xce));
      displayln(output,"DMA OUT OFS= *n*n DMA IN OFS= *n*n\n",
             inportb(0xc8),inportb(0xc8),
             inportb(0xcc),inportb(0xcc));

      r1=read_port(STATUS);
      if (r1!=_r1)
      {
        displayln(output,"Status Out:*c*c*c*c*c*c*c*s In:*c*c*c*c*c*c*c*s\n",
                           (r1&ODD_ERR )?' ':'E',
                           (r1&ODD_N   )?'N':' ',
                           (r1&ODD_MPE )?'M':' ',
                           (r1&ODD_ISI )?'I':' ',
                           (r1&ODD_XSI )?'X':' ',
                           (r1&ODD_BSY )?'B':' ',
                           (r1&MCCLK   )?'m':'p',
                           (r1&ODD_EMPT)?(char far *)"OBMT":(char far *)"OBFL",

                           (r1&EVEN_ERR )?' ':'E',
                           (r1&EVEN_N   )?'N':' ',
                           (r1&EVEN_MPE )?'M':' ',
                           (r1&EVEN_ISI )?'I':' ',
                           (r1&EVEN_XSI )?'X':' ',
                           (r1&EVEN_BSY )?'B':' ',
                           (r1&CARD_ARMD)?'A':' ',
                           (r1&EVEN_FUL )?(char far *)"IBFL":(char far *)"IBMT");
        _r1=r1;
      }
      else displayln(output,"\n");
      displayr(2,2,   "Out Terminate ");
      position(output,21,11);
      displayln(output,"*h *h",
                      int_queues[QUEUE_EVENTERM][0],
                      int_queues[QUEUE_EVENTERM][1]);
      displayr(3,10,  "In Terminate  ");
      position(output,21,12);
      displayln(output,"*h *h",
                      int_queues[QUEUE_ODDTERM][0],
                      int_queues[QUEUE_ODDTERM][1]);
      displayi(4,16,  "Int Queue     ");
      displayr(5,4,   "Out TI        ");
      position(output,21,14);
      displayln(output,"*h *h",
                      int_queues[QUEUE_EVENTI][0],
                      int_queues[QUEUE_EVENTI][1]);
      displayr(6,12,  "In TI         ");
      position(output,21,15);
      displayln(output,"*h *h",
                      int_queues[QUEUE_ODDTI][0],
                      int_queues[QUEUE_ODDTI][1]);
      displayr(7,6,   "Out Select    ");
      position(output,21,16);
      displayln(output,"*h *h",
                      int_queues[QUEUE_EVENSEL][0],
                      int_queues[QUEUE_EVENSEL][1]);
      displayr(8,14,  "In Select     ");
      position(output,21,17);
      displayln(output,"*h *h",
                      int_queues[QUEUE_ODDSEL][0],
                      int_queues[QUEUE_ODDSEL][1]);
      displayt(9,     "Time          ");
      position(output,0,19);
      displayln(output,"Control : *b  Packets: *d",control,packet_cnt);
    }
  }
}

char go=false;

void main()
{
  char temp,far *num;
  short i,cnt,lines;
  if (Get_environ("Debug"))
  {
    output=opendisplay(20,1,40,20
                      ,NO_CURSOR|BORDER|NEWLINE
                      ,BLACK|ON_GREY,YELLOW,0,"PCA_Diagnostics");
    clr_display(output,1);
    if (fork(INDEPENDANT))
    {
      fork(INDEPENDANT);
      fork(INDEPENDANT);
    }
  }
  else
  {
    fork(INDEPENDANT);
    fork(INDEPENDANT);
  }
  thread++;
  switch(thread)
  {
    case 1:do_output();
    case 2:even_channel();
    case 3:odd_channel();
    case 4:break;
    case 5:while (!go) Relinquish(0L);
             do_diags();
    default:Relinquish(1);
  }
  for (cnt=0;cnt<MAX_DEVICES;cnt++)
  {
    char number[10];
    itoa(cnt,number);
    devices[cnt]=Get_environ(number);
  }

  _dualpca();
  _Create_node();
  my_device=device_name;
  lines=atoi(Get_environ("WORK_lines"));
  if (lines<44)
    lines=44;
  if (lines>100)
    lines=100;

  nodes=opendisplay(60,1,20,lines
                   ,NO_CURSOR|BORDER|NEWLINE
                   ,GREEN,WHITE,0,"Work");
  if (lines>44)
    moddisplay(nodes,RESIZEY(44-lines),
                     PAGE_DOWN,
                     PAGE_DOWN,
                     END_MOD);
  base=atoi(Get_environ("PCA_BASE"));
  input_DMA=atoi(Get_environ("PCA_DMA_In"));
  output_DMA=atoi(Get_environ("PCA_DMA_Out"));
  PCA_num=atoi(Get_environ("PCA_device"));
  priority=atoi(Get_environ("PCA_priority"));
  DMP=atoi(Get_environ("PCA_DMP"));
  fill_name=Get_environ("Fill");
  config=0x8000|DMP|(PCA_num<<8)|((priority-3)<<4);
  for (i=0;i<4;i++)
    for (cnt=0;cnt<16;cnt++)
    {
      write_mail((i*MAIL_MAX)+cnt,0);
      mail[(i*MAIL_MAX)+cnt]=0;
      _mail[(i*MAIL_MAX)+cnt]=0x1234;
    }


  IRQ=atoi(Get_environ("PCA_IRQ"));
  i=0;
  while ((read_port(STATUS)&EVEN_FUL)&&(i<200))
  {
    read_port(DATA);
    i++;
  }
  outportb(0xa0,0x20);  /*send EOI's to the interupts chips.  5/5/93*/
  outportb(0x20,0x20);
  out_port(INTR_QUEUE,read_port(INTR_QUEUE));
  out_port(OUT_CHANNEL,0);
  out_port(IN_CHANNEL,0);
  connect_int(IRQ);
  control=4;
  go=true;
  out_port(CONFIGURATION,config);
  do_input();
}


