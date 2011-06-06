#pragma once

#include <cassert>
#include <utility>
#include <vector>

#include "interpolation.h"

enum Interpolations {
	INTERPOLATION_NONE,
	INTERPOLATION_LINEAR,
	INTERPOLATION_HERMITE
};

// block B - animations
struct ModelAnimation
{
	unsigned long animID;
	unsigned long timeStart;
	unsigned long timeEnd;

	float moveSpeed;

	unsigned long loopType;
	unsigned long flags;
	unsigned long d1;
	unsigned long d2;
	unsigned long playSpeed;  // note: this can't be play speed because it's 0 for some models

	Vec3D boxA, boxB;
	float rad;

	short s[2];
};

/*
	Generic animated value class:
	T is the data type to animate
*/
template <class T >
class Animated
{
public:
	Animated():type(INTERPOLATION_LINEAR)
	{
	}
	int type;

	// keyframes
	std::vector<unsigned long> m_KeyTimes;
	std::vector<T> m_KeyData;

	bool isUsed()const
	{
		return m_KeyData.size()!=0;
	}

	void addValue(unsigned int time,T data)
	{
		m_KeyTimes.push_back(time);
		m_KeyData.push_back(data);
	}

	T getValue(unsigned int time)const
	{
		if (/*type != INTERPOLATION_NONE || */m_KeyData.size()>1)
		{
			// obtain a time value and a data range
			time %= m_KeyTimes.back(); // I think this might not be necessary?

 			if (m_KeyData.size()>2)
			{
				size_t t1, t2;
				size_t pos=0;
				size_t keyEnd=m_KeyTimes.size()-1;
				for (size_t i=0; i<keyEnd; i++)
				{
					if (time >= m_KeyTimes[i] && time < m_KeyTimes[i+1])
					{
						pos = i;
						break;
					}
				}
				t1 = m_KeyTimes[pos];
				t2 = m_KeyTimes[pos+1];
				float r = (time-t1)/(float)(t2-t1);

				if (type == INTERPOLATION_LINEAR) 
					return interpolate<T>(r,m_KeyData[pos],m_KeyData[pos+1]);
				else if (type == INTERPOLATION_NONE) 
					return m_KeyData[pos];
				else
				{
					// INTERPOLATION_HERMITE is only used in cameras afaik?
					// for nonlinear interpolations:
					return interpolateHermite<T>(r,m_KeyData[pos*3],m_KeyData[pos*3+3],
						m_KeyData[pos*3+1],m_KeyData[pos*3+2]);
				}
			}
			else
			{
				return m_KeyData[0];
			}
		}
		else
		{
			// default value
			if (m_KeyData.size() == 0)
				return T();
			else
				return m_KeyData[0];
		}
	}

	void fix(T fixfunc(const T))
	{
		for (size_t i=0; i<m_KeyData.size(); i++)
		{
			m_KeyData[i] = fixfunc(m_KeyData[i]);
		}
	}
};
