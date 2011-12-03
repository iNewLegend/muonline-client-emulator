#include "stdafx.h"

#include "ZNetCharacter.h"
#include "ZGameInterface.h"
#include "ZGame.h"
#include "ZApplication.h"

MImplementRTTI(ZNetCharacter, ZCharacter);

ZNetCharacter::ZNetCharacter() : ZCharacter()
{

}

ZNetCharacter::~ZNetCharacter()
{

}

void  ZNetCharacter::OnUpdate(float fDelta)
{
	if (m_bInitialized==false) return;
	if (!IsVisible()) return;

	float fTimeLapse = ZGetGame()->GetTime() - m_fLastValidTime;
	m_fLastValidTime = ZGetGame()->GetTime();

	ZCharacter::OnUpdate(fDelta);

	// ���� �����̴� �κ��� module_movable �� �Űܰ���

	if(IsMoveAnimation())		// �ִϸ��̼ǿ� �������� ���Եȳ��� ����������Ѵ�.
	{
		rvector origdiff=fDelta*GetVelocity();

		rvector diff = m_AnimationPositionDiff.Ref();
		diff.z+=origdiff.z;
		//		rvector diff=m_RealPositionBefore-m_RealPosition;	// mesh �� ��ġ ��ȭ�� ����������
		if(GetDistToFloor()<0 && diff.z<0) diff.z=-GetDistToFloor();

		Move(diff);

		// debug
		/*
		mlog("%d pos : %3.3f %3.3f %3.3f      ani-move : %3.3f %3.3f %3.3f\n",
			m_AniState_Lower,
			m_Position.x,m_Position.y,m_Position.z,diff.x,diff.y,diff.z);
		*/
	}

	UpdateHeight(fDelta);
}

void ZNetCharacter::OnDraw()
{
	ZCharacter::OnDraw();
}

void ZNetCharacter::SetNetPosition(rvector& pos)
{
	// TODO : �̰Ͷ����� ���� �����°�찡 ���� �ִ�
	if (Magnitude(pos - GetPosition()) > 20.0f)
	{
		SetPosition( pos);
	}
}

void ZNetCharacter::SetNetPosition(rvector& position, rvector& velocity, rvector& dir)
{

	SetNetPosition(position);
	SetVelocity(velocity);
	SetAccel(rvector(0.0f, 0.0f, 0.0f));

/*
	float fT = 0.1f;
	//rvector pretar = position + velocity * fT + (0.5f * accel * fT * fT);
	rvector pretar = position + velocity * fT;
	rvector v = (pretar - m_Position) / fT;

	SetNetPosition(position);
	SetVelocity(v);
	SetAccel(rvector(0.0f,0.0f,0.0f));
*/



	m_TargetDir = dir;
	m_fLastValidTime = ZGetGame()->GetTime();
}