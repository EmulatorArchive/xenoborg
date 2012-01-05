/* Platform specific includes */
#include "win32.h"
#include "dbg_console.h"
#include "xbox_emu.h"

/* Resource */
#include "../resource/resource.h"
#include "../resource/resrc1.h"

/* Parent window handle */
struct window_t* pwnd;
/* Debugger flag */
extern int enable_debugger;


/* Standard win32 message box */
int msg_box( char* string, UINT type )
{
	return MessageBoxA( GetActiveWindow(), string, "Xenoborg", type );
}

/* Returns a copy of the parent window */
int w32_get_parent_window( struct window_t* w )
{
	if( !pwnd )
		return 0;

	w->hinst = pwnd->hinst;
	w->hwnd = pwnd->hwnd;
	w->rect = pwnd->rect;

	return 1;
}

/* Finds an .xbe file */
int find_xbe_file( char* filename )
{
	char filter[256] = {0};
	char name[256] = {0};
	int index = 0;
	OPENFILENAMEA File;

	strcat( filter, "XBox Executable Files (*.xbe)" );
	ZeroMemory( &File, sizeof( OPENFILENAME ) );

	index = lstrlenA( filter ) + 1;
	filter[index++] = '*';
	filter[index++] = '.';
	filter[index++] = 'x';
	filter[index++] = 'b';
	filter[index++] = 'e';

	File.lStructSize = sizeof( OPENFILENAME );
	File.hwndOwner = GetActiveWindow();
	File.lpstrFilter = filter;
	File.lpstrFile = name;
	File.nMaxFile = 256;
	File.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_NOCHANGEDIR;

	if( !GetOpenFileNameA( &File ) )
	{
		msg_box( "Error opening .xbe file!", MB_ICONEXCLAMATION );
		return 0;
	}

	strcpy( filename, File.lpstrFile );

	return 1;
}

char* find_bios_file()
{
	char filter[256] = {0};
	char name[256] = {0};
	char out[256];
	int index = 0;
	OPENFILENAMEA File;

	strcat( filter, "Xbox Bios Files (*.bin)" );
	ZeroMemory( &File, sizeof( OPENFILENAME ) );

	index = lstrlenA( filter ) + 1;
	filter[index++] = '*';
	filter[index++] = '.';
	filter[index++] = 'b';
	filter[index++] = 'i';
	filter[index++] = 'n';

	File.lStructSize = sizeof( OPENFILENAME );
	File.hwndOwner = GetActiveWindow();
	File.lpstrFilter = filter;
	File.lpstrFile = name;
	File.nMaxFile = 256;
	File.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_NOCHANGEDIR;

	if( !GetOpenFileNameA( &File ) )
	{
		msg_box( "Error bios file!", MB_ICONEXCLAMATION );
		return NULL;
	}

	/* Verify bios is "kosher" */
	if( !open_bios( File.lpstrFile, FALSE ) )
		return NULL;

	strcpy( out, File.lpstrFile );

	return out;
}

/* BIOS Dialog message handler */
BOOL WINAPI BiosDialog( HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam )
{
	/* General purpose string */
//	char string[256];

	switch( message )
	{
	case WM_INITDIALOG:
		return TRUE;
	case WM_COMMAND:
		switch( LOWORD( wParam ) )
		{
		case IDC_BROWSE_BIOS:
			SetDlgItemTextA( hDlg, IDC_EDIT_BIOS, find_bios_file() );
			return TRUE;

		case IDOK:
            EndDialog( hDlg, LOWORD( wParam ) == IDOK ); 
            return TRUE; 

        case IDCANCEL: 
            EndDialog( hDlg, LOWORD( wParam ) == IDOK ); 
            return TRUE; 
		}
	}

	return FALSE;
}

/* Debugger */
BOOL WINAPI DebuggerDialog( HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam )
{
	switch( message )
	{
	case WM_INITDIALOG:
		populate_debugger_list( hDlg, IDC_LIST_MEM );
		return TRUE;
	case WM_COMMAND:
		switch( LOWORD( wParam ) )
		{
		case IDC_BUTTON_DEBUGGER_CLOSE:
			EndDialog( hDlg, TRUE );
			return TRUE;
		}
	}

	return FALSE;
}

/* Emulator settings dialog */
BOOL WINAPI EmulatorSettingsDialog( HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam )
{
	switch( message )
	{
	case WM_INITDIALOG:
		CheckDlgButton( hDlg, IDC_CHECK_DEBUGGER, enable_debugger );
		return TRUE;
	case WM_COMMAND:
		switch( LOWORD( wParam ) )
		{
		case IDOK:
			enable_debugger = ( IsDlgButtonChecked( hDlg, IDC_CHECK_DEBUGGER ) == BST_CHECKED );
            EndDialog( hDlg, LOWORD( wParam ) == IDOK ); 
            return TRUE; 

        case IDCANCEL: 
            EndDialog( hDlg, LOWORD( wParam ) == IDOK ); 
            return TRUE; 
		}
	}

	return FALSE;
}


/* Handle's messages from the menu */
LRESULT menu_handler( WPARAM wparam )
{
	char string[256];
	extern int open_xbe( char* filename );
	extern void close_xbe();
	extern int launch_bios;

	switch( wparam )
	{
	case ID_FILE_EXIT:
		close_xbe();
		close_bios();
		PostQuitMessage(0);
		return 0;
	case ID_EXECUTE_XBE:
		if( find_xbe_file( string ) )
		{
			if( open_xbe( string ) )
			{
				launch_bios = No;
				initialize_emulation_thread();
			}
		}
		return 0;
	case ID_EXECUTE_BIOS:
		launch_bios = Yes;
		initialize_emulation_thread();
		return 0;
	case ID_EMULATION_STOP:
		set_emulator_state( emu_state_stopped );
		return 0;
	case ID_EMULATION_RESET:
	//	set_emulator_state( emu_state_reset );
		return 0;
	case ID_SETTINGS_BIOS:
		DialogBox( NULL, MAKEINTRESOURCE( IDD_DIALOG_BIOS ), pwnd->hwnd, BiosDialog );
		return 0;
	case ID_SETTINGS_EMULATOR:
		DialogBox( NULL, MAKEINTRESOURCE( IDD_DIALOG_SETTINGS ), pwnd->hwnd, EmulatorSettingsDialog );
		return 0;
	/*case ID_VIEW_DEBUGGER:
		DialogBox( NULL, MAKEINTRESOURCE( IDD_DIALOG_DEBUGGER ), pwnd->hwnd, DebuggerDialog );
		return 0;*/
	}

	return 0;
}

/* Shows the debugger */
void show_debugger()
{
	DialogBox( NULL, MAKEINTRESOURCE( IDD_DIALOG_DEBUGGER ), pwnd->hwnd, DebuggerDialog );
}

/* Win32 message procedure handler */
LRESULT CALLBACK WindowProcedure( HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam )
{
	extern void close_xbe();

	switch( Message )
	{
	case WM_CREATE:
		set_emulator_state( emu_state_stopped );
		return 0;

	case WM_CLOSE:
	case WM_DESTROY:
			close_xbe(); /* Precaution */
			PostQuitMessage(0);
		return 0;

	case WM_SYSCOMMAND:
		{
			switch( wParam )
			{
			case SC_MONITORPOWER:
			case SC_SCREENSAVE:
				return 0;
			}
			break;
		}

	case WM_COMMAND:
		return menu_handler(wParam);

	// Ignore for now
	case WM_SIZE:
		return 0;
	}

	return DefWindowProc( hWnd, Message, wParam, lParam );
}

/* Creates a new window */
int create_window( struct window_t* wnd, int width, int height )
{
	WNDCLASSEX wc;

	wnd->hinst = GetModuleHandle(NULL);
	
	SetRect( &wnd->rect, 0, 0, width, height );

	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.cbSize = sizeof( WNDCLASSEX );
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hbrBackground = (HBRUSH) GetStockObject( BLACK_BRUSH );
	wc.hCursor = LoadCursor( NULL, IDC_ARROW );
	wc.hIcon = LoadIcon( NULL, IDI_APPLICATION );
	wc.hIconSm = wc.hIcon;
	wc.hInstance = wnd->hinst;
	wc.lpfnWndProc = WindowProcedure;
	wc.lpszClassName = "Xenoborg";
	wc.lpszMenuName = MAKEINTRESOURCE( IDR_MENU1 );
	
	if( !RegisterClassEx( &wc ) )
		return 0;

	/* Init console */
	DbgConsoleInit();

	// Create our window
	wnd->hwnd = CreateWindowExA( 0, "Xenoborg", "Xenoborg", WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
		CW_USEDEFAULT, CW_USEDEFAULT, width, height+32, NULL, NULL, wnd->hinst, NULL );
	if( !wnd->hwnd )
		return 0;

	ShowWindow( wnd->hwnd, SW_SHOW );
	SetForegroundWindow( wnd->hwnd );
	SetFocus( wnd->hwnd );
	SetActiveWindow( wnd->hwnd );

	pwnd = wnd;

	/*if( !initialize_emulation_thread() )
		return 0;*/

	/* Init timer */
	timer_init();

	return 1;
}

/* Message pump */
int main_loop()
{
	MSG msg;

	while( GetMessage( &msg, NULL, 0, 0 ) )
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

//	pthread_exit(NULL);

	return 0;
}

