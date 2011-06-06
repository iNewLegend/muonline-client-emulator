#pragma once
#include "Template4D.h"

template<typename T>
class CRect:public Template4D<T>
{
public:
	CRect(){}
	CRect(const Template4D<T>& r):Template4D<T>(r){}
	CRect(T left,T top,T right, T bottom)
	{
		set(left,top,right,bottom);
	}
	CRect(const RECT& rc){set((T)rc.left,(T)rc.top,(T)rc.right,(T)rc.bottom);}

	T getWidth()const
	{
		return right-left;
	}

	T getHeight()const
	{
		return bottom-top;
	}

	bool ptInRect(const POINT& pt)
	{
		if (left>pt.x||right<pt.x||top>pt.y||bottom<pt.y)
		{
			return false;
		}
		return true;
	}

	void InflateRect(T x, T y)
	{
		left -= x;
		right += x;
		top -= y;
		bottom += y;
	}

	RECT getRECT()const
	{
		RECT rc;
		rc.left		= (LONG)left;
		rc.top		= (LONG)top;
		rc.right	= (LONG)right;
		rc.bottom	= (LONG)bottom;
		return rc;
	}
	void offset(T x, T y)
	{
		left += x;
		top += y;
		right += x;
		bottom += y;
	}

	void IntersectRect(const CRect<T>& src1, const CRect<T>& src2)
	{
		left = src1.left>src2.left?src1.left:src2.left;
		right = src1.right<src2.right?src1.right:src2.right;
		top = src1.top>src2.top?src1.top:src2.top;
		bottom = src1.bottom<src2.bottom?src1.bottom:src2.bottom;
	}

	T operator [](int index) const
	{
		return (&left)[index];
	}

	T& operator [](int index) 
	{
		return (&left)[index];
	}
};