#ifndef RFRAMEWORK_H
#define RFRAMEWORK_H

_NAMESPACE_REALSPACE2_BEGIN

// ������ �Լ��� ȣ���ϱ� ���� �ܺη� ����
void RFrame_Render();

// WM_ENTERIDLE �� ȣ���ϱ� ���� ����
void RFrame_UpdateRender();

// ��ƼŬ / �⺻��Ʈ ���� ���ҼҴ� framework �� �������� �ִ�.
// �̷� ���ҽ��� invalidate/restore ������Ѵ�.

void RSetFunction(RFUNCTIONTYPE ft,RFFUNCTION pfunc);
int RMain(const char *AppName, HINSTANCE this_inst, HINSTANCE prev_inst, LPSTR cmdline, int cmdshow, RMODEPARAMS *pModeParams, WNDPROC winproc=NULL, WORD nIconResID=NULL );
int RInitD3D( RMODEPARAMS* pModeParams );
int RRun();

void RFrame_Invalidate();
void RFrame_Restore();

void RFrame_ToggleFullScreen();

_NAMESPACE_REALSPACE2_END

bool IsToolTipEnable();

#endif