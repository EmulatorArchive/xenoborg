#pragma once

/* pci registers */
#pragma pack(push, 1)
struct pci_regs_t
{
	uint16_t vendor_id		: 16;
	uint16_t device_id		: 16;
	uint16_t command		: 16;
	uint16_t status			: 16;
	uint8_t  revision_id	: 8;
	uint8_t  class_code		: 8;
	uint16_t class_code2	: 16;
	uint8_t	 cache_line_size: 8;
	uint8_t  latency_timer	: 8;
	uint8_t  header_type	: 8;
	uint8_t  self_test		: 8;
	uint32_t base_address0	: 32;
	uint32_t base_address1	: 32;
	uint32_t base_address2	: 32;
	uint32_t base_address3	: 32;
	uint32_t base_address4	: 32;
	uint32_t base_address5	: 32;
	uint32_t reserved0		: 32;
	uint32_t reserved1		: 32;
	uint32_t expansion_rom_base_address	: 32;
	uint32_t reserved2		: 32;
	uint32_t reserved3		: 32;
	uint8_t  interrupt_line	: 8;
	uint8_t	 interrupt_pin	: 8;
	uint8_t  min_gnt		: 8;
	uint8_t  max_lat		: 8;
	uint8_t  reserved4[0xC0];
};
#pragma pack(pop)


/* pci device structure */
struct pci_device_t
{
	uint32_t address;
	struct pci_regs_t regs;
};

/* encoded pci device address */
/* 0x8000000 = enabled */
#define pci_addr(bus, device, function) 0x80000000 | (bus << 16) | (device << 11) | (function << 8)
