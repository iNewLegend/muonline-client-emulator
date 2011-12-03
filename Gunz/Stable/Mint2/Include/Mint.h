#ifndef MINT_H
#define MINT_H

#ifdef WIN32
#include <windows.h>
#endif

#include <map>
using namespace std;

#include "MTypes.h"
#include "MEvent.h"


class MDrawContext;
class MWidget;
class MFont;
class MBitmap;
class MResourceMap;
class MIDLResource;
class MListener;
class MEvent; 

#define MINT_VERSION	2
#define	MVersion()	MINT_VERSION

/// first KeyValue, second ActionID
//typedef map<unsigned long int, int>	ACTIONKEYMAP;

/// Global Event Callback
typedef bool(MGLOBALEVENTCALLBACK)(MEvent* pEvent);

/// Mint Global Object Class
class Mint{
protected:
	static Mint*			m_pInstance;	///< Instance
	MWidget*				m_pMainFrame;	///< Main Frame Widget
	MDrawContext*			m_pDC;			///< DrawContext
	MGLOBALEVENTCALLBACK*	m_fnGlobalEventCallBack;	///< Global Event Callback function

	// Drag & Drop
//	int			m_nDragObjectID;
	char		m_szDragObjectString[256];
	char		m_szDragObjectItemString[256];
	MBitmap*	m_pDragObjectBitmap;
	MPOINT		m_GrabPoint;
	bool		m_bVisibleDragObject;
	MWidget*	m_pDropableObject;
	MWidget*	m_pDragSourceObject;

	// Workspace Size
	int			m_nWorkspaceWidth;
	int			m_nWorkspaceHeight;

	/// Action Map
//	ACTIONKEYMAP	m_ActionKeyMap;
//#define ACTIONKEYMAP_IDCOUNT	256
//	bool			m_ActionKeyPressedTable[ACTIONKEYMAP_IDCOUNT];

	// IME ����
	void*	m_pCandidateList;		// Candidate List, LPCANDIDATELIST�� ĳ�����ؼ� ����Ѵ�.
	int		m_nCandidateListSize;	// Candidate List Size
	MPOINT	m_CandidateListPos;		// Candidate List�� ������ ��ġ

	bool	m_bEnableIME;

public:
	DWORD	m_nCompositionAttributeSize;				// Composition ���ڿ��� �Ӽ� ����
	BYTE	m_nCompositionAttributes[MIMECOMPOSITIONSTRING_LENGTH];		// Composition ���ڿ��� �Ӽ�
	//DWORD	m_dwCompositionClauses[MIMECOMPOSITIONSTRING_LENGTH];		// Composition ���ڿ��� �Ӽ��� ���� ���ڿ� ����
	int		m_nCompositionCaretPosition;				// Composition ���ڿ������� ĳ�� ��ġ

protected:
	/// Candidate List �׸���, �������� �������� �ʰ� ���� �׸���
	virtual void DrawCandidateList(MDrawContext* pDC, MPOINT& p);

public:
	Mint(void);
	virtual ~Mint(void);

	/// Mint Initialize
	bool Initialize(int nWorkspaceWidth, int nWorkspaceHeight, MDrawContext* pDC, MFont* pDefaultFont);
	/// Mint Finalize
	void Finalize(void);

#ifdef WIN32
	/// Event Processing
	bool ProcessEvent(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam);
#endif

	/// Mint Run
	virtual void Run(void);
	/// Mint Draw
	virtual void Draw(void);

	/// Event Action Key
//	bool EventActionKey(unsigned long int nKey, bool bPressed);

	/// Update(Redraw)
	virtual void Update(void){}

	/// Get MainFrame Widget
	MWidget* GetMainFrame(void);

	/// Get DrawContext
	MDrawContext* GetDrawContext(void);

	/// Singleton Instance
	static Mint* GetInstance(void);

#ifdef WIN32
	// ������ �ڵ� ����
	void SetHWND(HWND hWnd);
	HWND GetHWND(void);

	HIMC m_hImc;			// IME Handle, IME�� Enable/Disable�ϱ� ���� ������. IME���� �Լ��� ������ ImmGetContext()�� �� ����Ѵ�. ����, Disable�Ǿ� ������, ImmGetContext()�� NULL�� ������ ���̴�.
	//DWORD m_dwIMEConvMode;	// ������ IME ���
#endif

	void EnableIME(bool bEnable);
	bool IsEnableIME(void);

	/// ��Ű �߰�
	int RegisterHotKey(unsigned long int nModifier, unsigned long int nVirtKey);
	/// ��Ű ����
	void UnregisterHotKey(int nID);

	/*
	/// �׼Ǹ� �߰�
	bool RegisterActionKey(int nActionID, unsigned long int nKey);
	/// �׼Ǹ� ����
	bool UnregisterActionKey(int nActionID);

	/// Ű ��ȣ�� Ű �̸� �˾Ƴ���
	virtual const char* GetActionKeyName(unsigned long int nKey);
	/// ActionID�� Ű ��ȣ �˾Ƴ���
	unsigned long int GetActionKey(int nActionID);

	/// �׼� ��ư�� ���ȴ°�?
	bool IsActionKeyPressed(int nActionID);
	*/

	// Drag & Drop
	MWidget* SetDragObject(MWidget* pSender, MBitmap* pBitmap, const char* szString, const char* szItemString);
	MWidget* GetDragObject(void);

	virtual MWidget* NewWidget(const char* szClass, const char* szName, MWidget* pParent, MListener* pListener);

	MWidget* FindWidgetDropAble(MPOINT& p);
	MWidget* FindWidget(MPOINT& p);
	MWidget* FindWidget(int x, int y);

	/// Mint�� ���۵� ȭ���� ���� ũ�� ���
	int GetWorkspaceWidth(void);
	/// Mint�� ���۵� ȭ���� ���� ũ�� ���
	int GetWorkspaceHeight(void);
	/// Mint�� ���۵� ȭ���� ũ�� ����
	void SetWorkspaceSize(int w, int h);

	/// �ý��ۿ� �´� Bitmap Open Function�� �����ؾ� �Ѵ�.
	virtual MBitmap* OpenBitmap(const char* szName) = 0;
	/// �ý��ۿ� �´� Font Open Function�� �����ؾ� �Ѵ�.
	virtual MFont* OpenFont(const char* szName, int nHeight) = 0;

	void SetGlobalEvent(MGLOBALEVENTCALLBACK pGlobalEventCallback);

	// IME ���� �Լ���
	/// ���� �� ���� �⺻ ��Ʈ ���
	const char* GetDefaultFontName(void) const;

	/// �� ��� ID ���
	/// LANG_KOREAN, LANG_JAPANESE, LANG_CHINESE, etc... in "winnt.h"
	int GetPrimaryLanguageIdentifier(void) const;

	/// �� ��� ID ���
	/// �߱��� ��ü, ��ü�� ��� ���ؼ��� �� ������ �ʿ�
	/// SUBLANG_KOREAN, SUBLANG_CHINESE_TRADITIONAL, SUBLANG_CHINESE_SIMPLIFIED, ... in "winnt.h"
	int GetSubLanguageIdentifier(void) const;

	/// �ε�������(En, ��, ��, �� ��) ���
	const char* GetLanguageIndicatorString(void) const;

	/// Native ���� �����Ǿ� �ִ°�?
	bool IsNativeIME(void) const;

	/// WM_IME_NOTIFY�� IMN_OPENCANDIDATE ����, Candidate List �˾��� �� ȣ��
	void OpenCandidateList(void);
	/// WM_IME_NOTIFY�� IMN_CLOSECANDIDATE ����, Candidate List �˾��� ���� �� ȣ��
	void CloseCandidateList(void);

	// Candidate List ���� �Լ���
	/// �ε��������� Candidate ���ڿ� ���
	const char* GetCandidate(int nIndex) const;
	/// Candidate ���ڿ� �� ���� ���
	int GetCandidateCount(void) const;
	/// Candidate ���ڿ��� �� ���� ���õ� ���ڿ� �ε��� ���
	int GetCandidateSelection(void) const;
	/// ���� �������� ������ Candidate ���ڿ��� ù��° �ε��� ���
	int GetCandidatePageStart(void) const;
	/// �� �������� ������ Candidate ���ڿ��� ���� ���
	int GetCandidatePageSize(void) const;

	/// Candidate List�� ������ ��ġ �����ϱ�
	/// p�� ���յǴ� ������ ���� ��ġ
	/// �Ʒ��� ������ �����ϸ� ���� �������� �ϹǷ� ������ ���̸� �˾ƾ� �Ѵ�.
	void SetCandidateListPosition(MPOINT& p, int nWidgetHeight);
	/// Candidate List �ʺ�
	int GetCandidateListWidth(void);
	/// Candidate List ����
	int GetCandidateListHeight(void);

	/// Composition �Ӽ� ���� ���
	DWORD GetCompositionAttributeSize(void) const { return m_nCompositionAttributeSize; }
	/// Composition �Ӽ� ���
	const BYTE* GetCompositionAttributes(void) const { return m_nCompositionAttributes; }
	//const DWORD* GetCompositionClauses(void) const { return m_dwCompositionClauses; }

	/// �������� �Ӽ� �׸���
	/// p�� szComposition[i]�� ��ġ
	int DrawCompositionAttribute(MDrawContext* pDC, MPOINT& p, const char* szComposition, int i);
	/// p�� Composition�� �����ϴ� ��ġ
	void DrawCompositionAttributes(MDrawContext* pDC, MPOINT& p, const char* szComposition);
	/// ���ʿ� �ε������� �׸���
	void DrawIndicator(MDrawContext* pDC, MRECT& r);
};

/// Mint�� ���۵� ȭ���� ���� ũ�� ���
inline int MGetWorkspaceWidth(void){
	return Mint::GetInstance()->GetWorkspaceWidth();
}
/// Mint�� ���۵� ȭ���� ���� ũ�� ���
inline int MGetWorkspaceHeight(void){
	return Mint::GetInstance()->GetWorkspaceHeight();
}

/// 800*600�ػ󵵿��� UI�� �����ϱ� ������, ���� �ػ󵵿� �´� ������ ����� ��ȯ�� �� �ֵ��� �ϴ� ��ũ��
#define CONVERT800(x)	(int((x)/800.f * MGetWorkspaceWidth()))
#define CONVERT600(y)	(int((y)/600.f * MGetWorkspaceHeight()))

// Sample
void MCreateSample(void);
void MDestroySample(void);

/*
class MUIManager
{
private:
	MIDLResource*	m_pResource;
public:
	MUIManager() { }
	virtual ~MUIManager() { }
//	void Control(string szItem, MWidget* );
};
*/

#ifdef WIN32
#define MMODIFIER_ALT	MOD_ALT
#define MMODIFIER_CTRL	MOD_CONTROL
#define MMODIFIER_SHIFT	MOD_SHIFT
#else
#define MMODIFIER_ALT	1
#define MMODIFIER_CTRL	2
#define MMODIFIER_SHIFT	4
#endif

#define MIsActionKeyPressed(_ActionID)	(Mint::GetInstance()->IsActionKeyPressed(_ActionID))

#define IsHangul(x) ((unsigned char)(x)>127) 

#endif