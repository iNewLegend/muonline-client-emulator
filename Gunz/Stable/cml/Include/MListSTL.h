// STL Support

#ifndef MLISTSTL_H
#define MLISTSTL_H

#include <list>
using namespace std;

// list<A*> �� ���� ���·� ���� Ŭ����
// �Ҹ��ڿ��� �ڵ����� ��ü�� delete���ش�.
template<class _Ty, class _A = allocator<_Ty> >
class MAutoDelPtrList : public list<_Ty, _A>{
public:
	void Erase(iterator _P){
		delete (*_P);
		erase(_P);
	}
	void Clear(void){
		while(begin()!=end()){
			delete *begin();	// ���������̿��� �Ѵ�. �׷��� �ʴٸ� �����Ϸ� ������ ����.
			erase(begin());
		}
	}	
	virtual ~MAutoDelPtrList(void){
		Clear();
	}
};

#define FOR_BEGIN(_ClassName, _VarName)	{ for(_ClassName::iterator _i=_VarName.begin(); _i!=_VarName.end(); _i++){
#define FOR_EACH_ITOR()					(_i)
#define FOR_EACH_VALUE()				(*_i)
#define FOR_END()						}}

#endif
