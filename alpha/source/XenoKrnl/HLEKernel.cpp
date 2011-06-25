/*
 * Xenoborg - Xbox Emulator
 * Copyright (C) 2006-2010  blueshogun96
 *
 * Name: HLEKernel.h
 * Desc: Contains code for the HLE kernel. This is used as an alternative to true
 *		 BIOS emulation when necessary. Upon load time, Xenoborg will scan for any
 *		 call/jump commands referencing a kernel function and replace the address
 *		 with the appropriate function.
 *
 * Changelog: 
 *	5/2/11 - Initial writing.
 */

#include "XenoKrnl.h"

// Globals
LPVOID	FrameBuffer = (void*) 0xF0040000;	// This shouldn't remain constant.


//--------------------------------------------------------------------------------------
// 0x0001: AvGetSavedDataAddress
//--------------------------------------------------------------------------------------
void*	NTAPI	AvGetSavedDataAddress()
{
	// Return a pointer to the framebuffer address
	return FrameBuffer;
}

//--------------------------------------------------------------------------------------
// 0x0008: DbgPrint
//--------------------------------------------------------------------------------------
ULONG	__cdecl	DbgPrint( char* Format, ... )
{
	// Show string in the debugger and the debug console
	va_list args;
	char string[256];

	va_start( Format, args );
	vsprintf( string, Format, args );
	va_end( args );

	DbgPrintf( ">>> DbgPrint: %s\n", Format );

	OutputDebugStringA( ">>> DbgPrint: " );
	OutputDebugStringA( Format );
	OutputDebugStringA( "\n" );

	return 0;
}

//--------------------------------------------------------------------------------------
// 0x009C: KeTickCount
//--------------------------------------------------------------------------------------
volatile DWORD KeTickCount = 0;

//--------------------------------------------------------------------------------------
// 0x0168: HalInitiateShutdown
//--------------------------------------------------------------------------------------
void	NTAPI HalInitiateShutdown()
{
	EmuWarning( "HalInitiateShutdown(): Xbox is shutting down!\n" );

	// Trap 
	while(1) { }
}

// The Xbox kernel thunk table contains the address of every emulated kernel function
// to be used at runtime.

#define UNIMPL(x) x

extern "C" void* Xbox_KernelThunkTable[367] =
{
    (void*)UNIMPL(0x0000),							// 0x0000 (0)
    (void*)AvGetSavedDataAddress,					// 0x0001 (1)
    (void*)UNIMPL(0x0002),                          // 0x0002 (2)
    (void*)UNIMPL(0x0003),                          // 0x0003 (3)
    (void*)UNIMPL(0x0004),                          // 0x0004 (4)
    (void*)UNIMPL(0x0005),                          // 0x0005 (5)
    (void*)UNIMPL(0x0006),                          // 0x0006 (6)
    (void*)UNIMPL(0x0007),                          // 0x0007 (7)
    (void*)DbgPrint,								// 0x0008 (8)
    (void*)UNIMPL(0x0009),							// 0x0009 (9)
    (void*)UNIMPL(0x000A),                          // 0x000A (10)
    (void*)UNIMPL(0x000B),                          // 0x000B (11)
    (void*)UNIMPL(0x000C),                          // 0x000C (12)
    (void*)UNIMPL(0x000D),                          // 0x000D (13)
    (void*)UNIMPL(0x000E),							// 0x000E (14)
    (void*)UNIMPL(0x000F),							// 0x000F (15)
    (void*)UNIMPL(0x0010),                          // 0x0010 (16)
    (void*)UNIMPL(0x0011),                          // 0x0011 (17)
    (void*)UNIMPL(0x0012),                          // 0x0012 (18)
    (void*)UNIMPL(0x0013),                          // 0x0013 (19)
    (void*)UNIMPL(0x0014),                          // 0x0014 (20)
    (void*)UNIMPL(0x0015),                          // 0x0015 (21)
    (void*)UNIMPL(0x0016),                          // 0x0016 (22)
    (void*)UNIMPL(0x0017),                          // 0x0017 (23)
    (void*)UNIMPL(0x0018),							// 0x0018 (24)
    (void*)UNIMPL(0x0019),                          // 0x0019 (25)
    (void*)UNIMPL(0x001A),                          // 0x001A (26)
    (void*)UNIMPL(0x001B),                          // 0x001B (27)
    (void*)UNIMPL(0x001C),                          // 0x001C (28)
    (void*)UNIMPL(0x001D),                          // 0x001D (29)
    (void*)UNIMPL(0x001E),                          // 0x001E (30)
    (void*)UNIMPL(0x001F),                          // 0x001F (31)
    (void*)UNIMPL(0x0020),                          // 0x0020 (32)
    (void*)UNIMPL(0x0021),                          // 0x0021 (33)
    (void*)UNIMPL(0x0022),                          // 0x0022 (34)
    (void*)UNIMPL(0x0023),                          // 0x0023 (35)
    (void*)UNIMPL(0x0024),                          // 0x0024 (36)
    (void*)UNIMPL(0x0025),							// 0x0025 (37)
    (void*)UNIMPL(0x0026),                          // 0x0026 (38)
    (void*)UNIMPL(0x0027),                          // 0x0027 (39)
    (void*)UNIMPL(0x0028),                          // 0x0028 (40)
    (void*)UNIMPL(0x0029),                          // 0x0029 (41)
    (void*)UNIMPL(0x002A),                          // 0x002A (42)
    (void*)UNIMPL(0x002B),                          // 0x002B (43)
    (void*)UNIMPL(0x002C),                          // 0x002C (44)
    (void*)UNIMPL(0x002D),                          // 0x002D (45)
    (void*)UNIMPL(0x002E),                          // 0x002E (46)
    (void*)UNIMPL(0x002F),                          // 0x002F (47)
    (void*)UNIMPL(0x0030),                          // 0x0030 (48)
    (void*)UNIMPL(0x0031),							// 0x0031 (49)
    (void*)UNIMPL(0x0032),                          // 0x0032 (50)
    (void*)UNIMPL(0x0033),                          // 0x0033 (51)
    (void*)UNIMPL(0x0034),                          // 0x0034 (52)
    (void*)UNIMPL(0x0035),                          // 0x0035 (53)
    (void*)UNIMPL(0x0036),                          // 0x0036 (54)
    (void*)UNIMPL(0x0037),                          // 0x0037 (55)
    (void*)UNIMPL(0x0038),                          // 0x0038 (56)
    (void*)UNIMPL(0x0039),                          // 0x0039 (57)
    (void*)UNIMPL(0x003A),                          // 0x003A (58)
    (void*)UNIMPL(0x003B),                          // 0x003B (59)
    (void*)UNIMPL(0x003C),                          // 0x003C (60)
    (void*)UNIMPL(0x003D),                          // 0x003D (61)
    (void*)UNIMPL(0x003E),                          // 0x003E (62)
    (void*)UNIMPL(0x003F),                          // 0x003F (63)
    (void*)UNIMPL(0x0040),                          // 0x0040 (64)
    (void*)UNIMPL(0x0041),                          // 0x0041 (65)
    (void*)UNIMPL(0x0042),							// 0x0042 (66)
    (void*)UNIMPL(0x0043),							// 0x0043 (67)
    (void*)UNIMPL(0x0044),                          // 0x0044 (68)
    (void*)UNIMPL(0x0045),							// 0x0045 (69)
    (void*)UNIMPL(0x0046),                          // 0x0046 (70)
    (void*)UNIMPL(0x0047),                          // 0x0047 (71)
    (void*)UNIMPL(0x0048),                          // 0x0048 (72)
    (void*)UNIMPL(0x0049),                          // 0x0049 (73)
    (void*)UNIMPL(0x004A),                          // 0x004A (74)
    (void*)UNIMPL(0x004B),                          // 0x004B (75)
    (void*)UNIMPL(0x004C),                          // 0x004C (76)
    (void*)UNIMPL(0x004D),                          // 0x004D (77)
    (void*)UNIMPL(0x004E),                          // 0x004E (78)
    (void*)UNIMPL(0x004F),                          // 0x004F (79)
    (void*)UNIMPL(0x0050),                          // 0x0050 (80)
    (void*)UNIMPL(0x0051),                          // 0x0051 (81)
    (void*)UNIMPL(0x0052),                          // 0x0052 (82)
    (void*)UNIMPL(0x0053),                          // 0x0053 (83)
    (void*)UNIMPL(0x0054),                          // 0x0054 (84)
    (void*)UNIMPL(0x0055),                          // 0x0055 (85)
    (void*)UNIMPL(0x0056),                          // 0x0056 (86)
    (void*)UNIMPL(0x0057),                          // 0x0057 (87)
    (void*)UNIMPL(0x0058),                          // 0x0058 (88)
    (void*)UNIMPL(0x0059),                          // 0x0059 (89)
    (void*)UNIMPL(0x005A),                          // 0x005A (90)
	(void*)UNIMPL(0x005B),							// 0x005B (91)
    (void*)UNIMPL(0x005C),                          // 0x005C (92)
    (void*)UNIMPL(0x005D),                          // 0x005D (93)
    (void*)UNIMPL(0x005E),                          // 0x005E (94)
	(void*)UNIMPL(0x005F),							// 0x005F (95)
    (void*)UNIMPL(0x0060),                          // 0x0060 (96)
    (void*)UNIMPL(0x0061),                          // 0x0061 (97)
    (void*)UNIMPL(0x0062),                          // 0x0062 (98)
    (void*)UNIMPL(0x0063),							// 0x0063 (99)
    (void*)UNIMPL(0x0064),                          // 0x0064 (100)
    (void*)UNIMPL(0x0065),                          // 0x0065 (101)
    (void*)UNIMPL(0x0066),                          // 0x0066 (102)
    (void*)UNIMPL(0x0067),                          // 0x0067 (103)
    (void*)UNIMPL(0x0068),                          // 0x0068 (104)
    (void*)UNIMPL(0x0069),                          // 0x0069 (105)
    (void*)UNIMPL(0x006A),                          // 0x006A (106)
    (void*)UNIMPL(0x006B),							// 0x006B (107)
    (void*)UNIMPL(0x006C),                          // 0x006C (108)
    (void*)UNIMPL(0x006D),                          // 0x006D (109)
    (void*)UNIMPL(0x006E),                          // 0x006E (110)
    (void*)UNIMPL(0x006F),                          // 0x006F (111)
    (void*)UNIMPL(0x0070),                          // 0x0070 (112)
    (void*)UNIMPL(0x0071),							// 0x0071 (113)
    (void*)UNIMPL(0x0072),                          // 0x0072 (114)
    (void*)UNIMPL(0x0073),                          // 0x0073 (115)
    (void*)UNIMPL(0x0074),                          // 0x0074 (116)
    (void*)UNIMPL(0x0075),                          // 0x0075 (117)
    (void*)UNIMPL(0x0076),                          // 0x0076 (118)
    (void*)UNIMPL(0x0077),                          // 0x0077 (119)
    (void*)UNIMPL(0x0078),                          // 0x0078 (120)
    (void*)UNIMPL(0x0079),                          // 0x0079 (121)
    (void*)UNIMPL(0x007A),                          // 0x007A (122)
    (void*)UNIMPL(0x007B),                          // 0x007B (123)
    (void*)UNIMPL(0x007C),                          // 0x007C (124)
    (void*)UNIMPL(0x007D),                          // 0x007D (125)
    (void*)UNIMPL(0x007E),							// 0x007E (126)
    (void*)UNIMPL(0x007F),							// 0x007F (127)
    (void*)UNIMPL(0x0080),							// 0x0080 (128)
    (void*)UNIMPL(0x0081),							// 0x0081 (129)
    (void*)UNIMPL(0x0082),                          // 0x0082 (130)
    (void*)UNIMPL(0x0083),                          // 0x0083 (131)
    (void*)UNIMPL(0x0084),                          // 0x0084 (132)
    (void*)UNIMPL(0x0085),                          // 0x0085 (133)
    (void*)UNIMPL(0x0086),                          // 0x0086 (134)
    (void*)UNIMPL(0x0087),                          // 0x0087 (135)
    (void*)UNIMPL(0x0088),                          // 0x0088 (136)
    (void*)UNIMPL(0x0089),                          // 0x0089 (137)
    (void*)UNIMPL(0x008A),                          // 0x008A (138)
    (void*)UNIMPL(0x008B),                          // 0x008B (139)
    (void*)UNIMPL(0x008C),                          // 0x008C (140)
    (void*)UNIMPL(0x008D),                          // 0x008D (141)
    (void*)UNIMPL(0x008E),                          // 0x008E (142)
    (void*)UNIMPL(0x008F),                          // 0x008F (143)
    (void*)UNIMPL(0x0090),                          // 0x0090 (144)
    (void*)UNIMPL(0x0091),                          // 0x0091 (145)
    (void*)UNIMPL(0x0092),                          // 0x0092 (146)
    (void*)UNIMPL(0x0093),                          // 0x0093 (147)
    (void*)UNIMPL(0x0094),                          // 0x0094 (148)
    (void*)UNIMPL(0x0095),							// 0x0095 (149)
    (void*)UNIMPL(0x0096),							// 0x0096 (150)
    (void*)UNIMPL(0x0097),                          // 0x0097 (151)
    (void*)UNIMPL(0x0098),                          // 0x0098 (152)
    (void*)UNIMPL(0x0099),                          // 0x0099 (153)
    (void*)UNIMPL(0x009A),                          // 0x009A (154)
    (void*)UNIMPL(0x009B),                          // 0x009B (155)
    (void*)KeTickCount,							// 0x009C (156)
    (void*)UNIMPL(0x009D),                          // 0x009D (157)
    (void*)UNIMPL(0x009E),                          // 0x009E (158)
    (void*)UNIMPL(0x009F),                          // 0x009F (159)
    (void*)UNIMPL(0x00A0),							// 0x00A0 (160)
    (void*)UNIMPL(0x00A1),							// 0x00A1 (161)
    (void*)UNIMPL(0x00A2),                          // 0x00A2 (162)
    (void*)UNIMPL(0x00A3),                          // 0x00A3 (163)
    (void*)UNIMPL(0x00A4),							// 0x00A4 (164)
    (void*)UNIMPL(0x00A5),							// 0x00A5 (165)
    (void*)UNIMPL(0x00A6),							// 0x00A6 (166)
    (void*)UNIMPL(0x00A7),							// 0x00A7 (167)
    (void*)UNIMPL(0x00A8),                          // 0x00A8 (168)
    (void*)UNIMPL(0x00A9),					        // 0x00A9 (169)
    (void*)UNIMPL(0x00AA),							// 0x00AA (170)
    (void*)UNIMPL(0x00AB),							// 0x00AB (171)
    (void*)UNIMPL(0x00AC),							// 0x00AC (172)
    (void*)UNIMPL(0x00AD),                          // 0x00AD (173)
    (void*)UNIMPL(0x00AE),                          // 0x00AE (174)
    (void*)UNIMPL(0x00AF),                          // 0x00AF (175)
    (void*)UNIMPL(0x00B0),                          // 0x00B0 (176)
    (void*)UNIMPL(0x00B1),                          // 0x00B1 (177)
    (void*)UNIMPL(0x00B2),							// 0x00B2 (178)
    (void*)UNIMPL(0x00B3),                          // 0x00B3 (179)
    (void*)UNIMPL(0x00B4),							// 0x00B4 (180)
    (void*)UNIMPL(0x00B5),							// 0x00B5 (181)
    (void*)UNIMPL(0x00B6),							// 0x00B6 (182)
    (void*)UNIMPL(0x00B7),                          // 0x00B7 (183)
    (void*)UNIMPL(0x00B8),							// 0x00B8 (184)
    (void*)UNIMPL(0x00B9),                          // 0x00B9 (185)
    (void*)UNIMPL(0x00BA),							// 0x00BA (186)
    (void*)UNIMPL(0x00BB),							// 0x00BB (187)
    (void*)UNIMPL(0x00BC),                          // 0x00BC (188)
    (void*)UNIMPL(0x00BD),							// 0x00BD (189)
    (void*)UNIMPL(0x00BE),							// 0x00BE (190)
    (void*)UNIMPL(0x00BF),                          // 0x00BF (191)
    (void*)UNIMPL(0x00C0),							// 0x00C0 (192)
    (void*)UNIMPL(0x00C1),							// 0x00C1 (193)
    (void*)UNIMPL(0x00C2),                          // 0x00C2 (194)
    (void*)UNIMPL(0x00C3),                          // 0x00C3 (195)
    (void*)UNIMPL(0x00C4),                          // 0x00C4 (196)
    (void*)UNIMPL(0x00C5),							// 0x00C5 (197)
    (void*)UNIMPL(0x00C6),							// 0x00C6 (198)
    (void*)UNIMPL(0x00C7),							// 0x00C7 (199)
    (void*)UNIMPL(0x00C8),                          // 0x00C8 (200)
    (void*)UNIMPL(0x00C9),                          // 0x00C9 (201)
    (void*)UNIMPL(0x000A),							// 0x00CA (202)
    (void*)UNIMPL(0x00CB),                          // 0x00CB (203)
    (void*)UNIMPL(0x00CC),                          // 0x00CC (204)
    (void*)UNIMPL(0x00CD),                          // 0x00CD (205)
    (void*)UNIMPL(0x00CE),							// 0x00CE (206)
    (void*)UNIMPL(0x00CF),							// 0x00CF (207)
    (void*)UNIMPL(0x00D0),                          // 0x00D0 (208)
    (void*)UNIMPL(0x00D1),                          // 0x00D1 (209)
    (void*)UNIMPL(0x00D2),							// 0x00D2 (210)
    (void*)UNIMPL(0x00D3),							// 0x00D3 (211)
    (void*)UNIMPL(0x00D4),                          // 0x00D4 (212)
    (void*)UNIMPL(0x00D5),                          // 0x00D5 (213)
    (void*)UNIMPL(0x00D6),                          // 0x00D6 (214)
    (void*)UNIMPL(0x00D7),                          // 0x00D7 (215)
    (void*)UNIMPL(0x00D8),                          // 0x00D8 (216)
    (void*)UNIMPL(0x00D9),							// 0x00D9 (217)
    (void*)UNIMPL(0x00DA),							// 0x00DA (218)
    (void*)UNIMPL(0x00DB),							// 0x00DB (219)
    (void*)UNIMPL(0x00DC),                          // 0x00DC (220)
    (void*)UNIMPL(0x00DD),							// 0x00DD (221)
    (void*)UNIMPL(0x00DE),							// 0x00DE (222)
    (void*)UNIMPL(0x00DF),                          // 0x00DF (223)
    (void*)UNIMPL(0x00E0),							// 0x00E0 (224)
    (void*)UNIMPL(0x00E1),							// 0x00E1 (225)
    (void*)UNIMPL(0x00E2),							// 0x00E2 (226)
    (void*)UNIMPL(0x00E3),                          // 0x00E3 (227)
    (void*)UNIMPL(0x00E4),                          // 0x00E4 (228)
    (void*)UNIMPL(0x00E5),                          // 0x00E5 (229)
    (void*)UNIMPL(0x00E6),                          // 0x00E6 (230)
    (void*)UNIMPL(0x00E7),							// 0x00E7 (231)
    (void*)UNIMPL(0x00E8),							// 0x00E8 (232)
    (void*)UNIMPL(0x00E9),                          // 0x00E9 (233)
    (void*)UNIMPL(0x00EA),							// 0x00EA (234)
    (void*)UNIMPL(0x00EB),							// 0x00EB (235)
    (void*)UNIMPL(0x00EC),							// 0x00EC (236)
    (void*)UNIMPL(0x00ED),                          // 0x00ED (237)
    (void*)UNIMPL(0x00EE),							// 0x00EE (238)
    (void*)UNIMPL(0x00EF),                          // 0x00EF (239)
    (void*)UNIMPL(0x00F0),                          // 0x00F0 (240)
    (void*)UNIMPL(0x00F1),                          // 0x00F1 (241)
    (void*)UNIMPL(0x00F2),                          // 0x00F2 (242)
    (void*)UNIMPL(0x00F3),                          // 0x00F3 (243)
    (void*)UNIMPL(0x00F4),                          // 0x00F4 (244)
    (void*)UNIMPL(0x00F5),                          // 0x00F5 (245)
    (void*)UNIMPL(0x00F6),                          // 0x00F6 (246)
    (void*)UNIMPL(0x00F7),                          // 0x00F7 (247)
    (void*)UNIMPL(0x00F8),                          // 0x00F8 (248)
    (void*)UNIMPL(0x00F9),                          // 0x00F9 (249)
    (void*)UNIMPL(0x00FA),                          // 0x00FA (250)
    (void*)UNIMPL(0x00FB),                          // 0x00FB (251)
    (void*)UNIMPL(0x00FC),                          // 0x00FC (252)
    (void*)UNIMPL(0x00FD),                          // 0x00FD (253)
    (void*)UNIMPL(0x00FE),                          // 0x00FE (254)
    (void*)UNIMPL(0x00FF),							// 0x00FF (255)
    (void*)UNIMPL(0x0100),                          // 0x0100 (256)
    (void*)UNIMPL(0x0101),                          // 0x0101 (257)
    (void*)UNIMPL(0x0101),							// 0x0102 (258)
    (void*)UNIMPL(0x0103),                          // 0x0103 (259)
    (void*)UNIMPL(0x0104),							// 0x0104 (260)
    (void*)UNIMPL(0x0105),                          // 0x0105 (261)
    (void*)UNIMPL(0x0106),                          // 0x0106 (262)
    (void*)UNIMPL(0x0107),                          // 0x0107 (263)
    (void*)UNIMPL(0x0108),							// 0x0108 (264)
    (void*)UNIMPL(0x0109),                          // 0x0109 (265)
    (void*)UNIMPL(0x010A),                          // 0x010A (266)
    (void*)UNIMPL(0x010B),                          // 0x010B (267)
    (void*)UNIMPL(0x010C),                          // 0x010C (268)
    (void*)UNIMPL(0x010D),                          // 0x010D (269)
    (void*)UNIMPL(0x010E),                          // 0x010E (270)
    (void*)UNIMPL(0x010F),                          // 0x010F (271)
    (void*)UNIMPL(0x0110),                          // 0x0110 (272)
    (void*)UNIMPL(0x0111),                          // 0x0111 (273)
    (void*)UNIMPL(0x0112),                          // 0x0112 (274)
    (void*)UNIMPL(0x0113),                          // 0x0113 (275)
    (void*)UNIMPL(0x0114),                          // 0x0114 (276)
    (void*)UNIMPL(0x0115),							// 0x0115 (277)
    (void*)UNIMPL(0x0116),                          // 0x0116 (278)
    (void*)UNIMPL(0x0117),							// 0x0117 (279)
    (void*)UNIMPL(0x0118),                          // 0x0118 (280)
    (void*)UNIMPL(0x0119),                          // 0x0119 (281)
    (void*)UNIMPL(0x011A),                          // 0x011A (282)
    (void*)UNIMPL(0x011B),                          // 0x011B (283)
    (void*)UNIMPL(0x011C),                          // 0x011C (284)
    (void*)UNIMPL(0x011D),                          // 0x011D (285)
    (void*)UNIMPL(0x011E),                          // 0x011E (286)
    (void*)UNIMPL(0x011F),                          // 0x011F (287)
    (void*)UNIMPL(0x0120),                          // 0x0120 (288)
    (void*)UNIMPL(0x0121),							// 0x0121 (289)
    (void*)UNIMPL(0x0122),                          // 0x0122 (290)
    (void*)UNIMPL(0x0123),							// 0x0123 (291)
    (void*)UNIMPL(0x0124),                          // 0x0124 (292)
    (void*)UNIMPL(0x0125),                          // 0x0125 (293)
    (void*)UNIMPL(0x0126),							// 0x0126 (294)
    (void*)UNIMPL(0x0127),                          // 0x0127 (295)
    (void*)UNIMPL(0x0128),							// 0x0128 (296)
    (void*)UNIMPL(0x0129),                          // 0x0129 (297)
    (void*)UNIMPL(0x012A),                          // 0x012A (298)
    (void*)UNIMPL(0x012B),                          // 0x012B (299)
    (void*)UNIMPL(0x012C),                          // 0x012C (300)
    (void*)UNIMPL(0x012D),							// 0x012D (301)
    (void*)UNIMPL(0x012E),                          // 0x012E (302)
    (void*)UNIMPL(0x012F),                          // 0x012F (303)
    (void*)UNIMPL(0x0130),							// 0x0130 (304)
    (void*)UNIMPL(0x0131),							// 0x0131 (305)
    (void*)UNIMPL(0x0132),							// 0x0132 (306)
    (void*)UNIMPL(0x0133),                          // 0x0133 (307)
    (void*)UNIMPL(0x0134),							// 0x0134 (308)
    (void*)UNIMPL(0x0135),                          // 0x0135 (309)
    (void*)UNIMPL(0x0136),                          // 0x0136 (310)
    (void*)UNIMPL(0x0137),                          // 0x0137 (311)
    (void*)UNIMPL(0x0138),                          // 0x0138 (312)
    (void*)UNIMPL(0x0139),                          // 0x0139 (313)
    (void*)UNIMPL(0x013A),                          // 0x013A (314)
    (void*)UNIMPL(0x013B),                          // 0x013B (315)
    (void*)UNIMPL(0x013C),                          // 0x013C (316)
    (void*)UNIMPL(0x013D),                          // 0x013D (317)
    (void*)UNIMPL(0x013E),                          // 0x013E (318)
    (void*)UNIMPL(0x013F),                          // 0x013F (319)
    (void*)UNIMPL(0x0140),                          // 0x0140 (320)
    (void*)UNIMPL(0x0141),                          // 0x0141 (321)
    (void*)UNIMPL(0x0142),							// 0x0142 (322)
    (void*)UNIMPL(0x0143),							// 0x0143 (323)
    (void*)UNIMPL(0x0144),                          // 0x0144 (324)
    (void*)UNIMPL(0x0145),							// 0x0145 (325)
    (void*)UNIMPL(0x0146),                          // 0x0146 (326)
    (void*)UNIMPL(0x0147),                          // 0x0147 (327)
    (void*)UNIMPL(0x0148),                          // 0x0148 (328)
    (void*)UNIMPL(0x0149),                          // 0x0149 (329)
    (void*)UNIMPL(0x014A),                          // 0x014A (330)
    (void*)UNIMPL(0x014B),                          // 0x014B (331)
    (void*)UNIMPL(0x014C),                          // 0x014C (332)
    (void*)UNIMPL(0x014D),                          // 0x014D (333)
    (void*)UNIMPL(0x014E),                          // 0x014E (334)
    (void*)UNIMPL(0x014F),							// 0x014F (335)
    (void*)UNIMPL(0x0150),							// 0x0150 (336)
    (void*)UNIMPL(0x0151),							// 0x0151 (337)
    (void*)UNIMPL(0x0152),                          // 0x0152 (338)
    (void*)UNIMPL(0x0153),                          // 0x0153 (339)
    (void*)UNIMPL(0x0154),                          // 0x0154 (340)
    (void*)UNIMPL(0x0155),                          // 0x0155 (341)
    (void*)UNIMPL(0x0156),                          // 0x0156 (342)
    (void*)UNIMPL(0x0157),                          // 0x0157 (343)
    (void*)UNIMPL(0x0158),                          // 0x0158 (344)
    (void*)UNIMPL(0x0159),                          // 0x0159 (345)
    (void*)UNIMPL(0x015A),                          // 0x015A (346)
    (void*)UNIMPL(0x015B),                          // 0x015B (347)
    (void*)UNIMPL(0x015C),                          // 0x015C (348)
    (void*)UNIMPL(0x015D),                          // 0x015D (349)
    (void*)UNIMPL(0x015E),                          // 0x015E (350)
    (void*)UNIMPL(0x015F),                          // 0x015F (351)
    (void*)UNIMPL(0x0160),                          // 0x0160 (352)
    (void*)UNIMPL(0x0161),                          // 0x0161 (353)
    (void*)UNIMPL(0x0162),                          // 0x0162 (354)
    (void*)UNIMPL(0x0163),                          // 0x0163 (355)
    (void*)UNIMPL(0x0164),							// 0x0164 (356)
    (void*)UNIMPL(0x0165),                          // 0x0165 (357)
    (void*)UNIMPL(0x0166),                          // 0x0166 (358)
    (void*)UNIMPL(0x0167),                          // 0x0167 (359)
    (void*)HalInitiateShutdown,                     // 0x0168 (360)
    (void*)UNIMPL(0x0169),                          // 0x0169 (361)
    (void*)UNIMPL(0x016A),                          // 0x016A (362)
    (void*)UNIMPL(0x016B),                          // 0x016B (363)
    (void*)UNIMPL(0x016C),                          // 0x016C (364)
    (void*)UNIMPL(0x016D),                          // 0x016D (365)
    (void*)UNIMPL(0x016E),                          // 0x016E (366)
};


void KrnlInstallFunc1( void* DestFunc, void* SourceFunc )
{
	BYTE* ptr = (BYTE*) DestFunc;

	*(BYTE*) &ptr[0] = 0xE9;
	*(DWORD*)&ptr[1] = (DWORD) SourceFunc - (DWORD) DestFunc - 5;
	*(BYTE*) &ptr[5] = 0x90;
}

void KrnlInstallFunc2( BYTE op, void* DestFunc, void* SourceFunc )
{
	BYTE* ptr = (BYTE*) DestFunc;

	*(BYTE*) &ptr[0] = op;
	*(DWORD*)&ptr[1] = (DWORD) SourceFunc - (DWORD) DestFunc - 5;
}

void HookKrnlFunctions()
{
	// Go through the .xbe data and start replacing calls/jumps to kernel
	// functions with our HLE functions.
	DWORD dwXbeSize = (DWORD) Xbe.m_lXbeFileSize;
	BYTE* ptr = (BYTE*) 0x11000;
	INT	Hooked = 0;

	DbgPrintf( "HookKrnlFunctions(): Installing kernel function hooks... \n" );

	for( DWORD i = 0; i < ( dwXbeSize - 0x1000 ); i++ )
	{
		// Search for JMP DS:[KernelFunc] or CALL DS:[KernelFunc]
		if( ptr[i] == 0xFF && ( ptr[i+1] == 0x25 || ptr[i+1] == 0x15 ) )
		{
			// If we find one, read that address within the data segment
			DWORD dwAddr = GetDisp32( i+2, 0x11000 );
			
			// Sanity check
			if( dwAddr > 1024*1024*64 || dwAddr < 0x11000 ) continue;
			DWORD dwData = GetDisp32( dwAddr, 0 );
			
			// Is it a kernel function?
			if( dwData > 0x80000000 && dwData < 0x8000016E )
			{
				// Replace the jump with a jump to our emulated version
				void* pFunc = (void*) ( 0x11000 + i );
				KrnlInstallFunc1( pFunc, Xbox_KernelThunkTable[dwData&0x00000FFF] );
				Hooked++;

				DbgPrintf( "\t0x%.08X \n", dwAddr );
			}
		}

		// Search for MOV r32, offset KernelFunc
		if( ptr[i] == 0xB8 || ptr[i] == 0xB9 || ptr[i] == 0xBA || ptr[i] == 0xBB )
		{
			// If we find one, read that address within the data segment
			DWORD dwAddr = GetDisp32( i+1, 0x11000 );

			// Sanity check
			if( dwAddr > 1024*1024*64 || dwAddr < 0x11000 ) continue;
			DWORD dwData = GetDisp32( dwAddr, 0 );
			
			// Is it a kernel function?
			if( dwData > 0x80000000 && dwData < 0x8000016E )
			{
				// Replace the jump with a jump to our emulated version
				void* pFunc = (void*) ( 0x11000 + i );
				KrnlInstallFunc2( ptr[i], pFunc, Xbox_KernelThunkTable[dwData&0x00000FFF] );
				Hooked++;

				DbgPrintf( "\t0x%.08X \n", dwAddr );
			}
		}
	}

	DbgPrintf( "\t%d Total.\n", Hooked );
}
