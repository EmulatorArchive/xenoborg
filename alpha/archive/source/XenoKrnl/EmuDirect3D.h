/*
 * Xenoborg - Xbox Emulator
 * Copyright (C) 2006-2010  blueshogun96
 *
 * Name: EmuDirect3D.h
 * Desc: Contains Direct3D 9.0 routines used to handle gfx.
 *
 * Changelog:
 *	4/24/11 - Initial writing to temporarily replace OpenGL since I'm having driver
 *		issues with multithreaded applications (wglMakeCurrent keeps failing).
 */

#pragma once

#include <d3d9.h>
#include <d3dx9.h>

struct D3DContext
{
	IDirect3D9*			d3d;
	IDirect3DDevice9*	d3ddevice;
	IDirect3DSurface9*	rendertarget;
	IDirect3DSurface9*	depthstencil;
	HWND hWnd;
};

bool EnableDirect3D( D3DContext* wndData );
void DisableDirect3D( D3DContext* wndData );
void D3DClearRenderTarget( D3DContext* context );
void D3DClearDepthBuffer( D3DContext* context );
void D3DClearStencilBuffer( D3DContext* context );
void D3DSwap( D3DContext* context );
void D3DFrameBufferWrite( D3DContext* context, DWORD addr, DWORD val );
void D3DFrameBufferWrite8( D3DContext* context, DWORD addr, BYTE val );