#include "stdafx.h"

#include "ZMapDesc.h"
#include "RealSpace2.h"
#include "RToken.h"
#include "ZApplication.h"
#include "ZGame.h"

#define ZTOK_SPAWN			"spawn"
#define ZTOK_SPAWN_SOLO		"spawn_solo"
#define ZTOK_SPAWN_TEAM		"spawn_team"

#define ZTOK_SPAWN_NPC_MELEE	"spawn_npc_melee"
#define ZTOK_SPAWN_NPC_RANGE	"spawn_npc_range"
#define ZTOK_SPAWN_NPC_BOSS		"spawn_npc_boss"


#define ZTOK_WAIT_CAMERA_POS	"wait_pos"

#define ZTOK_SMOKE_SS			"smk_ss_"
#define ZTOK_SMOKE_TS			"smk_ts_"
#define ZTOK_SMOKE_ST			"smk_st_"

#define ZTOK_DUMMY_LINK			"link"

ZMapSpawnManager::ZMapSpawnManager()
{
	for(int i=0;i>MAX_BACKUP_SPAWN;i++)
		m_nBackUpIndex[i] = -1;
	m_nBackUpIndexCnt = MAX_BACKUP_SPAWN;
}

ZMapSpawnManager::~ZMapSpawnManager()
{
	Clear();
}

bool ZMapSpawnManager::Add(ZMapSpawnData* pMapSpawnData)
{
	m_Spawns.push_back(pMapSpawnData);

	if (!strnicmp(pMapSpawnData->m_szSpawnName, ZTOK_SPAWN_SOLO, strlen(ZTOK_SPAWN_SOLO)))
	{
		pMapSpawnData->m_nType = ZMST_SOLO;
		m_SpawnArray[ZMST_SOLO].push_back(pMapSpawnData);
	}
	else if (!strnicmp(pMapSpawnData->m_szSpawnName, ZTOK_SPAWN_TEAM, strlen(ZTOK_SPAWN_TEAM)))
	{
		if (strlen(pMapSpawnData->m_szSpawnName) > 11)
		{
			if (pMapSpawnData->m_szSpawnName[10] == '1')
			{
				pMapSpawnData->m_nType = ZMST_TEAM1;
				m_SpawnArray[ZMST_TEAM1].push_back(pMapSpawnData);
			}
			else if (pMapSpawnData->m_szSpawnName[10] == '2')
			{
				pMapSpawnData->m_nType = ZMST_TEAM2;
				m_SpawnArray[ZMST_TEAM2].push_back(pMapSpawnData);
			}
		}
	}
	else if (!strnicmp(pMapSpawnData->m_szSpawnName, ZTOK_SPAWN_NPC_MELEE, strlen(ZTOK_SPAWN_NPC_MELEE)))
	{
		pMapSpawnData->m_nType = ZMST_NPC_MELEE;
		m_SpawnArray[ZMST_NPC_MELEE].push_back(pMapSpawnData);
	}
	else if (!strnicmp(pMapSpawnData->m_szSpawnName, ZTOK_SPAWN_NPC_RANGE, strlen(ZTOK_SPAWN_NPC_RANGE)))
	{
		pMapSpawnData->m_nType = ZMST_NPC_RANGE;
		m_SpawnArray[ZMST_NPC_RANGE].push_back(pMapSpawnData);
	}
	else if (!strnicmp(pMapSpawnData->m_szSpawnName, ZTOK_SPAWN_NPC_BOSS, strlen(ZTOK_SPAWN_NPC_BOSS)))
	{
		pMapSpawnData->m_nType = ZMST_NPC_BOSS;
		m_SpawnArray[ZMST_NPC_BOSS].push_back(pMapSpawnData);
	}

	return true;
}

bool ZMapSpawnManager::AddSpawnData(char* szName, rvector& pos, rvector& dir)
{
	ZMapSpawnData*	pSpawnData = new ZMapSpawnData;

	pSpawnData->m_nType = ZMST_SOLO;
	strcpy(pSpawnData->m_szSpawnName, szName);
	pSpawnData->m_Pos = pos;
	pSpawnData->m_Dir = dir;

	return Add(pSpawnData);
}

void ZMapSpawnManager::Clear()
{
	for (ZMapSpawnList::iterator itor = m_Spawns.begin(); itor != m_Spawns.end(); ++itor)
	{
		ZMapSpawnData* pSpawnData = (*itor);
		delete pSpawnData;
	}
	m_Spawns.clear();

	for (int i = 0; i < ZMST_MAX; i++)
	{
		m_SpawnArray[i].clear();
	}

	for(i=0;i>MAX_BACKUP_SPAWN;i++) 
		m_nBackUpIndex[i] = -1;

	m_nBackUpIndexCnt = 0;
}

ZMapSpawnData* ZMapSpawnManager::GetData(int nIndex)
{
	if (nIndex >= GetCount()) return NULL;


	return (m_Spawns[nIndex]);
}

ZMapSpawnData* ZMapSpawnManager::GetSoloData(int nIndex)
{
	if ((nIndex < 0) || (nIndex >= GetSoloCount())) return NULL;

	return (m_SpawnArray[ZMST_SOLO][nIndex]);
}

ZMapSpawnData* ZMapSpawnManager::GetTeamData(int nTeamIndex, int nDataIndex)
{
	if ((nTeamIndex < 0) || (nTeamIndex >= MMATCH_TEAM_MAX_COUNT)) 
	{
		_ASSERT(0);
		return NULL;
	}

	int nTeamPositionsCount = GetTeamCount(nTeamIndex);
	if (nTeamPositionsCount <= 0)
	{
		// ���� �� ���� �������� 1���� ����.
		_ASSERT(0);
		return NULL;
	}

	if (nDataIndex < 0) nDataIndex = 0;
	while (nDataIndex >= nTeamPositionsCount)
	{
		nDataIndex -= nTeamPositionsCount;
	}

	ZMapSpawnType nSpawnType = ZMST_TEAM1;
	if (nTeamIndex == 1) nSpawnType = ZMST_TEAM2;

	return (m_SpawnArray[nSpawnType][nDataIndex]);
}

int ZMapSpawnManager::GetTeamCount(int nTeamIndex)
{
	ZMapSpawnType nSpawnType = ZMST_TEAM1;
	if (nTeamIndex == 1) nSpawnType = ZMST_TEAM2;

	return (int)(m_SpawnArray[nSpawnType].size());
}

bool ZMapSpawnManager::CheckBackup(int index)
{
	for(int i=0;i<MAX_BACKUP_SPAWN;i++) {
		if(m_nBackUpIndex[i]==index)
			return false;
	}
	return true;
}

void ZMapSpawnManager::ShiftBackupIndex(int index)
{
	for(int i=0;i<MAX_BACKUP_SPAWN-1;i++) {
		m_nBackUpIndex[i] = m_nBackUpIndex[i+1];
	}
	m_nBackUpIndex[MAX_BACKUP_SPAWN-1] = index;
}

ZMapSpawnData* ZMapSpawnManager::GetSoloRandomData()
{
	int nIndex = -1;
	unsigned long nRandomNumber = timeGetTime();	// rand() �� ��� peer�� seed���� ���� ����� ���� �ʴ´�.

	m_nBackUpIndexCnt = min(GetSoloCount(),MAX_BACKUP_SPAWN);

	// ������ ������ ��ġ�� ���Ѵ�..
	if(GetSoloCount() > MAX_BACKUP_SPAWN) {

		int cnt = 0;

		while(1) {

			nRandomNumber = timeGetTime() * rand();
			nIndex = nRandomNumber % GetSoloCount();

			if( CheckBackup(nIndex) ) {
				break;
			}

			cnt++;

			if(cnt > 1000)  { // Ȥ�ó�..
				nIndex = 0;
				break;
			}
		}

		// backup
		ShiftBackupIndex(nIndex);
	}
	else { // ������ �ʹ� ���ٸ� �׳� �������
		if (GetSoloCount() > 0) 
			nIndex = nRandomNumber % GetSoloCount();
	}

//	mlog("spawn index %d \n" , nIndex);

	ZMapSpawnData* pSpawnData = GetSoloData(nIndex);

	int t = 0;
	while ((pSpawnData != NULL) && (IsExistOtherCharacter(pSpawnData->m_Pos)))
	{
		nRandomNumber++;
		nIndex = nRandomNumber % GetSoloCount();
		pSpawnData = GetSoloData(nIndex);

		if (t > 999) return pSpawnData; t++;
	}
	
	return pSpawnData;
}


bool ZMapSpawnManager::IsExistOtherCharacter(rvector pos)
{
	if (ZGetGame() == NULL) return false;

	bool bExist = false;
	for (ZCharacterManager::iterator itor = ZGetGame()->m_CharacterManager.begin();
		itor != ZGetGame()->m_CharacterManager.end(); ++itor)
	{
		if ((*itor).second == ZGetGame()->m_pMyCharacter) continue;

		rvector dis = pos - ((*itor).second)->GetPosition();

#define SPAWN_DIST	100.f 

		if (Magnitude(dis) < SPAWN_DIST)
		{
			bExist = true;
			break;
		}
	}

	return bExist;
}

ZMapSpawnData* ZMapSpawnManager::GetSpawnData(ZMapSpawnType nSpawnType, int nIndex)
{
	int nPositionsCount = GetSpawnCount(nSpawnType);
	if (nPositionsCount <= 0)
	{
		// ���� �������� 1���� ����.
//		_ASSERT(0);
		return NULL;
	}

	if (nIndex < 0) nIndex = 0;
	while (nIndex >= nPositionsCount)
	{
		nIndex -= nPositionsCount;
	}
	if (nIndex < 0) nIndex=0;

	return (m_SpawnArray[nSpawnType][nIndex]);
}

//////////////////////////////////////////////////////////////////////////////
ZMapDesc::ZMapDesc()
{
	m_WaitCamPos = rvector(0.0f, 0.0f, 1000.0f);
	m_WaitCamDir = rvector(0.0f, 0.5f, -0.5f);

	m_SmokeSSDrawBackupTime = 0.f;
	m_SmokeSTDrawBackupTime = 0.f;
	m_SmokeTSDrawBackupTime = 0.f;

	m_bRenderedSS = false;
	m_bRenderedST = false;
	m_bRenderedTS = false;
}

ZMapDesc::~ZMapDesc()
{
	m_SmokeDummyMgr.Destroy();
}

bool ZMapDesc::Open(RBspObject* pBspObject)
{
	m_SpawnManager.Clear();

	const size_t nSpawnTokLen = strlen(ZTOK_SPAWN);
	const size_t nWaitCamTokLen = strlen(ZTOK_WAIT_CAMERA_POS);
	const size_t nSmokeSSTokLen = strlen(ZTOK_SMOKE_SS);
	const size_t nSmokeTSTokLen = strlen(ZTOK_SMOKE_TS);
	const size_t nSmokeSTTokLen = strlen(ZTOK_SMOKE_ST);

	RDummyList* pDummyList = pBspObject->GetDummyList();

	for (RDummyList::iterator itor = pDummyList->begin(); itor != pDummyList->end(); ++itor)
	{
		RDummy* pDummy = (*itor);
		const char* szDummyName = pDummy->szName.c_str();

		if (!strnicmp(pDummy->szName.c_str(), ZTOK_SPAWN, nSpawnTokLen))
		{
			char szSpawnName[256];
			strcpy(szSpawnName, szDummyName);

			m_SpawnManager.AddSpawnData(szSpawnName, pDummy->Position, pDummy->Direction);
		}
		else if (!strnicmp(pDummy->szName.c_str(), ZTOK_WAIT_CAMERA_POS, nWaitCamTokLen))
		{
			m_WaitCamDir = pDummy->Direction;
			m_WaitCamPos = pDummy->Position;
		}
		else if(!strnicmp(pDummy->szName.c_str(), ZTOK_SMOKE_SS , nSmokeSSTokLen ))
		{
			ZMapSmokeSS* smkSS = new ZMapSmokeSS;

			smkSS->m_vPos = pDummy->Position;
			smkSS->m_vDir = pDummy->Direction;
			smkSS->m_Name = pDummy->szName;

			m_SmokeDummyMgr.push_back(smkSS);
		}
		else if(!strnicmp(pDummy->szName.c_str(), ZTOK_SMOKE_TS , nSmokeTSTokLen ))
		{
			ZMapSmokeTS* smkTS = new ZMapSmokeTS;

			smkTS->m_vPos = pDummy->Position;
			smkTS->m_vDir = pDummy->Direction;
			smkTS->m_Name = pDummy->szName;

			m_SmokeDummyMgr.push_back(smkTS);
		}
		else if(!strnicmp(pDummy->szName.c_str(), ZTOK_SMOKE_ST , nSmokeSTTokLen ))
		{
			ZMapSmokeST* smkST = new ZMapSmokeST;

			smkST->m_vPos = pDummy->Position;
			smkST->m_vDir = pDummy->Direction;
			smkST->m_Name = pDummy->szName;

			m_SmokeDummyMgr.push_back(smkST);
		}
		else if(!strnicmp(pDummy->szName.c_str(), ZTOK_DUMMY_LINK , (int)strlen(ZTOK_DUMMY_LINK) ))
		{
			char szName[32];
			strcpy(szName, pDummy->szName.c_str());
			int idx = (int)strlen(ZTOK_DUMMY_LINK);
			int nLinkIndex = atoi(&szName[idx]) - 1;
			
			if ((nLinkIndex >= 0) && (nLinkIndex < MAX_QUEST_MAP_SECTOR_COUNT))
			{
				m_QuestMapDesc.m_vLinks[nLinkIndex] = pDummy->Position;
			}
		}

	}

	m_SmokeSSDrawBackupTime = 0.f;
	m_SmokeSTDrawBackupTime = 0.f;
	m_SmokeTSDrawBackupTime = 0.f;

	m_StartTime = timeGetTime();

	return true;
}

bool ZMapDesc::LoadSmokeDesc(const char* pFileName)
{
	MXmlDocument	Data;
	Data.Create();

	MZFile mzf;

	if( !mzf.Open( pFileName, g_pFileSystem ))
	{
		return false;
	}

	char* buffer;
	buffer	= new char[mzf.GetLength() + 1];
	mzf.Read( buffer, mzf.GetLength() );
	buffer[mzf.GetLength()] = 0;

	if( !Data.LoadFromMemory(buffer) )
	{
		delete buffer;
		return false;
	}

	delete buffer;
	mzf.Close();

	MXmlElement root, child;
	char TagName[256];
	char Attribute[256];
	root = Data.GetDocumentElement();
	int iCount = root.GetChildNodeCount();	

	ZMapSmokeDummy* pMapSmoke = NULL;

	char drive[_MAX_DRIVE],dir[_MAX_DIR],fname[_MAX_FNAME],ext[_MAX_EXT];

	for( int i = 0 ; i < iCount; ++i )
	{
		child		= root.GetChildNode(i);
		child.GetTagName( TagName );
		if( TagName[0] == '#' )
		{
			continue;
		}

		child.GetAttribute( Attribute, "NAME" );

		_splitpath(Attribute,drive,dir,fname,ext);
		
		pMapSmoke = m_SmokeDummyMgr.Get( string( fname ) );

		if( pMapSmoke ) {

			if( child.GetAttribute( Attribute, "DIRECTION" )) {

				if(pMapSmoke->m_SmokeType == ZMapSmokeType_ST) {

//					static rmatrix _mrot = RGetRotY(180) * RGetRotX(90);
//					((ZMapSmokeST*)pMapSmoke)->m_vSteamDir = pMapSmoke->m_vDir * _mrot;
					((ZMapSmokeST*)pMapSmoke)->m_vSteamDir = pMapSmoke->m_vDir;
				}
				
				D3DXMATRIX RotMat;
				rvector dir = rvector( 0,1,0 );
				int theta;
				sscanf( Attribute, "%d", &theta );
				D3DXMatrixRotationAxis( &RotMat, &rvector(0,0,1), ((float)theta*D3DX_PI/180) );
				dir = dir * RotMat;
					
				pMapSmoke->m_vDir = dir;
			}

			if( child.GetAttribute( Attribute, "LIFE" )) {

				float fLife=0.f;
				sscanf( Attribute, "%f", &fLife );
				pMapSmoke->m_fLife = fLife;
			}

			if( child.GetAttribute( Attribute, "TOGGLEMINTIME" )) {

				float fToggleMinTime=0.f;
				sscanf( Attribute, "%f", &fToggleMinTime );
				pMapSmoke->m_fToggleMinTime = fToggleMinTime;
			}

			if( child.GetAttribute( Attribute, "POWER" )) {

				float power=0.f;
				sscanf( Attribute, "%f", &power );
				pMapSmoke->m_fPower = power;
			}

			if(child.GetAttribute( Attribute, "DELAY" ) ) {

				int delay=0; 
				sscanf( Attribute, "%d", &delay );
				pMapSmoke->m_nDelay = delay;
			}

			if(child.GetAttribute( Attribute, "SIZE" ) ) {

				float _size=0;
				sscanf( Attribute, "%f", &_size );
				pMapSmoke->m_fSize = _size;
			}

			if(child.GetAttribute( Attribute, "COLOR" ) ) {

				int r,g,b;
				sscanf( Attribute, "%d,%d,%d", &r,&g,&b );
				pMapSmoke->m_dwColor = D3DCOLOR_ARGB(0,min(r,255),min(g,255),min(b,255));
			}
		}
	}

	return true;
}

float ZMapDesc::GetThisTime()
{
	if(ZGetGame())//�������̸�
		return ZGetGame()->GetTime();

	float _time = (timeGetTime()-m_StartTime)/1000.f;
	return _time;
}

void ZMapDesc::DrawSmokeSS(ZMapSmokeSS* pDummy)
{
	if(!pDummy) return;

	float f = pDummy->m_nDelay / 1000.f;

	float _time = GetThisTime();

	if( _time > pDummy->m_fDrawBackupTime + f)
//	if( _time > pDummy->m_fDrawBackupTime + 0.6f)
	{
		rvector addpos = rvector(0,0,0);

		if( rand()%2 )	addpos.x =  rand()%100;
		else			addpos.x = -rand()%100;

		if( rand()%2 )	addpos.y =  rand()%100;
		else			addpos.y = -rand()%100;

//		float fLife       = 6.f + (rand()%5);
		float fLife		  = pDummy->m_fLife + (rand()%5);
		float fStartScale = pDummy->m_fSize + (rand()%50);
		float fEndScale   = pDummy->m_fSize * 10 + (rand()%300);

		ZGetEffectManager()->AddMapSmokeSSEffect(	
			pDummy->m_vPos + addpos ,				// target
			rvector(0,0,500),						// dir				
			pDummy->m_vDir*pDummy->m_fPower,		// acc
			pDummy->m_dwColor,						// 
			pDummy->m_nDelay,						// ���� �ֱ⿡ ����Ѵ�.
			fLife ,
			fStartScale ,
			fEndScale 
		);

		pDummy->m_fDrawBackupTime = _time;

	}
}

void ZMapDesc::DrawSmokeST(ZMapSmokeST* pDummy)
{
	if(!pDummy) return;

	float f = pDummy->m_nDelay / 1000.f;

	float _time = GetThisTime();

	if( _time > pDummy->m_fToggleSaveTime + pDummy->m_fToggleTime ) {

		pDummy->m_bToggle = !pDummy->m_bToggle;
		pDummy->m_fToggleSaveTime = _time;
		pDummy->m_fToggleTime = pDummy->m_fToggleMinTime + (pDummy->m_fToggleMinTime * (rand()%3));

	}

	if( pDummy->m_fToggleMinTime < 0.01f) // ���� ������ ��� ���..
		pDummy->m_bToggle = false;

	if( pDummy->m_bToggle ) {
		return;
	}

	if( _time > pDummy->m_fDrawBackupTime + f)
	{
		rvector addpos = rvector(0,0,0);
	
		float fLife = pDummy->m_fLife + (rand()%2);
		float fStartScale = pDummy->m_fSize + (rand()%30);
		float fEndScale = pDummy->m_fSize   + (rand()%200);
	
		ZGetEffectManager()->AddMapSmokeSTEffect(	
			pDummy->m_vPos + addpos ,								// target
			pDummy->m_vSteamDir,									// dir
			pDummy->m_vDir * pDummy->m_fPower,						// acc  �ٶ�����..
			pDummy->m_vSteamDir * 200.f,//pDummy->m_fPower,			// acc2 ���̹���..
			pDummy->m_dwColor,										// 
			pDummy->m_nDelay,										// ���� �ֱ⿡ ����Ѵ�.
			fLife ,
			fStartScale ,
			fEndScale 
		);

		pDummy->m_fDrawBackupTime = _time;
	}
}

void ZMapDesc::DrawSmokeTS(ZMapSmokeTS* pDummy)
{
	if(!pDummy) return;

	float f = pDummy->m_nDelay / 1000.f;

	float _time = GetThisTime();

	if( _time > pDummy->m_fDrawBackupTime + f)
//	if( _time > pDummy->m_fDrawBackupTime + 0.25f) 
	{
		rvector addpos = rvector(0,0,0);

		if( rand()%2 )	addpos.x =  rand()%10;
		else			addpos.x = -rand()%10;

		if( rand()%2 )	addpos.y =  rand()%10;
		else			addpos.y = -rand()%10;

		float fLife = 4.f + (rand()%1);
		float fStartScale = pDummy->m_fSize + (rand()%50);
		float fEndScale = pDummy->m_fSize * 20 + (rand()%100);

		ZGetEffectManager()->AddMapSmokeTSEffect(	
			pDummy->m_vPos + addpos ,				// target
			rvector(0,0,500),						// dir				
			pDummy->m_vDir*pDummy->m_fPower,		// acc
			pDummy->m_dwColor,						// 
			pDummy->m_nDelay,						// ���� �ֱ⿡ ����Ѵ�.
			fLife ,
			fStartScale ,
			fEndScale 
		);
		pDummy->m_fDrawBackupTime = _time;
	}
}


void ZMapDesc::DrawSmoke()
{
	int cnt = (int)m_SmokeDummyMgr.size();

	ZMapSmokeDummy* pSmokeDummy = NULL;

	float _time = GetThisTime();

	for(int i=0;i<cnt;i++) {

		pSmokeDummy = m_SmokeDummyMgr[i];

			 if( pSmokeDummy->m_SmokeType == ZMapSmokeType_SS )	DrawSmokeSS((ZMapSmokeSS*)pSmokeDummy);
		else if( pSmokeDummy->m_SmokeType == ZMapSmokeType_TS ) DrawSmokeTS((ZMapSmokeTS*)pSmokeDummy);
		else if( pSmokeDummy->m_SmokeType == ZMapSmokeType_ST ) DrawSmokeST((ZMapSmokeST*)pSmokeDummy);

	}
}

void ZMapDesc::DrawMapDesc()
{
	DrawSmoke();
}



rvector ZMapDesc::GetQuestSectorLink(int nIndex)
{
	if ((nIndex < 0) || (nIndex >= MAX_QUEST_MAP_SECTOR_COUNT)) 
	{
		_ASSERT(0);
		return rvector(0,0,0);
	}

	return m_QuestMapDesc.m_vLinks[nIndex];
}
