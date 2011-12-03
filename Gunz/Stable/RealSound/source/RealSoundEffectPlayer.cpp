#include "stdafx.h"
#include "RealSoundEffectPlayer.h"
#include "RealSoundEffect.h"
#include "MDebug.h"

RealSoundEffectPlayer::RealSoundEffectPlayer(void)
{
	m_fX = m_fY = m_fZ = 0;
}

RealSoundEffectPlayer::~RealSoundEffectPlayer(void)
{
	for(list<RealSoundEffectPlay*>::iterator i=m_SEs.begin(); i!=m_SEs.end(); i++){
		RealSoundEffectPlay* pSE = *i;
		delete pSE;
		pSE = NULL;
	}
	m_SEs.clear();
	mSndBuffer.clear();
}

RealSoundEffectPlay* RealSoundEffectPlayer::Play(RealSoundEffectSource* pSES, float x, float y, float z, bool bLoop, bool bDupCheck, float fDigVol, bool b3D, float fMinDistance, float fMaxDistance, RealSoundEffectMode nMode)
{
	if(fDigVol==0.0f) return NULL;
	
	if(pSES==NULL) return NULL;
	if(bDupCheck==true){
		for(list<RealSoundEffectPlay*>::iterator i=m_SEs.begin(); i!=m_SEs.end(); i++){
			RealSoundEffectPlay* pSE = *i;
			if(pSE->m_pSES==pSES) return NULL;
		}
	}

	// �÷��� �ϴ� ��� ���ۿ� ��Ƶд�
	// �̶� �ߺ��Ǵ� ����� �����Ѵ�
	//SNDB_Iter iter = mSndBuffer.find( string(pSES->getFileName()) );
	//if( iter == mSndBuffer.end() )
//	{
	RealSoundEffectPlay* pNewSE = new RealSoundEffectPlay(pSES, b3D, fMinDistance, fMaxDistance, nMode );
	pNewSE->SetVolume(fDigVol);
	pNewSE->SetPos( x, y, z );
	pNewSE->SetLoop( bLoop );
	//mSndBuffer.insert( SNDBufList::value_type(string(pSES->getFileName()), pNewSE ));
	pNewSE->Play( x, y, z, bLoop );
	m_SEs.push_back( pNewSE);

	return pNewSE;
//	}

	//return NULL;
}

bool RealSoundEffectPlayer::IsPlaying(void)
{
	for(list<RealSoundEffectPlay*>::iterator i=m_SEs.begin(); i!=m_SEs.end(); i++){
		RealSoundEffectPlay* pSE = *i;
		if(pSE->IsPlaying()==true) return true;
	}
	return false;
}

void RealSoundEffectPlayer::RemoveLoopPlay(void)
{
	for(list<RealSoundEffectPlay*>::iterator i=m_SEs.begin(); i!=m_SEs.end(); ){
		RealSoundEffectPlay* pSE = *i;
		if(pSE->IsPlayingLoop()==true){
			pSE->Stop();
			delete pSE;
			pSE = NULL;
			list<RealSoundEffectPlay*>::iterator j = i;
			i++;
			m_SEs.erase(j);
		}
		else i++;
	}
}

void RealSoundEffectPlayer::Move(RealSoundEffectPlayer* pSource)
{
	while(pSource->m_SEs.empty()==false){
		m_SEs.insert(m_SEs.end(), *pSource->m_SEs.begin());
		pSource->m_SEs.erase(pSource->m_SEs.begin());
	}
}

void RealSoundEffectPlayer::SetPos(float x, float y, float z) // ��� ���� ����Ʈ�� ��ǥ�� �� ������ �ٲ۴�.. ������ ���� �����ΰ�?
{
	m_fX = x;
	m_fY = y;
	m_fZ = z;

	for(list<RealSoundEffectPlay*>::iterator i=m_SEs.begin(); i!=m_SEs.end(); ){
		RealSoundEffectPlay* pSE = *i;
		if(pSE->IsPlaying()==false){
			pSE->Stop();
			delete pSE;
			pSE = NULL;
			list<RealSoundEffectPlay*>::iterator j = i;
			i++;
			m_SEs.erase(j);
			continue;
		}
		else i++;

		pSE->SetPos(x, y, z);
	}
}

void RealSoundEffectPlayer::GetPos(float* x, float* y, float* z)
{
	*x = m_fX;
	*y = m_fY;
	*z = m_fZ;
}

void RealSoundEffectPlayer::Run(void)
{
	for(list<RealSoundEffectPlay*>::iterator i=m_SEs.begin(); i!=m_SEs.end(); )
	{
		RealSoundEffectPlay* pSE = *i;
		//mlog("sound list : %s\n", pSE->m_pSES->getFileName() );
		if( pSE == NULL )
		{
			++i;
			continue;
		}
		if(pSE->IsPlaying()==false){
			//mlog(" deleted sound : %s\n", pSE->m_pSES->getFileName() );
			pSE->Stop();
			delete pSE;
			pSE = NULL;
			list<RealSoundEffectPlay*>::iterator j = i;
			i++;
			m_SEs.erase(j);
		}
		else i++;
	}

	//// ���ۿ� ��Ƶ� ���带 �÷��� �Ѵ�
	//for( SNDB_Iter iter = mSndBuffer.begin(); iter != mSndBuffer.end(); ++iter )
	//{
	//	RealSoundEffectPlay* pSE = iter->second;
	//	pSE->Play();
	//	m_SEs.push_back( pSE );	// ���� ����Ʈ�� �߰��Ѵ�
	//}
	//mSndBuffer.clear();				// ���� Ŭ����
}