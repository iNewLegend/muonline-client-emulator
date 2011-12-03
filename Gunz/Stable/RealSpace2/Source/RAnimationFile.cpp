#include "stdafx.h"
#include "RMesh.h"
#include "RAnimationFile.h"

#include "RealSpace2.h"

#include "MZFileSystem.h"

_USING_NAMESPACE_REALSPACE2

_NAMESPACE_REALSPACE2_BEGIN

RAnimationFile::RAnimationFile()
{
//	m_filename[0]	= 0;
//	m_NameID		= -1;

	m_ani_node		= NULL;
	m_ani_node_cnt	= 0;
	m_ani_type		= RAniType_Bone;

	m_pBipRootNode = NULL;

	m_max_frame		= 0;
	m_nRefCount		= 0;

	AddRef();
}

RAnimationFile::~RAnimationFile()
{
	if( m_ani_node ) {
		for(int i=0;i<m_ani_node_cnt;i++) {
			delete m_ani_node[i];
		}
		delete[] m_ani_node;
		m_ani_node = NULL;
	}

	DecRef();

	if(m_nRefCount != 0) {//����..

	}
}

void RAnimationFile::AddRef()
{
	m_nRefCount++;
}

void RAnimationFile::DecRef()
{
	m_nRefCount--;

	if(m_nRefCount==0) {//������ �ұ�?

	}
}

/*
char* RAnimationFile::GetName() {
	return m_filename;
}

void RAnimationFile::SetName(char* name) {
	if(!name) return;
	strcpy( m_filename , name );
}
*/

RAnimationNode* RAnimationFile::GetNode(char* name)
{
	for(int i=0;i<m_ani_node_cnt;i++) {
		if(m_ani_node[i]->CheckName(name))
			return m_ani_node[i];
	}
	return NULL;
}

bool RAnimationFile::LoadAni(char* filename)
{
	int node_cnt=0;

	char t_mesh_name[256];

	MZFile mzf;

	if(g_pFileSystem) {
		if(!mzf.Open(filename,g_pFileSystem)) {
			if(!mzf.Open(filename)) {
				mlog("in zip : %s file not found!! \n ", filename );
				return false;
			}
		}
	} else {
		if(!mzf.Open(filename)) {
			mlog("%s file not found!! \n ", filename );
			return false;
		}
	}

	ex_ani_t t_hd;

	mzf.Read(&t_hd,sizeof(ex_ani_t));

	// 0,2 �� ���� ���� tm ��������
	// 1 �� ���ؽ�
	// 3 �� ��Ʈ���� ���ø�

	DWORD ver = t_hd.ver;

	m_ani_node_cnt = t_hd.model_num;

	if( m_ani_node_cnt == 0 ) {
		mlog("Ű�� ���� %s ���ϸ��̼� ���\n",filename);
		mzf.Close();
		return false;
	}

	m_ani_node = new RAnimationNode*[m_ani_node_cnt];

	RAnimationNode* pANode = NULL;

	int vis_max_frame = 0;
	int max_frame = 0;

	int mode = t_hd.ani_type;

	m_ani_type = (AnimationType)mode;

	if( mode == RAniType_Vertex) {

		int i,j,vcnt;

		for(i=0;i<m_ani_node_cnt;i++) {

			m_ani_node[i] = new RAnimationNode;

			pANode = m_ani_node[i];

			mzf.Read(t_mesh_name ,MAX_NAME_LEN );
//			strcpy(pANode->m_Name,t_mesh_name);
			pANode->SetName(t_mesh_name);

			//			pANode->ConnectToNameID();

			mzf.Read(&pANode->m_vertex_cnt,4 );//frame����ŭ

			if(pANode->m_vertex_cnt) {
				pANode->m_vertex = new D3DXVECTOR3*[pANode->m_vertex_cnt];
			}

			mzf.Read(&vcnt,4 );

			pANode->m_vertex_vcnt = vcnt;

			if(pANode->m_vertex_cnt) {
				pANode->m_vertex_frame = new DWORD[pANode->m_vertex_cnt];
			}

			mzf.Read(pANode->m_vertex_frame,sizeof(DWORD)*pANode->m_vertex_cnt);

			for(j=0;j<pANode->m_vertex_cnt;j++) {

				pANode->m_vertex[j] = new D3DXVECTOR3[vcnt];
				mzf.Read(pANode->m_vertex[j],sizeof(D3DXVECTOR3)*vcnt);
			}

			if(ver > EXPORTER_ANI_VER1) {
				mzf.Read(&pANode->m_vis_cnt,sizeof(DWORD) );

				if(pANode->m_vis_cnt) {
					pANode->m_vis = new RVisKey[pANode->m_vis_cnt];
					mzf.Read(pANode->m_vis,sizeof(RVisKey)*pANode->m_vis_cnt);

					if(pANode->m_vis[pANode->m_vis_cnt-1].frame > vis_max_frame) {
						vis_max_frame = pANode->m_vis[ pANode->m_vis_cnt-1 ].frame;
					}
				}
			}
		}

		int cnt = 0;

		// ���̳�尡 �����ִ� ��찡 �ִ�..

		for(i=0;i<m_ani_node_cnt;i++) {

			cnt = m_ani_node[i]->m_vertex_cnt;

			if(cnt) {
				max_frame = m_ani_node[i]->m_vertex_frame[cnt-1];
				break;
			}
		}

	}
	else if( mode == RAniType_Tm ) {

		for(int i=0;i<m_ani_node_cnt;i++) {

			m_ani_node[i] = new RAnimationNode;

			pANode = m_ani_node[i];

			mzf.Read(t_mesh_name ,MAX_NAME_LEN );
//			strcpy(pANode->m_Name,t_mesh_name);
			pANode->SetName(t_mesh_name);
			//			pANode->ConnectToNameID();

			mzf.Read(&pANode->m_mat_cnt,4 );
			pANode->m_mat = new RTMKey[pANode->m_mat_cnt];

			mzf.Read(pANode->m_mat,sizeof(RTMKey)*pANode->m_mat_cnt);

			if(ver > EXPORTER_ANI_VER1) {
				mzf.Read(&pANode->m_vis_cnt,sizeof(DWORD) );

				if(pANode->m_vis_cnt) {
					pANode->m_vis = new RVisKey[pANode->m_vis_cnt];
					mzf.Read(pANode->m_vis,sizeof(RVisKey)*pANode->m_vis_cnt);

					if(pANode->m_vis[pANode->m_vis_cnt-1].frame > vis_max_frame) {
						vis_max_frame = pANode->m_vis[ pANode->m_vis_cnt-1 ].frame;
					}
				}
			}
			pANode->m_mat_base = pANode->m_mat[0];//.m;
		}

		int cnt = 0;

		// ���̳�尡 �����ִ� ��찡 �ִ�..

		for(i=0;i<m_ani_node_cnt;i++) {

			cnt = m_ani_node[i]->m_mat_cnt;

			if(cnt) {
				max_frame = m_ani_node[i]->m_mat[cnt-1].frame;
				break;
			}
		}

	}
	else {

		for(int i=0;i<m_ani_node_cnt;i++) {

			m_ani_node[i] = new RAnimationNode;

			pANode = m_ani_node[i];

			mzf.Read(t_mesh_name  ,MAX_NAME_LEN );
			mzf.Read(&pANode->m_mat_base,sizeof(D3DXMATRIX) );//mat

//			strcpy(pANode->m_Name,t_mesh_name);
			pANode->SetName(t_mesh_name);

			if(strcmp(pANode->GetName(),"Bip01")==0) {
				m_pBipRootNode = pANode;
			}

			//			pANode->ConnectToNameID();

			int pos_key_num = 0;
			int rot_key_num = 0;
			int vertex_num	= 0;

			mzf.Read(&pos_key_num,4 );

			pANode->m_pos_cnt = pos_key_num;

			if(pos_key_num) {

				pANode->m_pos = new RPosKey[pos_key_num+1];

				mzf.Read(pANode->m_pos,sizeof(RPosKey)*pos_key_num);

				pANode->m_pos[pos_key_num] = pANode->m_pos[pos_key_num-1];

				if(pANode->m_pos[pos_key_num].frame > max_frame) {
					max_frame = pANode->m_pos[pos_key_num].frame;
				}
			}

			mzf.Read(&rot_key_num,4 );

			pANode->m_rot_cnt = rot_key_num;

			if(rot_key_num) {

				RQuatKey q;

				D3DXQUATERNION eq,q1,q2;

				pANode->m_quat = new RQuatKey[rot_key_num+1];

				RRotKey t_rk;

				for(int j=0;j<rot_key_num;j++) {

					if(ver > EXPORTER_ANI_VER3) {

						mzf.Read(&pANode->m_quat[j],sizeof(RQuatKey) );

					} else {// old

						mzf.Read(&t_rk,sizeof(RRotKey) );

						RRot2Quat(q,t_rk);

						memcpy(&pANode->m_quat[j] , &q , sizeof(RQuatKey));
						pANode->m_quat[j].frame  = t_rk.frame;
					}
				}

				pANode->m_quat[rot_key_num] = pANode->m_quat[rot_key_num-1];

				if(pANode->m_quat[rot_key_num].frame > max_frame) {
					max_frame = pANode->m_quat[rot_key_num].frame;
				}
			}

			if(ver > EXPORTER_ANI_VER1) {
				mzf.Read(&pANode->m_vis_cnt,sizeof(DWORD) );

				if(pANode->m_vis_cnt) {
					pANode->m_vis = new RVisKey[pANode->m_vis_cnt];
					mzf.Read(pANode->m_vis,sizeof(RVisKey)*pANode->m_vis_cnt);

					if(pANode->m_vis[pANode->m_vis_cnt-1].frame > vis_max_frame) {
						vis_max_frame = pANode->m_vis[ pANode->m_vis_cnt-1 ].frame;
					}
				}
			}
		}
	}

	m_max_frame = max_frame;

	if(m_max_frame < vis_max_frame) {
		m_max_frame = vis_max_frame;
	}

	//	fclose (fp);
	mzf.Close();

	return true;
}

_NAMESPACE_REALSPACE2_END