#pragma once

typedef void(ZCOUNTDOWNEVENTCALLBACK)(void);

// ī��Ʈ�ٿ� �ϴٰ� 0�� �Ǹ� �ݹ��� �����Ѵ�
struct ZCOUNTDOWN {
	int nSeconds;				// �����ð�
	const char *szLabelWidget;	// ���ڸ� �������� ����
	const char *szTargetWidget;	// �ð��� �ٵǸ� �� ������ �����. �� �� ������ �������� ī��Ʈ�� �����Ѵ�.
	ZCOUNTDOWNEVENTCALLBACK *pCallBack;	// 0�� �Ǿ����� ������ ���
};

void OnTimer_CountDown(void* pParam);
