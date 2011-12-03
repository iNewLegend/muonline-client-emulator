#include "stdafx.h"
#include "RMesh.h"
#include "RAnimationMgr.h"

#include "RealSpace2.h"

#include "MDebug.h"

//#include "MZFileSystem.h"

_USING_NAMESPACE_REALSPACE2

_NAMESPACE_REALSPACE2_BEGIN

/////////////////////////////////////////////////////////////////////

RAnimationFileMgr::RAnimationFileMgr()
{

}

RAnimationFileMgr::~RAnimationFileMgr()
{
	Destroy();
}

void RAnimationFileMgr::Destroy()
{
	RAnimationFileHashList_Iter  node;

//	mlog("���ϸ��̼� ���� ���� size %d \n",m_list.size() );

	for(node = m_list.begin(); node != m_list.end(); ) {
		delete (*node);
		node = m_list.erase(node);
	}

	m_list.Clear();
}

RAnimationFileMgr* RAnimationFileMgr::GetInstance()
{
	static RAnimationFileMgr m_AniMgr;
	return &m_AniMgr;
}

RAnimationFile* RAnimationFileMgr::Add(char* filename)
{
	RAnimationFile* pFile = Get(filename);

	if( pFile ) {
		pFile->AddRef();
//		mlog("���ϸ��̼� ���� �ߺ���� %s ref_cnt %d \n",filename,pFile->m_nRefCount);
		return pFile;
	}

//	mlog("���ϸ��̼� ���� ���� %s %d \n",filename,m_list.size());

	pFile = new RAnimationFile;

	pFile->LoadAni( filename );

	pFile->SetName(filename);

	m_list.PushBack( pFile );

	return pFile;
}

RAnimationFile* RAnimationFileMgr::Get(char* filename)
{
	return m_list.Find(filename);
}

/////////////////////////////////////////////////////////////////////

RAnimationMgr::RAnimationMgr() {
	m_id_last = 0;
//	m_node_table.reserve(MAX_ANIMATION_NODE);//�⺻
//	for(int i=0;i<MAX_ANIMATION_NODE;i++)
//		m_node_table[i] = NULL;
	m_list_map = NULL;
	m_list_map_size = 0;
}

RAnimationMgr::~RAnimationMgr() {
	DelAll();
}


RAnimation* RAnimationMgr::AddAnimationFile(char* name,char* filename,int sID,bool notload,int MotionTypeID) {

	RAnimation* node = new RAnimation;

	if(notload) {//���߿� load
		node->SetLoadDone(false);
	}
	else {

		if (!node->LoadAni(filename)) {
			mlog("elu %s file loading failure !!!\n",filename);
			delete node;
			return NULL;
		}
		node->SetLoadDone(true);
	}

	node->SetFileName(filename);
	node->SetName(name);
	node->m_sID = sID;
	node->SetWeaponMotionType(MotionTypeID);

	m_node_table.push_back(node);
//	m_node_table[m_id_last] = node;
	m_id_last++;

	if(m_id_last > MAX_ANIMATION_NODE)
		mlog("���ϸ��̼� ��� ���� ����� �ø��°��� ������..\n",filename);

	m_list.PushBack(node);

	if(m_list_map) {
		if(MotionTypeID != -1) {
			if(m_list_map_size > MotionTypeID) {
				m_list_map[MotionTypeID].PushBack(node);
			}
		}
	}

	return node;
}

void RAnimationMgr::DelAll() {

	RAnimationHashList_Iter node;

	if(m_list_map_size) {

		for(int i=0;i<m_list_map_size;i++) {
			m_list_map[i].Clear();
		}

		delete [] m_list_map;
		m_list_map	= NULL;
		m_list_map_size = 0;
	}

//	if(m_list.size())
//		mlog("ranimationmgr::del_all size = %d \n ",m_list.size() );

	for(node = m_list.begin(); node != m_list.end(); ) {
		delete (*node);
		node = m_list.erase(node);
	}

	m_list.Clear();

	if(!m_node_table.empty())
		m_node_table.clear();//���۴� ���� �ִ�..

	m_id_last = 0;
}

void RAnimationMgr::ReloadAll()
{
	RAnimationHashList_Iter node;
	RAnimation* pANode = NULL;

	for(node = m_list.begin(); node != m_list.end(); ++node) {
		pANode = *node;
		if(pANode) {
			if(pANode->IsLoadDone()==false) {
				if (!pANode->LoadAni(pANode->m_filename)) {
					mlog("elu %s file loading failure !!!\n",pANode->m_filename);
				}
				pANode->SetLoadDone(true);
			}
		}
	}
}

void RAnimationMgr::MakeListMap(int size)
{
	if(m_list_map)//Ŭ�����ϱ������� ������..
		return;

	if( size > 100 ) {
		mlog("RAnimationMgr::MakeListMap %d �� �ʹ����Ѱ� �ƴѰ�?\n",size);
	}

	m_list_map = new RAnimationHashList[size];
	m_list_map_size = size;
}

RAnimation* RAnimationMgr::GetAnimationListMap(char* name,int wtype) {

	if(m_list_map_size==0) return NULL;

	if(m_list_map_size-1 < wtype) {
		return NULL;
	}

	RAnimation* pAni = m_list_map[wtype].Find(name);

	return pAni;
}

RAnimation* RAnimationMgr::GetAnimation(char* name,int wtype) 
{
	if(!name) 
		return NULL;

	if(name[0]==0) 
		return NULL;

	if(m_list.empty())
		return NULL;

	RAnimation* pAni = NULL;

	if(wtype != -1)
		pAni = GetAnimationListMap(name,wtype);

	if(pAni) {
		return pAni;
	}

	pAni = m_list.Find(name);
	return pAni;
}

RAnimation* RAnimationMgr::GetAnimation(int sID,int wtype) {

	RAnimationHashList_Iter node;

	if( m_list.empty() ) 
		return NULL;

	for(node = m_list.begin(); node != m_list.end(); ++node) {
		if( (*node)->CheckWeaponMotionType(wtype) )
			if( (*node)->m_sID == sID )
				return *node;
	}
	return NULL;
}

_NAMESPACE_REALSPACE2_END
