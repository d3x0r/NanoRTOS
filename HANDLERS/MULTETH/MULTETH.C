#include "_null.h"
#include "mod.h"

#include "video.h"

#include "npca.h"
#include "multthrd.h"

#include "ether.h"


#define intswap(a) (((unsigned)a>>8)|((unsigned)a<<8))

#define Free(a) Free(a);dalloc++;
#define Allocate(a) Allocate(a);alloc++;

window_type far *win;

/*this is the device driver front end to interfact the PCA driver with
  the ether DLL. Buffering is done completely by the ether driver.  The
  only real problem I have to worry about is packet integrety for CNI.*/

connection far * far *output;
short dalloc=0,alloc=0;

#define false 0
#define true (!false)

void terminate_watch(Node far *what,connection far *ether,short read
          /*add the parameter so that the work can be found when
            it is in a foriegn body */)
{
  threads_active far *temp=first_thread;
  char cnt=0;
            /*locate our thread entry*/
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
/*      Ether_term(ether,read);*/
      Relinquish(16L);
    }
    if (!(cnt&0xf))
    {

      check_outstanding(temp);
    }
    if (cnt==1)              /*if we have looped once and the operation*/
    {                        /*is still active, then slough it*/
      Slough_work(what);
    }
    Relinquish(0L);
    cnt++;
  }
}


void open_terminate_watch(Node far *what)
          /*First, find our thread control block, and then watch the
          list for terminated blocks.  If we find any, then cheerfully
          deallocate them.  */
{
  threads_active far *temp=first_thread;
  char cnt=0;
            /*locate our thread entry*/
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
    if (!(cnt&0xf))
    {

      check_outstanding(temp);
    }
    if (cnt==1)              /*if we have looped once and the operation*/
    {                        /*is still active, then slough it*/
      Slough_work(what);
    }
    Relinquish(0L);
    cnt++;
  }
}


void process_node(Node far *my_node,short me)
{
  unsigned short temp;
proc_again:
  if (!output[me>>1])
  {
    (long)output[me>>1]=1L;  /*flag it, that someone is nodeing on the problem*/

    if (fork(CHILD))
      open_terminate_watch(my_node);
    output[me>>1]
        =openether(my_node->Node.Info.Ether.SourceIP,
                   my_node->Node.Info.Ether.DestIP,
                   intswap(my_node->Node.Info.Ether.SourceTCP),
                   intswap(my_node->Node.Info.Ether.DestTCP),1,5000,5000,5);

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

  if (my_node->Node.Node_Address)
  {
    switch(my_node->Node.Rex&0xf)
    {
      case 0:/*read*/
             if (my_node->Tracking.Side==0)
               Slough_work(my_node);

             if (fork(CHILD))
               terminate_watch(my_node,output[me>>1],true);

             if (my_node->Node.XOptions&0x10)
               flushether(output[me>>1]);
             temp=0;
             while (temp!=0xffff&&temp<my_node->Node.Byte_count)
             {
               temp+=readether((char far*)my_node->Data+temp,
                                my_node->Node.Byte_count-temp,output[me>>1]);
             }
             destory();              /*kill all children*/

             if (temp==0)     /*End of file status*/
               my_node->Return.Status=0x0020;
             if (temp==0xffff)
             {
               Slough_work(my_node);
               if (closeether(output[me>>1]))   /*6/10/93 return added so we can detect non closed connections*/
               {
                 output[me>>1]=0L;
               }
               my_node->Return.Status=0x9000;
               temp=0;
               /*some kind of error occured*/
             }
             while (my_node->Tracking.Status&WORK_IN_MAIL)
               Relinquish(0L);
             my_node->Tracking.Side=ODD;
             my_node->Return.Byte_count=temp;
             my_node->Node.FPI++;
             my_node->Return.Opcode=COMPLETE;
             Relinquish(0L);        /*Wait one time to handle the possibility
                                      of an ack&data packet*/
             break;
      case 1:

             if (fork(CHILD))
               terminate_watch(my_node,output[me>>1],false);
             temp=0;
             while (temp!=0xffff&&temp<my_node->Node.Byte_count)
             {
               temp+=sendether((char far *)my_node->Data+temp,
                            my_node->Node.Byte_count-temp,output[me>>1]);
             }
             destory();

             if (temp==0xffff)   /*some kind of error occured*/
             {
               Slough_work(my_node);
               if (closeether(output[me>>1])) /*6/10/93 return added so we can detect non closed connections*/
                 output[me>>1]=0L;

               my_node->Return.Status=0x9000;
             }
             Free(my_node->Data); /* we are done with the buffer,so we can take
                                     care of destorying it. */
             my_node->Data=NULL;  /* zero data so it doesn't try to DMA
                                     the buffer to the modcomp! */
             my_node->Node.FPI++;
             my_node->Return.Opcode=COMPLETE;
             break;
      case 7:

             if (fork(CHILD))
               terminate_watch(my_node,output[me>>1],false);
             temp=1;
             if (temp==0xffff)   /*some kind of error occured*/
             {
               Slough_work(my_node);
               my_node->Return.Status=0x9000;
               if (closeether(output[me>>1])) /*6/10/93 return added so we can detect non closed connections*/
                 output[me>>1]=0L;
             }
             destory();

             my_node->Node.FPI++;
             my_node->Return.Opcode=COMPLETE;
             break;

      case 2:
      case 3:
      case 4:
      case 5:
      case 6:
      case 8:
             if (output[me>>1]->state)
             {
               Slough_work(my_node);
               my_node->Return.Status=0x9000;
               if (closeether(output[me>>1])) /*6/10/93 return added so we can detect non closed connections*/
                 output[me>>1]=0L;
             }
             my_node->Node.FPI++;
             my_node->Return.Opcode=COMPLETE;
             break;

    }
    if (!(my_node->Tracking.Status&WORK_ACK)) /*if not acknowledged */
    {
      my_node->Tracking.Status|=WORK_ACK;       /*acknowledge */
      while (my_node->Tracking.Status&WORK_IN_PROGRESS)
        Relinquish(0L);
      if (!(my_node->Tracking.Status&WORK_IN_MAIL))  /*if not in mail*/
      {
        my_node->Tracking.Status|=WORK_IN_MAIL;
        Export(my_node->Tracking.Source,my_node);
      }
      Relinquish(0L);
    }
  }
  else
  {
    my_node->Return.Opcode=COMPLETE;
    Export(my_node->Tracking.Source,my_node);
  }
  my_node=get_old_node(me);
  if (!my_node)
    deactivate(me);
  goto proc_again;
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
