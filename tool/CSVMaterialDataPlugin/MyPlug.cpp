#include "MyPlug.h"
#include "CSVFile.h"
#include "Material.h"

BOOL WINAPI Data_Plug_CreateObject(void ** pobj){
	*pobj = new CMyPlug;
	return *pobj != NULL;
}

CMyPlug::CMyPlug(void)
{

}

CMyPlug::~CMyPlug(void)
{

}

#include "FileSystem.h"

std::string getLineCommand(IOReadBase* pRead, std::vector<std::string>& setWords)
{
	setWords.clear();
	std::string str;
	std::string strCommand;
	char c;
	bool bString = false;
	while (!pRead->IsEof())
	{
		pRead->Read(&c,sizeof(char));
		// ----
		if ('"'==c)
		{
			if (bString)
			{
				setWords.push_back(str);
			}
			bString=!bString;
		}
		else if (bString)
		{
			str.push_back(c);
		}
		else if ( '('==c||')'==c||','==c||';'==c||'	'==c||' '==c||'\n'==c||char(13)==c )
		{
			if (strCommand.length()==0)
			{
				strCommand = str;
				str.clear();
			}
			else if (','==c||')'==c)
			{
				setWords.push_back(str);
				str.clear();
			}
		}
		else
		{
			str.push_back(c);
		}
		// ----
		if (';'==c||'\n'==c||char(13)==c)
		{
			if (strCommand.size()>0)
			{
				break;
			}
		}
	}
	if (pRead->IsEof())
	{
		if (strCommand.length()==0)
		{
			strCommand = str;
		}
		else
		{
			setWords.push_back(str);
		}
	}
	return strCommand;
}
// ------------------------------------------------------------------------------------------

bool loadMaterialPass(CMaterial& material, const char* szFilename)
{
	IOReadBase* pRead = IOReadBase::autoOpen(szFilename);
	if (pRead==NULL)
	{
		return false;
	}
	std::string strCommand;
	std::vector<std::string> setWords;
	while (!pRead->IsEof())
	{
		strCommand = getLineCommand(pRead,setWords);
		size_t	uParametersCount = setWords.size();
		// ----
		if ("LightingEnabled"==strCommand)
		{
			if (uParametersCount>0)
			{
				material.bLightingEnabled = setWords[0]=="true"?true:false;
			}
		}
		// ----
		else if ("CullingMode"==strCommand)
		{
			if (uParametersCount>0)
			{
				material.uCull	= ConvertStringToEnum<CullingMode>(setWords[0].c_str());
			}
		}
		// ----
		else if ("AlphaTestFunc"==strCommand)
		{
			if (uParametersCount>0)
			{
				material.bAlphaTest = setWords[0]=="true"?true:false;
				// ----
				if (uParametersCount>=3)
				{
					material.nAlphaTestCompare	= ConvertStringToEnum<CompareFunction>(setWords[1].c_str());
					material.uAlphaTestValue	= atoi(setWords[2].c_str());
				}
			}
		}
		// ----
		else if ("BlendFunc"==strCommand)
		{
			if (uParametersCount>0)
			{
				material.bBlend = setWords[0]=="true"?true:false;
				// ----
				if (uParametersCount>=4)
				{
					material.nBlendOP	= ConvertStringToEnum<SceneBlendOperation>(setWords[1].c_str());
					material.nBlendSrc	= ConvertStringToEnum<SceneBlendFactor>(setWords[2].c_str());
					material.nBlendDest	= ConvertStringToEnum<SceneBlendFactor>(setWords[3].c_str());
				}
			}
		}
		// ----
		else if ("DepthBufferFunc"==strCommand)
		{
			if (uParametersCount>=2)
			{
				material.bDepthTest		= setWords[0]=="true"?true:false;
				// ----
				material.bDepthWrite	= setWords[1]=="true"?true:false;
			}
		}
		// ----
		else if ("TextureColorOP"==strCommand)
		{
			if (uParametersCount>=2)
			{
				int nID						= atoi(setWords[0].c_str());
				if (nID>=0 && nID<8)
				{
					CMaterial::TextureOP& texOP	= material.textureOP[nID];
					texOP.nColorOP				= ConvertStringToEnum<TextureBlendOperation>(setWords[1].c_str());
					// ----
					if (uParametersCount>=4)
					{
						texOP.nColorSrc1		= ConvertStringToEnum<TextureBlendSource>(setWords[2].c_str());
						texOP.nColorSrc2		= ConvertStringToEnum<TextureBlendSource>(setWords[3].c_str());
					}
				}
			}
		}
		// ----
		else if ("TextureAlphaOP"==strCommand)
		{
			if (uParametersCount>=2)
			{
				int nID						= atoi(setWords[0].c_str());
				if (nID>=0 && nID<8)
				{
					CMaterial::TextureOP& texOP	= material.textureOP[nID];
					texOP.nAlphaOP			= ConvertStringToEnum<TextureBlendOperation>(setWords[1].c_str());
					// ----
					if (uParametersCount>=4)
					{
						texOP.nAlphaSrc1		= ConvertStringToEnum<TextureBlendSource>(setWords[2].c_str());
						texOP.nAlphaSrc2		= ConvertStringToEnum<TextureBlendSource>(setWords[3].c_str());
					}
				}
			}
		}
		// ----
		else if ("Shader" == strCommand)
		{
			if (uParametersCount>0)
			{
				material.setShader(setWords[0].c_str());
			}
		}
	}
	IOReadBase::autoClose(pRead);
}

bool CMyPlug::importData(iRenderNodeMgr* pRenderNodeMgr, iRenderNode* pRenderNode, const char* szFilename)
{
	char szParentDir[16]="";
	CCsvFile csv;
	if (!csv.open(szFilename))
	{
		return NULL;
	}
	while (csv.seekNextLine())
	{
		const char* szMaterial	= csv.getStr("Name","");
		CMaterial* pMaterial	= (CMaterial*)pRenderNodeMgr->createRenderData("material",szMaterial);
		if(pMaterial)
		{
			pMaterial->setTexture(0,getRealFilename(szParentDir,csv.getStr("Diffuse","")).c_str());
			//material.m_fOpacity		=csv.getFloat("Opacity");
			const char* szPass		=csv.getStr("Pass",NULL);
			if (szPass)
			{
				char szPassFilename[255];
				sprintf(szPassFilename,"EngineRes\\pass\\%s.pass",szPass);
				loadMaterialPass(*pMaterial, szPassFilename);
			}
			else
			{
				pMaterial->bLightingEnabled	 = false;
				// ----
				pMaterial->uCull				= CULL_NONE;
				// ----
				pMaterial->bAlphaTest			= false;
				// ----
				pMaterial->bBlend				= false;
				// ----
				pMaterial->bDepthTest			= true;
				pMaterial->bDepthWrite			= true;
				// ----
				CMaterial::TextureOP& texOP0	= pMaterial->textureOP[0];
				CMaterial::TextureOP& texOP1	= pMaterial->textureOP[1];
				// ----
				texOP0.nColorOP					= TBOP_MODULATE;
				texOP0.nColorSrc1				= TBS_CURRENT;
				texOP0.nColorSrc2				= TBS_TEXTURE;
				texOP0.nAlphaOP					= TBOP_MODULATE;
				texOP0.nAlphaSrc1				= TBS_CURRENT;
				texOP0.nAlphaSrc2				= TBS_TEXTURE;
				// ----
				texOP1.nColorOP					= TBOP_DISABLE;
				texOP1.nAlphaOP					= TBOP_DISABLE;
			}
		}
	}
	csv.close();
	return true;
}

void CMyPlug::release()
{
	delete this;
}