//**************************************************************************/
// Copyright (c) 1998-2007 Autodesk, Inc.
// All rights reserved.
// 
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information written by Autodesk, Inc., and are
// protected by Federal copyright law. They may not be disclosed to third
// parties or copied or duplicated in any form, in whole or in part, without
// the prior written consent of Autodesk, Inc.
//**************************************************************************/
// DESCRIPTION: Appwizard generated plugin
// AUTHOR: 
//***************************************************************************/

#include "Max2009ModelExporter.h"

#include "decomp.h"

#include "IGame/IGame.h"
#include "IGame/IGameObject.h"
#include "IGame/IGameProperty.h"
#include "IGame/IGameControl.h"
#include "IGame/IGameModifier.h"
#include "IGame/IConversionManager.h"
#include "IGame/IGameError.h"
#include "IGame/IGameFX.h"

#include "3dsmaxport.h"



#include "Vec3D.h"
#include "ModelData.h"
#include "CsvFile.h"

#include <set>


inline Vec3D fixCoordSystemPos(Vec3D v)
{
	return Vec3D(v.x, v.z, v.y)*1.00f;
}
inline Vec3D fixCoordSystemNormal(Vec3D v)
{
	return Vec3D(v.x, v.z, v.y);
}

inline Quaternion fixCoordSystemRotate(Quat q)
{
	Vec3D v;
	QuatToEuler(q,(float*)&v);
	Quaternion my;
	my.rotate(Vec3D(v.x,v.y,-v.z));
	return Quaternion(my.x, -my.z, my.y, my.w);
}

#define Max2009ModelExporter_CLASS_ID	Class_ID(0x9a398ae6, 0xb1de19bf)

void Warn( LPCTSTR sz )
{
	MessageBox( NULL, sz, TEXT("Warn"), MB_OK | MB_ICONSTOP | MB_TOPMOST );
}

class Max2009ModelExporter : public SceneExport {
	public:
		IGameScene * m_pIgame;

		static HWND hParams;

		int curNode;

		char strFilename[FILENAME_MAX];// string
		//CLog m_Log;

		int staticFrame;		// 静态帧
		int framePerSample;		// 帧采样位数
		float exportScale;		// scale
		BOOL exportGeom;		// 输出模块化磁盘变换框架
		BOOL exportNormals;		// 输出法线
		BOOL exportVertexColor;	// 输出顶点色
		BOOL exportControllers;	// 输出控制器
		BOOL exportFaceSmgp;	// 输出面
		BOOL exportTexCoords;	// 输出纹理坐标
		BOOL exportMappingChannel;// 输出映射通道
		BOOL exportConstraints;	// 
		BOOL exportMaterials;	// 输出材质
		BOOL exportSplines;		// 输出曲线
		BOOL exportModifiers;	// 输出修改器
		BOOL exportSkin;		// 输出皮肤
		BOOL exportGenMod;		//
		BOOL forceSample;		//
		BOOL exportQuaternions;
		BOOL exportObjectSpace;
		BOOL exportRelative;

		int exportCoord;
		bool showPrompts;
		bool exportSelected;

		TSTR splitPath;

		float version;

		int	nMaterialsCount;

		int				ExtCount()
		{
			#pragma message(TODO("Returns the number of file name extensions supported by the plug-in."))
			return 1;
		}
		const TCHAR *	Ext(int n)
		{
			#pragma message(TODO("Return the 'i-th' file name extension (i.e. \"3DS\")."))
			return _T("SM");
		}
		const TCHAR *	LongDesc()
		{
			#pragma message(TODO("Return long ASCII description (i.e. \"Targa 2.0 Image File\")"))
			return _T("rpgsky model exporter");
		}
		const TCHAR *	ShortDesc()
		{
			#pragma message(TODO("Return short ASCII description (i.e. \"Targa\")"))
			return _T("rpgsky");
		}
		const TCHAR *	AuthorName()
		{			
			#pragma message(TODO("Return ASCII Author name"))
			return _T("panhuihui");
		}
		const TCHAR *	CopyrightMessage()
		{	
			#pragma message(TODO("Return ASCII Copyright message"))
			return _T("");
		}
		const TCHAR *	OtherMessage1()
		{
			//TODO: Return Other message #1 if any
			return _T("");
		}
		const TCHAR *	OtherMessage2()
		{		
			//TODO: Return other message #2 in any
			return _T("");
		}
		unsigned int	Version()
		{				
			#pragma message(TODO("Return Version number * 100 (i.e. v3.01 = 301)"))
			return 100;
		}
		void			ShowAbout(HWND hWnd){}


		BOOL SupportsOptions(int ext, DWORD options)
		{
			#pragma message(TODO("Decide which options to support.  Simply return true for each option supported by each Extension the exporter supports."))
			return TRUE;
		}
		int				DoExport(const TCHAR *name,ExpInterface *ei,Interface *i, BOOL suppressPrompts=FALSE, DWORD options=0);

		//void ExportMaterials( CModelData& model );
		void DumpMaterial( CModelData& model, IGameMaterial * mat, int index, int matID = -1 );

		void DumpChildBone(IGameNode * child, CSkeleton& skeleton);

		void DumpChildNodeInfo( IGameNode * child, CModelData& model );
		//void ExportPositionControllers(BoneAnim& boneAnim, IGameControl * pGameControl, Point3 pos);
		void DumpSampleKeys(IGameControl * pGameControl,BoneAnim& boneAnim, IGameControlType Type);
		bool DumpTCBKeys(IGameControl * pGameControl,BoneAnim& boneAnim, IGameControlType Type);
		bool DumpLinearKeys(IGameControl * pGameControl,BoneAnim& boneAnim, IGameControlType Type);
		bool DumpBezierKeys(IGameControl * pGameControl,BoneAnim& boneAnim, IGameControlType Type);

		bool DumpPos(IGameControl * pGameControl,BoneAnim& boneAnim);
		bool DumpRot(IGameControl * pGameControl,BoneAnim& boneAnim);
		bool DumpScale(IGameControl * pGameControl,BoneAnim& boneAnim);

		bool DumpAnim(IGameControl * pGameControl,BoneAnim& boneAnim);
		void DumpSampleKeys(IGameControl * sc,BoneAnim& boneAnim, IGameControlType Type, bool quick, Point3 pos);
		void DumpMesh(IGameNode * child, IGameMesh *gm, CModelData& model );

		//Constructor/Destructor
		Max2009ModelExporter();
		~Max2009ModelExporter();

		BOOL ReadConfig();
		void WriteConfig();
		TSTR GetCfgFilename();

		BOOL ReadAnimList(std::vector<ModelAnimation>& setAnimList);
};

class Max2009ModelExporterClassDesc : public ClassDesc2 
{
public:
	virtual int IsPublic() 							{ return TRUE; }
	virtual void* Create(BOOL /*loading = FALSE*/) 		{ return new Max2009ModelExporter(); }
	virtual const TCHAR *	ClassName() 			{ return GetString(IDS_CLASS_NAME); }
	virtual SClass_ID SuperClassID() 				{ return SCENE_EXPORT_CLASS_ID; }
	virtual Class_ID ClassID() 						{ return Max2009ModelExporter_CLASS_ID; }
	virtual const TCHAR* Category() 				{ return GetString(IDS_CATEGORY); }

	virtual const TCHAR* InternalName() 			{ return _T("Max2009ModelExporter"); }	// returns fixed parsable name (scripter-visible name)
	virtual HINSTANCE HInstance() 					{ return hInstance; }					// returns owning module handle
};

static Max2009ModelExporterClassDesc Max2009ModelExporterDesc;
ClassDesc2* GetMax2009ModelExporterDesc() { return &Max2009ModelExporterDesc; }

INT_PTR CALLBACK Max2009ModelExporterOptionsDlgProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam) {
	Max2009ModelExporter *exp = (Max2009ModelExporter*)GetWindowLongPtr(hWnd,GWLP_USERDATA); 
	ISpinnerControl * spin;
	int ID;
	switch(message)
	{
	case WM_INITDIALOG:
		exp = (Max2009ModelExporter *)lParam;
		SetWindowLongPtr(hWnd,GWLP_USERDATA,lParam); 
		CenterWindow(hWnd,GetParent(hWnd));
		spin = GetISpinner(GetDlgItem(hWnd, IDC_STATIC_FRAME_SPIN)); 
		spin->LinkToEdit(GetDlgItem(hWnd,IDC_STATIC_FRAME), EDITTYPE_INT ); 
		spin->SetLimits(0, 100, TRUE); 
		spin->SetScale(1.0f);
		spin->SetValue(exp->staticFrame ,FALSE);
		ReleaseISpinner(spin);

		spin = GetISpinner(GetDlgItem(hWnd, IDC_SAMPLE_FRAME_SPIN)); 
		spin->LinkToEdit(GetDlgItem(hWnd,IDC_SAMPLE_FRAME), EDITTYPE_INT ); 
		spin->SetLimits(1, 100, TRUE); 
		spin->SetScale(1.0f);
		spin->SetValue(exp->framePerSample ,FALSE);
		ReleaseISpinner(spin);

		spin = GetISpinner(GetDlgItem(hWnd, IDC_SCALE_SPIN)); 
		spin->LinkToEdit(GetDlgItem(hWnd,IDC_SCALE), EDITTYPE_FLOAT ); 
		spin->SetLimits(0.01f, 100.0f, TRUE); 
		spin->SetScale(0.01f);
		spin->SetValue(exp->exportScale ,FALSE);
		ReleaseISpinner(spin);


		CheckDlgButton(hWnd,IDC_EXP_GEOMETRY,exp->exportGeom);
		CheckDlgButton(hWnd,IDC_EXP_NORMALS,exp->exportNormals);
		CheckDlgButton(hWnd,IDC_EXP_CONTROLLERS,exp->exportControllers);
		CheckDlgButton(hWnd,IDC_EXP_FACESMGRP,exp->exportFaceSmgp);
		CheckDlgButton(hWnd,IDC_EXP_VCOLORS,exp->exportVertexColor);
		CheckDlgButton(hWnd,IDC_EXP_TEXCOORD,exp->exportTexCoords);
		CheckDlgButton(hWnd,IDC_EXP_MAPCHAN,exp->exportMappingChannel);
		CheckDlgButton(hWnd,IDC_EXP_MATERIAL,exp->exportMaterials);
		CheckDlgButton(hWnd,IDC_EXP_SPLINES,exp->exportSplines);
		CheckDlgButton(hWnd,IDC_EXP_MODIFIERS,exp->exportModifiers);
		CheckDlgButton(hWnd,IDC_EXP_SAMPLECONT,exp->forceSample);
		CheckDlgButton(hWnd,IDC_EXP_CONSTRAINTS,exp->exportConstraints);
		CheckDlgButton(hWnd,IDC_EXP_SKIN,exp->exportSkin);
		CheckDlgButton(hWnd,IDC_EXP_GENMOD,exp->exportGenMod);
		CheckDlgButton(hWnd,IDC_EXP_OBJECTSPACE,exp->exportObjectSpace);
		CheckDlgButton(hWnd,IDC_EXP_QUATERNIONS,exp->exportQuaternions);
		CheckDlgButton(hWnd,IDC_EXP_RELATIVE,exp->exportRelative);


		EnableWindow(GetDlgItem(hWnd, IDC_EXP_NORMALS), exp->exportGeom);
		EnableWindow(GetDlgItem(hWnd, IDC_EXP_FACESMGRP), exp->exportGeom);
		EnableWindow(GetDlgItem(hWnd, IDC_EXP_VCOLORS),  exp->exportGeom);
		EnableWindow(GetDlgItem(hWnd, IDC_EXP_TEXCOORD),  exp->exportGeom);
		EnableWindow(GetDlgItem(hWnd, IDC_EXP_MAPCHAN),  exp->exportGeom);
		EnableWindow(GetDlgItem(hWnd, IDC_EXP_OBJECTSPACE),  exp->exportGeom);

		EnableWindow(GetDlgItem(hWnd, IDC_EXP_CONSTRAINTS),  exp->exportControllers);
		EnableWindow(GetDlgItem(hWnd, IDC_EXP_SAMPLECONT),  exp->exportControllers);
		EnableWindow(GetDlgItem(hWnd, IDC_EXP_QUATERNIONS), exp->exportControllers);
		EnableWindow(GetDlgItem(hWnd, IDC_EXP_RELATIVE), exp->exportControllers);

		EnableWindow(GetDlgItem(hWnd, IDC_EXP_SKIN),  exp->exportModifiers);
		EnableWindow(GetDlgItem(hWnd, IDC_EXP_GENMOD),  exp->exportModifiers);

		//Versioning
		TCHAR Title [256];
		_stprintf(Title,"RPGsky Exporter version %.3f",exp->version);
		SetWindowText(hWnd,Title);
		return TRUE;

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDC_EXP_GEOMETRY:
			EnableWindow(GetDlgItem(hWnd, IDC_EXP_NORMALS), IsDlgButtonChecked(hWnd, IDC_EXP_GEOMETRY));
			EnableWindow(GetDlgItem(hWnd, IDC_EXP_FACESMGRP), IsDlgButtonChecked(hWnd, IDC_EXP_GEOMETRY));
			EnableWindow(GetDlgItem(hWnd, IDC_EXP_VCOLORS),  IsDlgButtonChecked(hWnd, IDC_EXP_GEOMETRY));
			EnableWindow(GetDlgItem(hWnd, IDC_EXP_TEXCOORD),  IsDlgButtonChecked(hWnd, IDC_EXP_GEOMETRY));
			EnableWindow(GetDlgItem(hWnd, IDC_EXP_MAPCHAN),  IsDlgButtonChecked(hWnd, IDC_EXP_GEOMETRY));
			EnableWindow(GetDlgItem(hWnd, IDC_EXP_OBJECTSPACE),  IsDlgButtonChecked(hWnd, IDC_EXP_GEOMETRY));
			break;
		case IDC_EXP_CONTROLLERS:
			EnableWindow(GetDlgItem(hWnd, IDC_EXP_CONSTRAINTS), IsDlgButtonChecked(hWnd, IDC_EXP_CONTROLLERS));
			EnableWindow(GetDlgItem(hWnd, IDC_EXP_SAMPLECONT),  IsDlgButtonChecked(hWnd, IDC_EXP_CONTROLLERS));
			EnableWindow(GetDlgItem(hWnd, IDC_EXP_QUATERNIONS), IsDlgButtonChecked(hWnd, IDC_EXP_CONTROLLERS));
			EnableWindow(GetDlgItem(hWnd, IDC_EXP_RELATIVE), IsDlgButtonChecked(hWnd, IDC_EXP_CONTROLLERS));
			break;
		case IDC_EXP_MODIFIERS:
			EnableWindow(GetDlgItem(hWnd, IDC_EXP_SKIN), IsDlgButtonChecked(hWnd, IDC_EXP_MODIFIERS));
			EnableWindow(GetDlgItem(hWnd, IDC_EXP_GENMOD),  IsDlgButtonChecked(hWnd, IDC_EXP_MODIFIERS));
			break;
		case IDC_EXP_NORMALS:
			break;
		case IDOK:
			exp->exportGeom = IsDlgButtonChecked(hWnd, IDC_EXP_GEOMETRY);
			exp->exportNormals = IsDlgButtonChecked(hWnd, IDC_EXP_NORMALS);
			exp->exportControllers = IsDlgButtonChecked(hWnd, IDC_EXP_CONTROLLERS);
			exp->exportFaceSmgp = IsDlgButtonChecked(hWnd, IDC_EXP_FACESMGRP);
			exp->exportVertexColor = IsDlgButtonChecked(hWnd, IDC_EXP_VCOLORS);
			exp->exportTexCoords = IsDlgButtonChecked(hWnd, IDC_EXP_TEXCOORD);
			exp->exportMappingChannel = IsDlgButtonChecked(hWnd, IDC_EXP_MAPCHAN);
			exp->exportMaterials = IsDlgButtonChecked(hWnd, IDC_EXP_MATERIAL);
			exp->exportSplines = IsDlgButtonChecked(hWnd, IDC_EXP_SPLINES);
			exp->exportModifiers = IsDlgButtonChecked(hWnd, IDC_EXP_MODIFIERS);
			exp->forceSample = IsDlgButtonChecked(hWnd, IDC_EXP_SAMPLECONT);
			exp->exportConstraints = IsDlgButtonChecked(hWnd, IDC_EXP_CONSTRAINTS);
			exp->exportSkin = IsDlgButtonChecked(hWnd, IDC_EXP_SKIN);
			exp->exportGenMod = IsDlgButtonChecked(hWnd, IDC_EXP_GENMOD);
			exp->exportQuaternions = IsDlgButtonChecked(hWnd,IDC_EXP_QUATERNIONS);
			exp->exportObjectSpace = IsDlgButtonChecked(hWnd,IDC_EXP_OBJECTSPACE);
			exp->exportRelative = IsDlgButtonChecked(hWnd,IDC_EXP_RELATIVE);

			spin = GetISpinner(GetDlgItem(hWnd, IDC_STATIC_FRAME_SPIN)); 
			exp->staticFrame = spin->GetIVal(); 
			ReleaseISpinner(spin);
			spin = GetISpinner(GetDlgItem(hWnd, IDC_SAMPLE_FRAME_SPIN)); 
			exp->framePerSample = spin->GetIVal(); 
			ReleaseISpinner(spin);
			spin = GetISpinner(GetDlgItem(hWnd, IDC_SCALE_SPIN)); 
			exp->exportScale = spin->GetFVal(); 
			ReleaseISpinner(spin);
			EndDialog(hWnd, 1);
			break;
		case IDCANCEL:
		case WM_CLOSE:
			EndDialog(hWnd,0);
			break;
		}
	}
	return 0;
}


//--- Max2009ModelExporter -------------------------------------------------------
Max2009ModelExporter::Max2009ModelExporter()
{

}

Max2009ModelExporter::~Max2009ModelExporter() 
{

}

// Dummy function for progress bar
DWORD WINAPI fn(LPVOID arg)
{
	return(0);
}

class MyErrorProc : public IGameErrorCallBack
{
public:
	void ErrorProc(IGameError error)
	{
		TCHAR * buf = GetLastIGameErrorText();
		DebugPrint("ErrorCode = %d ErrorText = %s\n", error,buf);
	}
};

int	Max2009ModelExporter::DoExport(const TCHAR *name,ExpInterface *ei,Interface *i, BOOL suppressPrompts, DWORD options)
{
	#pragma message(TODO("Implement the actual file Export here and"))

	HRESULT hr;

	Interface * ip = GetCOREInterface();

	MyErrorProc pErrorProc;

	// INIT
	strcpy(strFilename, name);
	//m_Log.SetFilename("F:\\max\\temp\\log.txt");

	UserCoord Whacky = {
		1,	//Right Handed
		1,	//X axis goes right
		3,	//Y Axis goes down
		4,	//Z Axis goes in.
		1,	//U Tex axis is right
		1,  //V Tex axis is Down
	};	

	SetErrorCallBack(&pErrorProc);

	ReadConfig();
	hr = CoInitialize(NULL); 
	// Check the return value, hr...
	//hr = CoCreateInstance(CLSID_DOMDocument, NULL, CLSCTX_INPROC_SERVER,  IID_IXMLDOMDocument, (void**)&pXMLDoc);
	//if(FAILED(hr))
	//	return false;
	// Check the return value, hr...
	//hr = pXMLDoc->QueryInterface(IID_IXMLDOMNode, (void **)&pRoot);
	//if(FAILED(hr))
	//	return false;

	// Set a global prompt display switch
	showPrompts = suppressPrompts ? FALSE : TRUE;

	exportSelected = (options & SCENE_EXPORT_SELECTED) ? true : false;
	version  = GetIGameVersion();

	DebugPrint("3ds max compatible version %.2f%\n",GetSupported3DSVersion());

	if(showPrompts) 
	{
		// Prompt the user with our dialogbox, and get all the options.
		if (!DialogBoxParam(hInstance, MAKEINTRESOURCE(IDD_PANEL),
			i->GetMAXHWnd(), Max2009ModelExporterOptionsDlgProc, (LPARAM)this)) {
				return 1;
		}
	}

	curNode = 0;
	ip->ProgressStart(_T("Exporting Using IGame.."), TRUE, fn, NULL);

	// 建立IGame
	m_pIgame = GetIGameInterface();

	// 转换坐标系统
	IGameConversionManager * cm = GetConversionManager();
	//	cm->SetUserCoordSystem(Whacky);  // 自定义坐标系
	cm->SetCoordSystem(IGameConversionManager::IGAME_MAX);
	//	m_pIgame->SetPropertyFile(_T("hello world"));
	m_pIgame->InitialiseIGame(exportSelected);

	m_pIgame->SetStaticFrame(staticFrame);

	TSTR path,file,ext;

	SplitFilename(TSTR(name),&path,&file,&ext);

	splitPath = path;



	int n1=m_pIgame->GetSceneStartTime()/m_pIgame->GetSceneTicks();
	int n2=m_pIgame->GetSceneEndTime()/m_pIgame->GetSceneTicks();
	int n3=GetFrameRate();
	int n4=m_pIgame->GetSceneTicks();

	// 初始化模型
	CModelData model;
	// Set the anim list.
	ReadAnimList(model.m_AnimList);
	model.m_Mesh.m_Lods.resize(1);
	nMaterialsCount = 0;
	// 堆积数据
	if (exportConstraints)
	{
		for(int loop = 0; loop <m_pIgame->GetTopLevelNodeCount();loop++)
		{
			IGameNode * pGameNode = m_pIgame->GetTopLevelNode(loop);
			//check for selected state - we deal with targets in the light/camera section
			if(pGameNode->IsTarget())
				continue;
			DumpChildBone(pGameNode,model.m_Skeleton);
		}
	}
	curNode = 0;
	for(int loop = 0; loop <m_pIgame->GetTopLevelNodeCount();loop++)
	{
		IGameNode * pGameNode = m_pIgame->GetTopLevelNode(loop);
		//check for selected state - we deal with targets in the light/camera section
		if(pGameNode->IsTarget())
			continue;
		DumpChildNodeInfo(pGameNode,model);
	}

	//model.m_ColorAnims.resize( nMaterialsCount );

	//	ExportSceneInfo();
	//ExportMaterials( model );
	//
	//
	//	for(int loop = 0; loop <m_pIgame->GetTopLevelNodeCount();loop++)
	//	{
	//		IGameNode * pGameNode = m_pIgame->GetTopLevelNode(loop);
	//		//check for selected state - we deal with targets in the light/camera section
	//		if(pGameNode->IsTarget())
	//			continue;
	//		ExportNodeInfo(pGameNode);
	//
	//	}
	m_pIgame->ReleaseIGame();
	//	pXMLDoc->save(CComVariant(name));
	////	pXMLDoc->Release();

	// 输出数据
	model.SaveFile(name);
	CoUninitialize();
	ip->ProgressEnd();	
	WriteConfig();
	//
	{
		HKEY hKey;
		if (ERROR_SUCCESS==RegOpenKeyExW(HKEY_LOCAL_MACHINE,L"software\\rpgsky\\modelview\\",
			0, KEY_READ, &hKey))
		{
			DWORD dwType = REG_SZ;
			wchar_t wszFilename[256];
			DWORD dwSize = sizeof(wszFilename);

			if (ERROR_SUCCESS==RegQueryValueExW(hKey, L"filename",
				NULL, &dwType, (PBYTE)&wszFilename, &dwSize))
			{
				std::wstring wstrParameter = L"\""+s2ws(name)+L"\"";

				ShellExecuteW(NULL,L"open",wszFilename,wstrParameter.c_str(),
					GetParentPath(wszFilename).c_str(),SW_SHOWNORMAL); 
			}
			RegCloseKey(hKey);
		}
	}

	return TRUE;

	//if(!suppressPrompts)
	//	DialogBoxParam(hInstance, 
	//			MAKEINTRESOURCE(IDD_PANEL), 
	//			GetActiveWindow(), 
	//			Max2009ModelExporterOptionsDlgProc, (LPARAM)this);

	#pragma message(TODO("return TRUE If the file is exported properly"))
	return FALSE;
}

void Max2009ModelExporter::DumpMaterial( CModelData& model, IGameMaterial * mat, int index, int matID )
{
	TSTR buf;
	IGameProperty *prop;

	if(matID !=-1)	// we are not a sub material
	{
		//buf.printf("%d",matID);
	}

	//mat->GetMaterialName();


	//WE ONLY WANT THE PROPERTIES OF THE ACTUAL MATERIAL NOT THE CONTAINER - FOR NOW.....
	if( !mat->IsMultiType() && matID !=-1 )
	{
		prop = mat->GetDiffuseData();
		//DumpProperties(propNode,prop);
		if(prop->GetType() == IGAME_POINT3_PROP)
		{
			Point3 p; 
			prop->GetPropertyValue(p);
			//Buf.printf("%f %f %f",p.x,p.y,p.z);

			model.m_ColorAnims[matID].color.type = INTERPOLATION_LINEAR;
			model.m_ColorAnims[matID].color.seq = -1;
			model.m_ColorAnims[matID].color.m_KeyData.resize( 1 );
			model.m_ColorAnims[matID].color.m_KeyData[0] = Vec3D( p.x, p.y, p.z );
		}

		//prop = mat->GetAmbientData();
		//DumpProperties(propNode,prop);
		//prop = mat->GetSpecularData();
		//DumpProperties(propNode,prop);
		//prop = mat->GetOpacityData();
		//DumpProperties(propNode,prop);
		//prop = mat->GetGlossinessData();
		//DumpProperties(propNode,prop);
		//prop = mat->GetSpecularLevelData();
		//DumpProperties(propNode,prop);
	}

	//do the textures if they are there

	////	DumpTexture(mat);

	if(mat->IsMultiType())
	{
		for(int k=0;k<mat->GetSubMaterialCount();k++)
		{
			int mID = mat->GetMaterialID(k);
			IGameMaterial * subMat = mat->GetSubMaterial(k);
			DumpMaterial( model, subMat,k, mID );
		}
	}

}
void Max2009ModelExporter::DumpChildBone(IGameNode * child, CSkeleton& skeleton )
{
	TSTR buf;

	curNode ++;
	buf = TSTR("Processing: ") + TSTR(child->GetName());
	GetCOREInterface()->ProgressUpdate((int)((float)curNode/m_pIgame->GetTotalNodeCount()*100.0f),FALSE,buf.data()); 

	if(child->IsGroupOwner())
	{
	}
	else
	{
		IGameObject * obj = child->GetIGameObject();
		IGameObject::MaxType T = obj->GetMaxType();
		switch(obj->GetIGameType())
		{
		case IGameObject::IGAME_BONE:
		case IGameObject::IGAME_HELPER:
		//case IGameObject::IGAME_MESH:
			{
				////			CComPtr <IXMLDOMNode> boneNode;
				////			CreateXMLNode(pXMLDoc,parent,_T("BoneData"),&boneNode);
				//AddXMLAttribute(parent,_T("NodeType"),_T("Bone"));
				//IGameSupportObject * hO = (IGameSupportObject*)obj;
				//IGameMesh * hm = hO->GetMeshObject();
				//if(hm->InitializeData())
				//{
				//	CreateXMLNode(pXMLDoc,parent,_T("GeomData"),&geomData);
				//	DumpMesh(hm,geomData);
				//}
				if(exportControllers)
				{
					BoneAnim boneAnim;
					boneAnim.strName = child->GetName();
					if (child->GetNodeParent())
					{
						boneAnim.parent = skeleton.getIDByName(child->GetNodeParent()->GetName());
					}
					else
					{
						INode* pNode = child->GetMaxNode()->GetParentNode();
						if (pNode)
						{
							boneAnim.parent = skeleton.getIDByName(pNode->GetName());
						}
					}
					IGameControl * pGameControl = child->GetIGameControl();
					// base matrix
					{
						GMatrix mWorld = child->GetWorldTM();
						boneAnim.mSkin = *(Matrix*)&child->GetWorldTM();
						boneAnim.mSkin.transpose();
						boneAnim.mSkin._14*=exportScale;
						boneAnim.mSkin._24*=exportScale;
						boneAnim.mSkin._34*=exportScale;

						AffineParts ap;
						Matrix3 m = child->GetWorldTM().ExtractMatrix3();//Even though its a 4x4 we dump it as a 4x3 ;-)
						decomp_affine(m, &ap);
						boneAnim.mSkin = Matrix::newTranslation(fixCoordSystemPos(*(Vec3D*)(&ap.t)*exportScale))*
							Matrix::newQuatRotate(fixCoordSystemRotate(ap.q))*
							Matrix::newScale(*(Vec3D*)(&ap.k));

						boneAnim.mSkin.Invert();
					}
	
					if (false==DumpAnim(pGameControl, boneAnim))
					{
						boneAnim.trans.m_KeyTimes.push_back(0);
						boneAnim.rot.m_KeyTimes.push_back(0);
						boneAnim.scale.m_KeyTimes.push_back(0);

						AffineParts ap;
						Matrix3 m = child->GetLocalTM().ExtractMatrix3();
						decomp_affine(m, &ap);

						boneAnim.trans.m_KeyData.push_back(fixCoordSystemPos(*(Vec3D*)(&ap.t)*exportScale));
						boneAnim.rot.m_KeyData.push_back(fixCoordSystemRotate(ap.q));
						boneAnim.scale.m_KeyData.push_back( *(Vec3D*)(&ap.k));

						//Warn(TSTR("DumpAnim: ") + TSTR(child->GetName()));
					}
					else
					{
						//AffineParts ap;
						//Matrix3 m = child->GetLocalTM().ExtractMatrix3();
						//decomp_affine(m, &ap);
						//for (size_t i=0;i<boneAnim.trans.m_KeyData.size();++i)
						//{
						//	boneAnim.trans.m_KeyData[i]+=*(Vec3D*)(&ap.t)*exportScale;
						//}
						//for (size_t i=0;i<boneAnim.rot.m_KeyData.size();++i)
						//{
						//	boneAnim.rot.m_KeyData[i]=(*(Quaternion*)(&ap.q)).Multiply(boneAnim.rot.m_KeyData[i]);
						//}
						//for (size_t i=0;i<boneAnim.scale.m_KeyData.size();++i)
						//{
						//	//boneAnim.scale.m_KeyData[i]*=( *(Vec3D*)(&ap.k));
						//}
					}
					skeleton.m_BoneAnims.push_back(boneAnim);
				}
				break;
			}
		}
	}
	for(int i=0;i<child->GetChildCount();i++)
	{
		IGameNode * newchild = child->GetNodeChild(i);
		// we deal with targets in the light/camera section
		if(newchild->IsTarget())
			continue;
		DumpChildBone( newchild, skeleton );
	}
	child->ReleaseIGameObject();
}


void Max2009ModelExporter::DumpChildNodeInfo( IGameNode * child, CModelData& model )
{
	TSTR buf,data;

	curNode ++;
	buf = TSTR("Processing: ") + TSTR(child->GetName());
	GetCOREInterface()->ProgressUpdate((int)((float)curNode/m_pIgame->GetTotalNodeCount()*100.0f),FALSE,buf.data()); 


	if(child->IsGroupOwner())
	{
	}
	else
	{
		TSTR b(child->GetName());
		//m_Log.Log("name:%s; ",b);

		//TCHAR nb[256]={""};
		//stripWhiteSpace(&b,*nb);
		//AddXMLAttribute(parent,_T("Name"),nb);
		//buf.printf("%d",child->GetNodeID());
		//AddXMLAttribute(parent,_T("NodeID"),buf.data());

		//IGameNode * p = child->GetNodeParent();
		//if(p)
		//{
		//	buf.printf("%d",p->GetNodeID());
		//	AddXMLAttribute(parent,_T("ParentNodeID"),buf.data());
		//}

		//CreateXMLNode(pXMLDoc,parent,_T("NodeTM"),&tmNodeParent);

		//DumpMatrix(child->GetWorldTM(staticFrame).ExtractMatrix3(),tmNodeParent);

		//ULONG  handle = child->GetMaxNode()->GetHandle();

		//if(child->GetMaterialIndex() !=-1){
		//	CreateXMLNode(pXMLDoc,parent,_T("MaterialIndex"),&matIndex);
		//	buf.printf("%d",child->GetMaterialIndex());
		//	AddXMLText(pXMLDoc,matIndex,buf.data());
		//}

		IGameObject * obj = child->GetIGameObject();

		IGameObject::MaxType T = obj->GetMaxType();

		//bool xref = obj->IsObjectXRef();

		//if(xref)
		//{
		//	AddXMLAttribute(parent,_T("XRefObject"),_T("True"));
		//}

		switch(obj->GetIGameType())
		{
		case IGameObject::IGAME_BONE:
			{
				break;
			}
		case IGameObject::IGAME_HELPER:
			{
				////			CComPtr <IXMLDOMNode> boneNode;
				////			CreateXMLNode(pXMLDoc,parent,_T("BoneData"),&boneNode);
				//AddXMLAttribute(parent,_T("NodeType"),_T("Helper"));
				//IGameSupportObject * hO = (IGameSupportObject*)obj;
				//IPropertyContainer * cc = hO->GetIPropertyContainer();
				//IGameProperty * prop = cc->QueryProperty(101);
				//if(prop)
				//{
				//	TCHAR * buf;
				//	prop->GetPropertyValue(buf);
				//}
				//prop = cc->QueryProperty(_T("IGameTestString"));

				//if(prop)
				//{
				//	TCHAR * buf;
				//	prop->GetPropertyValue(buf);
				//}
				//prop = cc->QueryProperty(_T("IGameTestString"));

				//IGameMesh * hm = hO->GetMeshObject();
				//if(hm->InitializeData())
				//{
				//	CreateXMLNode(pXMLDoc,parent,_T("GeomData"),&geomData);
				//	if(splitFile)
				//	{
				//		TSTR filename;
				//		MakeSplitFilename(child,filename);
				//		CComPtr<IXMLDOMNode>subMeshNode;						
				//		AddXMLAttribute(geomData, _T("Include"),filename.data());
				//		CreateXMLNode(pSubDocMesh,pSubMesh,_T("GeomData"),&subMeshNode);
				//		AddXMLAttribute(subMeshNode,_T("Node"),child->GetName());
				//		geomData = subMeshNode;

				//	}
				//	DumpMesh(hm,geomData);
				//}
				break;
			}
		case IGameObject::IGAME_LIGHT:
			{
				//CComPtr <IXMLDOMNode> lightNode;
				//CreateXMLNode(pXMLDoc,parent,_T("lightData"),&lightNode);

				//AddXMLAttribute(parent,_T("NodeType"),_T("Light"));
				//IGameLight * l = (IGameLight*)obj;
				//if(l->GetLightType()==IGameLight::IGAME_OMNI)
				//	AddXMLAttribute(lightNode,_T("Type"),_T("Omni"));
				//else if(l->GetLightType()==IGameLight::IGAME_TSPOT)
				//	AddXMLAttribute(lightNode,_T("Type"),_T("Targeted"));
				//else if(l->GetLightType()==IGameLight::IGAME_FSPOT)
				//	AddXMLAttribute(lightNode,_T("Type"),_T("Free"));
				//else if(l->GetLightType()==IGameLight::IGAME_TDIR)
				//	AddXMLAttribute(lightNode,_T("Type"),_T("TargetedDirectional"));
				//else
				//	AddXMLAttribute(lightNode,_T("Type"),_T("Directional"));
				//DumpLight(l,lightNode);
				break;
			}
		case IGameObject::IGAME_CAMERA:
			{
				//CComPtr <IXMLDOMNode> camNode;
				//CreateXMLNode(pXMLDoc,parent,_T("CameraData"),&camNode);
				//AddXMLAttribute(parent,_T("NodeType"),_T("Camera"));
				//IGameCamera * c = (IGameCamera*)obj;
				//if(c->GetCameraTarget())
				//	AddXMLAttribute(camNode,_T("Type"),_T("Targeted"));
				//else
				//	AddXMLAttribute(camNode,_T("Type"),_T("Free"));
				//DumpCamera(c,camNode);
				break;
			}
		case IGameObject::IGAME_MESH:
			if(exportGeom )
			{
				IGameMesh * gM = (IGameMesh*)obj;
				gM->SetCreateOptimizedNormalList();
				if(gM->InitializeData())
				{
					DumpMesh(child, gM, model);
				}
				else
				{
				}
			}
			break;
		case IGameObject::IGAME_SPLINE:
			if(exportSplines)
			{
				//AddXMLAttribute(parent,_T("NodeType"),_T("SplineObject"));
				//IGameSpline * sp = (IGameSpline*)obj;
				//sp->InitializeData();
				//CreateXMLNode(pXMLDoc,parent,_T("SplineData"),&splineData);
				//DumpSpline(sp,splineData);
			}
			break;
		case IGameObject::IGAME_IKCHAIN:
			//AddXMLAttribute(parent,_T("NodeType"),_T("IKChainObject"));
			//IGameIKChain * ikch = (IGameIKChain*)obj;
			//CreateXMLNode(pXMLDoc,parent,_T("IKChain"), & ikData);
			//DumpIKChain(ikch,ikData);
			break;
		}
		//if(splitFile)
		//{
		//	TSTR buf;
		//	MakeSplitFilename(child,buf);
		//	pSubDocMesh->save(CComVariant(buf.data()));

		//}
		////dump PRS Controller data
		//prsNode = NULL;

		//// In our "Game Engine" we deal with Bezier Position, Scale and TCB Rotation controllers !!
		//// Only list controllers on position, rotation...
		//if(exportControllers)
		//{
		//	CComPtr<IXMLDOMNode>subContNode;
		//	bool exportBiped = false;
		//	CreateXMLNode(pXMLDoc,parent,_T("PRSData"),&prsNode);
		//	IGameControl * pGameControl = child->GetIGameControl();
		//	ExportControllers(prsNode,pGameControl);

		//}

		//if(exportModifiers)
		//{
		//	int numMod = obj->GetNumModifiers();
		//	if(numMod > 0)
		//	{
		//		CComPtr <IXMLDOMNode> mod;
		//		CreateXMLNode(pXMLDoc,parent,_T("Modifiers"),&mod);
		//		TSTR Buf;
		//		buf.printf("%d",numMod);
		//		AddXMLAttribute(mod,_T("count"),buf.data());

		//		for(int i=0;i<numMod;i++)
		//		{
		//			IGameModifier * m = obj->GetIGameModifier(i);
		//			DumpModifiers(mod,m);
		//		}
		//	}
		//}
	}	
	for(int i=0;i<child->GetChildCount();i++)
	{
		IGameNode * newchild = child->GetNodeChild(i);
		TSTR name;
		if(newchild->IsGroupOwner())
			name = TSTR("Group");
		else
			name = TSTR("Node");

		// we deal with targets in the light/camera section
		if(newchild->IsTarget())
			continue;

		DumpChildNodeInfo( newchild, model );
	}

	child->ReleaseIGameObject();
}


void Max2009ModelExporter::DumpSampleKeys(IGameControl * pGameControl,BoneAnim& boneAnim, IGameControlType Type)
{
	{
		IGameKeyTab Key;
		if(pGameControl->GetFullSampledKeys(Key,framePerSample,IGameControlType(IGAME_TM),true) )
		{
			for(int i=0;i<Key.Count();i++)
			{
				if (Key[i].t>=0)
				{
				int fc = Key[i].t*1000/m_pIgame->GetSceneTicks()/GetFrameRate();

				boneAnim.trans.m_KeyTimes.push_back(fc);
				boneAnim.rot.m_KeyTimes.push_back(fc);
				boneAnim.scale.m_KeyTimes.push_back(fc);

				AffineParts ap;
				Matrix3 m = Key[i].sampleKey.gval.ExtractMatrix3();//Even though its a 4x4 we dump it as a 4x3 ;-)
				decomp_affine(m, &ap);
				// Quaternions are dumped as angle axis.
				//float rotAngle;
				//Point3 rotAxis;
				//float scaleAxAngle;
				//Point3 scaleAxis;
				//AngAxisFromQ(ap.q, &rotAngle, rotAxis);
				//AngAxisFromQ(ap.u, &scaleAxAngle, scaleAxis);

				boneAnim.trans.m_KeyData.push_back(fixCoordSystemPos(*(Vec3D*)(&ap.t)*exportScale));
				boneAnim.rot.m_KeyData.push_back(fixCoordSystemRotate(ap.q));
				boneAnim.scale.m_KeyData.push_back( *(Vec3D*)(&ap.k));
				}
			}
		}
	}
	/*{
	IGameKeyTab Key;
	if(pGameControl->GetFullSampledKeys(Key,framePerSample,IGameControlType(IGAME_POS),true) )
	{
	for(int i=0;i<Key.Count();i++)
	{
	boneAnim.trans.m_KeyTimes.push_back(Key[i].t);
	boneAnim.trans.m_KeyData.push_back(*(Vec3D*)(&Key[i].sampleKey.pval));
	}
	}
	}*/
	/*{
	IGameKeyTab Key;
	if(pGameControl->GetFullSampledKeys(Key,framePerSample,IGameControlType(IGAME_ROT),true) )
	{
	for(int i=0;i<Key.Count();i++)
	{
	boneAnim.rot.m_KeyTimes.push_back(Key[i].t);
	boneAnim.rot.m_KeyData.push_back(*(Quaternion*)(&Key[i].sampleKey.qval));
	}
	}
	}*/
	/*{
	IGameKeyTab Key;
	if(pGameControl->GetFullSampledKeys(Key,framePerSample,IGameControlType(IGAME_SCALE),true) )
	{
	for(int i=0;i<Key.Count();i++)
	{
	boneAnim.scale.m_KeyTimes.push_back(Key[i].t);
	boneAnim.scale.m_KeyData.push_back( *(Vec3D*)(&Key[i].sampleKey.sval.s));
	}
	}
	}*/
}






bool Max2009ModelExporter::DumpTCBKeys(IGameControl * pGameControl,BoneAnim& boneAnim, IGameControlType Type)
{
	IGameKeyTab keys;

	if (pGameControl->GetTCBKeys(keys,Type))
	{
		switch(Type)
		{
		case IGAME_POS:
			for (int i=0; i<keys.Count(); i++)
			{
				const IGameKey& key=keys[i];
				if (key.t>=0)
				{
					Vec3D k = *(Vec3D*)(&(key.tcbKey.pval));
					k*=exportScale;
					boneAnim.trans.m_KeyTimes.push_back(key.t*1000/m_pIgame->GetSceneTicks()/GetFrameRate());
					boneAnim.trans.m_KeyData.push_back(fixCoordSystemPos(k));
				}
			}
			break;
		case IGAME_ROT:
			for (int i=0; i<keys.Count(); i++)
			{
				const IGameKey& key=keys[i];
				//Quaternion q = *(Quaternion*)(&(key.tcbKey.qval));
				//AngAxis a = Keys[i].tcbKey.aval;
				//data.printf("%f %f %f %f",a.axis.x, a.axis.y, a.axis.z, a.angle);
				//boneAnim.rot.m_KeyTimes.push_back(key.t);
				//boneAnim.rot.m_KeyData.push_back(q);
			}
			break;
		case IGAME_SCALE:
			for (int i=0; i<keys.Count(); i++)
			{
				const IGameKey& key=keys[i];
				if (key.t>=0)
				{
					Vec3D s = *(Vec3D*)(&(key.tcbKey.sval.s));
					boneAnim.scale.m_KeyTimes.push_back(key.t*1000/m_pIgame->GetSceneTicks()/GetFrameRate());
					boneAnim.scale.m_KeyData.push_back(s);
				}
			}
			break;
		default:
			break;
		}
		return true;
	}
	return false;
}
bool Max2009ModelExporter::DumpLinearKeys(IGameControl * pGameControl,BoneAnim& boneAnim, IGameControlType Type)
{
	IGameKeyTab keys;

	if (pGameControl->GetLinearKeys(keys,Type))
	{
		switch(Type)
		{
		case IGAME_POS:
			for (int i=0; i<keys.Count(); i++)
			{
				const IGameKey& key=keys[i];
				if (key.t>=0)
				{
					Vec3D k = *(Vec3D*)(&(key.linearKey.pval));
					k*=exportScale;
					boneAnim.trans.m_KeyTimes.push_back(key.t*1000/m_pIgame->GetSceneTicks()/GetFrameRate());
					boneAnim.trans.m_KeyData.push_back(fixCoordSystemPos(k));
				}
			}
			break;
		case IGAME_ROT:
			for (int i=0; i<keys.Count(); i++)
			{
				const IGameKey& key=keys[i];
				if (key.t>=0)
				{
					boneAnim.rot.m_KeyTimes.push_back(key.t*1000/m_pIgame->GetSceneTicks()/GetFrameRate());
					boneAnim.rot.m_KeyData.push_back(fixCoordSystemRotate(key.linearKey.qval));
				}
			}
			break;
		case IGAME_SCALE:
			for (int i=0; i<keys.Count(); i++)
			{
				const IGameKey& key=keys[i];
				if (key.t>=0)
				{
					Vec3D s = *(Vec3D*)(&(key.linearKey.sval.s));
					boneAnim.scale.m_KeyTimes.push_back(key.t*1000/m_pIgame->GetSceneTicks()/GetFrameRate());
					boneAnim.scale.m_KeyData.push_back(s);
				}
			}
			break;
		default:
			break;
		}
		return true;
	}
	return false;
}
bool Max2009ModelExporter::DumpBezierKeys(IGameControl * pGameControl,BoneAnim& boneAnim, IGameControlType Type)
{
	IGameKeyTab keys;

	if (pGameControl->GetBezierKeys(keys,Type))
	{
		switch(Type)
		{
		case IGAME_POS:
			for (int i=0; i<keys.Count(); i++)
			{
				const IGameKey& key=keys[i];
				if (key.t>=0)
				{
					Vec3D k = *(Vec3D*)(&(key.bezierKey.pval));
					k*=exportScale;
					boneAnim.trans.m_KeyTimes.push_back(key.t*1000/m_pIgame->GetSceneTicks()/GetFrameRate());
					boneAnim.trans.m_KeyData.push_back(fixCoordSystemPos(k));
				}
			}
			break;
		case IGAME_ROT:
			for (int i=0; i<keys.Count(); i++)
			{
				const IGameKey& key=keys[i];
				if (key.t>=0)
				{
					boneAnim.rot.m_KeyTimes.push_back(key.t*1000/m_pIgame->GetSceneTicks()/GetFrameRate());
					boneAnim.rot.m_KeyData.push_back(fixCoordSystemRotate(key.bezierKey.qval));
				}
			}
			break;
		case IGAME_SCALE:
			for (int i=0; i<keys.Count(); i++)
			{
				const IGameKey& key=keys[i];
				if (key.t>=0)
				{
					Vec3D s = *(Vec3D*)(&(key.bezierKey.sval.s));
					boneAnim.scale.m_KeyTimes.push_back(key.t*1000/m_pIgame->GetSceneTicks()/GetFrameRate());
					boneAnim.scale.m_KeyData.push_back(s);
				}
			}
			break;
		default:
			break;
		}
		return true;
	}
	return false;
}

bool Max2009ModelExporter::DumpPos(IGameControl * pGameControl,BoneAnim& boneAnim)
{
	IGameControl::MaxControlType T = pGameControl->GetControlType(IGAME_POS);
	if (T==IGameControl::IGAME_MAXSTD&&DumpBezierKeys(pGameControl,boneAnim,IGAME_POS))
	{
	}
	else if (T==IGameControl::IGAME_MAXSTD&&DumpLinearKeys(pGameControl,boneAnim,IGAME_POS))
	{
	}
	else if(T==IGameControl::IGAME_INDEPENDENT_POS)
	{
		//Warn(TSTR("IGAME_INDEPENDENT_POS:"));
		Animated<float> anim[3];
		std::set<unsigned long> setTime;
		for (int id=0;id<3;++id)
		{
			IGameKeyTab keys;
			if(pGameControl->GetBezierKeys(keys,IGameControlType(IGAME_POS_X+id)))
			{
				for (int i=0; i<keys.Count(); i++)
				{
					const IGameKey& key=keys[i];
					float k = key.bezierKey.fval*exportScale;
					unsigned long uTime = key.t*1000/m_pIgame->GetSceneTicks()/GetFrameRate();
					setTime.insert(uTime);
					anim[id].m_KeyTimes.push_back(uTime);
					anim[id].m_KeyData.push_back(k);
				}
			}
			else if(pGameControl->GetLinearKeys(keys,IGameControlType(IGAME_POS_X+id)))
			{
				for (int i=0; i<keys.Count(); i++)
				{
					const IGameKey& key=keys[i];
					float k = key.linearKey.fval*exportScale;
					unsigned long uTime = key.t*1000/m_pIgame->GetSceneTicks()/GetFrameRate();
					setTime.insert(uTime);
					anim[id].m_KeyTimes.push_back(uTime);
					anim[id].m_KeyData.push_back(k);
				}
			}
			else
			{
				Warn(TSTR("IGAME_INDEPENDENT_POS::IGAME_POS_?"));
			}
		}
		for(std::set<unsigned long>::iterator it=setTime.begin();it!=setTime.end();it++)
		{
			boneAnim.trans.m_KeyTimes.push_back(*it);
			Vec3D vPos;
			vPos.x = anim[0].getValue(*it);
			vPos.y = anim[1].getValue(*it);
			vPos.z = anim[2].getValue(*it);
			boneAnim.trans.m_KeyData.push_back(fixCoordSystemPos(vPos));
		}
	}
	else if(T==IGameControl::IGAME_POS_CONSTRAINT)
	{
		Warn(TSTR("IGAME_POS_CONSTRAINT:"));
		return false;
	}
	else if(T==IGameControl::IGAME_LIST)
	{
		int subNum = pGameControl->GetNumOfListSubControls(IGAME_POS);
		for(int i=0;i<subNum;i++)
		{
			IGameControl * sub = pGameControl->GetListSubControl(i,IGAME_POS);
			if (DumpPos(sub,boneAnim))
			{
				return true;
			}
		}
		Warn(TSTR("Pos,IGAME_LIST:"));
		return false;
	}
	else
	{
		Warn(TSTR("Pos,IGAME_UNKNOWN:"));
		return false;
	}
	return true;
}

bool Max2009ModelExporter::DumpRot(IGameControl * pGameControl,BoneAnim& boneAnim)
{
	IGameControl::MaxControlType T = pGameControl->GetControlType(IGAME_ROT);
	if(T==IGameControl::IGAME_MAXSTD&&DumpTCBKeys(pGameControl,boneAnim,IGAME_ROT))
	{
	}
	else if(T==IGameControl::IGAME_MAXSTD&&DumpBezierKeys(pGameControl,boneAnim,IGAME_ROT))
	{
	}
	else if(T==IGameControl::IGAME_MAXSTD&&DumpLinearKeys(pGameControl,boneAnim,IGAME_ROT))
	{
	}
	else if(T==IGameControl::IGAME_ROT_CONSTRAINT)
	{
		Warn(TSTR("IGAME_ROT_CONSTRAINT:"));
		return false;
	}
	else if(T==IGameControl::IGAME_LIST)
	{
		int subNum = pGameControl->GetNumOfListSubControls(IGAME_ROT);
		for(int i=0;i<subNum;i++)
		{
			IGameControl * sub = pGameControl->GetListSubControl(i,IGAME_ROT);
			if (DumpRot(sub,boneAnim))
			{
				return true;
			}
		}
		Warn(TSTR("Pos,IGAME_LIST:"));
		return false;
	}
	else if(T==IGameControl::IGAME_EULER)
	{
		//Warn(TSTR("IGAME_EULER:"));
		Animated<float> anim[3];
		std::set<unsigned long> setTime;
		for (int id=0;id<3;++id)
		{
			IGameKeyTab keys;
			if(pGameControl->GetBezierKeys(keys,IGameControlType(IGAME_EULER_X+id)))
			{
				for (int i=0; i<keys.Count(); i++)
				{
					const IGameKey& key=keys[i];
					if (key.t>=0)
					{
						float k = key.bezierKey.fval;
						unsigned long uTime = key.t*1000/m_pIgame->GetSceneTicks()/GetFrameRate();
						setTime.insert(uTime);
						anim[id].m_KeyTimes.push_back(uTime);
						anim[id].m_KeyData.push_back(k);
					}
				}
			}
			else if(pGameControl->GetLinearKeys(keys,IGameControlType(IGAME_EULER_X+id)))
			{
				for (int i=0; i<keys.Count(); i++)
				{
					const IGameKey& key=keys[i];
					if (key.t>=0)
					{
						float k = key.linearKey.fval;
						unsigned long uTime = key.t*1000/m_pIgame->GetSceneTicks()/GetFrameRate();
						setTime.insert(uTime);
						anim[id].m_KeyTimes.push_back(uTime);
						anim[id].m_KeyData.push_back(k);
					}
				}
			}
			else
			{
				Warn(TSTR("IGAME_EULER::IGAME_EULER_?"));
			}
		}
		for(std::set<unsigned long>::iterator it=setTime.begin();it!=setTime.end();it++)
		{
			boneAnim.rot.m_KeyTimes.push_back(*it);
			//float x = anim[0].getValue(*it);
			//float y = anim[1].getValue(*it);
			//float z = anim[2].getValue(*it);
			//Quaternion q;
			////q.rotate(Vec3D(-y,x,z));
			//q.rotate(Vec3D(x,y,z));
			//boneAnim.rot.m_KeyData.push_back(Quaternion(q.x, q.z, -q.y, q.w));
			//Quat q;
			//q.SetEuler(x,y,z);
			//boneAnim.rot.m_KeyData.push_back(*(Quaternion*)(&q));
			float ang[3];
			ang[0] = anim[0].getValue(*it);
			ang[1] = anim[1].getValue(*it);
			ang[2] = anim[2].getValue(*it);
			Quat q;
			EulerToQuat(ang,q);
			Quaternion q1;q1=*(Quaternion*)(&q);

			boneAnim.rot.m_KeyData.push_back(fixCoordSystemRotate(q));
		}
	}
	else
	{
		Warn(TSTR("Rot,IGAME_UNKNOWN:"));
		return false;
		//if(IGameControl::IGAME_UNKNOWN)
		//DumpSampleKeys(pGameControl,node, IGAME_ROT);
	}
	return true;
}

bool Max2009ModelExporter::DumpScale(IGameControl * pGameControl,BoneAnim& boneAnim)
{
	IGameControl::MaxControlType T= pGameControl->GetControlType(IGAME_SCALE);
	if(T==IGameControl::IGAME_MAXSTD && DumpBezierKeys(pGameControl,boneAnim,IGAME_SCALE))
	{
	}
	else
	{
		Warn(TSTR("Scale,IGAME_UNKNOWN:"));
		return false;
		//if(T==IGameControl::IGAME_UNKNOWN)
		//DumpSampleKeys(pGameControl,node, IGAME_SCALE);
	}
}

bool Max2009ModelExporter::DumpAnim(IGameControl * pGameControl,BoneAnim& boneAnim)
{
	if ((pGameControl->IsAnimated(IGAME_POS)) || pGameControl->IsAnimated(IGAME_ROT) || pGameControl->IsAnimated(IGAME_SCALE))
	{
		if(pGameControl->GetControlType(IGAME_POS)==IGameControl::IGAME_BIPED||
			pGameControl->GetControlType(IGAME_ROT)==IGameControl::IGAME_BIPED||
			pGameControl->GetControlType(IGAME_SCALE)==IGameControl::IGAME_BIPED)
		{
			DumpSampleKeys(pGameControl, boneAnim, IGAME_TM);
			return true;
		}
		if(pGameControl->GetControlType(IGAME_POS)==IGameControl::IGAME_LIST||
			pGameControl->GetControlType(IGAME_ROT)==IGameControl::IGAME_LIST||
			pGameControl->GetControlType(IGAME_SCALE)==IGameControl::IGAME_LIST)
		{
			int subNum = pGameControl->GetNumOfListSubControls(IGAME_TM);
			if(subNum)
			{
				Warn(TSTR("GetListSubControl:"));
				for(int i=0;i<subNum;i++)
				{
					IGameControl * sub = pGameControl->GetListSubControl(i,IGAME_TM);
					if (DumpAnim(sub,boneAnim))
					{
						return true;
					}
				}
			}
		}

		{
			if (pGameControl->IsAnimated(IGAME_POS))
			{
				DumpPos(pGameControl, boneAnim);
			}
			if (pGameControl->IsAnimated(IGAME_ROT))
			{
				DumpRot(pGameControl, boneAnim);
			}
			if (pGameControl->IsAnimated(IGAME_SCALE))
			{
				DumpScale(pGameControl, boneAnim);
			}
		}
	}
	else
	{
		return false;
	}
	return true;
}

void Max2009ModelExporter::DumpSampleKeys(IGameControl * sc,BoneAnim& boneAnim, IGameControlType Type, bool quick, Point3 pos)
{
	Tab<Matrix3>sKey;  
	Tab<GMatrix>gKey;
	IGameKeyTab Key;
	IGameControl * c = sc;
	TSTR Buf;
	bool relative = false;

	//TSTR name;
	//GetKeyTypeName(name,Type);

	if(!c)
		return;

	relative = exportRelative ? true : false;

	if(!quick && c->GetFullSampledKeys(Key,framePerSample,IGameControlType(Type),relative) )
	{
		for(int i=0;i<Key.Count();i++)
		{
			if (Key[i].t>=0)
			{
				int fc = Key[i].t*1000/m_pIgame->GetSceneTicks()/GetFrameRate();//frame
				if(Type ==IGAME_POS)
				{
					Vec3D k = *(Vec3D*)(&(Key[i].sampleKey.pval-pos));
					boneAnim.trans.m_KeyTimes.push_back(fc);
					boneAnim.trans.m_KeyData.push_back(fixCoordSystemPos(k));
				}
				if(Type == IGAME_ROT)
				{
					//AngAxis a(q);
					boneAnim.rot.m_KeyTimes.push_back(fc);
					boneAnim.rot.m_KeyData.push_back(fixCoordSystemRotate(Key[i].sampleKey.qval));
					/*if(!exportQuaternions)
					Buf.printf("%f %f %f %f",a.axis.x, a.axis.y, a.axis.z, a.angle);
					else
					Buf.printf("%f %f %f %f",q.x,q.y, q.z, q.w);*/

				}
				if(Type == IGAME_SCALE)
				{
					ScaleValue sval = Key[i].sampleKey.sval;
					Vec3D s = *(Vec3D*)(&sval.s);
					AngAxis a(sval.q);
					boneAnim.scale.m_KeyTimes.push_back(fc);
					boneAnim.scale.m_KeyData.push_back(s);
					/*if(!exportQuaternions)
					Buf.printf("%f %f %f %f %f %f %f",sval.s.x,sval.s.y,sval.s.z,a.axis.x, a.axis.y, a.axis.z, a.angle);
					else
					Buf.printf("%f %f %f %f %f %f %f",sval.s.x,sval.s.y,sval.s.z,sval.q.x, sval.q.y,sval.q.z, sval.q.w);*/
				}
				if(Type == IGAME_FLOAT)
				{
					float f = Key[i].sampleKey.fval;
				}
				if(Type == IGAME_POINT3)
				{
					Point3 k = Key[i].sampleKey.pval;
				}
				if(Type == IGAME_TM)
				{
					//Even though its a 4x4 we dump it as a 4x3 ;-)
					//DumpMatrix(Key[i].sampleKey.gval.ExtractMatrix3(),data);
				}
			}
		}
	}

	//mainly for the IK On/Off controller
	/*if(quick && c->GetQuickSampledKeys(Key,IGameControlType(Type)) )
	{

		CreateXMLNode(pXMLDoc,prsNode,name,&sampleData);
		Buf.printf("%d",Key.Count());
		AddXMLAttribute(sampleData,_T("KeyCount"),Buf.data());
		AddXMLAttribute(sampleData,_T("Type"),_T("QuickSampled"));

		for(int i=0;i<Key.Count();i++)
		{
			CComPtr<IXMLDOMNode> data;
			CreateXMLNode(pXMLDoc,sampleData,_T("Sample"),&data);
			int fc = Key[i].t;
			Buf.printf("%d",fc);
			AddXMLAttribute(data,_T("frame"),Buf.data());
			if(Type == IGAME_FLOAT)
			{
				float f = Key[i].sampleKey.fval;
				Buf.printf("%f",f);	
				AddXMLText(pXMLDoc,data,Buf.data());
			}

		}
	}*/
}

/*void Max2009ModelExporter::DumpBezierPosKeys(IGameControlType Type, IGameKeyTab Keys, Point3 pos, BoneAnim& boneAnim)
{
	TSTR buf;
	CComPtr <IXMLDOMNode> prsChild;
	CComPtr <IXMLDOMNode> prsAxis;

	boneAnim.trans.
	boneAnim.trans.m_KeyTimes.push_back(fc);
	boneAnim.trans.m_KeyData.push_back(k);


	buf.printf("%f %f %f", pos.x, pos.y, pos.z);
	AddXMLAttribute(prsData,_T("Value"),buf.data());

	CreateXMLNode(pXMLDoc,prsData,_T("PositionXYZController"),&prsChild);

	CreateXMLNode(pXMLDoc,prsChild,_T("X"),&prsAxis);
	DumpBezierFloatKeys(Type, Keys, 0, prsAxis);
	buf.printf("%f", pos.x);
	AddXMLAttribute(prsAxis,_T("Value"),buf.data());

	prsAxis = NULL;
	CreateXMLNode(pXMLDoc,prsChild,_T("Y"),&prsAxis);
	buf.printf("%f", pos.x);
	AddXMLAttribute(prsAxis,_T("Value"),buf.data());
	DumpBezierFloatKeys(Type, Keys, 1, prsAxis);

	prsAxis = NULL;
	CreateXMLNode(pXMLDoc,prsChild,_T("Z"),&prsAxis);
	buf.printf("%f", pos.x);
	AddXMLAttribute(prsAxis,_T("Value"),buf.data());
	DumpBezierFloatKeys(Type, Keys, 2, prsAxis);
}

void Max2009ModelExporter::DumpBezierRotKeys(IGameKeyTab Keys, BoneAnim& boneAnim)
{
}

void Max2009ModelExporter::DumpBezierSclKeys(IGameKeyTab Keys, BoneAnim& boneAnim)
{
}*/

//void Max2009ModelExporter::ExportPositionControllers(BoneAnim& boneAnim, IGameControl * pGameControl, Point3 pos)
//{
//}
//
//void Max2009ModelExporter::ExportRotationControllers(CModelData& model, IGameControl * pGameControl)
//{
//	IGameKeyTab rotkeys;
//	//rotation
//	IGameControl::MaxControlType T = pGameControl->GetControlType(IGAME_ROT);
//
//	if(T==IGameControl::IGAME_MAXSTD && pGameControl->GetTCBKeys(rotkeys,IGAME_ROT) && !forceSample)
//	{
//		DumpTCBKeys(IGAME_ROT,rotkeys,node);
//	}
//	else if(T==IGameControl::IGAME_MAXSTD)
//	{
//		if (pGameControl->GetLinearKeys(rotkeys,IGAME_ROT))
//		{
//		}
//		else if (pGameControl->GetBezierKeys(rotkeys,IGAME_ROT))
//		{
//		}
//	}
//	else if(T==IGameControl::IGAME_ROT_CONSTRAINT && !forceSample)
//	{
//		//IGameConstraint * cnst = pGameControl->GetConstraint(IGAME_ROT);
//		//DumpConstraints(node,cnst);
//	}
//	else if(T==IGameControl::IGAME_LIST&& !forceSample)
//	{
//		//DumpListController(pGameControl,node);
//
//	}
//	else if(T==IGameControl::IGAME_EULER&& !forceSample)
//	{
//		DumpEulerController(pGameControl,node);
//	}
//	else
//	{
//		if(forceSample || T==IGameControl::IGAME_UNKNOWN)
//			DumpSampleKeys(pGameControl,node, IGAME_ROT);
//	}
//}

//void IGameExporter::ExportScaleControllers(CModelData& model, IGameControl * pGameControl)
//{
	/*IGameKeyTab scalekeys;

	//scale
	IGameControl::MaxControlType T= pGameControl->GetControlType(IGAME_SCALE);

	if(T==IGameControl::IGAME_MAXSTD && pGameControl->GetBezierKeys(scalekeys,IGAME_SCALE) && !forceSample)
		DumpBezierKeys(IGAME_SCALE,scalekeys,node);
	else
	{
		if(forceSample || T==IGameControl::IGAME_UNKNOWN)
			DumpSampleKeys(pGameControl,node, IGAME_SCALE);
	}*/

//}

void Max2009ModelExporter::DumpMesh(IGameNode * child, IGameMesh * gm, CModelData& model )
{
	bool vcSet = false;
	bool nSet = false;
	bool tvSet = false;

	size_t uStartSubset = model.m_mapPasses.size();
	size_t uStartPos = model.m_Mesh.pos.size();
	size_t uStartNormal = model.m_Mesh.normal.size();
	size_t uStartUV1 = model.m_Mesh.texcoord.size();
	size_t uStartColor = model.m_Mesh.color.size();
	//size_t uStartPos = model.m_Mesh.pos.size();
	{
		// 顶点
		int numVerts = gm->GetNumberOfVerts();
		{
			for (int i=0; i<numVerts; ++i)
			{
				Point3 v; 
				if(gm->GetVertex(i, v, exportObjectSpace))
				{
					Vec3D vPos;
					vPos.x = v.x;
					vPos.y = v.y;
					vPos.z = v.z;
					vPos*=exportScale;
					model.m_Mesh.pos.push_back( fixCoordSystemPos(vPos) );
				}
			}
		}

		// skin
		if(exportModifiers)
		{
			//IGameSkin* s=NULL;

			//int numModifiers = gm->GetNumModifiers();
			//for ( int i = 0; i < numModifiers; i ++ )
			//{
			//	IGameModifier * m = gm->GetIGameModifier( i );

			//	if ( m->IsSkin () )
			//	{
			//		s = (IGameSkin *)m;
			//	}
			//} 

			IGameSkin* s=gm->GetIGameSkin();
			if (s)
			{
				GMatrix skinTM;
				s->GetInitSkinTM(skinTM);
				for(int x=0; x<s->GetNumOfSkinnedVerts();x++)
				{
					int type=s->GetVertexType(x);
					if(type==IGameSkin::IGAME_RIGID)
					{
						unsigned long uBondID=0;
						unsigned long uWeight=0;
						INode* pNodeBone = s->GetBone(x,0);
						if (pNodeBone)
						{
							uBondID=model.m_Skeleton.getIDByName(pNodeBone->GetName());
							// 						if (255==uBondID)
							// 						{
							// 							uBondID=model.m_Skeleton.getIDByName(pNodeBone->GetName());
							// 						}
							uWeight=255;
						}
						if (255==uBondID)
						{
							uWeight = 0;
							Warn(TSTR("Can't find the bone! Bone:")+pNodeBone->GetName()+TSTR(" RigidMesh:")+child->GetName());
						}
						model.m_Mesh.bone.push_back(uBondID);
						model.m_Mesh.weight.push_back(uWeight);
					}
					else if (type==IGameSkin::IGAME_RIGID_BLENDED)
					{
						unsigned long uBondID=0;
						unsigned long uWeight=0;
						float	weights[4];
						float	allWeight=0;
						memset(weights,0,sizeof(weights));

						//Matrix3 matInvBone;
						//matInvBone.Zero();
						size_t uBoneCount = s->GetNumberOfBones(x);
						if (uBoneCount>4)
						{
							uBoneCount = 4;
							Warn(TSTR("Number Of Bones > 4! SkinMesh:")+child->GetName());
						}
						for(int y=0;y<uBoneCount;++y)
						{
							INode* pNodeBone = s->GetBone(x,y);
							if (pNodeBone)
							{
								unsigned char id = model.m_Skeleton.getIDByName(pNodeBone->GetName());
								uBondID|=(id<<(y*8));
								if (255!=id)
								{
									weights[y] = s->GetWeight(x,y);
									allWeight+=weights[y];
								}
								else
								{
									Warn(TSTR("Can't find the bone! Bone:")+pNodeBone->GetName()+TSTR(" SkinMesh:")+child->GetName());
								}
							}
							/*Matrix3 matb = pNodeBone->GetNodeTM(staticFrame);
							Matrix3 matw;
							matw.Zero();
							matw.SetScale(Point3(fWeight,fWeight,fWeight));
							matInvBone += matb*matw;*/
						}
						/*matInvBone.Invert();
						Vec3D& vPos = model.m_Mesh.pos[model.m_Mesh.bone.size()];
						Point3 point3 = (*(Point3*)&vPos) * matInvBone;
						vPos = *(Vec3D*)&point3;*/

						for (size_t i=0;i<4;++i)
						{
							uWeight|=((unsigned char(weights[i]/allWeight*255))<<(i*8));
						}

						model.m_Mesh.bone.push_back(uBondID);
						model.m_Mesh.weight.push_back(uWeight);
					}
				}
			}
			else
			{
				IGameNode* pNodeBone = child->GetNodeParent();
				unsigned char uBoneID = 0;
				if (pNodeBone)
				{
					uBoneID = model.m_Skeleton.getIDByName(pNodeBone->GetName());
				}
				for (int i=0; i<numVerts; ++i)
				{
					model.m_Mesh.bone.push_back( uBoneID );
					model.m_Mesh.weight.push_back( 255 );
				}
			}
		}
		// 法线
		int numNorms = gm->GetNumberOfNormals();

		for (int i=0; i<numNorms; ++i)
		{
			Point3 n;
			if( gm->GetNormal(i, n, exportObjectSpace) )
			{
				Vec3D vNormal;
				vNormal.x = n.x;
				vNormal.y = n.y;
				vNormal.z = n.z;
				model.m_Mesh.normal.push_back( vNormal );
			}
			else
			{
				Warn(TSTR("Can't get normal! Mesh:")+child->GetName());
				model.m_Mesh.normal.push_back( Vec3D(0,1,0) );
			}
		}
		// 纹理
		int numTVs = gm->GetNumberOfTexVerts();
		for (int i=0; i<numTVs; ++i)
		{
			
			Point2 tv;
			if (gm->GetTexVertex(i, tv))
			{
				Vec2D vTexcoord;
				vTexcoord.x = tv.x;
				vTexcoord.y = tv.y;
				model.m_Mesh.texcoord.push_back( vTexcoord );
			}
		}
		// 颜色
		int numVCs = gm->GetNumberOfColorVerts();
		for (int i=0; i<numVCs; ++i)
		{
			//Point3 c;
			//if(gm->GetColorVertex(VertexInfo.,n))
			//{	
			//}
		}
		//m_Log.Log("numVerts:%d; numNorms:%d; numTVs:%d; numVCs:%d; ",numVerts,numNorms,numTVs,numVCs);
		//MessageBox();
		//cout<<numVerts<<';'<<numNorms<<';'<<numTVs<<';'<<numVCs<<'/n';
	}

	// dump data by // 按材质分// 不需要细分||FaceexportFaceSmgp
	if(1)
	{
		Tab<int> matGrps = gm->GetActiveMatIDs();
		for ( int x=0; x < matGrps.Count(); x++ )
		{
			int nMatID = uStartSubset+x;//fe->matID;
			if ( nMaterialsCount < matGrps[x] )
			{
				nMaterialsCount = matGrps[x]+1;
			}
			Tab <FaceEx *> matFaces = gm->GetFacesFromMatID(matGrps[x]);
			if (matFaces.Count()>0)
			{
				// 顶点索引信息
				for(int f=0;f<matFaces.Count();f++)
				{
					FaceEx *fe = matFaces[f];
					FaceIndex	faceIndex;

					faceIndex.uSubID = nMatID;
					for ( int i = 0; i<3; i++ )
					{
						faceIndex.v[i] = fe->vert[i]+uStartPos;
						faceIndex.n[i] = fe->norm[i]+uStartNormal;
						faceIndex.c[i] = fe->color[i]+uStartColor;
						faceIndex.uv1[i] = fe->texCoord[i]+uStartUV1;
						//faceIndex.indexUV2[3];
						faceIndex.b[i]=fe->vert[i]+uStartPos;
						faceIndex.w[i]=fe->vert[i]+uStartPos;
					}
					model.m_Mesh.m_setFaceIndex.push_back(faceIndex);
				}
				std::string strDiffuse;
				int nColorID = 0;
				IGameMaterial* mat = gm->GetMaterialFromFace( matFaces[0] );
				if (mat)
				{
					IGameProperty* prop = mat->GetDiffuseData();
					{
						nColorID = model.m_ColorAnims.size();
						model.m_ColorAnims.resize( nColorID+1 );


						//DumpProperties(propNode,prop);
						if(prop->GetType() == IGAME_POINT3_PROP)
						{
							Point3 p; 
							prop->GetPropertyValue(p);
							model.m_ColorAnims[nColorID].color.type = INTERPOLATION_LINEAR;
							model.m_ColorAnims[nColorID].color.seq = -1;
							model.m_ColorAnims[nColorID].color.m_KeyData.resize( 1 );
							model.m_ColorAnims[nColorID].color.m_KeyData[0] = Vec3D( p.x, p.y, p.z );
						}
						prop = mat->GetSpecularData();
						if(prop->GetType() == IGAME_POINT3_PROP)
						{
							Point3 p; 
							prop->GetPropertyValue(p);
							model.m_ColorAnims[nColorID].specular.type = INTERPOLATION_LINEAR;
							model.m_ColorAnims[nColorID].specular.seq = -1;
							model.m_ColorAnims[nColorID].specular.m_KeyData.resize( 1 );
							model.m_ColorAnims[nColorID].specular.m_KeyData[0] = Vec3D( p.x, p.y, p.z );
						}
						//prop = mat->GetOpacityData();
						//if(prop->GetType() == IGAME_POINT3_PROP)
						//{
						//	Point3 p; 
						//	prop->GetPropertyValue(p);
						//	model.m_ColorAnims[nColorID].specular.type = INTERPOLATION_LINEAR;
						//	model.m_ColorAnims[nColorID].specular.seq = -1;
						//	model.m_ColorAnims[nColorID].specular.m_KeyData.resize( 1 );
						//	model.m_ColorAnims[nColorID].specular.m_KeyData[0] = Vec3D( p.x, p.y, p.z );
						//}

						int texCount = mat->GetNumberOfTextureMaps();
						if(texCount>0)
						{
						}
						for(int i=0;i<texCount&&i<1;i++)
						{
							IGameTextureMap * tex = mat->GetIGameTextureMap(i);
							//TCHAR * name = tex->GetTextureName();
							if(tex->IsEntitySupported())	//its a bitmap texture
							{
								TCHAR * name = tex->GetBitmapFileName();
								strDiffuse = name;
								{
									int len = strDiffuse.length();
									for (int i=len-1; i>=0; i--)
									{
										if (strDiffuse[i] == '\\')
										{
											strDiffuse = strDiffuse.substr(i+1);
											break;
										}
									}
								}
								//model.m_TexChannels[model.m_TexChannels.size()].strTexs.push_back(strDiffuse);
								// 渲染flag
								
							}
						}
					}
				}
				ModelRenderPass pass;
				// RenderFlag;
				pass.nRenderFlag = 0;
				pass.nBlendMode = 0;
				pass.nTransID = -1;
				pass.nTexanimID = -1;
				pass.nColorID = nColorID;
				//pass.nOrder;
				//pass.p;

				// Geoset ID
				pass.nSubID = nMatID;
				//
				pass.bUseTex2 = false;
				pass.bUseEnvMap = false;
				pass.bCull = false;
				pass.bTrans = false;
				pass.bUnlit = false;
				pass.bNoZWrite = false;
				pass.bHasAlphaTex = false;
				//// texture wrapping
				//pass.bSwrap, bTwrap;

				//// colours
				//pass.ocol, ecol;
				pass.material.strDiffuse = strDiffuse;
				pass.material.bCull = false;
				if (mat&&mat->GetOpacityData())
				{
					pass.material.bAlphaTest = true;
				}

				model.m_mapPasses[nMatID]=pass;
			}
		}
	}
	//else
	//{
	//	int numFaces = gm->GetNumberOfFaces();
	//	for(int i=0;i<numFaces;i++)
	//	{
	//		FaceEx *fe = gm->GetFace(i);
	//	}
	//}

	if(exportMappingChannel)
	{
		Tab<int> mapNums = gm->GetActiveMapChannelNum();
		int mapCount = mapNums.Count();

		for(int i=0;i < mapCount;i++)
		{
			int vCount = gm->GetNumberOfMapVerts(mapNums[i]);
			for(int j=0;j<vCount;j++)
			{
				Point3 v;
				if(gm->GetMapVertex(mapNums[i],j,v))
				{
					//data.printf("%f %f %f",v.x,v.y,v.z);
				}

			}
			int fCount = gm->GetNumberOfFaces();
			for(int k=0;k<fCount;k++)
			{
				DWORD  v[3];
				gm->GetMapFaceIndex(mapNums[i],k,v);
				//data.printf("%d %d %d",v[0],v[1],v[2]);
			}
		}
	}

#if 0
	//test code
	Tab<int> matids;

	matids = gm->GetActiveMatIDs();

	for(i=0;i<matids.Count();i++)
	{
		Tab<FaceEx*> faces;

		faces = gm->GetFacesFromMatID(matids[i]);

		for(int k=0; k<faces.Count();k++)
		{
			IGameMaterial * faceMat = gm->GetMaterialFromFace(faces[k]);
			//			TSTR name(faceMat->GetMaterialName());
		}
		for(k=0;k<gm->GetNumberOfFaces();k++)
		{
			IGameMaterial * faceMat = gm->GetMaterialFromFace(k);
		}

	}

	Tab <DWORD> smgrps = gm->GetActiveSmgrps();
#endif

}

BOOL Max2009ModelExporter::ReadAnimList(std::vector<ModelAnimation>& setAnimList)
{
	std::string filename = GetCOREInterface()->GetCurFilePath();
	filename = GetParentPath(filename)+"Anim.csv";
	CCsvFile csvAnim;
	if (csvAnim.Open(filename))
	{
		while (csvAnim.SeekNextLine())
		{
			ModelAnimation anim;
			//csvAnim.GetInt("name");
			anim.animID = setAnimList.size();
			anim.timeStart=csvAnim.GetInt("begin")*1000/GetFrameRate();
			anim.timeEnd=csvAnim.GetInt("end")*1000/GetFrameRate();
			anim.moveSpeed=1;
			anim.loopType = 0;
			anim.flags=0;
			anim.d1=0;
			anim.d2=0;
			anim.playSpeed=1;

			setAnimList.push_back(anim);
		}
		csvAnim.Close();
		return true;
	}
	return false;
}

TSTR Max2009ModelExporter::GetCfgFilename()
{
	TSTR filename;

	filename += GetCOREInterface()->GetDir(APP_PLUGCFG_DIR);
	filename += "\\";
	filename += "Max2009ModelExporter.cfg";
	return filename;
}

BOOL Max2009ModelExporter::ReadConfig()
{
	TSTR filename = GetCfgFilename();
	FILE* cfgStream;

	cfgStream = fopen(filename, "rb");
	if (!cfgStream)
		return FALSE;

	exportGeom = fgetc(cfgStream);
	exportNormals = fgetc(cfgStream);
	exportControllers = fgetc(cfgStream);
	exportFaceSmgp = fgetc(cfgStream);
	exportVertexColor = fgetc(cfgStream);
	exportTexCoords = fgetc(cfgStream);
	staticFrame = _getw(cfgStream);
	framePerSample = _getw(cfgStream);
	exportMappingChannel = fgetc(cfgStream);
	exportMaterials = fgetc(cfgStream);
	exportSplines = fgetc(cfgStream);
	exportModifiers = fgetc(cfgStream);
	forceSample = fgetc(cfgStream);
	exportConstraints = fgetc(cfgStream);
	exportSkin = fgetc(cfgStream);
	exportGenMod = fgetc(cfgStream);
	exportQuaternions = fgetc(cfgStream);
	exportObjectSpace = fgetc(cfgStream);
	exportRelative = fgetc(cfgStream);
	fread(&exportScale,sizeof(float),1,cfgStream);
	fclose(cfgStream);
	return TRUE;
}

void Max2009ModelExporter::WriteConfig()
{
	TSTR filename = GetCfgFilename();
	FILE* cfgStream;

	cfgStream = fopen(filename, "wb");
	if (!cfgStream)
		return;


	fputc(exportGeom,cfgStream);
	fputc(exportNormals,cfgStream);
	fputc(exportControllers,cfgStream);
	fputc(exportFaceSmgp,cfgStream);
	fputc(exportVertexColor,cfgStream);
	fputc(exportTexCoords,cfgStream);
	_putw(staticFrame,cfgStream);
	_putw(framePerSample,cfgStream);
	fputc(exportMappingChannel,cfgStream);
	fputc(exportMaterials,cfgStream);
	fputc(exportSplines,cfgStream);
	fputc(exportModifiers,cfgStream);
	fputc(forceSample,cfgStream);
	fputc(exportConstraints,cfgStream);
	fputc(exportSkin,cfgStream);
	fputc(exportGenMod,cfgStream);
	fputc(exportQuaternions,cfgStream);
	fputc(exportObjectSpace,cfgStream);
	fputc(exportRelative,cfgStream);
	fwrite(&exportScale,sizeof(float),1,cfgStream);
	fclose(cfgStream);
}


