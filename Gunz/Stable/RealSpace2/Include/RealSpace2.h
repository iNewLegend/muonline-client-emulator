#ifndef __REALSPACE2_H
#define __REALSPACE2_H

#include "RTypes.h"
#include "RError.h"
#include "MZFileSystem.h"

#pragma comment(lib,"d3dx9.lib")

#include "RNameSpace.h"
_NAMESPACE_REALSPACE2_BEGIN

class RParticleSystem;

// �ٱ����� �����Ҹ��� �͵�.
bool	RIsActive();
bool	RIsQuery();
void	RSetQuery(bool b);//�׽�Ʈ��..
bool	RIsFullScreen();
bool	RIsHardwareTNL();
bool	RIsSupportVS();
bool	RIsAvailUserClipPlane();
bool	RIsTrilinear();
int		RGetApproxVMem();
int		RGetScreenWidth();
int		RGetScreenHeight();
int     RGetIsWidthScreen();
int		RGetPicmip();
RPIXELFORMAT RGetPixelFormat();
D3DADAPTER_IDENTIFIER9*	RGetAdapterID();
void SetClearColor(DWORD c);
int		RGetVidioMemory();
void	RSetWBuffer(bool bEnable);
//bool	RIsStencilBuffer();


int RGetAdapterModeCount( D3DFORMAT Format,UINT Adapter = D3DADAPTER_DEFAULT );
bool REnumAdapterMode( UINT Adapter, D3DFORMAT Format ,UINT Mode, D3DDISPLAYMODE* pMode );


// ��ƼŬ �ý���
RParticleSystem *RGetParticleSystem();


extern int g_nFrameCount,g_nLastFrameCount;
extern float g_fFPS;
extern int g_nFrameLimitValue;
extern HWND	g_hWnd;
extern MZFileSystem *g_pFileSystem;

// �ʱ�ȭ & ����
bool RInitDisplay(HWND hWnd,const RMODEPARAMS *params);
bool RCloseDisplay();
void RSetFileSystem(MZFileSystem *pFileSystem);
void RAdjustWindow(const RMODEPARAMS *pModeParams);		// ������ ũ�⸦ ������

// ����̽� & ����Ʈ 
LPDIRECT3DDEVICE9	RGetDevice();

// �����ȯ & �ø��ΰ���
void RResetDevice(const RMODEPARAMS *params);
RRESULT RIsReadyToRender();
void RFlip();
bool REndScene();
bool RBeginScene();

// RS�� ������ ���õ� �ɼǵ� - �� �߰��� �� ������ �߰��ϵ��� ����.
enum RRENDER_FLAGS
{
	RRENDER_CLEAR_BACKBUFFER		= 0x000001,
};

void RSetRenderFlags(unsigned long nFlags);
unsigned long RGetRenderFlags();

//Fog
void RSetFog(bool bFog, float fNear = 0, float fFar=0, DWORD dwColor=0xFFFFFFFF );
bool RGetFog();
float RGetFogNear();
float RGetFogFar();
DWORD RGetFogColor();

// ī�޶� ����	( RCamera.cpp )
extern rvector RCameraPosition,RCameraDirection,RCameraUp;
extern rmatrix RView,RProjection,RViewProjection,RViewport,RViewProjectionViewport;

void RSetCamera(const rvector &from, const rvector &at, const rvector &up);
void RUpdateCamera();		// rsetcamera �� ����ϰų� ���� ��ģ�� update�� ������մϴ�.

void RSetProjection(float fFov,float fNearZ,float fFarZ);
void RSetProjection(float fFov,float fAspect,float fNearZ,float fFarZ);

inline rplane *RGetViewFrustum();
void RSetViewport(int x1,int y1,int x2,int y2);
D3DVIEWPORT9		*RGetViewport();

void RResetTransform();		// �ۿ��� �� ���� ���� ���.. ����̽� �ν�Ʈ �Ǿ����� �Ҹ�.

// ���� ���ؼ� ����
extern D3DPRESENT_PARAMETERS g_d3dpp;

///////////////////////// ������ �ִ� ��ǵ�.

// ȭ����ǥ�� x,y �� 3������ line���� ��ȯ
bool RGetScreenLine(int sx,int sy,rvector *pos,rvector *dir);

// ȭ����ǥ���� x,y �� ������ ������ ������ ���Ѵ�.
rvector RGetIntersection(int x,int y,rplane &plane);
bool RGetIntersection( rvector& a, rvector& b, rplane &plane, rvector* pIntersection );

// 3d ��ǥ�� ȭ����ǥ�� transform �Ѵ�.
rvector RGetTransformCoord(rvector &coord);

// �׸��°Ͱ� ���õ� ���..
void RDrawLine(rvector &v1,rvector &v2,DWORD dwColor);
void RDrawCylinder(rvector origin,float fRadius,float fHeight,int nSegment);
void RDrawCorn(rvector center,rvector pole,float fRadius,int nSegment);
void RDrawSphere(rvector origin,float fRadius,int nSegment);
void RDrawAxis(rvector origin,float fSize);
void RDrawCircle(rvector origin,float fRadius, int nSegment);
void RDrawArc(rvector origin, float fRadius, float fAngle1, float fAngle2, int nSegment, DWORD color);

// bmp�� �����Ѵ� data �� argb ���� 1byte�� �Ǿ��ִ�.
bool RSaveAsBmp(int x,int y,void *data,const char *szFilename);

// ��ũ������ �����Ѵ� data �� argb ���� 1byte�� �Ǿ��ִ�.
bool RScreenShot(int x,int y,void *data,const char *szFilename);

LPDIRECT3DSURFACE9 RCreateImageSurface(const char *filename);

// ������ ����
void RSetGammaRamp(unsigned short nGammaValue = 255);
// 1�ʴ� ������ ���� ����
void RSetFrameLimitPerSeceond(unsigned short nFrameLimit = 0);
// wbuffer enable
void RSetWBuffer(bool bEnable);

// winmain ��ý�Ʈ.

enum RFUNCTIONTYPE {
	RF_CREATE  =0,
	RF_DESTROY,
	RF_RENDER,
	RF_UPDATE,
	RF_INVALIDATE,
	RF_RESTORE,
	RF_ACTIVATE,
	RF_DEACTIVATE,
	RF_ERROR,

	RF_ENDOFRFUNCTIONTYPE		// �̰� ���� ���� ����� �ƴϰ�.. ���ڸ� �������Ѱ���.
};

typedef RRESULT (*RFFUNCTION)(void *Params);

//void RSetFunction(RFUNCTIONTYPE ft,RFFUNCTION pfunc);
//int RMain(const char *AppName, HINSTANCE this_inst, HINSTANCE prev_inst, LPSTR cmdline, int cmdshow, RMODEPARAMS *pModeParams, WNDPROC winproc=NULL, WORD nIconResID=NULL );
//
//int RRun();

//////Do Not Use This Method --> Danger...
//void FixedResolutionRenderStart();
//void FixedResolutionRenderEnd();
//void FixedResolutionRenderInvalidate();
////// Use This Method
//void FixedResolutionRenderFlip();
//bool IsFixedResolution();
//void SetFixedResolution( bool b );


////// inline functions
extern rplane RViewFrustum[6];
inline rplane *RGetViewFrustum()
{
	return RViewFrustum;
}


_NAMESPACE_REALSPACE2_END

#endif