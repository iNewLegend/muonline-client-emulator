#ifndef ZDIRECTINPUT_H
#define ZDIRECTINPUT_H

#define DIRECTINPUT_VERSION 0x0800

#include <dinput.h>
#include <windows.h>

struct ZDIBUFFER{
	BYTE nKey;
	bool bPressed;
};

// Immediate Mode : ���� ���¸� ������ �ִ� ���
// Buffered Mode : ���� �߻��� �Է��� ���۷� �о���� ���


/// DirectInput Wrapper
class ZDirectInput{
protected:
	HMODULE					m_hD3DLibrary;
	BOOL					m_bInitialized;
	LPDIRECTINPUT8			m_pDI;				///< The DirectInput object         
	LPDIRECTINPUTDEVICE8	m_pKeyboard;		///< The keyboard device 
	BOOL					m_bImmediateMode;

	LPDIRECTINPUTDEVICE8	m_pMouse;			///< mouse device
	unsigned int			m_nMouseButtons;	///> mouse buttons
	bool					m_bMouseButtonStates[8];

	LPDIRECTINPUTDEVICE8	m_pJoystick;		///< joystick device
	unsigned int			m_nJoyButtons;		///< joystick buttons
	unsigned int			m_nJoyPovs;			///< joystick povs
	unsigned int			m_nFFAxis;			///< force feedback axis
	bool					m_bForceFeedback;
	LPDIRECTINPUTEFFECT		m_pFFEffect;		///< force feedback effect

#define KEYNAMETABLE_COUNT	256
	char*					m_szKeyNameTable[KEYNAMETABLE_COUNT];

	static BOOL CALLBACK EnumJoysticksCallback( const DIDEVICEINSTANCE* pdidInstance, VOID* pContext );
	static BOOL CALLBACK EnumJoyObjectsCallback( const DIDEVICEOBJECTINSTANCE* pdidoi, VOID* pContext );

public:
	ZDirectInput(void);
	virtual ~ZDirectInput(void);

	bool CreateDirectInput();
	bool Create(HWND hWnd, BOOL bExclusive=TRUE, BOOL bImmediateMode=TRUE);
	void Destroy(void);

	void OnActivate(bool bActive);

	BOOL IsInitialized()	{ return m_bInitialized; }

	/// 256���� ��ĵ�ڵ尡 �����ִ���(&0x80) �˻� . ������� ����
//	bool GetImmediateData(BYTE ScanCode[256]);

	///////////////////////////////////////////////////////////////////////////////////////
	/// Ű���� 

	/// Ű���� ���۷� �о����, ��ĵ�ڵ� �� Down/Up �÷��� ����, ���� ���� ������ŭ �����Ѵ�.
	DWORD GetKeyboardBufferedData(ZDIBUFFER* pBuffer,unsigned int nBuffer);

	const char* GetKeyName(unsigned long int nKey);


	////////////////////////////////////////////////////////////////////////////////////////
	/// ���콺

	// ���콺 ��ư ���� ������
	unsigned int GetMouseButtonCount()		{ return m_nMouseButtons; }

	// ���콺 ���۷� �о����
	DWORD GetMouseBufferedData(int* pSumX,int* pSumY, ZDIBUFFER* pBuffer,unsigned int nBuffer);
	
	// ���콺 ������ ������
//	bool GetMouseImmediateData(DIMOUSESTATE2 *pdims2);



	///////////////////////////////////////////////////////////////////////////////////////
	/// ���̽�ƽ

	// pov ���� ������
	unsigned int GetJoystickPovCount()		{ return m_nJoyPovs; }
	// ��ư ���� ������
	unsigned int GetJoystickButtonCount()	{ return m_nJoyButtons; }
	// ���̽�ƽ ���� ������
	bool GetJoystickData(DIJOYSTATE2* pjs);

	bool SetDeviceForcesXY(int nXForce, int nYForce);

};

#define ISKEYDOWN(_ScanCodeTable, _ScanCode)	((_ScanCodeTable[_ScanCode]&0x80)?true:false)

#endif