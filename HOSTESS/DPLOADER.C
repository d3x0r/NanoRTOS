/* File:    dploader.c
   Company: Comtrol Corporation
   Author:  Craig Harrison
   Purpose: Reset controller and start Turbo Debugger Remote if needed.
            Load a binary file into the Smart HOSTESS dual port RAM and
            signal the COM processor to begin execution.   Strip off the
            first ? bytes of the binary file before downloading, if needed.
   Release: 1.00: February 4, 1988 - Craig Harrison - Original
            1.01: June 23, 1991 - Craig Harrison
                  1. Add support for HOSTESS i control register initialization.
            1.02: Aug  23, 1991 - Craig Harrison
                  1. Add reset option
                  2. Add Turbo Debugger Remote startup option
            1.03: 12-3-91 - Craig Harrison
                  Changed all "outportb" to "outp" for compatability with
                  Microsoft C.
*/
#define open
#define close
#define read
#define atoi
#include <mod.h>
#include <video.h>

windowptr output;

#define EN_RAM    0x00                      /* Enable DPRAM */
#define HiCR0     0x01                      /* Select Control Reg 0 */
#define HiCR1     0x02                      /* Select Control Reg 1 */
#define DIS_RAM   0x04                      /* Disable DPRAM */
#define HiCR2     0x08                      /* Select Control Reg 2 */
#define HiCR3     0x10                      /* Select Control Reg 3 */

/* HOSTESSi CR2 values for various window sizes and offsets */
char w16k[8] = {0x38,0x39,0x3a,0x3b,0x3c,0x3d,0x3e,0x3f};  /* 16K windows */
char w32k[4] = {0x30,0x32,0x34,0x36};            /* 32K windows */
char w64k[2] = {0x20,0x24};                      /* 64K windows */

/* HOSTESSi CR3 values for IRQ 0-15, 0xFF means no IRQ is set */
char cr4[16] = {0xFF,0xFF,0xFF,0,1,2,0xFF,0xFF,0xFF,3,4,5,6,0xFF,0xFF,7};

void cr_init(int io,unsigned far *flag_p);
unsigned far *get_dpbase();
int get_iobase();
void download(int io,unsigned far *flag_p);
void reset_board(int io,unsigned far *flag_p);
int invoke_tdrem(int io,unsigned far *flag_p);

short PORT=0x218;
unsigned far *MEM=0xD0000000;
short page_size;
unsigned char page;
short waiting;
#define false 0
#define true (!false)
char done=false;

public(void,hold_IO,(void))
{
  Load_DS;
  waiting++;
  while (done==1)
    Relinquish(0L);
  waiting--;
  if (done==2)
    perish();
  Restore_DS;
}

void main()
{
   done=0;
   _hold_IO();
   output=opendisplay(15,10,45,5,BORDER|NEWLINE,
                      WHITE|ON_BLUE,WHITE|ON_GREEN,YELLOW|ON_GREEN,
                      "DP_Loader Status");
   PORT=atoi(Get_environ("Port"));
   page=atoi(Get_environ("page"));
   switch(page&0xf)
   {
     case 0x0:page_size=4;
              break;
     case 0x4:page_size=1;
              break;
     case 0x8:page_size=2;
              break;
     case 0xc:page_size=1;
              break;
     default:
       displayln(output,"Page must Lie on a 16K boundry (#D0,#D4,#D8,#DC)...\n");
       displayln(output,"#*n is not such a number...",page);
       perish();
       break;
   }
   MEM=(unsigned far *)((long)page<<24);
   reset_board(PORT,MEM); /* reset board*/
   download(PORT,MEM);    /* download & start ctrlpgm */
   done=true;
   while (waiting) Relinquish(0L);
   perish();
}

void outp(short port,char value)
{
  asm mov dx,port;
  asm mov al,value;
  asm out dx,al;
}

/*****************************************************************************/
/* Purpose: HOSTESS i control register initialization                        */
/* Formal parms: None                                                        */
/* Parms modified: None                                                      */
/* Returns: Nothing                                                          */

void cr_init(int io,unsigned far *flag_p)
{
   outp(io+1,HiCR0 | DIS_RAM);              /* access CR0, PC mode */
   outp(io,0x80);                           /* below 1 Meg */
   outp(io+1,HiCR1 | DIS_RAM);              /* access CR1 */
   outp(io,page>>2);                        /* below 1 Meg RAM address */
   outp(io+1,HiCR2 | DIS_RAM);              /* access CR2 */
   switch(page_size)
   {
    case 1:outp(io,w16k[4]);
           break;
    case 2:outp(io,w32k[2]);
           break;
    case 4:outp(io,w64k[1]);
           break;
   }
// outp(io,w64k[1]);                        /* 64K upper window */
   outp(io+1,HiCR3 | DIS_RAM);              /* access CR3 */
   outp(io,cr4[0]);                         /* no IRQ set */
   outp(io+1,EN_RAM);                       /* enable DPRAM */
   if ((*flag_p&0xff00)==0xff00)
   {
     outp(io+1,HiCR0);                        /* access CR0, PC mode */
     outp(io,0);                              /* below 1 Meg */
   }
}


/*****************************************************************************/
/* Purpose: Reset board                                                      */
/* Entry: Nothing                                                            */
/* Exit:  Nothing                                                            */

void reset_board(int io,unsigned far *flag_p)
{
   int i;
   unsigned rec_flag;

   displayln(output,"   Dual Port Base Address = *H\n",(long)flag_p);
   displayln(output,"   I/O Base Address = *hH\n",io);
   outp(io+3,0);
   delay(100);
   outp(io+3,0xff);
   displayln(output,"   Waiting for reset to complete\n");
   delay(500);
   cr_init(io,flag_p);
   for(i = 0;i < 21;i++)
   {
      displayln(output,".");
      delay(1000);
      if(*flag_p == 0x55aa)
         break;
   }
   if(((rec_flag = *flag_p) != 0x55aa) && i >= 21)
   {
      displayln(output,"   Reset failed, interaction flag = *hH\n",rec_flag);
      done=2;
      while (waiting) Relinquish(0L);
      perish();
   }
}

/*****************************************************************************/
/* Purpose: Read a binary file into dual port memory, signal the COM         */
/*          Processor to start execution, verify execution started.          */
/* Entry:        flag_p - Pointer to interaction flag                        */
/*               io     - I/O base address                                  */
/* Returns: Nothing                                                          */
/* Notes: The file to be downloaded must:                                    */
/*        Have "org 0"                                                       */
/*        Have 80h bytes preceeding the code                                 */
/*        Write the value 55AAh to interaction flag when it starts execution */

void download(int io,unsigned far *flag_p)
{
   char page_count,page_place;
   unsigned short size,bytesread;
   char far *ram_p;
   char fname[13];                          /* Name of file to download */
   unsigned wait;                           /* Do nothing loop counter */
   unsigned rec_flag;                       /* Received copy of interact flag*/
   unsigned long strip;                     /* Number bytes to strip off file*/
   char buffer[256];                        /* Buffer for data to be xfered
                                               to dual port RAM */
   long int nbytes;                         /* Number of bytes downloaded */
   int retry,                               /* Loop control flag */
       i,
       f_h;                                 /* File handle */

   /* Check interaction flag */
   if((rec_flag = *flag_p) != 0x55aa)
   {
      displayln(output,"   Bad interaction flag = *hH before download\n",rec_flag);
      done=2;
      while (waiting)
        Relinquish(0L);
      perish();
   }

   /* Get file name and open file for read */
   retry = 1;
   while(retry)
   {
      if((f_h = open("cpa.com",0)) == -1)
         displayln(output,"   Unable to open file CPA.COM, try again.\n");
      else
         retry = 0;
   }
   /* Download file from disk to dual port RAM */

   *flag_p = 0xAA55;                        /* Write to interaction flag */
   displayln(output,"   Downloading Control Program ... \n");
   nbytes = 0;
   ram_p = (char far *)flag_p + 0x80;
   page_count=0;
   page_place=0;
   size=16384*page_size-0x80;
   while(1)
   {
      if (!(bytesread=read(ram_p,size,f_h)))
        break;
      ram_p+=bytesread;
      nbytes+=bytesread;
      page_count++;
      page_place++;
      if (page_count==page_size)
      {
        page_count=0;
        outp(io+1,HiCR2);                    /* access CR2 */
        switch(page_size)
        {
          case 1:outp(io,w16k[4+page_place]);
                 ram_p=0;
                 size=16384;
                 break;
          case 2:outp(io,w32k[2+page_place]);
                 ram_p=0;
                 size=32768;
                 break;
          case 4:displayln(output,"Program bigger than card's memory!");
                 done=2;
                 while (waiting)
                   Relinquish(0L);
                 perish();
                 break;
        }
      }
   }
    outp(io+1,HiCR2);                    /* access CR2 */
    switch(page_size)
    {
      case 1:outp(io,w16k[4]);
             break;
      case 2:outp(io,w32k[2]);
             break;
    }

   close(f_h);

   if(nbytes >= 0xFFFF)
   {  /* File to big */
      displayln(output,"   File too large to fit in one segment\n");
      done=2;
      while(waiting)
        Relinquish(0L);
      perish();
   }
   else
   {  /* Download successful, interrupt COM Processor if TDREM not invoked */
      displayln(output,"   *H bytes downloaded successfully\n",nbytes);
      outp(io + 2,0);               /* Interrupt COM Processor */
      displayln(output,"   COM processor interrupted to start control program\n");
      for(wait = 0;wait < 10;wait++)    /* wait for control program to */
      {                                 /*   restore interaction flag */
         delay(400);
         if(*flag_p == 0x55aa)
            break;
      }
      if((wait >= 10) && ((rec_flag = *flag_p) != 0x55aa))
      {
         displayln(output,"   Control program failed to start, interact flag = *hH\n",
                 rec_flag);
         done=2;
         while (waiting)
           Relinquish(0L);
         perish();

      }
      else
      {
         displayln(output,"   Control program started execution\n");
      }
   }
}
