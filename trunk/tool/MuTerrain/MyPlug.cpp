#include "MyPlug.h"
#include "IORead.h"
#include "FileSystem.h"
#include "3DMapSceneObj.h"

extern "C" {
#include "jpeg\jpeglib.h"
}

__declspec(dllexport) bool Data_Plug_CreateObject(void ** pobj){
	*pobj = new CMyPlug;
	return *pobj != NULL;
}

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

bool CMyPlug::importTerrainData(iTerrainData * pTerrainData, const std::string& strFilename)
{
	pTerrainData->clear();
	if (pTerrainData->resize(253,253,11))
	{
		// EncTerrain
		IOReadBase* pRead = IOReadBase::autoOpen(strFilename);
		if (pRead)
		{
			if (MAP_FILE_SIZE==pRead->GetSize())
			{
				char buffer[MAP_FILE_SIZE];
				pRead->Read(buffer,MAP_FILE_SIZE);
				decrypt(buffer,MAP_FILE_SIZE);
				char* p = buffer;
				unsigned short uMuFlgMap = *((unsigned short*)p);
				p+=2;
				for (int y=0; y<253; ++y)
				{
					for (int x=0; x<253; ++x)
					{
						pTerrainData->setCellTileID(x,y,*p,0);
						p++;
					}
					p+=3;
				}
				p+=256*3;
				for (int y=0; y<253; ++y)
				{
					for (int x=0; x<253; ++x)
					{
						pTerrainData->setCellTileID(x,y,*p,1);
						p++;
					}
					p+=3;
				}
				p+=256*3;
				for (int y=0; y<254; ++y)
				{
					for (int x=0; x<254; ++x)
					{
						pTerrainData->setVertexColor(x,y,Color32(*p,255,255,255));
						p++;
					}
					p+=2;
				}
			}
			IOReadBase::autoClose(pRead);
		}
		// EncTerrain.att
		pRead = IOReadBase::autoOpen(ChangeExtension(strFilename,".att"));
		if (pRead)
		{
			if (ATT_FILE_129KB_SIZE==pRead->GetSize())
			{
				char buffer[ATT_FILE_129KB_SIZE];
				pRead->Read(buffer,ATT_FILE_129KB_SIZE);
				decrypt(buffer,ATT_FILE_129KB_SIZE);
				decrypt2(buffer,ATT_FILE_129KB_SIZE);
				char* p = buffer;
				unsigned long uMuFlgAtt = *((unsigned long*)p);
				p+=4;
				for (int y=0; y<253; ++y)
				{
					for (int x=0; x<253; ++x)
					{
						pTerrainData->setCellAttribute(x,y,*p);
						p+=2;
					}
					p+=6;
				}
			}
			else if (ATT_FILE_65KB_SIZE==pRead->GetSize())
			{
				char buffer[ATT_FILE_65KB_SIZE];
				pRead->Read(buffer,ATT_FILE_65KB_SIZE);
				decrypt(buffer,ATT_FILE_65KB_SIZE);
				decrypt2(buffer,ATT_FILE_65KB_SIZE);
				char* p = buffer;
				unsigned long uMuFlgAtt = *((unsigned long*)p);
				p+=4;
				for (int y=0; y<253; ++y)
				{
					for (int x=0; x<253; ++x)
					{
						pTerrainData->setCellAttribute(x,y,*p);
						p++;
					}
					p+=3;
				}
			}
			IOReadBase::autoClose(pRead);
		}
		// TerrainLight
		std::string strTerrainLight = GetParentPath(strFilename)+"TerrainLight.ozj";
		pRead = IOReadBase::autoOpen(strTerrainLight);
		if (pRead)
		{
			size_t uFileSize = pRead->GetSize();
			char* buffer = new char[uFileSize];
			pRead->Read(buffer,uFileSize);

			//////////////////////////////////////////////////////////////////////////
			// 声明并初始化解压缩对象，同时制定错误信息管理器
			struct jpeg_decompress_struct cinfo;
			struct jpeg_error_mgr jerr;

			cinfo.err = jpeg_std_error(&jerr);
			jpeg_create_decompress(&cinfo);

			//////////////////////////////////////////////////////////////////////////
			// 打开jpg图像文件，并指定为解压缩对象的源文件
			jpeg_stdio_src(&cinfo, buffer+OZJ_HEAD_SIZE, uFileSize-OZJ_HEAD_SIZE);

			//////////////////////////////////////////////////////////////////////////
			// 读取图像信息
			jpeg_read_header(&cinfo, TRUE);

			//////////////////////////////////////////////////////////////////////////
			// 根据图像信息申请一个图像缓冲区
			BYTE* data = new BYTE[cinfo.image_width*cinfo.image_height*cinfo.num_components];
			//////////////////////////////////////////////////////////////////////////
			// 开始解压缩
			jpeg_start_decompress(&cinfo);

			JSAMPROW row_pointer[1];
			while (cinfo.output_scanline < cinfo.output_height)
			{
				row_pointer[0] = &data[(cinfo.output_height - cinfo.output_scanline-1)*cinfo.image_width*cinfo.num_components];
				jpeg_read_scanlines(&cinfo,row_pointer ,
					1);
			}
			jpeg_finish_decompress(&cinfo);

			//////////////////////////////////////////////////////////////////////////
			// 释放资源
			jpeg_destroy_decompress(&cinfo);
			delete[] buffer;
			IOReadBase::autoClose(pRead);

			unsigned char* pImg = (unsigned char*)data;
			for (int y=0; y<254; ++y)
			{
				for (int x=0; x<254; ++x)
				{
					Color32 c = pTerrainData->getVertexColor(x,y);
					c.r = *pImg;
					pImg++;
					c.g = *pImg;
					pImg++;
					c.b = *pImg;
					pImg++;
					pTerrainData->setVertexColor(x,y,c);
				}
				pImg+=2*3;
			}
			delete[] data;
		}

		// TerrainHeight
		std::string strHeightFilename = GetParentPath(strFilename)+"TerrainHeight.ozb";
		pRead = IOReadBase::autoOpen(strHeightFilename);
		if (pRead)
		{
			if (HEIGHT_HEAD_SIZE+HEIGHT_BUFFER_SIZE<=pRead->GetSize())
			{
				pRead->Move(HEIGHT_HEAD_SIZE);
				for (int y=0; y<254; ++y)
				{
					pRead->Move(2);
					for (int x=0; x<254; ++x)
					{
						unsigned char uVal;
						pRead->Read(&uVal,1);
						pTerrainData->setVertexHeight(x,y,uVal*0.015f);
					}
				}
			}
			IOReadBase::autoClose(pRead);
		}
	}
	return true;
}

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
	iTerrainData* pTerrainData = (iTerrainData*)m_pRenderNodeMgr->createRenderData("terrain",szFilename);
	importTerrainData(pTerrainData,szFilename);
	const char* szTerrainMaterial[21][3]={
		{"Terrain.0_0","TileGrass01.ozj","terrainTileX4"},
		{"Terrain.0_1","TileGrass02.ozj","terrainTileX2"},
		{"Terrain.0_2","TileGround01.ozj","terrainTileX4"},
		{"Terrain.0_3","TileGround02.ozj","terrainTileX2"},
		{"Terrain.0_4","TileGround03.ozj","terrainTileX2"},
		{"Terrain.0_5","TileWater01.ozj","terrainWater"},
		{"Terrain.0_6","TileWood01.ozj","terrainTileX2"},
		{"Terrain.0_7","TileRock01.ozj","terrainTileX2"},
		{"Terrain.0_8","TileRock02.ozj","terrainTileX2"},
		{"Terrain.0_9","TileRock03.ozj","terrainTileX2"},
		{"Terrain.1_0","TileGrass01.ozj","terrainTileAlphaX4"},
		{"Terrain.1_1","TileGrass02.ozj","terrainTileAlphaX2"},
		{"Terrain.1_2","TileGround01.ozj","terrainTileAlphaX4"},
		{"Terrain.1_3","TileGround02.ozj","terrainTileAlphaX2"},
		{"Terrain.1_4","TileGround03.ozj","terrainTileAlphaX2"},
		{"Terrain.1_5","TileWater01.ozj","terrainWaterAlpha"},
		{"Terrain.1_6","TileWood01.ozj","terrainTileAlphaX2"},
		{"Terrain.1_7","TileRock01.ozj","terrainTileAlphaX2"},
		{"Terrain.1_8","TileRock02.ozj","terrainTileAlphaX2"},
		{"Terrain.1_9","TileRock03.ozj","terrainTileAlphaX2"},
		{"Terrain.Grass","TileGrass01.OZT","grass"}
	};
	for (int i=0; i<21; ++i)
	{
		CMaterial* pMaterial = (CMaterial*)m_pRenderNodeMgr->createRenderData("material",szTerrainMaterial[i][0]);
		if(pMaterial)
		{
			char szTexture[256];
			sprintf(szTexture,"%s%s",GetParentPath(szFilename).c_str(),szTerrainMaterial[i][1]);
			pMaterial->setTexture(0,szTexture);
			char szShaderFilename[255];
			sprintf(szShaderFilename,"EngineRes\\fx\\%s.fx",szTerrainMaterial[i][2]);
			pMaterial->setShader(szShaderFilename);
		}
	}
	// tiles
	//std::string strTileFile = GetParentPath(szFilename)+"Tile.csv";
	//if (!IOReadBase::Exists(strTileFile))
	//{
	//	strTileFile="Plugins\\Data\\default\\Tile.csv";
	//}
	//pScene->getTerrainData()->create();
	pRenderNode->init(pTerrainData);
	return true;
}

bool CMyPlug::exportTerrainAtt(iTerrainData * pTerrainData, const std::string& strFilename)
{
	// Calc MU's map id from filename.
	int nMapID = getMapIDFromFilename(strFilename);
	// att
	// for server.att
	std::string strServerAttFile = GetParentPath(strFilename)+"(Server)Terrain"+ws2s(i2ws(nMapID))+".att";
	FILE* f=fopen(strServerAttFile.c_str(),"wb+");
	if (f)
	{
		char buffer[ATT_FILE_SERVER_SIZE];
		char* p = buffer;
		*((unsigned char*)p)=0x0;++p;
		*((unsigned char*)p)=0xFF;++p;
		*((unsigned char*)p)=0xFF;++p;
		for (int y=0; y<253; ++y)
		{
			for (int x=0; x<253; ++x)
			{
				*p = pTerrainData->getCellAttribute(x,y);
				p++;
			}
			for (int x=253; x<256; ++x)
			{
				*p =0;++p;
			}
		}
		for (int x=0; x<256*3; ++x)
		{
			*p =0;++p;
		}
		fwrite(buffer,ATT_FILE_SERVER_SIZE,1,f);
		fclose(f);
	}
	// EncTerrain.att
	std::string strEncTerrain=ChangeExtension(strFilename,".att");
	bool bAttFileSize128=true;
	{
		IOReadBase* pRead = IOReadBase::autoOpen(strEncTerrain);
		if (pRead)
		{
			bAttFileSize128 = (ATT_FILE_129KB_SIZE==pRead->GetSize());
			IOReadBase::autoClose(pRead);
		}
		else
		{
			bAttFileSize128 = (MessageBoxA(NULL,"Select the correct file size.\"OK\" as 128KB; \"Cancel\" as 64KB.",
				"Saving EncTerrain.att!",1)==1);
		}
	}
	f=fopen(strEncTerrain.c_str(),"wb+");
	if (f)
	{
		if (bAttFileSize128)
		{
			char buffer[ATT_FILE_129KB_SIZE];
			char* p = buffer;
			*((unsigned char*)p)=0x0;++p;
			*((unsigned char*)p)=nMapID;++p;
			*((unsigned char*)p)=0xFF;++p;
			*((unsigned char*)p)=0xFF;++p;
			{
				for (int y=0; y<253; ++y)
				{
					for (int x=0; x<253; ++x)
					{
						*p = pTerrainData->getCellAttribute(x,y);
						p++;
						*p =0;++p;
					}
					for (int x=253; x<256; ++x)
					{
						*p =0;++p;
						*p =0;++p;
					}
				}
				for (int x=0; x<256*6; ++x)
				{
					*p =0;++p;
				}
			}
			decrypt2(buffer,ATT_FILE_129KB_SIZE);
			encrypt(buffer,ATT_FILE_129KB_SIZE);
			fwrite(buffer,ATT_FILE_129KB_SIZE,1,f);
		}
		else
		{
			char buffer[ATT_FILE_65KB_SIZE];
			char* p = buffer;
			*((unsigned char*)p)=0x0;++p;
			*((unsigned char*)p)=nMapID;++p;
			*((unsigned char*)p)=0xFF;++p;
			*((unsigned char*)p)=0xFF;++p;
			{
				for (int y=0; y<253; ++y)
				{
					for (int x=0; x<253; ++x)
					{
						*p = pTerrainData->getCellAttribute(x,y);
						p++;
					}
					for (int x=253; x<256; ++x)
					{
						*p =0;++p;
					}
				}
				for (int x=0; x<256*3; ++x)
				{
					*p =0;++p;
				}
			}
			decrypt2(buffer,ATT_FILE_65KB_SIZE);
			encrypt(buffer,ATT_FILE_65KB_SIZE);
			fwrite(buffer,ATT_FILE_65KB_SIZE,1,f);
		}
		fclose(f);
	}
	//////////////////////////////////////////////////////////////////////////
	f=fopen(ChangeExtension(strFilename,".64KB.att").c_str(),"wb+");
	if (f)
	{
		char buffer[ATT_FILE_65KB_SIZE];
		char* p = buffer;
		*((unsigned char*)p)=0x0;++p;
		*((unsigned char*)p)=nMapID;++p;
		*((unsigned char*)p)=0xFF;++p;
		*((unsigned char*)p)=0xFF;++p;
		{
			for (int y=0; y<253; ++y)
			{
				for (int x=0; x<253; ++x)
				{
					*p = pTerrainData->getCellAttribute(x,y);
					p++;
				}
				for (int x=253; x<256; ++x)
				{
					*p =0;++p;
				}
			}
			for (int x=0; x<256*3; ++x)
			{
				*p =0;++p;
			}
		}
		decrypt2(buffer,ATT_FILE_65KB_SIZE);
		encrypt(buffer,ATT_FILE_65KB_SIZE);
		fwrite(buffer,ATT_FILE_65KB_SIZE,1,f);
		fclose(f);
	}

	f=fopen(ChangeExtension(strFilename,".128KB.att").c_str(),"wb+");
	if (f)
	{
		char buffer[ATT_FILE_129KB_SIZE];
		char* p = buffer;
		*((unsigned char*)p)=0x0;++p;
		*((unsigned char*)p)=nMapID;++p;
		*((unsigned char*)p)=0xFF;++p;
		*((unsigned char*)p)=0xFF;++p;
		{
			for (int y=0; y<253; ++y)
			{
				for (int x=0; x<253; ++x)
				{
					*p = pTerrainData->getCellAttribute(x,y);
					p++;
					*p =0;++p;
				}
				for (int x=253; x<256; ++x)
				{
					*p =0;++p;
					*p =0;++p;
				}
			}
			for (int x=0; x<256*6; ++x)
			{
				*p =0;++p;
			}
		}
		decrypt2(buffer,ATT_FILE_129KB_SIZE);
		encrypt(buffer,ATT_FILE_129KB_SIZE);
		fwrite(buffer,ATT_FILE_129KB_SIZE,1,f);

		fclose(f);
	}
	return true;
}

bool CMyPlug::exportTerrainLightmap(iTerrainData * pTerrainData, const std::string& strFilename)
{
	unsigned char buffer[LIGHT_MAP_SIZE];
	unsigned char* p = buffer;
	for (int x=0; x<256*2*3; ++x)
	{
		*p =0;++p;
	}
	for (int y=253; y>=0; --y)
	{

		for (int x=0; x<254; ++x)
		{
			Color32 c = pTerrainData->getVertexColor(x,y);
			*p = c.r;
			p++;
			*p = c.g;
			p++;
			*p = c.b;
			p++;
		}
		for (int i=0;i<6;++i)
		{
			*p =0;++p;
		}
	}

	{
		// 申请并初始化jpeg压缩对象，同时要指定错误处理器
		struct jpeg_compress_struct jcs;

		// 声明错误处理器，并赋值给jcs.err域
		struct jpeg_error_mgr jem;
		jcs.err = jpeg_std_error(&jem);

		jpeg_create_compress(&jcs);

		// 指定压缩后的图像所存放的目标
		char jpgBuffer[LIGHT_MAP_SIZE];
		int nJpgSize = 0;
		jpeg_stdio_dest(&jcs, jpgBuffer, &nJpgSize);
		// 设置压缩参数，主要参数有图像宽、高、色彩通道数（１：索引图像，３：其他），色彩空间（JCS_GRAYSCALE表示灰度图，JCS_RGB表示彩色图像），压缩质量等，如下：
		jcs.image_width = 256;    // 为图的宽和高，单位为像素 
		jcs.image_height = 256;
		jcs.input_components = 3;   // 在此为1,表示灰度图， 如果是彩色位图，则为3 
		jcs.in_color_space = JCS_RGB; //JCS_GRAYSCALE表示灰度图，JCS_RGB表示彩色图像 

		jpeg_set_defaults(&jcs); 
		jpeg_set_quality (&jcs, 80, true);
		// 上面的工作准备完成后，就可以压缩了，
		// 压缩过程非常简单，首先调用jpeg_start_compress，然后可以对每一行进行压缩，也可以对若干行进行压缩，甚至可以对整个的图像进行一次压缩，
		// 压缩完成后，记得要调用jpeg_finish_compress函数，如下：

		jpeg_start_compress(&jcs, TRUE);

		JSAMPROW row_pointer[1];   // 一行位图
		int row_stride = jcs.image_width*3;  // 每一行的字节数 如果不是索引图,此处需要乘以3

		// 对每一行进行压缩
		while (jcs.next_scanline < jcs.image_height)
		{
			row_pointer[0] = & buffer[jcs.next_scanline * row_stride];
			jpeg_write_scanlines(&jcs, row_pointer, 1);
		}

		jpeg_finish_compress(&jcs);

		if (LIGHT_MAP_SIZE<nJpgSize)
		{
			MessageBoxA(NULL,"LIGHT_MAP_SIZE<nJpgSize","LightMap Error", 0);
		}
		FILE* f=fopen(strFilename.c_str(),"wb+");
		if (f)
		{
			fseek(f,OZJ_HEAD_SIZE,SEEK_SET);
			fwrite(jpgBuffer,nJpgSize,1,f);
			fclose(f);
		}
		f=fopen(ChangeExtension(strFilename,".jpg").c_str(),"wb+");
		if (f)
		{
			fwrite(jpgBuffer,nJpgSize,1,f);
			fclose(f);
		}
	}
	return true;
}

bool CMyPlug::exportTerrainHeight(iTerrainData * pTerrainData, const std::string& strFilename)
{
	FILE* f=fopen(strFilename.c_str(),"wb+");
	if (f)
	{
		fseek(f,HEIGHT_HEAD_SIZE,SEEK_SET);
		char buffer[HEIGHT_BUFFER_SIZE];
		char* p = buffer;
		for (int y=0; y<254; ++y)
		{
			*p =0;++p;
			*p =0;++p;
			for (int x=0; x<254; ++x)
			{
				*p = max(min(pTerrainData->getVertexHeight(x,y)/0.015f,255),0);
				p++;
			}
		}
		for (int x=0; x<256*2; ++x)
		{
			*p =0;++p;
		}
		fwrite(buffer,HEIGHT_BUFFER_SIZE,1,f);
		fclose(f);
	}
	return true;
}

bool CMyPlug::exportTerrainData(iTerrainData * pTerrainData, const std::string& strFilename)
{
	//////////////////////////////////////////////////////////////////////////
	exportTerrainAtt(pTerrainData, strFilename);
	exportTerrainLightmap(pTerrainData, GetParentPath(strFilename)+"TerrainLight.ozj");
	exportTerrainHeight(pTerrainData, GetParentPath(strFilename)+"TerrainHeight.ozb");

	// Calc MU's map id from filename.
	int nMapID = getMapIDFromFilename(strFilename);

	// map
	FILE* f=fopen(strFilename.c_str(),"wb");
	if (f)
	{
		char buffer[MAP_FILE_SIZE];
		char* p = buffer;
		*((unsigned char*)p)=0;++p;
		*((unsigned char*)p)=nMapID;++p;
		// tile
		for (size_t layer=0;layer<2;++layer)
		{
			for (int y=0; y<253; ++y)
			{
				for (int x=0; x<253; ++x)
				{
					*p = pTerrainData->getCellTileID(x,y,layer);
					p++;
				}
				*p =0;++p;
				*p =0;++p;
				*p =0;++p;
			}
			for (int x=0; x<256*3; ++x)
			{
				*p =0;++p;
			}
		}
		// alpha
		{
			for (int y=0; y<254; ++y)
			{
				for (int x=0; x<254; ++x)
				{
					*p = pTerrainData->getVertexColor(x,y).a;
					p++;
				}
				*p =0;++p;
				*p =0;++p;
			}
			for (int x=0; x<256*2; ++x)
			{
				*p =0;++p;
			}
		}
		encrypt(buffer,MAP_FILE_SIZE);
		fwrite(buffer,MAP_FILE_SIZE,1,f);
		fclose(f);
	}

	return true;
}

bool CMyPlug::exportTiles(iTerrainData * pTerrain, const std::string& strFilename, const std::string& strPath)
{
	return true;
}

void CMyPlug::release()
{
	delete this;
}