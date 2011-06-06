#include "IORead.h"
#include "MyIORead.h"
#include "IOReadPak.h"

IOReadBase* IOReadBase::autoOpen(const std::string& strFilename, const std::string& strArchive)
{
	IOReadBase* res = NULL;
	if (IOReadFile::Exists(strFilename))
	{
		res = new IOReadFile();
	}
	else if (IOReadPak::Exists(strFilename))
	{
		res = new IOReadPak();
	}
	//else if (IOReadDnp::Exists(strFilename))
	//{
	//	res = new IOReadDnp();
	//}
	//else if (IOReadMPQ::Exists(strFilename))
	//{
	//	res = new IOReadMPQ();
	//}
	//else if (IOReadZip::Exists(strFilename))
	//{
	//	res = new IOReadZip();
	//}
	//if (archive == NULL)
	//{
	//	//res = new IOReadFile();
	//}
	//else
	//{
	//	//res = new IOReadZip();
	//}
	if (res)
	{
		if(res->Open(strFilename, strArchive))
		{
			return res;
		}
		else
		{
			delete res;
		}
	}
	return NULL;
}

void IOReadBase::autoClose(IOReadBase*& pRead)
{
	if (pRead)
	{
		pRead->Close();
		delete pRead;
		pRead=NULL;
	}
}

bool IOReadBase::IsEof()
{
	return Tell()>=GetSize();
}

bool IOReadBase::Exists(const std::string& strFilename)
{
	if (!IOReadFile::Exists(strFilename))
	{
		if (!IOReadPak::Exists(strFilename))
		{
			//if (!IOReadDnp::Exists(strFilename))
			{
				//if (!IOReadMPQ::Exists(strFilename))
				{
					//if (!IOReadZip::Exists(strFilename))
					{
						return false;
					}
				}
			}
		}
	}
	return true;
}