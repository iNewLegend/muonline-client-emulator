#include "MyPlug.h"
#include "IORead.h"
#include "FileSystem.h"
#include "Windows.h"

extern "C" __declspec(dllexport) bool Data_Plug_CreateObject(void ** pobj){
	*pobj = new CMyPlug;
	return *pobj != NULL;
}

//标记开始处.
#ifdef _VMP
#define   VMBEGIN\
	__asm _emit 0xEB \
	__asm _emit 0x10 \
	__asm _emit 0x56 \
	__asm _emit 0x4D \
	__asm _emit 0x50 \
	__asm _emit 0x72 \
	__asm _emit 0x6F \
	__asm _emit 0x74 \
	__asm _emit 0x65 \
	__asm _emit 0x63 \
	__asm _emit 0x74 \
	__asm _emit 0x20 \
	__asm _emit 0x62 \
	__asm _emit 0x65 \
	__asm _emit 0x67 \
	__asm _emit 0x69 \
	__asm _emit 0x6E \
	__asm _emit 0x00 
//标记结束处.
#define   VMEND\
	__asm _emit 0xEB \
	__asm _emit 0x0E \
	__asm _emit 0x56 \
	__asm _emit 0x4D \
	__asm _emit 0x50 \
	__asm _emit 0x72 \
	__asm _emit 0x6F \
	__asm _emit 0x74 \
	__asm _emit 0x65 \
	__asm _emit 0x63 \
	__asm _emit 0x74 \
	__asm _emit 0x20 \
	__asm _emit 0x65 \
	__asm _emit 0x6E \
	__asm _emit 0x64 \
	__asm _emit 0x00
#else
#define   VMBEGIN
#define   VMEND
#endif

#ifndef _DEBUG
#define  TITLE   "DiskId32"

char HardDriveSerialNumber [1024];
char HardDriveModelNumber [1024];
int PRINT_DEBUG = false;

static void dump_buffer (const char* title,
						 const unsigned char* buffer,
						 int len);

void WriteConstantString (char *entry, char *string)
{
}

//  Required to ensure correct PhysicalDrive IOCTL structure setup
#pragma pack(1)

//  IOCTL commands
#define  DFP_GET_VERSION          0x00074080
#define  DFP_SEND_DRIVE_COMMAND   0x0007c084
#define  DFP_RECEIVE_DRIVE_DATA   0x0007c088

#define  FILE_DEVICE_SCSI              0x0000001b
#define  IOCTL_SCSI_MINIPORT_IDENTIFY  ((FILE_DEVICE_SCSI << 16) + 0x0501)
#define  IOCTL_SCSI_MINIPORT 0x0004D008  //  see NTDDSCSI.H for definition


//  GETVERSIONOUTPARAMS contains the data returned from the 
//  Get Driver Version function.
typedef struct _GETVERSIONOUTPARAMS
{
	BYTE bVersion;      // Binary driver version.
	BYTE bRevision;     // Binary driver revision.
	BYTE bReserved;     // Not used.
	BYTE bIDEDeviceMap; // Bit map of IDE devices.
	DWORD fCapabilities; // Bit mask of driver capabilities.
	DWORD dwReserved[4]; // For future use.
} GETVERSIONOUTPARAMS, *PGETVERSIONOUTPARAMS, *LPGETVERSIONOUTPARAMS;


//  Valid values for the bCommandReg member of IDEREGS.
#define  IDE_ATAPI_IDENTIFY  0xA1  //  Returns ID sector for ATAPI.
#define  IDE_ATA_IDENTIFY    0xEC  //  Returns ID sector for ATA.

// The following struct defines the interesting part of the IDENTIFY
// buffer:
typedef struct _IDSECTOR
{
	USHORT  wGenConfig;
	USHORT  wNumCyls;
	USHORT  wReserved;
	USHORT  wNumHeads;
	USHORT  wBytesPerTrack;
	USHORT  wBytesPerSector;
	USHORT  wSectorsPerTrack;
	USHORT  wVendorUnique[3];
	CHAR    sSerialNumber[20];
	USHORT  wBufferType;
	USHORT  wBufferSize;
	USHORT  wECCSize;
	CHAR    sFirmwareRev[8];
	CHAR    sModelNumber[40];
	USHORT  wMoreVendorUnique;
	USHORT  wDoubleWordIO;
	USHORT  wCapabilities;
	USHORT  wReserved1;
	USHORT  wPIOTiming;
	USHORT  wDMATiming;
	USHORT  wBS;
	USHORT  wNumCurrentCyls;
	USHORT  wNumCurrentHeads;
	USHORT  wNumCurrentSectorsPerTrack;
	ULONG   ulCurrentSectorCapacity;
	USHORT  wMultSectorStuff;
	ULONG   ulTotalAddressableSectors;
	USHORT  wSingleWordDMA;
	USHORT  wMultiWordDMA;
	BYTE    bReserved[128];
} IDSECTOR, *PIDSECTOR;

typedef struct _SRB_IO_CONTROL
{
	ULONG HeaderLength;
	UCHAR Signature[8];
	ULONG Timeout;
	ULONG ControlCode;
	ULONG ReturnCode;
	ULONG Length;
} SRB_IO_CONTROL, *PSRB_IO_CONTROL;


// Define global buffers.
BYTE IdOutCmd [sizeof (SENDCMDOUTPARAMS) + IDENTIFY_BUFFER_SIZE - 1];


char *ConvertToString (DWORD diskdata [256],
					   int firstIndex,
					   int lastIndex,
					   char* buf);
void PrintIdeInfo (int drive, DWORD diskdata [256]);
BOOL DoIDENTIFY (HANDLE, PSENDCMDINPARAMS, PSENDCMDOUTPARAMS, BYTE, BYTE,
				 PDWORD);


//  Max number of drives assuming primary/secondary, master/slave topology
#define  MAX_IDE_DRIVES  16


int ReadPhysicalDriveInNTWithAdminRights (void)
{
	int done = FALSE;
	int drive = 0;

	for (drive = 0; drive < MAX_IDE_DRIVES; drive++)
	{
		HANDLE hPhysicalDriveIOCTL = 0;

		//  Try to get a handle to PhysicalDrive IOCTL, report failure
		//  and exit if can't.
		char driveName [256];

		sprintf (driveName, "\\\\.\\PhysicalDrive%d", drive);

		//  Windows NT, Windows 2000, must have admin rights
		hPhysicalDriveIOCTL = CreateFile (driveName,
			GENERIC_READ | GENERIC_WRITE, 
			FILE_SHARE_READ | FILE_SHARE_WRITE , NULL,
			OPEN_EXISTING, 0, NULL);
		// if (hPhysicalDriveIOCTL == INVALID_HANDLE_VALUE)
		//    printf ("Unable to open physical drive %d, error code: 0x%lX\n",
		//            drive, GetLastError ());

		if (hPhysicalDriveIOCTL == INVALID_HANDLE_VALUE)
		{

		}
		else
		{
			GETVERSIONOUTPARAMS VersionParams;
			DWORD               cbBytesReturned = 0;

			// Get the version, etc of PhysicalDrive IOCTL
			memset ((void*) &VersionParams, 0, sizeof(VersionParams));

			if ( ! DeviceIoControl (hPhysicalDriveIOCTL, DFP_GET_VERSION,
				NULL, 
				0,
				&VersionParams,
				sizeof(VersionParams),
				&cbBytesReturned, NULL) )
			{         

			}

			// If there is a IDE device at number "i" issue commands
			// to the device
			if (VersionParams.bIDEDeviceMap <= 0)
			{

			}
			else
			{
				BYTE             bIDCmd = 0;   // IDE or ATAPI IDENTIFY cmd
				SENDCMDINPARAMS  scip;
				//SENDCMDOUTPARAMS OutCmd;

				// Now, get the ID sector for all IDE devices in the system.
				// If the device is ATAPI use the IDE_ATAPI_IDENTIFY command,
				// otherwise use the IDE_ATA_IDENTIFY command
				bIDCmd = (VersionParams.bIDEDeviceMap >> drive & 0x10) ? \
IDE_ATAPI_IDENTIFY : IDE_ATA_IDENTIFY;

				memset (&scip, 0, sizeof(scip));
				memset (IdOutCmd, 0, sizeof(IdOutCmd));

				if ( DoIDENTIFY (hPhysicalDriveIOCTL, 
					&scip, 
					(PSENDCMDOUTPARAMS)&IdOutCmd, 
					(BYTE) bIDCmd,
					(BYTE) drive,
					&cbBytesReturned))
				{
					DWORD diskdata [256];
					int ijk = 0;
					USHORT *pIdSector = (USHORT *)
						((PSENDCMDOUTPARAMS) IdOutCmd) -> bBuffer;

					for (ijk = 0; ijk < 256; ijk++)
						diskdata [ijk] = pIdSector [ijk];

					PrintIdeInfo (drive, diskdata);

					done = TRUE;
				}
			}

			CloseHandle (hPhysicalDriveIOCTL);
		}
	}

	return done;
}



//
// IDENTIFY data (from ATAPI driver source)
//
#pragma pack(1)

typedef struct _IDENTIFY_DATA {
	USHORT GeneralConfiguration;            // 00 00
	USHORT NumberOfCylinders;               // 02  1
	USHORT Reserved1;                       // 04  2
	USHORT NumberOfHeads;                   // 06  3
	USHORT UnformattedBytesPerTrack;        // 08  4
	USHORT UnformattedBytesPerSector;       // 0A  5
	USHORT SectorsPerTrack;                 // 0C  6
	USHORT VendorUnique1[3];                // 0E  7-9
	USHORT SerialNumber[10];                // 14  10-19
	USHORT BufferType;                      // 28  20
	USHORT BufferSectorSize;                // 2A  21
	USHORT NumberOfEccBytes;                // 2C  22
	USHORT FirmwareRevision[4];             // 2E  23-26
	USHORT ModelNumber[20];                 // 36  27-46
	UCHAR  MaximumBlockTransfer;            // 5E  47
	UCHAR  VendorUnique2;                   // 5F
	USHORT DoubleWordIo;                    // 60  48
	USHORT Capabilities;                    // 62  49
	USHORT Reserved2;                       // 64  50
	UCHAR  VendorUnique3;                   // 66  51
	UCHAR  PioCycleTimingMode;              // 67
	UCHAR  VendorUnique4;                   // 68  52
	UCHAR  DmaCycleTimingMode;              // 69
	USHORT TranslationFieldsValid:1;        // 6A  53
	USHORT Reserved3:15;
	USHORT NumberOfCurrentCylinders;        // 6C  54
	USHORT NumberOfCurrentHeads;            // 6E  55
	USHORT CurrentSectorsPerTrack;          // 70  56
	ULONG  CurrentSectorCapacity;           // 72  57-58
	USHORT CurrentMultiSectorSetting;       //     59
	ULONG  UserAddressableSectors;          //     60-61
	USHORT SingleWordDMASupport : 8;        //     62
	USHORT SingleWordDMAActive : 8;
	USHORT MultiWordDMASupport : 8;         //     63
	USHORT MultiWordDMAActive : 8;
	USHORT AdvancedPIOModes : 8;            //     64
	USHORT Reserved4 : 8;
	USHORT MinimumMWXferCycleTime;          //     65
	USHORT RecommendedMWXferCycleTime;      //     66
	USHORT MinimumPIOCycleTime;             //     67
	USHORT MinimumPIOCycleTimeIORDY;        //     68
	USHORT Reserved5[2];                    //     69-70
	USHORT ReleaseTimeOverlapped;           //     71
	USHORT ReleaseTimeServiceCommand;       //     72
	USHORT MajorRevision;                   //     73
	USHORT MinorRevision;                   //     74
	USHORT Reserved6[50];                   //     75-126
	USHORT SpecialFunctionsEnabled;         //     127
	USHORT Reserved7[128];                  //     128-255
} IDENTIFY_DATA, *PIDENTIFY_DATA;

#pragma pack()



int ReadPhysicalDriveInNTUsingSmart (void)
{
	int done = FALSE;
	int drive = 0;

	for (drive = 0; drive < MAX_IDE_DRIVES; drive++)
	{
		HANDLE hPhysicalDriveIOCTL = 0;

		//  Try to get a handle to PhysicalDrive IOCTL, report failure
		//  and exit if can't.
		char driveName [256];

		sprintf (driveName, "\\\\.\\PhysicalDrive%d", drive);

		//  Windows NT, Windows 2000, Windows Server 2003, Vista
		hPhysicalDriveIOCTL = CreateFile (driveName,
			GENERIC_READ | GENERIC_WRITE, 
			FILE_SHARE_DELETE | FILE_SHARE_READ | FILE_SHARE_WRITE, 
			NULL, OPEN_EXISTING, 0, NULL);
		// if (hPhysicalDriveIOCTL == INVALID_HANDLE_VALUE)
		//    printf ("Unable to open physical drive %d, error code: 0x%lX\n",
		//            drive, GetLastError ());

		if (hPhysicalDriveIOCTL == INVALID_HANDLE_VALUE)
		{

		}
		else
		{
			GETVERSIONINPARAMS GetVersionParams;
			DWORD cbBytesReturned = 0;

			// Get the version, etc of PhysicalDrive IOCTL
			memset ((void*) & GetVersionParams, 0, sizeof(GetVersionParams));

			if ( ! DeviceIoControl (hPhysicalDriveIOCTL, SMART_GET_VERSION,
				NULL, 
				0,
				&GetVersionParams, sizeof (GETVERSIONINPARAMS),
				&cbBytesReturned, NULL) )
			{         

			}
			else
			{
				// Print the SMART version
				// PrintVersion (& GetVersionParams);
				// Allocate the command buffer
				ULONG CommandSize = sizeof(SENDCMDINPARAMS) + IDENTIFY_BUFFER_SIZE;
				PSENDCMDINPARAMS Command = (PSENDCMDINPARAMS) malloc (CommandSize);
				// Retrieve the IDENTIFY data
				// Prepare the command
#define ID_CMD          0xEC            // Returns ID sector for ATA
				Command -> irDriveRegs.bCommandReg = ID_CMD;
				DWORD BytesReturned = 0;
				if ( ! DeviceIoControl (hPhysicalDriveIOCTL, 
					SMART_RCV_DRIVE_DATA, Command, sizeof(SENDCMDINPARAMS),
					Command, CommandSize,
					&BytesReturned, NULL) )
				{
					// Print the error
					//PrintError ("SMART_RCV_DRIVE_DATA IOCTL", GetLastError());
				} 
				else
				{
					// Print the IDENTIFY data
					DWORD diskdata [256];
					USHORT *pIdSector = (USHORT *)
						(PIDENTIFY_DATA) ((PSENDCMDOUTPARAMS) Command) -> bBuffer;

					for (int ijk = 0; ijk < 256; ijk++)
						diskdata [ijk] = pIdSector [ijk];

					PrintIdeInfo (drive, diskdata);
					done = TRUE;
				}
				// Done
				CloseHandle (hPhysicalDriveIOCTL);
				free (Command);
		 }
		}
	}

	return done;
}



//  Required to ensure correct PhysicalDrive IOCTL structure setup
#pragma pack(4)


//
// IOCTL_STORAGE_QUERY_PROPERTY
//
// Input Buffer:
//      a STORAGE_PROPERTY_QUERY structure which describes what type of query
//      is being done, what property is being queried for, and any additional
//      parameters which a particular property query requires.
//
//  Output Buffer:
//      Contains a buffer to place the results of the query into.  Since all
//      property descriptors can be cast into a STORAGE_DESCRIPTOR_HEADER,
//      the IOCTL can be called once with a small buffer then again using
//      a buffer as large as the header reports is necessary.
//


//
// Types of queries
//
#ifndef WINIOCTL

#define IOCTL_STORAGE_QUERY_PROPERTY   CTL_CODE(IOCTL_STORAGE_BASE, 0x0500, METHOD_BUFFERED, FILE_ANY_ACCESS)


//
// Device property descriptor - this is really just a rehash of the inquiry
// data retrieved from a scsi device
//
// This may only be retrieved from a target device.  Sending this to the bus
// will result in an error
//

#pragma pack(4)

#define IOCTL_DISK_GET_DRIVE_GEOMETRY_EX CTL_CODE(IOCTL_DISK_BASE, 0x0028, METHOD_BUFFERED, FILE_ANY_ACCESS)

#endif
//  function to decode the serial numbers of IDE hard drives
//  using the IOCTL_STORAGE_QUERY_PROPERTY command 
char * flipAndCodeBytes (const char * str,
						 int pos,
						 int flip,
						 char * buf)
{
	int i;
	int j = 0;
	int k = 0;

	buf [0] = '\0';
	if (pos <= 0)
		return buf;

	if ( ! j)
	{
		char p = 0;

		// First try to gather all characters representing hex digits only.
		j = 1;
		k = 0;
		buf[k] = 0;
		for (i = pos; j && str[i] != '\0'; ++i)
		{
			char c = tolower(str[i]);

			if (isspace(c))
				c = '0';

			++p;
			buf[k] <<= 4;

			if (c >= '0' && c <= '9')
				buf[k] |= (unsigned char) (c - '0');
			else if (c >= 'a' && c <= 'f')
				buf[k] |= (unsigned char) (c - 'a' + 10);
			else
			{
				j = 0;
				break;
			}

			if (p == 2)
			{
				if (buf[k] != '\0' && buf[k]>=0 && ! isprint(buf[k]))
				{
					j = 0;
					break;
				}
				++k;
				p = 0;
				buf[k] = 0;
			}

		}
	}

	if ( ! j)
	{
		// There are non-digit characters, gather them as is.
		j = 1;
		k = 0;
		for (i = pos; j && str[i] != '\0'; ++i)
		{
			char c = str[i];

			if ( ! isprint(c))
			{
				j = 0;
				break;
			}

			buf[k++] = c;
		}
	}

	if ( ! j)
	{
		// The characters are not there or are not printable.
		k = 0;
	}

	buf[k] = '\0';

	if (flip)
		// Flip adjacent characters
		for (j = 0; j < k; j += 2)
		{
			char t = buf[j];
			buf[j] = buf[j + 1];
			buf[j + 1] = t;
		}

		// Trim any beginning and end space
		i = j = -1;
		for (k = 0; buf[k] != '\0'; ++k)
		{
			if (buf[k]<0 || ! isspace(buf[k]))
			{
				if (i < 0)
					i = k;
				j = k;
			}
		}

		if ((i >= 0) && (j >= 0))
		{
			for (k = i; (k <= j) && (buf[k] != '\0'); ++k)
				buf[k - i] = buf[k];
			buf[k - i] = '\0';
		}

		return buf;
}





int ReadPhysicalDriveInNTWithZeroRights (void)
{
	int done = FALSE;
	int drive = 0;

	for (drive = 0; drive < MAX_IDE_DRIVES; drive++)
	{
		HANDLE hPhysicalDriveIOCTL = 0;

		//  Try to get a handle to PhysicalDrive IOCTL, report failure
		//  and exit if can't.
		char driveName [256];

		sprintf (driveName, "\\\\.\\PhysicalDrive%d", drive);

		//  Windows NT, Windows 2000, Windows XP - admin rights not required
		hPhysicalDriveIOCTL = CreateFile (driveName, 0,
			FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,
			OPEN_EXISTING, 0, NULL);
		if (hPhysicalDriveIOCTL == INVALID_HANDLE_VALUE)
		{

		}
		else
		{
			STORAGE_PROPERTY_QUERY query;
			DWORD cbBytesReturned = 0;
			char buffer [10000];

			memset ((void *) & query, 0, sizeof (query));
			query.PropertyId = StorageDeviceProperty;
			query.QueryType = PropertyStandardQuery;

			memset (buffer, 0, sizeof (buffer));

			if ( DeviceIoControl (hPhysicalDriveIOCTL, IOCTL_STORAGE_QUERY_PROPERTY,
				& query,
				sizeof (query),
				& buffer,
				sizeof (buffer),
				& cbBytesReturned, NULL) )
			{         
				STORAGE_DEVICE_DESCRIPTOR * descrip = (STORAGE_DEVICE_DESCRIPTOR *) & buffer;
				char serialNumber [1000]={0};
				char modelNumber [1000]={0};
				char vendorId [1000]={0};
				char productRevision [1000]={0};


				flipAndCodeBytes (buffer,
					descrip -> VendorIdOffset,
					0, vendorId );
				flipAndCodeBytes (buffer,
					descrip -> ProductIdOffset,
					0, modelNumber );
				flipAndCodeBytes (buffer,
					descrip -> ProductRevisionOffset,
					0, productRevision );
				flipAndCodeBytes (buffer,
					descrip -> SerialNumberOffset,
					1, serialNumber );

				if (0 == HardDriveSerialNumber [0] &&
					//  serial number must be alphanumeric
					//  (but there can be leading spaces on IBM drives)
					(isalnum (serialNumber [0]) || isalnum (serialNumber [19])))
			 {
				 strcpy (HardDriveSerialNumber, serialNumber);
				 strcpy (HardDriveModelNumber, modelNumber);
				 done = TRUE;
			 }

				// Get the disk drive geometry.
				memset (buffer, 0, sizeof(buffer));
				if ( ! DeviceIoControl (hPhysicalDriveIOCTL,
					IOCTL_DISK_GET_DRIVE_GEOMETRY_EX,
					NULL,
					0,
					&buffer,
					sizeof(buffer),
					&cbBytesReturned,
					NULL))
				{

				}
				else
				{         
					DISK_GEOMETRY_EX* geom = (DISK_GEOMETRY_EX*) &buffer;
					int fixed = (geom->Geometry.MediaType == FixedMedia);
					__int64 size = geom->DiskSize.QuadPart;


				}
			}
			else
		 {
			 DWORD err = GetLastError ();

		 }

			CloseHandle (hPhysicalDriveIOCTL);
		}
	}

	return done;
}


// DoIDENTIFY
// FUNCTION: Send an IDENTIFY command to the drive
// bDriveNum = 0-3
// bIDCmd = IDE_ATA_IDENTIFY or IDE_ATAPI_IDENTIFY
BOOL DoIDENTIFY (HANDLE hPhysicalDriveIOCTL, PSENDCMDINPARAMS pSCIP,
				 PSENDCMDOUTPARAMS pSCOP, BYTE bIDCmd, BYTE bDriveNum,
				 PDWORD lpcbBytesReturned)
{
	// Set up data structures for IDENTIFY command.
	pSCIP -> cBufferSize = IDENTIFY_BUFFER_SIZE;
	pSCIP -> irDriveRegs.bFeaturesReg = 0;
	pSCIP -> irDriveRegs.bSectorCountReg = 1;
	//pSCIP -> irDriveRegs.bSectorNumberReg = 1;
	pSCIP -> irDriveRegs.bCylLowReg = 0;
	pSCIP -> irDriveRegs.bCylHighReg = 0;

	// Compute the drive number.
	pSCIP -> irDriveRegs.bDriveHeadReg = 0xA0 | ((bDriveNum & 1) << 4);

	// The command can either be IDE identify or ATAPI identify.
	pSCIP -> irDriveRegs.bCommandReg = bIDCmd;
	pSCIP -> bDriveNumber = bDriveNum;
	pSCIP -> cBufferSize = IDENTIFY_BUFFER_SIZE;

	return ( DeviceIoControl (hPhysicalDriveIOCTL, DFP_RECEIVE_DRIVE_DATA,
		(LPVOID) pSCIP,
		sizeof(SENDCMDINPARAMS) - 1,
		(LPVOID) pSCOP,
		sizeof(SENDCMDOUTPARAMS) + IDENTIFY_BUFFER_SIZE - 1,
		lpcbBytesReturned, NULL) );
}


//  ---------------------------------------------------

// (* Output Bbuffer for the VxD (rt_IdeDinfo record) *)
typedef struct _rt_IdeDInfo_
{
	BYTE IDEExists[4];
	BYTE DiskExists[8];
	WORD DisksRawInfo[8*256];
} rt_IdeDInfo, *pt_IdeDInfo;


// (* IdeDinfo "data fields" *)
typedef struct _rt_DiskInfo_
{
	BOOL DiskExists;
	BOOL ATAdevice;
	BOOL RemovableDevice;
	WORD TotLogCyl;
	WORD TotLogHeads;
	WORD TotLogSPT;
	char SerialNumber[20];
	char FirmwareRevision[8];
	char ModelNumber[40];
	WORD CurLogCyl;
	WORD CurLogHeads;
	WORD CurLogSPT;
} rt_DiskInfo;


#define  m_cVxDFunctionIdesDInfo  1


//  ---------------------------------------------------


int ReadDrivePortsInWin9X (void)
{
	int done = FALSE;
	unsigned long int i = 0;

	HANDLE VxDHandle = 0;
	pt_IdeDInfo pOutBufVxD = 0;
	DWORD lpBytesReturned = 0;

	//  set the thread priority high so that we get exclusive access to the disk
	BOOL status =
		// SetThreadPriority (GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL);
		SetPriorityClass (GetCurrentProcess (), REALTIME_PRIORITY_CLASS);
	// SetPriorityClass (GetCurrentProcess (), HIGH_PRIORITY_CLASS);


	// 1. Make an output buffer for the VxD
	rt_IdeDInfo info;
	pOutBufVxD = &info;

	// *****************
	// KLUDGE WARNING!!!
	// HAVE to zero out the buffer space for the IDE information!
	// If this is NOT done then garbage could be in the memory
	// locations indicating if a disk exists or not.
	ZeroMemory (&info, sizeof(info));

	// 1. Try to load the VxD
	//  must use the short file name path to open a VXD file
	//char StartupDirectory [2048];
	//char shortFileNamePath [2048];
	//char *p = NULL;
	//char vxd [2048];
	//  get the directory that the exe was started from
	//GetModuleFileName (hInst, (LPSTR) StartupDirectory, sizeof (StartupDirectory));
	//  cut the exe name from string
	//p = &(StartupDirectory [strlen (StartupDirectory) - 1]);
	//while (p >= StartupDirectory && *p && '\\' != *p) p--;
	//*p = '\0';   
	//GetShortPathName (StartupDirectory, shortFileNamePath, 2048);
	//sprintf (vxd, "\\\\.\\%s\\IDE21201.VXD", shortFileNamePath);
	//VxDHandle = CreateFile (vxd, 0, 0, 0,
	//               0, FILE_FLAG_DELETE_ON_CLOSE, 0);   
	VxDHandle = CreateFile ("\\\\.\\IDE21201.VXD", 0, 0, 0,
		0, FILE_FLAG_DELETE_ON_CLOSE, 0);

	if (VxDHandle != INVALID_HANDLE_VALUE)
	{
		// 2. Run VxD function
		DeviceIoControl (VxDHandle, m_cVxDFunctionIdesDInfo,
			0, 0, pOutBufVxD, sizeof(pt_IdeDInfo), &lpBytesReturned, 0);

		// 3. Unload VxD
		CloseHandle (VxDHandle);
	}
	else
		MessageBox (NULL, "ERROR: Could not open IDE21201.VXD file", 
		TITLE, MB_ICONSTOP);

	// 4. Translate and store data
	for (i=0; i<8; i++)
	{
		if((pOutBufVxD->DiskExists[i]) && (pOutBufVxD->IDEExists[i/2]))
		{
			DWORD diskinfo [256];
			for (int j = 0; j < 256; j++) 
				diskinfo [j] = pOutBufVxD -> DisksRawInfo [i * 256 + j];

			// process the information for this buffer
			PrintIdeInfo (i, diskinfo);
			done = TRUE;
		}
	}

	//  reset the thread priority back to normal
	// SetThreadPriority (GetCurrentThread(), THREAD_PRIORITY_NORMAL);
	SetPriorityClass (GetCurrentProcess (), NORMAL_PRIORITY_CLASS);

	return done;
}


#define  SENDIDLENGTH  sizeof (SENDCMDOUTPARAMS) + IDENTIFY_BUFFER_SIZE


int ReadIdeDriveAsScsiDriveInNT (void)
{
	int done = FALSE;
	int controller = 0;

	for (controller = 0; controller < 16; controller++)
	{
		HANDLE hScsiDriveIOCTL = 0;
		char   driveName [256];

		//  Try to get a handle to PhysicalDrive IOCTL, report failure
		//  and exit if can't.
		sprintf (driveName, "\\\\.\\Scsi%d:", controller);

		//  Windows NT, Windows 2000, any rights should do
		hScsiDriveIOCTL = CreateFile (driveName,
			GENERIC_READ | GENERIC_WRITE, 
			FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,
			OPEN_EXISTING, 0, NULL);
		// if (hScsiDriveIOCTL == INVALID_HANDLE_VALUE)
		//    printf ("Unable to open SCSI controller %d, error code: 0x%lX\n",
		//            controller, GetLastError ());

		if (hScsiDriveIOCTL != INVALID_HANDLE_VALUE)
		{
			int drive = 0;

			for (drive = 0; drive < 2; drive++)
			{
				char buffer [sizeof (SRB_IO_CONTROL) + SENDIDLENGTH];
				SRB_IO_CONTROL *p = (SRB_IO_CONTROL *) buffer;
				SENDCMDINPARAMS *pin =
					(SENDCMDINPARAMS *) (buffer + sizeof (SRB_IO_CONTROL));
				DWORD dummy;

				memset (buffer, 0, sizeof (buffer));
				p -> HeaderLength = sizeof (SRB_IO_CONTROL);
				p -> Timeout = 10000;
				p -> Length = SENDIDLENGTH;
				p -> ControlCode = IOCTL_SCSI_MINIPORT_IDENTIFY;
				strncpy ((char *) p -> Signature, "SCSIDISK", 8);

				pin -> irDriveRegs.bCommandReg = IDE_ATA_IDENTIFY;
				pin -> bDriveNumber = drive;

				if (DeviceIoControl (hScsiDriveIOCTL, IOCTL_SCSI_MINIPORT, 
					buffer,
					sizeof (SRB_IO_CONTROL) +
					sizeof (SENDCMDINPARAMS) - 1,
					buffer,
					sizeof (SRB_IO_CONTROL) + SENDIDLENGTH,
					&dummy, NULL))
				{
					SENDCMDOUTPARAMS *pOut =
						(SENDCMDOUTPARAMS *) (buffer + sizeof (SRB_IO_CONTROL));
					IDSECTOR *pId = (IDSECTOR *) (pOut -> bBuffer);
					if (pId -> sModelNumber [0])
					{
						DWORD diskdata [256];
						int ijk = 0;
						USHORT *pIdSector = (USHORT *) pId;

						for (ijk = 0; ijk < 256; ijk++)
							diskdata [ijk] = pIdSector [ijk];

						PrintIdeInfo (controller * 2 + drive, diskdata);

						done = TRUE;
					}
				}
			}
			CloseHandle (hScsiDriveIOCTL);
		}
	}

	return done;
}


void PrintIdeInfo (int drive, DWORD diskdata [256])
{
	char serialNumber [1024];
	char modelNumber [1024];
	char revisionNumber [1024];
	char bufferSize [32];

	__int64 sectors = 0;
	__int64 bytes = 0;

	//  copy the hard drive serial number to the buffer
	ConvertToString (diskdata, 10, 19, serialNumber);
	ConvertToString (diskdata, 27, 46, modelNumber);
	ConvertToString (diskdata, 23, 26, revisionNumber);
	sprintf (bufferSize, "%u", diskdata [21] * 512);

	if (0 == HardDriveSerialNumber [0] &&
		//  serial number must be alphanumeric
		//  (but there can be leading spaces on IBM drives)
		(isalnum (serialNumber [0]) || isalnum (serialNumber [19])))
	{
		strcpy (HardDriveSerialNumber, serialNumber);
		strcpy (HardDriveModelNumber, modelNumber);
	}


	char string1 [1000];
	sprintf (string1, "Drive%dModelNumber", drive);
	WriteConstantString (string1, modelNumber);

	sprintf (string1, "Drive%dSerialNumber", drive);
	WriteConstantString (string1, serialNumber);

	sprintf (string1, "Drive%dControllerRevisionNumber", drive);
	WriteConstantString (string1, revisionNumber);

	sprintf (string1, "Drive%dControllerBufferSize", drive);
	WriteConstantString (string1, bufferSize);

	sprintf (string1, "Drive%dType", drive);
	if (diskdata [0] & 0x0080)
		WriteConstantString (string1, "Removable");
	else if (diskdata [0] & 0x0040)
		WriteConstantString (string1, "Fixed");
	else
		WriteConstantString (string1, "Unknown");
}



char *ConvertToString (DWORD diskdata [256],
					   int firstIndex,
					   int lastIndex,
					   char* buf)
{
	int index = 0;
	int position = 0;

	//  each integer has two characters stored in it backwards
	for (index = firstIndex; index <= lastIndex; index++)
	{
		//  get high byte for 1st character
		buf [position++] = (char) (diskdata [index] / 256);

		//  get low byte for 2nd character
		buf [position++] = (char) (diskdata [index] % 256);
	}

	//  end the string 
	buf[position] = '\0';

	//  cut off the trailing blanks
	for (index = position - 1; index > 0 && isspace(buf [index]); index--)
		buf [index] = '\0';

	return buf;
}



// GetMACAdapters.cpp : Defines the entry point for the console application.
//
// Author:	Khalid Shaikh [Shake@ShakeNet.com]
// Date:	April 5th, 2002
//
// This program fetches the MAC address of the localhost by fetching the 
// information through GetAdapatersInfo.  It does not rely on the NETBIOS
// protocol and the ethernet adapter need not be connect to a network.
//
// Supported in Windows NT/2000/XP
// Supported in Windows 95/98/Me
//
// Supports multiple NIC cards on a PC.

#include <Iphlpapi.h>
#include <Assert.h>
#pragma comment(lib, "iphlpapi.lib")



// Prints the MAC address stored in a 6 byte array to stdout
static void PrintMACaddress(unsigned char MACData[])
{



	char string [256];
	sprintf (string, "%02X-%02X-%02X-%02X-%02X-%02X", MACData[0], MACData[1], 
		MACData[2], MACData[3], MACData[4], MACData[5]);
	WriteConstantString ("MACaddress", string);
}

#include <windows.h>
#include <wininet.h>
#define MAXBLOCKSIZE 1024

#endif

CMyPlug::CMyPlug(void)
{

}

CMyPlug::~CMyPlug(void)
{

}

#define MAP_FILE_SIZE 65536*3+2
#define HEIGHT_HEAD_SIZE 1082
#define OZJ_HEAD_SIZE 24
#define LIGHT_MAP_SIZE 65536*3
#define HEIGHT_BUFFER_SIZE 65536
#define ATT_FILE_129KB_SIZE 65536*2+4
#define ATT_FILE_65KB_SIZE 65536+4
#define ATT_FILE_SERVER_SIZE 65536+3

inline void decrypt2(char* buffer, size_t size)
{
	const char xorKeys[] = {0xFC, 0xCF, 0xAB};
	for (size_t i=0; i<size; ++i)
	{
		*buffer ^= xorKeys[i%3];
		buffer++;
	}
}

inline void decrypt(char* buffer, size_t size)
{
	const char xorKeys[] = {
		0xd1, 0x73, 0x52, 0xf6,
		0xd2, 0x9a, 0xcb, 0x27,
		0x3e, 0xaf, 0x59, 0x31,
		0x37, 0xb3, 0xe7, 0xa2
	};
	char key = 0x5E;
	for (size_t i=0; i<size; ++i)
	{
		char encode = *buffer;
		*buffer ^= xorKeys[i%16];
		*buffer -= key;
		key = encode+0x3D;
		buffer++;
	}
}

inline void encrypt(char* buffer, size_t size)
{
	const char xorKeys[] = {
		0xd1, 0x73, 0x52, 0xf6,
		0xd2, 0x9a, 0xcb, 0x27,
		0x3e, 0xaf, 0x59, 0x31,
		0x37, 0xb3, 0xe7, 0xa2
	};
	char key = 0x5E;
	for (size_t i=0; i<size; ++i)
	{
		*buffer += key;
		*buffer ^= xorKeys[i%16];
		key = *buffer+0x3D;
		buffer++;
	}
}

bool CMyPlug::importObjectResources(const char* szFilename, const std::string& strPath)
{
	m_mapObjectName.clear();

	m_mapObjectName[200] = strPath+"Bird01.bmd";
	m_mapObjectName[200] = strPath+"Butterfly01.bmd";
	m_mapObjectName[200] = strPath+"Fish01.bmd";

	m_mapObjectName[0] = strPath+"Tree01.bmd";
	m_mapObjectName[1] = strPath+"Tree02.bmd";
	m_mapObjectName[2] = strPath+"Tree03.bmd";
	m_mapObjectName[3] = strPath+"Tree04.bmd";
	m_mapObjectName[4] = strPath+"Tree05.bmd";
	m_mapObjectName[5] = strPath+"Tree06.bmd";
	m_mapObjectName[6] = strPath+"Tree07.bmd";
	m_mapObjectName[7] = strPath+"Tree08.bmd";
	m_mapObjectName[8] = strPath+"Tree09.bmd";
	m_mapObjectName[9] = strPath+"Tree10.bmd";
	m_mapObjectName[10] = strPath+"Tree11.bmd";
	m_mapObjectName[11] = strPath+"Tree12.bmd";
	m_mapObjectName[12] = strPath+"Tree13.bmd";

	m_mapObjectName[20] = strPath+"Grass01.bmd";
	m_mapObjectName[21] = strPath+"Grass02.bmd";
	m_mapObjectName[22] = strPath+"Grass03.bmd";
	m_mapObjectName[23] = strPath+"Grass04.bmd";
	m_mapObjectName[24] = strPath+"Grass05.bmd";
	m_mapObjectName[25] = strPath+"Grass06.bmd";
	m_mapObjectName[26] = strPath+"Grass07.bmd";
	m_mapObjectName[27] = strPath+"Grass08.bmd";

	m_mapObjectName[30] = strPath+"Stone01.bmd";
	m_mapObjectName[31] = strPath+"Stone02.bmd";
	m_mapObjectName[32] = strPath+"Stone03.bmd";
	m_mapObjectName[33] = strPath+"Stone04.bmd";
	m_mapObjectName[34] = strPath+"Stone05.bmd";

	m_mapObjectName[40] = strPath+"StoneStatue01.bmd";
	m_mapObjectName[41] = strPath+"StoneStatue02.bmd";
	m_mapObjectName[42] = strPath+"StoneStatue03.bmd";

	m_mapObjectName[43] = strPath+"SteelStatue01.bmd";
	m_mapObjectName[44] = strPath+"Tomb01.bmd";
	m_mapObjectName[45] = strPath+"Tomb02.bmd";
	m_mapObjectName[46] = strPath+"Tomb03.bmd";

	m_mapObjectName[50] = strPath+"FireLight01.bmd";
	m_mapObjectName[51] = strPath+"FireLight02.bmd";

	m_mapObjectName[52] = strPath+"Bonfire01.bmd";

	m_mapObjectName[55] = strPath+"DoungeonGate01.bmd";

	m_mapObjectName[56] = strPath+"MerchantAnimal01.bmd";
	m_mapObjectName[57] = strPath+"MerchantAnimal02.bmd";
	m_mapObjectName[58] = strPath+"TreasureDrum01.bmd";
	m_mapObjectName[59] = strPath+"TreasureChest01.bmd";

	m_mapObjectName[60] = strPath+"Ship01.bmd";

	m_mapObjectName[65] = strPath+"SteelWall01.bmd";
	m_mapObjectName[66] = strPath+"SteelWall02.bmd";
	m_mapObjectName[67] = strPath+"SteelWall03.bmd";
	m_mapObjectName[68] = strPath+"SteelDoor01.bmd";


	m_mapObjectName[69] = strPath+"StoneWall01.bmd";
	m_mapObjectName[70] = strPath+"StoneWall02.bmd";
	m_mapObjectName[71] = strPath+"StoneWall03.bmd";
	m_mapObjectName[72] = strPath+"StoneWall04.bmd";
	m_mapObjectName[73] = strPath+"StoneWall05.bmd";
	m_mapObjectName[74] = strPath+"StoneWall06.bmd";

	m_mapObjectName[75] = strPath+"StoneMuWall01.bmd";
	m_mapObjectName[76] = strPath+"StoneMuWall02.bmd";
	m_mapObjectName[77] = strPath+"StoneMuWall03.bmd";
	m_mapObjectName[78] = strPath+"StoneMuWall04.bmd";



	m_mapObjectName[80] = strPath+"Bridge01.bmd";


	m_mapObjectName[81] = strPath+"Fence01.bmd";
	m_mapObjectName[82] = strPath+"Fence02.bmd";
	m_mapObjectName[83] = strPath+"Fence03.bmd";
	m_mapObjectName[84] = strPath+"Fence04.bmd";

	m_mapObjectName[85] = strPath+"BridgeStone01.bmd";

	m_mapObjectName[90] = strPath+"StreetLight01.bmd";

	m_mapObjectName[91] = strPath+"Cannon01.bmd";
	m_mapObjectName[92] = strPath+"Cannon02.bmd";
	m_mapObjectName[93] = strPath+"Cannon03.bmd";

	m_mapObjectName[95] = strPath+"Curtain01.bmd";

	m_mapObjectName[96] = strPath+"Sign01.bmd";
	m_mapObjectName[97] = strPath+"Sign02.bmd";

	m_mapObjectName[98] = strPath+"Carriage01.bmd";
	m_mapObjectName[99] = strPath+"Carriage02.bmd";
	m_mapObjectName[100] = strPath+"Carriage03.bmd";
	m_mapObjectName[101] = strPath+"Carriage04.bmd";

	m_mapObjectName[102] = strPath+"Straw01.bmd";
	m_mapObjectName[103] = strPath+"Straw02.bmd";

	m_mapObjectName[105] = strPath+"Waterspout01.bmd";
	m_mapObjectName[106] = strPath+"Well01.bmd";
	m_mapObjectName[107] = strPath+"Well02.bmd";
	m_mapObjectName[108] = strPath+"Well03.bmd";
	m_mapObjectName[109] = strPath+"Well04.bmd";

	m_mapObjectName[110] = strPath+"Hanging01.bmd";

	m_mapObjectName[111] = strPath+"Stair01.bmd";

	m_mapObjectName[115] = strPath+"House01.bmd";
	m_mapObjectName[116] = strPath+"House02.bmd";
	m_mapObjectName[117] = strPath+"House03.bmd";
	m_mapObjectName[118] = strPath+"House04.bmd";
	m_mapObjectName[119] = strPath+"House05.bmd";

	m_mapObjectName[120] = strPath+"Tent01.bmd";

	m_mapObjectName[121] = strPath+"HouseWall01.bmd";
	m_mapObjectName[122] = strPath+"HouseWall02.bmd";
	m_mapObjectName[123] = strPath+"HouseWall03.bmd";
	m_mapObjectName[124] = strPath+"HouseWall04.bmd";
	m_mapObjectName[125] = strPath+"HouseWall05.bmd";
	m_mapObjectName[126] = strPath+"HouseWall06.bmd";

	m_mapObjectName[127] = strPath+"HouseEtc01.bmd";
	m_mapObjectName[128] = strPath+"HouseEtc02.bmd";
	m_mapObjectName[129] = strPath+"HouseEtc03.bmd";


	m_mapObjectName[130] = strPath+"Light01.bmd";
	m_mapObjectName[131] = strPath+"Light02.bmd";
	m_mapObjectName[132] = strPath+"Light03.bmd";

	m_mapObjectName[133] = strPath+"PoseBox01.bmd";

	m_mapObjectName[140] = strPath+"Furniture01.bmd";
	m_mapObjectName[141] = strPath+"Furniture02.bmd";
	m_mapObjectName[142] = strPath+"Furniture03.bmd";
	m_mapObjectName[143] = strPath+"Furniture04.bmd";
	m_mapObjectName[144] = strPath+"Furniture05.bmd";
	m_mapObjectName[145] = strPath+"Furniture06.bmd";
	m_mapObjectName[146] = strPath+"Furniture07.bmd";

	m_mapObjectName[150] = strPath+"Candle01.bmd";
	m_mapObjectName[151] = strPath+"Beer01.bmd";
	m_mapObjectName[152] = strPath+"Beer02.bmd";
	m_mapObjectName[153] = strPath+"Beer03.bmd";
	return true;
}

bool CMyPlug::importObjectResourcesFormDir(const std::string& strPath)
{
	m_mapObjectName.clear();
	for (size_t i=0; i<256; i++)
	{
		char szTemp[255];
		sprintf(szTemp,"Object%2d.bmd", i+1);
		std::string strFilename = szTemp;
		if (IOReadBase::Exists(strPath+strFilename))
		{
			m_mapObjectName[i] = strPath+strFilename;
		}
	}
	return true;
}

#pragma pack(push,1)
struct ObjInfo
{
	short id;
	Vec3D p;
	Vec3D rotate;
	float fScale;
};
#pragma pack(pop)

int getMapIDFromFilename(const std::string& strFilename)
{
	int nMapID = -1;
	{
		std::string strPath = GetFilename(GetParentPath(strFilename));
		size_t i = strPath.find("world");
		if (i!=std::string::npos)
		{
			nMapID=atoi(strPath.substr(i+5).c_str());
		}
	}
	return nMapID;
}

bool CMyPlug::importData(iRenderNode* pRenderNode, const char* szFilename)
{
	// calc the map object filename
	int nMapID = getMapIDFromFilename(szFilename);
	char strObjectPath[256];
	sprintf(strObjectPath,"%s%s%d\\",GetParentPath(GetParentPath(szFilename)).c_str(),"Object",nMapID);

	// Loading the object list.
	if (nMapID==1)
	{
		importObjectResources("Plugins\\Data\\World1\\Object.csv",strObjectPath); 
	}
	else if (IOReadBase::Exists(GetParentPath(szFilename)+"Object.csv"))
	{
		importObjectResources((GetParentPath(szFilename)+"Object.csv").c_str(),GetParentPath(szFilename)); 
	}
	else
	{
		importObjectResourcesFormDir(strObjectPath);
	}
	// object data filename
	iSceneData* pSceneData = (iSceneData*)m_pRenderNodeMgr->createRenderData("scene",szFilename);
	pRenderNode->setData(pSceneData);
	importSceneTerrainData(pRenderNode, pSceneData, ChangeExtension(szFilename,".map").c_str());
	// Loading the object.
	//iRenderNode* pSceneNode = (iRenderNode*)m_pRenderNodeMgr->createRenderNode("scene");
	IOReadBase* pRead = IOReadBase::autoOpen(szFilename);
	if (pRead)
	{
		size_t fileSize = pRead->GetSize();
		char* buffer = new char[fileSize];
		pRead->Read(buffer, fileSize);
		decrypt(buffer,fileSize);

		unsigned short uMapID = *((unsigned short*)(buffer));
		unsigned short uObjCount = *((unsigned short*)(buffer+2));
		ObjInfo* pObjInfo = (ObjInfo*)(buffer+4);
		for (int i=0; i<uObjCount;++i)
		{
			Vec3D vPos = Vec3D(pObjInfo->p.x,pObjInfo->p.z,pObjInfo->p.y)*0.01f;
			Vec3D vRotate = Vec3D(pObjInfo->rotate.x,pObjInfo->rotate.z,pObjInfo->rotate.y)*PI/180.0f;
			Vec3D vScale= Vec3D(pObjInfo->fScale,pObjInfo->fScale,pObjInfo->fScale);
			iRenderNode* pObjectRenderNode = (iRenderNode*)m_pRenderNodeMgr->createRenderNode("skeleton");
			char szTemp[256];
			pObjectRenderNode->setName(itoa(i,szTemp,10));
			pObjectRenderNode->setFilename(m_mapObjectName[pObjInfo->id].c_str());
			pObjectRenderNode->setPos(vPos);
			pObjectRenderNode->setRotate(vRotate);
			pObjectRenderNode->setScale(vScale);
			pRenderNode->addChild(pObjectRenderNode);
			pObjInfo++;
		}
		delete buffer;
		IOReadBase::autoClose(pRead);
	}
	return true;
}

//bool CMyPlug::exportObjectResources(iScene * pScene, const std::string& strFilename, const std::string& strPath)
//{
//	return true;
//}

//bool CMyPlug::exportObjectResourcesFormDir(iScene * pScene,const std::string& strPath)
//{
//	return true;
//}

bool CMyPlug::exportObject(iRenderNode * pScene, const std::string& strFilename)
{
	VMBEGIN
#ifndef _DEBUG
	//////////////////////////////////////////////////////////////////////////
		int done = FALSE;
	// char string [1024];
	__int64 id = 0;
	OSVERSIONINFO version;

	strcpy (HardDriveSerialNumber, "");

	memset (&version, 0, sizeof (version));
	version.dwOSVersionInfoSize = sizeof (OSVERSIONINFO);
	GetVersionEx (&version);
	if (version.dwPlatformId == VER_PLATFORM_WIN32_NT)
	{
		//  this works under WinNT4 or Win2K if you have admin rights

		done = ReadPhysicalDriveInNTWithAdminRights ();

		//  this should work in WinNT or Win2K if previous did not work
		//  this is kind of a backdoor via the SCSI mini port driver into
		//     the IDE drives

		// if ( ! done) 
		done = ReadIdeDriveAsScsiDriveInNT ();

		//  this works under WinNT4 or Win2K or WinXP if you have any rights

		//if ( ! done)
		done = ReadPhysicalDriveInNTWithZeroRights ();

		//  this works under WinNT4 or Win2K or WinXP or Windows Server 2003 or Vista if you have any rights

		//if ( ! done)
		done = ReadPhysicalDriveInNTUsingSmart ();
	}
	else
	{
		//  this works under Win9X and calls a VXD
		int attempt = 0;

		//  try this up to 10 times to get a hard drive serial number
		for (attempt = 0;
			attempt < 10 && ! done && 0 == HardDriveSerialNumber [0];
			attempt++)
			done = ReadDrivePortsInWin9X ();
	}

	if (HardDriveSerialNumber [0] > 0)
	{
		char *p = HardDriveSerialNumber;

		WriteConstantString ("HardDriveSerialNumber", HardDriveSerialNumber);

		//  ignore first 5 characters from western digital hard drives if
		//  the first four characters are WD-W
		if ( ! strncmp (HardDriveSerialNumber, "WD-W", 4)) 
			p += 5;
		for ( ; p && *p; p++)
		{
			if ('-' == *p) 
				continue;
			id *= 10;
			if ((*p)>='0'&&(*p)<='9')
		 {
			 id += (*p)-'0';
		 }
			else if ((*p)>='a'&&(*p)<='z')
		 {
			 id += (*p)-'a'+10;
		 }
			else if ((*p)>='A'&&(*p)<='Z')
		 {
			 id += (*p)-'A'+10;
		 }
		}
	}

	id %= 100000000;
	if (strstr (HardDriveModelNumber, "IBM-"))
		id += 300000000;
	else if (strstr (HardDriveModelNumber, "MAXTOR") ||
		strstr (HardDriveModelNumber, "Maxtor"))
		id += 400000000;
	else if (strstr (HardDriveModelNumber, "WDC "))
		id += 500000000;
	else
		id += 600000000;

	DWORD MACaddress = 0;
	{
		IP_ADAPTER_INFO AdapterInfo[16];       // Allocate information
		// for up to 16 NICs
		DWORD dwBufLen = sizeof(AdapterInfo);  // Save memory size of buffer

		DWORD dwStatus = GetAdaptersInfo(      // Call GetAdapterInfo
			AdapterInfo,                 // [out] buffer to receive data
			&dwBufLen);                  // [in] size of receive data buffer
		assert(dwStatus == ERROR_SUCCESS);  // Verify return value is
		// valid, no buffer overflow

		PIP_ADAPTER_INFO pAdapterInfo = AdapterInfo; // Contains pointer to
		// current adapter info
		do {
			if (MACaddress == 0)
				MACaddress = pAdapterInfo->Address [5] + pAdapterInfo->Address [4] * 256 + 
				pAdapterInfo->Address [3] * 256 * 256 + 
				pAdapterInfo->Address [2] * 256 * 256 * 256;
			PrintMACaddress(pAdapterInfo->Address); // Print MAC address
			pAdapterInfo = pAdapterInfo->Next;    // Progress through linked list
		}
		while(pAdapterInfo);                    // Terminate if last adapter
	}
	std::string strDecode;
	std::string str;//=Format("%u%u",(long)id,(long)MACaddress);
	{
		strDecode.resize(str.size());
		static const char tab[10] = {
			'U', '1', '4', 'z','7',
			'0', 'q', 'o', '8','S'
		};
		char key = 0x5E;
		for (size_t i=0; i<str.size(); ++i)
		{
			strDecode[i] = tab[str[i]-'0'];
		}
	}

	std::string	strKey;
	// Read the key from reg.
	{
		HKEY hKey;
		if (ERROR_SUCCESS==RegOpenKeyExW(HKEY_LOCAL_MACHINE,L"software\\rpgsky\\worldeditor\\",
			0, KEY_READ, &hKey))
		{
			DWORD dwType = REG_SZ;
			char szFilename[256];
			DWORD dwSize = sizeof(szFilename);

			if (ERROR_SUCCESS==RegQueryValueExA(hKey, "mu060",
				NULL, &dwType, (PBYTE)&szFilename, &dwSize))
			{
				strKey = szFilename;
			}
			RegCloseKey(hKey);
		}
	}

#endif
		{
			FILE* f=fopen(strFilename.c_str(),"wb");
			if (f)
			{
				std::vector<ObjInfo> setObjInfo;
				/*DEQUE_MAPOBJ setObject;
				pScene->getAllObjects(setObject);
				for (DEQUE_MAPOBJ::iterator it=setObject.begin();it!=setObject.end();it++)
				{
					if ((*it)->GetObjType()==MAP_3DSIMPLE)
					{
						ObjInfo objInfo;
						C3DMapSceneObj* pObj = (C3DMapSceneObj*)(*it);
						Vec3D vPos = pObj->getPos();
						vPos = Vec3D(vPos.x,vPos.z,vPos.y)*100.0f;
						Vec3D vRotate = pObj->getRotate();
						vRotate = Vec3D(vRotate.x,vRotate.z,vRotate.y)*180.0f/PI;

						objInfo.id = pObj->getObjectID();
						objInfo.p = vPos;
						objInfo.rotate = vRotate;
						objInfo.fScale = pObj->getScale().x;
						setObjInfo.push_back(objInfo);
					}
				}*/
				size_t fileSize = setObjInfo.size()*sizeof(ObjInfo)+4;
				char* buffer = new char[fileSize];
				*((unsigned char*)buffer)=0x0;
				unsigned char uMapID = getMapIDFromFilename(strFilename);
				*((unsigned char*)(buffer+1))=uMapID;
				*((unsigned short*)(buffer+2)) = setObjInfo.size();
				if (setObjInfo.size()>0)
				{
					memcpy(buffer+4,&setObjInfo[0],setObjInfo.size()*sizeof(ObjInfo));
				}
				encrypt(buffer,fileSize);
				fwrite(buffer,fileSize,1,f);
				fclose(f);
				delete buffer;
			}
		}
	//}
	//////////////////////////////////////////////////////////////////////////
	VMEND
	return true;
}

//int CMyPlug::exportData(iScene * pScene, const std::string& strFilename)
//{
//	checkKey();
//	exportObject(pScene,ChangeExtension(strFilename,".obj"));
//	return true;
//}

void CMyPlug::release()
{
	delete this;
}