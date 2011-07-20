

//void Octree::setupNodeBBox()
//{
//	if (pChild)
//	{
//		if (pChild[0].m_setObjet.size()>0)
//		{
//			pChild[0].setupNodeBBox();
//			bboxNode=pChild[0].bboxNode;
//			if (pChild[1].m_setObjet.size()>0)
//			{
//				pChild[1].setupNodeBBox();
//				bboxNode+=pChild[1].bboxNode;
//			}
//		}
//		else if (pChild[1].m_setObjet.size()>0)
//		{
//			pChild[1].setupNodeBBox();
//			bboxNode=pChild[1].bboxNode;
//		}
//	}
//	else
//	{
//		if (m_setObjet.size()>0)
//		{
//			bboxNode=m_setObjet[0]->GetBBox();
//		}
//		for (auto it=m_setObjet.begin();it!=m_setObjet.end();it++)
//		{
//			BBox box = (*it)->GetBBox();
//			bboxNode+=box;
//		}
//	}
//}

// void Octree::getNodesByBBox(const BBox& box,const std::vector<iRenderNode*>& setSrcNode, std::vector<iRenderNode*>& setDestNode)
// {
// 	FOR_IN(it, setSrcNode)
// 	{
// 		if (box.crossVertex((*it)->getPos()))
// 		{
// 			setDestNode.push_back(*it);
// 		}
// 	}
// }

//void Octree::getOctreesByFrustum(const CFrustum& frustum, std::vector<Octree*>& setOctree)
//{
//	if (0==m_setObjet.size())
//	{
//		return;
//	}
//	CrossRet crossRet = frustum.CheckAABBVisible(bboxNode);
//	if (cross_include == crossRet)
//	{
//		setOctree.push_back(this);
//	}
//	else if (cross_cross == crossRet)
//	{
//		if (pChild)
//		{
//			for (size_t i=0;i<2;++i)
//			{
//				pChild[i].getOctreesByFrustum(frustum, setOctree);
//			}
//		}
//		else
//		{
//			setOctree.push_back(this);
//		}
//	}
//}

// Octree* Octree::getNodeByAABB(const BBox& box)
// {
// 	CrossRet crossRet = bbox.checkAABBVisible(box);
// 	if (cross_include == crossRet)
// 	{
// 		if (pChild)
// 		{
// 			for (size_t i=0;i<8;++i)
// 			{
// 				Octree* pNode = pChild[i].getNodeByAABB(box);
// 				if (pNode)
// 				{
// 					return pNode;
// 				}
// 			}
// 		}
// 		return this;
// 	}
// 	return NULL;
// }