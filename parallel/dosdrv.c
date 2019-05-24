
int line; 

//#define IS_TEST // use test mode, special input handling

//#include <stdio.h>
//#include <malloc.h>

#define FALSE 0
#define TRUE (!FALSE)
#define NULL ((void*)0)
#define GetTickCount()  (*(short far*)0x46c)
#define Sleep(n)        Relinquish( -n )


#define extern_def
#include <mod.h>
#include <video.h>
#include "device.h"

extern windowptr output;


#define Allocate Allocate
#define Release Free

typedef struct lptprn_tag {
	STDPRN common;
	//HANDLE file;
	int bHoldOutput;
} LPTSTDPRN, *PLPTSTDPRN;

typedef struct buffer_tag {
	int head, tail;
	unsigned char buffer[4096];
} QUEUE, *PQUEUE;

static QUEUE in, out; 
int lock;
int direction; // direction I *THINK* it is....

static int Enque( PQUEUE q, char data )
{
	int newhead = q->head;
	newhead++;
	if( newhead >= 4096 )
		newhead -= 4096;
	if( newhead == q->tail )
		return 0;
	q->buffer[q->head] = data;
	q->head = newhead;
	return 1;
}

static int Deque( PQUEUE q )
{
	unsigned char data;
	int newtail;
	if( q->head == q->tail )
		return -1;
	data = q->buffer[newtail = q->tail];
	newtail++;
	if( newtail >= 4096 )
		newtail -= 4096;
	q->tail = newtail;
	return data;
}

static int IsData( PQUEUE q )
{
	if( q->head != q->tail )
		return 1;
	return 0;
}


#define IRQ_ENABLE  0x10 // applied to control register
#define BACKWARD   0x20 // DCR(+2)

#define IRQ_PRESENT  0x04
#define DMA_DISABLE  0x08 // NO DMA
#define IRQ_DISABLE  0x10 // may wish ti disable this...
#define TESTMODE    0xc0 
#define EPPMODE     0x80
#define FIFOMODE    0x60
#define SPPFIFOMODE 0x40
#ifdef IS_TEST
#define mode TESTMODE|DMA_DISABLE
#else
#define mode FIFOMODE|DMA_DISABLE 
#endif



static int IsInIntr, TestIRQ, TestOutLow, TestInHigh;
static int do_lpt_read(void);

static void GoForward( int nLine )
#define GoForward() GoForward( __LINE__ )
{
	if( inportb( 0x37a ) & BACKWARD )
	{
		displayln( output,"(%d)was backward...Going forward...\n", nLine );
		while( !(inportb( 0x77a ) & 0x1 ) ) // data coming in...
		{
			displayln( output,"Holding on input data...\n" );
			if( IsInIntr )
				do_lpt_read();
		}
		line = __LINE__;
		outportb( 0x77a, 0 );
		line = __LINE__;
		outportb( 0x37a, 0 );
		line = __LINE__;
		outportb( 0x77a, mode ); // does trigger interupt...
		line = __LINE__;
		displayln( output,"Allow interupt to go...\n" );
		line = __LINE__;
		Sleep(0);
		displayln( output, "Interupt finished?\n" );
	}
}
/*
static void GoForward( void )
{
	if( inportb( 0x37a ) & BACKWARD )
	{
		//displayln( output, "was backward...Going forward...\n" );
		while( !(inportb( 0x77a ) & 0x1 ) ) // data coming in...
		{
			//if( IsInIntr )
			do_lpt_read();
		}
		outportb( 0x77a, 0 );
		outportb( 0x37a, 0x10 );
		outportb( 0x77a, mode );
	}
	else
		do_lpt_write();
}
*/
static int GoBackward( int nLine )
#define GoBackward() GoBackward( __LINE__ )
{
	int start = GetTickCount();
	if( !(inportb(0x37a ) & BACKWARD ) )
	{
		displayln( output, "(%d)was forward...Going backward...\n", nLine );
		line = __LINE__;
		while( !(inportb( 0x77a ) & 0x1 ) ) // data going out - no action 
		{
#ifdef IS_TEST
			do_lpt_read();
#endif
			displayln( output, "GB wait %n.\n", inportb( 0x77a ) );
			if( (start + 10) > GetTickCount() )
         {
         	displayln( output, "Go backward wait timeout %n\n",inportb( 0x77a ) );
				return 0;
			}
			Sleep(0);
		}
		line = __LINE__;
		outportb( 0x77a, 0 );
		line = __LINE__;
		outportb( 0x37a, BACKWARD );
		line = __LINE__;
		outportb( 0x77a, mode ); // does not trigger interupt...
		line = __LINE__;
	}
	else
	{
		displayln( output, "Was backward you twit!(%d)\n", nLine );
	}
	return 1;
}
/*
static int GoBackward( void )
{
	int start = GetTickCount();
	if( !(inportb(0x37a ) & BACKWARD ) )
	{
		//displayln( output, "was forward...Going backward...\n" );
		while( !(inportb( 0x77a ) & 0x1 ) ) // data going out - no action 
		{
#ifdef IS_TEST
			do_lpt_read();
#endif
			displayln( output, "GB wait %2x.\n", inportb( 0x77a ) );
			if( (start + 10) < GetTickCount() )
         {
         	displayln( output, "Go backward wait timeout %2x\n",inportb( 0x77a ) );
				return 0;
			}
			Relinquish(0);
		}
		outportb( 0x77a, 0 );
		outportb( 0x37a, 0x30 );
		outportb( 0x77a, mode );
	}
	return 1;
}
*/
static int CheckNeedWrite(void )
{
	int state;
	GoForward();    // set for this purpose outgoing direction
	state = inportb( 0x77a ) & 3;
	GoBackward(); // set by default incoming direction
	return state;
}

static int do_lpt_write( void )
{
	char DATA;
	int status = 0;
	// direction forward....
	while( IsData( &out ) && !( inportb( 0x77a ) & 2 ) )
	{
		DATA = Deque( &out );
		status |= 1;
		if( DATA < 32 )
			displayln( output, "O:%n", DATA );
		else
			displayln( output, "O:%c", DATA );
		outportb( 0x778, DATA );
	}

	if( IsData( &out ) )
	{
		displayln( output, "held on: %n\n", inportb( 0x77a ) );
		outportb( 0x77a, mode ); // reset serviceint flag...
	}
	return status;
}
/*
static int do_lpt_write( void )
{
	char DATA;
	int status = 0;
	// direction forward....
	GoForward();    // set for this purpose outgoing direction
	while( IsData( &out ) && !( inportb( 0x77a ) & 2 ) )
	{
		unsigned char dataout = Deque( &out );
		
		if( dataout < 32 )
			displayln( output, "Write: %n %n\n", dataout, inportb( 0x77a ) );
		else
			displayln( output, "Write: %c %n\n", dataout, inportb( 0x77a ) );
		
		status |= 1;
		outportb( 0x778, dataout );
	}
	
	if( IsData( &out ) )
	{
		displayln( output, "held on: %n\n", inportb( 0x77a ) );
		outportb( 0x77a, mode );
	}	
	else
		GoBackward(); // set by default incoming direction
	//displayln( output, "Status: %n", inportb( 0x37a ) );// validate direction please...
	return status;
}
*/

static int do_lpt_read(void)
{
	int status = 0;
	// should already be 'incoming'
	if( inportb( 0x37a ) & 0x20 )
	{
		while( !(inportb( 0x77a ) & 0x1) )
		{
			int data = inportb( 0x778 );
			status = 1;
			displayln( output, "Read: %c %n %n", data, inportb( 0x77a ), inportb( 0x37a ) );
			Enque( &in, data );
		}
	}
	else
	{
		displayln( output, " fixup backward?!\n" );
		GoBackward();
	}
	return status;
}

void irqHandler(void)
{
	if( TestIRQ )
	{
		display( output, 'I' );
		if( inportb( 0x37a ) & 0x20 )
		{
			display( output, 'R' );
		}
		else	
		{
			if( TestOutLow )
				TestOutLow = 0; 
			display( output, 'W' );
		}
		return;
	}
	IsInIntr = 1;
	if( !(inportb( 0x77a ) & 0x4 ) )
	{
		displayln( output, "No IRQ\n" );
		return;
	}
	displayln( output, "I" );
	if( inportb( 0x37a ) & 0x20 )
	{
		displayln( output, "R" );
		while( do_lpt_read() );
	}
	else
	{
		displayln( output, "W" );
		while( do_lpt_write() )
			Relinquish(0);
		GoBackward(); // set by default incoming direction
	}

	displayln( output, "end interupt: %n", inportb( 0x77a ) );
	IsInIntr = 0;
}

static void CloseLPT( PLPTSTDPRN pStdPrn )
{
    // turn off IRQ 7 handling (mask irq)
    outportb( 0x77a, 0 ); // turn off everything ,re set to standard mode...
    //easyIrq(7, FALSE, &irqHandler);
	Release( pStdPrn );
	return;
}

// print and eject page?
static void ResetLPT( PLPTSTDPRN pStdPrn )
{
	pStdPrn->common.Write( (PSTDPRN)pStdPrn, "\x1b""E", 2 );
	return;
}

static int ReadLPT( PLPTSTDPRN pStdPrn, char *pointer, int size )
{
	int data, idx = 0;
	//do_lpt_read();
	while( ( idx < size ) && ( ( data = Deque( &in ) ) >= 0 ) )
		pointer[idx++] = data;
	return idx;
}

static int WriteLPT( PLPTSTDPRN pStdPrn, char *pointer, int size )
{
	int idx, status;
	for( idx = 0; idx < size; idx++ )
	{
		line = __LINE__;
		if( pointer[idx] < 32 )
			displayln( output, "W:%n ", pointer[idx] );
		else
			displayln( output, "W:%c ", pointer[idx] );
		if( !Enque( &out, pointer[idx] ) )
		{
			displayln( output, "Pausing on write...setting forward, sending 1\n" );

			// kick the write interupt?
			GoForward();
			Sleep(0); // give it time...
		}
	}
	if( !pStdPrn->bHoldOutput )
	{
		if( inportb( 0x37a ) & BACKWARD ) // If backward... do write...
		{
			line = __LINE__;
			displayln( output, " SetForward Please output now...\n" );
			line = __LINE__;
			GoForward();  // act of doing so makes an interupt...
			line = __LINE__;
		}
	}
	return 0;
}

/*
static int WriteLPT( PLPTSTDPRN pStdPrn, char *pointer, int size )
{
	int idx, status;
	for( idx = 0; idx < size; idx++ )
	{
		//displayln( output, "Queue: %c ", pointer[idx] );
		if( !Enque( &out, pointer[idx] ) )
		{
			displayln( output, "Pausing on write...setting forward, sending 1\n" );

			// kick the write interupt?
			GoForward();
			if( !( inportb( 0x77a ) & 2 ) )
	         outportb( 0x778, Deque( &out ) ); // should get subsequent interupts to write?
			else
				displayln( output, "Out full... no interupts?!" );
			//if( inportb( 0x77a ) & 1 ) 
			//	do_lpt_write();
			Relinquish(0); // give it time...
		}
	}

	if( inportb( 0x37a ) & 0x20 ) // If backward... do write... 
		do_lpt_write();
	//displayln( output, "Okay write's done...\n" );
	return 0;	
}
*/

static void EnterCommandMode( PLPTSTDPRN pStdPrn )
{
	pStdPrn->common.Write( (PSTDPRN)pStdPrn, "\x1b%-12345X@PJL\r\n", 15 );
}

static void LeaveCommandMode( PLPTSTDPRN pStdPrn )
{
	pStdPrn->common.Write( (PSTDPRN)pStdPrn, "\x1b%-12345X", 9 );
}

static void EnableStatus( PLPTSTDPRN pStdPrn )
{
	pStdPrn->bHoldOutput = 1;
	EnterCommandMode( pStdPrn );
	pStdPrn->common.Write( (PSTDPRN)pStdPrn, "@PJL ECHO Enabling verbose device status\r\n", 42 );
	pStdPrn->common.Write( (PSTDPRN)pStdPrn, "@PJL INFO USTATUS\r\n", 19);
	pStdPrn->common.Write( (PSTDPRN)pStdPrn, "@PJL USTATUS DEVICE=VERBOSE\r\n", 29 );
	pStdPrn->bHoldOutput = 0;
	LeaveCommandMode( pStdPrn );
}


void DoPortTestOut( void )
{
	int cnt;
//#define TEST_MODE_VAL TESTMODE|IRQ_DISABLE|IRQ_PRESENT
//#define TEST_MODE_VAL TESTMODE|IRQ_DISABLE|DMA_DISABLE
#define TEST_MODE_VAL TESTMODE
	TestIRQ = 1;

	displayln( output, "%n\n", inportb( 0x37a ) );
	displayln( output, "%n\n", inportb( 0x77a ) );

	line = __LINE__;
	outportb( 0x77a, TEST_MODE_VAL );  // config mode - all disables
	Relinquish(0);

	cnt = 0;
	displayln( output, "%n %n\n", inportb( 0x37a ), inportb( 0x77a ) );
	do
	{
		outportb( 0x778, cnt );
		cnt++;
	}
	while( !(inportb( 0x77a ) & 0x2 ) );

	displayln( output, "Filled output with %d chars\n", cnt );
	outportb( 0x77a, TEST_MODE_VAL&(~IRQ_PRESENT) );  // config mode - all disables
	TestOutLow = 1;
	while( !(inportb( 0x77a ) & 0x1 ) && TestOutLow )
	{
		cnt--;
		inportb( 0x778 );
		Relinquish(0); // relinquish to allow another thread to do interupt.
	}
	if( cnt < 0 )
	{
		displayln( output, "Got more input than made output -%d\n", -cnt );
	}
	else if( cnt == 0 )
	{
		displayln( output, "Think we never got a low water interupt.\n" );
	}
	else 
	{
		displayln( output, "Looks like interupt occurd with %d left\n", cnt );
	}
	while( !(inportb( 0x77a ) & 0x1 ) )
	{
		inportb( 0x778 );
		Relinquish(0); // relinquish to allow another thread to do interupt.
	}

	// phase 2... check readback interupt...

	TestIRQ = 0;
}

void DoPortTestIn( void )
{
	int cnt;
//#define TEST_MODE_VAL TESTMODE|IRQ_DISABLE|IRQ_PRESENT
//#define TEST_MODE_VAL TESTMODE|IRQ_DISABLE|DMA_DISABLE
#define TEST_MODE_VAL TESTMODE
	TestIRQ = 1;

	displayln( output, "%n %n\n", inportb( 0x37a ), inportb( 0x77a ) );

	outportb( 0x37a, BACKWARD );

	line = __LINE__;
	outportb( 0x77a, TEST_MODE_VAL );  // config mode - all disables
	Relinquish(0);

	cnt = 0;
	displayln( output, "%n %n\n", inportb( 0x37a ), inportb( 0x77a ) );
	TestInHigh = 1;
	do
	{
		displayln( output, "%n ", inportb( 0x77a ) );
		outportb( 0x778, cnt );
		Relinquish(-100);
		cnt++;
	}
	while( TestInHigh && !(inportb( 0x77a ) & 0x2 ) );

	if( cnt > 16 )
	{
		displayln( output, "Got more input than made output -%d\n", -cnt );
	}
	else if( cnt == 16 )
	{
		displayln( output, "Think we never got a high water interupt.\n" );
	}
	else
	{
		displayln( output, "Looks like interupt occured with %d present\n", cnt );
	}

	while( !(inportb( 0x77a ) & 0x1 ) )
	{
		inportb( 0x778 );
		Relinquish(0); // relinquish to allow another thread to do interupt.
	}
	TestIRQ = 0;
}

extern void lptintr(void);

extern char far *old_irq;


void connect_prn(void)
{
  long temp_ptr;
  asm sti
/*
  asm .386
  asm mov     di,2ch
  asm   mov   eax,es:[di]
  asm   mov   dword ptr old_irq,eax
*/

  asm mov ax,0x350F;
  asm int 21h
  asm mov word ptr old_irq,bx;
  asm mov word ptr old_irq+2,es;
  asm mov ax,0x250F;
  temp_ptr=(long)&lptintr;
  asm push ds
  asm mov dx,word ptr temp_ptr
  asm mov ds,word ptr temp_ptr+2
  asm int 21h
  asm pop ds;
  asm in      al,21h
  asm and     al,07fh;
  asm out     21h,al
  asm cli
}

cleanup(void,disconnect_prn,(void))
{
  asm push ds;
  asm in      al,21h
  asm or      al,80h;
  asm out     21h,al
  asm mov ax,seg old_irq
  asm mov ds,ax
  asm mov dx,word ptr old_irq
  asm mov ds,word ptr old_irq+2
  asm mov ax,0x250F;
  asm int 21h
  asm pop ds;
}


PLPTSTDPRN OpenLPT( void )
{
	PLPTSTDPRN newprn = Allocate( sizeof( LPTSTDPRN ) );
//	asm int 3;
	outportb( 0x77a, 0 );
	outportb( 0x37a, 0 );
	_disconnect_prn();
	connect_prn();

	outportb( 0x77a, 0 );
	outportb( 0x37a, 0 );

	DoPortTestOut();
	DoPortTestIn();
//	return NULL;

	{
		
		int a, b, c;
		outportb( 0x77a, 0xE0 ); // config registers, no interupts, no dma
		a = inportb( 0x778 );
		b = inportb( 0x779 );
		c = inportb( 0x77a );
		if( ( c & 1 ) && ( !(c&2)) )
		{
		   //displayln( output, "ecp..." );
		   outportb( 0x77a, 0x1c );  // config mode - all disables
		   c = inportb( 0x77a );
		   if( c == 0x1d )
		   {
		   	//displayln( output, "Yes ECP..." );
		   }
		   else
		   {
		   	Release( newprn );
		   	return NULL;
		   }	
		}
		else
		{
			displayln( output, "Not my computer and not ecp - dunno whatcha got there.\n" );
			Release( newprn );
			return NULL;
		}
		/*
      if ( easyIrq(7, TRUE, &irqHandler) ) {
         displayln( output, "Failed to connect IRQ7\n" );
         Release( newprn );
         return NULL;
      }
      */
		displayln( output, "Status: %n %n %n\n", a,b,c );
		a = inportb( 0x379 );
		b = inportb( 0x37a );
		displayln( output, "Status: %n %n\n", a,b );
	}
	GoBackward(); // sets mode and irq enable...
    
	newprn->common.Close  = (void(*)(PSTDPRN))CloseLPT;	
	newprn->common.Reset  = (void(*)(PSTDPRN))ResetLPT;
	newprn->common.Read   = (int(*)(PSTDPRN,char*,int))ReadLPT;
	newprn->common.Write  = (int(*)(PSTDPRN,char*,int))WriteLPT;
	newprn->common.Enter  = (void(*)(PSTDPRN))EnterCommandMode;
	newprn->common.Leave  = (void(*)(PSTDPRN))LeaveCommandMode;
	newprn->common.EnableStatus = (void(*)(PSTDPRN))EnableStatus;
	asm int 3;
	ResetLPT( newprn );
	return newprn;
}
