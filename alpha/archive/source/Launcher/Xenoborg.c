/*
 * Xenoborg - Xbox Emulator
 * Copyright (C) 2006-2010  blueshogun96
 *
 * Name: Xenoborg.c
 * Desc: This is the program's entry point and only source file of Xenoborg.exe.
 *		 The purpose of this .exe file is just to reserve the memory addresses
 *		 [0x10000 - 0x3FFFFFF] to emulate Xbox's ram (which is roughly 64mb).  
 *		 For a debug console or Dream-X, we can increase the range to 0x7FFFFFF
 *		 for 128mb, and Sega Chihiro will require up to 512mb depending on the
 *		 game... even though Chihiro itself can handle 1gb.
 *
 * Changelog: 
 *	12/12/10 - Decided to give this file .c linkage due to the fact that when
 *			   compiling in debug mode, I get retarded linker errors.  If this
 *			   is a problem, change it back.
 *
 *	10/14/10 - Just added this header containing more info about what this
 *			   .exe is really for.
 */

#include "..\\XenoKrnl\\XenoKrnl.h"
#include "resource.h"

unsigned int mem[1024*1024*64];	// 64mb


int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd )
{
	XenoKrnlNullFunc();

	return 0;
}