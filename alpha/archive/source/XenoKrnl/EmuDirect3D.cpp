/*
 * Xenoborg - Xbox Emulator
 * Copyright (C) 2006-2010  blueshogun96
 *
 * Name: EmuDirect3D.cpp
 * Desc: Contains Direct3D 9.0 routines used to handle gfx.
 *
 * Changelog:
 *	4/24/11 - Initial writing to temporarily replace OpenGL since I'm having driver
 *		issues with multithreaded applications (wglMakeCurrent keeps failing).
 */

#include "XenoKrnl.h"


bool EnableDirect3D( D3DContext* wndData )
{
	HRESULT hr;

	// Initialize Direct3D
	wndData->d3d = Direct3DCreate9( D3D_SDK_VERSION );
	if( !wndData->d3d )
		return EmuError( "Error initializing Direct3D9!\n" );

	// Get the current display mode setings
	D3DDISPLAYMODE d3ddm;
	hr = wndData->d3d->GetAdapterDisplayMode( D3DADAPTER_DEFAULT, &d3ddm );
	if( FAILED( hr ) )
		return EmuError( "Error getting display settings!\n", hr );

	// Create the Direct3D device
	D3DPRESENT_PARAMETERS d3dpp;
	ZeroMemory( &d3dpp, sizeof( d3dpp ) );
	d3dpp.BackBufferWidth = 640;
	d3dpp.BackBufferHeight = 480;
	d3dpp.BackBufferFormat = D3DFMT_A8R8G8B8; //d3ddm.Format;
//	d3dpp.EnableAutoDepthStencil = TRUE;
//	d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
	d3dpp.BackBufferCount = 1;
	d3dpp.hDeviceWindow = wndData->hWnd;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.Windowed = TRUE;
	d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
	d3dpp.Flags = D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;

	hr = wndData->d3d->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, wndData->hWnd,
		D3DCREATE_HARDWARE_VERTEXPROCESSING, &d3dpp, &wndData->d3ddevice );
	if( FAILED( hr ) )
		return EmuError( "Error creating Direct3D device!\n", hr );

	// Create a new render target with XBGR format to match the framebuffer
	/*hr = wndData->d3ddevice->CreateRenderTarget( 640, 480, D3DFMT_X8B8G8R8, D3DMULTISAMPLE_NONE, 0, 
		TRUE, &wndData->rendertarget, NULL );
	if( FAILED( hr ) )
		return EmuError( "Error creating render target!\n", hr );*/

	// Create a new depth and stencil buffer
	hr = wndData->d3ddevice->CreateDepthStencilSurface( 640, 480, D3DFMT_D24S8, D3DMULTISAMPLE_NONE, 0,
		TRUE, &wndData->depthstencil, NULL );
	if( FAILED( hr ) )
		return EmuError( "Error creating depth stencil surface!\n", hr );

	// Set default viewport
	D3DVIEWPORT9 vp;
	vp.X = 0;
	vp.Y = 0;
	vp.Width = 640;
	vp.Height = 480;
	vp.MinZ = 0.0f;
	vp.MaxZ = 1.0f;
	wndData->d3ddevice->SetViewport( &vp );

	return TRUE;
}

void DisableDirect3D( D3DContext* wndData )
{
	if( wndData->depthstencil )
		wndData->depthstencil->Release();
	if( wndData->rendertarget )
		wndData->rendertarget->Release();

	if( wndData->d3ddevice )
	{
		wndData->d3ddevice->Release();
	}

	if( wndData->d3d )
	{
		wndData->d3d->Release();
	}
}

void D3DClearRenderTarget( D3DContext* context )
{
	// Clears the currently set render target
	context->d3ddevice->Clear( 0, NULL, D3DCLEAR_TARGET, 0, 1.0f, 0 );
}

void D3DClearDepthBuffer( D3DContext* context )
{
	context->d3ddevice->Clear( 0, NULL, D3DCLEAR_ZBUFFER, 0, 1.0f, 0 );
}

void D3DClearStencilBuffer( D3DContext* context )
{
	context->d3ddevice->Clear( 0, NULL, D3DCLEAR_STENCIL, 0, 1.0f, 0 );
}

void D3DSwap( D3DContext* context )
{
	context->d3ddevice->Present( NULL, NULL, NULL, NULL );
}

void D3DFrameBufferWrite( D3DContext* context, DWORD addr, DWORD val )
{
	IDirect3DSurface9* backbuffer = NULL;
	D3DLOCKED_RECT LockedRect;

	// A really crappy function to write to the framebuffer
	HRESULT hr = context->d3ddevice->GetBackBuffer( 0, 0, D3DBACKBUFFER_TYPE_MONO, &backbuffer );
	if( FAILED( hr ) )
		return;

	if( SUCCEEDED( backbuffer->LockRect( &LockedRect, NULL, 0 ) ) )
	{
		DWORD* fb = (DWORD*) LockedRect.pBits;
		fb[addr] = val;

		backbuffer->UnlockRect();
	}
}

void D3DFrameBufferWrite8( D3DContext* context, DWORD addr, BYTE val )
{
	IDirect3DSurface9* backbuffer = NULL;
	D3DLOCKED_RECT LockedRect;

	// A really crappy function to write to the framebuffer
	HRESULT hr = context->d3ddevice->GetBackBuffer( 0, 0, D3DBACKBUFFER_TYPE_MONO, &backbuffer );
	if( FAILED( hr ) )
		return;

	if( SUCCEEDED( backbuffer->LockRect( &LockedRect, NULL, 0 ) ) )
	{
		BYTE* fb = (BYTE*) LockedRect.pBits;
		fb[addr] = val;

		backbuffer->UnlockRect();
	}

	backbuffer->Release();
}