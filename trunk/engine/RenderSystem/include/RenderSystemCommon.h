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

enum LayerBlendType
{
	LBT_COLOUR,
	LBT_ALPHA
};
//----------------------------------------------------------------

enum LayerBlendOperation
{
	/// Replace all colour with texture with no adjustment
	LBO_REPLACE,
	/// Add colour components together.
	LBO_ADD,
	/// Multiply colour components together.
	LBO_MODULATE,
	/// Blend based on texture alpha
	LBO_ALPHA_BLEND
};
//----------------------------------------------------------------

enum TextureBlendOperation
{
	TBOP_DISABLE,
	TBOP_SOURCE1,/// use source1 without modification
	TBOP_SOURCE2,/// use source2 without modification
	TBOP_MODULATE,/// multiply source1 and source2 together
	TBOP_MODULATE_X2,/// as TBOP_MODULATE but brighten afterwards (x2)
	TBOP_MODULATE_X4,/// as TBOP_MODULATE but brighten more afterwards (x4)
	TBOP_ADD,/// add source1 and source2 together
	TBOP_ADD_SIGNED,/// as TBOP_ADD, but subtract 0.5 from the result
	TBOP_ADD_SMOOTH,/// as TBOP_ADD, but subtract product from the sum
	TBOP_SUBTRACT,/// subtract source2 from source1
	TBOP_BLEND_DIFFUSE_ALPHA,/// use interpolated alpha value from vertices to scale source1, then add source2 scaled by (1-alpha)
	TBOP_BLEND_TEXTURE_ALPHA,/// as TBOP_BLEND_DIFFUSE_ALPHA, but use alpha from texture
	TBOP_BLEND_CURRENT_ALPHA,/// as TBOP_BLEND_DIFFUSE_ALPHA, but use current alpha from previous stages
	TBOP_BLEND_MANUAL,/// as TBOP_BLEND_DIFFUSE_ALPHA but use a constant manual blend value (0.0-1.0)
	TBOP_DOTPRODUCT,/// dot product of color1 and color2 
	TBOP_BLEND_DIFFUSE_COLOUR/// use interpolated color values from vertices to scale source1, then add source2 scaled by (1-color)
};
//----------------------------------------------------------------

const char* SEnumName<TextureBlendOperation>::List[] =
{
	"DISABLE",
	"SOURCE1",
	"SOURCE2",
	"MODULATE",
	"MODULATE_X2",
	"MODULATE_X4",
	"ADD",
	"ADD_SIGNED",
	"ADD_SMOOTH",
	"SUBTRACT",
	"BLEND_DIFFUSE_ALPHA",
	"BLEND_TEXTURE_ALPHA",
	"BLEND_CURRENT_ALPHA",
	"BLEND_MANUAL",
	"DOTPRODUCT",
	"BLEND_DIFFUSE_COLOUR"
};
//----------------------------------------------------------------

enum TextureBlendSource
{
	TBS_CURRENT,
	TBS_TEXTURE,
	TBS_DIFFUSE,
	TBS_SPECULAR,
	TBS_MANUAL,		// a colour supplied manually as a separate argument
	TBS_TFACTOR,
	TBS_TEMP		// select temporary register color (read/write) (from d3d)
};
//----------------------------------------------------------------

const char* SEnumName<TextureBlendSource>::List[] =
{
	"CURRENT",
	"TEXTURE",
	"DIFFUSE",
	"SPECULAR",
	"MANUAL",
	"TFACTOR",
	"TEMP"
};
//----------------------------------------------------------------

enum SceneBlendType
{
	/// Make the object transparent based on the final alpha values in the texture
	SBT_TRANSPARENT_ALPHA,
	/// Make the object transparent based on the colour values in the texture (brighter = more opaque)
	SBT_TRANSPARENT_COLOUR,
	/// Add the texture values to the existing scene content
	SBT_ADD,
	/// Multiply the 2 colours together
	SBT_MODULATE,
	/// The default blend mode where source replaces destination
	SBT_REPLACE
	// TODO : more
};
//----------------------------------------------------------------

enum SceneBlendOperation
{
	BLENDOP_ADD,
	BLENDOP_SUBTRACT,
	BLENDOP_REVSUBTRACT,
	BLENDOP_MIN,
	BLENDOP_MAX,
};
//----------------------------------------------------------------

const char* SEnumName<SceneBlendOperation>::List[] =
{
	"ADD",
	"SUBTRACT",
	"REVSUBTRACT",
	"MIN",
	"MAX",
};
//----------------------------------------------------------------

enum SceneBlendFactor
{
	SBF_ONE,
	SBF_ZERO,
	SBF_DEST_COLOUR,
	SBF_SOURCE_COLOUR,
	SBF_ONE_MINUS_DEST_COLOUR,
	SBF_ONE_MINUS_SOURCE_COLOUR,
	SBF_DEST_ALPHA,
	SBF_SOURCE_ALPHA,
	SBF_ONE_MINUS_DEST_ALPHA,
	SBF_ONE_MINUS_SOURCE_ALPHA
};
//----------------------------------------------------------------

const char* SEnumName<SceneBlendFactor>::List[] =
{
	"ONE",
	"ZERO",
	"DEST_COLOUR",
	"SOURCE_COLOUR",
	"ONE_MINUS_DEST_COLOUR",
	"ONE_MINUS_SOURCE_COLOUR",
	"DEST_ALPHA",
	"SOURCE_ALPHA",
	"ONE_MINUS_DEST_ALPHA",
	"ONE_MINUS_SOURCE_ALPHA"
};
//----------------------------------------------------------------

/*typedef enum _BLEND {
BLEND_ZERO               = 1,
BLEND_ONE                = 2,
BLEND_SRCCOLOR           = 3,
BLEND_INVSRCCOLOR        = 4,
BLEND_SRCALPHA           = 5,
BLEND_INVSRCALPHA        = 6,
BLEND_DESTALPHA          = 7,
BLEND_INVDESTALPHA       = 8,
BLEND_DESTCOLOR          = 9,
BLEND_INVDESTCOLOR       = 10,
BLEND_SRCALPHASAT        = 11,
BLEND_BOTHSRCALPHA       = 12,
BLEND_BOTHINVSRCALPHA    = 13,
BLEND_BLENDFACTOR        = 14, / Only supported if PBLENDCAPS_BLENDFACTOR is on /
BLEND_INVBLENDFACTOR     = 15, / Only supported if PBLENDCAPS_BLENDFACTOR is on /
} BLEND;
*/
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

enum CullingMode
{
	CULL_NULL,
	CULL_NONE,
	CULL_CLOCK_WISE,
	CULL_ANTI_CLOCK_WISE
};
//----------------------------------------------------------------

const char* SEnumName<CullingMode>::List[] =
{
	"NULL",
	"NONE",
	"CLOCK_WISE",
	"ANTI_CLOCK_WISE"
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

enum TextureTransformFlag
{
	TTF_DISABLE         = 0,    // texture coordinates are passed directly
	TTF_COUNT1          = 1,    // rasterizer should expect 1-D texture coords
	TTF_COUNT2          = 2,    // rasterizer should expect 2-D texture coords
	TTF_COUNT3          = 3,    // rasterizer should expect 3-D texture coords
	TTF_COUNT4          = 4,    // rasterizer should expect 4-D texture coords
	TTF_PROJECTED       = 256,  // texcoords to be divided by COUNTth element
};
//----------------------------------------------------------------

enum TextureCoordIndex
{
	TCI_CAMERASPACE_NORMAL				= 0x00010000,
	TCI_CAMERASPACE_POSITION			= 0x00020000,
	TCI_CAMERASPACE_REFLECTION_VECTOR	= 0x00030000,
	TCI_SPHEREMAP						= 0x00040000,
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