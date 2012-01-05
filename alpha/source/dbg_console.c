/*
 * Xenoborg - Xbox Emulator
 * Copyright (C) 2006-2010  blueshogun96
 *
 * Name: DbgConsole.cpp
 * Desc: The debug console code.
 *
 * Changelog: 
 *  8/24/11 - Yet another import to new project!
 *
 *	10/13/2010 - Initial import from one of my old builds.  Good code
 *				 is a terrible thing to waste.
 */

#include "win32.h"
#include "dbg_console.h"



//-----------------------------------------------------------------------------
// Name: DbgConsoleInit
// Desc: Initialize debug console for debug builds
//-----------------------------------------------------------------------------
BOOL DbgConsoleInit( void )
{
	if( AllocConsole() )
	{
		freopen( "CONOUT$", "wt", stdout );

		SetConsoleTitle( "Xenoborg : Debug Console" );

		SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ), 
								 FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_RED );

		return TRUE;
	}

	return FALSE;
}