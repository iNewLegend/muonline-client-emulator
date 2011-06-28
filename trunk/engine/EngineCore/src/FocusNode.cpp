#include "FocusNode.h"

Vec3D CFocusNode::getCenterPos()
{
	Vec3D vPos(0.0f,0.0f,0.0f);
	FOR_IN(it,m_mapChildObj)
	{
		vPos+=(*it)->getPos();
	}
	vPos/=(float)m_mapChildObj.size();
	return vPos;
}

void CFocusNode::setCenterPos(const Vec3D& vPos)
{
	Vec3D vMidPos = getCenterPos();
	FOR_IN(it,m_mapChildObj)
	{
		Vec3D vObjectPos = (*it)->getPos();
		vObjectPos+=vPos-vMidPos;
		(*it)->setPos(vObjectPos);
		//updateMapObj((*it));
	}
}

Vec3D CFocusNode::getCenterRotate()
{
	if(m_mapChildObj.size()==1)
	{
		return (*m_mapChildObj.begin())->getRotate();
	}
	return Vec3D(0.0f,0.0f,0.0f);
}

void CFocusNode::setCenterRotate(const Vec3D& vRotate)
{
	if(m_mapChildObj.size()==1)
	{
		(*m_mapChildObj.begin())->setRotate(vRotate);
		//updateMapObj(m_mapChildObj[0]);
	}
	else
	{
		Vec3D vMidPos = getCenterPos();
		Matrix mRotate;
		mRotate.rotate(vRotate);
		FOR_IN(it,m_mapChildObj)
		{
			{
				Vec3D vObjectPos = (*it)->getPos();
				vObjectPos = vMidPos+mRotate*(vObjectPos-vMidPos);
				(*it)->setPos(vObjectPos);
			}
			{
				Vec3D vObjectRotate = (*it)->getRotate();
				vObjectRotate += vRotate;
				(*it)->setRotate(vObjectRotate);
			}
			//updateMapObj(m_mapChildObj[i]);
		}
	}
}

Vec3D CFocusNode::getCenterScale()
{
	Vec3D vScale(0.0f,0.0f,0.0f);
	FOR_IN(it,m_mapChildObj)
	{
		vScale+=(*it)->getScale();
	}
	vScale/=(float)m_mapChildObj.size();
	return vScale;
}

void CFocusNode::setCenterScale(const Vec3D& vScale)
{
	Vec3D vMidPos = getCenterPos();
	Vec3D vMidScale = getCenterScale();
	FOR_IN(it,m_mapChildObj)
	{
		{
			Vec3D vObjectPos = (*it)->getPos();
			vObjectPos = vMidPos-(vMidPos-vObjectPos)*vScale/vMidScale;
			(*it)->setPos(vObjectPos);
		}
		{
			Vec3D vObjectScale = (*it)->getScale();
			vObjectScale*=vScale/vMidScale;
			(*it)->setScale(vObjectScale);
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