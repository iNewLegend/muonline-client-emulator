#ifndef _ZRANGEWEAPONHITDICE_H
#define _ZRANGEWEAPONHITDICE_H

#include "RTypes.h"

// ��� ���͸� ������ Actor�� �� ������ ���ϴ� ���� Ŭ����
class ZRangeWeaponHitDice
{
private:
	float		m_fHitProbability;		// ���� Ȯ��

	float		m_fGlobalFactor;
	float		m_fDistance;
	float		m_fTargetHeight;
	float		m_fTargetWidth;
	float		m_fTargetSpeed;
	rvector		m_TargetPosition;
	rvector		m_SourcePosition;

	void MakeHitProbability();
public:
	ZRangeWeaponHitDice();
	void BuildTargetBounds(float fWidth, float fHeight);	// ����� ũ��
	void BuildTargetSpeed(float fSpeed);					// ����� �ӵ�
	void BuildTargetPosition(rvector& vPosition);			// ��� ��ġ
	void BuildSourcePosition(rvector& vPosition);			// ��� ��� ��ġ
	void BuildGlobalFactor(float fGlobalFactor);			// ���������� ��ġ�� ����
	
	rvector ReturnShotDir();
	float GetHitProbability()		{ return m_fHitProbability; }
};







#endif