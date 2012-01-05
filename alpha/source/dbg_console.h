/*
 * Xenoborg - Xbox Emulator
 * Copyright (C) 2006-2010  blueshogun96
 *
 * Name: DbgConsole.h
 * Desc: The debug console code.
 *
 * Changelog: 
 *	10/13/2010 - Initial import from one of my old builds.  Good code
 *				 is a terrible thing to waste.
 */

#pragma once

// From caustik :)
#ifndef __cplusplus
__inline void null_func(char *format, ...) { }
#else
inline void null_func(char *format, ...) { }
#endif

#define DbgEmpty null_func

/* Debug Trace Enabled */
#ifdef  _DEBUG
#include <mmsystem.h>
#define DbgPrintf printf
#else
/* Debug Trace Disabled */
#define DbgPrintf DbgEmpty
#endif

BOOL DbgConsoleInit( void );