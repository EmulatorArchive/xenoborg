/*
 * Xenoborg - Xbox Emulator
 * Copyright (C) 2006-2010  blueshogun96
 *
 * Name: EmuNV2A.h
 * Desc: Handles emulation of the GPU.
 *
 * Changelog: 
 *	4/26/11 - Framebuffer emulation is working okay.  Still need to do a manual bitswap
 *		from RGB to BGR. Sucks because OpenGL doesn't have this problem and D3DFMT_X8B8G8R8
 *		doesn't appear to be a valid render target format.
 *
 *	10/16/2010 - Initial writing.
 */

#include "XenoKrnl.h"
#include "EmuNV2A.h"
#include "EmuDirect3D.h"


// Globals
const DWORD dwFBAddr	= 0xF0040000;	// This won't be constant for long...
const DWORD dwFBSize	= 640*480*4;
void*		pFB			= NULL;			// Pointer to the actual framebuffer data
D3DContext d3dContext;
extern HWND g_EmuWnd;


// Initializes the NV2A to it's default states
BOOL EmuNV2AInit()
{
	// Initialize Direct3D
	d3dContext.hWnd = g_EmuWnd;
	if( !EnableDirect3D( &d3dContext ) )
		return FALSE;

	D3DClearRenderTarget( &d3dContext );

	// Allocate the Xbox's framebuffer pointer
	// TODO: Don't just "assume" this size is constant...
	if( !( pFB = malloc( dwFBSize ) ) )
		return EmuError( "Error allocating framebuffer pointer!" );

	// TODO: Initialize other registers and render states.

	return TRUE;
}

// Uninitializes the NV2A
void EmuNV2AUninit()
{
	if( pFB )
	{
		free( pFB );
		pFB = NULL;
	}

	DisableDirect3D( &d3dContext );
}

// Updates framebuffer
void EmuNV2AUpdateFB()
{
	extern BOOL bVSync;

	if( bVSync )
	{
		bVSync = FALSE;
		D3DSwap( &d3dContext );
	}
}

// NV2A hardware read/write functions
BYTE EmuNV2ARead8( DWORD dwAddress )
{
	// Framebuffer read
	if( dwAddress >= dwFBAddr && dwAddress < (dwFBAddr+dwFBSize) )
		return ((BYTE*)pFB)[dwAddress-dwFBAddr];

	DbgPrintf( "NV2A: Unhandled Read8:= [0x%.08X]\n", dwAddress );
	return 0;
}

void EmuNV2AWrite8( DWORD dwAddress, BYTE Value )
{
	// Framebuffer write
	if( dwAddress >= dwFBAddr && dwAddress < (dwFBAddr+dwFBSize) )
	{
//		DbgPrintf( "NV2A: Framebuffer Write8:= 0x%.02X -> [0x%.08X]\n", Value, dwAddress );
		((BYTE*)pFB)[dwAddress-dwFBAddr] = Value;
		D3DFrameBufferWrite8( &d3dContext, dwAddress-dwFBAddr, Value );
//		D3DSwap( &d3dContext );
		return;
	}

	DbgPrintf( "NV2A: Unhandled Write8:= 0x%.02X -> [0x%.08X]\n", Value, dwAddress );
}

WORD EmuNV2ARead16( DWORD dwAddress )
{
	DbgPrintf( "NV2A: Unhandled Read16:= [0x%.08X]\n", dwAddress );
	return 0;
}

void EmuNV2AWrite16( DWORD dwAddress, WORD Value )
{
	DbgPrintf( "NV2A: Unhandled Write16:= 0x%.04X -> [0x%.08X]\n", Value, dwAddress );
}

DWORD EmuNV2ARead32( DWORD dwAddress )
{
	// Framebuffer read
	if( dwAddress >= dwFBAddr && dwAddress < (dwFBAddr+dwFBSize) )
		return ((DWORD*)pFB)[dwAddress-dwFBAddr];

	DbgPrintf( "NV2A: Unhandled Read32:= [0x%.08X]\n", dwAddress );
	return 0;
}

void EmuNV2AWrite32( DWORD dwAddress, DWORD Value )
{
	// Framebuffer write
	if( dwAddress >= dwFBAddr && dwAddress < (dwFBAddr+dwFBSize) )
	{
		DbgPrintf( "NV2A: Framebuffer Write32:= 0x%.08X -> [0x%.08X]\n", Value, dwAddress );
		((DWORD*)pFB)[dwAddress-dwFBAddr] = Value;
		D3DFrameBufferWrite( &d3dContext, dwAddress-dwFBAddr, Value );
		D3DSwap( &d3dContext );
		return;
	}

	DbgPrintf( "NV2A: Unhandled Write32:= 0x%.08X -> [0x%.08X]\n", Value, dwAddress );
}
