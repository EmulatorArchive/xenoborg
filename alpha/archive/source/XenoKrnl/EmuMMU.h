/*
 * Xenoborg - Xbox Emulator
 * Copyright (C) 2006-2010  blueshogun96
 *
 * Name: EmuMMU.h
 * Desc: Emulates memory reads/writes and handles emulation of priviledged
 *		 instructions that cannot be run within user mode (ring 3).
 *
 * Changelog: 
 *	2/9/11 - Changed the hardware writing procedures a bit.  Now the hardware
 *			 write description is saved to communicate with the updating thread
 *			 to elliminate the multithreading problems with OpenGL.
 *
 *	10/14/2010 - Initial writing.
 *
 */

#pragma once

// An x86 instruction descriptor
struct x86Instruction
{
	DWORD Eip;
	int Size;
	int Prefixes;
	int OperandSize;
	int AddressSize;
	int OpcodeSize;
	int Fpu;
	BYTE PrefixBytes[4];
	BYTE Op;
};

// MMU Write description
struct x86MMUWriteDesc
{
	DWORD Address;	// The address that was written to
	DWORD Value;	// The value written to the address
	int	Bits;		// The bit length of the value written
};

// *Very* useful defines!
#define mod( d )	( ( d >> 6 ) & 0x3 )
#define reg_op( d ) ( ( d >> 3 ) & 0x7 )
#define rm( d )		( d & 0x7 )

#define scale( d )	( ( d >> 6 ) & 0x3 )
#define index( d )	( ( d >> 3 ) & 0x7 )
#define base( d )	( d & 0x7 )

#define sign8(b)	( ( b >> 7 ) & 0x1 )
#define sign16(w)	( ( w >> 15 ) & 0x1 )
#define sign32(d)	( ( d >> 31 ) & 0x1 )

// Initializes special memory pointers
BOOL EmuMMUInit();
// Uninitializes special pointers 
void EmuMMUUninit();
// Saves the .exe's occupied memory range
void EmuMMUSaveExeContents();
// Restores the .exe's occupied memory range
void EmuMMURestoreExeContents();

#define ReadByte(offset) (BYTE) (*((BYTE*) Eip+offset))

__inline BYTE GetDisp8( DWORD Eip, DWORD Offset );
WORD GetDisp16( DWORD Eip, DWORD Offset );
DWORD GetDisp32( DWORD Eip, DWORD Offset );

extern int ExceptionFunction( LPEXCEPTION_POINTERS e );

BYTE ReadMemory8( DWORD dwAddress );
void WriteMemory8( DWORD dwAddress, BYTE Value );

DWORD ReadMemory32( DWORD dwAddress );
void WriteMemory32( DWORD dwAddress, DWORD Value );

void EmuMMUSetHwWrite( x86MMUWriteDesc* pWriteDesc );
void EmuMMUClearWriteState( void );