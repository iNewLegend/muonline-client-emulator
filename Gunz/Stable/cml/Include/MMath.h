#ifndef _MMATH_H
#define _MMATH_H

#include <math.h>
#include <crtdbg.h>

#define PI 3.141592f


/// min���� max������ ������ ���Ѵ�. (nMin, nMax�� ���� ���Եȴ�.)
inline int RandomNumber(int nMin, int nMax)
{
	// ������ ũ�Ⱑ 131,072�̻��̸� �����÷ο찡 �߻��Ѵ�.
	_ASSERT((nMax-nMin) < 131072);
	return (rand() * (nMax - nMin + 1) / (RAND_MAX+1) + nMin );
}

/// min���� max������ ������ ���Ѵ�.
inline float RandomNumber(float fMin, float fMax)
{
	float fRandom = (float)rand() / (float)RAND_MAX;
	return(fRandom*(fMax-fMin)+fMin);
}

/// d&d�� �ֻ����� ������ ���Ѵ�. - sides��ü �ֻ��� n���� ������ mod�� ���ؼ� ������ �� ��ȯ
inline long Dice(unsigned char n, unsigned char sides, short mod)
{
    int result = mod;
    for(int i = 0; i < n; i++)
    {
		result += RandomNumber(1, sides);
    }
    return result;
}

inline float Roundf(float x)
{
	return floorf(x + .5f);
}

#define ToRadian( degree ) ((degree) * (PI / 180.0f))
#define ToDegree( radian ) ((radian) * (180.0f / PI))


#endif