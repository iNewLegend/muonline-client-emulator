#pragma once
#include "Vec2D.h"
#include "Vec3D.h"
#include "Vec4D.h"
#include "Color.h"

template<typename EnumType>
struct SEnumName
{
	static const char* List[];
};
//----------------------------------------------------------------

template<typename EnumType>
EnumType ConvertStringToEnum(const char* szStr)
{
	EnumType fooEnum = (EnumType)(-1);
	int count = sizeof(SEnumName<EnumType>::List)/
		sizeof(SEnumName<EnumType>::List[0]);
	for(int i = 0; i < count; ++i)
	{
		if (strcmp(szStr, SEnumName<EnumType>::List[i])==0)
		{
			fooEnum = (EnumType)(i);
			break;
		}
	}
	return fooEnum;
}
//----------------------------------------------------------------

enum CompareFunction
{
	CMPF_ALWAYS_FAIL,
	CMPF_ALWAYS_PASS,
	CMPF_LESS,
	CMPF_LESS_EQUAL,
	CMPF_EQUAL,
	CMPF_NOT_EQUAL,
	CMPF_GREATER_EQUAL,
	CMPF_GREATER
};
//----------------------------------------------------------------

const char* SEnumName<CompareFunction>::List[] =
{
	"ALWAYS_FAIL",
	"ALWAYS_PASS",
	"LESS",
	"LESS_EQUAL",
	"EQUAL",
	"NOT_EQUAL",
	"GREATER_EQUAL",
	"GREATER"
};
//----------------------------------------------------------------

enum StencilOP {
	STENCILOP_KEEP           = 1,
	STENCILOP_ZERO           = 2,
	STENCILOP_REPLACE        = 3,
	STENCILOP_INCRSAT        = 4,
	STENCILOP_DECRSAT        = 5,
	STENCILOP_INVERT         = 6,
	STENCILOP_INCR           = 7,
	STENCILOP_DECR           = 8,
	STENCILOP_FORCE_DWORD    = 0x7fffffff, /* force 32-bit size enum */
};
//----------------------------------------------------------------

enum VertexRenderOperationType
{
	VROT_POINT_LIST,
	VROT_LINE_LIST,
	VROT_LINE_STRIP,
	VROT_TRIANGLE_LIST,
	VROT_TRIANGLE_STRIP,
	VROT_TRIANGLE_FAN,
};
//----------------------------------------------------------------

enum TextureFilterType
{
	TEXF_NONE,
	TEXF_POINT,
	TEXF_LINEAR,
	TEXF_ANISOTROPIC,
	TEXF_PYRAMIDALQUAD,
	TEXF_GAUSSIANQUAD,
};
//----------------------------------------------------------------

enum AddressUV
{
	ADDRESS_WRAP,
	ADDRESS_MIRROR,
	ADDRESS_CLAMP,
	ADDRESS_BORDER,
	ADDRESS_MIRRORONCE,
};
//----------------------------------------------------------------

enum VertexElementSemantic
{
	/// Position, 3 reals per vertex
	VES_POSITION = 1,
	/// Blending weights
	VES_BLEND_WEIGHTS = 2,
	/// Blending indices
	VES_BLEND_INDICES = 3,
	/// Normal, 3 reals per vertex
	VES_NORMAL = 4,
	/// Diffuse colours
	VES_DIFFUSE = 5,
	/// Specular colours
	VES_SPECULAR = 6,
	/// Texture coordinates
	VES_TEXTURE_COORDINATES = 7,
	/// Binormal (Y axis if normal is Z)
	VES_BINORMAL = 8,
	/// Tangent (X axis if normal is Z)
	VES_TANGENT = 9
};
//----------------------------------------------------------------

/// Vertex element type, used to identify the base types of the vertex contents
enum VertexElementType
{
	VET_FLOAT1 = 0,
	VET_FLOAT2 = 1,
	VET_FLOAT3 = 2,
	VET_FLOAT4 = 3,
	/// alias to more specific colour type - use the current rendersystem's colour packing
	VET_COLOUR = 4,
	VET_SHORT1 = 5,
	VET_SHORT2 = 6,
	VET_SHORT3 = 7,
	VET_SHORT4 = 8,
	VET_UBYTE4 = 9,
	/// D3D style compact colour
	VET_COLOUR_ARGB = 10,
	/// GL style compact colour
	VET_COLOUR_ABGR = 11
};
//----------------------------------------------------------------

enum FillMode
{
	FILL_POINT               = 1,
	FILL_WIREFRAME           = 2,
	FILL_SOLID               = 3
};
//----------------------------------------------------------------

struct VertexElement
{
	unsigned short			uStream;     // Stream index
	unsigned short			uOffset;     // Offset in the stream in bytes
	VertexElementType		Type;       // Data type
	//BYTE    Method;     // Processing method
	VertexElementSemantic	Semantics;      // Semantics
	unsigned char			uIndex; // Semantic index
};
//----------------------------------------------------------------

// Flexible vertex format bits
//
#define FVF_RESERVED0        0x001
#define FVF_POSITION_MASK    0x400E
#define FVF_XYZ              0x002
#define FVF_XYZRHW           0x004
#define FVF_XYZB1            0x006
#define FVF_XYZB2            0x008
#define FVF_XYZB3            0x00a
#define FVF_XYZB4            0x00c
#define FVF_XYZB5            0x00e
#define FVF_XYZW             0x4002

#define FVF_NORMAL           0x010
#define FVF_PSIZE            0x020
#define FVF_DIFFUSE          0x040
#define FVF_SPECULAR         0x080

#define FVF_TEXCOUNT_MASK    0xf00
#define FVF_TEXCOUNT_SHIFT   8
#define FVF_TEX0             0x000
#define FVF_TEX1             0x100
#define FVF_TEX2             0x200
#define FVF_TEX3             0x300
#define FVF_TEX4             0x400
#define FVF_TEX5             0x500
#define FVF_TEX6             0x600
#define FVF_TEX7             0x700
#define FVF_TEX8             0x800

#define FVF_LASTBETA_UBYTE4   0x1000
#define FVF_LASTBETA_COLOR 0x8000

#define FVF_RESERVED2         0x6000  // 2 reserved bits

#define FVF_TEXTUREFORMAT2 0         // Two floating point values
#define FVF_TEXTUREFORMAT1 3         // One floating point value
#define FVF_TEXTUREFORMAT3 1         // Three floating point values
#define FVF_TEXTUREFORMAT4 2         // Four floating point values

#define FVF_TEXCOORDSIZE3(CoordIndex) (FVF_TEXTUREFORMAT3 << (CoordIndex*2 + 16))
#define FVF_TEXCOORDSIZE2(CoordIndex) (FVF_TEXTUREFORMAT2)
#define FVF_TEXCOORDSIZE4(CoordIndex) (FVF_TEXTUREFORMAT4 << (CoordIndex*2 + 16))
#define FVF_TEXCOORDSIZE1(CoordIndex) (FVF_TEXTUREFORMAT1 << (CoordIndex*2 + 16))

struct VERTEX_XYZ_DIF
{
	enum _FVF { FVF=(FVF_XYZ|FVF_DIFFUSE) };
	Vec3D		p;
	Color32		c;
};

struct VERTEX_XYZW_DIF
{
	enum _FVF { FVF=(FVF_XYZRHW|FVF_DIFFUSE) };
	Vec4D			p;
	Color32			c;
};

struct VERTEX_XYZ_DIF_TEX
{
	enum _FVF { FVF=(FVF_XYZ|FVF_DIFFUSE|FVF_TEX1) };
	Vec3D		p;
	Color32		c;
	Vec2D		t;
};
typedef VERTEX_XYZ_DIF_TEX SKYBOXVERTEX;

struct VERTEX_XYZW_DIF_TEX
{
	enum _FVF { FVF=(FVF_XYZRHW|FVF_DIFFUSE|FVF_TEX1) };
	Vec4D			p;
	Color32			c;
	Vec2D			t;
};

struct Fog
{
	float fStart;
	float fEnd;
	float fDensity;
	Color32 color;
	Fog(){}
	Fog(float fFogStart, float fFogEnd, float fFogDensity, Color32 clr):
	fStart(fFogStart),
		fEnd(fFogEnd),
		fDensity(fFogDensity),
		color(clr)
	{
	}
};

struct DirectionalLight
{
	Vec4D vAmbient;
	Vec4D vDiffuse;
	Vec4D vSpecular;
	Vec3D vDirection;
	DirectionalLight(){}
	DirectionalLight(const Vec4D& ambient,const Vec4D& diffuse,const Vec4D& specular,const Vec3D& direction):
	vAmbient(ambient),
		vDiffuse(diffuse),
		vSpecular(specular),
		vDirection(direction)
	{
	}
};

struct PointLight
{
	Vec4D vAmbient;
	Vec4D vDiffuse;
	Vec4D vSpecular;
	Vec3D vPosition;
	float fRange;
	PointLight(){}
	PointLight(const Vec4D& ambient,const Vec4D& diffuse,const Vec4D& specular,const Vec3D& position, float range):
	vAmbient(ambient),
		vDiffuse(diffuse),
		vSpecular(specular),
		vPosition(position),
		fRange(range)
	{
	}
};

struct IndexedSubset
{
	IndexedSubset()
	{
		memset(this,0,sizeof(*this));
	}
	unsigned short vbase;	// base vertex
	unsigned short vstart;	// first vertex
	unsigned short vcount;	// num vertices
	unsigned short istart;	// first index
	unsigned short icount;	// num indices
};

enum E_MATERIAL_RENDER_TYPE
{
	MATERIAL_NONE				= 0,
	MATERIAL_GEOMETRY			= 1<<0,
	MATERIAL_ALPHA				= 1<<1,
	MATERIAL_GLOW				= 1<<2,
	MATERIAL_BUMP				= 1<<3,
	MATERIAL_NORMAL				= MATERIAL_GEOMETRY+MATERIAL_ALPHA+MATERIAL_GLOW,
	MATERIAL_ALL				= MATERIAL_NORMAL+MATERIAL_BUMP,
	MATERIAL_RENDER_ALPHA_TEST	= 1<<4,
	MATERIAL_RENDER_NO_MATERIAL	= 1<<5,
	MATERIAL_SHADOW				= 1<<6,
};
