#include "TextRender.h"

#include <ft2build.h>
#include <freetype/freetype.h>
#include <freetype/ftglyph.h>
#include <freetype/ftoutln.h>
#include <freetype/fttrigon.h>
#include "assert.h"

// 最接近的2的倍数
inline int next_p2 (int a)
{
	int rval=1;
	while(rval<a) rval<<=1;
	return rval;
}

CTextRender::CTextRender()
{
	m_dwColor			= 0xFFFFFFFF;
	m_bUpdated			= true;
	m_nTextTexWidth		= 1024;
	m_nTextTexHeight	= 1024;
	m_Face				= NULL;
	m_nShadowBorder		= 0;
	m_uShadowColor		= 0x0;
}

CTextRender::~CTextRender()
{
	Free();
}

int CTextRender::GetCharHeight() const
{
	return m_nH+2+m_nShadowBorder*2;
}

void CTextRender::load(const char* szFilename)
{
	//初始化FreeType库.. 
	if (FT_Init_FreeType((FT_Library*)&m_library))
		throw std::runtime_error("FT_Init_FreeType failed");

	//加载一个字体,取默认的Face,一般为Regualer 
	if (FT_New_Face((FT_Library)m_library, szFilename, 0, (FT_Face*)&m_Face)) 
		//throw std::runtime_error("FT_New_Face failed (there is probably a problem with your font file)");
		MessageBoxA(NULL,"FT_New_Face failed (there is probably a problem with your font file)","",0);
	if (NULL==m_Face)
	{
		MessageBoxA(NULL,"FT_New_Face failed (there is probably a problem with your font file)","",0);
	}

	FT_Select_Charmap((FT_Face)m_Face, FT_ENCODING_UNICODE);
}

void CTextRender::loadFromMemory(unsigned char* pFileBuffer, size_t fileSize)
{
	if (pFileBuffer==NULL)
	{
		MessageBoxA(NULL,"CTextRender::loadFromMemory() pFileBuffer==NULL","error",0);
	}

	//初始化FreeType库.. 
	if (FT_Init_FreeType((FT_Library*)&m_library))
		throw std::runtime_error("FT_Init_FreeType failed");

	//加载一个字体,取默认的Face,一般为Regualer 

	if (FT_New_Memory_Face((FT_Library)m_library, pFileBuffer, fileSize, 0, (FT_Face*)&m_Face)) 
		throw std::runtime_error("FT_New_Face failed (there is probably a problem with your font file)"); 
	if (NULL==m_Face)
	{
		MessageBoxA(NULL,"FT_New_Face failed (there is probably a problem with your font file)","",0);
	}

	FT_Select_Charmap((FT_Face)m_Face, FT_ENCODING_UNICODE);
}

void CTextRender::setFontSize(size_t fontSize) 
{
	m_nH=fontSize; 
	m_nLineHeight = int(m_nH*1.5f);
	FT_Set_Pixel_Sizes((FT_Face)m_Face, fontSize, fontSize);
}

void CTextRender::setShadowBorder(unsigned int uBorder)
{
	m_nShadowBorder = uBorder;
}
void CTextRender::setShadowColor(unsigned long uShadowColor)
{
	m_uShadowColor = uShadowColor;
}
// void CTextRender::Init(const std::wstring& wstrFontName, size_t fontSize) 
// {
// 	std::string	strFilename;
// 	if (m_mapFontFilename.find(wstrFontName)==m_mapFontFilename.end())
// 	{
// 		strFilename = m_mapFontFilename[m_wstrDefaultFont];
// 	}
// 	else
// 	{
// 		strFilename = m_mapFontFilename[wstrFontName];
// 	}
// 
// 	//加载一个字体,取默认的Face,一般为Regualer 
// 	IOReadBase* pRead = IOReadBase::autoOpen(strFilename);
// 	if (pRead)
// 	{
// 		char* pBuf = new char[pRead->GetSize()];
// 		if (pBuf)
// 		{
// 			pRead->Read(pBuf, pRead->GetSize());
// 		}
// 		IOReadBase::autoClose(pRead);
// 	}
//}

const TexCharInfo* CTextRender::GetCharInfo(wchar_t ch)
{
	std::map<wchar_t,TexCharInfo>::iterator it = m_CharsInfo.find(ch);
	if (it != m_CharsInfo.end())
	{
		if (it->second.nLeft != -1)
		{
			return &it->second;
		}
	}
	else
	{
		TexCharInfo charInfo;
		charInfo.nLeft = -1;
	//	m_CharsInfo.inserter()
		m_CharsInfo[ch] = charInfo;

		// 需要更新
		m_bUpdated = false;
	}
	return NULL;
}

bool CTextRender::scriptStringAnalyse(CScriptStringAnalysis& analysis, const wchar_t* wcsText)
{
	analysis.m_CharPos.clear();
	int nX = 0;
	size_t uTextLength = wcslen(wcsText);
	for(size_t n = 0; n < uTextLength;++n) 
	{
		const TexCharInfo* charInfo = GetCharInfo(wcsText[n]);
		if (charInfo)
		{
			nX += charInfo->nAdvX;
			analysis.m_CharPos.push_back(nX);
		}
		else
		{
			return false;
		}
	}
	return analysis.m_CharPos.size()==uTextLength;
}

#define CHAR_TEXTURE_BORDER_WIDTH 1
#define CHAR_TEXTURE_BORDER_HEIGHT 1
void CTextRender::OnFrameMove()
{
	if (m_bUpdated)
	{
		return;
	}

	unsigned long* imageData = new unsigned long[m_nTextTexWidth*m_nTextTexHeight];
	ZeroMemory(imageData,m_nTextTexWidth*m_nTextTexHeight*4);

	int nLeft	= 0;
	int nTop	= 0;
	int nHeight = 0;
	for (std::map<wchar_t,TexCharInfo>::iterator it=m_CharsInfo.begin(); it!=m_CharsInfo.end(); ++it)
	{
		// 取字模数据
		//FT_Load_Char((FT_Face)m_Face, it->first, FT_LOAD_RENDER
		//	|FT_LOAD_FORCE_AUTOHINT
		//	|(true?FT_LOAD_TARGET_NORMAL : FT_LOAD_MONOCHROME|FT_LOAD_TARGET_MONO)); 
		FT_Load_Char((FT_Face)m_Face, it->first, FT_LOAD_DEFAULT);

		//得到字模
		FT_Glyph glyph = NULL;
		if(FT_Get_Glyph( ((FT_Face)m_Face)->glyph, &glyph ))
			break;

		//转化成位图
		FT_Glyph_To_Bitmap( &glyph, FT_RENDER_MODE_NORMAL, 0, 1 );
		FT_BitmapGlyph bitmap_glyph = (FT_BitmapGlyph)glyph;

		//取道位图数据
		FT_Bitmap& bitmap=bitmap_glyph->bitmap;


		//FT_GlyphSlot slot = ((FT_Face)m_Face)->glyph; 

		if (nLeft + bitmap.width > m_nTextTexWidth)
		{
			nLeft = 0;
			nTop += nHeight+CHAR_TEXTURE_BORDER_HEIGHT;
			nHeight = 0;
		}
		// 超出纹理容量了
		if (nTop + bitmap.rows > m_nTextTexHeight)
		{
			m_CharsInfo.erase(it);
			continue;
		}
		// TexCharInfo
		TexCharInfo& charInfo = it->second;
		charInfo.nLeft		= nLeft;
		charInfo.nTop		= nTop;
		charInfo.nAdvX		= ((FT_Face)m_Face)->glyph->advance.x / 64+m_nShadowBorder;
		charInfo.nAdvY		= ((FT_Face)m_Face)->glyph->advance.y / 64+m_nShadowBorder;
		//m_FT_Face->size->metrics.y_ppem; //m_FT_Face->glyph->metrics.horiBearingY / 64.0f;

		charInfo.nWidth		= bitmap.width+m_nShadowBorder*2;
		charInfo.nHeight	= bitmap.rows+m_nShadowBorder*2;
		charInfo.nOffsetX	= bitmap_glyph->left;
		charInfo.nOffsetY	= bitmap_glyph->top;

		charInfo.fU0 = float(charInfo.nLeft+0.5f)	/ (float)m_nTextTexWidth;
		charInfo.fV0 = float(charInfo.nTop+0.5f)	/ (float)m_nTextTexHeight;
		charInfo.fU1 = float(charInfo.nLeft+charInfo.nWidth+0.5f) / (float)m_nTextTexWidth;
		charInfo.fV1 = float(charInfo.nTop+charInfo.nHeight+0.5f) / (float)m_nTextTexHeight;


		//把位图数据拷贝自己定义的画字纹理里。 
		if (m_nShadowBorder>0)
		{
			// shadow
			for (int i=0;i<=m_nShadowBorder*2;++i)
			{
				unsigned long* pImage = &imageData[charInfo.nTop*m_nTextTexWidth+charInfo.nLeft+i];
				unsigned char* pBitmap = (unsigned char*)bitmap.buffer;
				unsigned char uBitOffset = abs(i-m_nShadowBorder);
				for(int y=0; y < bitmap.rows; ++y)
				{
					for(int x=0; x < bitmap.width; ++x) 
					{
						*pImage += (((*pBitmap)>>uBitOffset)<<16);// alpha
						pBitmap++;
						pImage ++;
					}
					pImage+=m_nTextTexWidth-bitmap.width;
				}
			}
			for (int i=0;i<=m_nShadowBorder*2;++i)
			{
				unsigned long* pImage2 = &imageData[charInfo.nTop*m_nTextTexWidth+charInfo.nLeft];
				unsigned long* pImage = pImage2+i*m_nTextTexWidth;
				unsigned char uBitOffset = 16+abs(i-m_nShadowBorder);
				for(int y=0; y < bitmap.rows; ++y)
				{
					for(int x=0; x < charInfo.nWidth; ++x) 
					{
						*pImage += *pImage2>>uBitOffset;// alpha
						pImage2++;
						pImage ++;
					} 
					pImage+=m_nTextTexWidth-charInfo.nWidth;
					pImage2+=m_nTextTexWidth-charInfo.nWidth;
				}
			}
			{
				unsigned long* pImage = &imageData[charInfo.nTop*m_nTextTexWidth+charInfo.nLeft];
				for(int y=0; y < charInfo.nHeight; ++y)
				{
					for(int x=0; x < charInfo.nWidth; ++x) 
					{
						*pImage&=0xFFFF;
						if (*pImage>0xFF)
						{
							*pImage = 0xFF;
						}
						*pImage<<=24;
						//*pImage|=m_uShadowColor;;
						pImage ++;
					} 
					pImage+=m_nTextTexWidth-charInfo.nWidth;
				}
			}
			{
				unsigned long* pImage = &imageData[(charInfo.nTop+m_nShadowBorder)*m_nTextTexWidth+charInfo.nLeft+m_nShadowBorder];
				unsigned char* pBitmap = (unsigned char*)bitmap.buffer;
				for(int y=0; y < bitmap.rows; ++y)
				{
					for(int x=0; x < bitmap.width; ++x) 
					{
						//unsigned char* pBit = (unsigned char*)pImage;
						//unsigned char rate = 255-*pBitmap;
						//pBit++;
						/**pBit=((*pBit*rate)>>8)+*pBitmap;
						pBit++;
						*pBit=((*pBit*rate)>>8)+*pBitmap;
						pBit++;
						*pBit=((*pBit*rate)>>8)+*pBitmap;*/
						*pImage|=RGB(*pBitmap,*pBitmap,*pBitmap);

						pBitmap++;
						pImage ++;
					} 
					pImage+=m_nTextTexWidth-bitmap.width;
				}
			}
		}
		else
		{
			unsigned long* pImage = &imageData[charInfo.nTop*m_nTextTexWidth+charInfo.nLeft];
			unsigned char* pBitmap = (unsigned char*)bitmap.buffer;
			for(int y=0; y < bitmap.rows; ++y)
			{
				for(int x=0; x < bitmap.width; ++x) 
				{
					*pImage = 0xFFFFFF|((*pBitmap)<<24);// alpha
					pBitmap++;
					pImage ++;
				} 
				pImage+=m_nTextTexWidth-bitmap.width;
			}
		}

		//
		nLeft += charInfo.nWidth+CHAR_TEXTURE_BORDER_WIDTH;
		if (nHeight < charInfo.nHeight)
		{
			nHeight = charInfo.nHeight;
		}
		//
		FT_Done_Glyph( glyph );
	}
	// virtual
	updateTextureBuffer((unsigned char*)imageData,m_nTextTexWidth*m_nTextTexHeight*4,m_nTextTexWidth,m_nTextTexHeight);

	delete[] imageData;
	m_bUpdated = true;
}

void CTextRender::Free() 
{
	FT_Done_Face((FT_Face)m_Face); 
	FT_Done_FreeType((FT_Library)m_library); 
}

void CTextRender::buildUBB(CUBB* pUBB, const wchar_t* wcsText, const RECT& rc, int cchText, UINT format, unsigned long dwColor)
{
	// 初始化
	pUBB->Init(rc,m_nH,format);
	// ----
	pUBB->setColor(dwColor);
	// ----
	size_t uTextLength = cchText;
	// ----
	// # Check the string length.
	// ----
	if (cchText > 0)
	{
		assert(cchText<=wcslen(wcsText));
		uTextLength = cchText;
	}
	else
	{
		uTextLength = wcslen(wcsText);
	}
	// ----
	for(size_t n = 0 ; n < uTextLength;++n) 
	{
		// 标签判断中
		if (wcsText[n] == L'[')
		{
			wchar_t wcsTag[256];
			for (size_t i=0;i<256;++i)
			{
				n++;
				// ----
				if (wcsText[n] == L']')
				{
					wcsTag[i]=0;
					break;
				}
				// ----
				wcsTag[i]=wcsText[n];
			}
			pUBB->ParseTag(wcsTag);
		}
		else if(wcsText[n]==10)
		{
		}
		else // 处理字
		{
			const TexCharInfo* charInfo = GetCharInfo(wcsText[n]);
			if (charInfo)
			{
				pUBB->AddChar(charInfo);
			}
		}
	}
	// 针对最后一行文本
	pUBB->updateTextLine();
}

void CTextRender::drawUBB(CUBB* pUBB)
{
	int nVertexCount = pUBB->m_VB.size();
	if (nVertexCount)
	{
		drawTextVertexBuffer(nVertexCount,&pUBB->m_VB[0]);
	}
}

void CTextRender::drawText(const wchar_t* wcsText, const RECT& rc, int cchText, UINT format, unsigned long dwColor)
{
	CUBB ubb;
	buildUBB(&ubb, wcsText, rc, cchText, format, dwColor);
	drawUBB(&ubb);
}

void CTextRender::drawText(const wchar_t* wcsText, int x, int y, unsigned long dwColor)
{
	RECT rc = {x, y, 0, 0};
	drawText(wcsText, rc, -1, DT_NOCLIP, dwColor);
}

void CTextRender::drawText(const wchar_t* wcsText)
{
	drawText(wcsText, m_nX, m_nY, m_dwColor);
	m_nY += m_nLineHeight;
}

void CTextRender::calcUBBRect(const wchar_t* wcsText, RECT& rc)
{
	CUBB ubb;
	buildUBB(&ubb, wcsText, rc);
	if(!ubb.m_VB.empty())
	{
		rc = ubb.getRect();
	}
}