#include "stdafx.h"
#include "MCommandManager.h"
#include "CMLexicalAnalyzer.h"
#include <algorithm>
#include "MStrEx.h"
#include <Windows.h>	// for test

void MCommandManager::InitializeCommandDesc(void)
{
	for(MCommandDescMap::iterator i=m_CommandDescs.begin(); i!=m_CommandDescs.end(); i++){
		delete (*i).second;
	}
	m_CommandDescs.clear();
}

MCommandManager::MCommandManager(void)
{
	InitializeCommandMemPool();
	InitializeCommandDesc();
}

MCommandManager::~MCommandManager(void)
{
	InitializeCommandDesc();
	while(PeekCommand()) {
		delete GetCommand();
	}

	FinalizeCommandMemPool();
}

void MCommandManager::Initialize(void)
{
	for(MCommandList::iterator i=m_CommandQueue.begin(); i!=m_CommandQueue.end(); i++){
		delete (*i);
	}
	m_CommandQueue.clear();
}

int MCommandManager::GetCommandDescCount(void) const
{
	return (int)m_CommandDescs.size();
}

int MCommandManager::GetCommandQueueCount(void) const
{
	return (int)m_CommandQueue.size();
}


MCommandDesc* MCommandManager::GetCommandDesc(int i)
{
	if(i<0 || i>=(int)m_CommandDescs.size()) return NULL;

	MCommandDescMap::iterator itor = m_CommandDescs.begin();

	for (int t=0; t < i; t++)
	{
		itor++;
		if (itor == m_CommandDescs.end()) return NULL;
	}

	return (*itor).second;
}


void MCommandManager::AssignDescs(MCommandManager* pTarCM)
{
	for(MCommandDescMap::iterator i=m_CommandDescs.begin(); i!=m_CommandDescs.end(); i++)
	{
		MCommandDesc* pDesc = (*i).second;
		pTarCM->AddCommandDesc(pDesc->Clone());
	}
}

MCommandDesc* MCommandManager::GetCommandDescByID(int nID)
{
	MCommandDescMap::iterator itor = m_CommandDescs.find(nID);
	if (itor != m_CommandDescs.end())
	{
		return (*itor).second;
	}
	
	return NULL;
}

void MCommandManager::AddCommandDesc(MCommandDesc* pCD)
{
	_ASSERT(m_CommandDescs.find(pCD->GetID())==m_CommandDescs.end());	// Ŀ�ǵ�� �ߺ��Ǹ� �ȵȴ�
	m_CommandDescs.insert(MCommandDescMap::value_type(pCD->GetID(), pCD));
}

bool MCommandManager::Post(MCommand* pCmd)
{
	bool bCheckRule = pCmd->CheckRule();
	_ASSERT(bCheckRule==true);
	if(bCheckRule==false) return false;

	m_CommandQueue.push_back(pCmd);


	return true;
}

MCommand* MCommandManager::GetCommand(void)
{
	if(m_CommandQueue.size()==0) return NULL;

	MCommand* pCmd = *m_CommandQueue.begin();
	
	m_CommandQueue.erase(m_CommandQueue.begin());

	return pCmd;
}

MCommand* MCommandManager::PeekCommand(void)
{
	if(m_CommandQueue.size()==0) return NULL;

	MCommand* pCmd = *m_CommandQueue.begin();
	return pCmd;
}

void MCommandManager::GetSyntax(char* szSyntax, const MCommandDesc* pCD)
{
	sprintf(szSyntax, "%s ", pCD->GetName());
	for(int i=0; i<pCD->GetParameterDescCount(); i++){
		MCommandParameterDesc* pPD = pCD->GetParameterDesc(i);
		sprintf(szSyntax, "%s %s", szSyntax, pPD->GetDescription());
	}
}

bool MCommandManager::ParseMessage(MCommand* pCmd, char* szErrMsg, int nErrMsgMaxLength, const char* szMsg)
{
//#define USE_SLASH
#ifdef USE_SLASH
	if(!(szMsg[0]=='/' && szMsg[1]!=0)){
		MStrNCpy(szErrMsg, nErrMsgMaxLength, "Use Slash('/') First");
		return false;
	}
#endif

	CMLexicalAnalyzer la;
#ifdef USE_SLASH
	la.Create(szMsg+1);
#else
	la.Create(szMsg);
#endif
	
	if(la.GetCount()==0){
		MStrNCpy(szErrMsg, nErrMsgMaxLength, "Syntax Error");
		return false;
	}

#define ASMESSAGE_LENGTH	256
	char szTemp[ASMESSAGE_LENGTH];
	strcpy(szTemp, la.GetByStr(0));

	MCommandAliasMap::iterator itor = m_CommandAlias.find(string(szTemp));
	if (itor != m_CommandAlias.end())
	{
		strcpy(szTemp, (*itor).second.c_str());
	}

	//for(int i=0; i<(int)m_CommandDescs.size(); i++){
	for (MCommandDescMap::iterator itor = m_CommandDescs.begin(); itor != m_CommandDescs.end(); ++itor)
	{
		MCommandDesc* pCD = (*itor).second;
		//MCommandDesc* pCD = m_CommandDescs[i];

		if(stricmp(szTemp, pCD->GetName())==0){
			//if(pCD->IsFlag(ASCDF_CHEAT)==true && EnableDevDebug()==false) return false;	// ������ ���� Ŀ�ǵ��̸�... Debug�� Enable�Ǿ� �־�� �Ѵ�.

			pCmd->SetID(pCD);

			int nLAMaxCount = la.GetCount();
			int nLACount = 1;

			for(int j=0; j<pCD->GetParameterDescCount(); j++){
				MCommandParameterDesc* pPD = pCD->GetParameterDesc(j);

				bool bSyntaxError = false;
				MCommandParameter* pParam = NULL;
				switch(pPD->GetType()){
				case MPT_INT:
					if(nLACount+1>nLAMaxCount){
						bSyntaxError = true;
						break;
					}
					pParam = new MCommandParameterInt(la.GetByInt(nLACount));
					nLACount++;
					break;
				case MPT_UINT:
					{
						if(nLACount+1>nLAMaxCount){
							bSyntaxError = true;
							break;
						}
						pParam = new MCommandParameterUInt(la.GetByLong(nLACount));
						nLACount++;
						break;
					}
					break;
				case MPT_FLOAT:
					if(nLACount+1>nLAMaxCount){
						bSyntaxError = true;
						break;
					}
					pParam = new MCommandParameterFloat(la.GetByFloat(nLACount));
					nLACount++;
					break;
				case MPT_STR:
					if(nLACount+1>nLAMaxCount){
						bSyntaxError = true;
						break;
					}
					pParam = new MCommandParameterString(la.GetByStr(nLACount));
					nLACount++;
					break;
				case MPT_VECTOR:
					if(nLACount+3>nLAMaxCount){
						bSyntaxError = true;
						break;
					}
					pParam = new MCommandParameterVector(la.GetByFloat(nLACount), la.GetByFloat(nLACount+1), la.GetByFloat(nLACount+2));
					nLACount+=3;
					break;
				case MPT_POS:
					if(nLACount+3>nLAMaxCount){
						bSyntaxError = true;
						break;
					}
					pParam = new MCommandParameterPos(la.GetByFloat(nLACount), la.GetByFloat(nLACount+1), la.GetByFloat(nLACount+2));
					nLACount+=3;
					break;
				case MPT_DIR:
					if(nLACount+3>nLAMaxCount){
						bSyntaxError = true;
						break;
					}
					pParam = new MCommandParameterDir(la.GetByFloat(nLACount), la.GetByFloat(nLACount+1), la.GetByFloat(nLACount+2));
					nLACount+=3;
					break;
				case MPT_COLOR:
					if(nLACount+3>nLAMaxCount){
						bSyntaxError = true;
						break;
					}
					pParam = new MCommandParameterColor(la.GetByFloat(nLACount), la.GetByFloat(nLACount+1), la.GetByFloat(nLACount+2));
					nLACount+=3;
					break;
				case MPT_BOOL:
					if(nLACount+1>nLAMaxCount){
						bSyntaxError = true;
						break;
					}
					pParam = new MCommandParameterBool(la.GetByInt(nLACount)>0?true:false);
					nLACount++;
					break;
					
				case MPT_UID:
					if(nLACount+2>nLAMaxCount){
						bSyntaxError = true;
						break;
					}
					// UINT�� ������ �Ѵ�.
					pParam = new MCommandParameterUID(MUID(la.GetByInt(nLACount), la.GetByInt(nLACount+1)));
					nLACount+=2;
					break;
					
				default:
					_ASSERT(false);		// ���� �ڵ鸵�� �ڵ尡 �غ� �ȵ� �Ķ����
					return false;
					//break;
				}

				if(bSyntaxError==true){
					char szSyntax[256];
					static char temp[512];
					GetSyntax(szSyntax, pCmd->m_pCommandDesc);
					sprintf(temp, "Sytax Error: [Syntax] %s", szSyntax);
					MStrNCpy(szErrMsg, nErrMsgMaxLength, temp);
					return false;
				}

				if(pParam!=NULL) pCmd->AddParameter(pParam);
			}

			return true;
		}
	}

	MStrNCpy(szErrMsg, nErrMsgMaxLength, "Unknown Command");

	return false;
}

void MCommandManager::AddAlias(string szName, string szText)
{
	m_CommandAlias.insert(MCommandAliasMap::value_type(szName, szText));
}

void MCommandManager::InitializeCommandMemPool()
{

	InitMemPool(MCommand);
	InitMemPool(MCommandParameterInt);
	InitMemPool(MCommandParameterUInt);
	InitMemPool(MCommandParameterFloat);
	InitMemPool(MCommandParameterPos);
	InitMemPool(MCommandParameterDir);
	InitMemPool(MCommandParameterColor);
	InitMemPool(MCommandParameterBool);
	InitMemPool(MCommandParameterUID);
	InitMemPool(MCommandParameterChar);
	InitMemPool(MCommandParameterUChar);
	InitMemPool(MCommandParameterShort);
	InitMemPool(MCommandParameterUShort);
	InitMemPool(MCommandParameterInt64);
	InitMemPool(MCommandParameterUInt64);
	InitMemPool(MCommandParameterShortVector);

}

void MCommandManager::FinalizeCommandMemPool()
{

	ReleaseMemPool(MCommandParameterInt);
	ReleaseMemPool(MCommandParameterUInt);
	ReleaseMemPool(MCommandParameterFloat);
	ReleaseMemPool(MCommandParameterPos);
	ReleaseMemPool(MCommandParameterDir);
	ReleaseMemPool(MCommandParameterColor);
	ReleaseMemPool(MCommandParameterBool);
	ReleaseMemPool(MCommandParameterUID);
	ReleaseMemPool(MCommandParameterChar);
	ReleaseMemPool(MCommandParameterUChar);
	ReleaseMemPool(MCommandParameterShort);
	ReleaseMemPool(MCommandParameterUShort);
	ReleaseMemPool(MCommandParameterInt64);
	ReleaseMemPool(MCommandParameterUInt64);
	ReleaseMemPool(MCommandParameterShortVector);
	ReleaseMemPool(MCommand);


	UninitMemPool(MCommandParameterInt);
	UninitMemPool(MCommandParameterUInt);
	UninitMemPool(MCommandParameterFloat);
	UninitMemPool(MCommandParameterPos);
	UninitMemPool(MCommandParameterDir);
	UninitMemPool(MCommandParameterColor);
	UninitMemPool(MCommandParameterBool);
	UninitMemPool(MCommandParameterUID);
	UninitMemPool(MCommandParameterChar);
	UninitMemPool(MCommandParameterUChar);
	UninitMemPool(MCommandParameterShort);
	UninitMemPool(MCommandParameterUShort);
	UninitMemPool(MCommandParameterInt64);
	UninitMemPool(MCommandParameterUInt64);
	UninitMemPool(MCommandParameterShortVector);
	UninitMemPool(MCommand);

}
