int line;

//#define IS_TEST // use test mode, special input handling

#ifdef _WIN32
#include <windows.h>
#include <stdio.h>
#include <malloc.h>
#include "easyirq/easyirq.h"
#include "winio/winio.h"
#include "../device.h"
#define FBYTE "%02x"
#else
#define extern_def
#include <mod.h>
#include <video.h>
#include "device.h"
#define FBYTE "%n"
extern windowptr stdout;
#define fprintf displayln
#define Sleep(n) Relinquish(-n)
#define GetTickCount() (*(short far*)0x46c)
#define FALSE 0
#define TRUE (!FALSE)
#define NULL ((void*)0)
#endif


extern int inportb(int);
extern void outportb(int,int);

#ifdef _WIN32
#define Allocate malloc
#define Release free
#else
#define Release Free
#endif

typedef struct lptprn_tag {
	STDPRN common;
#ifdef _WIN32
	HANDLE file;
#endif
	int bHoldOutput;
} LPTSTDPRN, *PLPTSTDPRN;

typedef struct buffer_tag {
	int head, tail;
	unsigned char buffer[4096];
} QUEUE, *PQUEUE;

static QUEUE in, out; 
int lock;

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
#define BACKWARD    0x20 // DCR(+2)


#define DMA_DISABLE  0x08 // NO DMA
#define IRQ_DISABLE  0x10 // may wish ti disable this...
#define IRQ_PRESENT  0x04
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
	fprintf( stdout, "Status:" FBYTE "\n", inportb( 0x379 ) );
	if( inportb( 0x37a ) & BACKWARD )
	{
		fprintf( stdout, "(%d)was backward...Going forward...\n", nLine );
		while( !(inportb( 0x77a ) & 0x1 ) ) // data coming in...
		{
			fprintf( stdout, "Holding on input data...\n" );
			if( IsInIntr )
				do_lpt_read();
		}
		line = __LINE__;
		outportb( 0x77a, 0 );
		line = __LINE__;
		outportb( 0x37a, 0 );
		line = __LINE__;
		fprintf( stdout, "Allow interupt to go...\n" );
		line = __LINE__;
		outportb( 0x77a, mode ); // does trigger interupt...
		line = __LINE__;
		Sleep(0);
		fprintf( stdout, "Interupt finished?\n" );
	}	
}

static int GoBackward( int nLine )
#define GoBackward() GoBackward( __LINE__ )
{
	int start = GetTickCount();
	fprintf( stdout, "Status:" FBYTE "\n", inportb( 0x379 ) );
	if( !(inportb(0x37a ) & BACKWARD ) )
	{
		fprintf( stdout, "(%d)was forward...Going backward...\n", nLine );
		line = __LINE__;
		while( !(inportb( 0x77a ) & 0x1 ) ) // data going out - no action 
		{
#ifdef IS_TEST
			do_lpt_read();
#endif
			fprintf( stdout, "GB wait " FBYTE ".\n", inportb( 0x77a ) );
			if( (start + 10) > GetTickCount() )
         {
         	fprintf( stdout, "Go backward wait timeout " FBYTE "\n",inportb( 0x77a ) );
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
		fprintf( stdout, "Was backward you twit!(%d)\n", nLine );
	}
	return 1;
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
			fprintf( stdout, "O:" FBYTE "", DATA );
		else
			fprintf( stdout, "O:%c", DATA );
		outportb( 0x778, DATA );
	}
	
	if( IsData( &out ) )
	{
		fprintf( stdout, "held on: " FBYTE "\n", inportb( 0x77a ) );
		outportb( 0x77a, mode ); // reset serviceint flag...
	}	
	return status;
}

static int do_lpt_read(void)
{
	int status = 0;
	// should already be 'incoming'
#if !defined IS_TEST
	if( inportb( 0x37a ) & BACKWARD )
#endif
	{
		while( !(inportb( 0x77a ) & 0x1) )
		{
			int data = inportb( 0x778 );
			status = 1;
			fprintf( stdout, "Read: %c " FBYTE " " FBYTE "", data, inportb( 0x77a ), inportb( 0x37a ) );
			Enque( &in, data );
		}
	}
#if !defined IS_TEST
	else
	{
		//fprintf( stdout, " fixup backward?!\n" );
		//GoBackward();
		//line = __LINE__;
		Sleep(0);
	}
#endif
	return status;
}

#ifdef _WIN32
static void _stdcall irqHandler(DWORD irqNum)
#else
void irqHandler( void )
#endif
{
	if( TestIRQ )
	{
//		fprintf( stdout, "tI%d",line );
		fprintf( stdout, "tI" );
		if( inportb( 0x37a ) & BACKWARD )
		{
			if( TestInHigh )
				TestInHigh = 0;
			fprintf( stdout, "tR" );
		}
		else	
		{
			if( TestOutLow )
				TestOutLow = 0; 
			fprintf( stdout, "tW" );
		}
		return;
	}
	IsInIntr = 1;
	fprintf( stdout, "I[%d][" FBYTE "]", line, inportb( 0x379 ) );
	if( !(inportb( 0x77a ) & 0x4 ) )
	{
		fprintf( stdout, "No IRQ %d " FBYTE " \n", line, inportb( 0x77a ) );
		//GoBackward();
		//return;
	}
	if( inportb( 0x37a ) & BACKWARD )
	{
		line = __LINE__;
		fprintf( stdout, "R" );
		line = __LINE__;
		while( do_lpt_read() );
		line = __LINE__;
	}
	else
	{
		fprintf( stdout, "W" );
		line = __LINE__;
		while( do_lpt_write() ) 
			Sleep(0); // may have more if we sleep a bit...
		line = __LINE__;
		GoBackward(); // set by default incoming direction
	}
		line = __LINE__;
	fprintf( stdout, "end interupt: " FBYTE "", inportb( 0x77a ) );
		line = __LINE__;
	IsInIntr = 0;
		line = __LINE__;
}

static void CloseLPT( PLPTSTDPRN pStdPrn )
{
    // turn off IRQ 7 handling (mask irq)
	outportb( 0x77a, 0|IRQ_DISABLE ); // turn off everything ,re set to standard mode...
#ifdef _WIN32
	easyIrq(7, FALSE, &irqHandler);
#endif
	Release( pStdPrn );
	return;
}

// print and eject page?
static void ResetLPT( PLPTSTDPRN pStdPrn )
{
	pStdPrn->common.Write( (PSTDPRN)pStdPrn, "\x1b""E", 2 );
	return;
}

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

static int ReadLPT( PLPTSTDPRN pStdPrn, char *pointer, int size )
{
	int data, idx = 0;

	do_lpt_read();
	while( ( idx < size ) && ( ( data = Deque( &in ) ) >= 0 ) )
		pointer[idx++] = data;
	pointer[idx] = 0; // null terminate...
	return idx;

}

static int WriteLPT( PLPTSTDPRN pStdPrn, char *pointer, int size )
{
	int idx, status;
	for( idx = 0; idx < size; idx++ )
	{
		line = __LINE__;
		/*
		if( pointer[idx] < 32 )
			fprintf( stdout, "W:" FBYTE " ", pointer[idx] );
		else
			fprintf( stdout, "W:%c ", pointer[idx] );
		*/
		if( !Enque( &out, pointer[idx] ) )
		{
			fprintf( stdout, "Pausing on write...setting forward, sending 1\n" );

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
			fprintf( stdout, " SetForward Please output now...\n" );
			line = __LINE__;
			GoForward();  // act of doing so makes an interupt...
			line = __LINE__;
		}
	}
	return 0;	
}


void DoPortTestOut( void )
{
	int cnt;
//#define TEST_MODE_VAL TESTMODE|IRQ_DISABLE|IRQ_PRESENT
//#define TEST_MODE_VAL TESTMODE|IRQ_DISABLE|DMA_DISABLE
#define TEST_MODE_VAL TESTMODE
	TestIRQ = 1;

	fprintf( stdout, "" FBYTE "\n", inportb( 0x37a ) );
	fprintf( stdout, "" FBYTE "\n", inportb( 0x77a ) );

	line = __LINE__;
	outportb( 0x77a, TEST_MODE_VAL );  // config mode - all disables
	Sleep(0);

	cnt = 0;
	fprintf( stdout, "" FBYTE " " FBYTE "\n", inportb( 0x37a ), inportb( 0x77a ) );
	do
	{
		outportb( 0x778, cnt );
		cnt++;
	}
	while( !(inportb( 0x77a ) & 0x2 ) );

	fprintf( stdout, "Filled output with %d chars\n", cnt );
	outportb( 0x77a, TEST_MODE_VAL&(~IRQ_PRESENT) );  // config mode - all disables
	TestOutLow = 1;
	while( !(inportb( 0x77a ) & 0x1 ) && TestOutLow )
	{
		cnt--;
                inportb( 0x778 );
#ifdef _WIN32
                Sleep(0); // relinquish to allow another thread to do interupt.
#endif
	}
	if( cnt < 0 )
	{
		fprintf( stdout, "Got more input than made output -%d\n", -cnt );
	}
	else if( cnt == 0 )
	{
		fprintf( stdout, "Think we never got a low water interupt.\n" );
	}
	else 
	{
		fprintf( stdout, "Looks like interupt occurd with %d left\n", cnt );
	}
	while( !(inportb( 0x77a ) & 0x1 ) )
	{
		inportb( 0x778 );
		Sleep(0); // relinquish to allow another thread to do interupt.
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

	fprintf( stdout, "" FBYTE " " FBYTE "\n", inportb( 0x37a ), inportb( 0x77a ) );

	outportb( 0x37a, BACKWARD );

	line = __LINE__;
	outportb( 0x77a, TEST_MODE_VAL );  // config mode - all disables
	Sleep(0);

	cnt = 0;
	fprintf( stdout, "" FBYTE " " FBYTE "\n", inportb( 0x37a ), inportb( 0x77a ) );
	TestInHigh = 1;
	do
	{
		fprintf( stdout, "" FBYTE " ", inportb( 0x77a ) );
		outportb( 0x778, cnt );
		Sleep(100);
		cnt++;
	}
	while( TestInHigh && !(inportb( 0x77a ) & 0x2 ) );

	if( cnt > 16 )
	{
		fprintf( stdout, "Got more input than made output -%d\n", -cnt );
	}
	else if( cnt == 16 )
	{
		fprintf( stdout, "Think we never got a high water interupt.\n" );
	}
	else
	{
		fprintf( stdout, "Looks like interupt occured with %d present\n", cnt );
	}

	while( !(inportb( 0x77a ) & 0x1 ) )
	{
		inportb( 0x778 );
		Sleep(0); // relinquish to allow another thread to do interupt.
	}
	TestIRQ = 0;
}


#ifndef _WIN32
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
#endif

int PortMode = 0;




PLPTSTDPRN OpenLPT( void )
{
	PLPTSTDPRN newprn = Allocate( sizeof( LPTSTDPRN ) );
#ifdef _WIN32
   if ( easyIrq(7, TRUE, &irqHandler) ) {
      fprintf( stdout, "Failed to connect IRQ7\n" );
      Release( newprn );
      return NULL;
	}
#else
	_disconnect_prn();
	connect_prn();
#endif

	{
		int a, b, c;
		outportb( 0x77a, 0xE0 ); // config registers, no interupts, no dma
		c = inportb( 0x77a );
		if( ( c & 1 ) && ( !(c&2)) )
		{
		   outportb( 0x77a, 0x1F );  // config mode - interupts enable now.
		   c = inportb( 0x77a );
		   if( c == 0x1d )
		   {
		   	PortMode = 2;
		   	fprintf( stdout, "Yes ECP..." );
		   	goto testepp;
		   }
		   else
		   {
		   	goto testepp;
		   }
		}
		else
		{
			testepp:
			{
				int n;
				for( n = 0; n < 8; n++ )
				{
					outportb( 0x37b, 1<<n );
					if( inportb( 0x37b ) != 1<< n )
						break;   
					outportb( 0x379, 1 );
				}
				if( n < 8 )
				{
					fprintf( stdout, "EPP Test Failed.\n" );
					//Release( newprn );
					//return NULL;
				}
				else
				{
					fprintf( stdout, "Yes EPP..." );
					PortMode = 1; // epp
				}
			}

		}
	}

   if( PortMode == 2 )
		outportb( 0x77a, 0x0 ); // standard mode
	outportb( 0x37a, 0 );   // no options.

   // to go to epp (assuming we have an ecp port)
	//outportb( 0x77a, 0x80 );
	//outportb( 0x37a, 0x04 );
	if( PortMode == 2 )
	{
		DoPortTestOut();
		DoPortTestIn();
	}
//	return NULL;

	GoBackward(); // sets mode and irq enable...
    
	newprn->common.Close  = (void(*)(PSTDPRN))CloseLPT;	
	newprn->common.Reset  = (void(*)(PSTDPRN))ResetLPT;
	newprn->common.Read   = (int(*)(PSTDPRN,char*,int))ReadLPT;
	newprn->common.Write  = (int(*)(PSTDPRN,char*,int))WriteLPT;
	newprn->common.Enter  = (void(*)(PSTDPRN))EnterCommandMode;
	newprn->common.Leave  = (void(*)(PSTDPRN))LeaveCommandMode;
	newprn->common.EnableStatus = (void(*)(PSTDPRN))EnableStatus;
	ResetLPT( newprn );
	return newprn;
}