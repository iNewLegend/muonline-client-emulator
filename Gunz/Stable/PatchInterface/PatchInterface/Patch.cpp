// Patch.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "PatchInterface.h"
#include "Patch.h"
#include ".\patch.h"
#include "KeeperManager.h"


// CPatch ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(CPatch, CDialog)
CPatch::CPatch(CWnd* pParent /*=NULL*/)
	: CDialog(CPatch::IDD, pParent)
{
}

CPatch::~CPatch()
{
}

void CPatch::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CPatch, CDialog)
	ON_BN_CLICKED(ID_DOWNLOAD_SERVER_PATCH, OnBnClickedDownloadServerPatch)
	ON_BN_CLICKED(ID_PREPARE_SERVER_PATCH, OnBnClickedPrepareServerPatch)
	ON_BN_CLICKED(ID_PATCH_SERVER, OnBnClickedPatchServer)
	ON_BN_CLICKED(ID_RESET_SERVER_PATCH, OnBnClickedResetServerPatch)
	ON_BN_CLICKED(ID_DOWNLOAD_AGENT_PATCH, OnBnClickedDownloadAgentPatch)
	ON_BN_CLICKED(ID_PREPARE_AGENT_PATCH, OnBnClickedPrepareAgentPatch)
	ON_BN_CLICKED(ID_PATCH_AGENT, OnBnClickedPatchAgent)
	ON_BN_CLICKED(ID_RESET_AGENT_PATCH, OnBnClickedResetAgentPatch)
END_MESSAGE_MAP()


// CPatch �޽��� ó�����Դϴ�.

void CPatch::OnBnClickedDownloadServerPatch()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.

	GetKeeperMgr.RequestDownloadServerPatchFile();
}

void CPatch::OnBnClickedPrepareServerPatch()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.

	GetKeeperMgr.RequestPrepareServerPatch();
}

void CPatch::OnBnClickedPatchServer()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.

	GetKeeperMgr.RequestServerPatch();
}

void CPatch::OnBnClickedResetServerPatch()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.

	GetKeeperMgr.RequestResetPatch();
}

void CPatch::OnBnClickedDownloadAgentPatch()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.

	GetKeeperMgr.RequestDownloadAgentPatchFile();
}

void CPatch::OnBnClickedPrepareAgentPatch()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.

	GetKeeperMgr.RequestPrepareAgentPatch();
}

void CPatch::OnBnClickedPatchAgent()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.

	GetKeeperMgr.RequestAgentPatch();
}

void CPatch::OnBnClickedResetAgentPatch()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.

	GetKeeperMgr.RequestResetPatch();
}
