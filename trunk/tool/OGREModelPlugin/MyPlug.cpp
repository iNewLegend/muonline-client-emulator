#include "MyPlug.h"
#include "IORead.h"
#include "FileSystem.h"

BOOL WINAPI Data_Plug_CreateObject(void ** pobj){
	*pobj = new CMyPlug;
	return *pobj != NULL;
}

CMyPlug::CMyPlug(void)
{

}

CMyPlug::~CMyPlug(void)
{

}

enum MeshChunkID {
	M_HEADER                = 0x1000,
	// char*          version           : Version number check
	M_MESH                = 0x3000,
	// bool skeletallyAnimated   // important flag which affects h/w buffer policies
	// Optional M_GEOMETRY chunk
	M_SUBMESH             = 0x4000, 
	// char* materialName
	// bool useSharedVertices
	// unsigned int indexCount
	// bool indexes32Bit
	// unsigned int* faceVertexIndices (indexCount)
	// OR
	// unsigned short* faceVertexIndices (indexCount)
	// M_GEOMETRY chunk (Optional: present only if useSharedVertices = false)
	M_SUBMESH_OPERATION = 0x4010, // optional, trilist assumed if missing
	// unsigned short operationType
	M_SUBMESH_BONE_ASSIGNMENT = 0x4100,
	// Optional bone weights (repeating section)
	// unsigned int vertexIndex;
	// unsigned short boneIndex;
	// float weight;
	// Optional chunk that matches a texture name to an alias
	// a texture alias is sent to the submesh material to use this texture name
	// instead of the one in the texture unit with a matching alias name
	M_SUBMESH_TEXTURE_ALIAS = 0x4200, // Repeating section
	// char* aliasName;
	// char* textureName;

	M_GEOMETRY          = 0x5000, // NB this chunk is embedded within M_MESH and M_SUBMESH
	// unsigned int vertexCount
	M_GEOMETRY_VERTEX_DECLARATION = 0x5100,
	M_GEOMETRY_VERTEX_ELEMENT = 0x5110, // Repeating section
	// unsigned short source;  	// buffer bind source
	// unsigned short type;    	// VertexElementType
	// unsigned short semantic; // VertexElementSemantic
	// unsigned short offset;	// start offset in buffer in bytes
	// unsigned short index;	// index of the semantic (for colours and texture coords)
	M_GEOMETRY_VERTEX_BUFFER = 0x5200, // Repeating section
	// unsigned short bindIndex;	// Index to bind this buffer to
	// unsigned short vertexSize;	// Per-vertex size, must agree with declaration at this index
	M_GEOMETRY_VERTEX_BUFFER_DATA = 0x5210,
	// raw buffer data
	M_MESH_SKELETON_LINK = 0x6000,
	// Optional link to skeleton
	// char* skeletonName           : name of .skeleton to use
	M_MESH_BONE_ASSIGNMENT = 0x7000,
	// Optional bone weights (repeating section)
	// unsigned int vertexIndex;
	// unsigned short boneIndex;
	// float weight;
	M_MESH_LOD = 0x8000,
	// Optional LOD information
	// unsigned short numLevels;
	// bool manual;  (true for manual alternate meshes, false for generated)
	M_MESH_LOD_USAGE = 0x8100,
	// Repeating section, ordered in increasing depth
	// NB LOD 0 (full detail from 0 depth) is omitted
	// float fromSquaredDepth;
	M_MESH_LOD_MANUAL = 0x8110,
	// Required if M_MESH_LOD section manual = true
	// String manualMeshName;
	M_MESH_LOD_GENERATED = 0x8120,
	// Required if M_MESH_LOD section manual = false
	// Repeating section (1 per submesh)
	// unsigned int indexCount;
	// bool indexes32Bit
	// unsigned short* faceIndexes;  (indexCount)
	// OR
	// unsigned int* faceIndexes;  (indexCount)
	M_MESH_BOUNDS = 0x9000,
	// float minx, miny, minz
	// float maxx, maxy, maxz
	// float radius

	// Added By DrEvil
	// optional chunk that contains a table of submesh indexes and the names of
	// the sub-meshes.
	M_SUBMESH_NAME_TABLE = 0xA000,
	// Subchunks of the name table. Each chunk contains an index & string
	M_SUBMESH_NAME_TABLE_ELEMENT = 0xA100,
	// short index
	// char* name

	// Optional chunk which stores precomputed edge data					 
	M_EDGE_LISTS = 0xB000,
	// Each LOD has a separate edge list
	M_EDGE_LIST_LOD = 0xB100,
	// unsigned short lodIndex
	// bool isManual			// If manual, no edge data here, loaded from manual mesh
	// bool isClosed
	// unsigned long numTriangles
	// unsigned long numEdgeGroups
	// Triangle* triangleList
	// unsigned long indexSet
	// unsigned long vertexSet
	// unsigned long vertIndex[3]
	// unsigned long sharedVertIndex[3] 
	// float normal[4] 

	M_EDGE_GROUP = 0xB110,
	// unsigned long vertexSet
	// unsigned long triStart
	// unsigned long triCount
	// unsigned long numEdges
	// Edge* edgeList
	// unsigned long  triIndex[2]
	// unsigned long  vertIndex[2]
	// unsigned long  sharedVertIndex[2]
	// bool degenerate

	// Optional poses section, referred to by pose keyframes
	M_POSES = 0xC000,
	M_POSE = 0xC100,
	// char* name (may be blank)
	// unsigned short target	// 0 for shared geometry, 
	// 1+ for submesh index + 1
	M_POSE_VERTEX = 0xC111,
	// unsigned long vertexIndex
	// float xoffset, yoffset, zoffset
	// Optional vertex animation chunk
	M_ANIMATIONS = 0xD000, 
	M_ANIMATION = 0xD100,
	// char* name
	// float length
	M_ANIMATION_TRACK = 0xD110,
	// unsigned short type			// 1 == morph, 2 == pose
	// unsigned short target		// 0 for shared geometry, 
	// 1+ for submesh index + 1
	M_ANIMATION_MORPH_KEYFRAME = 0xD111,
	// float time
	// float x,y,z			// repeat by number of vertices in original geometry
	M_ANIMATION_POSE_KEYFRAME = 0xD112,
	// float time
	M_ANIMATION_POSE_REF = 0xD113, // repeat for number of referenced poses
	// unsigned short poseIndex 
	// float influence

	// Optional submesh extreme vertex list chink
	M_TABLE_EXTREMES = 0xE000,
	// unsigned short submesh_index;
	// float extremes [n_extremes][3];

	/* Version 1.2 of the .mesh format (deprecated)
	enum MeshChunkID {
	M_HEADER                = 0x1000,
	// char*          version           : Version number check
	M_MESH                = 0x3000,
	// bool skeletallyAnimated   // important flag which affects h/w buffer policies
	// Optional M_GEOMETRY chunk
	M_SUBMESH             = 0x4000, 
	// char* materialName
	// bool useSharedVertices
	// unsigned int indexCount
	// bool indexes32Bit
	// unsigned int* faceVertexIndices (indexCount)
	// OR
	// unsigned short* faceVertexIndices (indexCount)
	// M_GEOMETRY chunk (Optional: present only if useSharedVertices = false)
	M_SUBMESH_OPERATION = 0x4010, // optional, trilist assumed if missing
	// unsigned short operationType
	M_SUBMESH_BONE_ASSIGNMENT = 0x4100,
	// Optional bone weights (repeating section)
	// unsigned int vertexIndex;
	// unsigned short boneIndex;
	// float weight;
	M_GEOMETRY          = 0x5000, // NB this chunk is embedded within M_MESH and M_SUBMESH
	*/
	// unsigned int vertexCount
	// float* pVertices (x, y, z order x numVertices)
	M_GEOMETRY_NORMALS = 0x5100,    //(Optional)
	// float* pNormals (x, y, z order x numVertices)
	M_GEOMETRY_COLOURS = 0x5200,    //(Optional)
	// unsigned long* pColours (RGBA 8888 format x numVertices)
	M_GEOMETRY_TEXCOORDS = 0x5300,    //(Optional, REPEATABLE, each one adds an extra set)
	// unsigned short dimensions    (1 for 1D, 2 for 2D, 3 for 3D)
	// float* pTexCoords  (u [v] [w] order, dimensions x numVertices)
	/*
	M_MESH_SKELETON_LINK = 0x6000,
	// Optional link to skeleton
	// char* skeletonName           : name of .skeleton to use
	M_MESH_BONE_ASSIGNMENT = 0x7000,
	// Optional bone weights (repeating section)
	// unsigned int vertexIndex;
	// unsigned short boneIndex;
	// float weight;
	M_MESH_LOD = 0x8000,
	// Optional LOD information
	// unsigned short numLevels;
	// bool manual;  (true for manual alternate meshes, false for generated)
	M_MESH_LOD_USAGE = 0x8100,
	// Repeating section, ordered in increasing depth
	// NB LOD 0 (full detail from 0 depth) is omitted
	// float fromSquaredDepth;
	M_MESH_LOD_MANUAL = 0x8110,
	// Required if M_MESH_LOD section manual = true
	// String manualMeshName;
	M_MESH_LOD_GENERATED = 0x8120,
	// Required if M_MESH_LOD section manual = false
	// Repeating section (1 per submesh)
	// unsigned int indexCount;
	// bool indexes32Bit
	// unsigned short* faceIndexes;  (indexCount)
	// OR
	// unsigned int* faceIndexes;  (indexCount)
	M_MESH_BOUNDS = 0x9000
	// float minx, miny, minz
	// float maxx, maxy, maxz
	// float radius

	// Added By DrEvil
	// optional chunk that contains a table of submesh indexes and the names of
	// the sub-meshes.
	M_SUBMESH_NAME_TABLE,
	// Subchunks of the name table. Each chunk contains an index & string
	M_SUBMESH_NAME_TABLE_ELEMENT,
	// short index
	// char* name

	*/
};
const long STREAM_OVERHEAD_SIZE = sizeof(unsigned short) + sizeof(unsigned long);

int CMyPlug::Execute(iModelData * pModelData, bool bShowDlg, bool bSpecifyFileName)
{
	return -1;
}

/*enum VertexElementType
{
	VET_FLOAT1,
	VET_FLOAT2,
	VET_FLOAT3,
	VET_FLOAT4,
	VET_COLOUR,
	VET_SHORT1,
	VET_SHORT2,
	VET_SHORT3,
	VET_SHORT4,
	VET_UBYTE4
};
enum VertexElementSemantic
{
	VES_POSITION = 1,               // 位置：每个顶点要使用三个实数 (Real) 来表示
	VES_BLEND_WEIGHTS = 2,          // 混合权重
	VES_BLEND_INDICES = 3,          // 混合索引
	VES_NORMAL = 4,                 // 法线：每个顶点的法线要使用三个实数 (Real) 来表示
	VES_DIFFUSE = 5,                // 散射光的颜色值
	VES_SPECULAR = 6,               // 反射光的颜色值
	VES_TEXTURE_COORDINATES = 7,    // 纹理坐标
	VES_BINORMAL = 8,               // 副法线 (如果法线是 Z 轴，副法线就是 Y 轴)
	VES_TANGENT = 9                 // 切线 (如果法线是 Z 轴，切线就是 X 轴)
};*/

struct GeometryVertexElement
{
	unsigned short source;		// buffer bind source
	VertexElementType vType;
	VertexElementSemantic vSemantic;
	unsigned short offset;		// start offset in buffer in bytes
	unsigned short index;		// index of the semantic
};

void readGeometryVertexDeclaration(IOReadBase* pRead, std::vector<GeometryVertexElement>& setElement)
{
	// Find optional geometry streams
	if (!pRead->IsEof())
	{
		unsigned short streamID;
		unsigned int uLength;
		pRead->Read(&streamID,sizeof(unsigned short));
		pRead->Read(&uLength,sizeof(unsigned int));
		while(!pRead->IsEof() &&(streamID == M_GEOMETRY_VERTEX_ELEMENT ))
		{
			switch (streamID)
			{
			case M_GEOMETRY_VERTEX_ELEMENT:
				{
					GeometryVertexElement element; 
					unsigned short tmp;
					pRead->Read(&element.source,sizeof(unsigned short));
					pRead->Read(&tmp,sizeof(unsigned short));
					element.vType = static_cast<VertexElementType>(tmp);
					pRead->Read(&tmp,sizeof(unsigned short));
					element.vSemantic = static_cast<VertexElementSemantic>(tmp);
					pRead->Read(&element.offset,sizeof(unsigned short));
					pRead->Read(&element.index,sizeof(unsigned short));

					setElement.push_back(element);
					//dest->vertexDeclaration->addElement(source, offset, vType, vSemantic, index);

					// 	if (vType == VET_COLOUR)
					// 	{
					// 		LogManager::getSingleton().stream()
					// 			<< "Warning: VET_COLOUR element type is deprecated, you should use "
					// 			<< "one of the more specific types to indicate the byte order. "
					// 			<< "Use OgreMeshUpgrade on " << pMesh->getName() << " as soon as possible. ";
					// 	}
				}
				break;
			}
			// Get next stream
			if (!pRead->IsEof())
			{
				pRead->Read(&streamID,sizeof(unsigned short));
				pRead->Read(&uLength,sizeof(unsigned int));
			}
		}
		if (!pRead->IsEof())
		{
			// Backpedal back to start of non-submesh stream
			pRead->Move(-STREAM_OVERHEAD_SIZE);
		}
	}
}

void readGeometryVertexBuffer(IOReadBase* pRead, CSubMesh& subMesh,std::vector<GeometryVertexElement>& setElement,unsigned int vertexCount)
{
	unsigned short bindIndex, vertexSize;
	// unsigned short bindIndex;	// Index to bind this buffer to
	pRead->Read(&bindIndex,sizeof(unsigned short));
	// unsigned short vertexSize;	// Per-vertex size, must agree with declaration at this index
	pRead->Read(&vertexSize,sizeof(unsigned short));

	// Check for vertex data header
	unsigned short streamID;
	unsigned int uLength;
	pRead->Read(&streamID,sizeof(unsigned short));
	pRead->Read(&uLength,sizeof(unsigned int));
	if (streamID != M_GEOMETRY_VERTEX_BUFFER_DATA)
	{
		MessageBoxW(NULL, L"Can't find vertex buffer data area",	L"MeshSerializerImpl::readGeometryVertexBuffer",0);
	}

	for (size_t i=0;i<vertexCount;++i)
	{
		for (size_t n=0;n<setElement.size();++n)
		{
			if (setElement[n].source==bindIndex)
			{
				switch(setElement[n].vSemantic)
				{
				case VES_POSITION:
					{
						Vec3D vPos;
						pRead->Read(&vPos,sizeof(Vec3D));
						vPos.x=-vPos.x;
						subMesh.addPos(vPos);
					}
					break;
				case VES_BLEND_WEIGHTS:
					{
						unsigned int uWeight;
						pRead->Read(&uWeight,sizeof(unsigned int));
						subMesh.addWeight(uWeight);
					}
					break;
				case VES_BLEND_INDICES:
					{
						unsigned int uBone;
						pRead->Read(&uBone,sizeof(unsigned int));
						subMesh.addBone(uBone);
					}
					break;
				case VES_NORMAL:
					{
						Vec3D vNormal;
						pRead->Read(&vNormal,sizeof(Vec3D));
						vNormal.x=-vNormal.x;
						subMesh.addNormal(vNormal);
					}
					break;
				case VES_DIFFUSE:
					{
						Color32 color;
						pRead->Read(&color,sizeof(Color32));
						subMesh.addColor(color);
					}
					break;
				case VES_SPECULAR:
					{
						Color32 color;
						pRead->Read(&color,sizeof(Color32));
						//subMesh.addColor(color);
					}
					break;
				case VES_TEXTURE_COORDINATES:
					{
						Vec2D vUV;
						pRead->Read(&vUV,sizeof(Vec2D));
						subMesh.addTexcoord(vUV);
					}
					break;
				case VES_BINORMAL:
					{
						Vec3D vBiNormal;
						pRead->Read(&vBiNormal,sizeof(Vec3D));
						//pMesh->addBiNormal(vBiNormal);
					}
					break;
				case VES_TANGENT:
					{
						Vec3D vTangent;
						pRead->Read(&vTangent,sizeof(Vec3D));
						//pMesh->addTangent(vTangent);
					}
					break;
				default:
				    break;
				}
			}
		}
	}
	//pRead->read(pBuf, dest->vertexCount * vertexSize);

}

void readGeometry(IOReadBase* pRead, CSubMesh& subMesh)
{
	unsigned int vertexCount = 0;
	pRead->Read(&vertexCount,sizeof(unsigned int));

	unsigned short streamID;
	unsigned int uLength;
	pRead->Read(&streamID,sizeof(unsigned short));
	pRead->Read(&uLength,sizeof(unsigned int));

	std::vector<GeometryVertexElement> setElement; 
	while(!pRead->IsEof() && (streamID == M_GEOMETRY_VERTEX_DECLARATION ||streamID == M_GEOMETRY_VERTEX_BUFFER ))
	{
		switch (streamID)
		{
		case M_GEOMETRY_VERTEX_DECLARATION:
			readGeometryVertexDeclaration(pRead, setElement);
			break;
		case M_GEOMETRY_VERTEX_BUFFER:
			readGeometryVertexBuffer(pRead, subMesh, setElement, vertexCount);
			break;
		}
		// Get next stream
		if (!pRead->IsEof())
		{
			pRead->Read(&streamID,sizeof(unsigned short));
			pRead->Read(&uLength,sizeof(unsigned int));
		}
	}
	if (!pRead->IsEof())
	{
		// Backpedal back to start of non-submesh stream
		pRead->Move(-STREAM_OVERHEAD_SIZE);
	}
}

std::string readString(IOReadBase* pRead)
{
	char	c;
	std::string str;
	pRead->Read(&c,sizeof(char));
	while (c!='\n')
	{
		str.push_back(c);
		pRead->Read(&c,sizeof(char));
	}
	return str;
}

typedef struct VertexBoneAssignment_s
{
	unsigned int vertexIndex;
	unsigned short boneIndex;
	float weight;
} VertexBoneAssignment;
bool moveToString(IOReadBase* pRead,const std::string& str)
{
	char c;
	while (!pRead->IsEof())
	{
		for (size_t i=0;i<str.length();++i)
		{
			pRead->Read(&c,sizeof(char));
			if (pRead->IsEof())
			{
				return false;
			}
			if (str[i]!=c)
			{
				break;
			}
			if (i==str.length()-1)
			{
				return true;
			}
		}
	}
	return false;
}

std::string getWord(IOReadBase* pRead)
{
	std::string str;
	char c;
	pRead->Read(&c,sizeof(char));
	while (!pRead->IsEof())
	{
		if ('	'==c||' '==c||'\n'==c||char(13)==c)
		{
			if (str.size()>0)
			{
				return str;
			}
		}
		else
		{
			str.push_back(c);
		}
		pRead->Read(&c,sizeof(char));
	}
	return str;
}

std::string getLineCommand(IOReadBase* pRead, std::vector<std::string>& setWords)
{
	setWords.clear();
	std::string str;
	std::string strCommand;
	char c;
	while (!pRead->IsEof())
	{
		pRead->Read(&c,sizeof(char));
		if ('	'==c||' '==c||'\n'==c||char(13)==c)
		{
			if (strCommand.length()==0)
			{
				strCommand = str;
			}
			else
			{
				setWords.push_back(str);
			}
			str.clear();
			if ('\n'==c||char(13)==c)
			{
				if (strCommand.size()>0)
				{
					break;
				}
			}
		}
		else
		{
			str.push_back(c);
		}
	}
	if (pRead->IsEof())
	{
		if (strCommand.length()==0)
		{
			strCommand = str;
		}
		else
		{
			setWords.push_back(str);
		}
	}
	return strCommand;
}

bool readMaterialTexture(CMaterial& material, IOReadBase* pRead, const std::string& strFilename)
{
	std::string strCommand;
	std::vector<std::string> setWords;

	strCommand = getLineCommand(pRead,setWords);
	if ("{"!=strCommand)
	{
		return false;
	}

	std::string strTexture;
	bool bAdd = false;
	while (!pRead->IsEof())
	{
		strCommand = getLineCommand(pRead,setWords);
		if ("texture"==strCommand)
		{
			if (setWords.size()>0)
			{
				strTexture=GetParentPath(strFilename)+setWords[0];
				strTexture = ChangeExtension(strTexture,".dds");
			}
		}
		else if ("colour_op"==strCommand)
		{
			if (setWords.size()>0)
			{
				if ("add"==setWords[0])
				{
					bAdd = true;
				}
			}
		}
		else if ("scroll_anim"==strCommand)
		{
			if (setWords.size()==2)
			{
				material.vTexAnim.x=atof(setWords[0].c_str());
				material.vTexAnim.y=atof(setWords[1].c_str());
			}
		}
		else if ("}"==strCommand)
		{
			break;
		}
		else
		{
			MessageBoxA(NULL,strCommand.c_str(),"Unknon in texture_unit!",0);
		}
	}
	if (strTexture.length()>0)
	{
		if (bAdd)
		{
			//material.setEmissive(strTexture);
			material.setTexture(0,strTexture.c_str());
		}
		else
		{
			material.setTexture(0,strTexture.c_str());
		}
	}
	return true;
}

bool readMaterialPass(CMaterial& material, IOReadBase* pRead, const std::string& strFilename)
{
	std::string strCommand;
	std::vector<std::string> setWords;

	strCommand = getLineCommand(pRead,setWords);
	if ("{"!=strCommand)
	{
		return false;
	}
	while (!pRead->IsEof())
	{
		strCommand = getLineCommand(pRead,setWords);
		if ("ambient"==strCommand)
		{
			// 3
		}
		else if ("diffuse"==strCommand)
		{
			// 3
		}
		else if ("specular"==strCommand)
		{
			//4
		}
		else if ("emissive"==strCommand)
		{
			// 3
		}
		else if ("depth_write"==strCommand)
		{
			if (setWords.size()>0)
			{
				if ("off"==setWords[0])
				{
					material.bDepthWrite=false;
				}
				else
				{
					MessageBoxA(NULL,setWords[0].c_str(),"Unknon in depth_write!",0);
				}
			}
		}
		else if ("scene_blend"==strCommand)
		{
			if (setWords.size()==1)
			{
				if ("alpha_blend"==setWords[0])
				{
					material.bBlend=true;
				}
				else
				{
					MessageBoxA(NULL,setWords[0].c_str(),"Unknon in scene_blend!",0);
				}
			}
			else if (setWords.size()==2)
			{
				MessageBoxA(NULL,setWords[0].c_str(),"Unknon in scene_blend!",0);
			}
		}
		else if ("alpha_rejection"==strCommand)
		{
			if (setWords.size()==2)
			{
				material.bAlphaTest=true;
				material.uAlphaTestValue=atoi(setWords[1].c_str());
			}
		}
		else if ("lighting"==strCommand)
		{
			material.bLightingEnabled=true;
			// 1
		}
		else if ("texture_unit"==strCommand)
		{
			if (!readMaterialTexture(material,pRead,strFilename))
			{
				MessageBoxA(NULL,"readMaterialTexture()","Error!",0);
				return false;
			}
		}
		else if ("}"==strCommand)
		{
			break;
		}
		else
		{
			MessageBoxA(NULL,strCommand.c_str(),"Unknon in pass!",0);
		}
	}
	return true;
}

bool readMaterialTechnique(CMaterial& material, IOReadBase* pRead, const std::string& strFilename)
{
	std::string strCommand;
	std::vector<std::string> setWords;

	strCommand = getLineCommand(pRead,setWords);
	if ("{"!=strCommand)
	{
		return false;
	}
	while (!pRead->IsEof())
	{
		strCommand = getLineCommand(pRead,setWords);
		if ("pass"==strCommand)
		{
			if (!readMaterialPass(material,pRead,strFilename))
			{
				return false;
			}
		}
		else if ("}"==strCommand)
		{
			break;
		}
		else
		{
			MessageBoxA(NULL,strCommand.c_str(),"Unknon in technique!",0);
		}
	}
	return true;
}

bool readMaterial(CMaterial& material, IOReadBase* pRead, const std::string& strFilename)
{
	std::string strCommand;
	std::vector<std::string> setWords;

	strCommand = getLineCommand(pRead,setWords);
	if ("{"!=strCommand)
	{
		return false;
	}
	while (!pRead->IsEof())
	{
		strCommand = getLineCommand(pRead,setWords);
		if ("technique"==strCommand)
		{
			if (!readMaterialTechnique(material,pRead,strFilename))
			{
				return false;
			}
		}
		else if ("}"==strCommand)
		{
			break;
		}
		else
		{
			MessageBoxA(NULL,strCommand.c_str(),"Unknon in material!",0);
		}
	}
	return true;
}

bool readMaterial(iModelData * pModelData, const std::string& strFilename)
{
	IOReadBase* pRead = IOReadBase::autoOpen(strFilename);
	if (pRead==NULL)
	{
		return false;
	}
	std::string strCommand;
	std::vector<std::string> setWords;
	while (!pRead->IsEof())
	{
		strCommand = getLineCommand(pRead,setWords);
		if ("material"==strCommand)
		{
			if (setWords.size()>0)
			{
				CMaterial& material = pModelData->getMaterial(setWords[0].c_str());
				readMaterial(material,pRead,strFilename);
			}
		}
	}
	IOReadBase::autoClose(pRead);
	return true;
}

void readSubMesh(IOReadBase* pRead, iModelData* pModelData, const CSubMesh& sharedSubMesh)
{
	iLodMesh* pMesh = &pModelData->getMesh();
	std::string strMaterialName = readString(pRead);
	int nSubID=pMesh->getSubCount();
	pModelData->setRenderPass(nSubID,nSubID,strMaterialName);
	
// 	if(listener)
// 		listener->processMaterialName(pMesh, &materialName);
// 	sm->setMaterialName(materialName);

	// bool useSharedVertices
	bool useSharedVertices;
	pRead->Read(&useSharedVertices,sizeof(bool));
	CSubMesh& subMesh=pMesh->addSubMesh();
	if (useSharedVertices)
	{
		subMesh.pos			=sharedSubMesh.pos;
		subMesh.weight		=sharedSubMesh.weight;
		subMesh.bone		=sharedSubMesh.bone;
		subMesh.normal		=sharedSubMesh.normal;
		subMesh.color		=sharedSubMesh.color;
		subMesh.texcoord	=sharedSubMesh.texcoord;
		subMesh.texcoord2	=sharedSubMesh.texcoord2;
	}

	// sm->indexData->indexStart = 0;
	unsigned int indexCount = 0;
	pRead->Read(&indexCount,sizeof(unsigned int));
	//sm->indexData->indexCount = indexCount;

	// bool indexes32Bit
	bool idx32bit;
	pRead->Read(&idx32bit,sizeof(bool));

	if (idx32bit)
	{
		MessageBoxW(0,L"Can't read idx32bit",L"Error",0);
		VertexIndex vertexIndex;
		for (size_t i=0;i<indexCount;++i)
		{
			unsigned int uVertexIndex;
			pRead->Read(&uVertexIndex,sizeof(unsigned int));
			vertexIndex.p=uVertexIndex;
			vertexIndex.n=uVertexIndex;
			vertexIndex.c=uVertexIndex;
			vertexIndex.uv1=uVertexIndex;
			vertexIndex.b=uVertexIndex;
			vertexIndex.w=uVertexIndex;
			subMesh.m_setVertexIndex.push_back(vertexIndex);
		}
	}
	else // 16-bit
	{
		VertexIndex vertexIndex;
		for (size_t i=0;i<indexCount;++i)
		{
			unsigned short uVertexIndex;
			pRead->Read(&uVertexIndex,sizeof(unsigned short));
			vertexIndex.p=uVertexIndex;
			vertexIndex.n=uVertexIndex;
			vertexIndex.c=uVertexIndex;
			vertexIndex.uv1=uVertexIndex;
			vertexIndex.b=uVertexIndex;
			vertexIndex.w=uVertexIndex;
			subMesh.m_setVertexIndex.push_back(vertexIndex);
		}
	}

	// M_GEOMETRY stream (Optional: present only if useSharedVertices = false)
	if (!useSharedVertices)
	{
		unsigned short streamID;
		unsigned int uLength;
		pRead->Read(&streamID,sizeof(unsigned short));
		pRead->Read(&uLength,sizeof(unsigned int));
		if (streamID != M_GEOMETRY)
		{
			MessageBoxW(0,L"Missing geometry data in mesh file",L"readSubMesh",0);
		}
		readGeometry(pRead, subMesh);
	}


	// Find all bone assignments, submesh operation, and texture aliases (if present)
	if (!pRead->IsEof())
	{
		unsigned short streamID;
		unsigned int uLength;
		pRead->Read(&streamID,sizeof(unsigned short));
		pRead->Read(&uLength,sizeof(unsigned int));
		while(!pRead->IsEof() &&
			(streamID == M_SUBMESH_BONE_ASSIGNMENT ||
			streamID == M_SUBMESH_OPERATION ||
			streamID == M_SUBMESH_TEXTURE_ALIAS))
		{
			switch(streamID)
			{
			case M_SUBMESH_OPERATION:
			//	readSubMeshOperation(stream, pMesh, sm);
				// unsigned short operationType
				unsigned short opType;
				pRead->Read(&opType,sizeof(unsigned short));
				//sm->operationType = static_cast<RenderOperation::OperationType>(opType);
				break;
			case M_SUBMESH_BONE_ASSIGNMENT:
				{
					//readSubMeshBoneAssignment(stream, pMesh, sm);
					VertexBoneAssignment assign;

					// unsigned int vertexIndex;
					pRead->Read(&(assign.vertexIndex),sizeof(unsigned int));
					// unsigned short boneIndex;
					pRead->Read(&(assign.boneIndex),sizeof(unsigned short));
					// float weight;
					pRead->Read(&(assign.weight),sizeof(float));
break;
					unsigned long uBone = 0;
					unsigned long uWeight = 0;
					// get
					subMesh.getBone(assign.vertexIndex,uBone);
					subMesh.getWeight(assign.vertexIndex,uWeight);
					// add
					for (size_t i=0;i<4;++i)
					{
						if (((unsigned char*)&uWeight)[i]==0)
						{
							((unsigned char*)&uWeight)[i] = assign.weight*255;
							((unsigned char*)&uBone)[i] = assign.boneIndex;
							break;
						}
					}
					// set
					subMesh.setBone(assign.vertexIndex,uBone);
					subMesh.setWeight(assign.vertexIndex,uWeight);

					//sub->addBoneAssignment(assign);
				}
				break;
			case M_SUBMESH_TEXTURE_ALIAS:
				//readSubMeshTextureAlias(stream, pMesh, sm);
				std::string aliasName = readString(pRead);
				std::string textureName = readString(pRead);
				//sub->addTextureAlias(aliasName, textureName);
				break;
			}

			if (!pRead->IsEof())
			{
				pRead->Read(&streamID,sizeof(unsigned short));
				pRead->Read(&uLength,sizeof(unsigned int));
			}

		}
		if (!pRead->IsEof())
		{
			// Backpedal back to start of non-submesh stream
			pRead->Move(-STREAM_OVERHEAD_SIZE);
		}
	}
}

void readHeader(IOReadBase* pRead)
{
	unsigned short headerID;
	pRead->Read(&headerID,sizeof(unsigned short));

	if (headerID == M_HEADER)// OK
	{
		// Read version
		std::string strVersion = readString(pRead);
		//if (ver != mVersion)
		//{
		//	OGRE_EXCEPT(Exception::ERR_INTERNAL_ERROR, 
		//		"Invalid file: version incompatible, file reports " + String(ver) +
		//		" Serializer is version " + mVersion,
		//		"Serializer::readFileHeader");
		//}
	}
	else
	{
		MessageBoxW(NULL,L"Invalid file: no header",0,0);
	}
}

void readMesh(IOReadBase* pRead, iModelData* pModelData)
{
	bool skeletallyAnimated;
	pRead->Read(&skeletallyAnimated,sizeof(bool));
	// Find all substreams
	if (!pRead->IsEof())
	{
		CSubMesh sharedSubMesh;
		unsigned short streamID;
		unsigned int uLength;
		pRead->Read(&streamID,sizeof(unsigned short));
		pRead->Read(&uLength,sizeof(unsigned int));

		while(!pRead->IsEof() &&
			(streamID == M_GEOMETRY ||
			streamID == M_SUBMESH ||
			streamID == M_MESH_SKELETON_LINK ||
			streamID == M_MESH_BONE_ASSIGNMENT ||
			streamID == M_MESH_LOD ||
			streamID == M_MESH_BOUNDS ||
			streamID == M_SUBMESH_NAME_TABLE ||
			streamID == M_EDGE_LISTS ||
			streamID == M_POSES ||
			streamID == M_ANIMATIONS ||
			streamID == M_TABLE_EXTREMES))
		{
			switch(streamID)
			{
			case M_GEOMETRY:
				{
					//MessageBoxW(NULL, L"M_GEOMETRY",	L"readMesh",0);
					readGeometry(pRead, sharedSubMesh);
				}
				break;
			case M_SUBMESH:
				readSubMesh(pRead, pModelData, sharedSubMesh);
				break;
			case M_MESH_SKELETON_LINK:
				{
					std::string skelName = readString(pRead);

					// 				if(listener)
					// 					listener->processSkeletonName(pMesh, &skelName);
					// 
					// 				pMesh->setSkeletonName(skelName);
					//readSkeletonLink(stream, pMesh, listener);
				}
				break;
			case M_MESH_BONE_ASSIGNMENT:
				//readMeshBoneAssignment(stream, pMesh);
				{
					VertexBoneAssignment assign;

					// unsigned int vertexIndex;
					pRead->Read(&(assign.vertexIndex),sizeof(unsigned int));
					// unsigned short boneIndex;
					pRead->Read(&(assign.boneIndex),sizeof(unsigned short));
					// float weight;
					pRead->Read(&(assign.weight),sizeof(float));

					//sub->addBoneAssignment(assign);
				}
				break;
			case M_MESH_LOD:
				MessageBoxW(NULL,L"M_MESH_LOD",0,0);
				//readMeshLodInfo(stream, pMesh);
				break;
			case M_MESH_BOUNDS:
				//readBoundsInfo(stream, pMesh);
				{
					Vec3D min, max;
					// float minx, miny, minz
					pRead->Read(&min,sizeof(Vec3D));
					// float maxx, maxy, maxz
					pRead->Read(&max,sizeof(Vec3D));
					//AxisAlignedBox box(min, max);
					//pMesh->_setBounds(box, true);
					// float radius
					float radius;
					pRead->Read(&radius,sizeof(float));
					//pMesh->_setBoundingSphereRadius(radius);
				}
				break;
			case M_SUBMESH_NAME_TABLE:
				MessageBoxW(NULL,L"M_SUBMESH_NAME_TABLE",0,0);
				//readSubMeshNameTable(stream, pMesh);
				break;
			case M_EDGE_LISTS:
				MessageBoxW(NULL,L"M_EDGE_LISTS",0,0);
				//readEdgeList(stream, pMesh);
				break;
			case M_POSES:
				MessageBoxW(NULL,L"M_POSES",0,0);
				//readPoses(stream, pMesh);
				break;
			case M_ANIMATIONS:
				MessageBoxW(NULL,L"M_ANIMATIONS",0,0);
				//readAnimations(stream, pMesh);
				break;
			case M_TABLE_EXTREMES:
				MessageBoxW(NULL,L"M_TABLE_EXTREMES",0,0);
				//readExtremes(stream, pMesh);
				break;
			}

			if (!pRead->IsEof())
			{
				pRead->Read(&streamID,sizeof(unsigned short));
				pRead->Read(&uLength,sizeof(unsigned int));
			}
		}
		if (!pRead->IsEof())
		{
			// Backpedal back to start of non-submesh stream
			pRead->Move(-STREAM_OVERHEAD_SIZE);
		}
	}
}


bool CMyPlug::importData(iModelData * pModelData, const std::string& strFilename)
{
	assert(pModelData);
	// Loading the mesh.
	IOReadBase* pRead = IOReadBase::autoOpen(strFilename);
	if (!pRead)
	{
		return false;
	}
	// header
	readHeader(pRead);
	// mesh
	if (!pRead->IsEof())
	{
		unsigned short streamID;
		unsigned int uLength;
		pRead->Read(&streamID,sizeof(unsigned short));
		pRead->Read(&uLength,sizeof(unsigned int));

		switch (streamID)
		{
		case M_MESH:
			{
				readMesh(pRead,pModelData);
				break;
			}
		}
	}
	// close file
	IOReadBase::autoClose(pRead);

	// Loading the materials.
	if (!readMaterial(pModelData,ChangeExtension(strFilename,".material")))
	{
		std::string strMatFilename = GetParentPath(strFilename);
		strMatFilename=strMatFilename+GetFilename(strMatFilename)+".material";
		if (!readMaterial(pModelData,strMatFilename))
		{
			strMatFilename=ChangeExtension(strMatFilename,"_tileset.material");
			readMaterial(pModelData,strMatFilename);
		}
	}

	// mesh update
	pModelData->getMesh().update();

// 	//m_bbox = mesh.getBBox();
// 	std::string strMyPath ="Data\\"+GetFilename(GetParentPath(strFilename))+"\\";
// 	std::string strMatFilename = ChangeExtension(strFilename,".mat.csv");
// 	std::string strParFilename = ChangeExtension(strFilename,".par.csv");
// 	if (!IOReadBase::Exists(strMatFilename))
// 	{
// 		strMatFilename=strMyPath+ChangeExtension(GetFilename(strFilename),".mat.csv");
// 	}
// 	if (!IOReadBase::Exists(strParFilename))
// 	{
// 		strParFilename=strMyPath+ChangeExtension(GetFilename(strFilename),".par.csv");
// 	}
// 
// 	pModelData->loadMaterial(strMatFilename);
// 	pModelData->loadParticleEmitters(strParFilename);
	return true;
}

bool CMyPlug::exportData(iModelData * pModelData, const std::string& strFilename)
{
	return true;
}

void CMyPlug::release()
{
	delete this;
}