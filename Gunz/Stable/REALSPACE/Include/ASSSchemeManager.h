#ifndef __ASSSCHEMEMANAGER_H
#define __ASSSCHEMEMANAGER_H

#include "RSMaterialManager.h"
#include "CMList.h"

/*
   scheme�� �Ǵ� rml���� ������ ���� �̸��� ���� material���� ����ִ�.

pattern			:	base pattern ( ��Ƽ �ؽ��� )
base_#1			:	base�� �Ǵ� material
ext_#1_#2_#3	:	#1 : nTileSet			:	Ȯ��� �ؽ�ó ���� ���� : ( Ǯ, ��, ...)
				 	#2 : nTileStyle			:	Ÿ���� ���� ���� ( �ټ������� �ִ� )
				 	#3 : nTileAppearance	:	���� ��Ÿ���� �������
												�� �� ��ȣ�� �޶����� Ÿ���� �̾������Ѵ�.
												(�� ���� �� ���� �����ʿ�� ����)

					#2�� TileStyle�� ���� material�� ������ ���� �⺻���� ����� �������Ѵ�.

				0				1				2				3				4
            +-----------+   +-----------+   +-----------+   +-----------+   +-----------+
            |***********|   |*****      |   |***********|   |***********|   |*****      |
            |***********|   |*****      |   |***********|   |***********|   |*****      |
            |***********|   |*****      |   |***********|   |***********|   |*****      |
            |***********|   |           |   |           |   |*****      |   |      *****|
            |***********|   |           |   |           |   |*****      |   |      *****|
            |***********|   |           |   |           |   |*****      |   |      *****|
            +-----------+   +-----------+   +-----------+   +-----------+   +-----------+

  ���� 1. #1Ȥ�� #3�� �� �� �̻��� �� ��쿡�� MaterialBlast �� ������ ����. ����
			������ �����Ϸ��� 01 ~ 09 , 10 ~ ... �� ���� �������� ���־�� ��.

  ���� 2. MaterialBlast�� ��ҹ��ڸ� �����Ƿ� �� ���� ��������. ��~ �ҹ��ڷ�.
  ���� 3. Texture�̸��� �ƴ� Material�̸��̹Ƿ� MaterialBlast���� 
			add�Ҷ� Create Materials... �ؾ���.

  ���� ���� sample.rml�� ����, 

*/

class ASSTile
{
public:
	ASSTile();
	virtual ~ASSTile();

	int nTextureHandle[4];
	int nTextureIndex;
};

typedef CMLinkedList <ASSTile> ASSTileAppearance;

class ASSTileSet
{
public:
	ASSTileAppearance *Get(int i) { return &m_TileAppearances[i]; }

private:
	ASSTileAppearance m_TileAppearances[5];
};

class ASSSchemeManager : public CMLinkedList <ASSTileSet>
{
public:
	bool Create(const char *RMLName);
	void Destroy();

	int	GetBaseCount() { return m_BaseTileList.GetCount(); }
	ASSTile *GetBaseTile(int n) { return m_BaseTileList.Get(n); }

	int GetTextureHandle(int index);
	RSMaterialManager* GetMaterialManager() { return &m_MaterialManager; }

private:
	bool IsExtensionName(const char *name,int *nTileSet,int *nTileStyle,int *nTileAppearance);

	CMLinkedList <ASSTile> m_BaseTileList;
	RSMaterialManager m_MaterialManager;
};

#endif