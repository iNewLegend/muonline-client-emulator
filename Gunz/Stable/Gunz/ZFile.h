#ifndef _ZFILE_H
#define _ZFILE_H

#include <stdio.h>
#include "../sdk/zlib/include/zlib.h"

#define BUFFER_SIZE	1024


// TODO : �Է�/����� stream ���� �Ϲ�ȭ �Ͽ� 
//			CML & MZFileSystem �� MZFile�� ��ü�ϸ� ������.

// �Ѱ��� ������ �ܼ��� ����Ǿ��ִ� ����
class ZFile {

	FILE			*m_pFile;
	bool			m_bWrite;	// �����ִ� ������..
	z_stream		m_Stream;
	unsigned char	m_Buffer[BUFFER_SIZE];

public:
	ZFile();
	virtual ~ZFile();

	bool Open(const char *szFileName,bool bWrite = false);

	int Read(void *pBuffer,int nByte);
	int Write(const void *pBuffer,int nByte);

	bool Close();
};



ZFile *zfopen(const char *szFileName,bool bWrite = false);
int zfread(void *pBuffer,int nItemSize,int nItemCount,ZFile *pFile);
int zfwrite(const void *pBuffer,int nItemSize,int nItemCount,ZFile *pFile);
bool zfclose(ZFile *pFile);

#endif // of _ZREPLAYFILE