#include "mod.h"
#include "video.h"

#define false 0
#define true !false

#define	 priorsegment	 0
#define	 blocksize		 1
#define	 owner				 2
#define	 nextfree			 3
#define	 priorfree		 4
#define	 freed_by			 5

/*	If the os memory debug option is on then the header size is 2.	otherwise
		the headersize is 1 */

#define HEADER_SIZE 1

static char data_item;						/*something to establish our data seg */
#define DATASEG seg data_item

unsigned short far *first_block;
unsigned short far *curblock;
unsigned short first_seg;
unsigned short far *alloc_fails;

char okay=false;
unsigned short total_free,number_free,largest_free,smalest_free;
unsigned short total_used,number_used,		max_used,		 min_used;

#define seg2ptr(a) (((long)a)<<16)
#define ptr2seg(a) ((short)(((long)a)>>16))

short data_valid;
unsigned short prior;

window_type far *output,far *dump;

static void check_free_link(unsigned short far *current)
{
	unsigned short far *free_head;
	if (current[nextfree])
	{
		if (current[nextfree] < first_seg)
		{
			displayln(output,"NFree Before Begin");
			asm int 3;
			Exit(106);
		}
		free_head=(unsigned short far *)seg2ptr(current[nextfree]);
		if (free_head[priorfree]!=ptr2seg(current))
		{
			displayln(output,"Free Link Error1");
			asm int 3;
			Exit(102);
		}
	}
	if (current[priorfree])
	{
		if (current[priorfree] < first_seg)
		{
			displayln(output,"PFree Before Begin");
			asm int 3;
			Exit(104);
		}
		free_head=(unsigned short far *)seg2ptr(current[priorfree]);
		if (free_head[nextfree]!=ptr2seg(current))
		{
			displayln(output,"Free Link Error2");
			asm int 3;
			Exit(103);
		}
	}
}

public(void,Dump_blocks,(short segment))
{
	short far *memptr=(short far *)(((long)segment-1)<<16);
	short i;
	Load_DS;
	for (i=0;i<(8*4);i++)
	{
		displayln(dump,"%h ",memptr[i]);
	}
	Restore_DS;
}

public(void,memdiag,(short intr))
{
	asm push ds
	asm mov ax,DATASEG
	asm mov ds,ax

	if (okay)
		while ((unsigned long)curblock < 0xa0000000L)
		{
			if((*process_control & CONTEXT_SCAN) && intr)
			{
				 data_valid=false; /* we didn't get there */

				 break; /*task scan needed don't compute */
			}

			if (curblock[owner])
			{
/* owned blocks */

				total_used+=curblock[blocksize];
				number_used+=1;

			}
			else
			{

/* free	 blocks */
				check_free_link(curblock);
				total_free+=curblock[blocksize];
				number_free+=1;
				if(largest_free < curblock[blocksize])largest_free=
						 curblock[blocksize];

			}
			prior=(long)curblock >> 16;
			(long)curblock=((long)curblock+((long)curblock[blocksize]<<16)+((long)HEADER_SIZE<<16));
			if(prior!=curblock[priorsegment] &&
				 (unsigned long)curblock!=0xa0000000l)
			{			 /* this is an invalid block */
				displayln(output,"INVALID BLOCK\n*h *H\n"
								 ,prior,(unsigned long)curblock);
				asm int 3
				Exit(133);
			}
		}
	asm pop ds;
}


main()
{
	_memdiag();
	_Dump_blocks();

	alloc_fails=Inquire_begin(NULLS);		/* ask OS for failure count			 */
	first_block=Inquire_begin(BLOCK);		/* ask OS for first memory block */
	first_seg=ptr2seg(first_block);

	output=opendisplay(60,1,21,9,NEWLINE|NO_CURSOR|BORDER,0x70,0x4f,0,"MemSize");
	dump=opendisplay(20,1,40,5,NEWLINE|NO_CURSOR|BORDER,
									WHITE|ON_BLUE,RED|ON_BLACK,0,"MemDump");

	displayln(output,"Total Free:\n");
	displayln(output,"Total Used:\n");
	displayln(output,"Number Free:\n");
	displayln(output,"Number Used:\n");
	displayln(output,"Largest	 Free:\n");
	displayln(output,"Smallest Free:\n");
	displayln(output,"Most	Used:\n");
	displayln(output,"Least Used:\n");
	displayln(output,"Alloc. Fails:");

	max_used=		 0;
	min_used=65535;
	smalest_free=65535;
	okay = true;

	while (true)
	{
		curblock=first_block;

		total_free =0;
		total_used =0;

		number_free=0;
		number_used=0;

		largest_free=0;

		data_valid=true;	 /* assume we computed all information */

		memdiag(1);					/* go check the memory*/

		;

		if(data_valid) /* display the computed information */
		{
			 if(max_used < total_used)max_used=total_used;
			 if(min_used > total_used)min_used=total_used;

			 if(smalest_free > largest_free)smalest_free=largest_free;

			 position(output,15,0); displayln(output,"%5d",total_free);
			 position(output,15,1); displayln(output,"%5d",total_used);

			 position(output,15,2); displayln(output,"%5d",number_free);
			 position(output,15,3); displayln(output,"%5d",number_used);

			 position(output,15,4); displayln(output,"%5d",largest_free);
			 position(output,15,5); displayln(output,"%5d",smalest_free);

			 position(output,15,6); displayln(output,"%5d",max_used);
			 position(output,15,7); displayln(output,"%5d",min_used);

			 position(output,15,8); displayln(output,"%5d",*alloc_fails);

			 Relinquish (-100);
		}
		else Relinquish(0);

	}
}

