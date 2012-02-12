#include "MyPlug.h"
#include "Material.h"
#include "IniFile.h"

extern "C" __declspec(dllexport) bool Data_Plug_CreateObject(void ** pobj){
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

// ------------------------------------------------------------------------------------------

bool CMyPlug::importData(iRenderNode* pRenderNode, const char* szFilename)
{
	ParticleData* particle = (ParticleData*)m_pRenderNodeMgr->getRenderData("particle",szFilename);
	if(particle==NULL)
	{
		CIniFile ini;
		if (!ini.readIniFile(szFilename))
	{
		return false;
	}
	// ----
	particle = (ParticleData*)m_pRenderNodeMgr->createRenderData("particle");
	for (auto itSection=ini.m_isSection.begin(); itSection!=ini.m_isSection.end(); ++itSection)
	{
		for (auto iteEntry=itSection->m_ieEntry.begin(); iteEntry!=itSection->m_ieEntry.end(); ++iteEntry)
		{

			switch (itSection->m_strName[0])
			{
			case 'm':
				{
					auto& mat = particle->m_Material;
					for (auto iteEntry=itSection->m_ieEntry.begin(); iteEntry!=itSection->m_ieEntry.end(); ++iteEntry)
					{
						if (iteEntry->m_strName=="shader") mat.strShader=iteEntry->m_strValue;
						else if (iteEntry->m_strName=="s0") mat.strTexture[0]=iteEntry->m_strValue;
						else if (iteEntry->m_strName=="s1") mat.strTexture[1]=iteEntry->m_strValue;
						else if (iteEntry->m_strName=="s2") mat.strTexture[2]=iteEntry->m_strValue;
						else if (iteEntry->m_strName=="s3") mat.strTexture[3]=iteEntry->m_strValue;
						else if (iteEntry->m_strName=="s4") mat.strTexture[4]=iteEntry->m_strValue;
						else if (iteEntry->m_strName=="s5") mat.strTexture[5]=iteEntry->m_strValue;
						else if (iteEntry->m_strName=="s6") mat.strTexture[6]=iteEntry->m_strValue;
						else if (iteEntry->m_strName=="s7") mat.strTexture[7]=iteEntry->m_strValue;
					}
				}
				break;

			case 'p':
				{
					if (iteEntry->m_strName=="Speed")
					{
						sscanf(iteEntry->m_strValue.c_str(), "%f,%f", &particle->m_fSpeed1, &particle->m_fSpeed2);
					}
					else if (iteEntry->m_strName=="Spread")
					{
						sscanf(iteEntry->m_strValue.c_str(), "%f,%f", &particle->m_Spread1, &particle->m_Spread2);
					}
					else if (iteEntry->m_strName=="Gravity")	particle->m_fGravity=atof(iteEntry->m_strValue.c_str());
					else if (iteEntry->m_strName=="Lifespan")	particle->m_Lifespan=atof(iteEntry->m_strValue.c_str());
					else if (iteEntry->m_strName=="LifeMiddle") particle->m_fLifeMid=atof(iteEntry->m_strValue.c_str());
					else if (iteEntry->m_strName=="Rate")		particle->m_Rate=atof(iteEntry->m_strValue.c_str());
					else if (iteEntry->m_strName=="Area")
					{
						sscanf(iteEntry->m_strValue.c_str(), "%f,%f", &particle->m_Areal, &particle->m_Areaw);
					}
					else if (iteEntry->m_strName=="Dampen")		particle->m_fDampen=atof(iteEntry->m_strValue.c_str());
					else if (iteEntry->m_strName=="Color")
					{
						sscanf(iteEntry->m_strValue.c_str(), "%X,%X,%X", &particle->m_fColor1, &particle->m_fColor2, &particle->m_fColor3);
					}
					else if (iteEntry->m_strName=="Size")
					{
						sscanf(iteEntry->m_strValue.c_str(), "%f,%f,%f", &particle->m_fSize1, &particle->m_fSize2, &particle->m_fSize3);
					}
					else if (iteEntry->m_strName=="Rotation")	particle->m_fRotation=atof(iteEntry->m_strValue.c_str());
					else if (iteEntry->m_strName=="Order")		particle->m_nOrder=atoi(iteEntry->m_strValue.c_str());
					else if (iteEntry->m_strName=="Type")		particle->type=atoi(iteEntry->m_strValue.c_str());
					else if (iteEntry->m_strName=="Rows")		particle->m_nRows=atoi(iteEntry->m_strValue.c_str());
					else if (iteEntry->m_strName=="Cols")		particle->m_nCols=atoi(iteEntry->m_strValue.c_str());
					else if (iteEntry->m_strName=="IsBillboard")particle->m_bBillboard=atoi(iteEntry->m_strValue.c_str());
				}
				break;
			}

		}
	}
	}
	
	pRenderNode->setData(particle);
	return true;
}

void CMyPlug::release()
{
	delete this;
}