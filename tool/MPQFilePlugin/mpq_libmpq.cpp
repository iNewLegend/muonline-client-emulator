#include "mpq_libmpq.h"

#include <vector>
#include <algorithm> 

#define S_REL(p) { if(p) p->Release(); p = NULL; }
#define S_DEL(p) { if(p) delete p; p = NULL; }
#define S_DELS(p) { if(p) delete[] p; }
#define SIZEOF(array)   (sizeof(array)/sizeof(array[0]))

ArchiveSet gOpenArchives;

ArchiveSet& GetOpenArchives()
{
	return gOpenArchives;
}
//bool useLocalFiles = false;
bool bUseTestPatch = false;
bool bUsePatch = true;

std::string testPath = "D:\\World of Warcraft\\Data\\";
std::string gamePath = "D:\\World of Warcraft\\Data\\";

std::vector<MPQArchive*> archives;

MPQArchive::MPQArchive(const char* filename)
{
	int result = libmpq_archive_open(&mpq_a, (unsigned char*)filename);

	//wxLogMessage(_T("Opening %s"), filename);
	if(result) {
		//wxLogMessage(_T("Error opening archive %s"), filename);
		return;
	}
	gOpenArchives.push_back(&mpq_a);
}

MPQArchive::~MPQArchive()
{
	/*
	for(ArchiveSet::iterator i=gOpenArchives.begin(); i!=gOpenArchives.end();++i)
	{
		mpq_archive &mpq_a = **i;

		free(mpq_a.header);
	}
	*/
	//gOpenArchives.erase(gOpenArchives.begin(), gOpenArchives.end());
}

void MPQArchive::close()
{
	libmpq_archive_close(&mpq_a);
	for(ArchiveSet::iterator it=gOpenArchives.begin(); it!=gOpenArchives.end();++it)
	{
		mpq_archive &mpq_b = **it;
		if (&mpq_b == &mpq_a) {
			gOpenArchives.erase(it);
			//delete (*it);
			return;
		}
	}

}

MPQFile::MPQFile(const char* filename, bool bUseLocalFiles):
	eof(false),
	buffer(0),
	pointer(0),
	size(0)
{
	m_bUseLocalFiles = bUseLocalFiles;
	if(m_bUseLocalFiles) {

		std::FILE* fp = fopen(filename, "rb");
		if(fp)
		{
			fseek(fp, 0, SEEK_END);
			size = ftell(fp);
			//
			if (size<=1) {
				eof = true;
				buffer = 0;
				return;
			}

			fseek(fp, 0, SEEK_SET);
			buffer = new unsigned char[size];
			fread(buffer,size,1,fp);
			fclose(fp);
			return;
		}

		//String fn;

		//fn = gamePath;
		//fn.Append(filename);

		//if (wxFile::Exists(fn.fn_str())) {
		//	// success
		//	wxFile file;
		//	// if successfully opened
		//	if (file.Open(fn.fn_str(), wxFile::read)) {
		//		size = file.Length();
		//		if (size > 0) {
		//			buffer = new unsigned char[size];
		//			// if successfully read data
		//			if (file.Read(buffer, size) > 0) {
		//				eof = false;
		//				return;
		//			} else {
		//				wxDELETEA(buffer);
		//				eof = true;
		//				size = 0;
		//			}
		//		}
		//	}
		//}
	}

	for(ArchiveSet::iterator i=gOpenArchives.begin(); i!=gOpenArchives.end(); ++i)
	{
		mpq_archive &mpq_a = **i;
		int fileno = libmpq_file_number(&mpq_a, filename);
		if(fileno == LIBMPQ_EFILE_NOT_FOUND)
			continue;

		// Found!
		size = libmpq_file_info(&mpq_a, LIBMPQ_FILE_UNCOMPRESSED_SIZE, fileno);

		// HACK: in patch.mpq some files don't want to open and give 1 for filesize
		if (size<=1) {
			eof = true;
			buffer = 0;
			return;
		}

		buffer = new unsigned char[size];
 		libmpq_file_getdata(&mpq_a, fileno, buffer);
		return;
	}

	eof = true;
	buffer = 0;
}

MPQFile::~MPQFile()
{
	close();
}

bool MPQFile::exists(const char* filename)
{
//	if(m_bUseLocalFiles) {
		//wxString fn = gamePath;
		//fn.Append(filename);
		//if (wxFile::Exists(fn.fn_str()))
//		//	return true;
//	}

	for(ArchiveSet::iterator i=gOpenArchives.begin(); i!=gOpenArchives.end();++i)
	{
		mpq_archive &mpq_a = **i;
		int fileno = libmpq_file_number(&mpq_a, filename);
		if (fileno != LIBMPQ_EFILE_NOT_FOUND) 
			return true;
	}

	return false;
}

size_t MPQFile::read(void* dest, size_t bytes)
{
	if (eof) 
		return 0;

	size_t rpos = pointer + bytes;
	if (rpos > size) {
		bytes = size - pointer;
		eof = true;
	}

	memcpy(dest, &(buffer[pointer]), bytes);

	pointer = rpos;

	return bytes;
}

bool MPQFile::isEof()
{
    return eof;
}

void MPQFile::seek(int offset)
{
	pointer = offset;
	eof = (pointer >= size);
}

void MPQFile::seekRelative(int offset)
{
	pointer += offset;
	eof = (pointer >= size);
}

void MPQFile::close()
{
	S_DEL(buffer);
	eof = true;
}

size_t MPQFile::getSize()
{
	return size;
}

int MPQFile::getSize(const char* filename)
{
	//if(m_bUseLocalFiles) {
		//wxString fn = gamePath;
		//fn.Append(filename);
		//if (wxFile::Exists(fn.fn_str())) {
		//	wxFile file(fn);
		//	return file.Length();
		//}
	//}

	for(ArchiveSet::iterator i=gOpenArchives.begin(); i!=gOpenArchives.end();++i)
	{
		mpq_archive &mpq_a = **i;
		int fileno = libmpq_file_number(&mpq_a, filename);
		if (fileno != LIBMPQ_EFILE_NOT_FOUND)
			return libmpq_file_info(&mpq_a, LIBMPQ_FILE_UNCOMPRESSED_SIZE, fileno);
	}

	return 0;
}

size_t MPQFile::getPos()
{
	return pointer;
}

unsigned char* MPQFile::getBuffer()
{
	return buffer;
}

unsigned char* MPQFile::getPointer()
{
	return buffer + pointer;
}

void getFileLists(std::set<FileTreeItem> &dest, bool filterfunc(std::string))
{
	for(ArchiveSet::iterator i=gOpenArchives.begin(); i!=gOpenArchives.end();++i)
	{
		mpq_archive &mpq_a = **i;
		int fileno = libmpq_file_number(&mpq_a, "(listfile)");

		if(fileno != LIBMPQ_EFILE_NOT_FOUND) {
			// Found!
			size_t size = libmpq_file_info(&mpq_a, LIBMPQ_FILE_UNCOMPRESSED_SIZE, fileno);
			int retVal = libmpq_file_info(&mpq_a, LIBMPQ_FILE_COMPRESSION_TYPE, fileno);

			std::string temp = mpq_a.filename;
			//temp = temp.MakeLower();
			transform(temp.begin(),   temp.end(),   temp.begin(),   tolower);   
			int col = 0;
			if (temp.find("patch.mpq") != std::string::npos)
				col = 1;
			else if (temp.find("patch-2.mpq") != std::string::npos)
				col = 2;

			if (temp.find("wowtest") > -1)
				col = 3;

			// TODO: Add handling for uncompressed files.
			//if () {
			if (size > 0 && retVal != 0) {
				unsigned char *buffer = new unsigned char[size];
				libmpq_file_getdata(&mpq_a, fileno, buffer);
				unsigned char *p = buffer, *end = buffer + size;

				while (p <= end) {
					unsigned char *q=p;
					do {
						if (*q==13) 
							break;
					} while (q++<=end);

					std::string line((char*)p,q-p);
					if (line.length()==0) 
						break;

					//p += line.length();
					p = q + 2;
					//line.erase(line.length()-2, 2); // delete \r\n

					if (filterfunc(line.c_str())) {

						// This is just to help cleanup Duplicates
						// Ideally I should tokenise the string and clean it up automatically
						transform(line.begin(),   line.end(),   line.begin(),   tolower);   
						line[0] -= 32;
						int ret = line.find('\\');
						if (ret>-1)
							line[ret+1] -= 32;

						FileTreeItem tmp;
						tmp.fn = line;
						tmp.col = col;
						dest.insert(tmp);
					}
				}

				S_DEL(buffer);
				p = NULL;
				end = NULL;
			}
		}
	}
}
void SetGamePath(std::string strGamePath)
{
	testPath = strGamePath;
	gamePath = strGamePath;
}
void InitMPQArchives()
{
	const std::string dataArchives[] = {"texture.MPQ", "model.MPQ", "wmo.MPQ", "misc.MPQ"}; //, "terrain.MPQ"
	const std::string localeArchives[] = {"dbc.MPQ", "interface.MPQ"};
	const std::string patchArchives[] = {"patch-2.MPQ", "patch.MPQ"};

	std::string path;
	std::string tempPath;

	//if (useELP) {
	//	tempPath = gamePath.BeforeLast('\\');
	//	tempPath = tempPath.BeforeLast('\\');
	//	tempPath.Append(_T("\\WoW.enGB\\Data\\"));
	//} else if (useFLP) {
	//	tempPath = gamePath.BeforeLast('\\');
	//	tempPath = tempPath.BeforeLast('\\');
	//	tempPath.Append(_T("\\WoW.frFR\\Data\\"));
	//} else if (useGLP) {
	//	tempPath = gamePath.BeforeLast('\\');
	//	tempPath = tempPath.BeforeLast('\\');
	//	tempPath.Append(_T("\\WoW.deDE\\Data\\"));
	//}

	// If they're using a language pack,  load it
	//if (!tempPath.IsEmpty()) {
		//wxLogMessage(_T("Info: Language pack support temporarilly disabled until it is made more 'User Friendly'."));

		/*
		if (usePatch) {
		// load patches first
		for (size_t i=0; i<WXSIZEOF(patchArchives); i++) {
		path = tempPath;
		path.append(patchArchives[i]);
		archives.push_back(new MPQArchive(path.c_str()));
		}
		}

		// archives containing language specific text
		for (size_t i=0; i<WXSIZEOF(localeArchives); i++) {
		path = tempPath;
		path.append(localeArchives[i]);
		archives.push_back(new MPQArchive(path.c_str()));
		}
		*/
	//}


	if (bUseTestPatch) {
		// test patch goes first if available
		for (size_t i=0; i<SIZEOF(patchArchives); i++) {
			path = testPath;
			path.append(patchArchives[i]);
			//if (wxFileName::FileExists(path))
			if (access(path.c_str(), 0) == 0)
				archives.push_back(new MPQArchive(path.c_str()));
		}
	}

	if (bUsePatch) {
		// patch goes next -> fake priority handling
		for (size_t i=0; i<SIZEOF(patchArchives); i++) {
			path = gamePath;
			path.append(patchArchives[i]);
			//if (wxFileName::FileExists(path))
			if (access(path.c_str(), 0) == 0)
				archives.push_back(new MPQArchive(path.c_str()));
		}
	}

	// archives containing language specific text
	for (size_t i=0; i<SIZEOF(localeArchives); i++) {
		path = gamePath;
		path.append(localeArchives[i]);
		//if (wxFileName::FileExists(path))
	if (access(path.c_str(), 0) == 0)
			archives.push_back(new MPQArchive(path.c_str()));
	}

	// data archives
	for (size_t i=0; i<SIZEOF(dataArchives); i++) {
		path = gamePath;
		path.append(dataArchives[i]);
		//if (wxFileName::FileExists(path))
		if (access(path.c_str(), 0) == 0)
			archives.push_back(new MPQArchive(path.c_str()));
	}

	// Checks and logs the "TOC" version of the files that were loaded
	MPQFile f("Interface\\FrameXML\\FrameXML.TOC",false);
	if (!f.isEof()) {
		f.seek(51);
		unsigned char toc[6];
		memset(toc,'\0', 6);
		f.read(toc, 6);		
		//wxLogMessage(_T("Loaded Content Data: v%c.%c%c.%c%c\n"), toc[0], toc[1], toc[2], toc[3], toc[4]);
	}
}

bool filterModels(std::string s)
{
	//s.LowerCase();
	const size_t len = s.length();
	if (len < 4) return false;

	//return ((s[len-2]=='m' && s[len-1]=='2') || (s[len-3]=='w' && s[len-2]=='m'));
	//return true;
	return (
		((s[len-2]=='M' || s[len-2]=='m') && (s[len-1]=='2')) ||
		((s[len-3]|0x20)=='w' && (s[len-2]|0x20)=='m' && (s[len-1]|0x20)=='o') 
		);
}

bool filterNpcs(std::string s)
{
	// textures\BakedNpcTextures\*.*
	if (s.length() < 18) return false;
	return (s.substr(9, 8) == "BakedNpc");
}