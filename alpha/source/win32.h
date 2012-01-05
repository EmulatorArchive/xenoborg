#pragma once

#pragma warning (disable:4996)

#include <windows.h>
#include <stdio.h>
#include <stdint.h>
#include <pthread.h>

#define Yes	1
#define No	0

/* Window attributes */
struct window_t
{
	HWND		hwnd;
	HINSTANCE	hinst;
	RECT		rect;
};

/* Win32 specific functions */
int msg_box( char* string, UINT type );
int w32_get_parent_window( struct window_t* w );
int create_window( struct window_t* wnd, int width, int height );
int main_loop();