#include "3DMapEffect.h"
//#include "Role.h"

bool C3DMapEffect::s_bStroke = false;
C3DMapEffect::C3DMapEffect()
{
	m_bDie = false;
	strcpy(m_szIndex, "");
	m_fHorizontal	= 0.0;
	m_fVertical		= 0.0;
	m_bOver			= false;
	m_nDelay		= 0;
	m_bSimpleObj	= false;
	m_dwFrameIndex	= 0;
}

C3DMapEffect::~C3DMapEffect()
{
	
}
//-------------------------------------------------------------------
void C3DMapEffect::Show(void* pInfo)
{
	if(m_bDie)
		return;

	Pos2D posScreen;
	if(s_bStroke)
	{
	}
	if(!m_bDie)
	{
		if(!m_bSimpleObj)
		{
		}
		else
		{
			Vec3D vViewPos;
			//GetSceneMgr().GetViewPos( vViewPos );
			Pos2D posViewportBg;
			int nRotate	= 0;
			
			//GetSceneMgr().World2Bg( vViewPos, posViewportBg );
			//	this->IsFocus();
			//union
			//{
			//	unsigned long dwColor;
			//	struct
			//	{
			//		unsigned char ucAlpha;
			//		unsigned char ucRed;
			//		unsigned char ucGreen;
			//		unsigned char ucBlue;
			//	}InfoColor;
			//};
			float alphaShadow	= 0.8f;
		//	dwColor = GetSceneMgr().GetARGB();
			//float fColor = 0.1 * (InfoColor.ucRed + InfoColor.ucGreen + InfoColor.ucBlue)/(255*3);
		////	m_objSimple.SetLightOffset(CRole::s_nShadowType, 300, -300, -1000, alphaShadow, fColor, fColor, fColor);
			
			
			//m_objSimple.Rotate(m_fVertical, 0.0, m_fHorizontal);
			
		////	m_objSimple.SetFrame(m_dwFrameIndex);
		////	m_objSimple.SetPos( m_vWorldPos, nRotate, 1.0f);
		////	m_objSimple.Draw2BG( posViewportBg.x, posViewportBg.y );
			
		////	m_objSimple.ClearMatrix();

		}
	}
}
//-------------------------------------------------------------------
void C3DMapEffect::SetRotate(float fHorizontal, float fVertical)
{
	m_fHorizontal = fHorizontal;
	m_fVertical = fVertical;
}
//-------------------------------------------------------------------
//-------------------------------------------------------------------
C3DMapEffect* C3DMapEffect::CreateNew(const Vec3D& vWorldPos, char* pszIndex, bool bDelSelf, bool bSave)
{
	if(!pszIndex)
		return NULL;
	C3DMapEffect* pEffect = new C3DMapEffect;
	if(!pEffect)
		return NULL;
	if(!pEffect->Create( vWorldPos, pszIndex, bDelSelf, bSave))
	{
		S_DEL(pEffect);
		return NULL;
	}
	return pEffect;
}
//-------------------------------------------------------------------
bool C3DMapEffect::Create(const Vec3D& vWorldPos, char* pszIndex, bool bDelSelf, bool bSave)
{
	if(!pszIndex)
		return false;
	if(strlen(pszIndex) == 0)
		return false;
	setPos(vWorldPos);
	m_bDelSelf = bDelSelf;
	strcpy(m_szIndex, pszIndex);
	m_bSave = bSave;

	if(pszIndex[0] == '$')
	{
		m_bSimpleObj = true;
		unsigned long dwData;
		if(1 != sscanf(pszIndex, "$%u", &dwData))
			return false;
	////	m_objSimple.Create(dwData);
	}
	else
	{
	}
	return true;
}


void C3DMapEffect::LoadDataObj(FILE* fp)
{
	if(!fp)
		return;
	fread(m_szIndex, sizeof(char), 64, fp);
	// read posTemp
	Pos2D posTemp;
	fread(&posTemp, sizeof(Pos2D), 1, fp);
	m_vPos.x = posTemp.x;
	m_vPos.z = posTemp.y;

	this->Create( m_vPos, m_szIndex);
	//if(!m_bSimpleObj)
	//	m_objEffect.EverPlay();
	m_bDelSelf = true;
	m_bDie = false;
	m_bSave = true;
}

void C3DMapEffect::LoadTextObj(FILE* fp)
{
	if(!fp)
		return;
	fscanf(fp, "Index=%s\n", m_szIndex);
	Pos2D posTemp;
    fscanf(fp, "PosWorld=[%d,%d]\n", &posTemp.x, &posTemp.y);
	m_vPos.x = posTemp.x;
	m_vPos.z = posTemp.y;

	this->Create( m_vPos, m_szIndex);
//	if(!m_bSimpleObj)
//		m_objEffect.EverPlay();
	m_bDelSelf	= true;
	m_bDie = false;
	m_bSave = true;
}


void C3DMapEffect::SetDir(int nDir)
{
	nDir = nDir%8;
	float fPei = (float)3.14159;

	switch(nDir)
	{
	case 0:
		m_fHorizontal = fPei*5/4;
		break;
	case 1:
		m_fHorizontal = fPei;
		break;
	case 2:
		m_fHorizontal = fPei*3/4;
		break;
	case 3:
		m_fHorizontal = fPei/2;
		break;
	case 4:
		m_fHorizontal = fPei/4;
		break;
	case 5:
		m_fHorizontal = 0;
		break;
	case 6:
		m_fHorizontal = -fPei/4;
		break;
	case 7:
		m_fHorizontal = -fPei/2;
		break;
	}
}