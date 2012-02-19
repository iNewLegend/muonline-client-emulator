#pragma once
#include "RenderNode.h"

class CLightNode:public CRenderNode
{
public:
	CLightNode();
	~CLightNode();
public:
	virtual int			getType			() {return NODE_SKELETON;}
	virtual void		render			(int nRenderType)const;
protected:
};