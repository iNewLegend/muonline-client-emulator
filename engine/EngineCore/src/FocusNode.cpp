#include "FocusNode.h"

Vec3D CFocusNode::getCenterPos()
{
	Vec3D vPos(0.0f,0.0f,0.0f);
	FOR_IN(it,m_mapChildNode)
	{
		vPos+=(*it)->getPos();
	}
	vPos/=(float)m_mapChildNode.size();
	return vPos;
}

void CFocusNode::setCenterPos(const Vec3D& vPos)
{
	Vec3D vMidPos = getCenterPos();
	FOR_IN(it,m_mapChildNode)
	{
		Vec3D vNodePos = (*it)->getPos();
		vNodePos+=vPos-vMidPos;
		(*it)->setPos(vNodePos);
		//updateMapObj((*it));
	}
}

Vec3D CFocusNode::getCenterRotate()
{
	if(m_mapChildNode.size()==1)
	{
		return (*m_mapChildNode.begin())->getRotate();
	}
	return Vec3D(0.0f,0.0f,0.0f);
}

void CFocusNode::setCenterRotate(const Vec3D& vRotate)
{
	if(m_mapChildNode.size()==1)
	{
		(*m_mapChildNode.begin())->setRotate(vRotate);
		//updateMapObj(m_mapChildObj[0]);
	}
	else
	{
		Vec3D vMidPos = getCenterPos();
		Matrix mRotate;
		mRotate.rotate(vRotate);
		FOR_IN(it,m_mapChildNode)
		{
			{
				Vec3D vNodePos = (*it)->getPos();
				vNodePos = vMidPos+mRotate*(vNodePos-vMidPos);
				(*it)->setPos(vNodePos);
			}
			{
				Vec3D vNodeRotate = (*it)->getRotate();
				vNodeRotate += vRotate;
				(*it)->setRotate(vNodeRotate);
			}
			//updateMapObj(m_mapChildObj[i]);
		}
	}
}

Vec3D CFocusNode::getCenterScale()
{
	Vec3D vScale(0.0f,0.0f,0.0f);
	FOR_IN(it,m_mapChildNode)
	{
		vScale+=(*it)->getScale();
	}
	vScale/=(float)m_mapChildNode.size();
	return vScale;
}

void CFocusNode::setCenterScale(const Vec3D& vScale)
{
	Vec3D vMidPos = getCenterPos();
	Vec3D vMidScale = getCenterScale();
	FOR_IN(it,m_mapChildNode)
	{
		{
			Vec3D vNodePos = (*it)->getPos();
			vNodePos = vMidPos-(vMidPos-vNodePos)*vScale/vMidScale;
			(*it)->setPos(vNodePos);
		}
		{
			Vec3D vNodeScale = (*it)->getScale();
			vNodeScale*=vScale/vMidScale;
			(*it)->setScale(vNodeScale);
		}
		//updateMapObj((*it));
	}
}

void CFocusNode::transformCenter(const Vec3D& vPos)
{
	Vec3D vNewPos=getCenterPos();
	vNewPos+=vNewPos;
	setCenterPos(vNewPos);
}

void CFocusNode::rotateCenter(const Vec3D& vRotate)
{
	Vec3D vNewRotate=getCenterRotate();
	vNewRotate+=vRotate;
	setCenterRotate(vNewRotate);
}

void CFocusNode::scaleCenter(const Vec3D& vScale)
{

}