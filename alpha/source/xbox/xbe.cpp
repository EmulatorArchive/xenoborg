/*
 * Xenoborg - Xbox Emulator
 * Copyright (C) 2006-2010  blueshogun96
 *
 * Name: xbe.h
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


#pragma warning (disable:4996)

#include "..\\win32.h"
#include "..\\dbg_console.h"
#include "..\\crc32.h"
#include "xbe.h"


//-----------------------------------------------------------------------------
// Name: OpenFile
// Desc: Opens an xbe file, determines some basic
//		 properties such as file size, and loads
//		 the data into system memory.
//-----------------------------------------------------------------------------
int xbe_load( char* szXbeFileName, struct Xbe_t* xbe )
{
	// File pointer for xbe file
	FILE* fpXbe = NULL;

	// Check for valid string
	if( !szXbeFileName )
	{
		DbgPrintf( "XBELoad(): Invalid String!\n" );
		return FALSE;
	}

	// Open xbe file
	if( !( fpXbe = fopen( szXbeFileName, "rb" ) ) )
	{
		DbgPrintf( "XBELoad(): Unable to load Xbe file!\n" );
		return FALSE;
	}

	// Allocate memory for saved string
//	memset( xbe->m_szXbeFileName, 0x0, sizeof( *xbe->m_szXbeFileName ) );

//	xbe->m_szXbeFileName = (char*) malloc( sizeof( char ) * strlen( szXbeFileName ) ); // new char[ strlen( szXbeFileName ) ];
//	strcpy( xbe->m_szXbeFileName, szXbeFileName );
	xbe->m_szXbeFileName = szXbeFileName;

	// Retreive xbe's size
	fseek( fpXbe, 0, SEEK_END );
	xbe->m_lXbeFileSize = ftell( fpXbe );

	// Allocate memory for xbe data
	xbe->m_pXbeData = /*(BYTE*) malloc( sizeof( BYTE ) * xbe->m_lXbeFileSize );*/
		new unsigned char[ xbe->m_lXbeFileSize ];

	// Close xbe file
	fclose( fpXbe );

	// Get the 32-bit crc
	crc_generate_tables();
	xbe->crc = crc_update( -1, xbe->m_pXbeData, xbe->m_lXbeFileSize );

	// Process Xbe's data
	if( !xbe_process_data(xbe) )
		return FALSE;

	// Mark xbe file as opened successfully.
	xbe->m_bXbeHasBeenOpened = 1;

	return TRUE;
}

//-----------------------------------------------------------------------------
// Name: CloseFile
// Desc: ...
//-----------------------------------------------------------------------------
void xbe_unload( struct Xbe_t* xbe )
{
	DbgPrintf( "Closing Xbe file... \n" );

	// Release data pointers and reset variables
	{
		if( xbe->m_pXbeData )
		{
			/*free( xbe->m_pXbeData );*/ delete[] xbe->m_pXbeData;
			xbe->m_pXbeData = NULL;
		}
/*
		if( xbe->m_szXbeFileName )
		{
			free( xbe->m_szXbeFileName );	// delete[] xbe->m_szXbeFileName;
			xbe->m_szXbeFileName = NULL;
		}
*/
		if( xbe->m_XbeSectionHeader )
		{
			free( xbe->m_XbeSectionHeader );
			xbe->m_XbeSectionHeader = NULL;
		}

		if( xbe->m_XbeStandardLibrary )
		{
			free( xbe->m_XbeStandardLibrary );
			xbe->m_XbeStandardLibrary = NULL;
		}
	}

	SetWindowTextA( GetActiveWindow(), "Xenoborg" );

	xbe->m_bXbeHasBeenOpened = 0;
}

//-----------------------------------------------------------------------------
// Name: ProcessXbeData
// Desc: Processes and organizes xbe's data into 
//		 sections, checks to see if the xbe file
//		 is legit.  Xbe has to have been previously
//		 loaded in order to use Xbe function.
//-----------------------------------------------------------------------------
int xbe_process_data( struct Xbe_t* xbe )
{
	FILE* fpXbe = NULL;		// File pointer for xbe file
	UINT* ptr = NULL;		// Temporary pointer to xbe data
//	UINT i;

	// Discontinue if the xbe file's existance has not been verified
//	if( !xbe->m_bXbeHasBeenOpened )
//		return FALSE;

	DbgPrintf( "Loading Xbe file...\n\n" );

	// Reopen the xbe file
	if( !( fpXbe = fopen( xbe->m_szXbeFileName, "r" ) ) )
	{ 
		DbgPrintf( "XBEProcessData(): Unable to load Xbe file!\n" );
		return FALSE;
	}

	// Seek to the beginning of file
	fseek( fpXbe, 0, SEEK_SET );

	// Read xbe data into memory pointer
	fread( xbe->m_pXbeData, xbe->m_lXbeFileSize, 1, fpXbe );

	// Close the file handle
	fclose( fpXbe );

	// Create a copy of the xbe data to prevent alteration of the
	// actual data itself.
	ptr = (UINT*) xbe->m_pXbeData;

	// Xbe legitimacy check
	{
		int offset = 0;				// File location offset
        char* header = "XBEH";		// Xbe header		

		// Check to see if we have a valid Xbe file
		for( offset = 0; offset != 4; offset++ )
		{
			// If any chars in the file's header don't match, we have
			// a bogus xbe file.
			if( ( (char*) ptr )[offset] != header[offset] )
			{
				DbgPrintf( "Xbe Error: Not a valid xbe file!\n" );
				return FALSE;
			}
		}

		// Xbe is legitimate!
	}

	// Xbe parsing
//	{
		DWORD offset = 0;				// File location offset

		// Xbe Header
		{
			// Fill in Xbe image header structure
			memcpy( &xbe->m_XbeHeader, xbe->m_pXbeData, sizeof( XbeImageHeader ) );
		}

		// Xbe Certificate
		{
			// Set the offset for the Xbe certificate
			offset = xbe->m_XbeHeader.dwCertificateAddr - xbe->m_XbeHeader.dwBaseAddr;
			// Read in the Xbe certificate data
			memcpy( &xbe->m_XbeCertificate, xbe->m_pXbeData + offset, sizeof( XbeCertificate ) );
		}

		// Xbe Section Header(s)
		{
			// Allocate memory for the Xbe's section headers
			xbe->m_XbeSectionHeader = (XbeSectionHeader*) malloc( sizeof( XbeSectionHeader ) * xbe->m_XbeHeader.dwNumSections );
			// = new XbeSectionHeader[xbe->m_XbeHeader.dwNumSections];

			// Set the offset for the first xbe section header
			offset = xbe->m_XbeHeader.dwSectionAddr - xbe->m_XbeHeader.dwBaseAddr;
		
			// Retreive data for each and every section header in the Xbe
			for( DWORD i = 0; i < xbe->m_XbeHeader.dwNumSections; i++ )
			{
				memcpy( &xbe->m_XbeSectionHeader[i], xbe->m_pXbeData + offset, sizeof( XbeSectionHeader ) );
				offset += sizeof( XbeSectionHeader );
			}
		}

		// Xbe TLS
		{
			if (xbe->m_XbeHeader.dwTLSAddr>xbe->m_XbeHeader.dwBaseAddr)
			{
				try
				{
					// Set the offset for the Xbe's TLS data
					offset = xbe->m_XbeHeader.dwTLSAddr - xbe->m_XbeHeader.dwBaseAddr;
					// Read TLS data
					memcpy( &xbe->m_XbeTLS, xbe->m_pXbeData + offset, sizeof( XbeTLS ) );
				}
				catch( ... )
				{
					DbgPrintf( "This Xbe Contains no TLS data.\n" );
				}
			}
			else
			{
				DbgPrintf( "This Xbe Contains no TLS data.\n" );
			}
		}

		// Xbe Librarie(s)
		{
			// Set the offset for the first standard library in the xbe
			offset = xbe->m_XbeHeader.dwLibVersionAddr - xbe->m_XbeHeader.dwBaseAddr;

			// Allocate memory for xbe's standard libraries
			xbe->m_XbeStandardLibrary = (XbeLibrary*) malloc( sizeof( XbeLibrary ) * xbe->m_XbeHeader.dwNumLibVersions );
			// = new XbeLibrary[xbe->m_XbeHeader.dwNumLibVersions];

			// Retreive data for each and every library
			for( DWORD i = 0; i < xbe->m_XbeHeader.dwNumLibVersions; i++ )
			{
				memcpy( &xbe->m_XbeStandardLibrary[i], xbe->m_pXbeData + offset, sizeof( XbeLibrary ) );
				offset += sizeof( XbeLibrary );
			}

			// Set the offset for the Xbe's kernel library
			offset = xbe->m_XbeHeader.dwKernelLibVersionAddr - xbe->m_XbeHeader.dwBaseAddr;

			// Read kernel library data
			if (xbe->m_XbeHeader.dwKernelLibVersionAddr)
			{
				try
				{
					memcpy( &xbe->m_XbeKernelLibrary, xbe->m_pXbeData + offset, sizeof( XbeLibrary ) );
				}
				catch( ... )
				{
					DbgPrintf( "This Xbe contains no Kernel Library.\n" );
				}
			}

			// Set the offset for the Xbe's Xapi library
			offset = xbe->m_XbeHeader.dwXapiLibVersionAddr - xbe->m_XbeHeader.dwBaseAddr;

			// Read Xapi library data
			if (xbe->m_XbeHeader.dwXapiLibVersionAddr && int(offset) > 0 )
			{
				try
				{
					memcpy( &xbe->m_XbeXapiLibrary, xbe->m_pXbeData + offset, sizeof( XbeLibrary ) );
				}
				catch( ... )
				{
					DbgPrintf( "This Xbe contains no Xapi Library.\n" );
				}
			}
		}

		// Output information to debug file
		DbgPrintf( "\n**********Xbe general**********\n" );
		DbgPrintf( "Xbe file name: %s\nFile size: %d bytes\n", xbe->m_szXbeFileName, xbe->m_lXbeFileSize );
		DbgPrintf( "Base Address: 0x%08X\n", xbe->m_XbeHeader.dwBaseAddr );
		DbgPrintf( "CRC32: 0x%.08X\n", xbe->crc );

		// Check for a Sega Chihiro .xbe
		if( ( xbe->m_XbeHeader.dwEntryPoint & 0xF0000000 ) == 0x40000000 )
		{
			DbgPrintf( "Xbe type: Chihiro\n" );

			// This is a Sega Chihiro .xbe
			xbe->m_bIsChihiro = TRUE;

			// Decode actual entry point
			xbe->m_dwActualEntryPoint = xbe->m_XbeHeader.dwEntryPoint ^ XOR_ENTRY_CHIHIRO;
			DbgPrintf( "Entry point: 0x%08X ^ 0x%08X = 0x%08X\n", xbe->m_XbeHeader.dwEntryPoint, XOR_ENTRY_CHIHIRO,
				xbe->m_dwActualEntryPoint );

			// Decode kernel thunk location
			DbgPrintf( "Kernel Thunk Address: 0x%08X ^ 0x%08X = 0x%08X\n", xbe->m_XbeHeader.dwKernelThunkAddr, XOR_KERNEL_CHIHIRO,
				xbe->m_XbeHeader.dwKernelThunkAddr ^ XOR_KERNEL_CHIHIRO );
		}
		else
		{
			DbgPrintf( "Xbe type: Xbox\n" );

			// This is a Microsoft Xbox .xbe
			xbe->m_bIsChihiro = FALSE;

			if( ( xbe->m_XbeHeader.dwEntryPoint ^ XOR_ENTRY_RETAIL ) > 0x01000000 )
			{
				// Decode actual entry point
				xbe->m_dwActualEntryPoint = xbe->m_XbeHeader.dwEntryPoint ^ XOR_ENTRY_DEBUG;
				DbgPrintf( "Entry point: 0x%08X ^ 0x%08X = 0x%08X\n", xbe->m_XbeHeader.dwEntryPoint, XOR_ENTRY_DEBUG,
					xbe->m_dwActualEntryPoint );

				// Decode kernel thunk location
				DbgPrintf( "Kernel Thunk Address: 0x%08X ^ 0x%08X = 0x%08X\n", xbe->m_XbeHeader.dwKernelThunkAddr, XOR_KERNEL_DEBUG,
					xbe->m_XbeHeader.dwKernelThunkAddr ^ XOR_KERNEL_DEBUG );
			}
			else
			{
				// Decode actual entry point
				xbe->m_dwActualEntryPoint = xbe->m_XbeHeader.dwEntryPoint ^ XOR_ENTRY_RETAIL;
				DbgPrintf( "Entry point: 0x%08X ^ 0x%08X = 0x%08X\n", xbe->m_XbeHeader.dwEntryPoint, XOR_ENTRY_RETAIL,
					xbe->m_dwActualEntryPoint );

				// Decode kernel thunk location
				DbgPrintf( "Kernel Thunk Address: 0x%08X ^ 0x%08X = 0x%08X\n", xbe->m_XbeHeader.dwKernelThunkAddr, XOR_KERNEL_RETAIL,
					xbe->m_XbeHeader.dwKernelThunkAddr ^ XOR_KERNEL_RETAIL );
			}
		}
		

		DbgPrintf( "\n**********Xbe section(s) info**********\n" );
		DbgPrintf( "Section Address: 0x%08X\n", xbe->m_XbeHeader.dwSectionAddr );
		DbgPrintf( "Number of Sections: %d\n", xbe->m_XbeHeader.dwNumSections );

		for( DWORD i = 0; i < xbe->m_XbeHeader.dwNumSections; i++ )
		{
			offset = xbe->m_XbeSectionHeader[i].dwSectionNameAddr - xbe->m_XbeHeader.dwBaseAddr;
			DbgPrintf( "Section Name (#%d): %s\n", i, xbe->m_pXbeData + offset );
		}
//	}

	char string[128];
	sprintf( string, "Xenoborg: %S", xbe->m_XbeCertificate.wcTitle );

	SetWindowTextA( GetActiveWindow(), string );

//	if( !XBEWriteToMemory() ){};

	return TRUE;
}


//-----------------------------------------------------------------------------
// Name: XBEWriteToMemory
// Desc: Loads the XBE file into Ram starting at the XBE's base address.
//-----------------------------------------------------------------------------
//BOOL XBEWriteToMemory()
//{
//	// Save a copy of the actual base address
//	DWORD dwBaseAddress = xbe->m_XbeHeader.dwBaseAddr;
//
//	DbgPrintf( "Writing Xbe data to the reserved address...\n" );
//
//	if( !xbe->m_pXbeData )
//	{
//		EmuError( "NULL Xbe data!\n" );
//		return FALSE;
//	}
//
//	// Now that we have a loaded .xbe file, allow the 64mb region of memory to be
//	// both read and written to.
//	// TODO: Prevent writing to the header?
//
//	void* pBaseAddress = (void*) 0x10000;
//	DWORD dwMemSize = (64*1024*1024)-0x10000;
//	DWORD dwOldProtect;
//
//	if( !VirtualProtect( pBaseAddress, dwMemSize, PAGE_READWRITE, &dwOldProtect ) )
//	{
//		DWORD dwError = GetLastError();
//		return EmuError( "Error setting protection settings to required memory range!\nError code: 0x%.08X" );
//	}
//
//	// Zero out the entire 64mb region
//	ZeroMemory( ((DWORD*)pBaseAddress)+0x1000, dwMemSize-0x1000 );
//
//	try
//	{
//		// Write the XBE's bytes into memory
//		for( DWORD offset = 0x1000; offset < (unsigned) xbe->m_lXbeFileSize; offset++ )
//		{
//			((BYTE*)pBaseAddress)[offset/*+dwBaseAddress*/] = xbe->m_pXbeData[offset];
//		}
//	}
//	catch( ... )
//	{
//		EmuError( "Error loading Xbe file into memory!\n" );
//		return FALSE;
//	}
//
//	// Success
//	DbgPrintf( "Successfully written xbe contents to 0x10000.\n" );
//
//	return TRUE;
//}


