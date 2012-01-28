#include "MyPlug.h"
#include "CSVFile.h"
#include "Material.h"

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
	char szParentDir[16]="";
	CCsvFile csv;
	if (!csv.open(szFilename))
	{
		return NULL;
	}
	while (csv.seekNextLine())
	{
// 		const char* szMaterial	= csv.getStr("Name","");
// 		CMaterial* pMaterial	= (CMaterial*)m_pRenderNodeMgr->createRenderData("material",szMaterial);
// 		if(pMaterial)
// 		{
// 			pMaterial->setTexture(0,getRealFilename(szParentDir,csv.getStr("Diffuse","")).c_str());
// 			//material.m_fOpacity		=csv.getFloat("Opacity");
// 			pMaterial->strShader = csv.getStr("Pass","diffuseAlphaTest64");
// 		}
	}
	return true;
}

void CMyPlug::release()
{
	delete this;
}