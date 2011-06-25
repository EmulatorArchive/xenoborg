/*
 * Xenoborg - Xbox Emulator
 * Copyright (C) 2006-2010  blueshogun96
 *
 * Name: Error.h
 * Desc: Error reporting functions.
 *
 * Changelog: 
 *	9/21/2010 - Initial writing.
 */

#include "XenoKrnl.h"
#include "Error.h"


//--------------------------------------------------------------------------------------
// Name: EmuWarning
// Desc: Displays a warning message in the debug console.
//--------------------------------------------------------------------------------------
void EmuWarning( char* szWarning, ... )
{
	va_list args;
	char string[128];

	va_start( szWarning, args );
	vsprintf( string, szWarning, args );
	va_end( args );

	printf( string );
}

//--------------------------------------------------------------------------------------
// Name: EmuError
// Desc: Displays an error message in the debug console and on a message box for the user,
//		 then posts a quit message to terminate.
//--------------------------------------------------------------------------------------
bool EmuError( char* szError, ... )
{
	va_list args;
	char string[128];

	va_start( args, szError );
	vsprintf( string, szError, args );
	va_end( args );

	printf( string );
	MessageBoxA( NULL, string, "XenoKrnl", MB_ICONEXCLAMATION );

	PostQuitMessage(0);

	return false;
}

