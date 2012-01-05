/********************************************************************************
 *                               libemu
 *
 *                    - x86 shellcode emulation -
 *
 *
 * Copyright (C) 2007  Paul Baecher & Markus Koetter
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 * 
 * 
 *             contact nepenthesdev@users.sourceforge.net  
 *
 *******************************************************************************/
#pragma warning(disable:4018)

#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <stdbool.h>
#include <endian.h>

#include "emu/emu.h"
#include "emu/emu_log.h"
#include "emu/emu_memory.h"
#include "emu/emu_string.h"

#define bcopy(a,b,c) memcpy(b,a,c)

#define PAGE_BITS 12 /* size of one page, 2^12 = 4096 */
#define PAGESET_BITS 10 /* number of pages in one pageset, 2^10 = 1024 */


#define PAGE_SIZE (1 << PAGE_BITS)
#define PAGESET_SIZE (1 << PAGESET_BITS)

#define PAGESET(x) ((x) >> (PAGESET_BITS + PAGE_BITS))
#define PAGE(x) (((x) >> PAGE_BITS) & ((1 << PAGESET_BITS) - 1))
#define OFFSET(x) (((1 << PAGE_BITS) - 1) & (x))

#define FS_SEGMENT_DEFAULT_OFFSET 0x7ffdf000


#define MEGABYTE(x) (x*1024*1024)
#define RAM_SIZE	MEGABYTE(64)

#define MAX_MEM_HANDLERS	16
//
//struct emu_memory
//{
//	struct emu *emu;
//	void *mem;
//	void *rom;
//	uint32_t size;
//	
//	uint32_t segment_offset;
//	enum emu_segment segment_current;
//	
//	uint32_t segment_table[6];
//
//	bool read_only_access;
//
//	/* Memory range handlers */
//	int memory_handler_count;
//	struct emu_memory_handler* mem_handler;
//};

//struct emu_memory_handler
//{
//	uint32_t mem_start;
//	uint32_t mem_end;
//
//	bool read_only_access;
//	bool executable;
//
//	uint8_t (*read8)(uint32_t);
//	void	(*write8)(uint32_t, uint8_t);
//	uint16_t(*read16)(uint32_t);
//	void	(*write16)(uint32_t, uint16_t);
//	uint32_t (*read32)(uint32_t);
//	void	(*write32)(uint32_t, uint32_t);
//};

#if 1
/*static void emu_memory_debug_pagetable(struct emu_memory *m)
{
	int pagesets = 1 << (32 - PAGESET_BITS - PAGE_BITS);
	int pagesets_used = 0;
	
	printf("*** memory debug\n");
	
	int i, j;
	
	for( i = 0; i < pagesets; i++ )
	{
		if( m->pagetable[i] != NULL )
		{
			printf("  pageset %d allocated at 0x%08x\n", i, (int)m->pagetable[i]);
			int pages = 1 << (PAGESET_BITS);
			int pages_used = 0;

			for( j = 0; j < pages; j++ )
			{
				if( m->pagetable[i][j] != NULL )
				{
					printf("    page %d allocated at 0x%08x\n", j, (int)m->pagetable[i][j]);
					pages_used++;
				}
			}
			
			printf("    %d/%d pages used\n", pages_used, pages);
			
			pagesets_used++;
		}
	}
	
	printf("  %d/%d pagesets used\n", pagesets_used, pagesets);
	printf("*** end of memory debug\n");
}

static void emu_memory_debug_addr(uint32_t addr)
{
	printf("addr 0x%08x, pageset 0x%08x, page 0x%08x, offset 0x%08x\n",
		addr, PAGESET(addr), PAGE(addr), OFFSET(addr));
}*/
#endif

uint32_t emu_memory_get_usage(struct emu_memory *m)
{
	uint32_t usage = (1 << (32 - PAGE_BITS - PAGESET_BITS)) * sizeof(void *); /* pageset table */
	int pagesets = 1 << (32 - PAGESET_BITS - PAGE_BITS);
	
	/* TODO: Use this? */

	/*int i, j;
	
	for( i = 0; i < pagesets; i++ )
	{
		if( m->pagetable[i] != NULL )
		{
			usage += PAGESET_SIZE * sizeof(void *);
			int pages = 1 << (PAGESET_BITS);

			for( j = 0; j < pages; j++ )
				if( m->pagetable[i][j] != NULL )
					usage += PAGE_SIZE;
		}
	}*/
	
	return usage;
}

struct emu_memory *emu_memory_new(struct emu *e)
{
	struct emu_memory *em = (struct emu_memory *)malloc(sizeof(struct emu_memory));
	if( em == NULL )
	{
		return NULL;
	}
	memset(em, 0, sizeof(struct emu_memory));
	
	em->emu = e;
	
	em->mem = malloc( RAM_SIZE );
	if( em->mem == NULL )
	{
		return NULL;
	}
	memset(em->mem, 0, RAM_SIZE);
	em->size = RAM_SIZE;

	/* 16mb ROM region 0xFF000000 - 0xFFFFFFFF */
	em->rom = malloc(MEGABYTE(16));
	if( em->rom == NULL )
		return NULL;
	
	em->segment_table[s_fs] = FS_SEGMENT_DEFAULT_OFFSET;

	em->read_only_access = false;

	/* Set up memory handlers */
	em->memory_handler_count = 0;
	em->mem_handler = (struct emu_memory_handler *)malloc(sizeof(struct emu_memory_handler)*MAX_MEM_HANDLERS);
	memset( em->mem_handler, 0, sizeof(struct emu_memory_handler) * MAX_MEM_HANDLERS );

	return em;
}

void emu_memory_free(struct emu_memory *m)
{
	/*int i, j;
	
	for( i = 0; i < (1 << (32 - PAGESET_BITS - PAGE_BITS)); i++ )
	{
		if( m->pagetable[i] != NULL )
		{
			for( j = 0; j < PAGESET_SIZE; j++ )
				if( m->pagetable[i][j] != NULL )
					free(m->pagetable[i][j]);
			
			free(m->pagetable[i]);
		}
	}*/
	free(m->mem_handler);
	free(m->rom);
	free(m->mem);
	free(m);
}

void emu_memory_clear(struct emu_memory *m)
{
	/*int i, j;
	
	for( i = 0; i < (1 << (32 - PAGESET_BITS - PAGE_BITS)); i++ )
	{
		if( m->pagetable[i] != NULL )
		{
			for( j = 0; j < PAGESET_SIZE; j++ )
				if( m->pagetable[i][j] != NULL )
					free(m->pagetable[i][j]);
			
			free(m->pagetable[i]);
		}
	}*/

	memset(m->mem, 0, RAM_SIZE);
	
	m->segment_table[s_fs] = FS_SEGMENT_DEFAULT_OFFSET;

	m->read_only_access = false;
}

void* emu_memory_get_ptr(struct emu *e)
{
	return e->memory->mem;
}

int32_t emu_memory_add_handler(struct emu_memory* em, struct emu_memory_handler* h)
{
	if(em->memory_handler_count > MAX_MEM_HANDLERS)
		return -1;

	for( int i = 0; i < MAX_MEM_HANDLERS; i++ )
	{
		if( em->mem_handler[i].mem_start == 0 && em->mem_handler[i].mem_end == 0 )
		{
			memcpy( &em->mem_handler[em->memory_handler_count], h, sizeof( struct emu_memory_handler ) );					
			em->memory_handler_count++;

			return em->memory_handler_count;
		}
	}

	return 0;
}

int32_t emu_memory_remove_handler(struct emu_memory* em, uint32_t slot)
{
	if(slot < 0 || slot > MAX_MEM_HANDLERS-1)
		return -1;

	memset(&em->mem_handler[slot], 0, sizeof(struct emu_memory_handler));
	em->memory_handler_count--;

	return 0;
}

static inline int page_is_alloc(struct emu_memory *em, uint32_t addr)
{
	/*if( em->pagetable[PAGESET(addr)] != NULL )
	{
		if( em->pagetable[PAGESET(addr)][PAGE(addr)] != NULL )
		{
			return -1;
		} 
	}*/
	
	return 0;
}

static inline int page_alloc(struct emu_memory *em, uint32_t addr)
{
	/*if( em->pagetable[PAGESET(addr)] == NULL )
	{
		em->pagetable[PAGESET(addr)] = (void**)malloc(PAGESET_SIZE * sizeof(void *));
		
		if( em->pagetable[PAGESET(addr)] == NULL )
		{
			emu_errno_set(em->emu, ENOMEM);
			emu_strerror_set(em->emu, "out of memory\n", addr);
			return -1;
		}
		
		memset(em->pagetable[PAGESET(addr)], 0, PAGESET_SIZE * sizeof(void *));
	}

	if( em->pagetable[PAGESET(addr)][PAGE(addr)] == NULL )
	{
		em->pagetable[PAGESET(addr)][PAGE(addr)] = malloc(PAGE_SIZE);
		
		if( em->pagetable[PAGESET(addr)][PAGE(addr)] == NULL )
		{
			emu_errno_set(em->emu, ENOMEM);
			emu_strerror_set(em->emu, "out of memory\n", addr);
			return -1;
		}
	}*/

	return 0;
}

static inline void *translate_addr(struct emu_memory *em, uint32_t addr)
{
	/*if( em->pagetable[PAGESET(addr)] != NULL )
	{
		if( em->pagetable[PAGESET(addr)][PAGE(addr)] != NULL )
		{
			return ((uint32_t*)em->pagetable[PAGESET(addr)][PAGE(addr)]) + OFFSET(addr);
		}
	}*/
	
	return NULL;
}

int32_t emu_memory_read_byte(struct emu_memory *m, uint32_t addr, uint8_t *byte)
{
	addr += m->segment_offset;
//	void *address = m->mem[addr];
	
	if( addr < RAM_SIZE )
		*byte = ((uint8_t *)m->mem)[addr];
	else if( addr > 0xFEFFFFFF )
		*byte = ((uint8_t *)m->rom)[addr-0xFF000000];
	else
	{
		for( int i = 0; i < MAX_MEM_HANDLERS; i++ )
		{
			if( m->mem_handler[i].mem_start == 0 && m->mem_handler[i].mem_end == 0 )
				continue;

			if( addr >= m->mem_handler[i].mem_start && addr <= m->mem_handler[i].mem_end )
			{
				*byte = m->mem_handler[i].read8(addr);
				break;
			}
		}
	}
	
	return 0;
}

int32_t emu_memory_read_word(struct emu_memory *m, uint32_t addr, uint16_t *word)
{
#if BYTE_ORDER == BIG_ENDIAN
	uint16_t val;
	int32_t retval = emu_memory_read_block(m, addr, &val, 2);
	val =  ((val & 0xff00) >> 8) | 
		   ((val & 0x00ff) << 8);
	bcopy(&val,word,2);
	return retval;
#else
	if( addr < RAM_SIZE )
		return emu_memory_read_block(m, addr, word, 2);
	else
	{
		for( int i = 0; i < MAX_MEM_HANDLERS; i++ )
		{
			if( m->mem_handler[i].mem_start == 0 && m->mem_handler[i].mem_end == 0 )
				continue;

			if( addr >= m->mem_handler[i].mem_start && addr <= m->mem_handler[i].mem_end )
			{
				*word = m->mem_handler[i].read16(addr);
				break;
			}
		}
	}
#endif
	return 0;
}

extern "C" extern unsigned long impl_KeTickCount;

int32_t emu_memory_read_dword(struct emu_memory *m, uint32_t addr, uint32_t *dword)
{
#if BYTE_ORDER == BIG_ENDIAN
	uint32_t val;
	int32_t retval =  emu_memory_read_block(m, addr, &val, 4);
	val =  ((val & (0xff000000)) >> 24) | 
		   ((val & (0x00ff0000)) >> 8)  | 
		   ((val & (0x0000ff00)) << 8) | 
		   ((val & (0x000000ff)) << 24);
	memcpy(dword, &val, 4);
	return retval;
#else
	if( addr < RAM_SIZE )
		return emu_memory_read_block(m, addr, dword, 4);
	else if( addr == 0x8000009C )
	{
		/* HLE Kernel fix (not proper, but oh well) */
		*dword = impl_KeTickCount;
		return 0;
	}
	else
	{
		for( int i = 0; i < MAX_MEM_HANDLERS; i++ )
		{
			if( m->mem_handler[i].mem_start == 0 && m->mem_handler[i].mem_end == 0 )
				continue;

			if( addr >= m->mem_handler[i].mem_start && addr <= m->mem_handler[i].mem_end )
			{
				*dword = m->mem_handler[i].read32(addr);
				break;
			}
		}
	}
#endif
	return 0;
}


int32_t emu_memory_read_block(struct emu_memory *m, uint32_t addr, void *dest, size_t len)
{
	if( !m )
		return -1;

	uint32_t oaddr = addr; /* save original addr for recursive call */
	addr += m->segment_offset;
	
//	uint8_t bmem[1024*1024*64];
//	memmove( bmem, m->mem, 1024*1024*64 );

	if(addr < RAM_SIZE)
		memmove( dest, ((uint8_t*)m->mem)+addr, len );
	else if(addr > 0xFEFFFFFF)
		memmove( dest, ((uint8_t*)m->rom)+(addr-0xFF000000), len );
	else
	{
		// TODO
	}
	
	return 0;
}

int32_t emu_memory_read_string(struct emu_memory *m, uint32_t addr, struct emu_string *s, uint32_t maxsize)
{
	uint32_t i = 0;
	
	void *address;
	
	while( 1 )
	{
		if (i > maxsize - 1)
			return -1;
		
		/*address = translate_addr(m, addr + i);

		if( address == NULL )
			return -1;*/

		/* TODO: Fix this */
		if( ((uint8_t *)m->mem)[addr+i] == '\0' )
			break;

		i++;
	}

	s->data = malloc(i + 1);
	memset(s->data, 0, i + 1);
	s->size = i;

	return emu_memory_read_block(m, addr, s->data, i);
}


int32_t emu_memory_write_byte(struct emu_memory *m, uint32_t addr, uint8_t byte)
{
	if ( m->read_only_access == true )
		return 0;
	

	addr += m->segment_offset;

	if(addr > 0 && addr <= RAM_SIZE)
		((uint8_t *)m->mem)[addr] = byte;
	else if( addr > 0xFEFFFFFF )
		((uint8_t *)m->rom)[addr-0xFF000000] = byte;
	else
	{
		for( int i = 0; i < MAX_MEM_HANDLERS; i++ )
		{
			if( m->mem_handler[i].mem_start == 0 && m->mem_handler[i].mem_end == 0 )
				continue;

			if( addr >= m->mem_handler[i].mem_start && addr <= m->mem_handler[i].mem_end )
			{
				m->mem_handler[i].write8(addr, byte);
				break;
			}
		}
	}
	
	return 0;
}

int32_t emu_memory_write_word(struct emu_memory *m, uint32_t addr, uint16_t word)
{
	if (m->read_only_access == true)
		return 0;

#if BYTE_ORDER == BIG_ENDIAN
	uint16_t val;
	bcopy(&word, &val, 2);
	val = ((val & 0xff00) >> 8) | 
		  ((val & 0x00ff) << 8);
	return emu_memory_write_block(m, addr, &val, 2);
#else
	if( addr < RAM_SIZE )
		return emu_memory_write_block(m, addr, &word, 2);
	else
	{
		for( int i = 0; i < MAX_MEM_HANDLERS; i++ )
		{
			if( m->mem_handler[i].mem_start == 0 && m->mem_handler[i].mem_end == 0 )
				continue;

			if( addr >= m->mem_handler[i].mem_start && addr <= m->mem_handler[i].mem_end )
			{
				m->mem_handler[i].write16(addr, word);
				break;
			}
		}
	}
#endif
	return 0;
}

int32_t emu_memory_write_dword(struct emu_memory *m, uint32_t addr, uint32_t dword)
{
	if (m->read_only_access == true)
		return 0;

#if BYTE_ORDER == BIG_ENDIAN
	uint32_t val;
	bcopy(&dword, &val, 4);
	val = ((val & (0xff000000)) >> 24) |
		  ((val & (0x00ff0000)) >> 8)  |
		  ((val & (0x0000ff00)) << 8)  |
		  ((val & (0x000000ff)) << 24);
	return emu_memory_write_block(m, addr, &val, 4);
#else
	if( addr < RAM_SIZE )
		return emu_memory_write_block(m, addr, &dword, 4);
	else
	{
		for( int i = 0; i < MAX_MEM_HANDLERS; i++ )
		{
			if( m->mem_handler[i].mem_start == 0 && m->mem_handler[i].mem_end == 0 )
				continue;

			if( addr >= m->mem_handler[i].mem_start && addr <= m->mem_handler[i].mem_end )
			{
				m->mem_handler[i].write32(addr, dword);
				break;
			}
		}
	}
#endif
	return 0;
}


int32_t emu_memory_write_block(struct emu_memory *m, uint32_t addr, void *src, size_t len)
{
	if (m->read_only_access == true)
		return 0;

	uint32_t oaddr = addr; /* save original addr for recursive call */
	addr += m->segment_offset;

	if(addr > 0 && addr <= RAM_SIZE)
		memmove( ((uint8_t*)m->mem)+addr, src, len );
	else if( addr > 0xFEFFFFFF )
		memmove( ((uint8_t*)m->rom)+(addr-0xFF000000), src, len );
	else
	{
		// TODO 
	}

	/*void *address = translate_addr(m, addr);

	if( address == NULL )
	{
		if( page_alloc(m, addr) == -1 )
			return -1;

		address = translate_addr(m, addr);
	}

	if (OFFSET(addr) + len <= PAGE_SIZE)
	{
		bcopy(src, address, len);
		return 0;
	}
	else
	{
		uint32_t cb = PAGE_SIZE - OFFSET(addr);
		bcopy(src, address, cb);
		return emu_memory_write_block(m, oaddr + cb, ((uint32_t*)src) + cb, len - cb);
	}*/

	return 0;

}

void emu_memory_segment_select(struct emu_memory *m, enum emu_segment s)
{
	m->segment_current = s;
	m->segment_offset = m->segment_table[m->segment_current];
}

enum emu_segment emu_memory_segment_get(struct emu_memory *m)
{
	return m->segment_current;
}

/* make sure a memory block of size *len* is allocated at *addr* */
/*int32_t emu_memory_alloc_at(struct emu_memory *m, uint32_t addr, size_t len)
{
	len += addr % PAGE_SIZE;
	addr = (addr >> PAGE_BITS) << PAGE_BITS;
	
	while( len > 0 )
	{
		if( len > PAGE_SIZE )
		{
			len -= PAGE_SIZE;
			page_alloc(m, addr);
			addr += PAGE_SIZE;
		}
		else
		{
			len -= len; 
			page_alloc(m, addr);
		}
	}

	return 0;
}*/

int32_t emu_memory_alloc(struct emu_memory *m, uint32_t *addr, size_t len)
{
//	*addr = 0x00200000;
//	
//	uint32_t pages = len / PAGE_SIZE;
//	
//	if( len % PAGE_SIZE != 0 )
//	{
//		pages++;
//	}
//
//	int i;
//	
//	/* TODO: ensure termination */
//	for( ; ; )
//	{
//		for( i = 0; i < pages; i++ )
//		{
//			if( page_is_alloc(m, *addr + i * PAGE_SIZE) != 0 )
//			{
//				break;
//			}
//		}
//		
//		if( i == pages )
//		{
//			for( i = 0; i < pages; i++ )
//			{
//				if( page_alloc(m, *addr + i * PAGE_SIZE) )
//				{
//					return -1;
//				}
//			}
//			
//			return 0;
//		}
//		
//		*addr += len + PAGE_SIZE;
//	}
//	
	return -1;
}

void emu_memory_mode_ro(struct emu_memory *m)
{
	m->read_only_access = true;
}

void emu_memory_mode_rw(struct emu_memory *m)
{
	m->read_only_access = false;
}

