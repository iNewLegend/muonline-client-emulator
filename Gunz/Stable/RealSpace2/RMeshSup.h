#pragma once

struct TriangleConnectivityInfo
{
	int NeighborTriIndex[3];	// �̿� triangle index
	TriangleConnectivityInfo()
	{
		NeighborTriIndex[0] = -1;
		NeighborTriIndex[1] = -1;
		NeighborTriIndex[2] = -1;
	}
};

// PreProcessing Please...
TriangleConnectivityInfo* MakeTriangleInfoList( RMeshNode* pMeshNode )
{
	int pv1, nv1, pv2, nv2;
	
	// ��� ���̽��� �ﰢ������ �����Ѵ�.. �ƴ� ����
	int n = pMeshNode->m_face_num;
	
	TriangleConnectivityInfo* pTriConnectInfoList	= new TriangleConnectivityInfo[n];

	for( int i = 0; i < n; ++i )
	{
		for( int j = 0; j < n; ++j )
		{
			if( i == j ) continue;			// ���� �ﰢ�� ������ �� ����
			for( int k = 0; k < 3; ++k )
			{
				pv1 = ( l == 0 ? 2 : l -1 );
				nv1 = ( l == 2 ? 0 : l +1 );
				if( pTriConnectInfoList[k] != -1 ) continue;	// �̹� �̿��ϰ� �ִ� �ﰢ���� �ִ�
				for( int l = 0; l < 3; ++l )
				{
					pv2 = ( l == 0 ? 2 : l -1 );
					nv2 = ( l == 2 ? 0 : l +1 );
					if( pMeshNode->m_face_list[i].m_point_index[k] == pMeshNode->m_face_list[j].m_point_index[l] && pMeshNode->m_face_list[i].m_point_index[nv1] == pMeshNode->m_face_list[j].m_point_index[nv2])
					{
						pTriConnectInfoList[i].NeighborTriIndex[k] = j;	// i�� k��° �̿��� j
						pTriConnectInfoList[j].NeighborTriIndex[l] = i;		// j�� l��° �̿��� i
					}
					if( pMeshNode->m_face_list[i].m_point_index[k] == pMeshNode->m_face_list[j].m_point_index[pv2] && pMeshNode->m_face_list[i].m_point_index[nv1] == pMeshNode->m_face_list[j].m_point_index[l] )
					{
						pTriConnectInfoList[i].NeighborTriIndex[k] = j;			// i�� k��° �̿��� j
						pTriConnectInfoList[j].NeighborTriIndex[pv2] = i;		// j�� pv2��° �̿��� i
					}
				}
			}
		}
	}

	return pTriConnectInfoList;
}