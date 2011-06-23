#ifndef OBJATTACK_H
#define OBJATTACK_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "MagicInf.h"
#include "ObjBaseAttack.h"

class CObjAttack : public CObjBaseAttack
{

public:

	CObjAttack();
	virtual ~CObjAttack();

	BOOL Attack(LPOBJ lpObj, LPOBJ lpTargetObj, CMagicInf* lpMagic,  int magicsend, unsigned char MSBFlag, int AttackDamage, BOOL bCombo);
	int GetAttackDamage(LPOBJ lpObj, int targetDefense, BYTE& effect, BOOL bIsOnDuel, CMagicInf* lpMagic);
	int GetAttackDamageWizard(LPOBJ lpObj, int targetDefense, CMagicInf* lpMagic, BYTE& effect, BOOL bIsOnDuel);
	int GetShieldDamage(LPOBJ lpObj, LPOBJ lpTargetObj, int iAttackDamage);

};

BOOL gObjDenorantSprite(LPOBJ lpObj);
BOOL gObjDarkHorse(LPOBJ lpObj);
BOOL gObjFenrir(LPOBJ lpObj);

extern CObjAttack gclassObjAttack;

#endif