#pragma once
#include "RenderNode.h"

class CLightNode:public CRenderNode
{
public:
	CLightNode();
	~CLightNode();
public:
	virtual int			getType			() {return NODE_SKELETON;}
	virtual void		frameMove		(const Matrix& mWorld, double fTime, float fElapsedTime);
	virtual void		render			(const Matrix& mWorld, int nRenderType)const;
protected:
};