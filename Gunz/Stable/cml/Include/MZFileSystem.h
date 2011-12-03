#ifndef MZFileSYSTEM_H
#define MZFileSYSTEM_H

#include <list>
#include <map>
#include <string>
#include "MZip.h"

using namespace std;

#define DEF_EXT	"mrs"

// File Descrition ( by Zip File System )
struct MZFILEDESC{
	char	m_szFileName[_MAX_PATH];		// ���ϸ� ( ���� ������ ��쵵 �ְ�, zip ���Ͼȿ� ���� �ִ� ��쵵 �ִ�. )
	char	m_szZFileName[_MAX_PATH];		// "" �̸� ���� ������ �ǹ��Ѵ�.
	int		m_iSize;
	unsigned int	m_crc32;
	unsigned long	m_modTime;	// zip ���� �ȿ� �ִ� ���ϵ��� ���� dos time ���� �����Ѵ�
};

typedef map<string,MZFILEDESC*>	ZFLIST;
typedef ZFLIST::iterator				ZFLISTITOR;

// iterator �� �����������Ƿ� create �� ���Ŀ� ���ϸ���� �߰��Ǹ� ����ϴ�. �׷���� �ڵ� ������ �ʿ�

void GetRefineFilename(char *szRefine, const char *szSource);

// ���ϸ����� ����� �� ���� ���ڰ� ���ԵǾ� ������ ��ü���ڷ� ��ü�Ѵ�. ������ ���ϸ� �Ծ��� ���Ұ����� �ܿ��� ���������� �� �����Ƿ�
// �� �Լ��� ���ĵ� ���ϸ� �Ծ࿡ ������ �����Ѵٴ� ������ ���ٴ� ���� ����
void ValidateFilename(char* szOut, const char* szSrc, char substitute);

unsigned MGetCRC32(const char *data, int nLength);
class MZFileCheckList;

class MZFileSystem{
	char		m_szBasePath[256];
	ZFLIST		m_ZFileList;
	ZFLISTITOR	m_iterator;
	int			m_nIndex;

	char		m_szUpdateName[256];

	MZFileCheckList *m_pCheckList;

protected:
	bool AddItem(MZFILEDESC*);

	void RemoveFileList(void);
	// RefreshFileList Pure File List
	void RefreshFileList(const char* szBasePath);
	// Expand Z File List From File List
//	void ExpandZFileList(ZFLIST* pZFList);
	// '\' -> '/'

	int GetUpdatePackageNumber(const char *szPackageFileName);

public:
	MZFileSystem(void);
	virtual ~MZFileSystem(void);

	bool Create(const char* szBasePath,const char* szUpdateName=NULL);
	void Destroy(void);

	int GetFileCount(void) const;
	const char* GetFileName(int i);
	const MZFILEDESC* GetFileDesc(int i);

	const char *GetBasePath() { return m_szBasePath; }

	MZFILEDESC* GetFileDesc(const char* szFileName);

	unsigned int GetCRC32(const char* szFileName);
	unsigned int GetTotalCRC();

	int GetFileLength(const char* szFileName);
	int GetFileLength(int i);

	bool IsZipFile(const char* szFileName);

	bool ReadFile(const char* szFileName, void* pData, int nMaxSize);

	void SetFileCheckList(MZFileCheckList *pCheckList) { m_pCheckList = pCheckList; }
	MZFileCheckList *GetFileCheckList()	{ return m_pCheckList; }


	void SetPrivateKey( const unsigned char* pPrivateKey, size_t length);
};


class MZFile{
public:
	enum SeekPos { begin = 0x0, current = 0x1, end = 0x2 };

protected:
	FILE*	m_fp;
	bool	m_IsZipFile;
	bool	m_IsBufferd;
	bool	m_IsEncrypted;

	MZip	m_Zip;

	char*	m_pData;
	unsigned long	m_nFileSize;
	unsigned long	m_nEncryptFileSize;

	int		m_nPos;

	int		m_nIndexInZip;
	unsigned int	m_crc32;

	char	m_FileName[256];
	char	m_ZipFileName[256];

	static  unsigned long m_dwReadMode;

public:

	MZFile();
	virtual ~MZFile();

	bool Create();

//	bool Open(const char* szFileName);		// �� ����� �׳� ���Ͽ��� ���� �ϴ� ������� �Ʒ� ��ǿ� ��ģ��.
	bool Open(const char* szFileName, MZFileSystem* pZFS = NULL);
	bool Open(const char* szFileName, const char* szZipFileName, bool bFileCheck = false , unsigned int crc32 = 0);

	bool Seek(long off,int mode);

	void Close(void);

	static void SetReadMode(unsigned long mode) {	m_dwReadMode = mode; }
	static unsigned long GetReadMode(void)  { return m_dwReadMode; }
	static bool isMode(unsigned long mode ) { return (m_dwReadMode & mode) ? true : false ; }

//	bool OpenZip(char* szZipFileName);
//	void CloseZip(void);

	unsigned long GetLength(void);
	bool Read(void* pBuffer, int nMaxSize);

	// �ѹ��� �ٷ� �о�´�. ���ϳ��� seek ��ġ�ʹ� ����� ����
//	bool ReadAll(void* pBuffer, int nBufferSize);
};

class MZFileCheckList
{
	unsigned int				m_crc32;		// ���� ��ü�� crc32
	map<string,unsigned int>	m_fileList;

public:
	bool Open(const char *szFileName, MZFileSystem *pfs = NULL);

	unsigned int GetCRC32(const char *szFileName);	// Ư�������� crc32�� ����
	unsigned int GetCRC32()	{ return m_crc32; }		// �������� crc32�� ����
};

#endif

/*
	////	���� ����		////

	MZFileSystem fs;

	fs.Create("./");

	char buffer[1000];

	MZFile mzf;

	mzf.Open("6.txt");

	int size = mzf.GetLength();
	mzf.Read(buffer,size);

	mzf.Close();

	mzf.Open("5.txt","test2.zip");
//	mzf.Read(buffer,mzf.GetLength());
	mzf.Read(buffer,4);
	mzf.Read((buffer+4),6);

	mzf.Seek(10,MZFile::begin);
	mzf.Read(buffer,4);

	mzf.Seek(2,MZFile::current);
	mzf.Read(buffer,4);

	mzf.Seek(-2,MZFile::end);
	mzf.Read(buffer,4);

	mzf.Open("4.txt","test2.zip");
	mzf.Read(buffer,mzf.GetLength());

	mzf.Close();

	mzf.Open("5.txt",&fs);
	mzf.Read(buffer,mzf.GetLength());

	mzf.Open("4.txt",&fs);
	mzf.Read(buffer,mzf.GetLength());

	mzf.Open("6.txt",&fs);
	mzf.Read(buffer,mzf.GetLength());

	mzf.Close();

	fs.Destroy();
	*/