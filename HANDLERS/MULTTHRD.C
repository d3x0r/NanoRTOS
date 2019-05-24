#define COMMON

/*#define DEBUG*/

#define extern_def

#include <mod.h>
#include "npca.h"
#include "multthrd.h"

#define FALSE 0
#define TRUE (!FALSE)

#ifdef DEBUG
#include <video.h>

static window_type far *output;
#endif

threads_active far *first_thread=0L;

extern void process_node(Node far *what,short my_id);
extern void init_local_structs(void);

void Slough_work(Node far *what)
{
  if (!(what->Tracking.Status&(SLOUGHED|WORK_ACK|WORK_IN_MAIL)))
  {
    what->Return.Opcode=SLOUGH;
    what->Tracking.Status|=SLOUGHED|WORK_IN_MAIL;       /*stored*/
#ifdef DEBUG
    displayln(output,"ExS:*h",(short)what->Node.Node_Address);
#endif
    Export(what->Tracking.Source,what);
  }
  while (what->Tracking.Status&(WORK_IN_MAIL|WORK_IN_PROGRESS))
    Relinquish(0L);
}


void store_node(Node far *what,threads_active far *where)
{
  /*this procedure sends a slough of the Node to the PCA driver, and
    stores the Node in the Node queue attachted to the active thread
    that we have here */
  Node_holder far *temp=where->Nodeq;
  where->status|=STORING;
#ifdef DEBUG
  displayln(output,"Str:*h",(short)((long)what>>16));
#endif
  if (temp)
  {
    while (temp->next)
      temp=temp->next;
    while (!(temp->next=Allocate(sizeof(Node_holder))))
      Relinquish(0L);

    temp=temp->next;
  }
  else
  {
    while (!(temp=Allocate(sizeof(Node_holder))))
      Relinquish(0L);
    where->Nodeq=temp;
  }
  temp->next=0L;
  temp->Node=what;
  Slough_work(what);
  where->status&=~STORING;
/*From this point forward the node could be terminated...*/
}


void check_outstanding(threads_active far *where)
{
  Node_holder far *tempNode;
  Node_holder far *prior;
  Node far *check;
  tempNode=where->Nodeq;
  prior=0L;
  while (tempNode)
  {

    check=tempNode->Node;
    if (!check->Node.Node_Address)  /*if it was terminated, then free it
                                 and unchain it from the Node queue*/
    {

#ifdef DEBUG
  displayln(output,"Trm");
#endif
      if (prior)
      {
        prior->next=tempNode->next;
        Free(tempNode);
        tempNode=prior->next;
      }
      else
      {
        where->Nodeq=tempNode->next;
        Free(tempNode);
        tempNode=where->Nodeq;
      }
      while (check->Tracking.Status&(WORK_IN_MAIL|WORK_IN_PROGRESS))
        Relinquish(0L);

      if (!(check->Return.Status&WORK_ACK))
      {
        check->Return.Opcode=COMPLETE;  /*node is complete*/
        if (!(check->Return.Status&WORK_IN_MAIL))
        {

#ifdef DEBUG
  displayln(output,"Ext");
#endif

          check->Return.Status|=WORK_IN_MAIL|WORK_ACK;
          Export(check->Tracking.Source,check);
        }
      }
    }
    else
    {

/*#ifdef DEBUG
  displayln(output,"Ntr");
#endif*/

      prior=tempNode;
      tempNode=tempNode->next;
    }
  }
}


Node far *get_old_node(short id)
{
  /*this procedure retrieves previously stored Node from the queue*/
  threads_active far *temp=first_thread;
  Node_holder far *Nodeq;
  Node far *Node;
  while (temp)
  {
    if (temp->id==id)
    {

      while (temp->status&STORING)
        Relinquish(0L);
      while ((Nodeq=temp->Nodeq))
      {
        Node=Nodeq->Node;
#ifdef DEBUG
  displayln(output,"Nxt:*h",(short)((long)Node>>16));
#endif
        temp->Nodeq=Nodeq->next;
        Free(Nodeq);
        if (Node->Node.Node_Address)
          return(Node);
        else
        {
/*          if (Node->Data)
            Free(Node->Data);
          Free(Node);*/
#ifdef DEBUG
  displayln(output,"ExO");
#endif
          if (!(Node->Tracking.Status&WORK_ACK))
          {
            Node->Return.Opcode=COMPLETE;
            if (!(Node->Tracking.Status&WORK_IN_MAIL))
            {
              Node->Tracking.Status|=WORK_IN_MAIL|WORK_ACK;
          Export(Node->Tracking.Source,Node);  /*6/18/93 Fixed, we should not
                                                 just deallocate a dead node,
                                                 becuase we do not know how
                                                 to do it properly.  Send it
                                                 back to the person that gave
                                                 it to us.*/
            }
          }
        }
      }
      return(0L);
    }
    temp=temp->next;
  }
  return(0L);
}

threads_active far *thread_active(short which,Node far *what)
{
  /*This procedure returns true is the thread has already been activated,
    and false if it hasn't.  If it hasn't been activated, then it is
    implicitly activated via this routine.  So, if it is called with an
    inactive thread, then the thread is added to the list of actives,
    so a false return actually means that it wasn't active, but is now.*/
  threads_active far *temp=first_thread,far *new_thread;
  char sloughed=FALSE;

  while (!(new_thread=Allocate(sizeof(threads_active))))
  {
    if (!sloughed)
    {
      sloughed=TRUE;
      Slough_work(what);
/*The node could be terminated from this point forward*/
    }
    Relinquish(0L);
  }
  if (temp)
  {
    while (temp->next)           /*this loop checks all but the last one*/
    {
      if (temp->id==which)
      {

#ifdef DEBUG
  displayln(output,"Nx1");
#endif
        Free(new_thread);
        return(temp);
      }
      temp=temp->next;
    }
    if (temp->id==which)  /*check last one*/
    {

#ifdef DEBUG
  displayln(output,"Nx2");
#endif
      Free(new_thread);
      return(temp);
    }
        /* not before active, so activate*/
    temp->next=new_thread;
    temp=temp->next;

  }
  else
  {
    first_thread=new_thread;
    temp=first_thread;
  }
  temp->Nodeq=0L;
  temp->status=0;
  temp->next=0L;
  temp->id=which;
  return(0L);                          /*return false*/
}

void deactivate(short what)
{
  threads_active far *temp=first_thread;
  threads_active far *prior=0L;
  while (temp)
  {
    if (temp->id==what)
    {
      if (temp->Nodeq)  /*there was still Node*/
        return;
      if (prior)
        prior->next=temp->next;
      else
        first_thread=temp->next;
      Free(temp);
      perish();
    }
    prior=temp;
    temp=temp->next;
  }
  Exit(5);  /* We have a problem !*/
}

main()
{
  Node far *received;
  Node far *first_mark;
  threads_active far *alive;
  short i;
  short temp;


#ifdef DEBUG
  output=opendisplay(20,15,40,10,BORDER,0x1f,0x1f,0x2f,"MultThrd");
#endif

  init_local_structs();

  while (1==1)
  {
    received=Import(device_name);
#ifdef DEBUG
    displayln(output,"Rcv:*h[*h]",(short)received->Node.Node_Address,
                                 (short)((long)received>>16));
#endif
    if (!(alive=thread_active(received->Node.Channel,received)))
             /*check to see if active*/
    {
      if (fork(CHILD))
      {
        process_node(received,received->Node.Channel);
      }
    }
    else
      store_node(received,alive);
  }
}
