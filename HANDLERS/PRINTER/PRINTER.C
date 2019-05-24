/*#define DEBUG*/
#include <mod.h>
#include <npca.h>
#include "multthrd.h"
#include <video.h>
#include "iorout.h"

windowptr output;

#define false 0
#define true (!false)

/*this is the device driver front end to interfact the PCA driver with
  the ether DLL. Buffering is done completely by the ether driver.  The
  only real problem I have to worry about is packet integrety for CNI.*/


typedef struct message_holder
{
  unsigned short far *message;
  short length;
  struct message_holder far *next;
} message_holder;


void terminate_watch(Node far *what
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
      Relinquish(16L);
    }
    if (!(cnt&0xf))
      check_outstanding(temp);
    if (cnt==1)              /*if we have looped once and the operation*/
    {                        /*is still active, then slough it*/
      if (!(what->Tracking.Status&(SLOUGHED|WORK_ACK|WORK_IN_MAIL)))
      {
        what->Return.Opcode=SLOUGH;
        what->Tracking.Status|=SLOUGHED|WORK_IN_MAIL;
        Export(what->Tracking.Source,what);
      }
    }
    Relinquish(0L);
    cnt++;
  }
}

void Record_Print(Data_process Routine,Node far *what)
{
  char ch;
  short chars_to_go;
  asm int 3;
  do
  {
    ch=Routine(what,&chars_to_go);
    displayln(output,"*c",ch);
  }
  while (chars_to_go);
}

void process_node(Node far *what,short me)
{
  short temp;
  message_holder far *tmessage;
proc_again:
  if (what->Node.Node_Address)
  {
    displayln(output,"Processing: *n on *n Node:*H\n",
                     what->Node.Rex,
                     what->Tracking.Side,
                     what->Node.Node_Address);
    switch(what->Node.Rex&0xf)
    {
      case READ:/*read*/
             if (what->Tracking.Side==EVEN&&
                 !(what->Tracking.Status&(SLOUGHED|WORK_IN_MAIL)))
             {
               what->Return.Opcode=SLOUGH;
               what->Tracking.Status|=SLOUGHED|WORK_IN_MAIL;
               Export(what->Tracking.Source,what);
               what->Tracking.Side=ODD;
             }
             if (fork(CHILD))
               terminate_watch(what);
             else
             {
               if (what->Node.XOptions&0x10)
               {
                 displayln(output,"Flushing\n");
               }
             }
             destory();
             Free(what->Data);
             what->Data=0L;
             what->Return.Byte_count=0;
             what->Node.FPI++;
             what->Return.Opcode=COMPLETE;
             break;
      case WRITE:
             {
              /*inser specific code to carry out the write*/
              /*temp=byte_count*/
              Data_process far Routine;
             switch(what->Node.Options&(UFTOPTSTD|UFTOPTBI))
             {
               case UFTOPTSTD:
                 Routine=Get_Data_SA;
                 break;
               case UFTOPTBI:
                 Routine=Get_Data_NSB;
                 break;
               case UFTOPTBI|UFTOPTSTD:
                 Routine=Get_Data_SB;
                 break;
               default:
                 Routine=Get_Data_NSA;
                 break;
             }

             Record_Print(Routine,what);
             what->Data=0L;  /* zero data so it doesn't try to DMA or free
                                    the buffer to the modcomp! */
             what->Node.FPI++;
             what->Return.Opcode=COMPLETE;
             }
             break;
      case REWIND:
             Record_Print(Get_Rewind,what);
             what->Node.FPI++;
             what->Return.Opcode=COMPLETE;
             break;
      case BACK_RECORD:
             Record_Print(Get_Back_Record,what);
             what->Node.FPI++;
             what->Return.Opcode=COMPLETE;
             break;
      case BACK_FILE:
             Record_Print(Get_Back_File,what);
             what->Node.FPI++;
             what->Return.Opcode=COMPLETE;
             break;
      case ADVANCE_FILE:
             Record_Print(Get_Adv_File,what);
             what->Node.FPI++;
             what->Return.Opcode=COMPLETE;
             break;
      case ADVANCE_RECORD:
             Record_Print(Get_Adv_Record,what);
             what->Node.FPI++;
             what->Return.Opcode=COMPLETE;
             break;
      case WRITE_EOF:
             Record_Print(Get_EOF,what);
             what->Node.FPI++;
             what->Return.Opcode=COMPLETE;
             break;
      case HOME:
             if (what->Node.XOptions&UFTXOPDCL)
               displayln(output,"Disconnect Line\n");
             Record_Print(Get_Home,what);
             break;

    }
    if (!(what->Tracking.Status&WORK_ACK))
    {
      what->Tracking.Status|=WORK_ACK;
      if (!(what->Tracking.Status&WORK_IN_MAIL))  /*if not in mail */
      {
        what->Tracking.Status|=WORK_IN_MAIL;
        Export(what->Tracking.Source,what);
      }
    }
  }
  else
  {
    what->Return.Opcode=COMPLETE;
    Export(what->Tracking.Source,what);
  }
  asm int 3;
  what=get_old_node(me);
  if (!what)
    deactivate(me);
  goto proc_again;
}

void init_local_structs(void)
{
  short i;
  output=opendisplay(4,5,50,10,BORDER|NO_CURSOR|NEWLINE,
                     0x1f,0x1f,0x2e,"Printer");

}


