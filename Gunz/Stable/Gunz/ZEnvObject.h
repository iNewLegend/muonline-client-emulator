#ifndef _ZENVOBJECT_H
#define _ZENVOBJECT_H

#include "MRTTI.h"
#include <list>
#include <string>
#include "ZObject.h"
#include "ZActor.h"

using namespace std;

_USING_NAMESPACE_REALSPACE2


// �μ����� �ڽ����� ȯ�� ������Ʈ ���� Ŭ����
class ZEnvObject : public ZActor
{
	MDeclareRTTI;
private:
protected:

public:
	ZEnvObject() : ZActor() {  }
	virtual ~ZEnvObject() { }
};





#endif