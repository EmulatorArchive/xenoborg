#include "win32.h"
#include "gl_drv.h"
#include "xbox_emu.h"
#include "xbox\\xbe.h"

/* LibEmu (x86) includes */
#include <emu/emu.h>
#include <emu/emu_memory.h>
#include <emu/emu_cpu.h>



struct xbox_emu_t xbox;
int launch_bios = No;
int enable_debugger = No;


void emulation_uninit();
extern void show_debugger();

/* Sets the current state of the emulator */
void set_emulator_state( enum emu_state es )
{
	xbox.state = es;
}

/* Opens the bios file and determine it's size */
int open_bios( char* filename, int preload )
{
	struct _iobuf* f = NULL;
	int ret = 1;

	/* sanity check(s) */
	if( xbox.bios.ptr )
		free( xbox.bios.ptr );

	/* Open the bios file, get it's file size, validate that the file size is correct
	   and if we're not preloading it, then read it into a buffer */

	f = fopen( filename, "rb" );
	if(!f)
	{
		msg_box( "Could not open BIOS file!", MB_OK );
		ret = 0;
		goto stop;
	}

	fseek(f, 0, SEEK_END);
	xbox.bios.size = ftell(f);

	/* Xbox bioses are always 256kb, 512kb or 1mb */
	/* TODO: Verify that only Chihiro has a 512kb bios */
	if( xbox.bios.size != 1024*256 && xbox.bios.size != 1024*512 && xbox.bios.size != 1024*1024 )
	{
		msg_box( "BIOS file size is incorrect!", MB_OK );
		ret = 0;
		goto stop;
	}

	/* If we're preloading, stop here */
	if( preload )
		goto stop;

	/* Put it into a buffer */
	xbox.bios.ptr = malloc( xbox.bios.size );
	fread( xbox.bios.ptr, xbox.bios.size, 1, f );

	/* TODO: Do a CRC check to determine what version this BIOS is unless the version
	   number is somewhere in the BIOS itself. I haven't checked so I don't know... */

stop:
	fclose(f);

	return ret;
}

/* Closes the bios */
void close_bios()
{
	printf( "Closing BIOS...\n" );

	if( xbox.bios.ptr )
		free( xbox.bios.ptr );

	memset( &xbox.bios, 0, sizeof( struct xbox_bios_t ) );
}

/* */
int open_xbe( char* filename )
{
	/* sanity check */
	if( xbox.xbe.m_bXbeHasBeenOpened )
		xbe_unload( &xbox.xbe );

	/* Load .xbe file */
	if( !xbe_load( filename, &xbox.xbe ) )
		return 0;

	return 1;
}

/* Uninitialize xbe resources */
void close_xbe()
{
	xbe_unload( &xbox.xbe );
}

/* Setup memory handlers before emulation begins */
void setup_memory_handlers()
{
	struct emu_memory_handler m;

	/* NV2A (framebuffer)*/
	memset( &m, 0, sizeof( m ) );
	m.mem_start = fb_start;
	m.mem_end = fb_start + ( 640 * 480 * 4 );
	m.read_only_access = 0;
	m.executable = 0;
	m.read8 = nv2a_read8;
	m.write8 = nv2a_write8;
	m.read16 = nv2a_read16;
	m.write16 = nv2a_write16;
	m.read32 = nv2a_read32;
	m.write32 = nv2a_write32;
	emu_memory_add_handler( emu_memory_get(xbox.e), &m );

	m.mem_start = fb_start2;
	m.mem_end = fb_start2 + ( 640 * 480 * 4 );
	emu_memory_add_handler( emu_memory_get(xbox.e), &m );
}

/* Initializes the emulator components */
int emulation_init()
{
	uint32_t i = 0;

	/* Is the emulator already running? */
	if( xbox.state != emu_state_stopped )
		return 0;

	/* Verify that the .xbe was loaded first */
	if( launch_bios == No && !xbox.xbe.m_bXbeHasBeenOpened )
	{
		msg_box( "No xbe loaded!", MB_OK );
		return 0;
	}

	/* Get parent window */
	if( !w32_get_parent_window( &xbox.wnd ) )
		return 0;

	/* Initialize OpenGL */
	if( !enable_ogldrv( &xbox.wnd ) )
	{
		disable_ogldrv();
		msg_box( "Error initializing OpenGL!", MB_OK );
		return 0;
	}

	/* Clear the screen once */
	ogl_clear_colour_buffer();
	ogl_swap_buffers();

	/* Create a new instance of the x86 emulator */
	xbox.e = emu_new();
	if(!xbox.e)
	{
		char string[128];
		sprintf( string, "emu_new(): %s", emu_strerror(xbox.e) );
		msg_box( string, MB_OK );
		emu_free(xbox.e);
		return 0;
	}

	/* Set up memory handlers for memory mapped I/O devices */
	setup_memory_handlers();

	/* Set up the default register values */
	emu_cpu_reg32_set( emu_cpu_get(xbox.e), eax, 0 );
	emu_cpu_reg32_set( emu_cpu_get(xbox.e), ecx, 0 );
	emu_cpu_reg32_set( emu_cpu_get(xbox.e), edx, 0x00000600 );
	emu_cpu_reg32_set( emu_cpu_get(xbox.e), ebx, 0 );
	emu_cpu_reg32_set( emu_cpu_get(xbox.e), esp, 1024 );	/* TODO */
	emu_cpu_reg32_set( emu_cpu_get(xbox.e), ebp, 0 );
	emu_cpu_reg32_set( emu_cpu_get(xbox.e), esi, 0 );
	emu_cpu_reg32_set( emu_cpu_get(xbox.e), edi, 0 );
	emu_cpu_eflags_set( emu_cpu_get(xbox.e), 0x00000002 );

	if( launch_bios )
	{
		/* Set EIP to 0xFFFFFFF0 */
		emu_cpu_eip_set( emu_cpu_get(xbox.e), 0xFFFFFFF0 );

		/* Write bios contents to 0xFF000000 and mirror as many times as appropriate */
		while( i < 1024*1024*16 )
		{
			emu_memory_write_block( emu_memory_get(xbox.e), 0xFF000000+i, xbox.bios.ptr, xbox.bios.size );
			i += xbox.bios.size;
		}
	}
	else
	{
		/* Set EIP to entry point */
		emu_cpu_eip_set( emu_cpu_get(xbox.e), xbox.xbe.m_dwActualEntryPoint );

		/* Write .xbe contents starting at the base address */
		emu_memory_write_block( emu_memory_get(xbox.e), xbox.xbe.m_XbeHeader.dwBaseAddr,
			xbox.xbe.m_pXbeData, xbox.xbe.m_lXbeFileSize );
	}

#if 0
	/* TODO: Create patch system */
	if( xbox.xbe.crc == 0x5C524785 )
	{
		/* NOPs placed over jbe short 0x11070 @ [0x11074] */
		printf( "Applying patches...\n" );
		emu_memory_write_byte( emu_memory_get(xbox.e), 0x11074, 0x90 );
		emu_memory_write_byte( emu_memory_get(xbox.e), 0x11075, 0x90 );
		emu_memory_write_byte( emu_memory_get(xbox.e), 0x1107A, 0x90 );
		emu_memory_write_byte( emu_memory_get(xbox.e), 0x1107B, 0x90 );
	}
#endif

	/* Initialize fake kernel thunk */
	init_kernel_thunk();

	/* Set uninitialization function to go off at exit */
	//atexit( (void (__cdecl *)(void)) emulation_uninit );

	xbox.state = emu_state_running;

	return 1;
}

/* Uninitialize emulator components */
void emulation_uninit()
{
	/* Close the xbe */
	close_xbe();

	/* Uninitialize cpu */
	emu_free(xbox.e);

	/* Uninitialize OpenGL */
	disable_ogldrv();
}

/* Executes the CPU core */
void execute_cpu()
{
	/* Attempt to parse the instruction EIP points to */
	if( emu_cpu_parse( emu_cpu_get(xbox.e) ) )
	{
		printf( "emu_cpu_parse(): %s", emu_strerror(xbox.e) );
		//msg_box( string, MB_OK );
	}
	else
	{
		/* Now actually emulate the instruction */
		if( emu_cpu_step( emu_cpu_get(xbox.e) ) )
		{
			printf( "emu_cpu_step(): %s", emu_strerror(xbox.e) );
			//msg_box( string, MB_OK );
		}
	}
}

/* Emulator starts executing here */
void* emulation_thread( void* params )
{
	/* Initialize emulator */
	if( !emulation_init() )
		return NULL;

	/* Reset the timer */
	timer_reset();

	/* Emulation loop */
	while( TRUE )
	{
		/* Handle emulator states */
		if( xbox.state == emu_state_stopped ) break;
		if( xbox.state == emu_state_halted ) {}

		/* Execute CPU */
		execute_cpu();

		/* Update screen based on the timer value */
		if( (((float) timer_get_current_time()) / ( 1000.0f * 1000.0f ) ) >= XBOX_CPU_SPEED )
		{
			/* Update frame buffer */
			ogl_update_fb();
			ogl_swap_buffers();

			/* Reset timer */
			timer_reset();
		}

		/* Update HLEd tick count */
		impl_KeTickCount = GetTickCount();
	}

	/* Uninitialize emulator */
	emulation_uninit();

	msg_box( "Emulation thread terminated!", MB_OK );

	/* Exit thread */
//	pthread_exit(NULL);

	return NULL;
}

void populate_debugger_list( HWND hwnd, uint32_t list )
{	
	char string[128];
	void* mem = xbox.e->memory->mem;
	uint32_t i = 0;

	for( i = 0; i < (xbox.xbe.m_lXbeFileSize); i++ )
	{
		sprintf( string, "0x%X: 0x%.02X", i+0x10000, /*((uint8_t*)mem)[i+0x11000]*/ xbox.xbe.m_pXbeData[i] );
		SendDlgItemMessage( hwnd, list, LB_ADDSTRING, 0, (LPARAM) string );
	}
}

/* Initialize the emulation thread */
int initialize_emulation_thread()
{
	/* Initialize the emulation thread */
	int ret = pthread_create( &xbox.emulation_thread_handle, NULL, emulation_thread, NULL );
	if( ret )
	{
		msg_box( "Error creating POSIX thread!", MB_OK );
		return 0;
	}

	/* Enable the debugger if desired */
	if( enable_debugger )
	{
		/* Small delay */
		Sleep( 500 );
		show_debugger();
	}

	return 1;
}