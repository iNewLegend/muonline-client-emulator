#include <tchar.h>
#include "Direct.h"
#include "FileSystem.h"
#include "DecryptFuncs.h"
#include "ItemBMD.h"
#include "MUBmd.h"

int main(int argc, _TCHAR* argv[])
{
	mkdir("Dec");mkdir("Enc");
	std::wstring wstrFindPath = getCurrentDirectory();
	CDir dir;
	dir.ReadDir(wstrFindPath);
	for (size_t i=0; i<dir.m_FileInfo.size(); i++)
	{
		if (!dir.m_FileInfo[i].IsDirectory())
		{
			std::wstring wstrExt = GetExtension(dir.m_FileInfo[i].wstrFilename);
			std::string strFilename = ws2s(dir.m_FileInfo[i].wstrFilename);
			if (wstrExt==L".ozj")
			{
				std::string strDestFilename = "Dec\\"+ChangeExtension(strFilename,".jpg");
				fileOffset(strFilename,strDestFilename,24);
			}
			else if (wstrExt==L".ozt")
			{
				std::string strDestFilename = "Dec\\"+ChangeExtension(strFilename,".tga");
				fileOffset(strFilename,strDestFilename,4);
			}
			else if (wstrExt==L".ozb")
			{
				std::string strDestFilename = "Dec\\"+ChangeExtension(strFilename,".bmp");
				fileOffset(strFilename,strDestFilename,4);
			}
			else if (wstrExt==L".jpg")
			{
				std::string strDestFilename = "Enc\\"+ChangeExtension(strFilename,".ozj");
				fileOffset(strFilename,strDestFilename,-24);
			}
			else if (wstrExt==L".tga")
			{
				std::string strDestFilename = "Enc\\"+ChangeExtension(strFilename,".ozt");
				fileOffset(strFilename,strDestFilename,-4);
			}
			else if (wstrExt==L".bmp")
			{
				std::string strDestFilename = "Enc\\"+ChangeExtension(strFilename,".ozb");
				fileOffset(strFilename,strDestFilename,-4);
			}
			else if (wstrExt==L".map"||wstrExt==L".obj")
			{
				decryptMuFile(strFilename,"Dec\\"+strFilename+"d");
			}
			else if (wstrExt==L".att")
			{
				decryptMuATTFile(strFilename,"Dec\\"+strFilename+"d");
			}
			else if (wstrExt==L".bmd")
			{
				if ("item.bmd"==strFilename)
				{
					decryptItemBMD(strFilename,"Dec\\"+ChangeExtension(strFilename,".csv"));
				}
				else if ("BuffEffect.bmd"==strFilename)
				{
					decryptBuffEffectFile(strFilename,"Dec\\"+strFilename+"d");
				}
				//else if (isEncBmd(strFilename))
				//{
				//	decryptMuFile(strFilename,"Dec\\"+strFilename);
				//}
				else
				{
					Bmd2Smd(strFilename,"Dec\\"+ChangeExtension(strFilename,".smd"));
				//	decryptMuFileXOR3(strFilename,"Dec\\"+strFilename);
				}
			}
			else if (wstrExt==L".csv")
			{
				if ("item.csv"==strFilename)
				{
					encryptItemBMD(strFilename,"Enc\\"+ChangeExtension(strFilename,".bmd"));
				}
			}
			else if (wstrExt==L".smd")
			{
				Smd2Bmd(strFilename,"Enc\\"+ChangeExtension(strFilename,".bmd"));
			}
		}
	}

	return 0;
}