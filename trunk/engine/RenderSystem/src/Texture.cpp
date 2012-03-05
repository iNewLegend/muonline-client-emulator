#include "Texture.h"
#include "IORead.h"
#include "Color.h"
#include "ddslib.h"
#include "RenderSystem.h"

void DecompressDXTC(int formatDXT, int w, int h, size_t size, unsigned char *src, unsigned char *dest)
{
	// sort of copied from linghuye
	int bsx = (w<4) ? w : 4;
	int bsy = (h<4) ? h : 4;

	/*
	if (format == GL_COMPRESSED_RGBA_S3TC_DXT1_EXT) {
	DDSDecompressDXT1(src, w, h, dest);
	return;
	}
	*/

	if (formatDXT == 3)//D3DFMT_DXT3
	{
		DDSDecompressDXT3(src, w, h, dest);
		for(int y=0; y<h; y ++) {
			for(int x=0; x<w; x ++) {
				unsigned int* pixel = (unsigned int*) (dest + (x + y * w)*4);
				*pixel = (*pixel&0xFF000000) | ((*pixel&0x00FF0000)>>16) | ((*pixel&0x0000FF00)) | ((*pixel& 0x000000FF)<<16);
			}
		}
		return;
	}

	/*
	if (format == GL_COMPRESSED_RGBA_S3TC_DXT5_EXT)	{
	DXT5UnpackAlphaValues(src, w, h, dest);
	return;
	} 
	*/

	for(int y=0; y<h; y += bsy) {
		for(int x=0; x<w; x += bsx) {
			//unsigned long alpha = 0;
			//unsigned int a0 = 0, a1 = 0;

			unsigned int c0 = *(unsigned short*)(src + 0);
			unsigned int c1 = *(unsigned short*)(src + 2);
			src += 4;

			Color32 color[4];
			color[0].b = (unsigned char) ((c0 >> 11) & 0x1f) << 3;
			color[0].g = (unsigned char) ((c0 >>  5) & 0x3f) << 2;
			color[0].r = (unsigned char) ((c0) & 0x1f) << 3;
			color[1].b = (unsigned char) ((c1 >> 11) & 0x1f) << 3;
			color[1].g = (unsigned char) ((c1 >>  5) & 0x3f) << 2;
			color[1].r = (unsigned char) ((c1) & 0x1f) << 3;

			if(c0 > c1 || formatDXT == 3)
			{
				color[2].r = (color[0].r * 2 + color[1].r) / 3;
				color[2].g = (color[0].g * 2 + color[1].g) / 3;
				color[2].b = (color[0].b * 2 + color[1].b) / 3;
				color[3].r = (color[0].r + color[1].r * 2) / 3;
				color[3].g = (color[0].g + color[1].g * 2) / 3;
				color[3].b = (color[0].b + color[1].b * 2) / 3;
			}
			else
			{
				color[2].r = (color[0].r + color[1].r) / 2;
				color[2].g = (color[0].g + color[1].g) / 2;
				color[2].b = (color[0].b + color[1].b) / 2;
				color[3].r = 0;
				color[3].g = 0;
				color[3].b = 0;
			}

			for (int j=0; j<bsy; j++)
			{
				unsigned int index = *src++;
				unsigned char* dd = dest + (w*(y+j)+x)*4;
				for (int i=0; i<bsx; i++) {
					*dd++ = color[index & 0x03].r;
					*dd++ = color[index & 0x03].g;
					*dd++ = color[index & 0x03].b;
					//if (format == GL_COMPRESSED_RGBA_S3TC_DXT1_EXT)	{
					*dd++ = ((index & 0x03) == 3 && c0 <= c1) ? 0 : 255;
					//}
					index >>= 2;
				}
			}
		}
	}
}

void Decompress256Palette(int w, int h, size_t alphaBits, void* src, void* pal, void* dest)
{
	int cnt = 0;
	int alpha = 0;

	unsigned int* p = static_cast<unsigned int*>(dest);
	unsigned char *c = static_cast<unsigned char*>(src);
	unsigned char *a = c + w*h;

	for (int y=0; y<h; y++)
	{
		for (int x=0; x<w; x++)
		{
			unsigned int k = static_cast<unsigned int*>(pal)[*c++];

			k = ((k&0x00FF0000)>>16) | ((k&0x0000FF00)) | ((k& 0x000000FF)<<16);

			if (alphaBits!=0)
			{
				if (alphaBits == 8)
				{
					alpha = (*a++);
				}
				else if (alphaBits == 1)
				{
					alpha = (*a & (1 << cnt++)) ? 0xff : 0;
					if (cnt == 8)
					{
						cnt = 0;
						a++;
					}
				}
			}
			else
			{
				alpha = 0xff;
			}
			k |= alpha << 24;
			*p++ = k;
		}
	}
}

bool CTexture::createFromBLP(void* pBuf, int nSize, int nLevels)
{
	// BLP
	int offsets[16], sizes[16], w, h;
	int format;
	char attr[4];

	unsigned int* pointer = static_cast<unsigned int*>(pBuf);
	pointer+=2;
	memcpy(attr,pointer,4);
	pointer++;
	memcpy(&w,pointer,4);
	pointer++;
	memcpy(&h,pointer,4);
	pointer++;

	memcpy(offsets,pointer,4*16);
	pointer+=16;
	memcpy(sizes,pointer,4*16);
	pointer+=16;

	bool hasmipmaps = attr[3]>0;
	int mipmax = hasmipmaps ? 16 : 1;

	createTexture(w, h, mipmax, true);
	if (attr[0] == 2) // compressed
	{
		bool supportCompression = false;
		unsigned char *ucbuf = NULL;
		if (!supportCompression)
		{
			ucbuf = new unsigned char[w*h*4];
		}
		format = 1;//D3DFMT_DXT1;///GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
		int blocksize = 8;
		// guesswork here :(
		if (attr[1]==8)
		{
			format = 3;//D3DFMT_DXT3;//GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
			blocksize = 16;
		}
		else
		{
			if (!attr[3]) 
				format = 1;//D3DFMT_DXT1;//GL_COMPRESSED_RGB_S3TC_DXT1_EXT;
		}
		// do every mipmap level
		for (int i=0; i<mipmax; i++)
		{
			if (w==0) w = 1;
			if (h==0) h = 1;
			if (offsets[i] && sizes[i])
			{
				unsigned char* buf = static_cast<unsigned char*>(pBuf)+offsets[i];
				int nSize = ((w+3)/4) * ((h+3)/4) * blocksize;
				if (supportCompression)
				{
					//D3DXCreateTextureFromFileInMemoryEx(DXUTGetD3DDevice(),buf,nSize,w, h,0,0,
					//	format,
					//	D3DPOOL_MANAGED,D3DX_FILTER_BOX, D3DX_DEFAULT,0, NULL, NULL, &m_pd3dTexture);
					//D3DXCreateTextureFromFile(DXUTGetD3DDevice(), buf, nSize, &m_pd3dTexture);
					//glCompressedTexImage2DARB(GL_TEXTURE_2D, i, format, w, h, 0, nSize, buf);
				}
				else
				{
					DecompressDXTC(format, w, h, nSize, buf, ucbuf);
					FillLevel(ucbuf, nSize, w, h, i);
				}
			}
			else
			{
				break;
			}
			w >>= 1;
			h >>= 1;
		}
		if (!supportCompression) 
		{
			delete[] ucbuf;
		}
	}
	else if (attr[0]==1) // uncompressed
	{
		unsigned int* pal = pointer;
		unsigned int *buf2 = new unsigned int[w*h];
		int nSize = w*h*4;
		for (int i=0; i<mipmax; i++)
		{
			if (w==0) w = 1;
			if (h==0) h = 1;
			if (offsets[i] && sizes[i])
			{
				unsigned char* buf = static_cast<unsigned char*>(pBuf)+offsets[i];
				Decompress256Palette(w,h,attr[1],buf, pal,buf2);
				FillLevel(buf2, nSize, w, h, i);

			} else break;
			w >>= 1;
			h >>= 1;
		}
		delete[] buf2;
	}
	// ´¢´æ
	//{
	//	std::string strTempTex("TempTex\\");
	//	strTempTex.append(m_strName);
	//	strTempTex.append(".bmp");
	//	for (int i = 9; i<strTempTex.length(); i++)
	//	{
	//		if (strTempTex[i]=='\\')
	//		{
	//			strTempTex[i]='_';
	//		}
	//	}
	//	SaveToFile(strTempTex);
	//}
	return true;
}

CTexture::CTexture():
m_bPoolManaged(true),
m_bLoaded(false),
m_nLevels(0),
m_nWidth(0),
m_nHeight(0),
m_eTexType(TEX_TYPE_UNKNOWN),
m_pTextureMgr(NULL)
{
}

CTexture::~CTexture(void)
{
	if (m_pTextureMgr)
	{
		m_pTextureMgr->remove(this);
	}
}

void CTexture::load()
{
	if (m_strFilename.length()>0)
	{
		createTextureFromFile(m_strFilename,m_nLevels);
		m_bLoaded = true;
	}
}
void CTexture::setTextureMgr(CTextureMgr* pTextureMgr)
{
	m_pTextureMgr = pTextureMgr;
}

#include "FileSystem.h"
bool CTexture::createTextureFromFile(const std::string& strFilename, int nLevels)
{
	IOReadBase* pRead = IOReadBase::autoOpen(strFilename);
	if (pRead)
	{
		char* pBuf = new char[pRead->GetSize()];
		if (pBuf)
		{
			pRead->Read(pBuf, pRead->GetSize());
			std::string strExtension = GetExtension(strFilename); 

			if (".blp"==strExtension)
				createFromBLP(pBuf, pRead->GetSize(), nLevels);
			else if (".ozj"==strExtension)
				createTextureFromFileInMemory(pBuf+24, pRead->GetSize()-24, nLevels);
			else if (".ozt"==strExtension)
				createTextureFromFileInMemory(pBuf+4, pRead->GetSize()-4, nLevels);
			else if (".ozb"==strExtension)
				createTextureFromFileInMemory(pBuf+4, pRead->GetSize()-4, nLevels);
			else
				createTextureFromFileInMemory(pBuf, pRead->GetSize(), nLevels);
	
			delete[] pBuf;
		}
		IOReadBase::autoClose(pRead);
		return true;
	}
	return false;
}

void CTexture::createTextureFromMemory(void* pBuf, int nSize, int nWidth, int nHeight, int nLevels)
{
	createTexture(nWidth, nHeight, nLevels, true);
	FillLevel(pBuf, nSize, nWidth, nHeight, 0);
	Filter(0, m_nLevels);
}

void CTexture::Filter(int nSrcLevel, int nFilter)
{
	//int w = nWidth;
	//int n = nHeight;
	//unsigned long* buffer = new unsigned long[w*n];
	//for (int i=1; i<16; i++)
	//{
	//	unsigned long* p = buffer;
	//	w >>= 1;
	//	h >>= 1;
	//	if (w==0) w = 1;
	//	if (h==0) h = 1;
	//	for(int y=0; y<h; y++)
	//	{
	//		for(int x=0; x<w; x++)
	//		{
	//			p=p[x]
	//		}
	//	}
	//}
}

void CTexture::reset()
{
	releaseBuffer();
	switch(m_eTexType)
	{
	case TEX_TYPE_FORM_FILE:
		createTextureFromFile(m_strFilename, m_nLevels);
		break;
	case TEX_TYPE_NORMAL:
		createTexture(m_nWidth, m_nHeight, m_nLevels, m_bPoolManaged);
		break;
	case TEX_TYPE_DYNAMIC:
		createDynamicTexture(m_nWidth, m_nHeight);
		break;
	case TEX_TYPE_CUBETEXTURE:
		createTexture(m_nWidth, m_nHeight, m_nLevels, m_bPoolManaged);
		break;
	case TEX_TYPE_RENDERTARGET:
		createRenderTarget(m_nWidth, m_nHeight);
		break;
	default:
		break;
	}
}