#include "mod.h"
#include "comm.h"
#include "video.h"

windowptr output;
com_socket far *comchan;

#define UNKNOWN 0
#define NOCABLE 3
#define PCCABLE 1
#define UPSCABLE 2
#define OK 1
#define FAIL 2
#define LOWBATTERY 3
#define BATTERYFAIL 4

typedef struct comchannel
{
  short port;
  char interupt;
  char word;
  char stop;
  char parity;
} comchannel;

comchannel coms[2]={{0x3f8,4,'8','1','N'},
                    {0x2f8,3,'8','1','N'}};

#define false 0
#define true (!false)
char holding_status=UNKNOWN;
char ACState=UNKNOWN;
#define PCCABLE1   0
#define PCCABLE2   1
#define UPSCABLE1  2
#define UPSCABLE2  3
#define UPSCABLEOK 4
#define BATFAIL    5
#define POWEROK    6
#define BATLOW     7
#define ACFAIL     8

char *messages[]={"UPS CONTROL CABLE NOT         ",
                  "CONNECTED TO PC               ",
                  "UPS CONTROL CABLE NOT         ",
                  "CONNECTED TO UPS              ",
                  "UPS CABLE OKAY                ",
                  "BATTERY FAIL                  ",
                  "AC POWER OK                   ",
                  "BATTERY LOW                   ",
                  "AC POWER FAIL                 "};


void print_state(char  string,char attr)
{
  setattr(output,attr);
  output->borattr=(attr&0x70)|WHITE;
  moddisplay(output,SELECT_WINDOW,RESTORE_WINDOW,END_MOD);
  displayln(output,"*s",(char far *)messages[string]);
}

main()
{
    char com_channel;
  output=opendisplay(30,1,30,5,BORDER|NEWLINE|NO_CURSOR,
                     WHITE|ON_BLUE,YELLOW|ON_BLUE,WHITE|ON_GREEN,
                     "UPS Monitor");
  com_channel=*Get_environ("COM");
  com_channel-=0x31;
  if (com_channel>1||com_channel<0)
  {
    displayln(output,"*c is an invalid COM channel to be on.\n",
                  com_channel+0x31);
    displayln(output,"Set Environment variable COM to 1 or 2\n");
    perish();
  }

  comopen(coms[com_channel].port,0/*baud*/,
          coms[com_channel].word,coms[com_channel].stop,
          coms[com_channel].parity,0/*Inbuf*/,0/*outbuf*/,
          coms[com_channel].interupt,0/*opts*/,&comchan);
  comsetmodem(comchan,RTS);
  while (1)
  {
    char Cable2PC=true,Cable2UPS=true;
    char status=comchan->Modem_Status;
/*
  ÚÄÄÄ¿              ÚÄÄÄ¿
  ³IPC³ ÛÛÛ ÆÍÍÍÍÍÍÍµ³UPS³
  ÀÄÄÄÙ  ^     ^     ÀÄÄÄÙ
 MonitorÄÙ     ÀÄÄ Cable        */
    Relinquish(-2000L);
    status=comchan->Modem_Status;
/*    displayln(output,"Status:*n *s *s *s\n",
      status,
      (char far *)((status&DCD)?"DCD":"   "),
      (char far *)((status&DSR)?"DSR":"   "),
      (char far *)((status&CTS)?"CTS":"   "));*/
    if ((status&(DCD|DSR|CTS))==(DCD|DSR|CTS))
      Cable2UPS=false;
    else
      if (!(status&(CTS|DCD)))
        Cable2PC=false;

    if (!Cable2PC)
    {
      if (holding_status!=PCCABLE)
      {
        print_state(PCCABLE1,YELLOW|ON_BLUE|BLINK);
        print_state(PCCABLE2,YELLOW|ON_BLUE|BLINK);
        holding_status=PCCABLE;
      }
    }
    else
      if (!Cable2UPS)
      {
        if (holding_status!=UPSCABLE)
        {
          print_state(UPSCABLE1,YELLOW|ON_BLUE|BLINK);
          print_state(UPSCABLE2,YELLOW|ON_BLUE|BLINK);
          holding_status=UPSCABLE;
        }
      }
      else
      {
        if (holding_status!=NOCABLE)
        {
          print_state(UPSCABLEOK,WHITE|ON_GREEN);
          holding_status=NOCABLE;
          ACState=UNKNOWN;
        }
        if (!(status&DCD)&&
             (status&CTS))
        {
          if (!(status&DSR))
          {
            if (ACState!=BATTERYFAIL)
            {
              print_state(BATFAIL,WHITE|ON_MAGENTA|BLINK);
              ACState=BATTERYFAIL;
            }
          }
          else
            if (ACState!=OK)
            {
              ACState=OK;
              print_state(POWEROK,WHITE|ON_GREEN);
            }
        }
        else
        {
          if (!(status&DSR))
          {
            if (ACState!=LOWBATTERY)
            {
              print_state(BATLOW,LT_RED|ON_GREY);
              ACState=LOWBATTERY;

            }
            ACState=UNKNOWN;
            holding_status=UNKNOWN;
            {
              routine_ptr far *routine;
              routine=Inquire_begin(ROUTINE);
              while (routine)
              {
                if (routine->name[0]=='!')
                {
                  routine->procedure();
                }
                routine=routine->next;
              }
            }
            Relinquish(-400000L);
            comsetmodem(comchan,RTS|DTR);
            Relinquish(-10000L);
            comsetmodem(comchan,RTS);
          }
          else
            if (ACState!=FAIL)
            {
              print_state(ACFAIL,WHITE|ON_RED|BLINK);
              ACState=FAIL;
            }
        }
      }
  }
}
