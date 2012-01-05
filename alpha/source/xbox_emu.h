#pragma once

/* All xbox hardware\software includes */
#include "xbox\\pci.h"
#include "xbox\\xbe.h"
#include "xbox\\nv2a.h"
#include "xbox\\nv2afb.h"
#include "xbox\\ke.h"
#include "emu\\emu.h"
#include "timer.h"

/* Xbox processor speed */
#define XBOX_CPU_SPEED	733.38f

/* Emulator state */
enum emu_state
{
	emu_state_running = 0,
	emu_state_halted,
	emu_state_stopped,
	emu_state_reset
};


struct xbox_bios_t
{
	/* bios file name */
	char filename[256];
	/* bios file size */
	int size;
	/* pointer to bios data */
	void* ptr;
};

struct xbox_emu_t
{
	/* Emulator state */
	enum emu_state state;
	/* Libemu (x86) handle */
	struct emu* e;
	/* Xbe file */
	struct Xbe_t xbe;
	/* bios */
	struct xbox_bios_t bios;
	/* Handle to parent window */
	struct window_t wnd;
	/* Handle to emulation thread */
	pthread_t emulation_thread_handle;
};

/* Emulation functions */
void	set_emulator_state( enum emu_state es );
int		initialize_emulation_thread();
int		open_bios( char* filename, int preload );
void	close_bios();