#include "RenderNodeMgr.h"
#include "IORead.h"
#include "FileSystem.h"
#include "RenderSystem.h"
#include "Scene.h"
#include "SceneData.h"

static iRenderNode* s_pThreadRenderNode = NULL;
DWORD WINAPI LoadingThreads (LPVOID pScene)
{
	while(true)
	{
		iRenderNode* pRenderNode = s_pThreadRenderNode;
		s_pThreadRenderNode=NULL;
		if (pRenderNode)
		{
			pRenderNode->load(pRenderNode->getFilename());
		}
	}
	return 0;
}

iRenderNode* newSkeletonNode(){return new CSkeletonNode;}
iRenderNode* newParticleEmitter(){return new CParticleEmitter;}
iRenderNode* newSkinMesh(){return new CSkinMesh;};

void* newSkeletonData(){return new CSkeletonData;}
void* newParticleData(){return new ParticleData;}
void* newLodMesh(){return new CMeshData;}
void* newSceneData(){return new CSceneData;}

CRenderNodeMgr::CRenderNodeMgr()
{
	loadPlugs("Plugins\\*.dll");
	registerRenderNode("skeleton",	(P_FUNC_NEW_RENDER_NODE)newSkeletonNode/*(P_FUNC_NEW_RENDER_NODE)&[](){return new CSkeletonNode;}*/);
	registerRenderNode("particle",	(P_FUNC_NEW_RENDER_NODE)newParticleEmitter);
	registerRenderNode("mesh",		(P_FUNC_NEW_RENDER_NODE)newSkinMesh);
	
	registerRenderData("skeleton",	(P_FUNC_NEW_RENDER_DATA)newSkeletonData);
	registerRenderData("particle",	(P_FUNC_NEW_RENDER_DATA)newParticleData);
	registerRenderData("mesh",		(P_FUNC_NEW_RENDER_DATA)newLodMesh);
	registerRenderData("scene",		(P_FUNC_NEW_RENDER_DATA)newSceneData);

	//isStartImmediate = CREATE_SUSPENDED;
	m_hThread=CreateThread(NULL,0,LoadingThreads,(LPVOID)this,0,&m_dwThreadID); 
	//if (m_hThread)
	//{ 
	//	printf ("Thread launched successfully\n");                
	//}         
	//ResumeThread(m_hThread);
}

CRenderNodeMgr::~CRenderNodeMgr()
{
	if (m_hThread)
	{            
		CloseHandle(m_hThread);
	} 
}

void CRenderNodeMgr::registerRenderNode(const char* szClassName, P_FUNC_NEW_RENDER_NODE pfn)
{
	m_mapRenderNodeFunc[szClassName] = pfn;
}

void CRenderNodeMgr::registerRenderData(const char* szClassName, P_FUNC_NEW_RENDER_DATA pfn)
{
	m_mapRenderDataFunc[szClassName] = pfn;
}

void CRenderNodeMgr::PushMTLoading(iRenderNode* pRenderNode)
{
	s_pThreadRenderNode = pRenderNode;
}

bool CRenderNodeMgr::loadRenderNode(const char* szFilename, iRenderNode* pRenderNode)
{
	// 判断格式--根据文件后缀名
	std::string strExt = GetExtension(szFilename);
	CDataPlugBase* pModelPlug = getPlugByExtension(strExt.c_str());
	if (pModelPlug)
	{
		return pModelPlug->importData(pRenderNode, szFilename);
	}
	return false;
}

iRenderNode* CRenderNodeMgr::createRenderNode(const char* szClassName)
{
	auto it = m_mapRenderNodeFunc.find(szClassName);
	if (it!=m_mapRenderNodeFunc.end())
	{
		return it->second();
	}
	return NULL;
}

void* CRenderNodeMgr::createRenderData(const char* szClassName)
{
	auto it = m_mapRenderDataFunc.find(szClassName);
	if (it!=m_mapRenderDataFunc.end())
	{
		return it->second();
	}
	return NULL;
}

void* CRenderNodeMgr::getRenderData(const char* szClassName, const char* szName)
{
	auto it = m_mapRenderData.find(szClassName);
	if (it!=m_mapRenderData.end())
	{
		auto it2 = it->second.find(szName);
		if (it2!=it->second.end())
		{
			return it2->second;
		}
	}
	return NULL;
}

void* CRenderNodeMgr::createRenderData(const char* szClassName, const char* szName)
{
	if(getRenderData(szClassName,szName))
	{
		return NULL;
	}
	void* pData = createRenderData(szClassName);
	if(pData)
	{
		m_mapRenderData[szClassName][szName] = pData;
	}
	return pData;
}

typedef bool ( * PFN_DATA_Plug_CreateObject)(void ** pobj);

CDataPlugBase* CRenderNodeMgr::getPlugByExtension(const char* szExt)
{
	for (size_t i=0;i<m_setPlugObj.size();++i)
	{
		if (strcmp(m_setPlugObj[i].pObj->getFormat(),szExt)==0)
		{
			return m_setPlugObj[i].pObj;
		}
	}
	return NULL;
}

bool CRenderNodeMgr::loadPlugs(const char* szFilename)
{
	WIN32_FIND_DATAA wfd;
	std::string strFilename;
	std::string strDir;

	strDir = szFilename;
	for (int i=strDir.length()-2; i>= 0; i--)
	{
		if (strDir[i]=='\\' || strDir[i]=='/')
		{
			strDir = strDir.substr(0,i+1);
			break;
		}
	}

	HANDLE hf = FindFirstFileA(szFilename, &wfd);

	// If invalid, than return.
	if (INVALID_HANDLE_VALUE == hf)
	{
		return false;
	}

	// Add the plugs
	do 
	{
		strFilename = strDir + wfd.cFileName;
		loadPlug(strFilename.c_str());
	}while (FindNextFileA(hf, &wfd));

	// Find Close
	FindClose(hf);
	return true;
}

bool CRenderNodeMgr::loadPlug(const char* szFilename)
{
	bool brt = false;
	if (m_setPlugObj.size() > 255)
	{
		MessageBoxA(NULL,"插件过多", "message", MB_OK|MB_ICONINFORMATION);
		return false;
	}
	DATA_PLUG_ST stPs;
	ZeroMemory(&stPs, sizeof(stPs));
	stPs.hIns = LoadLibrary(szFilename);
	if (stPs.hIns)
	{
		PFN_DATA_Plug_CreateObject pFunc = (PFN_DATA_Plug_CreateObject)GetProcAddress(stPs.hIns, "Data_Plug_CreateObject");
		if (pFunc)
		{
			if (pFunc((void **)&stPs.pObj))
			{
#ifdef _DEBUG
				if (stPs.pObj->isDebug())
#else
				if (!stPs.pObj->isDebug())
#endif
				{
					brt =true;
					stPs.pObj->setRenderNodeMgr(this);
					m_setPlugObj.push_back(stPs);
				}
			}
		}
	}
	if (!brt)
	{
		if (stPs.pObj)
		{
			stPs.pObj->release();
		}
		if (stPs.hIns)
		{
			FreeLibrary(stPs.hIns);
		}
	}
	return brt;
}

std::string CRenderNodeMgr::getAllExtensions()
{
	std::string strExts;
	for (size_t i=0;i<m_setPlugObj.size();++i)
	{
		if (i>0)
		{
			strExts+="|";
		}
		strExts+=m_setPlugObj[i].pObj->getFormat();
	}
	return strExts;
}