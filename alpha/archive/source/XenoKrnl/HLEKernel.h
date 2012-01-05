/*
 * Xenoborg - Xbox Emulator
 * Copyright (C) 2006-2010  blueshogun96
 *
 * Name: HLEKernel.h
 * Desc: Contains code for the HLE kernel. This is used as an alternative to true
 *		 BIOS emulation when necessary. Upon load time, Xenoborg will scan for any
 *		 call/jump commands referencing a kernel function and replace the address
 *		 with the appropriate function.
 *
 * Changelog: 
 *	5/2/11 - Initial writing.
 */

#pragma once


//--------------------------------------------------------------------------------------
// HLE Kernel functions
//--------------------------------------------------------------------------------------
void*	NTAPI	AvGetSavedDataAddress();			// 0x001 (1)	
ULONG	__cdecl	DbgPrint( char* Format, ... );		// 0x008 (8)
extern volatile DWORD	KeTickCount;				// 0x09C (156)
void	NTAPI	HalInitiateShutdown();				// 0x168 (360)

// Installs hooks to emulated functions
void HookKrnlFunctions();