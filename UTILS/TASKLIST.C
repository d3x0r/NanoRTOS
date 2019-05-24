#include "mod.h"
#include "video.h"

#define GEN_WIDTH 1			/*one for the generation level of the program*/
#define NAME_WIDTH 8		/*eight for the name */
#define STATUS_WIDTH 5	/*one for a space and 4 for the LSW of task status*/
#define MAX_WIDTH		5		/*one for a space and 4 for the number */
#define PRIORITY_WIDTH 3 /*one for a space and 2 for the priority*/
#define TIME_WIDTH 5		/*one for a space and 4 for the time used display*/
#define BASE_CS_WIDTH 5
#define CS_IP_WIDTH 10

#define IDLE_COLOR 0x1f
#define STACK_ERROR 0xfc


#define WINDOW_WIDTH GEN_WIDTH+NAME_WIDTH+STATUS_WIDTH+		 \
										 PRIORITY_WIDTH+TIME_WIDTH+MAX_WIDTH+1

#define CODE_OPT_WID BASE_CS_WIDTH+CS_IP_WIDTH


#define false 0
#define true !false

#define LINES lines
#define DEF_LINES 30

short lines;

module far *current,far *first,far *gencnt;

window_type far *output;


unsigned long timer;

main()
{
	short i;
	long misc_count;
	char code_opt=false;
	unsigned short test_time,_time;
	short list_count;
	char far *lineparam=Get_environ("lines");
	char far *param;
	if (lineparam)
	{
		lines=atoi(lineparam);
		if (lines<=0)
			lines=DEF_LINES;
	}
	else
		lines=DEF_LINES;
	param=Get_environ("params");
	if (param[0]=='C'||param[0]=='c')
		code_opt=true;

	output=opendisplay(1,1,WINDOW_WIDTH+((code_opt)?CODE_OPT_WID:0)
										 ,LINES,NO_CURSOR|BORDER|NEWLINE
										 ,IDLE_COLOR,0x1c,0,"Task_List");

	first=Inquire_begin(MODULE);			/* get first module from OS */

	while (true)
	{
		if (keypressed(output)&&(readch(output)==27))
			Exit(2);
		misc_count += 100;
		Relinquish(-100L);
		gettime(&timer);
		if (list_count=((test_time=timer/1000) - _time ))
		{
			if(list_count <= 0)list_count=1;
			misc_count/=list_count;

			position(output,0,0);

			displayln(output,"comp. scans	 %5d\n",misc_count);
			displayln(output,"Gen/Name	Stat STK PR CPU ");

			if (code_opt)
				displayln(output," CODE		CS:IP");

			_time=test_time;

			current=first;
			list_count=2;
			do
			{
				displayln(output,"\n");
				misc_count=0;
				gencnt=current;
				while ((gencnt=gencnt->parent)!=0)
					misc_count++;
				display(output,0x30+misc_count);
				for (i=0;current->name[i]&&(i<NAME_WIDTH);i++)
				{
					display(output,current->name[i]);
				}

				for (;i<NAME_WIDTH;i++) display(output,' ');

				displayln(output," %h",(short)current->status);

				if (current->max_size>current->stack_size)
				{
					setattr(output,STACK_ERROR);
					moddisplay(output,SELECT_WINDOW,END_MOD);
				}
				displayln(output,"%4d",current->max_size);
				setattr(output,IDLE_COLOR);
				displayln(output," %n",current->Priority);
				displayln(output," %h",(short)current->Acumulated_time);
				if (code_opt)
				{
					displayln(output," %h",(long)current->code>>16);
					if (current==my_TCB)
						displayln(output," ????:????");
					else
					{
						short far *stack;
						stack=(short far *)((long)current->stackofs|
														 (long)current->stackseg<<16);
						displayln(output," %h:%h",stack[+6],stack[+5]);
					}
				}
				list_count++;

				current=current->next;

			}while (current!=first&&list_count<LINES);

			clr_display(output,3);

			misc_count=0;
		}
	}
}

