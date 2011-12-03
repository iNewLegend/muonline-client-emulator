#ifndef _ZSTAGESETTING_H
#define _ZSTAGESETTING_H

#include "ZPrerequisites.h"

class ZStageSetting
{
public:
	static void ShowStageSettingDialog( const MSTAGE_SETTING_NODE* pStageSetting, bool bShowAll);
	static void InitStageSettingDialog();
	static void ApplyStageSettingDialog();
	static void AdjustLimitTimeStageSettingDialog();
	static void PostDataToServer();
	static void InitStageSettingGameType();
	static void InitStageSettingGameFromGameType();
};

#endif