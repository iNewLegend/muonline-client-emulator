#include "stdafx.h"
#include "Mint4Gunz.h"

extern ZDirectInput	g_DInput;


const char* Mint4Gunz::GetActionKeyName(unsigned long int nKey){
	return g_DInput.GetKeyName(nKey);
}

