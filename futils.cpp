// FUTILS.CPP
// Copyright © 1999 Joe Riedel, Nick Randal.
// Author: Joe Riedel.
#include "futils.h"
#include <io.h>

bool bFirstFind=true;
struct _finddata_t fileinfo;
int nHandle = -1;

// Bloads a file into memory.
unsigned char* BloadFile(CString sFilename, int& nNumBytes)
{
	CFile hFile;

	nNumBytes = 0;

	if(!hFile.Open(sFilename, CFile::modeRead | CFile::shareDenyWrite | CFile::typeBinary))
		return NULL;

	nNumBytes = hFile.GetLength();
	if(nNumBytes == 0)
	{
		hFile.Close();
		return NULL;
	}

	// Allocate.
	unsigned char* pData = new unsigned char[nNumBytes];
	if(pData == NULL)
	{
		hFile.Close();
		nNumBytes = 0;
		return NULL;
	}

	// Load the file.
	if(hFile.Read(pData, nNumBytes) != (unsigned int)nNumBytes)
	{
		hFile.Close();
		nNumBytes = 0;
		delete[] pData;
		return NULL;
	}

	hFile.Close();

	return pData;
}

// Begins a file search.
bool BeginFileSearch(CString sFileExt)
{
	bFirstFind = true;
	nHandle = _findfirst(sFileExt, &fileinfo);
	if(nHandle == -1)
		return false;

	return true;
}

// Find the next file.
bool GetNextFile(CString& sFilename)
{
	if(bFirstFind)
	{
		bFirstFind = false;
		sFilename = fileinfo.name;
		return (fileinfo.attrib & _A_SUBDIR) != 0 ? true : false;
	}

	if(_findnext(nHandle, &fileinfo) == -1)
	{
		sFilename = "";
		return false;
	}

	sFilename = fileinfo.name;
	return (fileinfo.attrib & _A_SUBDIR) != 0 ? true : false;
}

// End search.
void EndFileSearch(void)
{
	if(nHandle > -1)
		_findclose(nHandle);
}