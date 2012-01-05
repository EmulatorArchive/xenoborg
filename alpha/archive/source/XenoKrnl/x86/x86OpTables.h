/*
 * Xenoborg - Xbox Emulator
 * Copyright (C) 2006-2010  blueshogun96
 *
 * Name: x86OpTables.h
 * Desc: x86 opcode tables of instructions that can't be directly executed
 *		 (i.e. memory reads/writes, priviledged instructions, etc).
 *
 * Changelog: 
 *	10/14/2010 - Initial import from an old source build.
 */

#pragma once

// x86 Opcode tables
// A standard interpreter would get too massive to maintain


typedef struct _x86op
{
	int ( *op16 )( void );
	int ( *op32 )( void );
}x86op;

int x86op_Unimplemented_1byte( void );
int x86op_Unimplemented_2byte( void );

extern const x86op x86_opcode_table_1byte[];
extern const x86op x86_opcode_table_2byte[];
