#pragma once

// Disable stupid depreciation warnings
#define _CRT_SECURE_NO_DEPRECATE

#include <windows.h>
#include <windowsx.h>
#include <mmsystem.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <conio.h>
#include <process.h>
#ifdef __cplusplus
#include <iostream>
#endif
#include <time.h>
#include <ddraw.h>		// DirectDraw is required for VBlank emulation

#ifdef __cplusplus		// Don't bother compiling with the launcher...
#include "Error.h"
#include "DbgConsole.h"
#include "EmuDirect3D.h"
#include "EmuMMU.h"
#include "EmuNV2A.h"
#include "Xbe.h"
#include "HLEKernel.h"
#endif


// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the XENOKRNL_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// XENOKRNL_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef XENOKRNL_EXPORTS
#define XENOKRNL_API __declspec(dllexport)
#else
#define XENOKRNL_API __declspec(dllimport)
#endif

#ifdef __cplusplus
extern "C" {
#endif

XENOKRNL_API void XenoKrnlNullFunc(void);

#ifdef __cplusplus
}
#endif

