/*
filesearch.h
*/

#ifndef FILESEARCH_H
#define FILESEARCH_H

#include <windows.h>
#include <io.h>

class CFileSearch
{
private:

	CFileSearch* m_pSubSearch;
	bool SetupRecursiveSearch();
	bool GetNextRecursiveFile(char* sFilename);
	bool KillRecursiveSearch();
	char m_sDir[1024], m_sFileExt[64];
	bool m_bRecursive;
	
#ifdef _WIN32
	bool m_bFirstFind;
	struct _finddata_t m_fileinfo;
	int	m_nHandle;
#elif macintosh
	int m_nIndex;
	short m_nVRefNum;
	long m_nDrDirID; 
	CInfoPBRec pbRec;
	char m_sCurFileName[1024];
	
	bool MatchesFileExt(const char* szFile, const C_String& sExt);
	
#endif

public:

	CFileSearch();
	virtual ~CFileSearch();
	
	bool BeginFileSearch(const char* sDir, const char* sFileExt, bool bRecursive=false);
	void EndFileSearch();
	bool GetNextFile(char* sFilename);

};

#endif