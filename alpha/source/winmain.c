/* Common include files */
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#include "win32.h"

/* Globals */
struct window_t wnd;


int APIENTRY WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd )
{
	/* Check for memory leaks */
	_CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );

	if( !create_window( &wnd, 640, 480 ) )
		return -1;

	return main_loop();
}

#if 0 
int APIENTRY WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd )
{
	uint8_t test_code[] = 
	{
		0xB8,	// mov eax, 0xDEADBEEF
		0xEF,
		0xBE,
		0xAD,
		0xDE,
		0x90,	// nop
		0xCC
	};

	/* Signify the beginning of the test */
	MessageBoxA( NULL, "Xenoborg libemu test 8/21/11", "Test", MB_OK );

	/* Create a new instance of the x86 emulator */
	e = emu_new();
	if(!e)
	{
		char string[128];
		sprintf( string, "emu_new(): %s", emu_strerror(e) );
		msg_box( string, MB_OK );
		emu_free(e);
		return -1;
	}

	/* Set up the default register values */
	emu_cpu_reg32_set( emu_cpu_get(e), eax, 0 );
	emu_cpu_reg32_set( emu_cpu_get(e), ecx, 0 );
	emu_cpu_reg32_set( emu_cpu_get(e), edx, 0x00000600 );
	emu_cpu_reg32_set( emu_cpu_get(e), ebx, 0 );
	emu_cpu_reg32_set( emu_cpu_get(e), esp, 0 );
	emu_cpu_reg32_set( emu_cpu_get(e), ebp, 0 );
	emu_cpu_reg32_set( emu_cpu_get(e), esi, 0 );
	emu_cpu_reg32_set( emu_cpu_get(e), edi, 0 );

	emu_cpu_eflags_set( emu_cpu_get(e), 0x00000002 );

	emu_cpu_eip_set( emu_cpu_get(e), 0x11000 );

	/* Give the emulator some code to chew on */
	emu_memory_write_block( emu_memory_get(e), emu_cpu_eip_get( emu_cpu_get(e) ), test_code, sizeof( uint8_t ) * 7 );

	/* Try executing the first instruction */
	if( emu_cpu_parse( emu_cpu_get(e) ) )
	{
		char string[128];
		sprintf( string, "emu_cpu_parse(): %s", emu_strerror(e) );
		msg_box( string, MB_OK );
	}
	else
	{
		uint32_t Eax;
		char string[128];

		emu_cpu_step( emu_cpu_get(e) );
		Eax = emu_cpu_reg32_get( emu_cpu_get(e), eax );
		sprintf( string, "emu_cpu_parse(): Eax = 0x%.08X", Eax );
		msg_box( string, MB_OK );
	}

//	emu_cpu_run(emu_cpu_get(e));

	/* Free up the x86 emulator instance */
	emu_free(e);

	/* Signify the end of the test */
	msg_box( "Test complete.", MB_OK );

	return 0;
}
#endif