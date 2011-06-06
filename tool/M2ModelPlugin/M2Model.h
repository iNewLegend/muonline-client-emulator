#pragma once
#include "ModelData.h"
#include "mpq/mpq_libmpq.h"

struct Lump //addressing
{
	uint32 count, offset;
};

struct CharModelDetails {
	bool closeRHand;
	bool closeLHand;

	bool isChar;
	bool isMounted;

	void Reset() {
		closeRHand = false;
		closeLHand = false;
		isChar = false;
		isMounted = false;
	}
};

//一个网格块
struct ModelGeoset
{
	uint16 id;		// mesh part id?
	IndexedSubset	subset;
	uint16 d3;		// number of bone indices
	uint16 d4;		// ? always 1 to 4
	uint16 d5;		// ?
	uint16 d6;		// root bone?
	Vec3D v;
};

// sub-block in block E - animation data
struct AnimationBlock {
	int16 type;		// interpolation type (0=none, 1=linear, 2=hermite)
	int16 seq;		// global sequence id or -1
	Lump aRanges;
	Lump aTimes;
	Lump aKeys;
};

struct ModelPass
{
	// probably the texture units
	// size always >=number of materials it seems
	ModelPass()
	{
		flags = 0;
		nOrder = 0;
		nGeosetID=0;
		nGeosetID2=0;
		nColorID= -1;
		nRenderFlag=0;
		nBlendMode=0;
		nTexunit=0;
		d4=1;
		nTextureID=0;
		nTexunit2=0;
		nTransID=0;
		nTexanimID=0;
		p=0;
	}
	uint16 flags;		// Flags
	uint16 nOrder;		// 渲染序列?
	uint16 nGeosetID;	// 网格部件索引 Material this texture is part of (index into mat)
	uint16 nGeosetID2;	// Always same as above?
	int16  nColorID;	// 颜色动画索引 color or -1 
	uint16 nRenderFlag;	//
	uint16 nBlendMode;	//
	uint16 nTexunit;	// Texture unit (0 or 1)
	uint16 d4;			// 多重纹理? (seems to be always 1)
	int16  nTextureID;	// 纹理索引 Texture id (index into global texture list)
	int16  nTexunit2;	// copy of texture unit value?
	int16  nTransID;	// 透明索引 transparency id (index into transparency list)
	int16  nTexanimID;	// 纹理动画索引 texture animation id
	float p;

	bool operator< (const ModelPass &m) const
	{
		// This is the old sort order method which I'm pretty sure is wrong - need to try something else.
		//return !trans;
		if (nOrder<m.nOrder)
			return true;
		else if (nOrder>m.nOrder)
			return false;
		else
			return nBlendMode == m.nBlendMode ? (p<m.p) : (nBlendMode<m.nBlendMode);
	}
};

enum eModelRenderFlag
{
	eMR_Unlit		= 1<<0,		// 无灯光
	eMR_NoZWrite	= 1<<1,		// 无深度写入 
	eMR_NoCull		= 1<<2,		// 不使用背面剔除
	eMR_Unknow3		= 1<<3,		// 
	eMR_UseEnvMap	= 1<<4,		// 纹理贴法环境反射贴图
	eMR_Unknow5		= 1<<5,		// 
	eMR_Unknow6		= 1<<6,		// 
	eMR_Unknow7		= 1<<7,		// 
	eMR_Unknow8		= 1<<8,		// 
	eMR_Unknow9		= 1<<9,		// 
	eMR_Unknow10	= 1<<10,	// 
	eMR_Unknow11	= 1<<11,	// 
	eMR_Unknow12	= 1<<12,	// 
	eMR_Unknow13	= 1<<13,	// 
	eMR_Unknow14	= 1<<14,	//
	eMR_Unknow15	= 1<<15,	// 
};

// block X - render flags
struct ModelRenderFlag {
	uint16 flags;
	//unsigned char f1;
	//unsigned char f2;
	uint16 blend;
};

struct ModelParticleParams {
	float mid;
	uint32 colors[3];
	float sizes[3];
	int16 d[10];
	float unk[3];
	float scales[3];
	float slowdown;
	float rotation;
	float f2[16];
};

struct ModelTextureDef
{
	uint32 type;
	uint32 flags;
	Lump aName;
};

// 模型顶点数据
struct M2ModelVertex {
	Vec3D pos;
	uint32 weights;
	uint32 bones;
	Vec3D normal;
	Vec2D texcoords;
	Vec2D texcoords2;
};

// block E - bones
struct ModelBoneDef
{
	int32 animid;
	int32 flags;
	int16 parent; // parent bone index
	int16 geoid;
	AnimationBlock translation;
	AnimationBlock rotation;
	AnimationBlock scaling;
	Vec3D pivot;
	void Init(MPQFile &f, BoneAnim &boneAnim, int *global);
};

// block G - color defs
struct ModelColorDef
{
	AnimationBlock color;
	AnimationBlock opacity;
	void Init(MPQFile &f, ColorAnim &colorAnim, int *global);
};

// block H - transp defs
struct ModelTransDef
{
	AnimationBlock trans;
	void Init(MPQFile &f, TransAnim& transAnim, int *global);
};

struct ModelTexAnimDef
{
	AnimationBlock trans, rot, scale;
	void Init(MPQFile &f, TexAnim& texAnim, int *global);
};

struct ModelLightDef
{
	int16 type;
	int16 bone;
	Vec3D pos;
	AnimationBlock ambColor;
	AnimationBlock ambIntensity;
	AnimationBlock color;
	AnimationBlock intensity;
	AnimationBlock attStart;
	AnimationBlock attEnd;
	AnimationBlock unk1;
	void Init(MPQFile &f, LightAnim &lightAnim, int *global);
};

struct ModelCameraDef
{
	int32 id;
	float fov, farclip, nearclip;
	AnimationBlock transPos;
	Vec3D pos;
	AnimationBlock transTarget;
	Vec3D target;
	AnimationBlock rot;
	void Init(MPQFile &f, ModelCamera & modelCamera, int *global);
};

struct ModelAttachmentDef
{
	int32 id;
	int32 bone;
	Vec3D pos;
	AnimationBlock unk;
	void Init(MPQFile &f, ModelAttachment &attachment, int *global);
};

struct ModelParticleEmitterDef
{
	int32 id;
	int32 flags;
	Vec3D pos;
	int16 bone;
	int16 texture;
	int32 nZero1;
	int32 ofsZero1;
	int32 nZero2;
	int32 ofsZero2;
	int16 blend;
	int16 type;
	int16 s1;
	int16 s2;
	int16 cols;
	int16 rows;
	AnimationBlock params[10];
	ModelParticleParams p;
	AnimationBlock en;
	void Init(MPQFile &f, CParticleEmitter &particleEmitter, int *globals);
};


struct ModelRibbonEmitterDef
{
	int32 id;
	int32 bone;
	Vec3D pos;
	Lump aTextures;
	int32 nUnknown;
	int32 ofsUnknown;
	AnimationBlock color;
	AnimationBlock opacity;
	AnimationBlock above;
	AnimationBlock below;
	float res, length, unk;
	int16 s1, s2;
	AnimationBlock unk1;
	AnimationBlock unk2;
	void Init(MPQFile &f, RibbonEmitter &ribbonEmitter, int *globals);
};

// 没有用到
struct ModelBlockQ
{
	char id[4];
	int32 dbid;
	int32 bone;
	Vec3D pos;
	int16 type;
	int16 seq;
	Lump aRanges;
	Lump aTimes;
};


// 渲染过程 A texture unit (sub of material)
struct M2ModelRenderPass
{
	// probably the texture units
	// size always >=number of materials it seems
	uint16 flags;		// Flags
	uint16 nOrder;		// 渲染序列?
	uint16 nGeosetID;	// 网格部件索引 Material this texture is part of (index into mat)
	uint16 nGeosetID2;	// Always same as above?
	int16  nColorID;	// 颜色动画索引 color or -1 
	uint16 nRenderFlagID;	// 渲染标识索引 more flags...
	uint16 nTexunit;	// Texture unit (0 or 1)
	uint16 d4;			// 多重纹理? (seems to be always 1)
	int16  nTextureID;	// 纹理索引 Texture id (index into global texture list)
	int16  nTexunit2;	// copy of texture unit value?
	int16  nTransID;	// 透明索引 transparency id (index into transparency list)
	int16  nTexanimID;	// 纹理动画索引 texture animation id
};

enum M2ModelLumpType
{
	M2_LUMP_GLOBAL_SEQUENCES,	// 全局序列ID号
	M2_LUMP_ANIM_ATIONS,			// 动画分组寻址
	M2_LUMP_C,					// C
	M2_LUMP_D,					// 总是-1？INT
	M2_LUMP_BONES,				// 骨骼寻址
	M2_LUMP_F,					// F
	M2_LUMP_VERTICES,			// 顶点数据寻址
	M2_LUMP_LODS,				// LOD
	M2_LUMP_COLORS,				// 颜色动画
	M2_LUMP_TEXTURES,			// 纹理
	M2_LUMP_TRANSES,			// 透明动画
	M2_LUMP_I,					// ALWAYS UNUSED ?
	M2_LUMP_TEX_ANIMS,			// 纹理动画
	M2_LUMP_TEX_REPLACES,		// 替换纹理用的 好像用法很BT？
	M2_LUMP_TEX_FLAGS,			// 渲染标识	
	M2_LUMP_Y,					// Y
	M2_LUMP_TEX_LOOKUP,			// 纹理查找
	M2_LUMP_TEX_UNIT_LOOKUP,	// L
	M2_LUMP_TRANS_LOOKUP,		// M
	M2_LUMP_TEX_ANIM_LOOKUP,		
	M2_LUMP_FLOAT_1,			// FLOAT FLOATS[14]
	M2_LUMP_FLOAT_2,
	M2_LUMP_FLOAT_3,
	M2_LUMP_FLOAT_4,
	M2_LUMP_FLOAT_5,
	M2_LUMP_FLOAT_6,
	M2_LUMP_FLOAT_7,
	M2_LUMP_BOUND_INDICES,		// 碰撞体
	M2_LUMP_BOUND_VERTICES,		// 碰撞体
	M2_LUMP_BOUND_NORMALS,		// 碰撞体
	M2_LUMP_ATTACH_MENTS,		// 换装
	M2_LUMP_ATTACH_LOOKUP,		// P
	M2_LUMP_Q,					// Q
	M2_LUMP_LIGHTS,				// 灯光
	M2_LUMP_CAMERAS,			// 摄像机
	M2_LUMP_T,					// T
	M2_LUMP_RIBBON_EMITTERS,	// 条带发射器
	M2_LUMP_PARTICLE_EMITTERS,	// 粒子发射器
	M2_LUMP_MAX,
};

struct M2ModelHeader {
	char id[4];
	uint8 version[4];
	// 名称寻址
	Lump aName;
	uint32 type;
	Lump lumps[M2_LUMP_MAX];
};

// 模型Lod寻址数据
struct ModelLodLookup {
	Lump aIndexLookup;	// 使用索引查找表可以优化骨骼动画（通过索引表去更新必要的顶点） Vertices in this model (index into vertices[])
	Lump aIndices;	// indices
	Lump aPropses;	// additional vtx properties
	Lump aSubs;		// materials/renderops/submeshes
	Lump aPasses;	// material properties/textures
	int32 nLevel;	// LOD bias?
};

struct TextureGroup {
	static const int num = 3;
	int base, count;
	std::string tex[3];
	TextureGroup()
	{
		for (int i=0; i<num; i++) {
			tex[i] = "";
		}
	}

	// default copy constr
	TextureGroup(const TextureGroup &grp)
	{
		for (int i=0; i<num; i++) {
			tex[i] = grp.tex[i];
		}
		base = grp.base;
		count = grp.count;
	}
	const bool operator<(const TextureGroup &grp) const
	{
		for (int i=0; i<num; i++) {
			//if (tex[i].length()==0||grp.tex[i].length()==0)
			//{
			//	if (tex[i].length() < grp.tex[i].length())return true;
			//}
			//else
			//{
			if (tex[i]<grp.tex[i]) return true;
			else if (tex[i]>grp.tex[i]) return false;
			//}

		}
		return false;
	}
};

class CM2Model: public CModelData
{
public:
	CM2Model();
	~CM2Model();

public:
	// 载入
	virtual bool LoadFile(const std::string& strFilename);

	void LoadVertices(MPQFile &f, const Lump& lump);
	void LoadBoundPos(MPQFile &f, const Lump& lump);
	void LoadBoundIndex(MPQFile &f, const Lump& lump);
	void LoadTexChannel(MPQFile &f, const Lump& lump);
	void LoadSkeleton(MPQFile &f, const Lump& lump);
	void LoadColorAnim(MPQFile &f, const Lump& lump);
	void LoadTransAnim(MPQFile &f, const Lump& lump);
	void LoadTexAnim(MPQFile &f, const Lump& lump);
	void LoadParticleEmitter(MPQFile &f, const Lump& lump);
	void LoadRibbonEmitter(MPQFile &f, const Lump& lump);
	void LoadCamera(MPQFile &f, const Lump& lump);
	void LoadLightAnim(MPQFile &f, const Lump& lump);
	void LoadAnimAtion(MPQFile &f, const Lump& lump);

	void InitSkins(const std::string& strFilename);
	void InitCommon(MPQFile &f);
	void InitAnimated(MPQFile &f);
	//// 保存
	//bool SaveToFile(std::string name);

	//void setupAtt(int id);
	//void setupAtt2(int id);

	//// 初始化皮肤
	//void InitSkins();
public:
	// 头信息 包含的数据比较多
	M2ModelHeader m_M2Header;

	int specialTextures[32];
	int replaceTextures[32];
	bool useReplaceTextures[32];

	//// 皮肤组 TextureGroups
	//std::vector<TextureGroup> m_skins;
};