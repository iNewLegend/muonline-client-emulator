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
	CIniFile ini;
	if (ini.readIniFile(szFilename))
	{
		for (auto itSection=ini.m_isSection.begin(); itSection!=ini.m_isSection.end(); ++itSection)
		{
			if (itSection->m_strName.length()<2)
			{
				continue;
			}
			if (itSection->m_strName[0]!='m')
			{
				continue;
			}
			for (auto iteEntry=itSection->m_ieEntry.begin(); iteEntry!=itSection->m_ieEntry.end(); ++iteEntry)
			{
				if (iteEntry->m_strName=="Speed1")			particle->m_fSpeed1=atof(iteEntry->m_strValue.c_str());
				else if (iteEntry->m_strName=="Speed2")		particle->m_fSpeed2=atof(iteEntry->m_strValue.c_str());
				else if (iteEntry->m_strName=="Spread1")	particle->m_Spread1=atof(iteEntry->m_strValue.c_str());
				else if (iteEntry->m_strName=="Spread2")	particle->m_Spread2=atof(iteEntry->m_strValue.c_str());
				else if (iteEntry->m_strName=="Gravity")	particle->m_fGravity=atof(iteEntry->m_strValue.c_str());
				else if (iteEntry->m_strName=="Lifespan")	particle->m_Lifespan=atof(iteEntry->m_strValue.c_str());
				else if (iteEntry->m_strName=="LifeMiddle") particle->m_fLifeMid=atof(iteEntry->m_strValue.c_str());
				else if (iteEntry->m_strName=="Rate")		particle->m_Rate=atof(iteEntry->m_strValue.c_str());
				else if (iteEntry->m_strName=="Areal")		particle->m_Areal=atof(iteEntry->m_strValue.c_str());
				else if (iteEntry->m_strName=="Areaw")		particle->m_Areaw=atof(iteEntry->m_strValue.c_str());
				else if (iteEntry->m_strName=="Dampen")		particle->m_fDampen=atof(iteEntry->m_strValue.c_str());
				else if (iteEntry->m_strName=="Color1")		particle->m_fColor1=atoi(iteEntry->m_strValue.c_str());
				else if (iteEntry->m_strName=="Color2")		particle->m_fColor2=atoi(iteEntry->m_strValue.c_str());
				else if (iteEntry->m_strName=="Color3")		particle->m_fColor3=atoi(iteEntry->m_strValue.c_str());
				else if (iteEntry->m_strName=="Size1")		particle->m_fSize1=atof(iteEntry->m_strValue.c_str());
				else if (iteEntry->m_strName=="Size2")		particle->m_fSize2=atof(iteEntry->m_strValue.c_str());
				else if (iteEntry->m_strName=="Size3")		particle->m_fSize3=atof(iteEntry->m_strValue.c_str());
				else if (iteEntry->m_strName=="Rotation")	particle->m_fRotation=atof(iteEntry->m_strValue.c_str());
				else if (iteEntry->m_strName=="Order")		particle->m_nOrder=atoi(iteEntry->m_strValue.c_str());
				else if (iteEntry->m_strName=="Type")		particle->type=atoi(iteEntry->m_strValue.c_str());
				else if (iteEntry->m_strName=="Rows")		particle->m_nRows=atoi(iteEntry->m_strValue.c_str());
				else if (iteEntry->m_strName=="Cols")		particle->m_nCols=atoi(iteEntry->m_strValue.c_str());
				else if (iteEntry->m_strName=="IsBillboard")particle->m_bBillboard=atoi(iteEntry->m_strValue.c_str());
			}
		}
	}
	return true;
}

void CMyPlug::release()
{
	delete this;
}