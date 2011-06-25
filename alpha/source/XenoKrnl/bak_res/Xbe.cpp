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



#include "XenoKrnl.h"
#include "Error.h"
#include "DbgConsole.h"
#include "Xbe.h"


Xbe_t Xbe;

//-----------------------------------------------------------------------------
// Name: OpenFile
// Desc: Opens an xbe file, determines some basic
//		 properties such as file size, and loads
//		 the data into system memory.
//-----------------------------------------------------------------------------
BOOL XBELoad( char* szXbeFileName )
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
//	memset( Xbe.m_szXbeFileName, 0x0, sizeof( *Xbe.m_szXbeFileName ) );

//	Xbe.m_szXbeFileName = (char*) malloc( sizeof( char ) * strlen( szXbeFileName ) ); // new char[ strlen( szXbeFileName ) ];
//	strcpy( Xbe.m_szXbeFileName, szXbeFileName );
	Xbe.m_szXbeFileName = szXbeFileName;

	// Retreive xbe's size
	fseek( fpXbe, 0, SEEK_END );
	Xbe.m_lXbeFileSize = ftell( fpXbe );

	// Allocate memory for xbe data
	Xbe.m_pXbeData = /*(BYTE*) malloc( sizeof( BYTE ) * Xbe.m_lXbeFileSize );*/
		new unsigned char[ Xbe.m_lXbeFileSize ];

	// Close xbe file
	fclose( fpXbe );

	// Process Xbe's data
	if( !XBEProcessData() )
		return FALSE;

	// Mark xbe file as opened successfully.
	Xbe.m_bXbeHasBeenOpened = 1;

	return TRUE;
}

//-----------------------------------------------------------------------------
// Name: CloseFile
// Desc: ...
//-----------------------------------------------------------------------------
void XBEUnload()
{
	DbgPrintf( "Closing Xbe file... \n" );

	// Release data pointers and reset variables
	{
		if( Xbe.m_pXbeData )
		{
			/*free( Xbe.m_pXbeData );*/ delete[] Xbe.m_pXbeData;
			Xbe.m_pXbeData = NULL;
		}
/*
		if( Xbe.m_szXbeFileName )
		{
			free( Xbe.m_szXbeFileName );	// delete[] Xbe.m_szXbeFileName;
			Xbe.m_szXbeFileName = NULL;
		}
*/
		if( Xbe.m_XbeSectionHeader )
		{
			free( Xbe.m_XbeSectionHeader );
			Xbe.m_XbeSectionHeader = NULL;
		}

		if( Xbe.m_XbeStandardLibrary )
		{
			free( Xbe.m_XbeStandardLibrary );
			Xbe.m_XbeStandardLibrary = NULL;
		}
	}

	SetWindowTextA( GetActiveWindow(), "Xenoborg" );

	Xbe.m_bXbeHasBeenOpened = 0;
}

//-----------------------------------------------------------------------------
// Name: ProcessXbeData
// Desc: Processes and organizes xbe's data into 
//		 sections, checks to see if the xbe file
//		 is legit.  Xbe has to have been previously
//		 loaded in order to use Xbe function.
//-----------------------------------------------------------------------------
BOOL XBEProcessData()
{
	FILE* fpXbe = NULL;		// File pointer for xbe file
	UINT* ptr = NULL;		// Temporary pointer to xbe data
//	UINT i;

	// Discontinue if the xbe file's existance has not been verified
//	if( !Xbe.m_bXbeHasBeenOpened )
//		return FALSE;

	DbgPrintf( "Loading Xbe file...\n\n" );

	// Reopen the xbe file
	if( !( fpXbe = fopen( Xbe.m_szXbeFileName, "r" ) ) )
	{ 
		DbgPrintf( "XBEProcessData(): Unable to load Xbe file!\n" );
		return FALSE;
	}

	// Seek to the beginning of file
	fseek( fpXbe, 0, SEEK_SET );

	// Read xbe data into memory pointer
	fread( Xbe.m_pXbeData, Xbe.m_lXbeFileSize, 1, fpXbe );

	// Close the file handle
	fclose( fpXbe );

	// Create a copy of the xbe data to prevent alteration of the
	// actual data itself.
	ptr = (UINT*) Xbe.m_pXbeData;

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
			memcpy( &Xbe.m_XbeHeader, Xbe.m_pXbeData, sizeof( XbeImageHeader ) );
		}

		// Xbe Certificate
		{
			// Set the offset for the Xbe certificate
			offset = Xbe.m_XbeHeader.dwCertificateAddr - Xbe.m_XbeHeader.dwBaseAddr;
			// Read in the Xbe certificate data
			memcpy( &Xbe.m_XbeCertificate, Xbe.m_pXbeData + offset, sizeof( XbeCertificate ) );
		}

		// Xbe Section Header(s)
		{
			// Allocate memory for the Xbe's section headers
			Xbe.m_XbeSectionHeader = (XbeSectionHeader*) malloc( sizeof( XbeSectionHeader ) * Xbe.m_XbeHeader.dwNumSections );
			// = new XbeSectionHeader[Xbe.m_XbeHeader.dwNumSections];

			// Set the offset for the first xbe section header
			offset = Xbe.m_XbeHeader.dwSectionAddr - Xbe.m_XbeHeader.dwBaseAddr;
		
			// Retreive data for each and every section header in the Xbe
			for( DWORD i = 0; i < Xbe.m_XbeHeader.dwNumSections; i++ )
			{
				memcpy( &Xbe.m_XbeSectionHeader[i], Xbe.m_pXbeData + offset, sizeof( XbeSectionHeader ) );
				offset += sizeof( XbeSectionHeader );
			}
		}

		// Xbe TLS
		{
			if (Xbe.m_XbeHeader.dwTLSAddr>Xbe.m_XbeHeader.dwBaseAddr)
			{
				try
				{
					// Set the offset for the Xbe's TLS data
					offset = Xbe.m_XbeHeader.dwTLSAddr - Xbe.m_XbeHeader.dwBaseAddr;
					// Read TLS data
					memcpy( &Xbe.m_XbeTLS, Xbe.m_pXbeData + offset, sizeof( XbeTLS ) );
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
			offset = Xbe.m_XbeHeader.dwLibVersionAddr - Xbe.m_XbeHeader.dwBaseAddr;

			// Allocate memory for xbe's standard libraries
			Xbe.m_XbeStandardLibrary = (XbeLibrary*) malloc( sizeof( XbeLibrary ) * Xbe.m_XbeHeader.dwNumLibVersions );
			// = new XbeLibrary[Xbe.m_XbeHeader.dwNumLibVersions];

			// Retreive data for each and every library
			for( DWORD i = 0; i < Xbe.m_XbeHeader.dwNumLibVersions; i++ )
			{
				memcpy( &Xbe.m_XbeStandardLibrary[i], Xbe.m_pXbeData + offset, sizeof( XbeLibrary ) );
				offset += sizeof( XbeLibrary );
			}

			// Set the offset for the Xbe's kernel library
			offset = Xbe.m_XbeHeader.dwKernelLibVersionAddr - Xbe.m_XbeHeader.dwBaseAddr;

			// Read kernel library data
			if (Xbe.m_XbeHeader.dwKernelLibVersionAddr)
			{
				try
				{
					memcpy( &Xbe.m_XbeKernelLibrary, Xbe.m_pXbeData + offset, sizeof( XbeLibrary ) );
				}
				catch( ... )
				{
					DbgPrintf( "This Xbe contains no Kernel Library.\n" );
				}
			}

			// Set the offset for the Xbe's Xapi library
			offset = Xbe.m_XbeHeader.dwXapiLibVersionAddr - Xbe.m_XbeHeader.dwBaseAddr;

			// Read Xapi library data
			if (Xbe.m_XbeHeader.dwXapiLibVersionAddr && offset > 0 )
			{
				try
				{
					memcpy( &Xbe.m_XbeXapiLibrary, Xbe.m_pXbeData + offset, sizeof( XbeLibrary ) );
				}
				catch( ... )
				{
					DbgPrintf( "This Xbe contains no Xapi Library.\n" );
				}
			}
		}

		// Output information to debug file
		DbgPrintf( "\n**********Xbe general**********\n" );
		DbgPrintf( "Xbe file name: %s\nFile size: %d bytes\n", Xbe.m_szXbeFileName, Xbe.m_lXbeFileSize );
		DbgPrintf( "Base Address: 0x%08X\n", Xbe.m_XbeHeader.dwBaseAddr );

		// Check for a Sega Chihiro .xbe
		if( ( Xbe.m_XbeHeader.dwEntryPoint & 0xF0000000 ) == 0x40000000 )
		{
			DbgPrintf( "Xbe type: Chihiro\n" );

			// This is a Sega Chihiro .xbe
			Xbe.m_bIsChihiro = TRUE;

			// Decode actual entry point
			Xbe.m_dwActualEntryPoint = Xbe.m_XbeHeader.dwEntryPoint ^ XOR_ENTRY_CHIHIRO;
			DbgPrintf( "Entry point: 0x%08X ^ 0x%08X = 0x%08X\n", Xbe.m_XbeHeader.dwEntryPoint, XOR_ENTRY_CHIHIRO,
				Xbe.m_dwActualEntryPoint );

			// Decode kernel thunk location
			DbgPrintf( "Kernel Thunk Address: 0x%08X ^ 0x%08X = 0x%08X\n", Xbe.m_XbeHeader.dwKernelThunkAddr, XOR_KERNEL_CHIHIRO,
				Xbe.m_XbeHeader.dwKernelThunkAddr ^ XOR_KERNEL_CHIHIRO );
		}
		else
		{
			DbgPrintf( "Xbe type: Xbox\n" );

			// This is a Microsoft Xbox .xbe
			Xbe.m_bIsChihiro = FALSE;

			if( ( Xbe.m_XbeHeader.dwEntryPoint ^ XOR_ENTRY_RETAIL ) > 0x01000000 )
			{
				// Decode actual entry point
				Xbe.m_dwActualEntryPoint = Xbe.m_XbeHeader.dwEntryPoint ^ XOR_ENTRY_DEBUG;
				DbgPrintf( "Entry point: 0x%08X ^ 0x%08X = 0x%08X\n", Xbe.m_XbeHeader.dwEntryPoint, XOR_ENTRY_DEBUG,
					Xbe.m_dwActualEntryPoint );

				// Decode kernel thunk location
				DbgPrintf( "Kernel Thunk Address: 0x%08X ^ 0x%08X = 0x%08X\n", Xbe.m_XbeHeader.dwKernelThunkAddr, XOR_KERNEL_DEBUG,
					Xbe.m_XbeHeader.dwKernelThunkAddr ^ XOR_KERNEL_DEBUG );
			}
			else
			{
				// Decode actual entry point
				Xbe.m_dwActualEntryPoint = Xbe.m_XbeHeader.dwEntryPoint ^ XOR_ENTRY_RETAIL;
				DbgPrintf( "Entry point: 0x%08X ^ 0x%08X = 0x%08X\n", Xbe.m_XbeHeader.dwEntryPoint, XOR_ENTRY_RETAIL,
					Xbe.m_dwActualEntryPoint );

				// Decode kernel thunk location
				DbgPrintf( "Kernel Thunk Address: 0x%08X ^ 0x%08X = 0x%08X\n", Xbe.m_XbeHeader.dwKernelThunkAddr, XOR_KERNEL_RETAIL,
					Xbe.m_XbeHeader.dwKernelThunkAddr ^ XOR_KERNEL_RETAIL );
			}
		}
		

		DbgPrintf( "\n**********Xbe section(s) info**********\n" );
		DbgPrintf( "Section Address: 0x%08X\n", Xbe.m_XbeHeader.dwSectionAddr );
		DbgPrintf( "Number of Sections: %d\n", Xbe.m_XbeHeader.dwNumSections );

		for( DWORD i = 0; i < Xbe.m_XbeHeader.dwNumSections; i++ )
		{
			offset = Xbe.m_XbeSectionHeader[i].dwSectionNameAddr - Xbe.m_XbeHeader.dwBaseAddr;
			DbgPrintf( "Section Name (#%d): %s\n", i, Xbe.m_pXbeData + offset );
		}
//	}

	char string[128];
	sprintf( string, "Xenoborg: %S", Xbe.m_XbeCertificate.wcTitle );

	SetWindowTextA( GetActiveWindow(), string );

	if( !XBEWriteToMemory() ){};

	return TRUE;
}


//-----------------------------------------------------------------------------
// Name: XBEWriteToMemory
// Desc: Loads the XBE file into Ram starting at the XBE's base address.
//-----------------------------------------------------------------------------
BOOL XBEWriteToMemory()
{
	// Save a copy of the actual base address
	DWORD dwBaseAddress = Xbe.m_XbeHeader.dwBaseAddr;

	DbgPrintf( "Writing Xbe data to the reserved address...\n" );

	if( !Xbe.m_pXbeData )
	{
		EmuError( "NULL Xbe data!\n" );
		return FALSE;
	}

	// Now that we have a loaded .xbe file, allow the 64mb region of memory to be
	// both read and written to.
	// TODO: Prevent writing to the header?

	void* pBaseAddress = (void*) 0x10000;
	DWORD dwMemSize = (64*1024*1024)-0x10000;
	DWORD dwOldProtect;

	if( !VirtualProtect( pBaseAddress, dwMemSize, PAGE_READWRITE, &dwOldProtect ) )
		return EmuError( "Error setting protection settings to required memory range!" );

	// Zero out the entire 64mb region
	ZeroMemory( ((DWORD*)pBaseAddress)+0x1000, dwMemSize-0x1000 );

	try
	{
		// Write the XBE's bytes into memory
		for( DWORD offset = 0x1000; offset < (unsigned) Xbe.m_lXbeFileSize; offset++ )
		{
			((BYTE*)pBaseAddress)[offset+dwBaseAddress] = Xbe.m_pXbeData[offset];
		}
	}
	catch( ... )
	{
		EmuError( "Error loading Xbe file into memory!\n" );
		return FALSE;
	}

	return TRUE;
}


