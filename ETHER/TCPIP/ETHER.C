#define strcmp
#include <mod.h>
#define ETHER
#include "ether.h"
#include <video.h>
#define NULL 0L
#define SOCKWID 50
#define SOCKHEI 20
#define false 0
#define true (!false)
/* external routines of interest-
    allocate socket
    Set receive address
    set destination address
    set receive port
    set destination port
    deallocate socket
    send reset
    respond to arp
    generate arp
    GetDatFromIP
    GetDatFromTCP
    SendTCP
    SendIP
    SendEther
*/

#define max_xmit_size 1000
#define retrans_time(socket) (socket->rto)
#define force_update l_disp_con=NULL;
module far *input_TCB,far *output_TCB;

connection far *first_con=NULL,far *disp_con=NULL,far *l_disp_con=NULL;
windowptr output,tcpwindow=NULL;

#define NUM_CARDS 1
char *card_names[NUM_CARDS]={"WD8013"};

extern short sendWD(char far *Edest,short type,char buffers,buftype far *bufs);
short (*send_ether[NUM_CARDS])(char far *Edest,
                               short type,char buffers,
                               buftype far *buf)={sendWD};

extern short initWD(windowptr output,short port);
short (*init_ether[NUM_CARDS])(windowptr output,short port)={initWD};

extern void releaseWD(windowptr output,char far *buffer);
void (*release_ether[NUM_CARDS])(windowptr output,char far *buffer)={releaseWD};

extern short readWD(windowptr output,char far * far *buffer);
short (*read_ether[NUM_CARDS])(windowptr otuput,char far * far *buffer)={readWD};

extern void stopWD(void);
void (*stop_ether[NUM_CARDS])(void)={stopWD};



#define send_ether send_ether[card_type]
#define init_ether init_ether[card_type]
#define release_ether release_ether[card_type]
#define read_ether read_ether[card_type]
#define stop_ether stop_ether[card_type]
extern short ipcheck(IP_layer far *pktptr);
extern short tcpcheck(Pseudo_IP far *pseudo_ptr,
                      TCP_layer far *tcp_ptr,
                      short length,
                      char far *data,
                      short data_length);

char card_type;


char broad_ether[6]={0xff,0xff,0xff,0xff,0xff,0xff};
char MY_ether[6]={0xff,0xff,0xff,0xff,0xff,0xff};

short card_collisions=0;
short card_allignment=0;
short card_crcerrors=0;
short card_missedcount=0;
short card_packets_sent=0;
short card_received=0;
short card_busyxmt=0;
short max_retrans_delay=0;

void wake_output(void)
{
  output_TCB->status=0;
}

void wake_input(void)
{
  input_TCB->status=0;
}

void complete(ARP_prot far *arp_reply)
{
  /*This routines tries to complete the connection which it thinks
    I started... I got a reply after all */

  connection far *temp;
  temp=first_con;
  while (temp)
  {
    if (cmpIP(temp->I.source,arp_reply->A.target_prot_addr)&&
        cmpIP(temp->I.dest,arp_reply->A.source_prot_addr)&&
        temp->state&SNARP)
    {
      moveEther(temp->Edest,arp_reply->A.source_hard_addr);
      temp->state&=~SNARP;
      if (!(temp->state&(SNARP|SWSYN|SNSYN))) temp->state&=~SRESET;
      temp->connect_timer=0;  /*reset clock of connect */
      force_update;
    }
    temp=temp->next;
  }
}

void respond(ARP_prot far *request)
{
  char i;
  buftype buffers[1];
  ARP_layer far *arp_responce;
  connection far *temp;
  temp=first_con;
  while (temp)
  {
    if (cmpIP(temp->I.source,request->A.target_prot_addr) &&
        cmpIP(temp->I.dest,request->A.source_prot_addr)      ) break;

    temp=temp->next;
  }
  if (!temp)
  {
    temp=first_con;
    while (temp)
    {
      if (cmpIP(temp->I.source,request->A.target_prot_addr) &&
          cmpIP(temp->I.dest,"\0\0\0\0")      ) break;

      temp=temp->next;
    }
  }

  if (temp)
  {

    if (temp->state&SNARP)
    {
      temp->state&=~SNARP;
      moveEther(temp->Edest,request->M.source);
      if (!(temp->state&(SNARP|SNSYN|SWSYN))) temp->state&=~SRESET;
      temp->connect_timer=0;
    }
    arp_responce=Allocate(sizeof(ARP_layer));
    moveEther(arp_responce->source_hard_addr,MY_ether);
    moveEther(arp_responce->target_hard_addr,request->M.source);
    *(long far*)(arp_responce->target_prot_addr)=
        *(long far *)(request->A.source_prot_addr);
    *(long far*)(arp_responce->source_prot_addr)=
        *(long far *)(request->A.target_prot_addr);
    moveIP(temp->PI.dest,arp_responce->target_prot_addr);
    moveIP(temp->PI.source,arp_responce->source_prot_addr);
    arp_responce->hardlen=6;
    arp_responce->protlen=4;
    arp_responce->opcode=0x200;   /*reply*/   /*1=arp_responce RARP 3,4*/
    arp_responce->hardware=0x100;
    arp_responce->protocol=8;
    buffers[0].data=arp_responce;
    buffers[0].length=sizeof(ARP_layer);
    send_ether(request->M.source,ARP_type,1,buffers);
    Free(arp_responce);
    force_update;
  }
}

void request(connection far *socket)
{
  char i;
  buftype buffers[1];
  ARP_layer far *arpptr;
  arpptr=Allocate(sizeof(ARP_layer));
  for (i=0;i < 6;i++)
  {
    arpptr->target_hard_addr[i]=socket->Edest[i];
    arpptr->source_hard_addr[i]=MY_ether[i];
  }
  moveIP(arpptr->target_prot_addr,socket->I.dest);
  moveIP(arpptr->source_prot_addr,socket->I.source);
  arpptr->hardlen=6;
  arpptr->protlen=4;
  arpptr->opcode=0x100;   /*request*/   /*2=reply RARP 3,4*/
  arpptr->hardware=0x100;
  arpptr->protocol=8;
  buffers[0].data=arpptr;
  buffers[0].length=sizeof(ARP_layer);
  send_ether(broad_ether,ARP_type,1,buffers);
  Free(arpptr);
}


void do_arp(ARP_prot far *packet)
{
  if (packet->A.opcode==REQUEST)
  {
    respond(packet);
  }
  else if (packet->A.opcode==REPLY)
  {
    complete(packet);
  }
}

void Send_IP(connection far *socket,char buffers,buftype far *bufdata)
{
  char buf;
  short total_len;
  buftype far *newbuf;
  newbuf=Allocate(sizeof(buftype)*(buffers+1));
  newbuf[0].data=&socket->I;
  newbuf[0].length=sizeof(IP_layer);
  socket->I.ident=intswap(intswap(socket->I.ident)+1);
  total_len=newbuf[0].length;
  for (buf=0;buf<buffers;buf++)
  {
    newbuf[buf+1]=bufdata[buf];
    total_len+=bufdata[buf].length;
  }
  socket->I.length=intswap(total_len);
  socket->I.checksum=0;
  socket->I.checksum=ipcheck(&socket->I);
  send_ether(socket->Edest,IP_type,3,newbuf);
  Free(newbuf);

}

short Send_TCP(connection far *socket,char far *data,short length,long cur_seq)
{
  buftype bufs[2];

  if (cur_seq)
  {
    if (length>socket->Dwindow)
      return(false);
    socket->T.seq=longswap(cur_seq);

  }
  socket->flags&=~(socket->T.control=socket->flags);
  socket->PI.length=intswap(length+sizeof(TCP_layer));

  socket->T.checksum=0;
  socket->T.checksum=tcpcheck(&socket->PI,
                              &socket->T,
                              sizeof(TCP_layer),
                              data,
                              length);

  bufs[0].data=&socket->T;
  bufs[0].length=sizeof(TCP_layer);
  bufs[1].data=data;
  bufs[1].length=length;
  gettime(&socket->last_time_sent);
  socket->last_time_sent&=0x3fff;
  Send_IP(socket,2,bufs);
  socket->Dwindow=socket->MaxDwindow-((cur_seq+length)-socket->tail_seq);
  socket->status&=0x31;
  socket->status|=2;
  return(true);
}

void send_reset(TCP_prot far *packet)
{
  buftype reset_packet[1];
  Pseudo_IP PsIP;
  char ether_temp[6];
  char IP_temp[4];
  short TCP_temp;
  moveEther(ether_temp,packet->M.dest);
  moveEther(packet->M.dest,packet->M.source);
  moveEther(packet->M.source,ether_temp);
  moveIP(IP_temp,packet->I.source);
  moveIP(packet->I.source,packet->I.dest);
  moveIP(packet->I.dest,IP_temp);
  moveIP(PsIP.source,packet->I.source);
  moveIP(PsIP.dest,packet->I.dest);
  PsIP.prot=6;
  PsIP.zero=0;
  PsIP.zeros=0;
  packet->I.length=intswap(sizeof(TCP_layer)+sizeof(IP_layer));
  packet->I.checksum=0;
  packet->I.checksum=ipcheck(&packet->I);
  PsIP.length=intswap(sizeof(TCP_layer));
  TCP_temp=packet->T.source;
  packet->T.source=packet->T.dest;
  packet->T.dest=TCP_temp;
  packet->T.control=TRESET;
  packet->T.checksum=0;
  packet->T.checksum=tcpcheck(&PsIP,
                              &packet->T,
                              sizeof(TCP_layer),
                              NULL,
                              0);
  reset_packet[0].data=&packet->I;
  reset_packet[0].length=sizeof(TCP_layer)+sizeof(IP_layer);

  send_ether(packet->M.dest,IP_type,1,reset_packet);

}

connection far *find_socket(TCP_prot far *packet)
{
  connection far *current=first_con;
  while (current)
  {
    if (cmpIP(packet->I.source,current->I.dest)&&
        cmpIP(packet->I.dest,current->I.source)&&
        packet->T.source==current->T.dest&&
        packet->T.dest==current->T.source)
      break;
    current=current->next;
  }
  if (!current)
  {
    /*look for blank ports if there wasn't an actaully known
      connection*/
    current=first_con;
    while (current)
    {
      if (cmpIP("\0\0\0\0",current->I.dest)&&
          cmpIP(packet->I.dest,current->I.source)&&
          current->T.dest==0&&
          packet->T.dest==current->T.source)
        break;
      current=current->next;
    }
    if (current)
    {
      /*fill in the blanks for the connection addresses.  And, all
        is filled for a complete socket from now on.*/
      moveIP(current->I.dest,packet->I.source);
      moveIP(current->PI.dest,packet->I.source);
      current->T.dest=packet->T.source;
    }
  }

   /*If there wasn't a socket that matched, or the one that was
     found was closed, then send a reset so we don't connect */
  if (!current||(current->state&SCLOSED))
  {
    send_reset(packet);
    current=NULL;
  }
  return(current);
}

short add_data(connection far *socket,char far *data,
               unsigned long seq,short length)
{
  receive_block far *blocks=socket->blocks;
  char far *buffer;
  short idx=0;
  short avail;
  short newhead,dataidx;
  char newblock;
  unsigned long dif;

  /*check to see if the new block is before first block of data to
    be taken.  If so, remove part of block that is before the first
    block stored.  */
  if (seq<blocks[idx].begin_sequence)
  {
    /*check to see if all the data is before the first block,
      then check to see if the remaining data is already accounted for.*/
    length-=blocks[idx].begin_sequence-seq;    /*sub length of data before
                                                 the first buffer*/
    data+=blocks[idx].begin_sequence-seq;       /*increment the buffer*/
    seq=blocks[idx].begin_sequence;            /*justify the seq number*/

    if ((length<=0)||(length<blocks[idx].length)) /*if nothing left after
                                                    amputation, ack it, and
                                                    return false*/
    {
      socket->flags|=TACK;
      return(false);
    }
  }
  dif=seq-blocks[idx].begin_sequence;
  if ((dif+length) > socket->insize)
  {
    length-=(dif+length)-socket->insize;  /*remove end of data that
                                            will not fit in the buffer*/
  }
  if (length<=0) /*none of the data of this packet could fit in the
                   socket receive window.*/
    return(false);

  /* at this point, seq is within the range of storability somewhere
     within the receive buffer.  the data is of length less than the
     receive window size.  */


  if (dif+length<blocks[idx].length) /*check to see if end of block
                                       is within the first storage block*/
  {
    /* if it is encompased by the first block */
    socket->flags|=TACK;
    return(false);
  }
  if (dif<=blocks[idx].length)      /*check to see if the beginning of the
                                      block is within the first storage block*/
  {
    /*if it is, then shorten the length by the amount that the beginning is
      within the block.  update the buffer, and increase the offset of
      storage.*/
    length-=blocks[idx].length-dif;
    data+=blocks[idx].length-dif;
    dif+=blocks[idx].length-dif;
    if (length<=0)  /*if nothing was beyond the block, return, this
                      should never be gotten to... should have been
                      filtered out by the previous section that checks
                      to see if the block was entirely encompased.*/
    {
      socket->flags|=TACK;
      return(false);
    }
  }
  else
  {
    /* the block received was beyond what we could deal with.  In the future
    the block will still be stored, so that the hole may be tracked, and
    later filled.*/
    return(0);
  }


  /*store the data into the area pointed at by the current head.*/

store_data:
  buffer=socket->inbuf;
  newhead=socket->inhead+dif;
  if (newhead>=socket->insize)
    newhead-=socket->insize;
  for (dataidx=0;
       dataidx<length;
       dataidx++,newhead=((++newhead)<socket->insize)?newhead:0)
    buffer[newhead]=data[dataidx];
  blocks[0].length+=length;
  socket->T.ack=longswap(longswap(socket->T.ack)+length);
  socket->flags|=TACK;
  socket->T.window=intswap(socket->insize-socket->blocks[0].length);
  return(true);
}

short ack_socket(connection far *socket,unsigned long ack)
{
  /* if the ack is greater than what was sent, but is less than
     the head of the buffer, then during this time we have been
     retransmitting, and the other end has completed the hole
     within itself.  This is not an error, but it is confusing.*/
  unsigned long max;
  short data_avail;
  short new_tail;
  if (ack<(max=socket->tail_seq))
    return(false);
  data_avail=socket->outmaxbnd-socket->outtail;
  if (data_avail<0)
    data_avail+=socket->outsize;
  max+=data_avail;
  if (max<ack)
    return(false);
  /*we at this point have determined that the ack is within reasonable
    parameters. */
  new_tail=ack-socket->tail_seq;       /*get amount of data acked */
  new_tail+=socket->outtail;           /*add amount to old tail */
  if (new_tail>=socket->outsize)      /*if greater than buffer, must have*/
    new_tail-=socket->outsize;        /*  wrapped to beginning, so sub size*/

  socket->tail_seq=ack;               /*new sequence begin*/
  socket->outtail=new_tail;           /*new tail index*/

  /* if the boundries are not equal, then we were working on retransmitting,
     and for ease of figuring out what to do, we can just say well the other
     end has not completed what it was missing, and so we can move the
     boundry of to send to the max boundry.  If it needs more retransmittions
     then it will be reset to the tail, and we will go again, but in the
     meantime I am going to assume that everything is kosher.*/
  if (socket->outmaxbnd!=socket->outbound)
    socket->outbound=socket->outmaxbnd;
  if (socket->outtail==socket->outhead&&(socket->state&SCLOSED))
    socket->flags|=TFIN;
  return(true);
}

void do_calculation(connection far *socket)
{
  time_struc longtime;
  short rtt;
  short dif;

  gettime(&longtime);

  rtt = (longtime & 0x3fff) - socket->last_time_sent;
  if(rtt < 0)
    rtt+=0x4000;

  if(rtt > (max_retrans_delay << 1))
    rtt=socket->srtt;

  socket->srtt = ldiv(lmult(srtt_weight,socket->srtt)
            + lmult(100-srtt_weight,rtt),100) ;

  if(socket->srtt < 10)
    socket->srtt = 10;

  if(socket->srtt > (max_retrans_delay>>1))
    socket->srtt = (max_retrans_delay>>1);

  if ((dif=(socket->srtt-rtt)) < 0)
    socket->sdev =ldiv(lmult(sdev_weight,socket->sdev)
            +  lmult(100 - sdev_weight,-dif),100) ;
  else
    socket->sdev =ldiv(lmult(sdev_weight,socket->sdev)
            +  lmult(100 - sdev_weight, dif),100) ;

  if ( socket->sdev < (dif = (socket->srtt >> 2) ) )
    socket->sdev = dif;

  if ((socket->rto = socket->srtt + (2 * (socket->sdev+=2)))
      > max_retrans_delay)
    socket->rto = max_retrans_delay;
}


void input_task(void)
{
  /*this task sits in an endless loop reading the ethernet card for data
    and coordinates what connections the data is supposed to go to, and
    responds to other requests, such as ARPs */
  char far *buffer;
  connection far *current;

#define Ether ((Ether_layer far *)buffer)
  input_TCB=my_TCB;
  while (1)
  {
    if (read_ether(output,(char far * far *)&buffer))
    {
      if (Ether->type==ARP_type)
        do_arp((ARP_prot far *)buffer);
      else if (Ether->type==IP_type)
      {
#define buffer ((TCP_prot far *)buffer)
        switch(buffer->I.protocol)
        {
          case 1:  /* ICMP */
            break;
          case 6:
            current=find_socket(buffer);
            if (buffer->T.control&TFIN)
            {
              current->state|=SCLOSED;
            }
            if (buffer->T.control&TRESET)
            {
              if (!(current->state&SRESET))
                send_reset(buffer);
              current->state|=(SRESET|SNSYN|SWSYN);
            }
            if (buffer->T.control&TACK)
            {
              if (current->state&SNSYN)
              {
                long tempack=longswap(buffer->T.ack);
                if ((current->tail_seq+1)==tempack)
                {
                  current->state&=~SNSYN;
                  current->tail_seq=tempack;
                }
              }
              else
              {
                short outstand;
                /*ack data, move tail pointer, update tail seq...*/
                if (!ack_socket(current,longswap(buffer->T.ack)))
                {
                  send_reset(buffer);
                }
                /* if we have acked all data that has been transmitted,
                   then calculate parameters about the socket timing*/
                if (current->outtail==current->outmaxbnd)
                  do_calculation(current);
                outstand=current->outbound-current->outtail;
                if (outstand<0)
                  outstand+=current->outsize;
                current->Dwindow=intswap(buffer->T.window)-outstand;
                current->status&=0x31;
                current->status|=4;
                if (current->MaxDwindow<intswap(buffer->T.window))
                  current->MaxDwindow=intswap(buffer->T.window);
              }
            }
            if (buffer->T.control&TSYN)
            {
              if (current->state&SWSYN)
              {
                unsigned long returnack=longswap(buffer->T.seq)+1;
                current->T.ack=longswap(returnack);
                current->flags|=TACK;
                current->state&=~SWSYN;
                if (current->status&SERVER_SOCKET&&current->state&SNSYN)
                  current->flags|=TSYN;
                current->blocks[0].begin_sequence=returnack;
              }
              else
                send_reset(buffer);
            }
            if (!(buffer->T.control&TSYN))
              add_data(current,
                       buffer->T.data,
                       longswap(buffer->T.seq),
                       intswap(buffer->I.length)-
                          (sizeof(IP_layer)+sizeof(TCP_layer)));
            /*store received data */
            break;
        }
#undef buffer
      }
      release_ether(output,buffer);
    }
    else
      Relinquish(-1000L);
  }
}

unsigned long last_xmit;

void output_task(void)
{
  short sent,maxsent;
  connection far *current;
  unsigned long curtime;
  char isclosed;
  output_TCB=my_TCB;
  /*this task cycles through the sockets that are active, and handles the
    transmission of any data that they may have to transmit.  */
  while (1)
  {
    current=first_con;
    while (current)
    {
      if (current->state)
      {
        gettime(&curtime);
        if ((signed)(current->connect_timer-curtime)<0)
        {
          if (current->state&SNARP)
          {
            if (!cmpIP(current->I.dest,"\0\0\0\0"))
              request(current);
          }
          else
            if (current->state&SNSYN)
            {
              if (!cmpIP(current->I.dest,"\0\0\0\0"))
                current->flags|=TSYN;
            }
          gettime(&current->connect_timer);
          current->connect_timer+=CONNECT_DELAY;
        }
        if (current->state&SCLOSED)
        {
          if (current->outhead!=current->outtail)
          {
            isclosed=true;
            current->state&=~SCLOSED;
          }
        }
      }
      if (!(current->state&(SNSYN|SNARP|SWSYN)))
        current->state&=~SRESET;
      if ((current->outbound!=current->outhead)  && !current->state)
      {
        /* if we have new data that we can send it at this point.  We
           have no outstanding items, and the socket looks connected*/
        short length,offset;
        char far *buffer;
        unsigned long sequence;
        buffer=current->outbuf+current->outbound;
        length=current->outhead-current->outbound;
        if (length<0)
          length=current->outsize-current->outbound;
        if (length>current->Dwindow)
          length=current->Dwindow;
        if (length>max_xmit_size)
          length=max_xmit_size;
        if (length)
        {
          offset=current->outbound-current->outtail;
          if (offset<0)
            offset+=current->outsize;
          sequence=current->tail_seq+offset;
          buffer=current->outbuf+current->outbound;
          if (Send_TCP(current,buffer,length,sequence))
          {
            gettime(&last_xmit);
            current->outbound+=length;
            if (current->outbound==current->outsize)
              current->outbound=0;
            if ((sent=current->outbound-current->outtail)<0)
              sent+=current->outsize;
            if ((maxsent=current->outmaxbnd-current->outtail)<0)
              maxsent+=current->outsize;
            if (sent>maxsent)
              current->outmaxbnd=current->outbound;
          }
        }

      }
      gettime(&curtime);
      if ((curtime-last_xmit)>retrans_time(current) &&
           current->outtail!=current->outbound &&
           !current->state)
      {
        /*reset the sent-to pointer to the tail of the data as of yet
          unacknowledged*/
        current->outbound=current->outtail;
        current->Dwindow=current->MaxDwindow;
          current->status&=0x31;
          current->status|=6;
      }
      if (isclosed)
      {
        current->state|=SCLOSED;
        isclosed=false;
      }
      if (current->flags)
      {
        Send_TCP(current,NULL,0,0);
      }
      current=current->next;
    }

    Relinquish(-10L);
  }
}


short update;
long time,save_time;
void process_display()
{
  char cnt=0;
  short keyboard_input;
  short how_many=0;

  if(!disp_con)       /* if we are not displaying already */
  {                   /*   start up  tests */

    disp_con=first_con;
    if(!disp_con)return;

    if(!tcpwindow)tcpwindow=opendisplay(20,1,SOCKWID,SOCKHEI
                          ,NO_CURSOR|BORDER|NEWLINE,0x1f,0x1a,0,"Socket");
  }
  if(keypressed(tcpwindow))              /* check our keyboard input */
  {
    position(output,10,6);

    keyboard_input=readch(tcpwindow);
    switch(keyboard_input)
    {
      case 0:
        keyboard_input=readch(tcpwindow)|0x0100;
        break;
      case 'n':
      case 'N':
        if (disp_con->next)
        {
          disp_con=disp_con->next;
          l_disp_con=0L;
        }
        break;
      case 'F':
      case 'f':
        disp_con=first_con;
        l_disp_con=0L;
        break;
      case 'P':
      case 'p':
        if (disp_con->prior)
        {
          disp_con=disp_con->prior;
          l_disp_con=NULL;
        }
        break;
    }
  }
mine:
  gettime(&time);
  if (time-save_time>100)
  {
    save_time=time;
  }
  else
    return;

  position(output,0,0);

  displayln(output," Sockets:*5d       Collisions:*5d  \n"
                  ,how_many,card_collisions);

  displayln(output,"Received:*5d       Allignment:*5d  \n"
                  ,card_received,card_allignment);

  displayln(output,"CRC errs:*5d   Missed packets:*5d  \n"
                  ,card_crcerrors,card_missedcount);

/*  displayln(output,"    Sent:*5d       Found busy:*5d  \n"
                  ,card_sent_count,card_send_busy);*/


  position(tcpwindow,0,0);
  if(l_disp_con != disp_con)clr_display(tcpwindow,3);

  displayln(tcpwindow,"State:*s *s *s *s *s *s *s *s\n",
               (char far*)"",(char far*)"",
               (char far*)((disp_con->state&DLYWSYN  )?"DLYW":"    "),
               (char far*)((disp_con->state&SCLOSED  )?"CLOS":"    "),
               (char far*)((disp_con->state&SRESET   )?"RST" :"   " ),
               (char far*)((disp_con->state&SWSYN    )?"WSYN":"    "),
               (char far*)((disp_con->state&SNSYN    )?"NSYN":"    "),
               (char far*)((disp_con->state&SNARP    )?"NARP":"    "));

  if(l_disp_con != disp_con)
  {
    displayln(tcpwindow,"D IP:*n.*n.*n.*n TCP:*d\n",
                        disp_con->I.dest[0],disp_con->I.dest[1],
                        disp_con->I.dest[2],disp_con->I.dest[3],
                        intswap(disp_con->T.dest));

    displayln(tcpwindow,"S IP:*n.*n.*n.*n TCP:*d\n",
                        disp_con->I.source[0],disp_con->I.source[1],
                        disp_con->I.source[2],disp_con->I.source[3],
                        intswap(disp_con->T.source));
  }
  else
    position(tcpwindow,0,3);

  displayln(tcpwindow,"D window:*5d *n  MD window: *5d Flags:*n\n"
                          ,disp_con->Dwindow,disp_con->status,
                          disp_con->MaxDwindow,disp_con->flags);

  displayln(tcpwindow,"S window:*5d   SRTT:*4d *4d *4d\n"
                          ,intswap(disp_con->T.window)
                          ,disp_con->srtt
                          ,disp_con->sdev
                          ,disp_con->rto);



  if(l_disp_con != disp_con)
  {
    displayln(tcpwindow,"First:*H  Next :*H\n"
                            ,(long)first_con
                            ,(long)disp_con->next);
    displayln(tcpwindow,"  IAm:*H  Prior:*H\n",(long)disp_con,disp_con->prior);
  }
  else
    position(tcpwindow,0,7);

  displayln(tcpwindow,"Seq :*R  Ack:*H\n"
                          ,(long)disp_con->T.seq
                          ,(long)longswap(disp_con->T.ack));

  l_disp_con=disp_con;

/*-v-v-v-v-v-v-v-v-v-v-v-v-v-v- fix some day -v-v-v-v-v-v-v-v-v-v-*/
/*                       these need headings                      */

  position(tcpwindow,31, 1);
  displayln(tcpwindow,"*5d:XMT", disp_con->transmits);

  position(tcpwindow,31, 2);
  displayln(tcpwindow,"*5d:RXT", disp_con->retransmits);

  position(tcpwindow,31, 5);
  displayln(tcpwindow,"*5d:RCV", disp_con->p_received);

  position(tcpwindow,31, 6);
  displayln(tcpwindow,"*5d:DUP", disp_con->rec_duplicats );

  position(tcpwindow,31, 7);
  displayln(tcpwindow,"*5d:OVF", disp_con->over_flows );

/*-^-^-^-^-^-^-^-^-^-^-^-^-^-^- fix some day -^-^-^-^-^-^-^-^-^-^-*/

  position(tcpwindow,0,15);
  displayln(tcpwindow,"*H *H *h",disp_con->blocks[0].begin_sequence,
                                 disp_con->blocks[0].buf_begin,
                                 disp_con->blocks[0].length);

}

void display_task(void)
{
  /*this task does the display work for any connections that are up.
    it doesn't run often, unless configured to do so... guess I should
    put in a environment parameter to handle this */
  while (1)
  {
    process_display();
    Relinquish(0L);
  }
}

void find_type(char far *card)
{
  char idx;
  for (idx=0;idx<NUM_CARDS;idx++)
  {
    if (!stricmp(card,card_names[idx]))
    {
      card_type=idx;
      break;
    }
  }
}

extern void _openether(void);
extern void _sendether(void);
extern void _readether(void);
extern void _closeether(void);
extern void _flushether(void);


cleanup(void,disconnect,(void))
{
  Load_DS;
/*  shuttingdown=true;*/
  displayln(output," Closing Connections!");
/*  close_connections();*/
  stop_ether();
  Restore_DS;
}


void init(void)
{
  char far *card;
  short port;
  /*read the environment, and configure the card, and any other
  miscellaneous items that must be done for initialization*/
  /*register all routines that other tasks can have */
  card=Get_environ("Card_type");
  find_type(card);
   output=opendisplay(20,17,40,8
                    ,NO_CURSOR|BORDER|NEWLINE,0x1f,0x1a,0,"Ethernet");


  port=atoi(Get_environ("Card_port"));
  if (!init_ether(output,port))
  {
    disowndisplay(output);
    perish();
  }
  max_retrans_delay=atoi(Get_environ("maxretrans"));
  if (max_retrans_delay<1000)
    max_retrans_delay=1000;
  _openether();
  _sendether();
  _readether();
  _flushether();
  _closeether();
  _disconnect();
}

void main(void)
{
  init();
  /*fork three tasks, one to read the card, and one to handle output
    to the card, and one to handle displaying of the sockets.*/
  if (fork(CHILD))
    if (fork(CHILD))
      input_task();
    else
      output_task();
  else
  {
    change_priority(255);
    display_task();
  }
}
