#include "stdafx.h"
#include "MZip.h"
#include "zip/zlib.h"
#include <memory.h>
#include <string.h>
#include "MDebug.h"
#include <tchar.h>
#include <io.h>

typedef unsigned long dword;
typedef unsigned short word;

#define MRS_ZIP_CODE	0x05030207
#define MRS2_ZIP_CODE	0x05030208

#pragma pack(2)
struct MZip::MZIPLOCALHEADER{
	enum{
		SIGNATURE   = 0x04034b50,
		SIGNATURE2  = 0x85840000,
		COMP_STORE  = 0,
		COMP_DEFLAT = 8,
	};

	dword   sig;
	word    version;
	word    flag;
	word    compression;      // COMP_xxxx
	word    modTime;
	word    modDate;
	dword   crc32;
	dword   cSize;
	dword   ucSize;
	word    fnameLen;         // Filename string follows header.
	word    xtraLen;          // Extra field follows filename.
};

struct MZip::MZIPDIRHEADER{
	enum{
		SIGNATURE = 0x06054b50,
	};

	dword   sig;
	word    nDisk;
	word    nStartDisk;
	word    nDirEntries;
	word    totalDirEntries;
	dword   dirSize;
	dword   dirOffset;
	word    cmntLen;
};

struct MZip::MZIPDIRFILEHEADER{
	enum{
		SIGNATURE   = 0x02014b50,
		SIGNATURE2  = 0x05024b80,
		COMP_STORE  = 0,
		COMP_DEFLAT = 8,
	};

	dword   sig;
	word    verMade;
	word    verNeeded;
	word    flag;
	word    compression;      // COMP_xxxx
	word    modTime;
	word    modDate;
	dword   crc32;
	dword   cSize;            // Compressed size
	dword   ucSize;           // Uncompressed size
	word    fnameLen;         // Filename string follows header.
	word    xtraLen;          // Extra field follows filename.
	word    cmntLen;          // Comment field follows extra field.
	word    diskStart;
	word    intAttr;
	dword   extAttr;
	dword   hdrOffset;

	char *GetName   () const { return (char *)(this + 1);   }
	char *GetExtra  () const { return GetName() + fnameLen; }
	char *GetComment() const { return GetExtra() + xtraLen; }
};

#pragma pack()

// ���� ���� �����ϱ�..������ ������ ��۸��� ��,,, ��ȣ�� �˰��� ����...

void ConvertChar(char* pData,int _size)
{
	if(!pData) return;

	WORD w;
	BYTE b,bh;

	for(int i=0;i<_size;i++) {
		b = *pData ^ 0xFF;
		w = b<<3;
		bh = (w&0xff00)>>8;
		b = w&0xff;
		*pData = BYTE( b | bh );
		pData++;

	}
}

void RecoveryChar(char* pData,int _size)
{
	if(!pData) return;

	BYTE b,bh,d;

	for(int i=0;i<_size;i++) {

		b = *pData;
		bh = b&0x07;
		d = (bh<<5)|(b>>3);
		*pData = d ^ 0xff;
		pData++;
	}
}


MZip::MZip(void)
{
	m_fp = NULL;
	m_pDirData = NULL;
	m_ppDir = NULL;
	m_nDirEntries = 0;
	m_nZipMode = ZMode_Zip;
//	m_dwReadMode = MZIPREADFLAG_ZIP | MZIPREADFLAG_MRS | MZIPREADFLAG_MRS2 | MZIPREADFLAG_FILE;
	m_dwReadMode = 0;
}

MZip::~MZip(void)
{
	Finalize();
}

bool MZip::isReadAble(unsigned long mode)
{
	if(m_nZipMode == ZMode_Zip) {
		return ( MZIPREADFLAG_ZIP & mode) ? true : false ; 	
	}
	else if(m_nZipMode == ZMode_Mrs) {
		return ( MZIPREADFLAG_MRS & mode) ? true : false ; 
	}
	else if(m_nZipMode == ZMode_Mrs2) {
		return ( MZIPREADFLAG_MRS2 & mode) ? true : false ; 
	}
	return false;
}

bool MZip::Initialize(FILE* fp,unsigned long ReadMode)
{
	if(fp==NULL) return false;

	// zip , mrs1 ,mrs2 �� �о���Ѵ�..
	// publish ��,,, zip �� ���� ���ϵ��� �ɼǿ� ��带 �д�.. 
	// ���Ͽ����� �б� �Ұ� + zip ���ɿ��� + mrs1 ���ɿ���..
	// zip �� ���� �ʿ䰡 �ִ°�?

	m_dwReadMode = ReadMode;

	if(isZip(fp)) {
		m_nZipMode = ZMode_Zip;
		// �÷��׿��� �������� ������...
		if(isMode(MZIPREADFLAG_ZIP)==false)
			return false;
	}
	else if(isVersion1Mrs(fp)) {
		m_nZipMode = ZMode_Mrs;
		if(isMode(MZIPREADFLAG_MRS)==false)
			return false;
	}
	else {//mrs2 �̻�...
		m_nZipMode = ZMode_Mrs2;
		if(isMode(MZIPREADFLAG_MRS2)==false)
			return false;
	}
	
	MZIPDIRHEADER dh;

	fseek(fp, -(int)sizeof(dh), SEEK_END);
	long dhOffset = ftell(fp);
	memset(&dh, 0, sizeof(dh));
	fread(&dh, sizeof(dh), 1, fp);

	if( m_nZipMode>=ZMode_Mrs2 )							// mrs2 �̻���� ������ ����..
		RecoveryChar((char*)&dh,sizeof(MZIPDIRHEADER));		// v2 �̻��̸�..

	// �����͸� �����ϴ� ���.... zip , mrs1 , mrs2 ��ξƴҰ��...

	if( dh.sig != MRS2_ZIP_CODE && dh.sig != MRS_ZIP_CODE && dh.sig != MZIPDIRHEADER::SIGNATURE ) {
		return false;		
	}

	fseek(fp, dhOffset - dh.dirSize, SEEK_SET);

	m_pDirData = new char[dh.dirSize + dh.nDirEntries*sizeof(*m_ppDir)];
	memset(m_pDirData, 0, dh.dirSize + dh.nDirEntries*sizeof(*m_ppDir));
	fread(m_pDirData, dh.dirSize, 1, fp);

	if( m_nZipMode>=ZMode_Mrs2 )
		RecoveryChar( (char*)m_pDirData , dh.dirSize );//mrs ��� ��ȯ..

	char *pfh = m_pDirData;
	m_ppDir = (const MZIPDIRFILEHEADER **)(m_pDirData + dh.dirSize);

	for (int i = 0; i < dh.nDirEntries; i++){
		MZIPDIRFILEHEADER& fh = *(MZIPDIRFILEHEADER*)pfh;

		m_ppDir[i] = &fh;

		if(fh.sig != MZIPDIRFILEHEADER::SIGNATURE){
			if(fh.sig != MZIPDIRFILEHEADER::SIGNATURE2) {
				delete[] m_pDirData;
				m_pDirData = NULL;
				return false;
			}
		}

		{
			pfh += sizeof(fh);

			for (int j = 0; j < fh.fnameLen; j++)
			if (pfh[j] == '/')
			  pfh[j] = '\\';

			pfh += fh.fnameLen + fh.xtraLen + fh.cmntLen;
		}
	}

	m_nDirEntries = dh.nDirEntries;
	m_fp = fp;

	return true;
}

bool MZip::Finalize()
{
	if(m_pDirData!=NULL) {
		delete[] m_pDirData;
		m_pDirData=NULL;
	}

	m_fp = NULL;
	m_ppDir = NULL;
	m_nDirEntries = 0;

	return true;
}

int MZip::GetFileCount(void) const
{
	return m_nDirEntries;
}

void MZip::GetFileName(int i, char *szDest) const
{
	if(szDest!=NULL){
		if (i < 0 || i >= m_nDirEntries){
			*szDest = '\0';
		}
		else{
			memcpy(szDest, m_ppDir[i]->GetName(), m_ppDir[i]->fnameLen);
			szDest[m_ppDir[i]->fnameLen] = '\0';
		}
	}
}

int t_strcmp(const char* str1,const char* str2)
{
	int len = strlen(str1);
	if(strlen(str2)!=len) return -1;
	
	for(int i=0;i<len;i++) {

		if(str1[i] != str2[i]) {
			if(	((str1[i]=='\\') || (str1[i]=='/')) && ((str1[i]=='\\') || (str1[i]=='/')) ) {
				continue;
			}
			else
				return -1;
		}
	}
	return 0;
}

int MZip::GetFileIndex(const char* szFileName) const
{
	if(szFileName==NULL) return -1;

	char szSourceName[256];
	for(int i=0; i<GetFileCount();i++){
		GetFileName(i, szSourceName);
//		if(strcmp(szFileName, szSourceName)==0) 
		if(t_strcmp(szFileName, szSourceName)==0) 
			return i;
	}

	return -1;
}

int MZip::GetFileLength(int i) const
{
	if(i<0 || i>=m_nDirEntries)
		return 0;
	else
		return m_ppDir[i]->ucSize;
}

int MZip::GetFileLength(const char* filename)
{
	int index = GetFileIndex(filename);

	if(index == -1) return 0;

	return GetFileLength(index);
}

unsigned int MZip::GetFileCRC32(int i)
{
	if(i<0 || i>=m_nDirEntries)
		return 0;
	else
		return m_ppDir[i]->crc32;
}

unsigned int MZip::GetFileCRC32(const char* filename)
{
	int index = GetFileIndex(filename);

	if(index == -1) return 0;

	return GetFileCRC32(index);
}

unsigned int MZip::GetFileTime(int i)
{
	if(i<0 || i>=m_nDirEntries)
		return 0;
	else
		return MAKELONG(m_ppDir[i]->modTime,m_ppDir[i]->modDate);
}

unsigned int MZip::GetFileTime(const char* filename)
{
	int index = GetFileIndex(filename);

	if(index == -1) return 0;

	return GetFileCRC32(index);
}


bool MZip::ReadFile(int i, void* pBuffer, int nMaxSize)
{
	if (pBuffer==NULL || i<0 || i>=m_nDirEntries)
		return false;

	fseek(m_fp, m_ppDir[i]->hdrOffset, SEEK_SET);
	MZIPLOCALHEADER h;

	fread(&h, sizeof(h), 1, m_fp);

	if(m_nZipMode >= ZMode_Mrs2)
		RecoveryChar((char*)&h,sizeof(h));

	if(h.sig!=MZIPLOCALHEADER::SIGNATURE)
		if(h.sig!=MZIPLOCALHEADER::SIGNATURE2) 
			return false;

	fseek(m_fp, h.fnameLen + h.xtraLen, SEEK_CUR);// MRS �ǰ�� ���� ���� ����..

	if(h.compression==MZIPLOCALHEADER::COMP_STORE){

		fread(pBuffer, h.cSize, 1, m_fp);
		return true;
	}
	else if(h.compression!=MZIPLOCALHEADER::COMP_DEFLAT)
		return false;

	char *pData = new char[h.cSize];
	if(pData==NULL) return false;

	memset(pData, 0, h.cSize);

	int pos = ftell(m_fp);

	fread(pData, h.cSize, 1, m_fp);

	z_stream stream;
	int err;

//#define min(_a, _b)	((_a<=_b)?_a:_b)
	stream.zalloc = (alloc_func)0;
	stream.zfree = (free_func)0;
	stream.opaque = (voidpf)0;

	stream.next_in = (Bytef*)pData;
	stream.avail_in = (uInt)h.cSize;
	stream.next_out = (Bytef*)pBuffer;
	stream.avail_out = min((unsigned int)nMaxSize, h.ucSize);

	err = inflateInit2(&stream, -MAX_WBITS);
	if(err == Z_OK){
		err = inflate(&stream, Z_FINISH);
		inflateEnd(&stream);
		if (err==Z_STREAM_END) err = Z_OK;
		inflateEnd(&stream);
	}

	delete[] pData;

#define CHECK_ZLIB_CRC32
#ifdef CHECK_ZLIB_CRC32
	uLong crc = crc32(0L, Z_NULL, 0);
	crc = crc32(crc, (const Bytef*)pBuffer, stream.total_out);

	if(h.crc32 != crc)
	{
		mlog("crc error \n");
		return false;
	}
#endif

	if(err!=Z_OK) 
		return false;

	return true;
}

bool MZip::ReadFile(const char* filename, void* pBuffer, int nMaxSize)
{
	int index = GetFileIndex(filename);

	if(index == -1) return false;

	return ReadFile(index , pBuffer , nMaxSize);
}

static char _fileheaderReader[1024*16];// sizeof(fh) + fh.fnameLen + fh.xtraLen + fh.cmntLen �� ũ��..����
static int	_fileheaderReaderSize = 0;

bool MZip::UpgradeMrs(char* mrs_name) // Mrs To Mrs2
{
	FILE* fp = fopen(mrs_name, "rb+");

	if(fp==NULL) {
		mlog("%s ������ �б� �������� Ȯ���ϼ���!~ \n",mrs_name);
		return false;
	}

	if( isVersion1Mrs(fp)==false ) // mrs1 ��...
	{
		fclose(fp);
		return false;
	}

	// ����...
	fseek(fp, 0, SEEK_SET);
	int code = MZIPLOCALHEADER::SIGNATURE;
	fwrite(&code, 4, 1, fp);


	MZIPDIRHEADER dh;
	fseek(fp, -(int)sizeof(MZIPDIRHEADER), SEEK_END);

	long dhOffset = ftell(fp);

	fread(&dh, sizeof(dh), 1, fp);

	dh.sig = MZIPDIRHEADER::SIGNATURE;

	long dir_data_pos = dhOffset - dh.dirSize;
	long dir_data_size = dh.dirSize;

	//////////////////////////////////////////////////////////////////

	fseek(fp, dir_data_pos, SEEK_SET);

	char* pDirData = new char[dir_data_size];
	memset(pDirData, 0, dir_data_size);
	fread(pDirData, dir_data_size, 1, fp);

	// ����..

	DWORD _sig = MZIPDIRFILEHEADER::SIGNATURE;

	for(int i=0;i<dir_data_size-3;i++) {

		if((BYTE)pDirData[i] == 0x80) {
			if((BYTE)pDirData[i+1] == 0x4b) {
				if((BYTE)pDirData[i+2] == 0x02) {
					if((BYTE)pDirData[i+3] == 0x05) {
						memcpy(&pDirData[i], &_sig,4);
					}
				}
			}
		}
	}

	////////////////////////////////////////////////////////////////
	// local header 

	char* pTPos = pDirData;

	MZIPDIRFILEHEADER fh;
	MZIPLOCALHEADER h;

	for (int i = 0; i < dh.nDirEntries; i++) {

		fh = *(MZIPDIRFILEHEADER*)pTPos;
/*
		_fileheaderReaderSize = sizeof(fh) + fh.fnameLen + fh.xtraLen ;

		fseek(fp, fh.hdrOffset, SEEK_SET);
		fread(_fileheaderReader, _fileheaderReaderSize, 1, fp);

		ConvertChar( _fileheaderReader , _fileheaderReaderSize );

		fseek(fp, fh.hdrOffset, SEEK_SET);
		fwrite(_fileheaderReader , _fileheaderReaderSize, 1, fp);

		pTPos += _fileheaderReaderSize + fh.cmntLen;
*/
		fseek(fp, fh.hdrOffset, SEEK_SET);

		fread(&h, sizeof(h), 1, fp);

		_fileheaderReaderSize = h.fnameLen + h.xtraLen;

		if(_fileheaderReaderSize)
			fread(_fileheaderReader, _fileheaderReaderSize, 1, fp);

		ConvertChar( (char*)&h , sizeof(h) );
		ConvertChar( _fileheaderReader , _fileheaderReaderSize );

		fseek(fp, fh.hdrOffset, SEEK_SET);

		fwrite(&h,sizeof(h), 1, fp);

		if(_fileheaderReaderSize)
			fwrite(_fileheaderReader , _fileheaderReaderSize, 1, fp);

		pTPos += sizeof(fh) + fh.fnameLen + fh.xtraLen + fh.cmntLen;
	}

	////////////////////////////////////////////////////////////////
	// mrs signature

	ConvertChar( (char*)pDirData , dir_data_size );

	fseek(fp, dir_data_pos, SEEK_SET);
	fwrite(pDirData,dir_data_size,1,fp);

	//////////////////////////////////////////////////////////

	fseek(fp, dhOffset - dh.dirSize, SEEK_SET);

	delete [] pDirData;

	///////////////////////////////////////////////////////////////////

	dh.sig = MRS2_ZIP_CODE;

	ConvertChar((char*)&dh,sizeof(MZIPDIRHEADER));

	fseek(fp, -(int)sizeof(MZIPDIRHEADER), SEEK_END);

	fwrite(&dh, sizeof(dh), 1, fp);

	fclose(fp);


	return true;
}

bool MZip::ConvertZip(char* zip_name)
{
	FILE* fp = fopen(zip_name, "rb+");

	if(fp==NULL) {
		mlog("%s ������ �б� �������� Ȯ���ϼ���!~ \n",zip_name);
		return false;
	}

	MZIPDIRHEADER dh;
	fseek(fp, -(int)sizeof(MZIPDIRHEADER), SEEK_END);

	long dhOffset = ftell(fp);

	fread(&dh, sizeof(dh), 1, fp);

	long dir_data_pos = dhOffset - dh.dirSize;
	long dir_data_size = dh.dirSize;

	//////////////////////////////////////////////////////////////////

	fseek(fp, dir_data_pos, SEEK_SET);

	char* pDirData = new char[dir_data_size];
	memset(pDirData, 0, dir_data_size);
	fread(pDirData, dir_data_size, 1, fp);

	////////////////////////////////////////////////////////////////
	// local header 

	char* pTPos = pDirData;

	MZIPDIRFILEHEADER fh;
	MZIPLOCALHEADER h;

	for (int i = 0; i < dh.nDirEntries; i++) {

		fh = *(MZIPDIRFILEHEADER*)pTPos;
/*
		_fileheaderReaderSize = sizeof(h) + fh.fnameLen + fh.xtraLen;

		fseek(fp, fh.hdrOffset, SEEK_SET);

		fread(_fileheaderReader, _fileheaderReaderSize, 1, fp);

		ConvertChar( _fileheaderReader , _fileheaderReaderSize );

		fseek(fp, fh.hdrOffset, SEEK_SET);
		fwrite(_fileheaderReader , _fileheaderReaderSize, 1, fp);

		pTPos += _fileheaderReaderSize + fh.cmntLen;
*/
		fseek(fp, fh.hdrOffset, SEEK_SET);

		fread(&h, sizeof(h), 1, fp);

		_fileheaderReaderSize = h.fnameLen + h.xtraLen;

		if(_fileheaderReaderSize)
			fread(_fileheaderReader, _fileheaderReaderSize, 1, fp);

		ConvertChar( (char*)&h , sizeof(h) );
		ConvertChar( _fileheaderReader , _fileheaderReaderSize );

		fseek(fp, fh.hdrOffset, SEEK_SET);

		fwrite(&h,sizeof(h), 1, fp);

		if(_fileheaderReaderSize)
			fwrite(_fileheaderReader , _fileheaderReaderSize, 1, fp);

		pTPos += sizeof(fh) + fh.fnameLen + fh.xtraLen + fh.cmntLen;
	}

	////////////////////////////////////////////////////////////////
	// mrs signature

	ConvertChar( (char*)pDirData , dir_data_size );

	fseek(fp, dir_data_pos, SEEK_SET);
	fwrite(pDirData,dir_data_size,1,fp);

	//////////////////////////////////////////////////////////

	fseek(fp, dhOffset - dh.dirSize, SEEK_SET);

	delete [] pDirData;

	///////////////////////////////////////////////////////////////////

	dh.sig = MRS2_ZIP_CODE;

	ConvertChar((char*)&dh,sizeof(MZIPDIRHEADER));

	fseek(fp, -(int)sizeof(MZIPDIRHEADER), SEEK_END);

	fwrite(&dh, sizeof(dh), 1, fp);

	fclose(fp);

	return true;
}

// ���� ���� ���ϵ� �����ؾ� �Ѵ�..

bool MZip::RecoveryMrs(FILE* fp)
{
	fseek(fp, 0, SEEK_SET);
	int code = MZIPLOCALHEADER::SIGNATURE;
	fwrite(&code, 4, 1, fp);

	MZIPDIRHEADER dh;
	fseek(fp, -(int)sizeof(MZIPDIRHEADER), SEEK_END);
	long dhOffset = ftell(fp);
	memset(&dh, 0, sizeof(dh));
	fread(&dh, sizeof(dh), 1, fp);

	dh.sig = MZIPDIRHEADER::SIGNATURE;

	long dir_data_pos = dhOffset - dh.dirSize;
	long dir_data_size = dh.dirSize;

	//////////////////////////////////////////////////////////////////

	fseek(fp, dir_data_pos, SEEK_SET);

	char* pDirData = new char[dir_data_size];
	memset(pDirData, 0, dir_data_size);
	fread(pDirData, dir_data_size, 1, fp);

	DWORD _sig = MZIPDIRFILEHEADER::SIGNATURE;

	for(int i=0;i<dir_data_size-3;i++) {

		if((BYTE)pDirData[i] == 0x80) {
			if((BYTE)pDirData[i+1] == 0x4b) {
				if((BYTE)pDirData[i+2] == 0x02) {
					if((BYTE)pDirData[i+3] == 0x05) {
						memcpy(&pDirData[i], &_sig,4);
					}
				}
			}
		}
	}

	fseek(fp, dir_data_pos, SEEK_SET);
	fwrite(pDirData,dir_data_size,1,fp);

	delete [] pDirData;

	///////////////////////////////////////////////////////////////////

	fseek(fp, -(int)sizeof(MZIPDIRHEADER), SEEK_END);

	fwrite(&dh, sizeof(dh), 1, fp);

	return true;
}

bool MZip::RecoveryMrs2(FILE* fp)
{
	MZIPDIRHEADER dh;
	fseek(fp, -(int)sizeof(dh), SEEK_END);
	long dhOffset = ftell(fp);
	memset(&dh, 0, sizeof(dh));
	fread(&dh, sizeof(dh), 1, fp);

	RecoveryChar((char*)&dh,sizeof(dh));

	dh.sig = MZIPDIRHEADER::SIGNATURE; // ZipCode

	long dir_data_pos = dhOffset - dh.dirSize;
	long dir_data_size = dh.dirSize;

	//////////////////////////////////////////////////////////////////

	fseek(fp, dir_data_pos, SEEK_SET);

	char* pDirData = new char[dir_data_size];
	memset(pDirData, 0, dir_data_size);
	fread(pDirData, dir_data_size, 1, fp);

	RecoveryChar( (char*)pDirData , dir_data_size );//mrs ��� ��ȯ..

	fseek(fp, dir_data_pos, SEEK_SET);
	fwrite(pDirData,dir_data_size,1,fp);


	////////////////////////////////////////////////////////////////
	// local header 

	char* pTPos = pDirData;

	MZIPDIRFILEHEADER	fh;
	MZIPLOCALHEADER h;

	for (int i = 0; i < dh.nDirEntries; i++) {

		fh = *(MZIPDIRFILEHEADER*)pTPos;
/*
		_fileheaderReaderSize = sizeof(fh) + fh.fnameLen + fh.xtraLen;

		fseek(fp, fh.hdrOffset, SEEK_SET);
		fread(_fileheaderReader, _fileheaderReaderSize, 1, fp);

		RecoveryChar( _fileheaderReader , _fileheaderReaderSize );

		fseek(fp, fh.hdrOffset, SEEK_SET);
		fwrite(_fileheaderReader , _fileheaderReaderSize, 1, fp);

		pTPos += _fileheaderReaderSize + fh.cmntLen;
*/
		fseek(fp, fh.hdrOffset, SEEK_SET);

		fread(&h, sizeof(h), 1, fp);

		RecoveryChar( (char*)&h , sizeof(h) );

		_fileheaderReaderSize = h.fnameLen + h.xtraLen;

		if(_fileheaderReaderSize)
			fread(_fileheaderReader, _fileheaderReaderSize, 1, fp);

		RecoveryChar( _fileheaderReader , _fileheaderReaderSize );

		fseek(fp, fh.hdrOffset, SEEK_SET);

		fwrite(&h,sizeof(h), 1, fp);

		if(_fileheaderReaderSize)
			fwrite(_fileheaderReader , _fileheaderReaderSize, 1, fp);

		pTPos += sizeof(fh) + fh.fnameLen + fh.xtraLen + fh.cmntLen;

	}

	//////////////////////////////////////////////////////////////////

	delete [] pDirData;

	///////////////////////////////////////////////////////////////////

	fseek(fp, -(int)sizeof(dh), SEEK_END);

	fwrite(&dh, sizeof(dh), 1, fp);

	return true;
}

bool MZip::isZip(FILE* fp)
{
	fseek(fp, 0, SEEK_SET);
	DWORD sig = 0;
	fread(&sig, 4, 1, fp);

	if(sig == MZIPLOCALHEADER::SIGNATURE)
		return true;

	return false;
}

bool MZip::isVersion1Mrs(FILE* fp)
{
	fseek(fp, 0, SEEK_SET);
	DWORD sig = 0;
	fread(&sig, 4, 1, fp);

	if(sig == MZIPLOCALHEADER::SIGNATURE2)
		return true;

	return false;
}

bool MZip::RecoveryZip(char* zip_name)
{
	FILE* fp = fopen(zip_name, "rb+");

	if(fp==NULL) {
		mlog("%s ������ �б� �������� Ȯ���ϼ���!~ \n",zip_name);
		return false;
	}

	// mrs1 ���� �ĺ�..�̹� ���ϵ��� �������¿��� �ĺ������ �̰ͻ�...

	if( isVersion1Mrs(fp) ) {	// ���ʸ�...
		RecoveryMrs( fp );
	}
	else {
		RecoveryMrs2( fp );		// v2 ���ʹ� ����� sig ������ ��������....
	}

	fclose(fp);

	return true;
}

/////////////////////////////////////////////////////////////////////////////////

FNode::FNode()
{
	memset(m_name,0,256);
	m_size	 = 0;
	m_offset = 0;
}

void FNode::SetName(char* str)	
{
	if(strlen(str) > 255) return;
	strcpy(m_name,str);
	str[255] = 0;
}

/////////////////////////////////////////////////////////////////////////////////

FFileList::FFileList()
{

}

FFileList::~FFileList() 
{
	DelAll();
}

void FFileList::Add(FNode* pNode) 
{
	push_back(pNode);
}

void FFileList::DelAll() 
{
	if(size()==0) return;

	iterator node;

	for(node = begin(); node != end(); ) 
	{
		delete (*node);
		(*node) = NULL;
		node = erase(node);
	}
}

void FFileList::UpgradeMrs() 
{
	iterator node;
	FNode* pNode = NULL;

	for(node = begin(); node != end(); ++node) 
	{
		pNode = (*node);

		if(MZip::UpgradeMrs( pNode->m_name ))
			mlog(" Upgrade mrs : %s\n",pNode->m_name);
	}
}

void FFileList::ConvertZip() 
{
	iterator node;
	FNode* pNode = NULL;

	for(node = begin(); node != end(); ++node) {
		pNode = (*node);

		if(MZip::ConvertZip( pNode->m_name ))
			mlog("convert zip : %s\n",pNode->m_name);
	}
}

void FFileList::RecoveryZip() 
{
	iterator node;
	FNode* pNode = NULL;

	for(node = begin(); node != end(); ++node) {
		pNode = (*node);

		MZip::RecoveryZip( pNode->m_name );
	}
}

void FFileList::ConvertVtf() 
{
	iterator node;
	FNode* pNode = NULL;

	char temp[1024];
	char temp_arg[1024];
	int len;

	for(node = begin(); node != end(); ++node) {
		pNode = (*node);

//		ShellExecute()
		strcpy(temp,pNode->m_name);
		len = strlen(temp);
		temp[len-3] = 0;
		strcat(temp,"tga");

		sprintf(temp_arg,"%s %s",pNode->m_name,temp);
		HINSTANCE hr = ShellExecute(NULL, _T("open"), _T("vtf2tga.exe"),_T(temp_arg), NULL, SW_HIDE);
//		ShellExecute()
//		_execl("vtf2tga.exe","%s %s",pNode->m_name,temp);
	}
}

void FFileList::ConvertNameMRes2Zip() 
{
	iterator node;
	FNode* pNode = NULL;

	char _buf_rename[256];
	int len;

	for(node = begin(); node != end(); ++node) {
		pNode = (*node);

		strcpy(_buf_rename,pNode->m_name);
		len = (int)strlen(pNode->m_name);

		_buf_rename[len-3] = NULL;
		strcat(_buf_rename,"zip");

		rename( pNode->m_name, _buf_rename);

		mlog("rename : %s -> %s \n",_buf_rename,pNode->m_name);
	}
}

void FFileList::ConvertNameZip2MRes() 
{
	iterator node;
	FNode* pNode = NULL;

	char _buf_rename[256];
	int len;

	for(node = begin(); node != end(); ++node) {
		pNode = (*node);

		strcpy(_buf_rename,pNode->m_name);
		len = (int)strlen(pNode->m_name);

		_buf_rename[len-3] = NULL;
		strcat(_buf_rename,"mrs");

		rename( pNode->m_name, _buf_rename);

		mlog("rename : %s -> %s \n",pNode->m_name,_buf_rename);
	}
}

/////////////////////////////////////////////////////////////////////////////////

// ������ ��ο��� ���� ����Ʈ ���� 

bool GetDirList(char* path,	FFileList& pList)
{
	struct _finddata_t file_t;
	long hFile;

	FNode* pNode;

	if( (hFile = _findfirst( path , &file_t )) != -1L ) {
		do{
			if(strcmp(file_t.name, "." )==0)	continue;
			if(strcmp(file_t.name, "..")==0)	continue;
			if( !(file_t.attrib & _A_SUBDIR) )	continue;

			pNode = new FNode;
			pNode->SetName(file_t.name);
			pList.Add(pNode);

		} 
		while( _findnext( hFile, &file_t ) == 0 );

		_findclose(hFile);
	}

	return true;
}

// ������ ��ο��� ���丮 ����Ʈ ���� 

bool GetFileList(char* path,FFileList& pList)
{
	struct _finddata_t file_t;
	long hFile;

	FNode* pNode;

	if( (hFile = _findfirst( path , &file_t )) != -1L ) {
		do{
			if(strcmp(file_t.name, "." )==0) continue;
			if(strcmp(file_t.name, "..")==0) continue;
			if(file_t.attrib & _A_SUBDIR )	 continue;

			pNode = new FNode;
			pNode->SetName(file_t.name);
			pList.Add(pNode);

		} 
		while( _findnext( hFile, &file_t ) == 0 );

		_findclose(hFile);
	}

	return true;
}

bool GetFileListWin(char* path,FFileList& pList)
{

	WIN32_FIND_DATA		file_t;
	HANDLE				hFile;

	FNode* pNode;

	if( (hFile = FindFirstFile( path , &file_t )) != INVALID_HANDLE_VALUE ) {

		do {

			if(strcmp(file_t.cFileName, "." )==0)					continue;
			if(strcmp(file_t.cFileName, "..")==0)					continue;
			if(file_t.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )	continue;

			pNode = new FNode;
			pNode->SetName(file_t.cFileName);
			pList.Add(pNode);

		} while( FindNextFile( hFile, &file_t ) );

		FindClose(hFile);
	}

	return true;
}


// ��� ���������� ���ϴ� ���ϵ� �˻�..

bool GetFindFileList(char* path,char* ext,FFileList& pList)
{
	struct _finddata_t file_t;
	long hFile;

	FNode* pNode;

	if( (hFile = _findfirst( path , &file_t )) != -1L ) {
		do{
			if(strcmp(file_t.name, "." )==0) continue;
			if(strcmp(file_t.name, "..")==0) continue;

			if(file_t.attrib & _A_SUBDIR ) {
				char _path[256];
				strcpy(_path,file_t.name);
				strcat(_path,"/");
				strcat(_path,path);

				GetFindFileList(_path,ext,pList);
				continue;
			}

			int len = (int)strlen(ext);
			int filelen = (int)strlen(file_t.name);

			char* pName = &file_t.name[filelen-len];

			if(stricmp(pName,ext)==0) {

				int len = (int)strlen(path);

				char temp_name[256];

				if(len > 3) {

					strncpy(temp_name,path,len-3);
					temp_name[len-3]=0;
					strcat(temp_name,file_t.name);
				}
				else {
					strcpy(temp_name,file_t.name);
				}

				pNode = new FNode;
				pNode->SetName(temp_name);
				pList.Add(pNode);

			}

		} 
		while( _findnext( hFile, &file_t ) == 0 );

		_findclose(hFile);
	}

	return true;
}

bool GetFindFileListWin(char* path,char* ext,FFileList& pList)
{

	WIN32_FIND_DATA		file_t;
	HANDLE				hFile;

	FNode* pNode;

	if( (hFile = FindFirstFile( path , &file_t )) != INVALID_HANDLE_VALUE ) {

		do{
			if(strcmp(file_t.cFileName, "." )==0)	continue;
			if(strcmp(file_t.cFileName, "..")==0)	continue;

			if(file_t.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )	{
				char _path[1024];

				int len = strlen(path);

				strcpy(_path,path);
				_path[len-1] = 0;
				strcat(_path,file_t.cFileName);
				strcat(_path,"/*");

				GetFindFileListWin(_path,ext,pList);
				continue;
			}

			int len = (int)strlen(ext);
			int filelen = (int)strlen(file_t.cFileName);

			char* pName = &file_t.cFileName[filelen-len];

			if(stricmp(pName,ext)==0) {

				int len = (int)strlen(path);

				char temp_name[1024];

				if(len > 1) {

					strncpy(temp_name,path,len-1);
					temp_name[len-1]=0;
					strcat(temp_name,file_t.cFileName);
				}
				else {
					strcpy(temp_name,file_t.cFileName);
				}

				pNode = new FNode;
				pNode->SetName(temp_name);
				pList.Add(pNode);

			}

		} while( FindNextFile( hFile, &file_t ) );

		FindClose(hFile);
	}

	return true;
}
