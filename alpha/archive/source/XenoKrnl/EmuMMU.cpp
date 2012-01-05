/*
 * Xenoborg - Xbox Emulator
 * Copyright (C) 2006-2010  blueshogun96
 *
 * Name: EmuMMU.h
 * Desc: 
 *
 * Changelog: 
 *  5/2/11 - Added EmuMMUSave/RestoreExeContents() to save the memory range that 
 *		the .exe occupies, then restore it upon exiting the emulator.
 *
 *	4/25/11 - Fixed a bug in DecodeInstruction(). Forgot to default instruction
 *		size to 1 byte.
 *
 *	2/9/11 - Changed the hardware writing procedures a bit.  Now the hardware
 *			 write description is saved to communicate with the updating thread
 *			 to elliminate the multithreading problems with OpenGL.
 *
 *	10/14/2010 - Initial writing.
 *
 *	10/16/2010 - Added some preliminary memory map emulation code as well as the
 *				 instruction decoder for hardware emulation.  Added code to handle
 *				 the first 64kb of the address space plus the .xbe header so that
 *				 the .exe header doesn't need to be overwritten.
 */

#include "XenoKrnl.h"
#include "EmuMMU.h"
#include "EmuNV2A.h"
#include "Xbe.h"
#include "x86/x86OpTables.h"


// Globals
x86Instruction			Instruction;		// The description of the offending x86 instruction
LPEXCEPTION_POINTERS	pE = NULL;			// Global exception pointer
void*					pHeader = NULL;		// Xbe's header (so we won't have to overwrite the .exe's)
void*					pFirst64K = NULL;	// The very first 64kb of the address space (0-0xFFFF)
x86MMUWriteDesc			HwWriteDesc;		// Description of the write operation caught by the MMU.
BOOL					bHwWrite = FALSE;	// Has the hardware been written to?
void*					pExeData = NULL;	// The launcher (Xenoborg.exe) data before it was overwritten.


// Decodes an instruction caught by the MMU 
int DecodeInstruction( LPEXCEPTION_POINTERS e );

// Read one byte from memory
__inline BYTE GetDisp8( DWORD Eip, DWORD Offset )
{
	return ReadByte( Offset );
}

// Read one word from memory (two bytes converted to big endian)
__inline WORD GetDisp16( DWORD Eip, DWORD Offset )
{
	BYTE byte1 = ReadByte( Offset+1 );
	BYTE byte2 = ReadByte( Offset );
	WORD ret = ( byte1 << 8 ) | byte2;

	return ret;
}

// Read one dword from memory (four bytes converted to big endian)
__inline DWORD GetDisp32( DWORD Eip, DWORD Offset )
{
	BYTE byte1 = ReadByte( Offset+3 );
	BYTE byte2 = ReadByte( Offset+2 );
	BYTE byte3 = ReadByte( Offset+1 );
	BYTE byte4 = ReadByte( Offset );
	DWORD ret = ( byte1 << 24 ) | ( byte2 << 16 ) | ( byte3 << 8 ) | byte4;

	return ret;
}

__inline int ExceptionFunction( LPEXCEPTION_POINTERS e )
{
	char szExceptionString[256];

	Sleep(1);

	if( e->ExceptionRecord->ExceptionCode == 0x80000003 )
	{
		sprintf( szExceptionString, "Recieved Breakpoint exception (int 3) @ 0x%8.8X\n\n"
									"Would you like to continue?  (Click no to debug)",
			e->ContextRecord->Eip );
		int Result = MessageBoxA( NULL, szExceptionString, "Exception Information", MB_ICONERROR | MB_YESNO );

		if( Result == IDNO )
		{
			return EXCEPTION_CONTINUE_SEARCH;
		}
		else
		{
			e->ContextRecord->Eip += 1;
			return EXCEPTION_CONTINUE_EXECUTION;
		}
	}
	else
	{
		if( !DecodeInstruction(e) )
		{
			BYTE FirstByte = GetDisp8( e->ContextRecord->Eip, 0 );

			sprintf( szExceptionString, "Recieved Exception\n\nException Code: (0x%8.8X)\nAddress [0x%8.8X]\nFirst byte: 0x%2.2X", 
				e->ExceptionRecord->ExceptionCode, e->ContextRecord->Eip, FirstByte );

			int Result = MessageBoxA( NULL, szExceptionString, "Exception Information", MB_ICONERROR | MB_YESNO );

			if( Result == IDNO )
			{
				return EXCEPTION_CONTINUE_SEARCH;
			}
		}
	}

	// Update display if we VSync is ending
	EmuNV2AUpdateFB();

	return EXCEPTION_CONTINUE_EXECUTION;
}

__inline int DecodeInstruction( LPEXCEPTION_POINTERS e )
{
	// Clear instruction structure
	ZeroMemory( &Instruction, sizeof( x86Instruction ) );
	pE = e;

	// Get the actual location of the exception address
	Instruction.Eip = e->ContextRecord->Eip;

	// Read the first byte
	// TODO: Use opcode table instead of interpreter
	Instruction.Op = GetDisp8(Instruction.Eip, Instruction.Size++);

	// Get prefix bytes (if any)
	while( Instruction.Op == 0xF0 ||	// LOCK
		   Instruction.Op == 0xF2 ||	// REPNZ/REPNE
		   Instruction.Op == 0xF3 ||	// REP or REPE/REPZ
		   Instruction.Op == 0x2E ||	// CS segment override, branch not taken (with Jcc only)
		   Instruction.Op == 0x36 ||	// SS segment override
		   Instruction.Op == 0x3E ||	// DS segment override, branch taken (with Jcc only)
							// Commonly used when the Xbe is calling a kernel function.
		   Instruction.Op == 0x26 ||	// ES segment override
		   Instruction.Op == 0x64 ||	// FS segment override
		   Instruction.Op == 0x65 ||	// GS segment override
		   Instruction.Op == 0xF6 ||	// Operand size override
		   Instruction.Op == 0xF7 )		// Address size override
	{
		if( Instruction.Op == 0xF6 ) Instruction.OperandSize = 2;
		if( Instruction.Op == 0xF7 ) Instruction.AddressSize = 2;

		Instruction.PrefixBytes[Instruction.Prefixes++] = Instruction.Op;
		Instruction.Op = GetDisp8(Instruction.Eip, Instruction.Size++);
	}

	// Opcode size default is 1 byte
	Instruction.OpcodeSize = 1;

	// Check for 2 or 3 byte opcodes
	if( Instruction.Op == 0x0F )
	{
		Instruction.OpcodeSize = 2;
		Instruction.Op = GetDisp8(Instruction.Eip, Instruction.Size++);

		// Shouldn't get here since the Pentium III doesn't support any
		// 3 byte opcodes (afaik).
		if( Instruction.Op == 0x38 || Instruction.Op == 0x3A )
		{
			Instruction.OpcodeSize = 3;
			Instruction.Op = GetDisp8(Instruction.Eip, Instruction.Size++);
		}
	}

	// Emulate opcodes
	BOOL bRet = FALSE;

	switch( Instruction.OpcodeSize )
	{
	case 1:	// No escape bytes
		if( Instruction.OperandSize == 2 ) 
			bRet = x86_opcode_table_1byte[Instruction.Op].op16();
		else
			bRet = x86_opcode_table_1byte[Instruction.Op].op32();
		break;

	case 2:	// escape byte: 0x0F
		if( Instruction.OperandSize == 2 )
			bRet = x86_opcode_table_2byte[Instruction.Op].op16();
		else
			bRet = x86_opcode_table_2byte[Instruction.Op].op32();
		break;

	case 3:	// escape bytes: 0x38 or 0x3A
		{
			EmuWarning( "3-byte opcodes not supported!" );
		}
		break;
	}

	// Skip the entire instruction if we successfully decoded it
	if( bRet )
		e->ContextRecord->Eip += Instruction.Size;

	/*if( Op == 0xC7 )
	{
		BYTE modrm = ReadByte(InstructionSize++);

		if( reg_op(modrm) == 0 )
		{
			if( modrm == 0x80 )
			{
				DWORD Address, Value;

				Address = GetDisp32( Eip, InstructionSize );
				Value = GetDisp32( Eip, InstructionSize + 4 );

				InstructionSize += 8;
				PutPixel32( Address, Value );

				e->ContextRecord->Eip += InstructionSize;
				return 1;
			}
			else if( modrm == 0x00 )
			{
				DWORD Address, Value;

				Address = e->ContextRecord->Eax - 0xF0040240;
				Value = GetDisp32( Eip, InstructionSize );

				InstructionSize += 4;
				PutPixel32( Address, Value );

				e->ContextRecord->Eip += InstructionSize;
				return 1;
			}
			else
			{
				char string[32];
				sprintf( string, "modrm = 0x%2.2X", modrm );
				MessageBoxA( NULL, string, "", MB_OK );
			}
		}
	}*/

	return bRet;
}

//--------------------------------------------------------------------------------------
// Name: SupportSSE
// Desc: Checks host processor for SSE support.
//--------------------------------------------------------------------------------------
BOOL SupportSSE()
{
	DWORD dwFlags = 0;
	BOOL Supported = FALSE;

	// Detect SSE support on this CPU
	__asm
	{
		mov eax, 1	// CPUID function 1
		cpuid
		mov dwFlags, edx
	}

	Supported = ( dwFlags >> 25 ) & 0x1;

	return Supported;
}

//--------------------------------------------------------------------------------------
// Name: EmuMMUInit
// Desc: Initializes pointers that represent memory in the 4GB address space that cannot
//       be accessed normally.
//--------------------------------------------------------------------------------------
BOOL EmuMMUInit()
{
	// Allocate memory to store the .xbe's header (which is 0x1000 in size).  Since
	// I don't want to overwrite the .exe's header and cause it to cease functioning,
	// we can catch any reads or writes to 0x10000-0x10FFF and emulate the appropriate
	// actions necesarry.  Dxbx solves this problem by swapping in and out parts of the
	// .exe header so it continues running even with the .xbe header in memory.  I just
	// thought this would be a bit easier to manage =)

	// NOTE: In order for the above method to work, the memory range 0x10000 - 0x10FFF
	// needs to be protected from both read and write operations!
	if( !( pHeader = malloc( 0x1000 ) ) )
		return EmuError( "Error allocating seperate .xbe header!" );

	// Allocate memory for the first 64kb of memory in the 4GB address space (0x0000 - 0xFFFF).
	// In general, we can't actually get this space for ourselves, so we'll do the same for
	// the above scenario.
	if( !( pFirst64K = malloc( 0x10000 ) ) )
		return EmuError( "Error allocating first 64k!" );

	// TODO: Allocate space for ROM (0xFFF00000 - 0xFFFFFFFF) and some memory for the
	// random memory allocation done somewhere in 0x8xxxxxxx for Halo and Halo2.

	return TRUE;
}

//--------------------------------------------------------------------------------------
// Name: EmuMMUUninit
// Desc: Uninitializes special pointers.
//--------------------------------------------------------------------------------------
void EmuMMUUninit()
{
	if( pFirst64K )
	{
		free( pFirst64K );
		pFirst64K = NULL;
	}

	if( pHeader )
	{
		free( pHeader );
		pHeader = NULL;
	}
}

//--------------------------------------------------------------------------------------
// Name: EmuMMUSaveExeContents
// Desc: Saves the data that occupies the memory range the exe is residing in before we
//		 overwrite it with .xbe data.  We don't copy the header, leave that intact.
//--------------------------------------------------------------------------------------
void EmuMMUSaveExeContents()
{
	void* pBase = (void*) 0x11000;

	DbgPrintf( ">>> Backing up .exe memory range!\n" );

	// Allocate a pointer to hold the data starting at 0x11000.  I recommend saving everything
	// up until the end of the .rdata section.
	// TODO: These should not remain constant!
	pExeData = malloc( 0x271FF - 0x11000 );
	if( !pExeData )
		return;

	// Now, back up the .exe contents before emulation begins.
	memcpy( pExeData, pBase, 0x271FF - 0x11000 );
}

//--------------------------------------------------------------------------------------
// Name: EmuMMURestoreExeContents
// Desc: Restores the .exe contents to the base address.  Once again, we do not overwrite
//		 the header (0x10000 - 0x10FFF).
//--------------------------------------------------------------------------------------
void EmuMMURestoreExeContents()
{
	void* pBase = (void*) 0x11000;

	DbgPrintf( ">>> Restoring .exe to memory range!\n" );

	// If the .exe contents were saved, go ahead and restore the contents.  Afterwards, free
	// the buffer used to save it.
	if( pExeData )
	{
		memcpy( pBase, pExeData, 0x271FF - 0x11000 );
		free( pExeData );
	}
}

//--------------------------------------------------------------------------------------
// Name: EmuMMUSetHwWrite
// Desc: 
//--------------------------------------------------------------------------------------
void EmuMMUSetHwWrite( x86MMUWriteDesc* pWriteDesc )
{
	if( pWriteDesc )
	{
		bHwWrite = TRUE;

		memcpy( &HwWriteDesc, pWriteDesc, sizeof( x86MMUWriteDesc ) );
	}
}

//--------------------------------------------------------------------------------------
// Name: EmuMMUClewrWriteState
// Desc: 
//--------------------------------------------------------------------------------------
void EmuMMUClearWriteState( void )
{
	bHwWrite = FALSE;
	memset( &HwWriteDesc, 0, sizeof( x86MMUWriteDesc ) );
}

//--------------------------------------------------------------------------------------
// Name: ReadMemory8
// Desc: 
//--------------------------------------------------------------------------------------
BYTE ReadMemory8( DWORD dwAddress )
{
	if( dwAddress >= 0x00000000 && dwAddress < 0x00010000 )
		return ((BYTE*)pFirst64K)[dwAddress];

	if( dwAddress >= 0x00010000 && dwAddress < 0x00011000 )
		return ((BYTE*)pHeader)[dwAddress-0x10000];

	if( ( dwAddress >= 0xF0000000 && dwAddress < 0xF016C000 ) ||
		( dwAddress >= 0xFD000000 && dwAddress < 0xFDFFFFFF ) )
	{
		return EmuNV2ARead8( dwAddress );
	}

	DbgPrintf( "ReadMemory8: Unhandled:= [0x%.08X]\n", dwAddress );
	return 0;
}

//--------------------------------------------------------------------------------------
// Name: WriteMemory8
// Desc: 
//--------------------------------------------------------------------------------------
void WriteMemory8( DWORD dwAddress, BYTE Value )
{	
	x86MMUWriteDesc HwWrite;
	HwWrite.Address = dwAddress;
	HwWrite.Value = Value;
	HwWrite.Bits = 8;

	EmuMMUSetHwWrite( &HwWrite );

	if( dwAddress >= 0x00000000 && dwAddress < 0x00010000 )
	{
		((BYTE*)pFirst64K)[dwAddress] = Value;
		return;
	}

	if( dwAddress >= 0x00010000 && dwAddress < 0x00011000 )
	{
		((BYTE*)pHeader)[dwAddress-0x10000] = Value;
		return; 
	}

	if( ( dwAddress >= 0xF0000000 && dwAddress < 0xF016C000 ) ||
		( dwAddress >= 0xFD000000 && dwAddress < 0xFDFFFFFF ) )
	{
		EmuNV2AWrite8( dwAddress, Value );
		return; 
	}

	DbgPrintf( "WriteMemory8: Unhandled:= 0x%.08X -> [0x%.08X]\n", Value, dwAddress );
}

//--------------------------------------------------------------------------------------
// Name: 
// Desc: 
//--------------------------------------------------------------------------------------
DWORD ReadMemory32( DWORD dwAddress )
{
	if( dwAddress >= 0x00000000 && dwAddress < 0x00010000 )
		return ((DWORD*)pFirst64K)[dwAddress];

	if( dwAddress >= 0x00010000 && dwAddress < 0x00011000 )
		return ((DWORD*)pHeader)[dwAddress-0x10000];

	if( ( dwAddress >= 0xF0000000 && dwAddress < 0xF016C000 ) ||
		( dwAddress >= 0xFD000000 && dwAddress < 0xFDFFFFFF ) )
	{
		return EmuNV2ARead32( dwAddress );
	}

	DbgPrintf( "ReadMemory32: Unhandled:= [0x%.08X]\n", dwAddress );
	return 0;
}

//--------------------------------------------------------------------------------------
// Name: WriteMemory32
// Desc: 
//--------------------------------------------------------------------------------------
void WriteMemory32( DWORD dwAddress, DWORD Value )
{	
	x86MMUWriteDesc HwWrite;
	HwWrite.Address = dwAddress;
	HwWrite.Value = Value;
	HwWrite.Bits = 32;

	EmuMMUSetHwWrite( &HwWrite );

	if( dwAddress >= 0x00000000 && dwAddress < 0x00010000 )
	{
		((DWORD*)pFirst64K)[dwAddress] = Value;
		return;
	}

	if( dwAddress >= 0x00010000 && dwAddress < 0x00011000 )
	{
		((DWORD*)pHeader)[dwAddress-0x10000] = Value;
		return; 
	}

	if( ( dwAddress >= 0xF0000000 && dwAddress < 0xF016C000 ) ||
		( dwAddress >= 0xFD000000 && dwAddress < 0xFDFFFFFF ) )
	{
		EmuNV2AWrite32( dwAddress, Value );
		return; 
	}

	DbgPrintf( "WriteMemory32: Unhandled:= 0x%.08X -> [0x%.08X]\n", Value, dwAddress );
}

//DWORD get_ea16( BYTE modrm )
//{
//	if( modrm >= 0xC0 )
//	{
//		DbgPrintf( "***Warning: rm field is a register, not an address!\n" );
//		return 0x0;
//	}
//
//	switch( mod( modrm ) )
//	{
//	case 0:
//		switch( rm( modrm ) )
//		{
//		case 0: return REG_BX + REG_SI;	break;
//		case 1: return REG_BX + REG_DI; break;
//		case 2: return REG_BP + REG_SI;	break;
//		case 3: return REG_BP + REG_DI; break;
//		case 4: return REG_SI;			break;
//		case 5: return REG_DI;			break;
//		case 6: return GetDisp16( pE->ContextRecord->Eip, Instruction.Size++ );	Instruction.Size++; break;
//		case 7: return REG_BX;			break;
//		}
//
//	case 1:
//		{
//			BYTE disp8 = GetDisp8( pE->ContextRecord->Eip, Instruction.Size++ );;
//			
//			switch( rm( modrm ) )
//			{
//			case 0: return REG_BX + REG_SI + disp8;
//			case 1: return REG_BX + REG_DI + disp8;
//			case 2: return REG_BP + REG_SI + disp8;
//			case 3: return REG_BP + REG_DI + disp8; 
//			case 4: return REG_SI + disp8;
//			case 5: return REG_DI + disp8;
//			case 6: return REG_BP + disp8;
//			case 7: return REG_BX + disp8;
//			}
//		}
//
//	case 2:
//		{
//			WORD disp16 = get_disp16();
//
//			switch( rm( modrm ) )
//			{
//			case 0: return REG_BX + REG_SI + disp16;
//			case 1: return REG_BX + REG_DI + disp16;
//			case 2: return REG_BP + REG_SI + disp16;
//			case 3: return REG_BP + REG_DI + disp16; 
//			case 4: return REG_SI + disp16;
//			case 5: return REG_DI + disp16;
//			case 6: return REG_BP + disp16;
//			case 7: return REG_BX + disp16;
//			}
//		}
//	}
//
//	// Shouldn't get here!  
//#ifdef _DEBUG
//	__asm int 3
//#endif
//
//	return 0x0;
//}

DWORD get_scaled_index( BYTE mod, int ds );

DWORD get_ea32( BYTE modrm )
{
	if( modrm >= 0xC0 )
	{
		DbgPrintf( "***Warning: rm field is a register, not an address!\n" );
		return 0x0;
	}

	switch( mod( modrm ) )
	{
	case 0:
		{
			DWORD disp32 = GetDisp32( pE->ContextRecord->Eip, Instruction.Size );
			switch( rm( modrm ) )
			{
			case 0: return pE->ContextRecord->Eax;
			case 1: return pE->ContextRecord->Ecx;
			case 2: return pE->ContextRecord->Edx;
			case 3: return pE->ContextRecord->Ebx;
			case 4: return get_scaled_index( mod( modrm ), 0 );
			case 5: Instruction.Size += 4; return disp32;
			case 6: return pE->ContextRecord->Esi;
			case 7: return pE->ContextRecord->Edi;
			}
		}

	case 1:
		{
			BYTE disp8 = GetDisp8( pE->ContextRecord->Eip, Instruction.Size );

			// BUG FIX: Do not increment the instruction size until we
			// know that the byte we just got wasn't an SIB.
			if( rm( modrm ) != 4 )
				Instruction.Size++;
			else
			{
				// Get displacement ahead of time
				disp8 = GetDisp8( pE->ContextRecord->Eip, Instruction.Size+1 );
			}

			switch( rm( modrm ) )
			{
			case 0: return pE->ContextRecord->Eax + disp8;
			case 1: return pE->ContextRecord->Ecx + disp8;
			case 2: return pE->ContextRecord->Edx + disp8;
			case 3: return pE->ContextRecord->Ebx + disp8;
			case 4: 
				{
					//return get_scaled_index( mod( modrm ) ) + disp8;
					//DWORD ret = get_scaled_index( mod( modrm ), 1 );
					//Instruction.Size++;
					//return ret + disp8;
					return get_scaled_index( mod( modrm ), 1 );
				}
			case 5: return pE->ContextRecord->Ebp + disp8;
			case 6: return pE->ContextRecord->Esi + disp8;
			case 7: return pE->ContextRecord->Edi + disp8;
			}
		}

	case 2:
		{
			DWORD disp32 = GetDisp32( pE->ContextRecord->Eip, Instruction.Size );
			
			// BUG FIX: Do not increment the instruction size until we know
			// that we didn't need an SIB byte first.
			if( rm( modrm ) != 4 )
				Instruction.Size += 4;

			switch( rm( modrm ) )
			{
			case 0: return pE->ContextRecord->Eax + disp32;
			case 1: return pE->ContextRecord->Ecx + disp32;
			case 2: return pE->ContextRecord->Edx + disp32;
			case 3: return pE->ContextRecord->Ebx + disp32;
			case 4: return get_scaled_index( mod( modrm ), 2 );
			case 5: return pE->ContextRecord->Ebp + disp32;
			case 6: return pE->ContextRecord->Esi + disp32;
			case 7: return pE->ContextRecord->Edi + disp32;
			}
		}
	}

	return 0x0;
}

DWORD get_scaled_index( BYTE mod, int ds ) // ds = displacement size
{
	DWORD base;
	BYTE sib = GetDisp8( pE->ContextRecord->Eip, Instruction.Size++ );	// Get the SIB byte before the displacement
	DWORD disp;

	switch( ds )
	{
	case 0: disp = 0; break;
	case 1: disp = GetDisp8( pE->ContextRecord->Eip, Instruction.Size++ ); break;
	case 2: disp = GetDisp32( pE->ContextRecord->Eip, Instruction.Size ); Instruction.Size += 4; break;
	}

#if 0
	switch( mod )
	{
	case 0: base = GetDisp32( pE->ContextRecord->Eip, Instruction.Size ); /*Instruction.Size += 4;*/	break;
	case 1: base = pE->ContextRecord->Ebp + GetDisp8( pE->ContextRecord->Eip, Instruction.Size/*++*/ );	break;
	case 2: base = pE->ContextRecord->Ebp + GetDisp32( pE->ContextRecord->Eip, Instruction.Size ); /*Instruction.Size += 4;*/ break;
	}
#endif

	switch( base( sib ) )
	{
	case 0: base = pE->ContextRecord->Eax; break;
	case 1: base = pE->ContextRecord->Ecx; break;
	case 2: base = pE->ContextRecord->Edx; break;
	case 3: base = pE->ContextRecord->Ebx; break;
	case 4: base = pE->ContextRecord->Esp; break;
	case 5: base = 0; break;
	case 6: base = pE->ContextRecord->Esi; break;
	case 7: base = pE->ContextRecord->Edi; break;
	}

	switch( scale( sib ) )
	{
	case 0:
		switch( index( sib ) )
		{
		case 0: return pE->ContextRecord->Eax * 1 + base;
		case 1: return pE->ContextRecord->Ecx * 1 + base;
		case 2: return pE->ContextRecord->Edx * 1 + base;
		case 3: return pE->ContextRecord->Ebx * 1 + base;
		case 4: return 0 * 1 + base;
		case 5: return pE->ContextRecord->Ebp * 1 + base;
		case 6: return pE->ContextRecord->Esi * 1 + base;
		case 7: return pE->ContextRecord->Edi * 1 + base;
		}
		
	case 1:
		switch( index( sib ) )
		{
		case 0: return pE->ContextRecord->Eax * 2 + base;
		case 1: return pE->ContextRecord->Ecx * 2 + base;
		case 2: return pE->ContextRecord->Edx * 2 + base;
		case 3: return pE->ContextRecord->Ebx * 2 + base;
		case 4: return 0 * 2 + base;
		case 5: return pE->ContextRecord->Ebp * 2 + base;
		case 6: return pE->ContextRecord->Esi * 2 + base;
		case 7: return pE->ContextRecord->Edi * 2 + base;
		}

	case 2:
		switch( index( sib ) )
		{
		case 0: return base + pE->ContextRecord->Eax * 4 + disp;
		case 1: return base + pE->ContextRecord->Ecx * 4 + disp;
		case 2: return base + pE->ContextRecord->Edx * 4 + disp;
		case 3: return base + pE->ContextRecord->Ebx * 4 + disp;
		case 4: return base + 0 * 4 + disp;
		case 5: return base + pE->ContextRecord->Ebp * 4 + disp;
		case 6: return base + pE->ContextRecord->Esi * 4 + disp;
		case 7: return base + pE->ContextRecord->Edi * 4 + disp;
		}

	case 3:
		switch( index( sib ) )
		{
		case 0: return pE->ContextRecord->Eax * 8 /*+ base*/;
		case 1: return pE->ContextRecord->Ecx * 8 /*+ base*/;
		case 2: return pE->ContextRecord->Edx * 8 /*+ base*/;
		case 3: return pE->ContextRecord->Ebx * 8 /*+ base*/;
		case 4: return 0 * 8 /*+ base*/;
		case 5: return pE->ContextRecord->Ebp * 8 /*+ base*/;
		case 6: return pE->ContextRecord->Esi * 8 /*+ base*/;
		case 7: return pE->ContextRecord->Edi * 8 /*+ base*/;
		}
	}

	// Shouldn't get here
#ifdef _DEBUG
	__asm int 3
#endif

	return 0x0;
}