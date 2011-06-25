/*
 * Xenoborg - Xbox Emulator
 * Copyright (C) 2006-2010  blueshogun96
 *
 * Name: Xbe.h
 * Desc: Xbe loader.
 *
 * Changelog: 
 *	10/14/2010 - Initial import from an old build. Fixed it up a bit because
 *				 the old version looks so ugly.
 *
 *	10/15/2010 - Added support for Sega Chihiro .xbe files.  The XOR keys are
 *				 different than normal Xbox xbe files.  Chihiro mode will be a
 *				 seperate Xenoborg build though.
 */


#ifndef XBE_H
#define XBE_H


//******************************************
// Important Xbe defines
//******************************************

// Known (or at least used) Xbe flags
#define MountUtilityDrive	0x00000001
#define FormatUtilityDrive	0x00000002
#define Limit64Megabytes	0x00000004
#define DontSetUpHardDisk	0x00000008

#define XOR_ENTRY_DEBUG		0x94859d4b
#define XOR_ENTRY_RETAIL	0xA8FC57AB //0xa8fc57ab
#define XOR_KERNEL_DEBUG		0xefb1f152
#define XOR_KERNEL_RETAIL		0x5b6d40b6

// Sega Chihiro .xbe have a different XOR key.
#define XOR_ENTRY_CHIHIRO	0x40B5C16E
#define XOR_KERNEL_CHIHIRO	0x2290059D

#define XBEIMAGE_MEDIA_TYPE_HARD_DISK	0x00000001
#define XBEIMAGE_MEDIA_TYPE_DVD_X2		0x00000002
#define XBEIMAGE_MEDIA_TYPE_DVD_CD		0x00000004
#define XBEIMAGE_MEDIA_TYPE_CD			0x00000008
#define XBEIMAGE_MEDIA_TYPE_DVD_5_RO	0x00000010
#define XBEIMAGE_MEDIA_TYPE_DVD_9_RO	0x00000020
#define XBEIMAGE_MEDIA_TYPE_DVD_5_RW	0x00000040
#define XBEIMAGE_MEDIA_TYPE_DVD_9_RW	0x00000080
#define XBEIMAGE_MEDIA_TYPE_DONGLE		0x00000100
#define XBEIMAGE_MEDIA_TYPE_MEDIA_BOARD	0x00000200
#define XBEIMAGE_MEDIA_TYPE_NONSECURE_HARD_DISK	0x40000000
#define XBEIMAGE_MEDIA_TYPE_NONSECURE_mode	0x80000000
#define XBEIMAGE_MEDIA_TYPE_MEDIA_MASK	0x00FFFFFF

// Game info 
#define XBEIMAGE_GAME_REIGON_NA		0x00000001
#define XBEIMAGE_GAME_REIGON_JAPAN	0x00000002
#define XBEIMAGE_GAME_REIGON_RESTOFWORLD	0x00000004
#define XBEIMAGE_GAME_REIGON_MANUFACTURING	0x80000000

// library defines
#define LIB_QFEVERSION	0x1fff
#define LIB_APPROVED	0x6000
#define LIB_DEBUG_BUILD 0x8000

// section flags
#define SECTION_WRITABLE	0x00000001
#define SECTION_PRELOAD		0x00000002
#define SECTION_EXECUTABLE	0x00000004
#define SECTION_INSERTED_FILE	0x00000008
#define SECTION_HEAD_PAGE_READ_ONLY	0x00000010
#define SECTION_TAIL_PAGE_READ_ONLY	0x00000020



#pragma pack( 1 )
	typedef struct _XbeImageHeader
	{
		char sbSignature[4];
		BYTE ubAuthorizationSignature[0x100];
		DWORD dwBaseAddr;
		DWORD dwSizeHeader;
		DWORD dwSizeImage;
		DWORD dwSizeImageHeader;
		DWORD dwTimeDate;
		DWORD dwCertificateAddr;
		DWORD dwNumSections;
		DWORD dwSectionAddr;
		DWORD dwInitFlags;
		DWORD dwEntryPoint;
		DWORD dwTLSAddr;
		DWORD dwPlsStackCommit;
		DWORD dwPeHeapReserve;
		DWORD dwPeHeapCommit;
		DWORD dwPeSizeImage;
		DWORD dwPeChecksum;
		DWORD dwTimedata;
		DWORD dwPathnameAddr;
		DWORD dwFilenameAddr;
		DWORD dwUnicodeFilenameAddr;
		DWORD dwKernelThunkAddr;
		DWORD dwNonKernelDirAddr;
		DWORD dwNumLibVersions;
		DWORD dwLibVersionAddr;
		DWORD dwKernelLibVersionAddr;
		DWORD dwXapiLibVersionAddr;
		DWORD dwLogoBitmapAddr;
		DWORD dwLogoBitmapSize;
	}
#pragma pack()
	XbeImageHeader;

#pragma pack( 1 )
	typedef struct _XbeCertificate
	{
		DWORD dwSize;
		DWORD dwTimeDate;
		DWORD dwTitleID;
		wchar_t wcTitle[0x28];
		DWORD dwAlternateTitleIDs[0x16];
		DWORD dwMediaFlags;
		DWORD dwGameReigon;
		DWORD dwGameRatings;
		DWORD dwCertificateVersion;
		BYTE ubLanKey[0x10];
		BYTE ubSignatureKey[0x10];
		BYTE ubAltSignatureKey[0x10 * 0x10];
	}
#pragma pack()
	XbeCertificate;

#pragma pack( 1 )
	typedef struct _XbeSectionHeader
	{
		DWORD dwFlags;
		DWORD dwVirtualAddr;
		DWORD dwVirtualSize;
		DWORD dwRawAddr;
		DWORD dwRawSize;
		DWORD dwSectionNameAddr;
		DWORD dwSectionNameReferenceCount;
		DWORD dwHeadSharedPageReferenceCountAddr;
		DWORD dwTailSharedPageReferenceCountAddr;
		BYTE ubSectionDigest[0x14];
	}
#pragma pack()
	XbeSectionHeader;

#pragma pack( 1 )
	typedef struct _XbeLibrary
	{
		char sbLibraryName[8];
		WORD wMajorVersion;
		WORD wMinorVersion;
		WORD wBuildVersion;
		DWORD dwFlags;
	}
#pragma pack()
	XbeLibrary;

#pragma pack( 1 )
	typedef struct _XbeTLS
	{
		DWORD dwDataStartAddr;
		DWORD dwDataEndAddr;
		DWORD dwTlsIndexAddr;
		DWORD dwTlsCallbackAddr;
		DWORD dwSizeZeroFill;
		DWORD dwCharacteristics;
	}
#pragma pack()
	XbeTLS;

//***********************************************
// Xbe struct
//***********************************************
struct Xbe_t
{
	BYTE*	m_pXbeData;
	char*	m_szXbeFileName;
	BOOL	m_bXbeHasBeenOpened;
	long	m_lXbeFileSize;
	DWORD	m_dwActualEntryPoint;
	BOOL	m_bIsChihiro;

	XbeImageHeader		m_XbeHeader;
	XbeCertificate		m_XbeCertificate;
	XbeSectionHeader*	m_XbeSectionHeader;
	XbeLibrary*			m_XbeStandardLibrary, m_XbeKernelLibrary, m_XbeXapiLibrary;
	XbeTLS				m_XbeTLS;
};


BOOL XBELoad( char* szXbeFileName );
void XBEUnload();
BOOL XBEProcessData();
BOOL XBEWriteToMemory();

extern Xbe_t Xbe;

#endif // Xbe_h