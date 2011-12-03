#ifndef MANIMATION_H
#define MANIMATION_H

#include "MWidget.h"

enum MAPlayMode{
	MAPM_FORWARDONCE,
	MAPM_FORWARDNBACKWARD,
	MAPM_REPETITION,
};

class MAnimation : public MWidget{
private:
	unsigned int	m_nStartTime;
	MAniBitmap*		m_pAniBitmap;
public:
	int				m_nDelay;
	MAPlayMode		m_nPlayMode;
protected:
	virtual void OnDraw(MDrawContext* pDC);
public:
	MAnimation(const char* szName=NULL, MAniBitmap* pAniBitmap=NULL, MWidget* pParent=NULL);
	void SetAniBitmap(MAniBitmap* pAniBitmap);
	void InitStartTime();
	MBitmap* GetBitmap();

#define MINT_ANIMATION	"Animation"
	virtual const char* GetClassName(void){ return MINT_ANIMATION; }



/* ��ȯ���� �߰� �κ� :
    
	�Ʒ� �߰��� �κ��� Ư�� ��Ȳ���� �̹����� ���÷� �����ϴ� ��� �̸� �ִϸ��̼� ��������
	�ذ��ϱ� ���ؼ� �߰��� ���̴�. ���� ���ؼ� �ִϸ��̼��� ������Ų ���¿��� ������ ������
	�̹��� ��ȣ�� �ٲ��ִ� �����̴�.
*/
protected:
	int				m_nCurrFrame;;			// ���� �������� �ִ� �ִϸ��̼� �̹��� ��ȣ

public:
	bool			m_bRunAnimation;		// ���� �ִϸ��̼��� �ǰ��ִ��� ����
	bool			GetRunAnimation( void)  { return m_bRunAnimation;}
	void			SetRunAnimation( bool bRun);
	int				GetCurrentFrame( void);
	void			SetCurrentFrame( int nFrame);
};

#endif