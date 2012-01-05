/*
 * Xenoborg - Xbox Emulator
 * Copyright (C) 2006-2010  blueshogun96
 *
 * Name: Error.h
 * Desc: Error reporting functions.
 *
 * Changelog: 
 *	9/21/2010 - Initial writing.
 */

#pragma once

void EmuWarning( char* szWarning, ... );
bool EmuError( char* szError, ... );
