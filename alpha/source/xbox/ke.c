#include "..\\win32.h"
#include "..\\xbox_emu.h"
#include "xbe.h"
#include "ke.h"

/* LibEmu (x86) includes */
#include <emu/emu.h>
#include <emu/emu_memory.h>
#include <emu/emu_cpu_data.h>
#include <emu/emu_cpu.h>


/* This is a test to see whether HLE-ing the kernel is feasable or not for the
   time being until full blown BIOS emulation is possible. */
struct xbox_emu_t xbox;

/* The kernel function being accessed */
uint32_t current_function = 0;


/* This keeps track of what kernel function is actually being accessed */
/* in case we miss one */
void set_current_kernel_function_address( uint32_t addr )
{
	current_function = addr;
}


/*
 * High-Level Emulated kernel functions
 */

/* DbgPrint 0x008 */
void impl_DbgPrint()
{
	char	 string[256];
	uint32_t addr;
	int		 i = 0;

	/* Get the return address, reset the eip */
	emu_memory_read_dword( emu_memory_get(xbox.e), 
		emu_cpu_reg32_get( emu_cpu_get( xbox.e ), esp ), 
		&addr );
	emu_cpu_get( xbox.e )->reg[esp] += 4;
	emu_cpu_get( xbox.e )->eip = addr;


	/* Pop params off the stack */
	emu_memory_read_dword( emu_memory_get(xbox.e), 
		emu_cpu_reg32_get( emu_cpu_get( xbox.e ), esp ), 
		&addr );
	emu_cpu_get( xbox.e )->reg[esp] += 4;

	/* Get the string bytes until we reach a 0 byte */
	while( i < 256 )
	{
		emu_memory_read_byte( emu_memory_get( xbox.e ), addr+i, (uint8_t*) &string[i] );

		if( i >= 255 || string[i] == 0 ) break;
		i++;
	}

	printf( "DbgPrintf >>> %s\n", string );
}

/* KeTickCount 0x09C */
DWORD impl_KeTickCount = 0;

/* HalInitiateShutdown 0x168 */
void impl_HalInitiateShutdown()
{
	printf( "HalInitiateShutdown() >>> Xbox has shutdown!\n" );

	/* TODO */
//	while(TRUE);
}

/* Unimplemented */
void impl_Unimplemented()
{
	printf( "Unimplemented kernel function! (0x%x)\n", current_function );

	/* TODO */
//	while(TRUE);
}


/* Kernel thunk table */
void (*impl_KernelThunk[336])() = { NULL };


void init_kernel_thunk()
{
	int i = 0;

	/* Initialize kernel thunk */
	while( i < 337 )
	{
		impl_KernelThunk[i] = impl_Unimplemented;
		i++;
	}

	impl_KernelThunk[0] = NULL;
	impl_KernelThunk[0x008] = impl_DbgPrint;
	impl_KernelThunk[0x168] = impl_HalInitiateShutdown;
}