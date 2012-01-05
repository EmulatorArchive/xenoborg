#include "..\\win32.h"
#include "pci.h"


/* PCI host bridge memory range: 0x40000000 - 0x7FFFFFFF */
/* 1gb, prefetchable memory */

/* pci devices: all 14 of them */
struct pci_device_t pci_devices[14];


/* For now, this just sets the more important PCI registers */
void pci_init_devices()
{
	/* PCI host bridge */
	pci_devices[0].address = pci_addr( 0, 0, 0 );
	/* Memory controller */
	pci_devices[1].address = pci_addr( 0, 0, 3 );
	/* ISA bridge */
	pci_devices[2].address = pci_addr( 0, 1, 0 );
	/* SMBus controller */
	pci_devices[3].address = pci_addr( 0, 1, 1 );
	/* USB1 (controller ports 1 and 2) */
	pci_devices[4].address = pci_addr( 0, 2, 0 );
	/* USB2 (controller ports 3 and 4) */
	pci_devices[5].address = pci_addr( 0, 3, 0 );
	/* MCPX Network adapter */
	pci_devices[6].address = pci_addr( 0, 4, 0 );
	/* MCPX APU (soundstorm) */
	pci_devices[7].address = pci_addr( 0, 5, 0 );
	/* AC97 codec */
	pci_devices[8].address = pci_addr( 0, 6, 0 );
	/* MC97 (no, I don't actually plan on emulating this!) */
	pci_devices[0].address = pci_addr( 0, 6, 1 );
	/* PCI bridge (PCI-to-PCI) */
	pci_devices[0].address = pci_addr( 0, 8, 0 );
	/* IDE controller */
	pci_devices[0].address = pci_addr( 0, 9, 0 );
	/* AGP bridge (AGP-to-PCI) */
	pci_devices[0].address = pci_addr( 0, 30, 0 );
	/* NV2A Northbridge (VGA compatible) */
	pci_devices[0].address = pci_addr( 1, 0, 0 );
}

uint8_t pci_host_read8( uint32_t addr )
{
	return 0;
}

void	pci_host_write8( uint32_t addr, uint8_t byte )
{
}

uint16_t pci_host_read16( uint32_t addr )
{
	return 0;
}

void	pci_host_write16( uint32_t addr, uint16_t word )
{
}

uint32_t pci_host_read32( uint32_t addr )
{
	return 0;
}

void	pci_host_write32( uint32_t addr, uint32_t dword )
{
}

