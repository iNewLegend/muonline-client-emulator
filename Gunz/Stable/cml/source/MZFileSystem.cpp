#include "stdafx.h"
#include "MZFileSystem.h"
#include "MXml.h"
#include <io.h>
#include <crtdbg.h>
#include "MZip.h"
#include "FileInfo.h"
#include "zip/zlib.h"
#include "MDebug.h"


unsigned char* g_pPrivateKey = NULL;
size_t lenPrivateKey = 0;


//#define DELETE(p)       { if(p) { delete (p);     (p)=NULL; } }
#define DELETE_ARRAY(p) { if(p) { delete[] (p);   (p)=NULL; } }

void ReplaceBackSlashToSlash(char* szPath)
{
	int nLen = strlen(szPath);
	for(int i=0; i<nLen; i++){
		if(szPath[i]=='\\') szPath[i]='/';
	}
}

void GetRefineFilename(char *szRefine, const char *szSource)
{
	char pBasePath[256];
	GetCurrentDirectory(sizeof(pBasePath),pBasePath);
	strcat(pBasePath,"\\");

	GetRelativePath(szRefine,pBasePath,szSource);

	ReplaceBackSlashToSlash(szRefine);
}

void ValidateFilename(char* szOut, const char* szSrc, char substitute)
{
	// ���� ���ڵ��� ġȯ�Ѵ� <>:"/\|?*
	strcpy(szOut, szSrc);
	size_t len = strlen(szOut);
	for (size_t i=0; i<len; ++i)
	{
		if (szOut[i] == '<' ||
			szOut[i] == '>' ||
			szOut[i] == ':' ||
			szOut[i] == '\"' ||
			szOut[i] == '/' ||
			szOut[i] == '\\' ||
			szOut[i] == '|' ||
			szOut[i] == '?' ||
			szOut[i] == '*')
			szOut[i] = substitute;
	}
}

time_t dos2unixtime(unsigned long dostime)
{
	struct tm *t;         /* argument for mktime() */
	time_t clock = time(NULL);

	t = localtime(&clock);
	t->tm_isdst = -1;     /* let mktime() determine if DST is in effect */
	/* Convert DOS time to UNIX time_t format */
	t->tm_sec  = (((int)dostime) <<  1) & 0x3e;
	t->tm_min  = (((int)dostime) >>  5) & 0x3f;
	t->tm_hour = (((int)dostime) >> 11) & 0x1f;
	t->tm_mday = (int)(dostime >> 16) & 0x1f;
	t->tm_mon  = ((int)(dostime >> 21) & 0x0f) - 1;
	t->tm_year = ((int)(dostime >> 25) & 0x7f) + 80;

	return mktime(t);
}

unsigned long dostime(int y, int n, int d, int h, int m, int s)
/* year, month, day, hour, minute, second */
/*	Convert the date y/n/d and time h:m:s to a four byte DOS date and
time (date in high two bytes, time in low two bytes allowing magnitude
comparison). */
{
	return y < 1980 ? dostime(1980, 1, 1, 0, 0, 0) :
	(((unsigned long)y - 1980) << 25) | ((unsigned long)n << 21) | 
		((unsigned long)d << 16) | ((unsigned long)h << 11) | 
		((unsigned long)m << 5) | ((unsigned long)s >> 1);
}


unsigned long unix2dostime(time_t t)          /* unix time to convert */
/* Return the Unix time t in DOS format, rounded up to the next two
second boundary. */
{
	time_t t_even;
	struct tm *s;         /* result of localtime() */

	t_even = (t + 1) & (~1);     /* Round up to even seconds. */
	s = localtime(&t_even);       /* Use local time since MSDOS does. */
	return dostime(s->tm_year + 1900, s->tm_mon + 1, s->tm_mday,
		s->tm_hour, s->tm_min, s->tm_sec);
}

void MZFileSystem::RemoveFileList(void)
{
	while(1){
		ZFLISTITOR i=m_ZFileList.begin();
		if(i==m_ZFileList.end()) break;
		MZFILEDESC* pZFile = i->second;
		delete pZFile;
		m_ZFileList.erase(i);
	}
}

bool MZFileSystem::AddItem(MZFILEDESC* pDesc)
{
	char key[_MAX_PATH];
	strcpy(key,pDesc->m_szFileName);
	strlwr(key);
	
//	_RPT1(_CRT_WARN,"%s\n",key);

	ZFLISTITOR it=m_ZFileList.find(key);
	if(it!=m_ZFileList.end())
	{
		MZFILEDESC *pOld=it->second;

		// ���� �ִ� �ͺ��� ���ʵ��� �����ΰ� ? ���������̸� ������Ʈ �� �ʿ����
		double diff=difftime(dos2unixtime(pDesc->m_modTime),dos2unixtime(pOld->m_modTime));
		if(diff<0) // �� ������ �����̹Ƿ� ��ü�� �ʿ䰡 ����
		{
			// ������Ʈ ��ȣ�� �ռ��� �Ǽ� ?
			int nOldPkgNum=GetUpdatePackageNumber(pOld->m_szZFileName);
			int nNewPkgNum=GetUpdatePackageNumber(pDesc->m_szZFileName);
			_ASSERT(nOldPkgNum>nNewPkgNum);

			return false;
		}

		// ���� �����͸� �����
		delete pOld;
		m_ZFileList.erase(it);
	}

	m_ZFileList.insert(ZFLIST::value_type(string(key), pDesc));
	return true;
}

void MZFileSystem::RefreshFileList(const char* szBasePath)
{
	_ASSERT(szBasePath!=NULL);

	char szFilter[_MAX_PATH];
	sprintf(szFilter,"%s*",szBasePath);

    struct _finddata_t c_file;
    long hFile;
	if( (hFile = _findfirst( szFilter, &c_file )) != -1L ){
		do{
			if(c_file.attrib&_A_SUBDIR){

				// Recursive Run if Sub Directory.
				if(strcmp(c_file.name, ".")==0) continue;
				if(strcmp(c_file.name, "..")==0) continue;
				
				char szPath[256];
				sprintf(szPath,"%s%s/",szBasePath,c_file.name);
				RefreshFileList(szPath);
			}
			else{

				char szDrive[_MAX_PATH], szDir[_MAX_PATH], szFileName[_MAX_PATH], szExt[_MAX_PATH];
				_splitpath(c_file.name, szDrive, szDir, szFileName, szExt);
				
				// zip �����̸� ���� ���ϵ��� ���Ѵ�
				if(stricmp(szExt, "."DEF_EXT)==0 || stricmp(szExt, ".zip")==0) {

					char szZipFileName[_MAX_PATH],szBaseLocation[_MAX_PATH];
					sprintf(szZipFileName,"%s%s",szBasePath,c_file.name);
					char szRelZipFileName[_MAX_PATH];
					GetRelativePath(szRelZipFileName,m_szBasePath,szZipFileName);
					ReplaceBackSlashToSlash(szRelZipFileName);

					// base directory �� �ִ� ������Ʈ�� ����� ��Ű���� ���ϸ��� ������ �������� �ʴ´�
					if(GetUpdatePackageNumber(szRelZipFileName)>0)
					{
						szBaseLocation[0]=0;
					}else
					{
						GetRelativePath(szBaseLocation,m_szBasePath,szBasePath);
						sprintf(szBaseLocation,"%s%s/",szBaseLocation,szFileName);
					}

					FILE* fp = fopen(szZipFileName, "rb");
					if(fp==NULL) continue;

					MZip zf;
//					unsigned long dwReadMode = MZIPREADFLAG_ZIP | MZIPREADFLAG_MRS | MZIPREADFLAG_MRS2 | MZIPREADFLAG_FILE;

					if(zf.Initialize(fp,MZFile::GetReadMode()))
					{
						for(int i=0; i<zf.GetFileCount(); i++)
						{
							char szCurFileName[_MAX_PATH];
							zf.GetFileName(i, szCurFileName);

							// ���丮�� �����ʿ����
							char lastchar=szCurFileName[strlen(szCurFileName)-1];
							if(lastchar!='\\' && lastchar!='/')
							{
								MZFILEDESC* pDesc = new MZFILEDESC;
								sprintf(pDesc->m_szFileName, "%s%s",szBaseLocation,szCurFileName);
								ReplaceBackSlashToSlash(pDesc->m_szFileName);
								strcpy(pDesc->m_szZFileName, szRelZipFileName);
								pDesc->m_iSize = zf.GetFileLength(i);
								pDesc->m_crc32 = zf.GetFileCRC32(i);
								pDesc->m_modTime = zf.GetFileTime(i);

								if(!AddItem(pDesc))
									delete pDesc;
							}
						}
					}
					else
					{
						fclose(fp);
					}
				}
				else
				{
					// Add File Desc
					MZFILEDESC* pDesc = new MZFILEDESC;

					// ������
					char szFullPath[_MAX_PATH];
					sprintf(szFullPath, "%s%s", szBasePath,c_file.name);
					GetRelativePath(pDesc->m_szFileName,m_szBasePath,szFullPath);
					ReplaceBackSlashToSlash(pDesc->m_szFileName);
					pDesc->m_szZFileName[0] = NULL;
					pDesc->m_iSize = c_file.size;
					pDesc->m_crc32 = 0;
					pDesc->m_modTime=unix2dostime(c_file.time_write);

					if(!AddItem(pDesc))
						delete pDesc;
				}
			}
		}while( _findnext( hFile, &c_file ) == 0 );

		_findclose( hFile );
	}
}

int MZFileSystem::GetUpdatePackageNumber(const char *szPackageFileName)
{
	if(!szPackageFileName || szPackageFileName[0]==0) return 0;		// �׳� ������ �ִ� �����̴�

	int nLength=strlen(m_szUpdateName);
	if(m_szUpdateName[0] && strnicmp(szPackageFileName,m_szUpdateName,nLength)==0)
	{
		int nNumber=atoi(szPackageFileName+nLength)+1;
		return nNumber;		// �������� ��Ű�� ���� �ִ� ����
	}
	return -1;		// �Ϲ����� ��Ű�� ���� �ִ� ����
}

unsigned MGetCRC32(const char *data, int nLength)
{
	uLong crc = crc32(0L, Z_NULL, 0);
	crc = crc32(crc, (byte*)data, nLength);
	return crc;
}

unsigned int MZFileSystem::GetCRC32(const char* szFileName)
{
	MZFILEDESC* pDesc = GetFileDesc(szFileName);
	if(!pDesc) return 0;

	// zip ���� �ȿ� �ִ°��� �̹� crc�� �ִ�
	if(pDesc->m_szZFileName[0]) {
		return pDesc->m_crc32;
	}

	// �׷��� ������ �о ����Ѵ�

	MZFile mzf;
	if(!mzf.Open(szFileName,this)) return 0;

	int nFileLength=mzf.GetLength();
	char *buffer=new char[nFileLength];
	mzf.Read(buffer,nFileLength);
	mzf.Close();

	unsigned int crc = MGetCRC32(buffer,nFileLength);
	delete buffer;

	return crc;
}

unsigned int MZFileSystem::GetTotalCRC()
{
	unsigned int nCRCTotal = 0;
	for (int i=0; i<GetFileCount(); i++) {
		const MZFILEDESC* pDesc = GetFileDesc(i);
		nCRCTotal += pDesc->m_crc32;
	}
	return nCRCTotal;
}

MZFileSystem::MZFileSystem(void) : m_pCheckList(NULL)
{
	m_szBasePath[0] = 0;
	m_szUpdateName[0] = 0;
}

MZFileSystem::~MZFileSystem(void)
{
	Destroy();
}

void AddSlash(char *szPath)
{
	// ��εڿ� '/'�� �ֵ��� �����
	int nLength=strlen(szPath);
	if(nLength>0 && (szPath[nLength-1]!='/' && szPath[nLength-1]!='\\'))
		strcat(szPath,"/");
}

bool MZFileSystem::Create(const char* szBasePath,const char* szUpdateName)
{
	Destroy();

	strcpy(m_szBasePath,szBasePath);
	AddSlash(m_szBasePath);

	if(szUpdateName) {
		char szRelative[_MAX_PATH];
		sprintf(szRelative,"%s%s",m_szBasePath,szUpdateName);
		GetRelativePath(m_szUpdateName,m_szBasePath,szRelative);
	}

	RemoveFileList();
	RefreshFileList(m_szBasePath);

	/*
	// �ϰ������� '\'�� '/'�� �ٲ۴�.
	for(ZFLISTITOR li=m_ZFileList.begin(); li!=m_ZFileList.end(); li++){
		MZFILEDESC* pDesc = li->second;
		ReplaceBackSlashToSlash(pDesc->m_szFileName);
//		ReplaceBackSlashToSlash(pDesc->m_szLocation);
	}
	*/

	m_nIndex=0;
	m_iterator=m_ZFileList.begin();

	return true;
}

void MZFileSystem::Destroy(void)
{
	m_szBasePath[0] = 0;
	RemoveFileList();

	if ( g_pPrivateKey != NULL)
	{
		delete g_pPrivateKey;
		g_pPrivateKey = NULL;

		lenPrivateKey = 0;
	}
}

int MZFileSystem::GetFileCount(void) const
{
	return m_ZFileList.size();
}

const char* MZFileSystem::GetFileName(int i)
{
	return GetFileDesc(i)->m_szFileName;
}

const MZFILEDESC* MZFileSystem::GetFileDesc(int i)
{
	while(m_nIndex!=i)
	{
		if(m_nIndex<i)
		{
			m_nIndex++;
			m_iterator++;
			if(m_iterator==m_ZFileList.end())
				return NULL;
		}
		else
		{
			if(m_iterator==m_ZFileList.begin())
				return NULL;
			m_nIndex--;
			m_iterator--;
		}
	}

	return m_iterator->second;
}

MZFILEDESC* MZFileSystem::GetFileDesc(const char* szTarget)
{
	char key[_MAX_PATH];
	strcpy(key,szTarget);
	strlwr(key);

	ZFLISTITOR found=m_ZFileList.find(key);
	if(found!=m_ZFileList.end())
		return found->second;;

	return NULL;
}

int MZFileSystem::GetFileLength(int i)
{
	const MZFILEDESC* pDesc = GetFileDesc(i);
	return pDesc->m_iSize;
}

int MZFileSystem::GetFileLength(const char* szFileName)
{
	MZFILEDESC* pDesc = GetFileDesc(szFileName);
	return pDesc->m_iSize;
}

bool MZFileSystem::ReadFile(const char* szFileName, void* pData, int nMaxSize)
{
	MZFILEDESC* pDesc = GetFileDesc(szFileName);
	if(!pDesc) return false;

	/*
	if(pDesc->m_szLocation[0] != NULL)//zip
	{
		FILE* fp = fopen(pDesc->m_szLocation, "rb");

		if(fp==NULL) return false;

		MZip z;

		if(z.Initialize(fp) == false)
		{
			fclose(fp);
			return false;
		}

		bool hr = z.ReadFile(pDesc->m_szFileName, pData, nMaxSize);

		fclose(fp);

		return hr;
	}
	else
	*/
	{
		FILE* fp = fopen(pDesc->m_szFileName, "rb");

		if(fp==NULL) return false;

		fread(pData,1,nMaxSize,fp);

		fclose(fp);

		return true;
	}
}

void MZFileSystem::SetPrivateKey( const unsigned char* pPrivateKey, size_t length)
{
	if ( g_pPrivateKey != NULL)
	{
		delete g_pPrivateKey;
		g_pPrivateKey = NULL;

		lenPrivateKey = 0;
	}

	g_pPrivateKey = new unsigned char[ length];
	memcpy( g_pPrivateKey, pPrivateKey, length);
	lenPrivateKey = length;
}






unsigned long MZFile::m_dwReadMode = MZIPREADFLAG_ZIP | MZIPREADFLAG_MRS | MZIPREADFLAG_MRS2 | MZIPREADFLAG_FILE;


// ������
MZFile::MZFile() : m_nIndexInZip(-1)
{
	m_fp			= NULL;

	m_IsZipFile		= false;
	m_IsBufferd		= false;
	m_IsEncrypted	= false;

	m_pData				= NULL;
	m_nFileSize			= 0;
	m_nEncryptFileSize	= 0;
	m_nPos				= 0;

	m_FileName[ 0]		= 0;
	m_ZipFileName[ 0]	= 0;
}


// �Ҹ���
MZFile::~MZFile()
{
	Close();
}


bool MZFile::Create()
{
	return true;
}


#include "MRsaEncrypt.h"


// ��ȣȭ�� �������� Ȯ��
bool IsEncryptedFile( const char* szFileName)
{
	if ( strstr( szFileName, ".mef") != NULL)
		return true;

	return false;
}


// ���� ����
bool MZFile::Open( const char* szFileName, MZFileSystem* pZFS)
{
	// ���� �ʱ�ȭ
	Close();


	// ���� �ý����� ������ ��Ʈ�� ���� ����
	if ( pZFS == NULL)
	{
		 // �׳� ���� �б� ����
		if ( isMode( MZIPREADFLAG_FILE) == false)
			return false;


		// ���� ����
		m_fp = fopen( szFileName, "rb");

		if ( m_fp == NULL)
			return false;


		// ���� ������ ���ϱ�
		fseek( m_fp, 0, SEEK_END);
		long size = ftell( m_fp);

		fseek( m_fp, 0, SEEK_SET);


		// Encrypt �� ������ ���...
		if ( IsEncryptedFile( szFileName))
		{
			// ���� �б�
			char* pBuff = new char[ size];
			fread( pBuff, sizeof( char), size, m_fp);
			fclose( m_fp);

			// ��ȣȭ ����
			if ( g_pPrivateKey == NULL)
				return false;

			RSAHEADER header;
			string strBuff = RSADecryptString( g_pPrivateKey, lenPrivateKey, pBuff, size, &header);
			DELETE_ARRAY( pBuff);

			// ������ ���۸�
			m_pData = new char[ strBuff.size() + 1];
			memcpy( m_pData, strBuff.c_str(), strBuff.size());
			m_pData[ strBuff.size()] = 0;
			strBuff.clear();

			m_IsBufferd = true;
			m_IsEncrypted = true;

			m_nFileSize = m_nFileSize = header.GetLength();
		}

		// �Ϲ� ��Ʈ�� ������ ���...
		else
		{
			m_IsBufferd = false;
			m_nFileSize = size;
		}


		// ��Ÿ ����
		strcpy( m_FileName, szFileName);
		m_IsZipFile = false;


		return true;
	}

	// ZIP ���� ����
	else
	{
		// ZIP ���� ����
		MZFILEDESC* pDesc = pZFS->GetFileDesc( szFileName);

		if ( pDesc == NULL) 
			return false;


		// ������ ZIP ��ŷ �ȿ� �ִ� ���...
		if ( pDesc->m_szZFileName[ 0])
		{
			char szRelativePathName[ _MAX_PATH];			// ������ ��� ��� �н�
			char *pRelative	= pDesc->m_szFileName;			// ���� �̸�
			char *pDest		= pDesc->m_szZFileName;			// 


			// ������ ã�´�
			while ( strnicmp( pRelative, pDest,1) == 0)
			{
				pRelative++;
				pDest++;
			}

			if ( *pRelative == '/')
				pRelative++;

			sprintf( szRelativePathName, "%s", pRelative);


			// ZIP ������ ��θ� ���Ѵ�
			char szZipFullPath[ _MAX_PATH];
			sprintf( szZipFullPath, "%s%s", pZFS->GetBasePath(), pDesc->m_szZFileName);


			// CRC �˻縦 �Ѵ�
			bool bFileCheck = false;
			if ( pZFS->GetFileCheckList())
			{
				/*unsigned int crc = pZFS->GetFileCheckList()->GetCRC32( szFileName);
				if ( crc != pDesc->m_crc32)
				{
#ifdef _DEBUG
					char szBuffer[ 256];
					sprintf( szBuffer, "CRC error, %s file %u , source %u \n", szFileName, m_crc32, crc);
					OutputDebugString( szBuffer);
#endif
					return false;
				}

				bFileCheck = true;*/
			}


			// ������ ����
			return Open( szRelativePathName, szZipFullPath, bFileCheck,pDesc->m_crc32);
		}


		// �׳� ���� �б� ����
		if ( isMode( MZIPREADFLAG_FILE) == false)
			return false;


		// �� ������ �ִ°��
		char szFullPath[ _MAX_PATH];
		sprintf( szFullPath, "%s%s", pZFS->GetBasePath(), szFileName);
		return Open( szFullPath);
	}
}


// ���� ����(ZIP ���� ����)
bool MZFile::Open( const char* szFileName, const char* szZipFileName, bool bFileCheck , unsigned int crc32)
{
	Close();


	if ( stricmp( m_ZipFileName, szZipFileName) != 0)
	{
		m_fp = fopen( szZipFileName, "rb");

		if( m_fp==NULL)
			return false;

		if ( m_Zip.Initialize( m_fp, m_dwReadMode) == false)
			return false;

		strcpy( m_ZipFileName, szZipFileName);
	}


	// ZIP ������ ���� ���ϸ� �׳� ����
	if ( m_Zip.isReadAble( m_dwReadMode) == false) 
		return false;


	// ���� �ε����� ����
	m_nIndexInZip = m_Zip.GetFileIndex( szFileName);
	m_crc32 = m_Zip.GetFileCRC32( m_nIndexInZip);
	if ( bFileCheck && (m_crc32 != crc32))
	{
		// filesystem �� �ʱ�ȭ �ɶ��� ������ crc�� �ٸ��� Ȯ��
#ifdef _DEBUG
		char szBuffer[ 256];
		sprintf( szBuffer,"CRC error, modified after initialize, %s file in %s %u , source %u \n", szFileName, szZipFileName, m_crc32, crc32);
		OutputDebugString( szBuffer);
#endif
		return false;
	}

	
	// ��ȣȭ�� ������ ��쿣 ����� �о ����� ���Ѵ�
	if ( IsEncryptedFile( szFileName))
	{
		m_nFileSize = m_Zip.GetFileLength( m_nIndexInZip);

		char* pBuff = new char[ m_nFileSize + 1];
		if ( !m_Zip.ReadFile( m_nIndexInZip, pBuff, m_nFileSize))
			return true;

		RSAHEADER header = RSAGetHeader( pBuff);
		DELETE_ARRAY( pBuff);

		if ( header.IsValidHeader() == false)
			return false;

		m_nEncryptFileSize = header.GetLength();
		m_IsEncrypted = true;
	}
	else
		m_nFileSize = m_Zip.GetFileLength( m_nIndexInZip);

	// ���� ����
	m_IsZipFile = true;
	m_IsBufferd = true;
	strcpy( m_FileName, szFileName);

	return true;
}


// ���� �ݱ�
void MZFile::Close(void)
{
	if ( m_IsZipFile)
		m_Zip.Finalize();


	if ( m_fp)
	{
		fclose( m_fp);
		m_fp = NULL;
	}


	m_IsZipFile	= false;
	m_IsBufferd	= false;

	m_nPos				= 0;
	m_nFileSize			= 0;
	m_nEncryptFileSize	= 0;

	m_FileName[ 0]	  = 0;
	m_ZipFileName[ 0] = 0;

	DELETE_ARRAY( m_pData);
}


unsigned long MZFile::GetLength(void)
{
	return ( m_IsEncrypted) ? m_nEncryptFileSize : m_nFileSize;
}


bool MZFile::Seek(long off,int mode)
{
	// ���۸� ������ ���...
	if ( m_IsBufferd && (m_pData != NULL))
	{
		if ( mode == begin)
		{
			m_nPos = off;
			return true;
		}
		else if ( mode == current)
		{
			m_nPos += off;
			return true;
		}
		else if ( mode == end)
		{
			m_nPos = GetLength() + off;
			return true;
		}
	}

	// ��Ʈ�� �����̸�...
	else
	{
		if ( mode == begin)
		{
			fseek( m_fp, off, SEEK_SET);
			return true;
		}
		else if( mode == current)
		{
			fseek( m_fp, off, SEEK_CUR);
			return true;
		}
		else if(mode == end)
		{
			fseek( m_fp, off, SEEK_END);
			return true;
		}
	}

	return false;
}


bool MZFile::Read( void* pBuffer, int nMaxSize)
{
	// ���۸� ������ ���...
	if ( m_IsBufferd) 
	{
		// ������ ��ȿ���� Ȯ��
		if ( nMaxSize > ( GetLength() - m_nPos))
			return false;


		// ��ȣȭ�� �����̸�...
		if ( m_IsEncrypted)
		{
			// ���ʷ� Read �Ҷ� �Ҵ�
			if ( m_pData == NULL)
			{
				// ���� �б�
				char* pBuff = new char[ m_nFileSize + 1];

				if ( !m_Zip.ReadFile( m_nIndexInZip, pBuff, m_nFileSize))
				{
					mlog( "%s open failed\n", m_FileName);
					return false;
				}

				// ��ȣȭ ����
				if ( g_pPrivateKey == NULL)
					return false;

				string strBuff = RSADecryptString( g_pPrivateKey, lenPrivateKey, pBuff, m_nFileSize);
				DELETE_ARRAY( pBuff);

				// ������ ���۸�
				m_pData = new char[ strBuff.size() + 1];
				memcpy( m_pData, strBuff.c_str(), strBuff.size());
				m_pData[ strBuff.size()] = 0;

				strBuff.clear();
			}

			memcpy( pBuffer, (m_pData + m_nPos), nMaxSize);
		}

		// ��ȣȭ���� ���� �����̸�...
		else
		{
			// �ѹ��� ������ �������� �ϴ� �õ��� �޸� �Ҵ� ���� �о��ش�
			if ( (nMaxSize == GetLength()) && (m_nPos == 0))
			{
				if ( !m_Zip.ReadFile( m_FileName, pBuffer, m_nFileSize))
				{
					mlog( "%s file open failed\n", m_FileName);
					return false;
				}
			}
			else
			{
				// ���ʷ� Read �Ҷ� �Ҵ�
				if ( m_pData == NULL)
				{
					m_pData = new char[ m_nFileSize + 1];
					m_pData[ m_nFileSize] = 0;

					if ( !m_Zip.ReadFile( m_nIndexInZip, m_pData, m_nFileSize))
					{
						mlog( "%s open failed\n", m_FileName);
						return false;
					}
				}

				memcpy( pBuffer, (m_pData + m_nPos), nMaxSize);
			}
		}


		m_nPos += nMaxSize;
	}
	
	// ��Ʈ�� ������ ���...
	else
	{
		size_t numread = fread( pBuffer, 1, nMaxSize, m_fp);

		if ( numread != nMaxSize)
			return false;
	}


	return true;
}


bool MZFileCheckList::Open(const char *szFileName, MZFileSystem *pfs)
{
	MZFile mzf;
	if(!mzf.Open(szFileName,pfs))
		return false;

	char *buffer;
	buffer=new char[mzf.GetLength()+1];
	mzf.Read(buffer,mzf.GetLength());
	buffer[mzf.GetLength()]=0;

	MXmlDocument aXml;
	aXml.Create();
	if(!aXml.LoadFromMemory(buffer))
	{
		delete buffer;
		return false;
	}

	m_crc32 = MGetCRC32(buffer,mzf.GetLength());
	delete buffer;

	int iCount, i;
	MXmlElement		aParent, aChild;
	aParent = aXml.GetDocumentElement();
	iCount = aParent.GetChildNodeCount();

	char szTagName[256];
	for (i = 0; i < iCount; i++)
	{
		aChild = aParent.GetChildNode(i);
		aChild.GetTagName(szTagName);
		if(stricmp(szTagName,"FILE")==0)
		{
			char szContents[256],szCrc32[256];
			aChild.GetAttribute(szContents,"NAME");
			aChild.GetAttribute(szCrc32,"CRC32");

			if(stricmp(szContents,"config.xml")!=0)
			{
				unsigned int crc32_current;
				sscanf(szCrc32,"%x",&crc32_current);

				char szLowerName[256];
				strcpy(szLowerName,szContents);
				strlwr(szLowerName);

				m_fileList.insert(map<string,unsigned int>::value_type(string(szLowerName),crc32_current));
			}
		}
	}
	return true;
}


unsigned int MZFileCheckList::GetCRC32(const char *szFileName)
{
	char szLowerName[256];
	strcpy(szLowerName,szFileName);
	strlwr(szLowerName);

	map<string,unsigned int>::iterator i;
	i = m_fileList.find(string(szLowerName));

	if(i!=m_fileList.end()) {
		return i->second;
	}

	return 0;
}
