/*
 * Xenoborg - Xbox Emulator
 * Copyright (C) 2006-2010  blueshogun96
 *
 * Name: 
 * Desc: 
 *
 * Changelog: 
 *	5/2/11 - Stopped the emulator from crashing after emulation was started
 *		(see EmuMMU.cpp). Also added code to halt the currently running threads
 *		but not the main thread.
 *
 *	4/25/11 - Added menu functionality code and changed CreateThread to 
 *		_beginthreadex. Emu doesn't exit properly without a hard exit.
 *
 *  9/21/2010 - Doing a major overhaul on the overall code architecture
 *				of the emulator in general. For now, we won't use DirectDraw
 *				for VBlank detection until I can write a better implementation
 *				if needed.
 *
 *	5/1/2010 - Initial writing.
 */


#include "XenoKrnl.h"
#include "DbgConsole.h"
#include "EmuMain.h"
#include "Error.h"
#include "resource.h"



//--------------------------------------------------------------------------------------
// Global
//--------------------------------------------------------------------------------------
HANDLE			g_hUpdateThread = NULL;		// Handle to the update thread.
HANDLE			g_hExecutionThread = NULL;	// Handle to the code execution thread
HANDLE			g_hEmuThread	= NULL;		// Handle to the main emulator thread.
BOOL			g_bEmuIsRunning = FALSE;	// Is the emulator actually running?
HWND			g_EmuWnd		= NULL;		// The handle to the emulation window.

BOOL bVBlank = FALSE;
BOOL bVSync = FALSE;


//--------------------------------------------------------------------------------------
// Name: OpenXbeFile
// Desc: Allows the user to search for any given .xbe file to use with the emulator.
//--------------------------------------------------------------------------------------
bool OpenXbeFile( void )
{
	char filter[256] = {0};
	char filename[256] = {0};
	OPENFILENAMEA File;

	if( Xbe.m_bXbeHasBeenOpened )
		XBEUnload();

	strcat( filter, "XBox Executable Files (*.xbe)" );
	ZeroMemory( &File, sizeof( OPENFILENAME ) );

	int index = lstrlenA( filter ) + 1;
	filter[index++] = '*';
	filter[index++] = '.';
	filter[index++] = 'x';
	filter[index++] = 'b';
	filter[index++] = 'e';

	File.lStructSize = sizeof( OPENFILENAME );
	File.hwndOwner = GetActiveWindow();
	File.lpstrFilter = filter;
	File.lpstrFile = filename;
	File.nMaxFile = 256;
	File.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_NOCHANGEDIR;

	if( !GetOpenFileNameA( &File ) )
		return EmuError( "Unable to load file.\n" );

	// Back up the .exe data before executing
	EmuMMUSaveExeContents();

	// Now, actually load the .xbe into memory.
	if( !XBELoad( File.lpstrFile ) )
	{
		EmuError( "Unable to load Xbe file!\n" );
		return 0;
	}

	// Hijack kernel functions
	HookKrnlFunctions();

	// Activate other threads
	ResumeThread( g_hExecutionThread );
	ResumeThread( g_hUpdateThread );

	return true;
}

//--------------------------------------------------------------------------------------
// Name: ProcessMenuActivity
// Desc: Responds accordingly to clicks on the menu.
//--------------------------------------------------------------------------------------
void ProcessMenuActivity( WORD wMenuID, HWND hWnd )
{
	// Interpret appropriate action per menu option
	switch( wMenuID )
	{
	case ID_FILE_EXIT:	// Exits the emulator
		PostMessage( hWnd, WM_DESTROY, 0, 0 );
		break;

	case ID_EXECUTE_XBE:	// Open an xbe file and start emulating
		OpenXbeFile();
		break;
	}
}

//--------------------------------------------------------------------------------------
// Name: EmuWindowProcedure
// Desc: Window event handler for the UI and rendering window.
//--------------------------------------------------------------------------------------
LRESULT CALLBACK EmuWindowProcedure( HWND hWnd, UINT uMessage, WPARAM wParam, LPARAM lParam )
{
	switch( uMessage )
	{
	case WM_CREATE:
		return 0;

	case WM_CLOSE:
	case WM_DESTROY:
		HaltEmuThreads();
		XBEUnload();
		EmuMMURestoreExeContents();
		exit(0);
//		PostQuitMessage(0);
		return 0;

	case WM_KEYDOWN:
		/*switch( wParam )
		{
		case VK_F1:
			ResumeThread( g_hUpdateThread );
			ResumeThread( g_hExecutionThread );
			break;

		case VK_F2:
			SuspendThread( g_hUpdateThread );
			SuspendThread( g_hExecutionThread );
			break;
		}*/
		return 0;

	case WM_COMMAND:
		ProcessMenuActivity( (WORD) LOWORD( wParam ), hWnd );
		return 0;

	case WM_SYSCOMMAND:
		switch( wParam )
		{
		case SC_MONITORPOWER:
		case SC_SCREENSAVE:
			return 0;
		}
		break;
	}

	return DefWindowProc( hWnd, uMessage, wParam, lParam );
}

//--------------------------------------------------------------------------------------
// Name: EmuWindowCreate
// Desc: Creates the emulation window.
//--------------------------------------------------------------------------------------
bool EmuWindowCreate( void )
{
	WNDCLASSEX wc;
	wc.cbSize			= sizeof( WNDCLASSEX );
	wc.style			= CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.cbClsExtra		= 0;
	wc.cbWndExtra		= 0;
	wc.hbrBackground	= (HBRUSH) GetStockObject( BLACK_BRUSH );
	wc.hCursor			= LoadCursor( NULL, IDC_ARROW );
	wc.hIcon			= LoadIcon( NULL, IDI_APPLICATION );
	wc.hIconSm			= wc.hIcon;
	wc.hInstance		= GetModuleHandle( NULL );
	wc.lpfnWndProc		= EmuWindowProcedure;
	wc.lpszClassName	= L"Xenoborg";
	wc.lpszMenuName		= MAKEINTRESOURCE(IDR_MENU1);//NULL;

	RegisterClassEx( &wc );

	// Create the window.
	// TODO: Adjust window dimensions to compensate for title bar and menu size.
	g_EmuWnd = CreateWindowEx( WS_EX_APPWINDOW | WS_EX_WINDOWEDGE, 
								L"Xenoborg", L"Xenoborg", WS_OVERLAPPEDWINDOW/* & (~WS_MAXIMIZEBOX)*/,
								CW_USEDEFAULT, CW_USEDEFAULT, 640, 480, 
								NULL, NULL, GetModuleHandle( NULL ), NULL ); 
	if( !g_EmuWnd )
		return false;

	// Show the window
	ShowWindow( g_EmuWnd, SW_SHOW );
	UpdateWindow( g_EmuWnd );
	SetForegroundWindow( g_EmuWnd );
	SetFocus( g_EmuWnd );
	SetActiveWindow( g_EmuWnd );

//	char string[128];
//	sprintf( string, "Xenoborg: %S", Xbe.m_XbeCertificate.wcTitle );
//	SetWindowTextA( GetActiveWindow(), string );

	return true;
}

//--------------------------------------------------------------------------------------
// Name: EmuThread
// Desc: This is the emulator's main thread. Handles initialization of the emulator's
//		 window and other components that are required to make it run.
//--------------------------------------------------------------------------------------
unsigned int WINAPI EmuThread( void* Params )
{
	MSG msg;

	// Initialize debug console
	if( DbgConsoleInit() )
		DbgPrintf( " Xenoborg: Emulator thread started (Thread ID: 0x%X)\n", 
					GetCurrentThreadId() );

	// Initialize the window
	if( !EmuWindowCreate() )
	{
		EmuError( "Error creating main window!" );
		return 0;
	}

	// Message pump
	while( GetMessage( &msg, NULL, 0, 0 ) )
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	
	DestroyWindow( g_EmuWnd );

	return 0;
}
		
//--------------------------------------------------------------------------------------
// Name: ExecutionThread
// Desc: Handles the execution of x86 code within the exception handling block.  When an
//		 exception is caught, the exception handling function can either perform the 
//		 necessary hardware emulation equivelant to the task being executed and continue
//		 execution, or exit the thread altogether.
//--------------------------------------------------------------------------------------
unsigned int WINAPI ExecutionThread( LPVOID Params )
{
	// Signifiy (in the debug console) that the code execution thread has started and show
	// the thread's ID number for debugging purposes.
	DbgPrintf( ">>> Execution thread started (Thread ID: 0x%X)\n", GetCurrentThreadId() );

	if( !EmuNV2AInit() )
		return -1;

	// Set the emulator as running
	g_bEmuIsRunning = TRUE;

	// Now create a function pointer that points to the entry point of
	// the .xbe file.
	void (*Execute)(void) = (void(*)(void)) Xbe.m_dwActualEntryPoint;

	// Execute the code within the .xbe file and catch hardware read/write
	// operations via MMU (see EmuMMU.cpp)
	__try
	{
		Execute();
	}
	__except( ExceptionFunction( GetExceptionInformation() ) )
	{
		MessageBoxW( NULL, L"Exception occured!", L"Exception", MB_ICONERROR );
	}

	EmuError( "Exiting execution thread...\n" );

	return 0;
}

//--------------------------------------------------------------------------------------
// Name: UpdateThread
// Desc: This thread is responsible for handling graphics, sound, user input, networking,
//		 etc. after the required Windows API components are initialized (also done in this 
//		 thread to reduce conflicts with other threads sharing resources).  This thread also
//		 keeps track of time sensitive operations such as VBlank emulation keeps track of 
//		 the tick count for the HLE kernel.
//--------------------------------------------------------------------------------------
unsigned int WINAPI UpdateThread( LPVOID Params )
{
	// Signifiy (in the debug console) that the emulator update thread has started and show
	// the thread's ID number for debugging purposes.
	DbgPrintf( ">>> Update thread started (Thread ID: 0x%X)\n", GetCurrentThreadId() );

	// Wait until the emulator is ready to run.
	while( !g_bEmuIsRunning );

	while( TRUE )
	{
		// Swap VBlank status
		bVBlank = !bVBlank;

		// If we are in VBlank, set the VSync flag
		if( bVBlank )
			bVSync = TRUE;

		// Is Vsync ending?
		if( !bVBlank && bVSync )
		{
			// If so, end it. 
		//	bVSync = FALSE;
		}

		// Update KeTickCount
		KeTickCount = GetTickCount();

		// Yield time to other threads
		Sleep(1);
	}

	return 0;
}

//--------------------------------------------------------------------------------------
// Name: InitEmuThreads
// Desc: Initializes the code execution and VBlank timing emulation threads.
//--------------------------------------------------------------------------------------
bool InitEmuThreads()
{
	if( g_bEmuIsRunning )
		return false;

	// Now actually create handles to the threads
	g_hEmuThread = (HANDLE) _beginthreadex( NULL, 0, EmuThread, NULL, CREATE_SUSPENDED, NULL );
	g_hExecutionThread = (HANDLE) _beginthreadex( NULL, 0, ExecutionThread, NULL, CREATE_SUSPENDED, NULL );
	g_hUpdateThread = (HANDLE) _beginthreadex( NULL, 0, UpdateThread, NULL, CREATE_SUSPENDED, NULL ); 

	// Activate main thread
	ResumeThread( g_hEmuThread );
//	ResumeThread( g_hUpdateThread );
//	ResumeThread( g_hExecutionThread );

	return true;
}

//--------------------------------------------------------------------------------------
// Name: CloseEmuThreads
// Desc: Closes the active threads.
//--------------------------------------------------------------------------------------
void CloseEmuThreads()
{
	DbgPrintf( "Closing emu threads...\n" );
	Sleep( 1000 );

	g_bEmuIsRunning = FALSE;

	if( g_hExecutionThread )
	{
		CloseHandle( g_hExecutionThread );
		g_hExecutionThread = NULL;
	}

	if( g_hUpdateThread )
	{
		CloseHandle( g_hUpdateThread );
		g_hUpdateThread = NULL;
	}

	EmuNV2AUninit();

	if( g_hEmuThread )
	{
		CloseHandle( g_hEmuThread );
		g_hEmuThread = NULL;
	}
}

//--------------------------------------------------------------------------------------
// Name: HaltEmuThreads
// Desc: Halts the active threads (does not halt the main thread!)
//--------------------------------------------------------------------------------------
void HaltEmuThreads()
{
	if( g_hExecutionThread )
		SuspendThread( g_hExecutionThread );

	if( g_hUpdateThread )
		SuspendThread( g_hUpdateThread );
}
