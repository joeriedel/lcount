// lcount.cpp : Defines the entry point for the console application.
//

#include <stdio.h>
#include <stdio.h>
#include "filesearch.h"
#include "cmdlib.h"
#include <conio.h>

int main(int argc, char* argv[])
{
	char file[1024];
	int nNumLines=0, nTotalLines=0;
	int nNumFiles=0;
	FILE* fp;
	int c;
	CFileSearch search;

	if( search.BeginFileSearch( ".\\", "*.*", true ) )
	{	
		for(;;)
		{
			if(search.GetNextFile(file))
				continue;
			if( file[0] == 0 )
				break;
			
			if(!CompareExt(file, ".c")&&
			   !CompareExt(file, ".s")&&
			   !CompareExt(file, ".x86")&&
			   !CompareExt(file, ".cpp")&&
			   !CompareExt(file, ".cs")&&
			   !CompareExt(file, ".h"))
				continue;
			
			fp = fopen(file, "rt");
			if(fp == NULL)
			{
				printf("Error opening file %s\n", file);
				continue;
			}

			nNumLines=0;
			while((c = fgetc(fp)) != EOF)
			{
				if((char)c == '\n')
					nNumLines++;
			}

			fclose(fp);
			nNumLines++;
			nNumFiles++;
			nTotalLines += nNumLines;
			printf("%s: %d\n", file, nNumLines);
		}
		
		search.EndFileSearch();
	}

	printf("%d file(s), %d line(s)\n", nNumFiles, nTotalLines);
	fflush( stdout );
	while( !kbhit() ) {};

	return 0;
}
