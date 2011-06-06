#include "M2Model.h"
#include "database.h"
#include "RenderSystem.h"

#define NEW_POINTER_AT_BUFFER(_name, _type, _address)	_type *##_name = (_type##*)(f.getBuffer() + _address##.offset);
#define NEW_MEMCPY_FROM_BUFFER(_pointer, _type, _address) _pointer = new _type[ _address##.count ]; memcpy(_pointer, f.getBuffer() + _address##.offset, _address##.count*sizeof(_type));

inline Vec3D fixCoordSystem(Vec3D v)
{
	return Vec3D(v.x, v.z, -v.y);
	//return Vec3D(-v.y, v.z, -v.x);
}

inline Vec3D fixCoordSystem2(Vec3D v)
{
	return Vec3D(v.x, v.z, v.y);
	//return Vec3D(v.y, v.z, v.x);
}

inline Quaternion fixCoordSystemQuat(Quaternion v)
{
	//return Quaternion(-v.x, -v.z, v.y, v.w);
	return Quaternion(v.x, v.z, -v.y, -v.w);
	//return Quaternion(v.y, -v.z, v.x, v.w);
}

template <typename T >
inline void AnimatedInit(Animated<T>& animated, MPQFile& f, AnimationBlock &b, int *gs)
{
	animated.globals = gs;
	animated.type = b.type;
	animated.seq = b.seq;
	if (animated.seq!=-1) {
		assert(gs);
	}
	// 数据为空 则无效
	if (b.aKeys.count > 0)
	{
		// FrameRanges
		//if (b.aRanges.count > 0)
		//{
		//	FrameRange *pRanges = (FrameRange*)(f.getBuffer() + b.aRanges.offset);
		//	for (size_t i=0; i<b.aRanges.count; i++)
		//	{
		//		animated.m_FrameRanges.push_back(pRanges[i]);
		//	}
		//}
		//else if (animated.type!=INTERPOLATION_NONE && animated.seq==-1)
		//{
		//	FrameRange r;
		//	r.begin = 0;
		//	r.end = b.aKeys.count - 1;
		//	animated.m_FrameRanges.push_back(r);
		//}

		// times
		assert(b.aTimes.count == b.aKeys.count);
		uint32 *ptimes = (uint32*)(f.getBuffer() + b.aTimes.offset);
		for (size_t i=0; i<b.aTimes.count; i++)
		{
			animated.m_KeyTimes.push_back(ptimes[i]);
		}

		// keyframes
		assert((T*)(b.aKeys.offset));
		T *keys = (T*)(f.getBuffer() + b.aKeys.offset);

		for (size_t i=0; i<b.aKeys.count; i++) 
		{
			animated.m_KeyData.push_back(keys[i]);
		}

		if (animated.m_KeyData.size()==0) 
			animated.m_KeyData.push_back(T());
	}
}

CM2Model::CM2Model()
{
	for (int i=0; i<32; i++)
	{
		specialTextures[i] = -1;
		replaceTextures[i] = 0;
		useReplaceTextures[i] = false;
	}
}

CM2Model::~CM2Model()
{
}

void CM2Model::LoadVertices(MPQFile &f, const Lump& lump)
{
	if (lump.count > 0)
	{
		NEW_POINTER_AT_BUFFER(origVertices, M2ModelVertex, lump)
		m_Mesh.pos.resize(lump.count);
		m_Mesh.normal.resize(lump.count);
		m_Mesh.texcoord.resize(lump.count);
		m_Mesh.weight.resize(lump.count);
		m_Mesh.bone.resize(lump.count);
		for (uint32 i=0; i<lump.count; i++)
		{
			m_Mesh.pos[i] = fixCoordSystem(origVertices[i].pos);
			m_Mesh.normal[i] = fixCoordSystem(origVertices[i].normal);
			m_Mesh.texcoord[i] = origVertices[i].texcoords;
			m_Mesh.weight[i] = origVertices[i].weights;
			m_Mesh.bone[i] = origVertices[i].bones;
		}
		m_Mesh.update();
	}
}

bool CM2Model::LoadFile(const std::string& strFilename)
{
	// replace .MDX with .M2
	// 用M2代替MDX
	std::string strTempname = strFilename;
	if (strFilename.length()>4)
	{
		if (strTempname[strFilename.length()-1] != '2')
		{
			strTempname[strFilename.length()-2] = '2';
			strTempname[strFilename.length()-1] = 0;
		}
	}

	// 打开mpq文件
	MPQFile f(strTempname.c_str(),false);
	if (f.isEof() || (f.getSize() < sizeof(M2ModelHeader))) {
		// 无法载入模型
	//	MessageBox(NULL,L"Error: Unable to load model: [%s].", L"d",1);
		delete this;
		f.close();
		return false;
	}

	// 获取模型头
	memcpy(&m_M2Header, f.getBuffer(), sizeof(m_M2Header));

	// 检查文件大小正确
	if (f.getSize() < m_M2Header.lumps[M2_LUMP_PARTICLE_EMITTERS].offset) {
	//	MessageBox(NULL,L"Error: Model '%s' appears to be corrupted.", L"d",1);
	}

	if (m_M2Header.lumps[M2_LUMP_GLOBAL_SEQUENCES].count)
	{
		NEW_MEMCPY_FROM_BUFFER(globalSequences, int, m_M2Header.lumps[M2_LUMP_GLOBAL_SEQUENCES])
	}

	// 原顶点数据copy
	LoadVertices(f, m_M2Header.lumps[M2_LUMP_VERTICES]);

	//
	InitCommon(f);
	InitSkins(strFilename);


	InitAnimated(f);// 初始化动画

	// mpq 文件关闭
	f.close();

	//D3DXCreateEffectFromFile(GetRenderSystem().GetDevice(),L"model.fx", NULL, NULL, dwShaderFlags, 

	return true;
}

void CM2Model::InitSkins(const std::string& strFilename)
{
	std::string fn(strFilename);
	// replace M2 with MDX
	//if (fn[fn.length()-1] == '2') {
		fn[fn.length()-1] = 'd';
		fn.append("x");
	//}
	// textureGroups


	std::vector<TextureGroup> Skins;

	if (1)
	{
		try
		{
			CreatureModelDB::Record rec = modeldb.getByFilename(fn);
			// for character models, don't use skins
			if (rec.getUInt(CreatureModelDB::Type) != 4) {
				//TextureSet skins;
				unsigned int modelid = rec.getUInt(CreatureModelDB::ModelID);

				std::set<TextureGroup> skins;
				for (CreatureSkinDB::Iterator it = skindb.begin();  it!=skindb.end();  ++it)
				{
					if (it->getUInt(CreatureSkinDB::ModelID) == modelid)
					{
						TextureGroup grp;
						for (int i=0; i<TextureGroup::num; i++)
						{
							//const char *skin = it->getString(CreatureSkinDB::Skin + i);
							std::string skin(it->getString(CreatureSkinDB::Skin + i));
							grp.tex[i] = skin;
						}
						grp.base = 11;
						grp.count = 3;
						if (grp.tex[0].length() > 0) 
							skins.insert(grp);
					}
				}

				// Hard coded skin additions - missing from DBC ?
				if (fn == "Creature\\Furbolg\\furbolg.mdx") {
					TextureGroup grp;
					grp.tex[0] = "FurbolgSkinPanda";
					grp.tex[1] = "FurbolgStuffWhite";
					grp.base = 11;
					grp.count = 2;
					skins.insert(grp);
				} else if (fn == "Creature\\Murloc\\babymurloc.mdx") {
					TextureGroup grp;
					grp.tex[0] = "MurlocBabyBlue";
					grp.tex[1] = "TopHat";
					grp.tex[2] = "MurlocBabyReflect";
					grp.base = 11;
					grp.count = 3;
					skins.insert(grp);
					grp.tex[0] = "MurlocBabyGreen";
					skins.insert(grp);
					grp.tex[0] = "MurlocBabyOrange";
					skins.insert(grp);
					grp.tex[0] = "MurlocBabyPink";
					skins.insert(grp);
					grp.tex[0] = "MurlocBabyPurple";
					skins.insert(grp);
					grp.tex[0] = "MurlocBabyWhite";
					skins.insert(grp);
				} else if (fn == "Creature\\Felbeast\\felbeast.mdx") {
					TextureGroup grp;
					grp.tex[0] = "FelBeastSkinGreenBlack";
					grp.base = 11;
					grp.count = 1;
					skins.insert(grp);
				} else if (fn == "Creature\\Gryphon\\gryphon.mdx") {
					TextureGroup grp;
					grp.tex[0] = "Gryphon_Skin01Black";
					grp.tex[1] = "Gryphon_Skin02Black";
					grp.base = 11;
					grp.count = 2;
					skins.insert(grp);
					grp.tex[0] = "Gryphon_Skin01White";
					grp.tex[1] = "Gryphon_Skin02White";
					skins.insert(grp);
				} else if (fn == "Creature\\Lasher\\lasher.mdx") {
					TextureGroup grp;
					grp.tex[0] = "LasherSkinBlue";
					grp.base = 11;
					grp.count = 1;
					skins.insert(grp);
					grp.tex[0] = "LasherSkinGreen";
					skins.insert(grp);
					grp.tex[0] = "LasherSkinPurple";
					skins.insert(grp);
				} else if (fn == "Creature\\Minespider\\minespider.mdx") {
					TextureGroup grp;
					grp.tex[0] = "MineSpiderSkinViolet";
					grp.base = 11;
					grp.count = 1;
					skins.insert(grp);
				} else if (fn == "Creature\\Owl\\owl.mdx") {
					TextureGroup grp;
					grp.tex[0] = "OwlSpirit";
					grp.base = 11;
					grp.count = 1;
					skins.insert(grp);
				} else if (fn == "Creature\\Parrot\\parrot.mdx") {
					TextureGroup grp;
					grp.tex[0] = "ParrotSkinBrown";
					grp.base = 11;
					grp.count = 1;
					skins.insert(grp);
					grp.tex[0] = "ParrotSkinCrimson";
					skins.insert(grp);
					grp.tex[0] = "ParrotSkinlavender";
					skins.insert(grp);
				} else if (fn == "Creature\\Pterrordax\\pterrordax.mdx") {
					TextureGroup grp;
					grp.tex[0] = "PterrordaxSkinBrown";
					grp.tex[1] = "PterrordaxWingSkinBrown";
					grp.base = 11;
					grp.count = 2;
					skins.insert(grp);
				} else if (fn == "Creature\\Silithidscarab\\silithidscarab.mdx") {
					TextureGroup grp;
					grp.tex[0] = "SilithidScarabSkinGreen";
					grp.base = 11;
					grp.count = 1;
					skins.insert(grp);
				} else if (fn == "Creature\\Silithidflyer\\silithidflyer.mdx") {
					TextureGroup grp;
					grp.tex[0] = "SilithidFlyerSkinBlack";
					grp.base = 11;
					grp.count = 1;
					skins.insert(grp);
					grp.tex[0] = "SilithidFlyerSkinOrange";
					skins.insert(grp);
				}

				// hard-coded fixes.
				if (skins.empty() == true) {
					if (fn == "Creature\\Dwarfmalewarriorlight\\dwarfmalewarriorlight.mdx") {
						TextureGroup grp;
						grp.tex[0] = "DwarfMaleWarriorLightSkin";
						grp.base = 11;
						grp.count = 1;
						skins.insert(grp);
						grp.tex[0] = "DwarfMaleWarriorLightSkin2";
						grp.base = 11;
						grp.count = 1;
						skins.insert(grp);
					} else if (fn == "Creature\\Faeriedragon\\faeriedragon_ghost.mdx") {
						TextureGroup grp;
						grp.tex[0] = "FaerieDragon_Ghost";
						grp.tex[1] = "faeriewing_Ghost";
						grp.base = 11;
						grp.count = 2;
						skins.insert(grp);
					} else if (fn == "Creature\\Felbat\\felbat.mdx") {
						TextureGroup grp;
						grp.tex[0] = "BatSkin01";
						grp.tex[1] = "BatSkin02";
						grp.base = 11;
						grp.count = 2;
						skins.insert(grp);
						grp.tex[0] = "BatSkinBrown01";
						grp.tex[1] = "BatSkinBrown02";
						grp.base = 11;
						grp.count = 2;
						skins.insert(grp);
						grp.tex[0] = "BatSkinViolet01";
						grp.tex[1] = "BatSkinViolet02";
						grp.base = 11;
						grp.count = 2;
						skins.insert(grp);
						grp.tex[0] = "BatSkinWhite01";
						grp.tex[1] = "BatSkinWhite02";
						grp.base = 11;
						grp.count = 2;
						skins.insert(grp);
					} else if (fn == "Creature\\Highelf\\highelfmale_priest.mdx") {
						TextureGroup grp;
						grp.tex[0] = "HighElfMalePriest";
						grp.base = 11;
						grp.count = 1;
						skins.insert(grp);
					} else if (fn == "Creature\\Orcmalewarriorheavy\\orcmalewarriorheavy.mdx") {
						TextureGroup grp;
						grp.tex[1] = "OrcMaleSamuHairSkin";
						grp.tex[0] = "OrcNPCSkin";
						grp.base = 11;
						grp.count = 2;
						skins.insert(grp);
					} else if (fn == "Creature\\Seagiant\\seagiant.mdx") {
						TextureGroup grp;
						grp.tex[0] = "SeaGiantSkin01";
						grp.tex[1] = "SeaGiantSkin02";
						grp.base = 11;
						grp.count = 2;
						skins.insert(grp);
					} else if (fn == "Creature\\Sealion\\sealion.mdx") {
						TextureGroup grp;
						grp.tex[0] = "SeaLionSkin";
						grp.base = 11;
						grp.count = 1;
						skins.insert(grp);
					} else if (fn == "Creature\\Stormcrow\\stormcrow.mdx") {
						TextureGroup grp;
						grp.tex[0] = "StormCrowSkin";
						grp.base = 11;
						grp.count = 1;
						skins.insert(grp);
					} else if(fn == "Creature\\Humanmalewizard\\humanmalewizard.mdx") {
						TextureGroup grp;
						grp.tex[0] = "HumanMaleMerchant01Skin";
						grp.base = 11;
						grp.count = 1;
						skins.insert(grp);
						grp.tex[0] = "HumanMaleMerchant02Skin";
						grp.base = 11;
						grp.count = 1;
						skins.insert(grp);
					} 
				}

				// 从set中导到表中
				for (std::set<TextureGroup>::iterator it = skins.begin(); it != skins.end(); ++it)
				{
					Skins.push_back(*it);
				}
			}
		}catch (CreatureModelDB::NotFound) {
			// Try hardcoding some fixes for missing model info from the DBC
			if(fn == "Creature\\Dwarfmalewarriorlight\\dwarfmalewarriorlight_ghost.mdx") {
				TextureGroup grp;
				grp.tex[0] = "DwarfMaleWarriorLightSkin_Ghost";
				grp.base = 11;
				grp.count = 1;
				Skins.push_back(grp);
			} else if(fn == "Creature\\Mounteddemonknight\\mounteddemonknight.mdx") {
				TextureGroup grp;
				grp.tex[0] = "MountedDemonKnightBlack_01";
				grp.tex[1] = "MountedDemonKnightBlack_02";
				grp.tex[2] = "MountedDemonKnightBlack_03";
				grp.base = 11;
				grp.count = 3;
				Skins.push_back(grp);
			} else if(fn == "Creature\\Orcfemalewarriorlight\\orcfemale.mdx") {
				TextureGroup grp;
				grp.tex[0] = "OrcFemaleSkin";
				//grp.tex[1] = "OrcFemaleSkin";
				grp.base = 11;
				grp.count = 1;
				Skins.push_back(grp);
			} else if(fn == "Creature\\Tigon\\tigon.mdx") {
				TextureGroup grp;
				grp.tex[0] = "TIGONSKIN_ORANGE";
				grp.tex[1] = "TIGONEYEGLOW";
				grp.base = 11;
				grp.count = 2;
				Skins.push_back(grp);
				grp.tex[0] = "CatManSkin_Orange";
				grp.tex[1] = "TIGONEYEGLOW";
				grp.base = 11;
				grp.count = 2;
				Skins.push_back(grp);
				grp.tex[0] = "CatManSkin_Black";
				grp.tex[1] = "TIGONEYEGLOW";
				grp.base = 11;
				grp.count = 2;
				Skins.push_back(grp);
			} else if(fn == "Creature\\Humanmalepiratecaptain\\humanmalepiratecaptain_ghost.mdx") {
				TextureGroup grp;
				grp.tex[0] = "PirateCaptain02_Ghost";
				grp.base = 11;
				grp.count = 1;
				Skins.push_back(grp);
			} else if(fn == "Creature\\Humanmalepiratecrewman\\humanmalepiratecrewman_ghost.mdx") {
				TextureGroup grp;
				grp.tex[0] = "PirateCrewman01_Ghost";
				grp.base = 11;
				grp.count = 1;
				Skins.push_back(grp);
				grp.tex[0] = "PirateCrewman02_Ghost";
				grp.base = 11;
				grp.count = 1;
				Skins.push_back(grp);
				grp.tex[0] = "PirateCrewman03_Ghost";
				grp.base = 11;
				grp.count = 1;
				Skins.push_back(grp);
			}
		}
	}
	// 模型皮肤初始化
	/*m_Skins.resize(Skins.size());
	for (uint32 nSkinID = 0 ; nSkinID < Skins.size(); nSkinID++)
	{
		TextureGroup &texGrp = Skins[nSkinID];
		for (int i=0; i<texGrp.count; i++)
		{
			if (useReplaceTextures[texGrp.base+i])
			{
				if (texGrp.tex[i].length() > 0)
				{
					int p = strFilename.find_last_of('\\');
					std::string strTex(strFilename,0,p+1);
					strTex.append(texGrp.tex[i]);
					strTex.append(".blp");
					int nTexID = GetRenderSystem().GetTextureMgr().RegisterTexture(strTex);
					for (uint32 nTexCount = 0; nTexCount < m_M2Header.lumps[M2_LUMP_TEXTURES].count; nTexCount++)
					{
						if (specialTextures[nTexCount] == texGrp.base+i)
						{
							m_Skins[nSkinID].RepTex[nTexCount] = m_TexChannels[nTexCount].Texs.size();
							m_TexChannels[nTexCount].Texs.push_back(nTexID);// 添加到纹理通道中
						}
					}
				}
			}
		}
	}*/

	/*if (m_Skins.size() == 0)
	{
		// 默认皮肤
		m_Skins.resize(1);
		for (uint32 nTexCount = 0; nTexCount < m_M2Header.lumps[M2_LUMP_TEXTURES].count; nTexCount++)
		{
			if (m_TexChannels[nTexCount].Texs.size() > 0)
			{
				m_Skins[0].RepTex[nTexCount] = 0;
			}
		}
	}*/
}

void CM2Model::LoadBoundPos(MPQFile &f, const Lump& lump)
{
	if (lump.count > 0)
	{
		m_BoundMesh.pos.resize(lump.count);
		NEW_POINTER_AT_BUFFER(b, Vec3D, lump);
		for (size_t i=0; i<lump.count; i++)
		{
			m_BoundMesh.pos[i] = fixCoordSystem(b[i]);
		}
	}
}

void CM2Model::LoadBoundIndex(MPQFile &f, const Lump& lump)
{
	if (lump.count > 0)
	{
		m_BoundMesh.indices.resize(lump.count);
		memcpy(&m_BoundMesh.indices[0], f.getBuffer() + lump.offset, m_BoundMesh.indices.size()*sizeof(uint16));
	}
}

void CM2Model::LoadTexChannel(MPQFile &f, const Lump& lump)
{
	/*if (lump.count)
	{
		NEW_POINTER_AT_BUFFER(texdef, ModelTextureDef, lump);
		m_TexChannels.clear();
		for (size_t i=0; i<lump.count; i++) {
			char texname[256];
			if (texdef[i].type == 0) {
				strncpy(texname, (const char*)f.getBuffer() + texdef[i].aName.offset, texdef[i].aName.count);
				texname[texdef[i].aName.count] = 0;
				// 添加到该纹理通道的纹理索引数据中
				int nTexID = GetRenderSystem().GetTextureMgr().RegisterTexture(texname);
				m_TexChannels[i].Texs.push_back(nTexID);
			} else {
				//while (texdef[i].type < 16 && specialTextures[texdef[i].type]!=-1) texdef[i].type++;
				//if (texdef[i].type < 16)specialTextures[texdef[i].type] = (int)i;
				specialTextures[i] = texdef[i].type;
				useReplaceTextures[texdef[i].type] = true;
			}
		}
	}*/
}

void CM2Model::InitCommon(MPQFile &f)
{
	// Bounds
	LoadBoundPos(f, m_M2Header.lumps[M2_LUMP_BOUND_VERTICES]);
	LoadBoundIndex(f, m_M2Header.lumps[M2_LUMP_BOUND_INDICES]);
	LoadTexChannel(f, m_M2Header.lumps[M2_LUMP_TEXTURES]);
	// replacable textures - it seems to be better to get this info from the texture types
	//if (m_M2Header.lumps[M2_LUMP_TexReplaces.count) {
	//	size_t m = m_M2Header.lumps[M2_LUMP_TexReplaces.count;
	//	if (m>16) m = 16;
	//	int16 *texrep = (int16*)(f.getBuffer() + m_M2Header.lumps[M2_LUMP_TexReplaces.offset);
	//	for (size_t i=0; i<m; i++) specialTextures[i] = texrep[i];
	//}


	// 附属装备初始化 attachments
	if (m_M2Header.lumps[M2_LUMP_ATTACH_MENTS].count) {
		NEW_POINTER_AT_BUFFER(attachments, ModelAttachmentDef, m_M2Header.lumps[M2_LUMP_ATTACH_MENTS]);
		for (size_t i=0; i<m_M2Header.lumps[M2_LUMP_ATTACH_MENTS].count; i++)
		{
			ModelAttachment att;
			att.model = this;
			attachments[i].Init(f, att, globalSequences);
			atts.push_back(att);
		}
	}

	if (m_M2Header.lumps[M2_LUMP_ATTACH_LOOKUP].count) {
		NEW_POINTER_AT_BUFFER(p, int16, m_M2Header.lumps[M2_LUMP_ATTACH_LOOKUP]);
		for (size_t i=0; i<m_M2Header.lumps[M2_LUMP_ATTACH_LOOKUP].count; i++) {
			attLookup[i] = p[i];
		}
	}
	NEW_POINTER_AT_BUFFER(transLookup, int16, m_M2Header.lumps[M2_LUMP_TRANS_LOOKUP]);
	// 获取渲染标识
	NEW_POINTER_AT_BUFFER(pRenderFlags, ModelRenderFlag, m_M2Header.lumps[M2_LUMP_TEX_FLAGS]);

	// TODO: Add support for selecting the LOD.
	// 重置LODs大小
	m_Mesh.m_Lods.resize(1);//m_M2Header.lumps[M2_LUMP_LODS].count
	for (uint32 i = 0;i<1&& i < m_M2Header.lumps[M2_LUMP_LODS].count; i++)
	{
		// 当前的LodLookup
		NEW_POINTER_AT_BUFFER(lodLookup, ModelLodLookup, sizeof(ModelLodLookup)*i + m_M2Header.lumps[M2_LUMP_LODS]);

		ModelLod& modelLod = m_Mesh.m_Lods[i];

		// 索引赋值
		if (lodLookup->aIndexLookup.count > 0)
		{
			modelLod.IndexLookup.resize(lodLookup->aIndexLookup.count);
			memcpy(&modelLod.IndexLookup[0], f.getBuffer() + lodLookup->aIndexLookup.offset, lodLookup->aIndexLookup.count*sizeof(uint16));
		}
		if (lodLookup->aIndices.count > 0)
		{
			modelLod.Indices.resize(lodLookup->aIndices.count);
			memcpy(&modelLod.Indices[0], f.getBuffer() + lodLookup->aIndices.offset, lodLookup->aIndices.count*sizeof(uint16));
		}

		std::vector<ModelGeoset> setGeoset;
		if (lodLookup->aSubs.count > 0)
		{
			setGeoset.resize(lodLookup->aSubs.count);
			memcpy(&setGeoset[0], f.getBuffer() + lodLookup->aSubs.offset, lodLookup->aSubs.count*sizeof(ModelGeoset));

			for (std::vector<ModelGeoset>::iterator it=setGeoset.begin();it!=setGeoset.end();it++)
			{
				modelLod.setSubset.push_back(it->subset);
			}
		}

		NEW_POINTER_AT_BUFFER(passes, M2ModelRenderPass, lodLookup->aPasses)
		NEW_POINTER_AT_BUFFER(texLookup, uint16, m_M2Header.lumps[M2_LUMP_TEX_LOOKUP])
		NEW_POINTER_AT_BUFFER(texanimLookup, uint16, m_M2Header.lumps[M2_LUMP_TEX_ANIM_LOOKUP])
		NEW_POINTER_AT_BUFFER(texunitLookup, uint16, m_M2Header.lumps[M2_LUMP_TEX_UNIT_LOOKUP])

		m_mapPasses.clear();
		m_bHasAlphaTex = false;
		for (uint32 j = 0; j < lodLookup->aPasses.count; j++)
		{
			ModelRenderPass& pass = m_mapPasses[j];
			//pass.flags = passes[j].flags;
			pass.nOrder = passes[j].nOrder;
			pass.nSubID = passes[j].nGeosetID;
			//pass.nGeosetID2 = passes[j].nGeosetID2;
			pass.nColorID = passes[j].nColorID;
			uint16 RenderFlag = pRenderFlags[passes[j].nRenderFlagID].flags;
			pass.nBlendMode = pRenderFlags[passes[j].nRenderFlagID].blend;

			// 纹理寻址
			pass.bSwrap = (RenderFlag & 1) != 0;
			pass.bTwrap = (RenderFlag & 2) != 0;

			// (pass.blendmode>1) && !(rf.blend==4 && rf.flags==17);
			pass.bNoZWrite = /*(RenderFlag & eMR_NoZWrite)&&*/(pass.nBlendMode >1) && !(pass.nBlendMode==4 && RenderFlag==17);
			pass.bTrans =  (pass.nBlendMode >0);// && (pass.opacity>0);
			pass.bUnlit = RenderFlag & eMR_Unlit;

			pass.bHasAlphaTex = (0!=(pass.nBlendMode & (BM_TRANSPARENT|BM_ALPHA_BLEND|BM_ADDITIVE_ALPHA)));
			m_bHasAlphaTex |= pass.bHasAlphaTex;

			//pass.nTexunit = passes[j].nTexunit;
			//pass.d4 = passes[j].d4;
			//pass.nTexunit2 = passes[j].nTexunit2;


			//pass.nTexID = texLookup[passes[j].nTextureID];
			pass.nTransID = transLookup[passes[j].nTransID];
			pass.nTexanimID = texanimLookup[passes[j].nTexanimID];
			if (1)
			{
				if (RenderFlag & 15)
				{
					pass.nTexanimID = -1; // no texture animation
				} else {
					pass.nTexanimID = transLookup[passes[j].nTransID];
				}
			} else {
				pass.nTexanimID = -1; // no texture animation
			}

			pass.p = setGeoset[passes[j].nGeosetID].v.z;

			pass.bUseEnvMap = (RenderFlag & eMR_UseEnvMap)&&(pass.nBlendMode >2);

			pass.bCull = (RenderFlag & 0x04)==0 && pass.nBlendMode!=1 && pass.nBlendMode!=4 && (RenderFlag & 17)!=17;


		/*	switch (pass.nBlendMode)
			{
			case BM_OPAQUE:	// 0不透明物
				pass.material.uDiffuse = pass.nTexID;
				//R.SetTextureAlphaOP(0, TBOP_DISABLE);
				break;
			case BM_TRANSPARENT: // 1透明镂空
				pass.material.bAlphaTest = true;
				pass.material.uDiffuse = pass.nTexID;
				//R.SetTextureAlphaOP(0, TBOP_DISABLE);
				break;
			case BM_ALPHA_BLEND: // 2Alpha混合
				pass.material.bBlend = true;
				pass.material.uDiffuse = pass.nTexID;
				//R.SetBlendFunc(true, BLENDOP_ADD, SBF_SOURCE_ALPHA, SBF_ONE_MINUS_SOURCE_ALPHA);
				//R.SetTextureAlphaOP(0, TBOP_MODULATE, TBS_TEXTURE, TBS_TFACTOR);
				break;
			case BM_ADDITIVE: // 3颜色叠加
				pass.material.uEmissive = pass.nTexID;
			//	R.SetBlendFunc(true, BLENDOP_ADD, SBF_SOURCE_COLOUR, SBF_ONE);
			//	R.SetTextureAlphaOP(0, TBOP_DISABLE);
				break;
			case BM_ADDITIVE_ALPHA: // 4颜色Alpha叠加
				pass.material.uEmissive = pass.nTexID;
			//	R.SetBlendFunc(true, BLENDOP_ADD, SBF_SOURCE_ALPHA, SBF_ONE);
			//	R.SetTextureAlphaOP(0, TBOP_MODULATE, TBS_TEXTURE, TBS_TFACTOR);
				break;
			case BM_MODULATE:	// 5颜色混合
				pass.material.uReflection = pass.nTexID;
				//R.SetBlendFunc(true, BLENDOP_ADD, SBF_DEST_COLOUR, SBF_ZERO);
				//R.SetTextureAlphaOP(0, TBOP_DISABLE);
				break;
			case BM_MODULATEX2:	// 6颜色混合x2
				pass.material.uReflection = pass.nTexID;
				//R.SetBlendFunc(true, BLENDOP_ADD, SBF_DEST_COLOUR, SBF_SOURCE_COLOUR);
				//R.SetTextureAlphaOP(0, TBOP_DISABLE);
				break;
			default:// 默认：Alpha混合
				assert(0);
				//R.SetBlendFunc(true, BLENDOP_ADD, SBF_SOURCE_ALPHA, SBF_ONE_MINUS_SOURCE_ALPHA);
			}
*/
			
			pass.material.bCull = (RenderFlag & 0x04)==0 && pass.nBlendMode!=1 && pass.nBlendMode!=4 && (RenderFlag & 17)!=17;
			//pass.nTextureID = texLookup[pass.nTextureID];
			//pass.nTransID = transLookup[pass.nTransID];
			//pass.nTexanimID = texanimLookup[pass.nTexanimID];
			//pass.nTransID = 0
		}
		//sort(m_Passes.begin(), m_Passes.end());
	}
}

void CM2Model::LoadSkeleton(MPQFile &f, const Lump& lump)
{
	if (lump.count)
	{
		m_Skeleton.m_BoneAnims.resize(lump.count);
		NEW_POINTER_AT_BUFFER(mb, ModelBoneDef, lump);
		for (size_t i=0; i<lump.count; i++)
		{
			//if (i==0) mb[i].rotation.aRanges.offset = 1.0f;
			mb[i].Init(f, m_Skeleton.m_BoneAnims[i], globalSequences);
		}
	}
}

void CM2Model::LoadColorAnim(MPQFile &f, const Lump& lump)
{
	if (lump.count)
	{
		m_ColorAnims.resize(lump.count);
		NEW_POINTER_AT_BUFFER(colorDefs, ModelColorDef, lump);
		for (size_t i=0; i<lump.count; i++)
		{
			colorDefs[i].Init(f, m_ColorAnims[i], globalSequences);
		}
	}
}

void CM2Model::LoadTransAnim(MPQFile &f, const Lump& lump)
{
	if (lump.count)
	{
		m_TransAnims.resize(lump.count);
		NEW_POINTER_AT_BUFFER(trDefs, ModelTransDef, lump);
		for (size_t i=0; i<lump.count; i++)
		{
			trDefs[i].Init(f, m_TransAnims[i], globalSequences);
		}
	}
}

void CM2Model::LoadTexAnim(MPQFile &f, const Lump& lump)
{
	if (lump.count)
	{
		m_TexAnims.resize(lump.count);
		NEW_POINTER_AT_BUFFER(ta, ModelTexAnimDef, lump);
		for (size_t i=0; i<lump.count; i++)
		{
			ta[i].Init(f, m_TexAnims[i], globalSequences);
		}
	}
}

void CM2Model::LoadParticleEmitter(MPQFile &f, const Lump& lump)
{
	if (lump.count)
	{
		ModelParticleEmitterDef *pdefs = (ModelParticleEmitterDef *)(f.getBuffer() + lump.offset);
		m_setParticleEmitter.resize(lump.count);
		for (size_t i = 0; i <lump.count; i++)
		{
			pdefs[i].Init(f, m_setParticleEmitter[i], globalSequences);
		}
	}
}

void CM2Model::LoadRibbonEmitter(MPQFile &f, const Lump& lump)
{
	if (lump.count)
	{
		//ModelRibbonEmitterDef *rdefs = (ModelRibbonEmitterDef *)(f.getBuffer() + lump.offset);
		//ribbons = new RibbonEmitter[lump.count];
		//for (size_t i=0; i<lump.count; i++) {
		//	ribbons[i].model = this;
		//	ribbons[i].Init(f, rdefs[i], globalSequences);
		//}
	}
}

void CM2Model::LoadCamera(MPQFile &f, const Lump& lump)
{
	if (lump.count)
	{
		NEW_POINTER_AT_BUFFER(camDefs, ModelCameraDef, lump);
		camDefs[0].Init(f, m_Camera, globalSequences);
	}
}

void CM2Model::LoadLightAnim(MPQFile &f, const Lump& lump)
{
	if (lump.count)
	{
		m_LightAnims.resize(lump.count);
		NEW_POINTER_AT_BUFFER(lDefs, ModelLightDef, lump);
		for (uint32 i = 0; i < m_LightAnims.size(); i++)
		{
			lDefs[i].Init(f, m_LightAnims[i], globalSequences);
		}
	}
}

void CM2Model::LoadAnimAtion(MPQFile &f, const Lump& lump)
{
	if (lump.count)
	{
		m_AnimList.resize(lump.count);
		memcpy(&m_AnimList[0], f.getBuffer() + lump.offset, lump.count*sizeof(ModelAnimation));
	}
}

void CM2Model::InitAnimated(MPQFile &f)
{
	//动画信息的初始化
	LoadSkeleton(f, m_M2Header.lumps[M2_LUMP_BONES]);

	// 骨骼查询表
	NEW_POINTER_AT_BUFFER(p, int16, m_M2Header.lumps[M2_LUMP_F]);
	for (size_t i=0; i<m_M2Header.lumps[M2_LUMP_F].count; i++) {
		boneLookup[i] = p[i];
	}

	LoadColorAnim(f, m_M2Header.lumps[M2_LUMP_COLORS]);
	LoadTransAnim(f, m_M2Header.lumps[M2_LUMP_TRANSES]);
	LoadTexAnim(f, m_M2Header.lumps[M2_LUMP_TEX_ANIMS]);
	LoadParticleEmitter(f, m_M2Header.lumps[M2_LUMP_PARTICLE_EMITTERS]);
	LoadRibbonEmitter(f, m_M2Header.lumps[M2_LUMP_RIBBON_EMITTERS]);
	LoadCamera(f, m_M2Header.lumps[M2_LUMP_CAMERAS]);
	LoadLightAnim(f, m_M2Header.lumps[M2_LUMP_LIGHTS]);
	LoadAnimAtion(f, m_M2Header.lumps[M2_LUMP_ANIM_ATIONS]);
}

void ModelCameraDef::Init(MPQFile &f, ModelCamera & modelCamera, int *global)
{
	modelCamera.ok = true;
	modelCamera.nearclip = nearclip;
	modelCamera.farclip = farclip;
	modelCamera.fov = fov;
	modelCamera.pos = fixCoordSystem(pos);
	modelCamera.target = fixCoordSystem(target);

	AnimatedInit(modelCamera.tPos,		f, transPos, global);
	AnimatedInit(modelCamera.tTarget,	f, transTarget, global);
	AnimatedInit(modelCamera.rot,		f, rot, global);
	modelCamera.tPos.fix(fixCoordSystem);
	modelCamera.tTarget.fix(fixCoordSystem);
}

void ModelTexAnimDef::Init(MPQFile &f, TexAnim& texAnim, int *global)
{//unsigned char* 
	AnimatedInit(texAnim.trans,	f, trans, global);
	AnimatedInit(texAnim.rot,	f, rot, global);
	AnimatedInit(texAnim.scale,	f, scale, global);
}

void ModelBoneDef::Init(MPQFile &f, BoneAnim &boneAnim, int *global)
{
	boneAnim.parent = (uint8)parent;
	boneAnim.pivot = fixCoordSystem(pivot);
	boneAnim.billboard = (flags & 8) != 0;
	//billboard = false;
	AnimatedInit(boneAnim.trans,	f, translation, global);
	AnimatedInit(boneAnim.rot,	f, rotation, global);
	AnimatedInit(boneAnim.scale,	f, scaling, global);
	boneAnim.trans.fix(fixCoordSystem);
	boneAnim.rot.fix(fixCoordSystemQuat);
	boneAnim.scale.fix(fixCoordSystem2);
}

void ModelColorDef::Init(MPQFile &f, ColorAnim &colorAnim, int *global)
{
	AnimatedInit(colorAnim.color, f, color, global);
	AnimatedInit(colorAnim.opacity, f, opacity, global);
}

void ModelTransDef::Init(MPQFile &f, TransAnim& transAnim, int *global)
{
	AnimatedInit(transAnim.trans, f, trans, global);
}

void ModelLightDef::Init(MPQFile &f, LightAnim &lightAnim, int *global)
{
	lightAnim.tpos = lightAnim.pos = fixCoordSystem(pos);
	lightAnim.tdir = lightAnim.dir = Vec3D(0,1,0); // no idea
	lightAnim.type = type;
	lightAnim.parent = bone;
	AnimatedInit(lightAnim.ambColor,		f,ambColor, global);
	AnimatedInit(lightAnim.ambIntensity,	f,ambIntensity, global);
	AnimatedInit(lightAnim.diffColor,		f,color, global);
	AnimatedInit(lightAnim.diffIntensity,	f,intensity, global);
}

void ModelAttachmentDef::Init(MPQFile &f, ModelAttachment &attachment, int *global)
{
	attachment.pos = fixCoordSystem(pos);
	attachment.bone = bone;
	attachment.id = id;
}


void ModelParticleEmitterDef::Init(MPQFile &f, CParticleEmitter &particleEmitter, int *globals)
{
	AnimatedInit(particleEmitter.m_Speed,			f, params[0], globals);
	AnimatedInit(particleEmitter.m_Variation,		f, params[1], globals);
	AnimatedInit(particleEmitter.m_Spread,			f, params[2], globals);
	AnimatedInit(particleEmitter.m_Lat,				f, params[3], globals);
	AnimatedInit(particleEmitter.m_Gravity,			f, params[4], globals);
	AnimatedInit(particleEmitter.m_Lifespan,		f, params[5], globals);
	AnimatedInit(particleEmitter.m_Rate,			f, params[6], globals);
	AnimatedInit(particleEmitter.m_Areal,			f, params[7], globals);
	AnimatedInit(particleEmitter.m_Areaw,			f, params[8], globals);
	AnimatedInit(particleEmitter.m_Deacceleration,	f, params[9], globals);
	AnimatedInit(particleEmitter.m_Enabled,			f, en, globals);

	for (size_t i=0; i<3; i++) {
		particleEmitter.m_Colors[i] = *(Color32*)&p.colors[i];
		particleEmitter.m_Sizes[i] = p.sizes[i];// * p.scales[i];
	}
	particleEmitter.m_fLifeMid = p.mid;
	particleEmitter.m_fSlowdown = p.slowdown;
	particleEmitter.m_fRotation = p.rotation;
	particleEmitter.m_vPos = fixCoordSystem(pos);
	particleEmitter.m_nTexChannel = texture;
	//particleEmitter.m_nBlend = blend;
	//// 设置混合模式
	//switch (m_pEmitter->m_nBlend)
	//{
	//case 0:	// 透明 镂空
	//	R.SetBlendFunc(true, BLENDOP_ADD, SBF_SOURCE_ALPHA, SBF_ONE_MINUS_SOURCE_ALPHA);
	//	R.SetAlphaTestFunc(true);
	//	break;
	//case 1:	//MODULATE 加亮
	//	R.SetBlendFunc(true, BLENDOP_ADD, SBF_SOURCE_COLOUR, SBF_ONE);
	//	R.SetAlphaTestFunc(false);
	//	break;
	//case 2: // 透明
	//	R.SetBlendFunc(true, BLENDOP_ADD, SBF_SOURCE_ALPHA, SBF_ONE_MINUS_SOURCE_ALPHA);
	//	R.SetAlphaTestFunc(false);
	//	break;
	//case 3:	// 镂空
	//	R.SetBlendFunc(false);
	//	R.SetAlphaTestFunc(true);
	//	break;
	//case 4: // 加亮
	//	R.SetBlendFunc(true, BLENDOP_ADD, SBF_SOURCE_ALPHA, SBF_ONE);
	//	R.SetAlphaTestFunc(false);
	//	break;
	//}
	particleEmitter.m_nRows = rows;
	particleEmitter.m_nCols = cols;
	particleEmitter.type = s1;
	//m_nOrder = s2;
	particleEmitter.m_nOrder = s1>0 ? -1 : 0;
	particleEmitter.m_nBoneID = bone;

	//switch (type) {
	//case 1:
	//	emitter = new PlaneParticleEmitter(this);
	//	break;
	//case 2:
	//	emitter = new SphereParticleEmitter(this);
	//	break;
	//}

	//transform = flags & 1024;

	// Flags && Type 1
	// 4121 = water ele
	// 4097 = water elemental
	// 1041 = Transcendance Halo
	// 1039 = water ele
	// 569 = Faith shoulders (angel wings)
	// 313 = Faith halo (centre flame?)
	// 49 = particle moving up?
	// 29 = particle being static
	// 25 = flame on weapon - move up/along the weapon
	// 17 = glow on weapon - static, random direction
	// 17 = aurastone - ?
	// 1 = perdition blade && water elemental

	// Type 2
	// 3145 = water ele
	// 1305 = water ele
	// 1049 = water elemental
	// 1033 = water elemental
	// 281 = water ele
	// 256 = Water elemental
	// 57 = Faith halo, ring?
	// 9 = water elemental

	particleEmitter.m_bBillboard = !(flags & 0x1000);

	// diagnosis test info
	particleEmitter.pType = type;
	particleEmitter.flags = flags;

	particleEmitter.tofs = frand();

	// 初始化粒子纹理动画UV坐标
	for (int i = 0; i < particleEmitter.m_nRows * particleEmitter.m_nCols; i++)
	{
		TexCoordSet tc;
		particleEmitter.InitTile(tc.tc,i);
		particleEmitter.m_Tiles.push_back(tc);
	}
}

void ModelRibbonEmitterDef::Init(MPQFile &f, RibbonEmitter &ribbonEmitter, int *globals)
{
	//
	AnimatedInit(ribbonEmitter.color,	f, color, globals);
	AnimatedInit(ribbonEmitter.opacity,	f, opacity, globals);
	AnimatedInit(ribbonEmitter.above,	f, above, globals);
	AnimatedInit(ribbonEmitter.below,	f, below, globals);

	//gfhfghfghfghfghfg//m_pParentBone = model->bones + bone;
	int *texlist = (int*)(f.getBuffer() + aTextures.offset);
	// just use the first texture for now; most models I've checked only had one
	ribbonEmitter.m_nTexChannel = texlist[0];

	ribbonEmitter.tpos = ribbonEmitter.pos = fixCoordSystem(pos);

	// TODO: figure out actual correct way to calculate length
	// in BFD, res is 60 and len is 0.6, the trails are very short (too long here)
	// in CoT, res and len are like 10 but the trails are supposed to be much longer (too short here)
	ribbonEmitter.numsegs = (int)res;
	ribbonEmitter.seglen = length;
	ribbonEmitter.length = res * ribbonEmitter.seglen;
	// 我自己家的
	ribbonEmitter.seglen/=10;

	// create first segment
	RibbonSegment rs;
	rs.pos = ribbonEmitter.tpos;
	rs.len = 0;
	ribbonEmitter.segs.push_back(rs);
}