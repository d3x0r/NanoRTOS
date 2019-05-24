/******************************************************************************
 File:    hiterm.c
 Purpose: A sample terminal emulation program to demonstrate the use of         hilib.asm
          hilib.asm and cp.asm with the HOSTESS i controller.
 Company: Comtrol Corporation
 Author:  Lori Slater, Craig Harrison
 Release: 1.00, 8-23-91 - Original release
*******************************************************************************
 Copyright 1991 Comtrol Corporation. All rights reserved. Subject to developers
 license agreement.
******************************************************************************/
#define atoi
#include <mod.h>
#include <video.h>
#include "hostess.h"
windowptr output;

main()
{
        int i;
        int pnum;
        int cnt;
        char buf[80];
        output=opendisplay(5,10,45,25,BORDER|NEWLINE,
                      WHITE|ON_BLUE,WHITE|ON_GREEN,YELLOW|ON_GREEN,
                      "Hostterm");

        displayln(output,"Serial Line Number (0-15): ");
        cnt=0;
        while (1)
        {
          while (!keypressed(output))
            Relinquish(0L);
          buf[cnt]=readch(output);
          displayln(output,"*c",buf[cnt]);
          if (buf[cnt]==13)
          {
            displayln(output,"\n");
            buf[cnt]=0;
            break;
          }
          cnt++;
        }
        pnum=atoi(buf);
        displayln(output,"Serial Line Number *d\nHit F10 to Quit\n",pnum);

        if (!hiopen(pnum))
        {
                displayln(output,"Can't open line %d\n",pnum);
                Relinquish(1L);
        }

        while(1)                                       /* infinite loop */
        {
          Relinquish(0L);
                /* attempt to read char from device and write to screen */
                if ((cnt = hiread(buf,80,pnum)) > 0)
                        for (i=0;i<cnt;i+=2)
                        {
                                if (buf[i] == 015)          /* CR = CRLF */
                                        displayln(output,"\n");
                                else if (buf[i] != 012)         /* not LF */
                                        displayln(output,"*c",buf[i]);
                        }

                /* attempt to read char from keyboard and write to device */
                if (keypressed(output))        /* if char waiting */
                {
                        buf[0] = readch(output);    /* read keybd char */
                        if ((buf[0] == 0) &&         /* 2 char key */
                        keypressed(output))
                        {
                                buf[1] = readch(output);    /* next */
                                if(buf[1] == 0x44)      /* F10 = quit */
                                        break;
                                else
                                        hiwrite(buf,2,pnum);
                        }
                        else if (buf[0] == 015)         /* CR = CRLF */
                        {
                                buf[1] = 012;
                                hiwrite(buf,2,pnum);
                        }
                        else if (buf[0] != 012)         /* not LF */
                        {
                                hiwrite(buf,1,pnum);
                        }
                }
        }
        hiclose(pnum);
        return(0);
}

