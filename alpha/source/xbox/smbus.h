#pragma once

struct smbus_t
{
	// Ports
	unsigned char status;	// 0xC000
	unsigned char control;	// 0xC002
	unsigned char address;	// 0xC004
	unsigned char data;		// 0xC006
	unsigned char command;	// 0xC008
};

struct pic_t
{
	unsigned char pic_version[3];			// 0x01	(r)
	unsigned char pic_ver_string_counter;	// 0x01 (w)
	unsigned char reset_and_poweroff_ctrl;	// 0x02	(w)
	unsigned char tray_state_r;				// 0x03 (r)
	unsigned char av_pack_state;			// 0x04 (r)
	unsigned char power_fan_mode;			// 0x05	(w)
	unsigned char power_fan_speed;			// 0x06 (w)
	unsigned char led_mode;					// 0x07 (w)
	unsigned char led_flash_seq;			// 0x08 (w)
	unsigned char cpu_temperature;			// 0x09 (r)
	unsigned char board_temperature;		// 0x0A (r)
	unsigned char tray_eject;				// 0x0C	(w)
	unsigned char unknown_scratch_register;	// 0x0E (w?)
	unsigned char scratch_register;			// 0x0F (r)
	unsigned char fan_speed;				// 0x10 (r)
	unsigned char irq_reason;				// 0x11 (r)
	unsigned char overheat;					// 0x18 (r)
	unsigned char reset_on_eject;			// 0x19 (w)
	unsigned char irq_enable;				// 0x1A	(r?/w)
	unsigned char krnl_scratch_reg;			// 0x1B (r/w)
};

// This struct is a slightly moddified version of the one found below:
// http://www.xbox-linux.org/docs/Xbox_Serial_EEPROM.html

#pragma pack( 1 )
struct eeprom_t
{
	unsigned long           HMAC_SHA1_Hash	: 20;	// 0x00 - 0x13 HMAC_SHA1 Hash
	unsigned long           Confounder		: 8;	// 0x14 - 0x1B RC4 Encrypted Confounder ??
	unsigned long           HDDKkey			: 16;	// 0x1C - 0x2B RC4 Encrypted HDD key
	unsigned long           XBERegion		: 4;	// 0x2C - 0x2F RC4 Encrypted Region code (0x01 North America, 0x02 Japan, 0x04 Europe)
	unsigned long           Checksum2		: 4;	// 0x30 - 0x33 Checksum of next 44 bytes
	unsigned long           SerialNumber	: 12;	// 0x34 - 0x3F Xbox serial number
	unsigned long           MACAddress		: 6;	// 0x40 - 0x45 Ethernet MAC address
	unsigned long           UNKNOWN2		: 2;	// 0x46 - 0x47 Unknown Padding ?
 
	unsigned long           OnlineKey		: 16;	// 0x48 - 0x57 Online Key ?
 
	unsigned long           VideoStandard	: 4;	// 0x58 - 0x5B  0x00800300 = PAL, 0x00400100 = NTSC, 0x00400200=NTSC_J 
 
	unsigned long           UNKNOWN3		: 4;	// 0x5C - 0x5F  Unknown Padding ?
 
    //Comes configured up to here from factory..  everything after this can be zero'd out...
    //To reset XBOX to Factory settings, Make checksum3 0xFFFFFFFF and zero all data below (0x64-0xFF)
    //Doing this will Reset XBOX and upon startup will get Language & Setup screen...
	unsigned long           Checksum3		: 4;	// 0x60 - 0x63  other Checksum of next
 
	unsigned long           TimeZoneBias	: 4;	// 0x64 - 0x67 Zone Bias?
	unsigned long           TimeZoneStdName	: 4;	// 0x68 - 0x6B Standard timezone
	unsigned long           TimeZoneDltName	: 4;	// 0x5C - 0x6F Daylight timezone
	unsigned long           UNKNOWN4		: 8;	// 0x70 - 0x77 Unknown Padding ?
	unsigned long           TimeZoneStdDate	: 4;	// 0x78 - 0x7B 10-05-00-02 (Month-Day-DayOfWeek-Hour)
	unsigned long           TimeZoneDltDate	: 4;	// 0x7C - 0x7F 04-01-00-02 (Month-Day-DayOfWeek-Hour)
	unsigned long           UNKNOWN5		: 8;	// 0x80 - 0x87 Unknown Padding ?
	unsigned long           TimeZoneStdBias	: 4;	// 0x88 - 0x8B Standard Bias?
	unsigned long           TimeZoneDltBias	: 4;	// 0x8C - 0x8F Daylight Bias?
 
	unsigned long           LanguageID		: 4;	// 0x90 - 0x93 Language ID
 
	unsigned long           VideoFlags		: 4;	// 0x94 - 0x97 Video Settings - 0x96 b0==widescreen 0x96 b4 == letterbox
	unsigned long           AudioFlags		: 4;	// 0x98 - 0x9B Audio Settings
 
	unsigned long           ParentalControlGames	: 4;	// 0x9C - 0x9F 0=MAX rating
	unsigned long           ParentalControlPwd		: 4;	// 0xA0 - 0xA3 7=X, 8=Y, B=LTrigger, C=RTrigger
	unsigned long           ParentalControlMovies	: 4;	// 0xA4 - 0xA7 0=Max rating
 
	unsigned long           XBOXLiveIPAddress		: 4;	// 0xA8 - 0xAB XBOX Live IP Address..
	unsigned long           XBOXLiveDNS			: 4;		// 0xAC - 0xAF XBOX Live DNS Server..
	unsigned long           XBOXLiveGateWay		: 4;		// 0xB0 - 0xB3 XBOX Live Gateway Address..
	unsigned long           XBOXLiveSubNetMask		: 4;	// 0xB4 - 0xB7 XBOX Live Subnet Mask..
	unsigned long           OtherSettings			: 4;	// 0xA8 - 0xBB Other XBLive settings ?
 
	unsigned long           DVDPlaybackKitZone		: 4;	// 0xBC - 0xBF DVD Playback Kit Zone
 
	unsigned __int64        UNKNOWN6				: 64;	// 0xC0 - 0xFF Unknown Codes / Memory timing table?
};
#pragma pack()

// Read/Write port mapped I/O for SMBus devices
unsigned char xinpb( unsigned short Port );
unsigned short xinpw( unsigned short Port );
unsigned long xinpdw( unsigned short Port );

void xoutpb( unsigned short Port, unsigned char Data );
void xoutpw( unsigned short Port, unsigned short Data );
void xoutpdw( unsigned short Port, unsigned long Data );

void smbus_reset( void );

/* PIC (Programmable Interrupt Controller) functions */
void pic_reset( void );
unsigned char pic_read( unsigned char Command );
void pic_write( unsigned char Command, unsigned char Data );
void pic_irq( void );
