// XenoKrnl.cpp : Defines the entry point for the DLL application.
//

#include "XenoKrnl.h"
#include "EmuMain.h"
#include "Error.h"

//--------------------------------------------------------------------------------------
// Global
//--------------------------------------------------------------------------------------
BOOL	g_bNotExiting = TRUE;


//--------------------------------------------------------------------------------------
// Name: DllMain
// Desc: Entry point of the emulator.  When Xenoborg.exe and XenoKrnl.dll are loaded, we
//		 give total control to the XenoKrnl.dll and Xenoborg.exe is used to reserve the
//		 required memory range (0x10000 onwards, depending on how much Ram we are emulating)
//		 to load our .xbe files for direct code execution.
//--------------------------------------------------------------------------------------
BOOL APIENTRY DllMain( HMODULE hModule, DWORD dwReason, LPVOID lpReserved )
{
	switch( dwReason )
	{
	case DLL_PROCESS_ATTACH:
		// Initialize the emulator threads
		if( !InitEmuThreads() )
		{
			EmuError( "Error creating emulator threads!" );
			//exit(0);
			g_bNotExiting = FALSE;
		}
		break;
	case DLL_THREAD_ATTACH: break;
	case DLL_THREAD_DETACH: break;
	case DLL_PROCESS_DETACH:
		CloseEmuThreads();
		g_bNotExiting = FALSE;
		break;
	}

    return TRUE;
}


//--------------------------------------------------------------------------------------
// Name: XenoKrnlNullFunc
// Desc: The purpose of this function is to give Xenoborg.exe something to do outside of
//		 the .exe itself while we overwrite it with .xbe contents.  The trick is to res-
//		 tore the contents of the .exe prior to exiting.  Whether this is necessary or
//		 not remains to be seen...
//--------------------------------------------------------------------------------------
XENOKRNL_API void XenoKrnlNullFunc(void)
{
//	exit(0);
	while( g_bNotExiting );

	MessageBox( NULL, L"Exiting...", L"", MB_OK );
}
