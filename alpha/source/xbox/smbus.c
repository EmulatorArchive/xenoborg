#include "..\\win32.h"
#include "smbus.h"

/* NOTES 
When accessing an SMBus device, there are 5 ports to be dealt with.
Usually, they are accessed in this order AFAIK.

Writing:
Write port 0xc004 (Address)
Write port 0xc008 (Command)
Write port 0xc006 (Data)
Read port 0xc000 (Status)
Write port 0xc002 (Control) <- the operation begins when this port is written to

Reading:
Write port 0xc004 (Address)
Write port 0xc008 (Command)
Write port 0xc000 (Status)
Write port 0xc002 (Control)
Read port 0xc006 (Data)

My knowledge of the SMBus and how it's ports work is fairly limited, any comments
are greatly appreciated :)
*/

/* PIC Power state flags (there are NOT bitflags) */
#define PIC_POWER_RESET 0x01
#define PIC_POWER_CYCLE 0x40
#define PIC_POWER_OFF	0x80

struct pic_t	pic;		/* Programmable Interrupt Controller */
struct smbus_t	smb;		/* System Management Bus */
unsigned char	system_temperature = 30;

#define pic_debug_kit 0

// Read/Write port mapped I/O for SMBus devices
unsigned char xinpb( unsigned short Port )
{
	// Interpret Port address
	switch( Port )
	{
	case 0xC000:	// Status
		return smb.status;
	case 0xC004:	// Address
		return smb.address;
	case 0xC002:	// Control
		return smb.control;
	case 0xC006:	// Data
		return smb.data;
	case 0xC008:	// Command
		return smb.command;
	}

	printf( "xinpb: Unknown SMB Port! := 0x%x\n", Port );

	return 0x00;
}

unsigned short xinpw( unsigned short Port )
{
	// Interpret Port address
	switch( Port )
	{
	case 0xC000:	// Status
		return (unsigned short) smb.status;
	case 0xC004:	// Address
		return (unsigned short) smb.address;
	case 0xC002:	// Control
		return (unsigned short) smb.control;
	case 0xC006:	// Data
		return (unsigned short) smb.data;
	case 0xC008:	// Command
		return (unsigned short) smb.command;
	}

	printf( "xinpw: Unknown SMB Port! := 0x%x\n", Port );

	return 0x0000;
}

unsigned long xinpdw( unsigned short Port )
{
	// Interpret Port address
	switch( Port )
	{
	case 0xC000:	// Status
		return (unsigned long) smb.status;
	case 0xC004:	// Address
		return (unsigned long) smb.address;
	case 0xC002:	// Control
		return (unsigned long) smb.control;
	case 0xC006:	// Data
		return (unsigned long) smb.data;
	case 0xC008:	// Command
		return (unsigned long) smb.command;
	}

	printf( "xinpdw: Unknown SMB Port! := 0x%x\n", Port );

	return 0x00000000;
}


VOID xoutpb( unsigned short Port, unsigned char Data )
{
	// Interpret Port address
	switch( Port )
	{
	case 0xC000:	// Status
		smb.status = Data;
		break;
	case 0xC004:	// Address
		smb.address = Data;
		break;
	case 0xC002:	// Control
		smb.control = Data;

		switch( smb.address )
		{
		case 0x20:	// PIC
			pic_write( smb.command, smb.data );
			break;
		case 0x8A:	// Video encoder (Conexant)
		case 0x98:	// System temp monitor
		case 0x99:
		case 0xA8:	// EEPROM
			printf( "xoutpb: Unhandled device!\nPort 0x%x Data 0x%x\n", Port, Data );
			break;
		}
		break;
	case 0xC006:	// Data
		smb.data = Data;
		break;
	case 0xC008:	// Command
		smb.command = Data;
		break;
	default:
		printf( "xoutpb: Unknown SMB Port! := 0x%x\n", Port );
		break;
	}
}

VOID xoutpw( unsigned short Port, unsigned short Data )
{
	// Interpret Port address
	switch( Port )
	{
	case 0xC000:	// Status
		smb.status = Data;
		break;
	case 0xC004:	// Address
		smb.address = Data;
		break;
	case 0xC002:	// Control
		smb.control = Data;

		switch( smb.address )
		{
		case 0x20:	// PIC
			pic_write( smb.command, smb.data );
			break;
		case 0x8A:	// Video encoder (Conexant)
		case 0x98:	// System temp monitor
		case 0x99:
		case 0xA8:	// EEPROM
			printf( "xoutpw: Unhandled device!\nPort 0x%x Data 0x%x\n", Port, Data );
			break;
		}
		break;
	case 0xC006:	// Data
		smb.data = Data;
		break;
	case 0xC008:	// Command
		smb.command = Data;
		break;
	default:
		printf( "xoutpw: Unknown SMB Port! := 0x%x\n", Port );
		break;
	}
}

VOID xoutpdw( unsigned short Port, unsigned long Data )
{
	// Interpret Port address
	switch( Port )
	{
	case 0xC000:	// Status
		smb.status = Data;
		break;
	case 0xC004:	// Address
		smb.address = Data;
		break;
	case 0xC002:	// Control
		smb.control = Data;

		switch( smb.address )
		{
		case 0x20:	// PIC
			pic_write( smb.command, smb.data );
			break;
		case 0x8A:	// Video encoder (Conexant)
		case 0x98:	// System temp monitor
		case 0x99:
		case 0xA8:	// EEPROM
			printf( "xoutpdw: Unhandled device!\nPort 0x%x Data 0x%x\n", Port, Data );
			break;
		}
		break;
	case 0xC006:	// Data
		smb.data = Data;
		break;
	case 0xC008:	// Command
		smb.command = Data;
		break;
	default:
		printf( "xoutpdw: Unknown SMB Port! := 0x%x\n", Port );
		break;
	}
}

void pic_reset( void )
{
	ZeroMemory( &pic, sizeof( struct pic_t ) );

#if pic_debug_kit // Debug Xbox (DXB)
	pic.pic_version[0] = 0x44;
	pic.pic_version[1] = 0x58;
	pic.pic_version[2] = 0x42;
#else	// Retail Xbox v1.0 (P01)
	pic.pic_version[0] = 0x50;
	pic.pic_version[1] = 0x30;
	pic.pic_version[2] = 0x31;
#endif
	pic.tray_state_r = 1;		// Tray state loaded
	pic.av_pack_state = 0x06;	// Standard
	pic.cpu_temperature = 25; 
	pic.board_temperature = 25;
	pic.fan_speed = 20;
}

unsigned char pic_read( unsigned char Command )
{
	unsigned char ret_val;

	// Interpret PIC read command
	switch( Command )
	{
	case 0x01:	// PIC version string
		ret_val = pic.pic_version[pic.pic_ver_string_counter++];
		if( pic.pic_ver_string_counter == 3 )
			pic.pic_ver_string_counter = 0;
		return ret_val;

	case 0x03:	// Tray state
		return pic.tray_state_r;

	case 0x04:
		return pic.av_pack_state;

	case 0x09:
		return pic.cpu_temperature;

	case 0x0A:
		return pic.board_temperature;

	case 0x0F:
		return pic.scratch_register;

	case 0x10:
		return pic.fan_speed;

	case 0x11:
		return pic.irq_reason;

	case 0x18:
		break;

	case 0x1B:
		return pic.krnl_scratch_reg;

	default:
		break;
	}

	printf( "***Unhandled PIC Command (PICRead) 0x%2.2x\n", Command );

	return 0x0;
}


void pic_write( unsigned char Command, unsigned char Data )
{
	switch( Command )
	{
	case 0x01:
		if( Data == 0x00 )
			pic.pic_ver_string_counter = Data;
		break;

	case 0x02:
		pic.reset_and_poweroff_ctrl = Data;

		// I'll deal with this later
		switch( Data )
		{
		case 0x01:	// Reset
			printf( "Xbox has been reset!" );
			break;
		case 0x40:	// Power cycle
			printf( "Xbox is undergoing power cycle!" );
			break;
		case 0x80:	// Power off
			printf( "Xbox has shutdown!" );
			break;
		default:
			printf( "***PICWrite: Unknown power state! 0x%2.2x\n", Data );
			break;
		}
		break;

	case 0x05:
		pic.power_fan_mode = Data;

		if( Data == 0 )
			pic.power_fan_speed = 20;
		break;

	case 0x06:
		pic.power_fan_speed = Data;
		break;

	case 0x07:
		pic.led_mode = Data;
		break;

	case 0x08:
		pic.led_flash_seq = Data;
		break;

	case 0x0C:
		pic.tray_eject = Data;
		if( Data == 0 )
			MessageBoxW( GetActiveWindow(), L"DVD tray has been ejected!\n", L"Xenoborg", MB_OK );
		else if( Data == 1 )
			MessageBoxW( GetActiveWindow(), L"DVD tray has been closed!\n", L"Xenoborg", MB_OK );
		break;

	case 0x0E:
		break;

	case 0x19:
		pic.reset_on_eject = Data;
		break;

	case 0x1A:
		pic.irq_enable = Data;
		break;

	case 0x1B:
		pic.scratch_register = Data;
		break;
	}
}


void smbus_reset()
{
	printf( "SMBus Init...\n" );

	pic_reset();
}