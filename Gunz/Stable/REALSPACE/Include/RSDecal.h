// RSDecal.h: interface for the RSDecal class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RSDECAL_H__7DDFD227_50C7_11D2_A32A_00AA006E4A4E__INCLUDED_)
#define AFX_RSDECAL_H__7DDFD227_50C7_11D2_A32A_00AA006E4A4E__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "RsTypes.h"
#include "RSMaterialList.h"
#include "rsd3d.h"

class RSDecal  
{
public:
	RSDecal();
	virtual ~RSDecal();

	void Draw();
	void Draw(rmatrix44& tm);		// tm �� Ʈ�������� �̿��� ��´�. (��ġ.����.������ ��� ����)
	/*
		nDirection
		��������...
		0 : �ٷ� ��´�.
		1 : 90�� ȸ��
		2 : 180�� ȸ��
		3 : 240�� ȸ��
		4 : ���Ϸ� �ø�
		5 : �¿�� �ø�
		6 : ��/��/��/��� �ø�
	*/
	void DrawPanel( int nLeft, int nTop, int nRight, int nBottom);

	void GetOrientation(rvector *dir, rvector *up);
	void SetOrientation(rvector,rvector);
	void SetOrientationToCamera(float);
	void SetColor(DWORD);
	
	void SetOpacity(unsigned char nOpacity);

	void SetMaterial(RSMaterial*);
	void SetTexture(int iT);
	inline rvector GetPosition(){return position;}
	inline void SetPosition(const rvector &p){position=p;}
	inline void SetPosition(float x,float y,float z){SetPosition(rvector(x,y,z));}

	void SetScaleXY(float,float);
	void AddRotationX(float theta);
	void AddRotationY(float theta);
	void AddRotationZ(float theta);	

	void InitAnimation(void);
	void MoveAnimation(DWORD);
	BOOL isAnimationEnded(){return (thistime>Material->nTexture*Material->AnimationSpeed);}

	int GetAnimationCount(void);
	int GetAnimationSpeed(void);
	void MoveAminmationByIndex(int i);
	int	GetTextureHandle(int);
	int	GetTextureHandle();
	RSMaterial* GetMaterial();

private:
	LITVERTEX v[4];

	rmatrix mat,CorMat;
	rvector position;
	float ScaleFactorX,ScaleFactorY;
	RSMaterial *Material;
	int TextureHandle;
	DWORD thistime;
};

#endif // !defined(AFX_RSDECAL_H__7DDFD227_50C7_11D2_A32A_00AA006E4A4E__INCLUDED_)
