/*
filesearch.cpp
*/
#include "filesearch.h"

CFileSearch::CFileSearch()
{
	m_pSubSearch = 0;
	
#ifdef _WIN32
	m_nHandle = -1;
#endif

}

CFileSearch::~CFileSearch()
{
	EndFileSearch();
}

bool CFileSearch::BeginFileSearch(const char* sDir, const char* sFileExt, bool bRecursive)
{
	char temp[1024];

	strcpy( m_sDir, sDir );
	strcpy( m_sFileExt, sFileExt );
	m_bRecursive = bRecursive;
	m_pSubSearch = NULL;
	
	strcpy( temp, sDir );
	strcat( temp, "\\" );
	strcat( temp, sFileExt );

	m_bFirstFind = true;
	m_nHandle = _findfirst(temp, &m_fileinfo);
	if(m_nHandle == -1)
		return false;

	return true;
}

void CFileSearch::EndFileSearch()
{
	KillRecursiveSearch();
	if(m_nHandle > -1)
		_findclose(m_nHandle);
}

bool CFileSearch::SetupRecursiveSearch()
{
	m_pSubSearch = new CFileSearch();

	char temp[1024];

	strcpy( temp, m_sDir );
	strcat( temp, "\\" );
	strcat( temp, m_fileinfo.name );

	if(m_pSubSearch->BeginFileSearch(temp, m_sFileExt, true))
		return true;

	KillRecursiveSearch();
	return false;
}

bool CFileSearch::GetNextRecursiveFile(char* sFilename)
{
	m_pSubSearch->GetNextFile(sFilename);
	if(sFilename[0] == 0)
		return false;

	char temp[1024];
	strcpy( temp, m_fileinfo.name );
	strcat( temp, "\\" );
	strcat( temp, sFilename );

	strcpy( sFilename, temp );

	return false;
}

bool CFileSearch::KillRecursiveSearch()
{
	if(m_pSubSearch != NULL)
		delete m_pSubSearch;
	m_pSubSearch = NULL;
	return true;
}

bool CFileSearch::GetNextFile(char* sFilename)
{
	// Doing a recursive search?
	if(m_pSubSearch)
	{
		GetNextRecursiveFile(sFilename);
		if(sFilename[0] == 0)
		{
			KillRecursiveSearch();
			return GetNextFile(sFilename);
		}

		return false;
	}
	
	if(m_bFirstFind)
	{
		m_bFirstFind = false;
		strcpy( sFilename, m_fileinfo.name );
		if(!stricmp(sFilename, "..") || !stricmp(sFilename, "."))
			return GetNextFile(sFilename);
		
		if(m_bRecursive && (m_fileinfo.attrib&_A_SUBDIR))
		{
			if(SetupRecursiveSearch())
			{
				// Get next file.
				GetNextRecursiveFile(sFilename);
				if(sFilename[0] == 0)
				{
					KillRecursiveSearch();
					return GetNextFile(sFilename);
				}
			}
			
			return GetNextFile(sFilename);
		}

		return (m_fileinfo.attrib & _A_SUBDIR) != 0 ? true : false;
	}

	m_fileinfo.name[0] = 0;

	while (!m_fileinfo.name[0]) {
		if(_findnext(m_nHandle, &m_fileinfo) == -1)
		{
			sFilename[0] = 0;
			return false;
		}
	}

	strcpy( sFilename, m_fileinfo.name );
	if(!stricmp(sFilename, "..") || !stricmp(sFilename, "."))
		return GetNextFile(sFilename);

	if(m_bRecursive && (m_fileinfo.attrib&_A_SUBDIR))
	{
		if(SetupRecursiveSearch())
		{
			// Get next file.
			GetNextRecursiveFile(sFilename);
			if(sFilename == "")
			{
				KillRecursiveSearch();
				return GetNextFile(sFilename);
			}
		}
		
		return GetNextFile(sFilename);
	}

	return (m_fileinfo.attrib & _A_SUBDIR) != 0 ? true : false;

}