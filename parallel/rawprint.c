//#include <windows.h> // createthread
//#include <stdio.h>

#include "device.h"

PSTDPRN pStdPrn;

#include <mod.h>
#include <video.h>
windowptr stdout;

//----------------------------------------------------------------

void ReadPrn( void )
{
	char buffer[256];
	int len;
	while( 1 )
	{
		if( len = pStdPrn->Read( pStdPrn, buffer, 256 ) )
		{
			buffer[len] = 0;
			displayln( stdout, "stdprn: %s", buffer );
		}
		else
			Relinquish( -10 );
	}
}

//----------------------------------------------------------------

PSTDPRN OpenLPT( void );

//----------------------------------------------------------------

char *GetLine( char *buffer, int maxlen, windowptr where )
{
	int ofs = 0;
	maxlen--; // leave room for the NUL char...
	if( maxlen <= 0 )
		return 0; // invalid...

	while( ofs < maxlen )
	{
		unsigned char blah;
		blah = readch( where );
		if( blah == '\r' )
		{
			buffer[ofs++] = '\n';
			break;
		}
		display( stdout, blah );
		buffer[ofs++] = blah;
	}
	buffer[ofs] = 0;
	return buffer;
}

//----------------------------------------------------------------

int main( void )
{
	char buffer[256];
	stdout = opendisplay( 5, 5,
								60, 101,
								BORDER|NEWLINE|NO_CURSOR,
								0x1f, 0x1f, 0x2f, "Printer IO" );
	moddisplay( stdout, RESIZEY(-70),
                     //PAGE_DOWN,
                     //PAGE_DOWN,
                     END_MOD);

	pStdPrn = OpenLPT();
	if( !pStdPrn )
	{
		displayln( stdout, "No sorry can't open lpt1:\n" );
		Relinquish( 1 );
		return(0);
	}

	if( fork(CHILD) )
	{
		ReadPrn();
	}

	pStdPrn->EnableStatus( pStdPrn );
	displayln( stdout, "Begin printer control...\n" );
	pStdPrn->Enter( pStdPrn );
	if(0)
	{
		int i;
		displayln( stdout, "Big Spam!\n" );
		for( i = 0; i < 20000; i++ )
		{
			if( !(i%100) ) displayln( stdout, "%d\r", i );
			pStdPrn->Write( pStdPrn, "0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789", 160 );
		}
	}
	while( GetLine( buffer, 256, stdout ) )
	{
		int len = strlen( buffer );
		//printf( "Got STring...\n" );
		if( len > 1 )
		{
			len -= 1;	
			buffer[len] = 0; // kill \n
		}
		displayln( stdout, "Keyboard: %s\n", buffer );
		pStdPrn->Write( pStdPrn, buffer, len );
     	pStdPrn->Write( pStdPrn, "\r\n", 2 );
	}
	pStdPrn->Write( pStdPrn, "\x1b%-12345X\r\n", 11 );

	pStdPrn->Close( pStdPrn );

	return 0;
}

//----------------------------------------------------------------
