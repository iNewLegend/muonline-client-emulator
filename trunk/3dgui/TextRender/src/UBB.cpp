#include "UBB.h"

CUBB::CUBB()
{
}

CUBB::~CUBB()
{
}

void CUBB::Init(const RECT& rc, int nFontSize, int nAlignType)
{
	m_nFontSize = nFontSize;

	m_nBold = 0;
	m_nItalic = 0;
	m_bUnderline = false;
	m_nAlignType = nAlignType;
	m_bSup = false;
	m_bSub = false;
	m_bFlipH = false;
	m_bFlipV = false;
	m_dwColor = 0xFFFFFFFF;
	m_nSize = m_nFontSize;
	m_TagTypeRecords[TAG_TYPE_BOLD].push_back(m_nBold);
	m_TagTypeRecords[TAG_TYPE_ITALIC].push_back(m_nItalic);
	m_TagTypeRecords[TAG_TYPE_UNDERLINE].push_back(m_bUnderline);
	m_TagTypeRecords[TAG_TYPE_BR].push_back(0);
	m_TagTypeRecords[TAG_TYPE_ALIGN].push_back(m_nAlignType);
	m_TagTypeRecords[TAG_TYPE_URL].push_back(0);
	m_TagTypeRecords[TAG_TYPE_SUP].push_back(m_bSup);
	m_TagTypeRecords[TAG_TYPE_SUB].push_back(m_bSub);
	m_TagTypeRecords[TAG_TYPE_FLIPH].push_back(m_bFlipH);
	m_TagTypeRecords[TAG_TYPE_FLIPV].push_back(m_bFlipV);
	m_TagTypeRecords[TAG_TYPE_COLOR].push_back(m_dwColor);
	m_TagTypeRecords[TAG_TYPE_SIZE].push_back(m_nSize);

	m_nShowLeft = rc.left;
	m_nShowTop = rc.top;
	m_nShowWidth = rc.right - rc.left;
	m_nShowHeight = rc.bottom - rc.top;

	m_nMaxWidth = 0;
	m_nMaxHeight = 0;

	m_nLineWidth = 0;
	m_nLineHeight = 0;

	m_nLineBegin = 0;
	m_nLineEnd = 0;

	m_VB.clear();
}

void CUBB::ParseTag(wchar_t* wcsTagt)
{
	if (wcsTagt==NULL && wcslen(wcsTagt)==0)
	{
		return;
	}

	bool bEnd =false;
	// Is the end flag?
	if (wcsTagt[0] == L'/')
	{
		bEnd =true;
	}

	//_tcstok()
	// Tag Name
	const wchar_t* pwszTagName=wcstok(&wcsTagt[bEnd?1:0],L"=");

	if (pwszTagName==NULL)
	{
		return;
	}

	int nTagType = -1;
	// Find the tag.
	for (int i=0; i<TAG_TYPE_MAX; ++i)
	{
		if (wcscmp(gs_UBB_TAG_TYPE_NAME[i],pwszTagName)==0)
		{
			nTagType = i;
			break;
		}
	}
	if (nTagType !=-1)
	{
		if (bEnd) // 回退
		{
			int nRecordCount = m_TagTypeRecords[nTagType].size()-1;
			if (nRecordCount > 0) // 防止为空
			{
				m_TagTypeRecords[nTagType].resize(nRecordCount);
				nRecordCount--;
				switch(nTagType)
				{
				case TAG_TYPE_BOLD:
					m_nBold = m_TagTypeRecords[nTagType][nRecordCount];
					break;
				case TAG_TYPE_ITALIC:
					m_nItalic = m_TagTypeRecords[nTagType][nRecordCount];
					break;
				case TAG_TYPE_UNDERLINE:
					break;
				case TAG_TYPE_BR:
					// 不做处理
					break;
				case TAG_TYPE_ALIGN:
					// 进入换行移位啦！
					updateTextLine();
					m_nAlignType = m_TagTypeRecords[nTagType][nRecordCount];
					break;
				case TAG_TYPE_URL:
					break;
				case TAG_TYPE_SUP:
					break;
				case TAG_TYPE_SUB:
					break;
				case TAG_TYPE_FLIPH:
					m_bFlipH = m_TagTypeRecords[nTagType][nRecordCount]!=0;
					break;
				case TAG_TYPE_FLIPV:
					m_bFlipV = m_TagTypeRecords[nTagType][nRecordCount]!=0;
					break;
				case TAG_TYPE_IMG:
					break;
				case TAG_TYPE_FLY:
					break;
				case TAG_TYPE_MOVE:
					break;
				case TAG_TYPE_GLOW:
					break;
				case TAG_TYPE_SHADOW:
					break;
				case TAG_TYPE_COLOR:
					m_dwColor = m_TagTypeRecords[nTagType][nRecordCount];
					break;
				case TAG_TYPE_SIZE:
					m_nSize = m_TagTypeRecords[nTagType][nRecordCount];
					break;
				case TAG_TYPE_FACE:
					break;
				case TAG_TYPE_DIR:
					break;
				case TAG_TYPE_RM:
					break;
				default:
					break;
				}
			}
		}
		else
		{
			switch(nTagType)
			{
			case TAG_TYPE_BOLD:
				{
					wchar_t* pwszParameter = wcstok(NULL,L",");
					if (pwszParameter==NULL)
					{
						m_nBold = 1;
					}
					else
					{
						m_nBold = _wtoi(pwszParameter);
					}
					m_TagTypeRecords[nTagType].push_back(m_nBold);
				}
				break;
			case TAG_TYPE_ITALIC:
				{
					wchar_t* pwszParameter = wcstok(NULL,L",");
					if (pwszParameter==NULL)
					{
						m_nItalic = 3;
					}
					else
					{
						m_nItalic = _wtoi(pwszParameter);
					}
					m_TagTypeRecords[nTagType].push_back(m_nItalic);
				}
				break;
			case TAG_TYPE_UNDERLINE:
				break;
			case TAG_TYPE_BR:
				{
					// 进入换行移位啦！
					updateTextLine();
				}
				break;
			case TAG_TYPE_ALIGN:
				{
					// 进入换行移位啦！
					updateTextLine();
					wchar_t* pwszParameter = wcstok(NULL,L",");
					if (pwszParameter!=NULL)
					{
						if (wcscmp(pwszParameter,L"center")==0)
						{
							m_nAlignType = ALIGN_TYPE_CENTER;
						}
						else if (wcscmp(pwszParameter,L"left")==0)
						{
							m_nAlignType = ALIGN_TYPE_LEFT;
						}
						else if (wcscmp(pwszParameter,L"right")==0)
						{
							m_nAlignType = ALIGN_TYPE_RIGHT;
						}
					}
					m_TagTypeRecords[nTagType].push_back(m_nAlignType);
				}
				break;
			case TAG_TYPE_URL:
				break;
			case TAG_TYPE_SUP:
				break;
			case TAG_TYPE_SUB:
				break;
			case TAG_TYPE_FLIPH:
				{
					m_bFlipH = true;
					m_TagTypeRecords[nTagType].push_back(m_bFlipH);

				}
				break;
			case TAG_TYPE_FLIPV:
				{
					m_bFlipV = true;
					m_TagTypeRecords[nTagType].push_back(m_bFlipV);
				}
				break;
			case TAG_TYPE_IMG:
				break;
			case TAG_TYPE_FLY:
				break;
			case TAG_TYPE_MOVE:
				break;
			case TAG_TYPE_GLOW:
				break;
			case TAG_TYPE_SHADOW:
				break;
			case TAG_TYPE_COLOR:
				{
					wchar_t* pwszParameters[4];
					pwszParameters[0] = wcstok(NULL,L",");
					pwszParameters[1] = wcstok(NULL,L",");
					pwszParameters[2] = wcstok(NULL,L",");
					pwszParameters[3] = wcstok(NULL,L",");

					if (pwszParameters[2]==NULL)
					{
					}
					else if (pwszParameters[3]==NULL)
					{
						byte r = _wtoi(pwszParameters[0]);
						byte g = _wtoi(pwszParameters[1]);
						byte b = _wtoi(pwszParameters[2]);
						m_dwColor = (255<<24)+(r<<16)+(g<<8)+b;
					}
					else
					{
						byte a = _wtoi(pwszParameters[0]);
						byte r = _wtoi(pwszParameters[1]);
						byte g = _wtoi(pwszParameters[2]);
						byte b = _wtoi(pwszParameters[3]);
						m_dwColor = (a<<24)+(r<<16)+(g<<8)+b;
					}
					m_TagTypeRecords[nTagType].push_back(m_dwColor);
				}
				break;
			case TAG_TYPE_SIZE:
				{
					wchar_t* pwszParameter = wcstok(NULL,L",");
					if (pwszParameter==NULL)
					{
					}
					else
					{
						m_nSize = _wtoi(pwszParameter);
					}
					m_TagTypeRecords[nTagType].push_back(m_nSize);
				}
				break;
			case TAG_TYPE_FACE:
				break;
			case TAG_TYPE_DIR:
				break;
			case TAG_TYPE_RM:
				break;
			default:
				break;
			}
		}
	}
}

void CUBB::AddChar(const TexCharInfo* charInfo)
{
	float fScaling = (float)m_nSize / (float)m_nFontSize;
	float fCharWidth = (charInfo->nWidth+charInfo->nOffsetX+m_nBold) * fScaling;/*abs(nItalic)*/;
	if (m_nLineHeight < m_nSize)
	{
		m_nLineHeight = m_nSize;
	}

	if (m_nShowWidth>0)
	{
		if (m_nLineWidth + fCharWidth > m_nShowWidth && m_nLineWidth + charInfo->nAdvX*fScaling > m_nShowWidth)
		{
			// 进入换行移位啦！
			updateTextLine();
		}
	}
	//m_nLineWidth+=(int)(charInfo->nOffsetX * fScaling);

	float fX0 = (float)m_nLineWidth+charInfo->nOffsetX * fScaling;
	float fX1 = m_nLineWidth+fCharWidth;
	float fY0 = -charInfo->nOffsetY * fScaling;
	float fY1 = fY0 + charInfo->nHeight * fScaling;

	// 非空白
	if (charInfo->nWidth > 0)
	{
		float u0,v0,u1,v1;
		if (m_bFlipH)
		{
			u0 = charInfo->fU1;
			u1 = charInfo->fU0;
		}
		else
		{
			u0 = charInfo->fU0;
			u1 = charInfo->fU1;
		}
		if (m_bFlipV)
		{
			v0 = charInfo->fV1;
			v1 = charInfo->fV0;
		}
		else
		{
			v0 = charInfo->fV0;
			v1 = charInfo->fV1;
		}

		VERTEX_TEXT v[4]=
		{
			fX0+(float)m_nItalic, fY0, 0.0f, m_dwColor, u0, v0,
			fX1+(float)m_nItalic, fY0, 0.0f, m_dwColor, u1, v0,
			fX1,				  fY1, 0.0f, m_dwColor, u1, v1,
			fX0,				  fY1, 0.0f, m_dwColor, u0, v1,
		};

		m_VB.push_back(v[0]);m_VB.push_back(v[1]);m_VB.push_back(v[2]);m_VB.push_back(v[3]);
	}
	m_nLineWidth += charInfo->nAdvX*fScaling;
	m_nMaxWidth = max(m_nMaxWidth,m_nLineWidth);
}

void CUBB::updateTextLine()
{
	m_nLineEnd = m_VB.size();
	if (m_nLineBegin < m_nLineEnd)
	{
		int nOffsetX = m_nShowLeft;
		int nOffsetY = m_nMaxHeight+m_nLineHeight+m_nShowTop;
		// 文本对其设置
		if (m_nAlignType == ALIGN_TYPE_CENTER)
		{
			nOffsetX += (m_nShowWidth-m_nLineWidth)/2;
		}
		else if (m_nAlignType == ALIGN_TYPE_RIGHT)
		{
			nOffsetX += m_nShowWidth-m_nLineWidth;
		}
		for (int i = m_nLineBegin; i < m_nLineEnd; i++)
		{
			m_VB[i].x += nOffsetX;
			m_VB[i].y += nOffsetY;
		}
		// 文本高度设置
		m_nMaxHeight += int(m_nLineHeight*1.5f);
		// 下一行为行宽度为0
		m_nLineWidth = 0;
		m_nLineHeight = 0;
		// begin重置
		m_nLineBegin = m_nLineEnd;
	}
}

RECT CUBB::getRect()
{
	RECT rc;
	rc.left = m_nShowLeft;
	rc.top = m_nShowTop;
	rc.right = rc.left+m_nMaxWidth;
	rc.bottom = rc.top+m_nMaxHeight;
	return rc;
}