#ifndef _ZMAP_H
#define _ZMAP_H

#include "ZFilePath.h"
// �ʰ� ���õ� �͵��� �� ���� �����ô�.


class MWidget;

// ���� �� ������ ��ȯ�Ѵ�. - ����Ʈ ���� �н��� �ٸ��� ������ �����д�.
void ZGetCurrMapPath(char* outPath);

// �ش�����(�޺��ڽ�)�� ���Ӱ����� ���� �߰��Ѵ�.
bool InitMaps(MWidget *pWidget);


#endif