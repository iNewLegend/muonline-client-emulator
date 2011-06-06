#pragma once

#include <iostream>
#include <cmath>
#include "Template4D.h"

template <typename _T> inline const _T& TClamp(const _T& _X, const _T& _Limit1, const _T& _Limit2)
{
	if(_Limit1<_Limit2)
		return (_X<=_Limit1) ? _Limit1 : ((_X>=_Limit2) ? _Limit2 : _X);
	else
		return (_X<=_Limit2) ? _Limit2 : ((_X>=_Limit1) ? _Limit1 : _X);
}

class Color32:public Template4D<unsigned char>
{
public:
	Color32(){}
	Color32(unsigned long color) {c=color;}
	Color32(unsigned char a0, unsigned char r0, unsigned char g0, unsigned char b0)
	{
		a=a0;r=r0;g=g0;b=b0;
	}
	Color32(const Template4D<unsigned char>& v):Template4D<unsigned char>(v){}
	Color32(int nHue, int nLight, int nSaturation);
	Color32(float fHue, float fLight, float fSaturation);
	static const Color32 modulate(const Color32 &v1, const Color32 &v2);
	static const Color32 modulateX2(const Color32 &v1, const Color32 &v2);
	static const Color32 modulateX4(const Color32 &v1, const Color32 &v2);
	Color32& operator= (const unsigned long color);
	bool operator== (const Color32& clr) const;
	Color32& operator= (const std::string& str);
};