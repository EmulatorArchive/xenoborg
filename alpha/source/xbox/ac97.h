#pragma once

#include <stdint.h>

/* ac97 buffer decription */
struct ac97bufferdesc_t
{
	uint32_t start_addr;		/* address of buffer data */
	uint16_t length_in_samples;	/* length of buffer in samples (0=no samples) */
	uint16_t control;			/* bit 15=1: issue irq on completion */
								/* bit 14=1: last in stream */
};

/* ac97 context structure */
struct ac97_t
{
	uint8_t					mmio[0x1000];	/* mmio registers (are these all 8-bit wide?) */
	struct ac97bufferdesc_t pcm_spdif[32];  /* PCM spdif description */
	struct ac97bufferdesc_t pcm_out[32];	/* PCM output description */
	int						irq;			/* ac97's current irql */
};