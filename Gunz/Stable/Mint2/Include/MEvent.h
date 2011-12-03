#ifndef MEVENT_H
#define MEVENT_H

#include "MTypes.h"

#ifdef WIN32
	#include <windows.h>
	#pragma comment(lib, "Imm32.lib")
#endif

//////////////////////////////////
// Messages
#define MWM_LBUTTONDOWN			0x0010
#define MWM_LBUTTONUP			0x0011
#define MWM_LBUTTONDBLCLK		0x0012
#define MWM_RBUTTONDOWN			0x0013
#define MWM_RBUTTONUP			0x0014
#define MWM_RBUTTONDBLCLK		0x0015
#define MWM_MBUTTONDOWN			0x0016
#define MWM_MBUTTONUP			0x0017
#define MWM_MBUTTONDBLCLK		0x0018
#define MWM_MOUSEMOVE			0x0019
#define MWM_MOUSEWHEEL			0x0020
#define MWM_CHAR				0x0030
#define MWM_KEYDOWN				0x0031
#define MWM_KEYUP				0x0032
#define MWM_SYSCHAR				0x0033
#define MWM_SYSKEYDOWN			0x0034
#define MWM_SYSKEYUP			0x0035
#define MWM_HOTKEY				0x0040
#define MWM_ACTIONKEYDOWN		0x0041
#define MWM_ACTIONKEYUP			0x0042
#define MWM_ACTIONPRESSED		0x0043
#define MWM_ACTIONRELEASED		0x0044
#define MWM_IMECOMPOSE			0x0050
#define MWM_IMECONVERSION		0x0051
//#define MWM_RESIZE				0x0052
//#define MWM_MOVIE_NOTIFY		0x0060


#define MIMECOMPOSITIONSTRING_LENGTH	256	// �Ϻ���� �߱���� Composition String�� ��� ��������Ƿ� �˳��� ũ�⸦ �ʿ�� �Ѵ�.
//////////////////////////////////
// Event class

class MEvent{
public:
	int				nMessage;
	MPOINT			Pos;
	unsigned int	nKey;			// Key Code of WM_CHAR
	int				nDelta;			// Wheel Mouse Delta
	//bool			bShift;			// Shift�� �����Ѵ�.
	bool			bCtrl;
	bool			bAlt;

	static bool		bLButton;
	static bool		bMButton;
	static bool		bRButton;

	static MPOINT	LatestPos;	// ������ ��ġ

	char	szIMECompositionString[MIMECOMPOSITIONSTRING_LENGTH];		// IME�� ���� Composition String
	char	szIMECompositionResultString[MIMECOMPOSITIONSTRING_LENGTH];	// IME�� ���� Composition Result String
	static bool	bIMESupport;											// IME Composing Support?
public:

	// Async Functions
	static bool GetShiftState(void);
	static bool GetCtrlState(void);
	static bool GetAltState(void);
	static bool IsKeyDown(int key);

	static bool GetLButtonState(void);
	static bool GetMButtonState(void);
	static bool GetRButtonState(void);

	static MPOINT GetMousePos(void);

	static void MEvent::ForceSetIME(DWORD fdwConversion,DWORD fdwSentence);

	static void SetIMESupport( bool bSupport)		{ bIMESupport = bSupport; }
	static bool GetIMESupport()						{ return bIMESupport;}

	// WIN32 Message�� MEvent�� ��ȯ
	// ����: ���� �޽��� ����, 2005-08-18, ����ȣ
	// IME ����â�� ��µǴ� ������ ���� ���� 3�ܰ�� ���� ����
	// EVENT_MINT_TRANSLATED�� ó�� ����� ������� WndProc() 0�� �����Ϸ���, EVENT_PROCESSED|EVENT_MINT_TRANSLATED�� ����
#define EVENT_NOT_PROCESSED			0x00	// ������ �޽��� ó���� �ȵ�
#define EVENT_PROCESSED				0x01	// ������ �޽��� ó���� ��, EVENT_MINT_TRANSLATED�� ���� �ʾƵ� WndProc()�� ������ 0 ����
#define EVENT_MINT_TRANSLATED		0x02	// ��Ʈ �޽����� ������
	int TranslateEvent(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam);
};


#endif