/*
	----------------
	Multimedia Timer
	----------------

	MMTimer.h

	"Multimedia Timer" handling class header file

	Programming by Chojoongpil
*/
#include <windows.h>
#include <mmsystem.h>

#pragma comment( lib, "winmm.lib" )

typedef BOOL (*MMTIMERCALLBACK)(DWORD);

// �ϳ��� Ŭ������ �Ѱ��� Ÿ�̸Ӹ� ���´�.
// �������� ������ �������� �����Ƿ� ������ ��.
class MMTimer
{
public:
    MMTimer();
    ~MMTimer();
	
	// Multimedia Timer�Ѱ��� �����Ѵ�.
    BOOL Create(UINT nPeriod, UINT nRes, DWORD dwUser,  MMTIMERCALLBACK pfnCallback);
	void Destroy();

protected:
    static void CALLBACK TimeProc(UINT uID, UINT uMsg, DWORD dwUser, DWORD dw1, DWORD dw2);

	// ���� Ÿ�̸Ӱ� �б��� �ݹ� �Լ�
    MMTIMERCALLBACK m_pfnCallback;

    DWORD m_dwUser;
    UINT m_nPeriod;
    UINT m_nRes;
    UINT m_nIDTimer;
};
