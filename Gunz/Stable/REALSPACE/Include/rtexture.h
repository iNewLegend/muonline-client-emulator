#ifndef __RTEXTURE_H
#define __RTEXTURE_H

#include <stdlib.h>
#include <stdio.h>

typedef unsigned long       DWORD;
typedef unsigned short      WORD;
typedef unsigned char       BYTE;

enum RTEXTUREFORMAT
{
	RTEXTUREFORMAT_24,
	RTEXTUREFORMAT_32
};

struct rtexture_header
{
	int sx,sy;
	RTEXTUREFORMAT fmt;
};

class rtexture
{
public:
	rtexture();
	virtual ~rtexture();

	void New(int x,int y,RTEXTUREFORMAT fm);
	void Create(int x,int y,void* colorbuf,void* alphabuf=NULL);
	bool Create(FILE *file);
	void Create(rtexture *source);
	bool CreateAsHalf(rtexture *source);
	bool CreateAsDouble(rtexture *source);
	bool CreateAsCopy(rtexture *source,int sx,int sy,int width,int height,bool bFlipU=false,bool bFlipV=false);
	bool CreateFromBMP(const char *filename);
	void Destroy();

	bool Save(FILE *file);
	bool SaveAsBMP(const char *filename);

	int	GetWidth() { return x; }
	int GetHeight() { return y; }
	RTEXTUREFORMAT GetFormat() { return format; }
	void *GetData() { return data; }
	int GetBytesPerPixel();
	int GetDataSize();

	// ��ü�� �ѻ�����.
	void Fill(DWORD color);
	
	// ��輱 ĥ�ϱ�
	void FillBoundary(DWORD color);

	// source�� �о ĥ�ϴ� ���. Ŭ���ε��������� ����.
	void Fill(int dx,int dy,int nDir,rtexture *source);
	
	// Ư���� �׸��� �ؽ��� �������� ����.
	void FillTriangle(float sx0,float sy0,float sx1,float sy1,float sx2,float sy2,DWORD color);

	void SetPixel(int x,int y,DWORD color);
	DWORD GetPixel(int x,int y);

private:
	int x,y;
	void *data;
	RTEXTUREFORMAT format;
};

#endif