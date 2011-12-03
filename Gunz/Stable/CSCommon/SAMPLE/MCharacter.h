#ifndef MCHARACTER_H
#define MCHARACTER_H

#include "MXml.h"

struct MCHR
{
	// �ĺ�
	char		szName[24];
	char		szSurname[24];
	int			iClass;				// enum
	int			iRace;				// enum
	int			iType;				// enum
	int			iSex;				// enum
	int			iSpecialization;	// enum
	int			iBindingSpot;		// pos

	// ����ġ
	int			iCurrentXP;
	int			iNextXP;

	// ����
	int			iRelationships;		// list
	int			iRshipRaise;		// list
	int			iRshipLower;		// list

	// �⺻Ư��ġ
	int			iHP;
	int			iEN;
	int			iSTR;
	int			iCON;
	int			iDEX;
	int			iAGI;
	int			iINT;
	int			iCHA;

	// ���׷�
	int			iMR;
	int			iFR;
	int			iCR;
	int			iPR;
	int			iLR;

	// ����
	int			iBUFFS;		// list
	int			iSkillList;	// list

	// ����
	int			iAC;
	int			iMFlee;
	int			iRFlee;
	int			iSFlee;
	int			iHPRegen;
	int			iENRegen;

	// ���ݷ�
	int			iMATK;
	int			iRATK;
	int			iMCritical;
	int			iRCritical;
	int			iMAspd;
	int			iRAspd;
	int			iMAccu;
	int			iRAccu;

	// �̵�
	int			iDSight;
	int			iNSight;
	int			iSpd;

	// ����
	int			iLevitate;			// flag
	int			iInvisibility;		// flag
	int			iInvincibility;		// flag
	int			iStun;				// flag
	int			iStealth;			// flag
	int			iPoisoned;			// flag

	// �̹�
	int			iIVMelee;			// flag
	int			iIVRange;			// flag
	int			iIVMagic;			// flag
	int			iIVCold;			// flag
	int			iIVLightning;		// flag
	int			iIVPoison;			// flag
	int			iIVFire;			// flag
	int			iIVStun;			// flag
	int			iIVSleep;			// flag
	int			iIVCrit;			// flag
	int			iIVParalysis;		// flag

	// ����ȭ
	int			iOCS;
	int			iDCS;
	int			iRCS;
	int			iECS;
	int			iOMS;
	int			iDMS;
	int			iEMS;
};


bool chr_LoadFromXmlFile(MCHR* pOutChr, char* szFileName, int iChrID);
#endif