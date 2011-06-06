
#include "database.h"

// dbs
ItemDatabase		items;
NPCDatabase			npcs;

// --
HelmGeosetDB		helmetdb;
ItemVisualEffectDB	effectdb;
ItemDisplayDB		itemdb;
StartOutfitDB		startdb;
ItemSubClassDB		subclassdb;
ItemVisualDB		visualdb;
ItemSetDB			setsdb;
// --
AnimDB				animdb;
CharHairGeosetsDB	hairdb;
CharSectionsDB		chardb;
CharClassesDB		classdb;
CharFacialHairDB	facialhairdb;
CharRacesDB			racedb;
//--
CreatureModelDB		modeldb;
CreatureSkinDB		skindb;
CreatureTypeDB		npctypedb;
NPCDB				npcdb;

ItemDatabase&		GetItemDatabase()		{ return items; }
AnimDB&				GetAnimDB()				{ return animdb; }
HelmGeosetDB&		GetHelmGeosetDB()		{ return helmetdb; }
ItemVisualEffectDB&	GetItemVisualEffectDB()	{ return effectdb; }
ItemDisplayDB&		GetItemDisplayDB()		{ return itemdb; }
CreatureModelDB&	GetCreatureModelDB()	{ return modeldb; }
NPCDatabase&		GetNPCDatabase()		{ return npcs; }
ItemSetDB&			GetItemSetDB()			{ return setsdb; }
CreatureSkinDB&		GetCreatureSkinDB()		{ return skindb; }
StartOutfitDB&		GetStartOutfitDB()		{ return startdb; }
ItemSubClassDB&		GetItemSubClassDB()		{ return subclassdb; }
ItemVisualDB&		GetItemVisualDB()		{ return visualdb; }
CharHairGeosetsDB&	GetCharHairGeosetsDB()	{ return hairdb; }
CharSectionsDB&		GetCharSectionsDB()		{ return chardb; }
CharClassesDB&		GetCharClassesDB()		{ return classdb; }
CharFacialHairDB&	GetCharFacialHairDB()	{ return facialhairdb; }
CharRacesDB&		GetCharRacesDB()		{ return racedb; }
NPCDB&				GetNPCDB()				{ return npcdb; }
CreatureTypeDB&		GetCreatureTypeDB()		{ return npctypedb; }
// ANIMDB.H
AnimDB::AnimDB(): DBCFile("DBFilesClient\\AnimationData.dbc")
{
}

AnimDB::~AnimDB()
{
}

AnimDB::Record AnimDB::getByAnimID(unsigned int id)
{
	/// Brute force search for now
	for(Iterator i=begin(); i!=end(); ++i)
	{
		if(i->getUInt(AnimID) == id)
			return (*i);
	}
	throw NotFound();
}
// --


// --
// CHARDB.H
// HairGeosets

CharHairGeosetsDB::Record CharHairGeosetsDB::getByParams(unsigned int race, unsigned int gender, unsigned int section)
{
	for(Iterator i=begin(); i!=end(); ++i)
	{
		if (i->getUInt(Race)==race && i->getUInt(Gender)==gender && i->getUInt(Section)==section)
			return (*i);
	}
	throw NotFound();
}

int CharHairGeosetsDB::getGeosetsFor(unsigned int race, unsigned int gender)
{
	int n = 0;
	for(Iterator i=begin(); i!=end(); ++i)
	{
		if (i->getUInt(Race)==race && i->getUInt(Gender)==gender) {
			n++;
		}
	}
    return n;
}

// Sections

int CharSectionsDB::getColorsFor(unsigned int race, unsigned int gender, unsigned int type, unsigned int section, unsigned int npc)
{
	int n = 0;
	for(Iterator i=begin(); i!=end(); ++i)
	{
		// don't allow NPC skins ;(
		if (i->getUInt(Race)==race && i->getUInt(Gender)==gender && i->getUInt(Type)==type && i->getUInt(Section)==section && i->getUInt(IsNPC)==npc) {
			n++;
		}
	}
    return n;
}

int CharSectionsDB::getSectionsFor(unsigned int race, unsigned int gender, unsigned int type, unsigned int color, unsigned int npc)
{
	int n = 0;
	for(Iterator i=begin(); i!=end(); ++i)
	{
		if (i->getUInt(Race)==race && i->getUInt(Gender)==gender && i->getUInt(Type)==type && i->getUInt(Color)==color && i->getUInt(IsNPC)==npc) {
			n++;
		}
	}
    return n;
}

CharSectionsDB::Record CharSectionsDB::getByParams(unsigned int race, unsigned int gender, unsigned int type, unsigned int section, unsigned int color, unsigned int npc)
{
	for(Iterator i=begin(); i!=end(); ++i)
	{
		if (i->getUInt(Race)==race && i->getUInt(Gender)==gender && i->getUInt(Type)==type && i->getUInt(Section)==section && i->getUInt(Color)==color && i->getUInt(IsNPC)==npc)
			return (*i);
	}
	throw NotFound();
}

// Races
CharRacesDB::Record CharRacesDB::getByName(std::string name)
{
	for(Iterator i=begin(); i!=end(); ++i)
	{
		if (stricmp(name.c_str(),i->getString(Name)) == 0)
			return (*i);
	}
	throw NotFound();
}

CharRacesDB::Record CharRacesDB::getById(unsigned int id)
{
	for(Iterator i=begin(); i!=end(); ++i)
	{
		if (i->getUInt(RaceID)==id) return (*i);
	}
	throw NotFound();
}


// FacialHair

CharFacialHairDB::Record CharFacialHairDB::getByParams(unsigned int race, unsigned int gender, unsigned int style)
{
	for(Iterator i=begin(); i!=end(); ++i)
	{
		if (i->getUInt(Race)==race && i->getUInt(Gender)==gender && i->getUInt(Style)==style)
			return (*i);
	}
	throw NotFound();
}

int CharFacialHairDB::getStylesFor(unsigned int race, unsigned int gender)
{
	int n = 0;
	for(Iterator i=begin(); i!=end(); ++i)
	{
		if (i->getUInt(Race)==race && i->getUInt(Gender)==gender) {
			n++;
		}
	}
    return n;
}


// Classes

CharClassesDB::Record CharClassesDB::getById(unsigned int id)
{
	for(Iterator i=begin(); i!=end(); ++i)
	{
		if (i->getUInt(ClassID)==id) return (*i);
	}
	throw NotFound();
}


// Head and Helmet display info
HelmGeosetDB::Record HelmGeosetDB::getById(unsigned int id)
{
	for(Iterator i=begin(); i!=end(); ++i)
	{
		if (i->getUInt(TypeID)==id)
			return (*i);
	}
	throw NotFound();
}
// --


// --
// CREATUREDB.H
//

CreatureModelDB::CreatureModelDB(): DBCFile("DBFilesClient\\CreatureModelData.dbc")
{

}

CreatureModelDB::~CreatureModelDB()
{

}

CreatureModelDB::Record CreatureModelDB::getByFilename(std::string fn)
{
	/// Brute force search for now
	for(Iterator i=begin(); i!=end(); ++i)
	{
		std::string str(i->getString(Filename));
		if(stricmp(str.c_str(),fn.c_str()) == 0)
			return (*i);
	}
	throw NotFound();
}

CreatureModelDB::Record CreatureModelDB::getByID(unsigned int id)
{
	for(Iterator i=begin(); i!=end(); ++i)
	{
		if (i->getUInt(ModelID)==id)
			return (*i);
	}
	throw NotFound();
}

CreatureSkinDB::CreatureSkinDB(): DBCFile("DBFilesClient\\CreatureDisplayInfo.dbc")
{

}

CreatureSkinDB::~CreatureSkinDB()
{

}

CreatureSkinDB::Record CreatureSkinDB::getByModelID(unsigned int id)
{
	/// Brute force search for now
	for(Iterator i=begin(); i!=end(); ++i)
	{
		if(i->getUInt(ModelID) == id)
			return (*i);
	}
	throw NotFound();
}

CreatureSkinDB::Record CreatureSkinDB::getBySkinID(unsigned int id)
{
	/// Brute force search for now
	for(Iterator i=begin(); i!=end(); ++i)
	{
		if(i->getUInt(SkinID) == id)
			return (*i);
	}
	throw NotFound();
}

CreatureTypeDB::CreatureTypeDB(): DBCFile("DBFilesClient\\CreatureType.dbc")
{

}

CreatureTypeDB::~CreatureTypeDB()
{

}

CreatureTypeDB::Record CreatureTypeDB::getByID(unsigned int id)
{
	/// Brute force search for now
	for(Iterator i=begin(); i!=end(); ++i)
	{
		if(i->getUInt(ID) == id)
			return (*i);
	}
	throw NotFound();
}

NPCDB::NPCDB(): DBCFile("DBFilesClient\\CreatureDisplayInfoExtra.dbc")
{
}

NPCDB::~NPCDB()
{
}

NPCDB::Record NPCDB::getByFilename(std::string fn)
{
	/// Brute force search for now
	for(Iterator i=begin(); i!=end(); ++i)
	{
		if(i->getString(Filename) == fn) {
			std::cout << i->getString(Filename) << "\n";
			return (*i);
		}
	}
	throw NotFound();
}

NPCDB::Record NPCDB::getByNPCID(unsigned int id)
{
	/// Brute force search for now
	for(Iterator i=begin(); i!=end(); ++i)
	{
		if(i->getUInt(NPCID) == id)
			return (*i);
	}
	throw NotFound();
}
// --



// --
// ITEMDB.H
//
// --------------------------------
// Item Database Stuff
// --------------------------------
char* ItemTypeNames[NUM_ITEM_TYPES] = {
	"All",
	"Helmets",
	"Neck",
	"Shoulder armor",
	"Shirts",
	"Chest armor",
	"Belts",
	"Pants",
	"Boots",
	"Bracers",
	"Gloves",
	"Rings",
	"Accessories",
	"Daggers",
	"Shields",
	"Bows",
	"Capes",
	"Two-handed weapons",
	"Quivers",
	"Tabards",
	"Robes",
	"One-handed weapons",
	"Claws",
	"Offhand",
	"?", // unused?
	"Thrown",
	"Guns and wands"
};

// ItemDisplayInfo

ItemDisplayDB::Record ItemDisplayDB::getById(unsigned int id)
{
	for(Iterator i=begin(); i!=end(); ++i)
	{
		if (i->getUInt(ItemDisplayID)==id)
			return (*i);
	}
	throw NotFound();
}


ItemVisualDB::Record ItemVisualDB::getById(unsigned int id)
{
	for(Iterator i=begin(); i!=end(); ++i)
	{
		if (i->getUInt(VisualID)==id)
			return (*i);
	}
	throw NotFound();
}

ItemVisualEffectDB::Record ItemVisualEffectDB::getById(unsigned int id)
{
	for(Iterator i=begin(); i!=end(); ++i)
	{
		if (i->getUInt(EffectID)==id)
			return (*i);
	}
	throw NotFound();
}

ItemSetDB::Record ItemSetDB::getById(unsigned int id)
{
	for(Iterator i=begin(); i!=end(); ++i)
	{
		if (i->getUInt(SetID)==id)
			return (*i);
	}
	throw NotFound();
}

void ItemSetDB::cleanup(ItemDatabase &itemdb)
{
	for(Iterator i=begin(); i!=end(); ++i) {
		for (int j=0; j<NumItems; j++) {
			int id = i->getUInt(ItemIDBase+j);
			if (id > 0) {
				const ItemRecord &r = itemdb.get(id);
				if (r.type > 0) {
					avail.insert(i->getUInt(SetID));
					break;
				}
			}
		}
	}
}

bool ItemSetDB::available(unsigned int id)
{
	return (avail.find(id)!=avail.end());
}


StartOutfitDB::Record StartOutfitDB::getById(unsigned int id)
{
	for(Iterator i=begin(); i!=end(); ++i)
	{
		if (i->getUInt(StartOutfitID)==id)
			return (*i);
	}
	throw NotFound();
}



////////////////////

ItemRecord::ItemRecord(const char* line)
{
	sscanf(line, "%u,%u,%u,%u,%u,%u,%u", &id, &model, &itemclass, &subclass, &type, &sheath, &quality);
	for (size_t i=strlen(line)-2; i>1; i--) {
		if (line[i]==',') {
			name = (line + i + 1);
			break;
		}
	}
}


ItemDatabase::ItemDatabase(const char* filename)
{
	ItemRecord all("---- None ----", IT_ALL);
	items.push_back(all);
	std::ifstream fin(filename);
	char line[512];
	while (fin.getline(line,512)) {
		ItemRecord rec(line);
		if (rec.type > 0) {
			items.push_back(rec);
		}
	}
	fin.close();
	sort(items.begin(), items.end());
}

void ItemDatabase::open(const char* filename)
{
	ItemRecord all("---- None ----", IT_ALL);
	items.push_back(all);
	std::ifstream fin(filename);
	char line[512];
	while (fin.getline(line,512)) {
		ItemRecord rec(line);
		if (rec.type > 0) {
			items.push_back(rec);
		}
	}
	fin.close();
	sort(items.begin(), items.end());
}

void ItemDatabase::cleanup(ItemDisplayDB &itemdb)
{
	std::set<unsigned int> itemset;
	for (ItemDisplayDB::Iterator it = itemdb.begin(); it != itemdb.end(); ++it) {
		itemset.insert(it->getUInt(ItemDisplayDB::ItemDisplayID));
	}
	for (unsigned int i=0; i<items.size();) {
		bool keepItem = (items[i].type==0) || (itemset.find(items[i].model)!=itemset.end());
		if (keepItem) {
			itemLookup[items[i].id] = i;
			i++;
		}
		else items.erase(items.begin() + i);
	}
}

const ItemRecord& ItemDatabase::get(int id)
{
    if (itemLookup.find(id)!=itemLookup.end()) 
		return items[itemLookup[id]];
	else 
		return items[0];
}

int ItemDatabase::getItemNum(int id)
{
	for (std::vector<ItemRecord>::iterator it = items.begin(); it != items.end(); ++it)
		if(it->model == id) return it->id;

	return 0;
}

///////////////////

ItemSubClassDB::Record ItemSubClassDB::getById(int id, int subid)
{
	for(Iterator i=begin(); i!=end(); ++i)
	{
		if (i->getInt(ClassID)==id && i->getInt(SubClassID)==subid)
			return (*i);
	}
	throw NotFound();
}
// ============================================================
// =============================================================


NPCRecord::NPCRecord(const char* line)
{
	sscanf(line, "%u,%u,%u,", &id, &model, &type);
	for (size_t i=strlen(line)-2; i>1; i--) {
		if (line[i]==',') {
			name = (line + i + 1);
			break;
		}
	}
}

NPCDatabase::NPCDatabase(const char* filename)
{
	//ItemRecord all("---- None ----", IT_ALL);
	//items.push_back(all);

	std::ifstream fin(filename);
	char line[512];
	while (fin.getline(line,512)) {
		NPCRecord rec(line);
		if (rec.model > 0) {
			npcs.push_back(rec);
		}
	}
	fin.close();
	sort(npcs.begin(), npcs.end());
}

void NPCDatabase::open(const char* filename)
{
	std::ifstream fin(filename);
	char line[512];
	while (fin.getline(line,512)) {
		NPCRecord rec(line);
		if (rec.model > 0) {
			npcs.push_back(rec);
		}
	}
	fin.close();
	sort(npcs.begin(), npcs.end());
}


const NPCRecord& NPCDatabase::get(int id)
{
	return npcs[id];
}

const NPCRecord& NPCDatabase::getByID(int id)
{
	for (std::vector<NPCRecord>::iterator it=npcs.begin();  it!=npcs.end(); ++it) {
		if (it->id == id) {
			return (*it);
		}
	}

	return npcs[0];
}

// --



// --
// SPELLDB.H
//

/*

SpellVisualeffects.dbc
column 1 = id, int
column 2 = spell name, string
column 3 = model name, string
column 4 = number between 0 and 11, int
column 5 = number 0 or 1,  1 entry is 50?, int.. possibly boolean.

Column 3, ignore entries starting with "zzOLD__" ?
Column 4, wtf are .mdl files? they're from warcraft 3?

col 5 and 6? figure out what they're for.
Column5 is either Spell Type,  or Spell slot or something similar
*/


SpellEffectsDB::Record SpellEffectsDB::getByName(const std::string name)
{
	for(Iterator i=begin(); i!=end(); ++i)
	{
		if (stricmp(name.c_str(),i->getString(EffectName)) == 0) 
			return (*i);
	}
	throw NotFound();
}

SpellEffectsDB::Record SpellEffectsDB::getById(unsigned int id)
{
	for(Iterator i=begin(); i!=end(); ++i)
	{
		if (i->getUInt(ID)==id)
			return (*i);
	}
	throw NotFound();
}
// --
