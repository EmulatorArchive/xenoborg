/*
 * Xenoborg - Xbox Emulator
 * Copyright (C) 2006-2010  blueshogun96
 *
 * Name: x86Op1.cpp
 * Desc: Handles 1 byte x86 opcodes.
 *
 * Changelog: 
 *	10/14/2010 - Initial import from an old source build.
 *
 *	10/16/2010 - Added one implementation of MOV (32-bit) to test the code.
 */


#include "..\\XenoKrnl.h"
#include "..\\EmuMMU.h"
#include "x86OpTables.h"

// Globals
extern x86Instruction Instruction;
extern LPEXCEPTION_POINTERS pE;

extern DWORD get_scaled_index( BYTE mod );
extern DWORD get_ea32( BYTE modrm );


__inline int x86op_MOV_r8_rm8( void )	// Opcode 0x88 /r
{
	BYTE modrm = GetDisp8( pE->ContextRecord->Eip, Instruction.Size++ );

	if( modrm >= 0xC0 )
	{
		return 0;
	}
	else
	{
		if( modrm == 0x44 )
		{
			int x = 5;
		}

		DWORD ea = get_ea32( modrm );
		BYTE val = 0;

		switch( reg_op( modrm ) )
		{
		case 0: val = pE->ContextRecord->Eax & 0x0000000F; break;	// AL
		case 1: val = pE->ContextRecord->Ecx & 0x0000000F; break;	// CL
		case 2: val = pE->ContextRecord->Edx & 0x0000000F; break;	// DL
		case 3: val = pE->ContextRecord->Ebx & 0x0000000F; break;	// BL
		case 4: val = ( pE->ContextRecord->Eax >> 4 ) & 0x0000000F; break;	// AH
		case 5: val = ( pE->ContextRecord->Ecx >> 4 ) & 0x0000000F; break;	// CH
		case 6: val = ( pE->ContextRecord->Edx >> 4 ) & 0x0000000F; break;	// DH
		case 7: val = ( pE->ContextRecord->Ebx >> 4 ) & 0x0000000F; break;	// BH
		}

		WriteMemory8( ea, val );
	}

	return 1;
}

__inline int x86op_MOV_rm32_imm32( void )	// Opcode 0xC7 /0
{
	BYTE modrm = GetDisp8( pE->ContextRecord->Eip, Instruction.Size++ );

	if( modrm >= 0xC0 )
	{
		return 0;
	}
	else
	{
		DWORD ea = get_ea32( modrm );
		DWORD imm32 = GetDisp32( pE->ContextRecord->Eip, Instruction.Size );
		Instruction.Size += 4;

		WriteMemory32( ea, imm32 );
	}

	return 1;
}

__inline int x86op_group_0xC7_32( void )
{
	BYTE modrm = GetDisp8( pE->ContextRecord->Eip, Instruction.Size );

	switch( reg_op( modrm ) )
	{
	case 0: return x86op_MOV_rm32_imm32();
	default: return x86op_Unimplemented_1byte(); 
	}

	return 0;
}

__inline int x86op_Unimplemented_1byte( void )
{
	return 0;
}


const x86op x86_opcode_table_1byte[] = 
{
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//0 (0x0)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//1 (0x1)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//2 (0x2)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//3 (0x3)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//4 (0x4)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//5 (0x5)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//6 (0x6)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//7 (0x7)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//8 (0x8)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//9 (0x9)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//10 (0xa)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//11 (0xb)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//12 (0xc)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//13 (0xd)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//14 (0xe)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//15 (0xf)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//16 (0x10)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//17 (0x11)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//18 (0x12)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//19 (0x13)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//20 (0x14)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//21 (0x15)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//22 (0x16)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//23 (0x17)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//24 (0x18)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//25 (0x19)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//26 (0x1a)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//27 (0x1b)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//28 (0x1c)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//29 (0x1d)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//30 (0x1e)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//31 (0x1f)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//32 (0x20)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//33 (0x21)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//34 (0x22)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//35 (0x23)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//36 (0x24)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//37 (0x25)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//38 (0x26)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//39 (0x27)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//40 (0x28)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//41 (0x29)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//42 (0x2a)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//43 (0x2b)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//44 (0x2c)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//45 (0x2d)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//46 (0x2e)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//47 (0x2f)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//48 (0x30)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//49 (0x31)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//50 (0x32)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//51 (0x33)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//52 (0x34)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//53 (0x35)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//54 (0x36)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//55 (0x37)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//56 (0x38)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//57 (0x39)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//58 (0x3a)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//59 (0x3b)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//60 (0x3c)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//61 (0x3d)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//62 (0x3e)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//63 (0x3f)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//64 (0x40)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//65 (0x41)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//66 (0x42)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//67 (0x43)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//68 (0x44)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//69 (0x45)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//70 (0x46)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//71 (0x47)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//72 (0x48)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//73 (0x49)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//74 (0x4a)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//75 (0x4b)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//76 (0x4c)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//77 (0x4d)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//78 (0x4e)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//79 (0x4f)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//80 (0x50)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//81 (0x51)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//82 (0x52)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//83 (0x53)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//84 (0x54)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//85 (0x55)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//86 (0x56)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//87 (0x57)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//88 (0x58)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//89 (0x59)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//90 (0x5a)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//91 (0x5b)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//92 (0x5c)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//93 (0x5d)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//94 (0x5e)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//95 (0x5f)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//96 (0x60)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//97 (0x61)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//98 (0x62)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//99 (0x63)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//100 (0x64)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//101 (0x65)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//102 (0x66)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//103 (0x67)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//104 (0x68)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//105 (0x69)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//106 (0x6a)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//107 (0x6b)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//108 (0x6c)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//109 (0x6d)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//110 (0x6e)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//111 (0x6f)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//112 (0x70)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//113 (0x71)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//114 (0x72)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//115 (0x73)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//116 (0x74)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//117 (0x75)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//118 (0x76)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//119 (0x77)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//120 (0x78)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//121 (0x79)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//122 (0x7a)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//123 (0x7b)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//124 (0x7c)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//125 (0x7d)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//126 (0x7e)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//127 (0x7f)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//128 (0x80)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//129 (0x81)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//130 (0x82)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//131 (0x83)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//132 (0x84)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//133 (0x85)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//134 (0x86)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//135 (0x87)
	{ x86op_MOV_r8_rm8, x86op_MOV_r8_rm8 },						//136 (0x88)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//137 (0x89)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//138 (0x8a)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//139 (0x8b)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//140 (0x8c)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//141 (0x8d)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//142 (0x8e)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//143 (0x8f)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//144 (0x90)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//145 (0x91)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//146 (0x92)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//147 (0x93)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//148 (0x94)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//149 (0x95)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//150 (0x96)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//151 (0x97)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//152 (0x98)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//153 (0x99)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//154 (0x9a)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//155 (0x9b)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//156 (0x9c)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//157 (0x9d)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//158 (0x9e)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//159 (0x9f)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//160 (0xa0)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },   //161 (0xa1)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//162 (0xa2)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//163 (0xa3)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//164 (0xa4)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//165 (0xa5)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//166 (0xa6)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//167 (0xa7)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//168 (0xa8)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//169 (0xa9)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//170 (0xaa)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//171 (0xab)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//172 (0xac)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//173 (0xad)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//174 (0xae)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//175 (0xaf)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//176 (0xb0)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//177 (0xb1)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//178 (0xb2)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//179 (0xb3)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//180 (0xb4)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//181 (0xb5)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//182 (0xb6)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//183 (0xb7)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//184 (0xb8)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//185 (0xb9)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//186 (0xba)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//187 (0xbb)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//188 (0xbc)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//189 (0xbd)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//190 (0xbe)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//191 (0xbf)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//192 (0xc0)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//193 (0xc1)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//194 (0xc2)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//195 (0xc3)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//196 (0xc4)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//197 (0xc5)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//198 (0xc6)
	{ x86op_Unimplemented_1byte, x86op_group_0xC7_32 },			//199 (0xc7)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//200 (0xc8)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//201 (0xc9)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//202 (0xca)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//203 (0xcb)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//204 (0xcc)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//205 (0xcd)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//206 (0xce)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//207 (0xcf)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//208 (0xd0)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//209 (0xd1)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//210 (0xd2)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//211 (0xd3)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//212 (0xd4)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//213 (0xd5)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//214 (0xd6)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//215 (0xd7)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//216 (0xd8)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//217 (0xd9)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//218 (0xda)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//219 (0xdb)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//220 (0xdc)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//221 (0xdd)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//222 (0xde)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//223 (0xdf)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//224 (0xe0)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//225 (0xe1)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//226 (0xe2)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//227 (0xe3)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//228 (0xe4)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//229 (0xe5)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//230 (0xe6)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//231 (0xe7)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//232 (0xe8)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//233 (0xe9)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//234 (0xea)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//235 (0xeb)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//236 (0xec)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//237 (0xed)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//238 (0xee)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//239 (0xef)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//240 (0xf0)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//241 (0xf1)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//242 (0xf2)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//243 (0xf3)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//244 (0xf4)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//245 (0xf5)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//246 (0xf6)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//247 (0xf7)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//248 (0xf8)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//249 (0xf9)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//250 (0xfa)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//251 (0xfb)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//252 (0xfc)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//253 (0xfd)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//254 (0xfe)
	{ x86op_Unimplemented_1byte, x86op_Unimplemented_1byte },	//255 (0xff)
	NULL,
};
