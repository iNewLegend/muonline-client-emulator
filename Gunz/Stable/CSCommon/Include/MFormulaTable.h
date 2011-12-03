#ifndef MFORMULATABLE_H
#define MFORMULATABLE_H

#include "MObjectTypes.h"

const unsigned long int	g_nHPRegenTickInteval = 1000;		// (1 sec)
const unsigned long int g_nENRegenTickInteval = 1000;		// (1 sec)


int MGetHP_LCM(int nLevel, MCharacterClass nClass);
float MGetEN_LCM(int nLevel, MCharacterClass nClass);

int MGetChangingModeTickCount(MCharacterMode mode);		///< ĳ���� ��� ���Խð�(msec����)

int MCalculateMaxHP(int nLevel, MCharacterClass nClass, int nCON, int nDCS);	///< �ִ� HP ���
int MCalculateMaxEN(int nLevel, MCharacterClass nClass, int nINT, int nDMS);	///< �ִ� EN ���
float MCalculateRegenHPPerTick(int nLevel, MCharacterRace nRace, int nDCS);		///< �ʴ� ���� HP ���
float MCalculateRegenENPerTick(int nLevel, MCharacterClass nClass, int nDMS);	///< �ʴ� ���� EN ���
float MCalculateAtkDamage();	///< Attack Damage ���
#endif
