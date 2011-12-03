#ifndef _MSCALABLELOOK_H
#define _MSCALABLELOOK_H

#include "Mint.h"

#define DEFAULT_BASIC_WIDTH	1600

class MScalableLook {

public:

	MScalableLook() { m_bScale = false; m_nBasicWidth = DEFAULT_BASIC_WIDTH; }

	bool		m_bScale;		// �ػ󵵿� ���� (��������) �������Ѵ�
	int			m_nBasicWidth;	// ���� �ػ� Width

	void SetScaleEnable(bool b) {
		m_bScale = b;
	}

	void SetBasicWidth(int n) {
		m_nBasicWidth = n;
	}


	// ���� �ػ󵵿� ���� �����ػ��� ���� jintriple3
	float GetScale() { 
		//return m_bScale ? 
		//	( (float)MGetWorkspaceWidth()/(float)m_nBasicWidth >0.7f ? 0.5f : (float)MGetWorkspaceWidth()/(float)m_nBasicWidth)
		//	: 0.5f ; 
		
		//return 0.5f;

		if (m_bScale)
			return (float)MGetWorkspaceWidth()/(float)m_nBasicWidth;
		return 0.5f;
	}
};

#endif