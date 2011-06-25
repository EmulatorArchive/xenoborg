/*
 * Xenoborg - Xbox Emulator
 * Copyright (C) 2006-2010  blueshogun96
 *
 * Name: 
 * Desc: 
 *
 * Changelog: 
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

#include <GL\glew.h>
#include <GL\wglew.h>


//--------------------------------------------------------------------------------------
// Globals
//--------------------------------------------------------------------------------------
struct WindowData
{
	HDC hDC;
	HGLRC hRC;
	GLuint PixelFormat;
	PIXELFORMATDESCRIPTOR pfd;
	HWND hWnd;
};

//--------------------------------------------------------------------------------------
// Global
//--------------------------------------------------------------------------------------
HANDLE			g_hUpdateThread = NULL;		// Handle to the update thread.
HANDLE			g_hExecutionThread = NULL;	// Handle to the code execution thread
HANDLE			g_hEmuThread	= NULL;		// Handle to the main emulator thread.
BOOL			g_bEmuIsRunning = FALSE;	// Is the emulator actually running?
HWND			g_EmuWnd		= NULL;		// The handle to the emulation window.
WindowData		WndData;

static BOOL bVBlank = FALSE;
static BOOL bVSync = FALSE;


//--------------------------------------------------------------------------------------
// Name: InitOpenGL
// Desc: Initializes OpenGL
//--------------------------------------------------------------------------------------
BOOL InitOpenGL( WindowData* wndData )
{
	ZeroMemory( &wndData->pfd, sizeof( PIXELFORMATDESCRIPTOR ) );
	wndData->pfd.nSize = sizeof( PIXELFORMATDESCRIPTOR );
	wndData->pfd.nVersion = 1;
	wndData->pfd.cColorBits = 32;
	wndData->pfd.cDepthBits = 16;
	wndData->pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	wndData->pfd.iPixelType = PFD_TYPE_RGBA;
	wndData->pfd.iLayerType = PFD_MAIN_PLANE;

	wndData->hDC = GetDC( wndData->hWnd );
	if( !wndData->hDC )
	{
		EmuError( "Error getting DC!\n" );
		return FALSE;
	}

	wndData->PixelFormat = ChoosePixelFormat( wndData->hDC, &wndData->pfd );
	if( wndData->PixelFormat == 0 )
	{
		EmuError( "Error choosing pixel format!\n" );
		return FALSE;
	}

	if( !SetPixelFormat( wndData->hDC, wndData->PixelFormat, &wndData->pfd ) )
	{
		EmuError( "Error setting pixel format!\n" );
		return FALSE;
	}

	wndData->hRC = wglCreateContext( wndData->hDC );
	if( !wndData->hRC )
	{
		EmuError( "Error creating RC!\n" );
		return FALSE;
	}

	if( !wglMakeCurrent( wndData->hDC, wndData->hRC ) )
	{
		EmuError( "Error setting contexts!\nGetLastError(): 0x%.08X\n", GetLastError() );
		return FALSE;
	}

	if( glewInit() != GLEW_OK )
	{
		EmuError( "Initializing GLEW!\n" );
		return FALSE;
	}

	wglSwapIntervalEXT( GL_FALSE );

	return TRUE;
}

//--------------------------------------------------------------------------------------
// Name: UninitOpenGL
// Desc: Uninitializes OpenGL
//--------------------------------------------------------------------------------------
void UninitOpenGL( WindowData* wndData )
{
	if( wndData->hRC )
	{
		wglMakeCurrent( NULL, NULL );
		wglDeleteContext( wndData->hRC );
		wndData->hRC = NULL;
	}

	if( wndData->hDC )
	{
		ReleaseDC( wndData->hWnd, wndData->hDC );
		wndData->hDC = NULL;
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
		/*{
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
			{
				DbgPrintf( "Unable to load file.\n" );
				return 0;
			}

			if( !XBELoad( File.lpstrFile ) )
			{
				EmuError( "Unable to load Xbe file!\n" );
				return 0;
			}

			// Activate other threads
			ResumeThread( g_hExecutionThread );
			ResumeThread( g_hUpdateThread );
		}*/
		return 0;

	case WM_CLOSE:
	case WM_DESTROY:
//		XBEUnload();
		exit(0);
//		PostQuitMessage(0);
		return 0;

	case WM_KEYDOWN:
		switch( wParam )
		{
		case VK_F1:
		//	ResumeThread( g_hInterruptThread );
		//	ResumeThread( g_hExecutionThread );
			break;

		case VK_F2:
		//	CloseHandle( g_hInterruptThread );
		//	CloseHandle( g_hExecutionThread );
			break;
		}
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
	wc.lpszMenuName		= NULL;

	RegisterClassEx( &wc );

	// Create the window.
	// TODO: Adjust window dimensions to compensate for title bar and menu size.
	g_EmuWnd = CreateWindowEx( WS_EX_APPWINDOW | WS_EX_WINDOWEDGE, 
								L"Xenoborg", L"Xenoborg", WS_OVERLAPPEDWINDOW & (~WS_MAXIMIZEBOX),
								CW_USEDEFAULT, CW_USEDEFAULT, 640, 480, 
								NULL, NULL, GetModuleHandle( NULL ), NULL ); 
	if( !g_EmuWnd )
		return false;

	WndData.hWnd = g_EmuWnd;

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
DWORD WINAPI EmuThread( LPVOID Params )
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
DWORD WINAPI ExecutionThread( LPVOID Params )
{
	// Signifiy (in the debug console) that the code execution thread has started and show
	// the thread's ID number for debugging purposes.
	DbgPrintf( ">>> Execution thread started (Thread ID: 0x%X)\n", GetCurrentThreadId() );
#if 0
	// Now create a function pointer that points to the entry point of
	// the .xbe file.
	void (*Execute)(void) = (void(*)(void)) &Xbe.m_dwActualEntryPoint;

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
#endif

	// Initialize OpenGL and set default rendering states
	if( !InitOpenGL( &WndData ) )
	{
		EmuError( "Error initialing OpenGL!\n" );
		return 0;
	}

	glEnable( GL_DEPTH_TEST );
	glDepthFunc( GL_LEQUAL );
	glClearDepth( 1.0f );
	glDisable( GL_LIGHTING );
	glDisable( GL_CULL_FACE );
	
	glViewport( 0, 0, 640, 480 );
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();

	gluPerspective( 45.0f, float(640.0f/480.0f), 0.1f, 100.0f );
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();

	// Now that OpenGL is initialized, the update thread can safely process
	// screen updates.
//	Initialized = TRUE;
	g_bEmuIsRunning = TRUE;

	while( TRUE )
	{
		static float zrot = 0.0f;

		if( bVSync )
		{
			// Clear the screen one good time
			glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
			glLoadIdentity();
			glTranslatef( 0.0f, 0.0f, -3.0f );
			glRotatef( zrot, 0.0f, 0.0f, zrot );
			glBegin( GL_QUADS );
			glColor3f( 1.0f, 0.0f, 0.0f );	glVertex3f( -1.0f, -1.0f, 0.0f );
			glColor3f( 0.0f, 1.0f, 0.0f );	glVertex3f( +1.0f, -1.0f, 0.0f );
			glColor3f( 0.0f, 0.0f, 1.0f );	glVertex3f( +1.0f, +1.0f, 0.0f );
			glColor3f( 1.0f, 1.0f, 0.0f );	glVertex3f( -1.0f, +1.0f, 0.0f );
			glEnd();
			zrot += 0.5f;
			SwapBuffers( WndData.hDC );
		}
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
DWORD WINAPI UpdateThread( LPVOID Params )
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
			bVSync = FALSE;
		}
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
	g_hEmuThread = CreateThread( NULL, 0, EmuThread, NULL, CREATE_SUSPENDED, NULL );
	g_hExecutionThread = CreateThread( NULL, 0, ExecutionThread, NULL, CREATE_SUSPENDED, NULL );
	g_hUpdateThread = CreateThread( NULL, 0, UpdateThread, NULL, CREATE_SUSPENDED, NULL ); 

	// Activate main thread
	ResumeThread( g_hEmuThread );
	ResumeThread( g_hUpdateThread );
	ResumeThread( g_hExecutionThread );

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

	UninitOpenGL( &WndData );

	if( g_hEmuThread )
	{
		CloseHandle( g_hEmuThread );
		g_hEmuThread = NULL;
	}
}
