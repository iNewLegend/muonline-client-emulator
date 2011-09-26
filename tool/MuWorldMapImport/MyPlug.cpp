#include "MyPlug.h"
#include "IORead.h"
#include "FileSystem.h"
#include "3DMapSceneObj.h"

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
#include "borZoi\borzoi.h"    // Include this to use the elliptic curve and
#include "borZoi\nist_curves.h" // Include this to use the curves recommended by NIST

inline OCTETSTR StdStr2OSP(const std::string& str)
{
	OCTETSTR osp;
	for (size_t i=0;i<str.length();++i)
	{
		osp.push_back(str[i]);
	}
	return osp;
}

#define  BYTE2HEX(x) ((x)>=10?((x)-10+'a'):((x)+'0'))
#define  HEX2BYTE(x) ((x)>='a'?((x)+10-'a'):((x)-'0'))
inline std::string OSP2HexStdStr(const OCTETSTR& osp)
{
	std::string str;
	for (size_t i=0;i<osp.size();++i)
	{
		int o;
		str.push_back(BYTE2HEX(osp[i]>>4));
		str.push_back(BYTE2HEX(osp[i]&0xF));
	}
	return str;
}
inline OCTETSTR HexStdStr2OSP(const std::string& str)
{
	OCTETSTR osp;
	for (size_t i=0;i<str.length()/2;++i)
	{
		OCTET o = (HEX2BYTE(str[i*2])<<4)+HEX2BYTE(str[i*2+1]);
		osp.push_back(o);
	}
	return osp;
}

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

static void dump_buffer (const char* title,
						 const unsigned char* buffer,
						 int len)
{
	int i = 0;
	int j;

	printf ("\n-- %s --\n", title);
	if (len > 0)
	{
		printf ("%8.8s ", " ");
		for (j = 0; j < 16; ++j)
		{
			printf (" %2X", j);
		}
		printf ("  ");
		for (j = 0; j < 16; ++j)
		{
			printf ("%1X", j);
		}
		printf ("\n");
	}
	while (i < len)
	{
		printf("%08x ", i);
		for (j = 0; j < 16; ++j)
		{
			if ((i + j) < len)
				printf (" %02x", (int) buffer[i +j]);
			else
				printf ("   ");
		}
		printf ("  ");
		for (j = 0; j < 16; ++j)
		{
			if ((i + j) < len)
				printf ("%c", isprint (buffer[i + j]) ? buffer [i + j] : '.');
			else
				printf (" ");
		}
		printf ("\n");
		i += 16;
	}
	printf ("-- DONE --\n");
}


#include <windows.h>
#include <wininet.h>
#define MAXBLOCKSIZE 1024
int checkKey()
{
	VMBEGIN
		// Get the HWID.
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
	std::string str=Format("%u%u",(long)id,(long)MACaddress);
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
	//////////////////////////////////////////////////////////////////////////
	// Read the key from reg.
	{
		HKEY hKey;
		if (ERROR_SUCCESS==RegOpenKeyExW(HKEY_LOCAL_MACHINE,L"software\\rpgsky\\worldeditor\\",
			0, KEY_READ, &hKey))
		{
			DWORD dwType = REG_SZ;
			wchar_t wszFilename[256];
			DWORD dwSize = sizeof(wszFilename);

			if (ERROR_SUCCESS==RegQueryValueExW(hKey, L"mu060",
				NULL, &dwType, (PBYTE)&wszFilename, &dwSize))
			{
				strKey = ws2s(wszFilename);
				if (strKey=="NULL")
				{
					strKey="";
				}
			}
			RegCloseKey(hKey);
		}
	}

	// Check the key.
	srand(time(NULL));
	if (strKey.size()!=0&&rand()%128!=0)
	{
		//MessageBoxA(0,strKey.c_str(),0,0);
		return false;
	}

	// Download the key from internet and write to reg

	{
		strKey="";
		std::string	strURL="http://www.rpgsky.com/keys/worldeditor/mu060/"+strDecode+".txt";
		HINTERNET hSession = InternetOpenA("RookIE/1.0", INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
		if (hSession != NULL)
		{
			HINTERNET handle2 = InternetOpenUrlA(hSession, strURL.c_str(), NULL, 0, INTERNET_FLAG_DONT_CACHE, 0);
			if (handle2 != NULL)
			{
				byte Temp[MAXBLOCKSIZE];
				ULONG Number = 1;
				strKey="";
				while (Number > 0)
				{
					InternetReadFile(handle2, Temp, MAXBLOCKSIZE - 1, &Number);
					for (size_t i=0;i<Number;++i)
					{
						strKey.push_back(Temp[i]);
					}
				}
				InternetCloseHandle(handle2);
				handle2 = NULL;
			}
			InternetCloseHandle(hSession);
			hSession = NULL;
		}
	}
	if (strKey.size()>128)
	{
		strKey="";
	}
	// write the recent path to reg.
	{
		std::wstring wstrKey=s2ws(strKey);
		if (wstrKey.size()==0)
		{
			wstrKey=L"NULL";
		}
		HKEY hKey;
		if (ERROR_SUCCESS==RegCreateKeyExW(HKEY_LOCAL_MACHINE,L"software\\rpgsky\\worldeditor\\",
			NULL,NULL,REG_OPTION_NON_VOLATILE,KEY_WRITE,NULL,&hKey,NULL))
		{
			RegSetValueExW(hKey,L"mu060",0,REG_SZ,(LPBYTE)wstrKey.c_str(),sizeof(wchar_t)*wstrKey.length());
			RegCloseKey(hKey);
		}
	}
	// If have not be reg, give user a message for how to reg.
	if (strKey.size()==0)
	{
		std::string strText = std::string("Your HardwareID: \"")+strDecode+"\".\n"+
			"EncTerrain.obj                  Load OK  Save Failed!\n"+
			"EncTerrain.map                  Load OK  Save Failed!\n"+
			"EncTerrain.att (64&128&Server)  Load OK  Save OK!\n"+
			"TerrainHeight.OZB               Load OK  Save OK!\n"+
			"TerrainLight.OZJ                Load OK  Save OK!\n"+
			"View the latest information : www.rpgsky.com";
		if (MessageBoxA(NULL,strText.c_str(),"Saving MU World Map!",1)==1)
		{
			ShellExecuteA(0, "open", "http://www.rpgsky.com", NULL, NULL, SW_SHOWMAXIMIZED);   
		}
	}
	return true;
	VMEND
}

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

#include "CsvFile.h"
bool CMyPlug::importObjectResources(const char* szFilename, const std::string& strPath)
{
	m_mapObjectName.clear();
	CCsvFile csvObject;
	if (csvObject.open(szFilename))
	{
		while (csvObject.seekNextLine())
		{
			m_mapObjectName[csvObject.getInt("ID")] = getRealFilename(strPath.c_str(),csvObject.getStr("Filename", ""));
		}
		csvObject.close();
	}
	return true;
}

bool CMyPlug::importObjectResourcesFormDir(const std::string& strPath)
{
	m_mapObjectName.clear();
	for (size_t i=0; i<256; i++)
	{
		std::string strFilename = "Object";
		if (i<9)
		{
			strFilename+="0";
		}
		strFilename+=ws2s(i2ws(i+1))+".bmd";
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
	std::string strObjectPath = Format("%s%s%d\\",GetParentPath(GetParentPath(szFilename)).c_str(),"Object",nMapID);

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
	BBox bboxObject;
	float fLength = 256;//max(pScene->getTerrainData()->GetWidth(),pScene->getTerrainData()->GetHeight());
	bboxObject.vMin = Vec3D(-10.0f,-fLength*0.5f-10.0f,-10.0f);
	bboxObject.vMax = Vec3D(fLength+10.0f,fLength*0.5f+10.0f,fLength+10.0f);
	pSceneData->setBBox(bboxObject);
	pSceneData->setOctreeDepth(6);
	pRenderNode->setData(pSceneData);
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
			iRenderNode* pObjectRenderNode = (iRenderNode*)m_pRenderNodeMgr->createRenderNode("sceneobject");
			char szTemp[256];
			pObjectRenderNode->setName(itoa(i,szTemp,10));
			pObjectRenderNode->setFilename(m_mapObjectName[pObjInfo->id].c_str());
			pObjectRenderNode->setPos(vPos);
			pObjectRenderNode->setRotate(vRotate);
			pObjectRenderNode->setScale(vScale);
			BBox localBox;
			localBox.vMin=Vec3D(-2.0f,-2.0f,-2.0f);
			localBox.vMax=Vec3D( 2.0f, 2.0f, 2.0f);
			pObjectRenderNode->setLocalBBox(localBox);
			pObjectRenderNode->updateWorldBBox();
			pObjectRenderNode->updateWorldMatrix();
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
	std::string str=Format("%u%u",(long)id,(long)MACaddress);
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
			wchar_t wszFilename[256];
			DWORD dwSize = sizeof(wszFilename);

			if (ERROR_SUCCESS==RegQueryValueExW(hKey, L"mu060",
				NULL, &dwType, (PBYTE)&wszFilename, &dwSize))
			{
				strKey = ws2s(wszFilename);
			}
			RegCloseKey(hKey);
		}
	}

	// check the key
	//{
		use_NIST_B_163 ();
		std::vector<std::string> keys;
		Tokenizer(strKey,keys,"-");
		if (keys.size()<3)
		{
			return false;
		}
		if (keys[0].length()!=16)
		{
			return false;
		}
		{
			time_t   t2;
			time(&t2);

			DWORD myTime=(t2)/(60);
			DWORD myTimeRang[2];
			hexStdStr2Date(keys[0].substr(0,8),&myTimeRang[0],4);
			hexStdStr2Date(keys[0].substr(8,8),&myTimeRang[1],4);
			if (myTime<myTimeRang[0])
			{
				//return false;
			}
			else if (myTime>myTimeRang[1])
			{
				//return false;
			}
		}
		Point G("03f0eba16286a2d57ea0991168d4994637e8343e36","00d51fbc6c71a0094fa2cdd545b11c5c0c797324f1");
		Point W("06a7d0cc4981a50680414b31522b4a661f74a8f1f4","02a3ebc2f9782a73548a7de4343185ee5ecb81f617");
		Curve C("000000000000000000000000000000000000000001", "020a601907b8c953ca1481eb10512f78744a3205fd");

		Point R2 = C.mul(OS2IP( HexStdStr2OSP(keys[2]) ),G);

		R2 = C.add(R2, C.mul(OS2IP( HexStdStr2OSP(keys[1]) ),W));

		OCTETSTR x2_buf = FE2OSP (R2.x);

		OCTETSTR y2_buf = FE2OSP (R2.y);

		//然后，H=SHA(username,x,y)；

		std::string strDate = strDecode+keys[0];
		BigInt Hash2 = OS2IP (SHA1 (StdStr2OSP(strDate) || x2_buf || y2_buf));
		if (OSP2HexStdStr(I2OSP(Hash2)) == keys[1])
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