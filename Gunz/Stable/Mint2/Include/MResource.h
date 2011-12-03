#ifndef MRESOURCE_H
#define MRESOURCE_H

#pragma warning(disable: 4786)

#include <map>
#include <string>
#include <list>

using namespace std;

#include "MTypes.h"

// ���� ����
struct MWIDGETINFO{
	char	szWidgetClass[256];
	char	szWidgetName[256];
	int		nResourceID;
};

// ���� ���ҽ�
struct MWIDGETRESOURCE{
	MRECT				Bounds;		// ���
	bool				bBounds;	// ��谡 �ٲ���°�?

	class MWIDGETINFOLIST : public list<MWIDGETINFO*>{
	public:
		virtual ~MWIDGETINFOLIST(void){
			// Delete Automatically
			while(empty()==false){
				delete (*begin());
				erase(begin());
			}
		}
	} Children;	// �ڽ� ���� ����Ʈ
};

class MResourceMap : public map<string, MWIDGETRESOURCE*>{
public:
	virtual ~MResourceMap(void){
		// Delete Automatically
		while(empty()==false){
			delete (*begin()).second;
			erase(begin());
		}
	}
};


#endif