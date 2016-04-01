// CMDLIB.CPP - Command library for games.
// Copyright © 1998 Joe Riedel.
// Author: Joe Riedel.
#include "cmdlib.h"

gbool com_eof;
char com_token[CMD_MAX_TOKEN_LEN];
char token[CMD_MAX_TOKEN_LEN];

static char* script_ptr = 0;
static gbool unget = false;
int scriptline = 1;
static gbool firstfind;
static struct _finddata_t fileinfo;
static int handle = -1;

gbool FindCommandLineParm( const char* parm, int argc, const char* argv[] )
{
	int i;
	
	for(i = 0; i < argc; i++)
	{
		if( stricmp( argv[i], parm ) == 0 )
			return true;
	}
	
	return false;
}

gbool GetCommandLineParm( const char* parm, char* value, int argc, const char* argv[] )
{
	int i;
	
	value[0] = 0;
	
	for(i = 0; i < argc; i++)
	{
		if( stricmp( argv[i], parm ) == 0 )
		{
			if( i+1 < argc )
			{
				strcpy( value, argv[i+1] );
				return true;
			}
			
			return false;
		}
	}
	
	return false;
}


void ConvertUnixPath(char* szString)
{
	int i;
	int l = strlen(szString);
	
	for(i = 0; i < l; i++)
	{
		if(szString[i] == '\\')
			szString[i] = '/';
	}
}

void StringReplaceChar( char* string, char src, char dst )
{
	int i;
	int l = strlen( string );
	
	for(i = 0; i < l; i++)
	{
	
		if( string[i] == src )
			string[i] = dst;
	}	
}

// Returns true if the extensions match.
gbool CompareExt(const char* szString, const char* szExt)
{
	int nStrLen, nExtLen;

	nStrLen = strlen(szString);
	nExtLen = strlen(szExt);

	if(nStrLen < nExtLen)
		return false;

	return !strncmp(&szString[nStrLen-nExtLen], szExt, nExtLen);
}

double GetTime(void)
{
	time_t t;

	time(&t);
	return t;
}

// Returns the filename from a path.
void GetFileNameFromPath(char* file, char* path)
{
	int i, l, k, pl;

	file[0] = 0;

	pl = strlen(path);
	l = 0;
	i = pl-1;
	while(path[i--] != '\\')
	{ l++; }

	// Fill with 0S.
	memset(file, 0, l+1);

	for(i = 0, k = l; i < l; i++, k--)
		file[i] = path[pl-k];
}

// Returns the path from the filename
void GetPathFromFileName(char* path, char* file)
{
	int i, len;

	path[0] = 0;

	len = strlen( file );
	if( len < 1 )
		return;

	i = len;
	while( i>0 && file[i-1] != ':' && file[i-1] != '\\' && file[i-1] != '/' )
	{
		i--;
	}

	len = i;
	memset( path, 0, len );
	
	for(i = 0; i < len; i++)
		path[i] = file[i];

	path[len] = 0;
}

// Returns true if the string has the character in it.
gbool StringHasChar(char* string, char c)
{
	int i, l;

	l = strlen(string);
	for(i = 0; i < l; i++)
	{
		if(string[i] == c)
			return(true);
	}

	return(false);
}

// Changes file extension.
void ChangeFileExtension(char* dst, char* src, char* ext)
{
	int len = 0;
	int extlen = 0;
	gbool hasext = false;
	char temp[1024];

	// Make sure the string has an extension.
	while(src[len] != 0)
	{
		if(src[len] == '.')
			hasext = true;
		
		if(hasext)
			extlen++;

		len++;
	}

	// Subtract string.
	memset(temp, 0, 1024);
	
	if(hasext == true)
		strncpy(temp, src, strlen(src) - extlen);
	else
		strcpy(temp, src);

	if(ext) { strcat(temp, ext); }

	strcpy(dst, temp);
}

// Returns true if string has a * in it.
gbool StringHasWildCard(char* string)
{
	int len = 0;

	while(string[len] != 0)
	{
		if(string[len] == '*')
			return(true);

		len++;
	}

	return(false);
}

// NOTE: Parsing only supports // style comments.
// Parses a token out of a string.
gbool GetStringToken(char* string)
{
	int len=0;
	char c;
	com_eof = false;

getstringtoken_skipwhite:

	// Skip white space.
	while((c = *string) <= 32)
	{
		if(c == 0)
		{
			com_eof = true;
			return(false);
		}

		string++;
	}

	// Skip comments.
	if(string[0] == '/' && string[1] == '/')
	{
		while(*string != '\n')
			string++;

		goto getstringtoken_skipwhite;
	}
	if( string[0] == '/' && string[1] == '*' )
	{
		while( string && *string && (string[0] != '*' && string[1] != '/') )
			string++;
			
		if( !string || !(*string) )
		{
			com_eof = true;
			return false;
		}
		
		string++;
		
		if( !string || !(*string) )
		{
			com_eof = true;
			return false;
		}
		
		goto getstringtoken_skipwhite;
	}
	
	// Handle quoted strings.
	if(c == '"')
	{
		string++;
		while(*string != '"')
		{
			com_token[len++] = *string++;
			if(len == CMD_MAX_TOKEN_LEN-1)
				Error("GetStringToken(): string literal exceeds max length");

		}

		com_token[len] = 0;
		return(true);
	}

	// Handle single characters.
	if (c=='{' || c=='}'|| c==')'|| c=='(' || c=='\'' || c==':')
	{
		com_token[len++] = c;
		com_token[len] = 0;
		return true;
	}
	
	// Handle normal word.
	string++;
	while(c > 32)
	{
		if (c=='{' || c=='}'|| c==')'|| c=='(' || c=='\'' || c==':')
			break;
		com_token[len++] = c;
		c = *string++;
	}

	com_token[len] = 0;
	return(true);
}

// Begins parsing of text.
void BeginTokenParsing(char* text)
{
	script_ptr = text;
	unget = false;
	scriptline = 1;
}

// Get a token out of a string.
gbool GetToken(void)
{
	char c;
	char* token_ptr;
	
	if(unget == true)
	{
		unget = false;
		return(true);
	}

	token[0] = 0;	// Init to 0.
	// Skip white space.
gettoken_skipwhite:

	while((c = *script_ptr) <= 32)
	{
		if(c == '\n')
			scriptline++;
		else if(c == 0)
			return false;

		script_ptr++;
	}

	// Skip comments.
	if(script_ptr[0] == '/' && script_ptr[1] == '/')
	{
		while(*script_ptr != '\n')
		{
			if(script_ptr == 0 || *script_ptr == 0)
				return false;

			script_ptr++;
		}

		goto gettoken_skipwhite;

	}
	if( script_ptr[0] == '/' && script_ptr[1] == '*' )
	{
		while( script_ptr && *script_ptr && (script_ptr[0] != '*' || script_ptr[1] != '/') )
			script_ptr++;
			
		if( !script_ptr || !(*script_ptr) )
		{
			com_eof = true;
			return false;
		}
		
		script_ptr++;
		
		if( !script_ptr || !(*script_ptr) )
		{
			com_eof = true;
			return false;
		}
		
		script_ptr++;
		
		if( !script_ptr || !(*script_ptr) )
		{
			com_eof = true;
			return false;
		}
		
		goto gettoken_skipwhite;
	}

	// Copy the token.
	token_ptr = token;
	*token_ptr = 0;

	// Get "quoted" token.
	if(*script_ptr == '"')
	{
		script_ptr++;
		// Copy it.
		while(*script_ptr != '"')
		{
			if(*script_ptr == 0)
				return false;

			*token_ptr++ = *script_ptr++;
			if(token_ptr == &token[CMD_MAX_TOKEN_LEN])
				Error("GetToken(): string length too long. Script Line = %d", scriptline);
			*token_ptr = 0;
		}

		script_ptr++;
	}
	else
	{
		// Copy until next space.
		while(*script_ptr > 32)
		{
			*token_ptr++ = *script_ptr++;
			if(token_ptr == &token[CMD_MAX_TOKEN_LEN])
				Error("GetToken(): token length too long. Script Line = %d", scriptline);
			*token_ptr = 0;
		}
	}

	return(true);
}

// Ungettoken.
void UngetToken(void)
{
	unget = true;
}

// Converts a string to upper/lower case.
void StrUpper(char* in)
{
	int i = 0;

	while(in[i] != 0)
	{
		in[i] = toupper(in[i]);
		i++;
	}
}

void StrLower(char* in)
{
	int i = 0;

	while(in[i] != 0)
	{
		in[i] = tolower(in[i]);
		i++;
	}
}

int LoadScriptFileAbort(const char* filename, void** buffer)
{
	int size;
	FILE* fp;

	fp = Z_OpenRead(filename);
	if(!fp)
		Error("unable to open %s\n", filename);

	size = FileSize(fp);
	*buffer = MemAlloc(size+1);
	if(!(*buffer))
	{
		fclose(fp);
		Error("unable to open %s\n", filename);
	}

	if(!Z_ReadFile(*buffer, size, fp))
	{
		MemFree(*buffer);
		*buffer = 0;
		size = 0;
	}

	fclose(fp);
	return(size);
}

// Loads a file into memory.
int LoadFile(const char* filename, void** buffer)
{
	int size;
	FILE* fp;

	fp = Z_OpenRead(filename);
	if(!fp)
		return(0);

	size = FileSize(fp);
	*buffer = MemAlloc(size);
	if(!(*buffer))
	{
		fclose(fp);
		return(0);
	}

	if(!Z_ReadFile(*buffer, size, fp))
	{
		MemFree(*buffer);
		*buffer = 0;
		size = 0;
	}

	fclose(fp);
	return(size);
}

// Aborts if trouble occurs.
int LoadFileAbort(const char* filename, void** buffer)
{
	int size;

	size = LoadFile(filename, buffer);
	if(size == 0)
		Error("LoadFileAbort(): unable to load %s", filename);

	return(size);
}

// Returns true if file exists.
gbool FileExists(const char* filename)
{
	FILE* fp;

	fp = Z_OpenRead(filename);
	if(!fp)
		return(false);

	fclose(fp);
	return(true);
}

// Returns filesize.
int FileSize(FILE* fp)
{
	int pos, end;

	pos = ftell(fp);
	fseek(fp, 0, SEEK_END);
	end = ftell(fp);
	fseek(fp, pos, SEEK_SET);

	return(end);
}

// FILEIO.
// Closes file.
void Z_CloseFile(FILE* fp)
{
	fclose(fp);
}

FILE* Z_OpenRead(const char* filename)
{
	FILE* fp;

	fp = fopen(filename, "rb");
	return(fp);
}

FILE* Z_OpenWrite(const char* filename)
{
	FILE* fp;

	fp = fopen(filename, "wb");
	return(fp);
}

FILE* Z_OpenReadAbort(const char* filename)
{
	FILE* fp;

	fp = Z_OpenRead(filename);
	if(!fp)
		Error("OpenReadAbort(): unable to open %s", filename);

	return(fp);
}

FILE* Z_OpenWriteAbort(const char* filename)
{
	FILE* fp;

	fp = Z_OpenWrite(filename);
	if(!fp)
		Error("OpenWriteAbort(): unable to open %s", filename);

	return(fp);
}

// File Read/Write.
gbool Z_ReadFile(void* buffer, int size, FILE* fp)
{
	if(fread(buffer, 1, size, fp) != (unsigned)size)
		return(false);

	return(true);
}

gbool Z_WriteFile(void* buffer, int size, FILE* fp)
{
	if(fwrite(buffer, 1, size, fp) != (unsigned)size)
		return(false);

	return(true);
}

// Dies if failed.
void Z_ReadFileAbort(void* buffer, int size, FILE* fp)
{
	if(!Z_ReadFile(buffer, size, fp))
		Error("ReadFileAbort(): read failed");
}

void Z_WriteFileAbort(void* buffer, int size, FILE* fp)
{
	if(!Z_WriteFile(buffer, size, fp))
		Error("Z_WriteFileAbort(): write failed");
}

// Dies in console.
void Error (char *error, ...)
{
	va_list argptr;

	printf ("\n************ ERROR ************\n");

	va_start (argptr,error);
	vprintf (error,argptr);
	va_end (argptr);
	printf ("\n");

	exit (1);
}
