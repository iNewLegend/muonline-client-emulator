#pragma once
//#include "SFmpqapi.h"
#include "libmpq/mpq.h"

// C++ files
#include <string>
#include <set>
#include <vector>

struct FileTreeItem {
	std::string fn;
	int col;

	/// Comparison
	bool operator<(const FileTreeItem &i) const {
		return fn < i.fn;
	}

	bool operator>(const FileTreeItem &i) const {
		return fn < i.fn;
	}
};


class MPQArchive
{
	//MPQHANDLE handle;
	mpq_archive mpq_a;
public:
	MPQArchive(const char* filename);
	~MPQArchive();

	void close();
};

typedef std::vector<mpq_archive*> ArchiveSet;
ArchiveSet& GetOpenArchives();

class MPQFile
{
	//MPQHANDLE handle;
	bool eof;
	unsigned char *buffer;
	size_t pointer, size;
	bool m_bUseLocalFiles;

	// disable copying
	//MPQFile(const MPQFile &f) {}
	void operator=(const MPQFile &f) {}

public:
	MPQFile(const char* filename, bool bUseLocalFiles);	// filenames are not case sensitive
	~MPQFile();
	size_t read(void* dest, size_t bytes);
	size_t getSize();
	size_t getPos();
	unsigned char* getBuffer();
	unsigned char* getPointer();
	bool isEof();
	void seek(int offset);
	void seekRelative(int offset);
	void close();

	static bool exists(const char* filename);
	static int getSize(const char* filename); // Used to do a quick check to see if a file is corrupted
};

inline void flipcc(char *fcc)
{
	char t;
	t=fcc[0];
	fcc[0]=fcc[3];
	fcc[3]=t;
	t=fcc[1];
	fcc[1]=fcc[2];
	fcc[2]=t;
}

inline bool defaultFilterFunc(std::string) { return true; }
void getFileLists(std::set<FileTreeItem> &dest, bool filterfunc(std::string) = defaultFilterFunc);
void SetGamePath(std::string);
void InitMPQArchives();

bool filterModels(std::string);
bool filterNpcs(std::string);
