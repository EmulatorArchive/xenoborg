/*
 * Xenoborg - Xbox Emulator
 * Copyright (C) 2006-2010  blueshogun96
 *
 * Name: EmuNV2A.h
 * Desc: Handles emulation of the GPU.
 *
 * Changelog: 
 *	10/16/2010 - Initial writing.
 */

#pragma once

// Initializes the NV2A to it's default states
BOOL EmuNV2AInit();
// Uninitializes the NV2A
void EmuNV2AUninit();
// Updates framebuffer
void EmuNV2AUpdateFB();

// NV2A hardware read/write functions
BYTE EmuNV2ARead8( DWORD dwAddress );
void EmuNV2AWrite8( DWORD dwAddress, BYTE Value );
WORD EmuNV2ARead16( DWORD dwAddress );
void EmuNV2AWrite16( DWORD dwAddress, WORD Value );
DWORD EmuNV2ARead32( DWORD dwAddress );
void EmuNV2AWrite32( DWORD dwAddress, DWORD Value );
