// CMDLIB.H - Basic command library. Included in all apps.
// Copyright © 1998 Joe Riedel.
// Author: Joe Riedel.

// "Now let's see here. What floats in water?"
// "Tiny li'l pebbles."
// "Wood."
// "Lead, Lead!!!"
// "A duck."
// "..."
// "A duck? Did he just say a duck?... Why he's a bloomin genius he is!"

#ifndef CMDLIB_H
#define CMDLIB_H

#ifdef _WIN32
#pragma warning(disable : 4244)     // truncate from double to float
#pragma warning(disable : 4237)		// true, false extensions.
#pragma warning(disable : 4800)		// forcing int to bool.
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <malloc.h>
#include <io.h>
#include <direct.h>
#include <ctype.h>

#ifdef __cplusplus
extern "C" {
#endif

#include "memalloc.h"	// Alloc lib.

// Note: This has been redefined since I have switched to 5.0
#ifndef __cplusplus
#ifndef __BYTEBOOL__
#define __BYTEBOOL__
typedef enum {false, true} gbool;
//typedef char gbool;
//typedef unsigned char byte;
#endif
#else
typedef bool gbool;
#endif

// EXTERN MACROS.
#define __EXTERN_C	extern "C"
#define __EXPORT	__declspec(dllexport)
#define __EXPFUNC	__EXTERN_C __EXPORT

#define F_S_DW	(0x7FFFFFFF)
#define F_US_DW (0xFFFFFFFF)

// Terminates with an error message.
void Error(char* msg, ...);
void WError(char* msg, ...); // Windowed error.

// File I/O.
// Closes file.
void Z_CloseFile(FILE* fp);
// Opens a file.
FILE* Z_OpenRead(const char* filename);
FILE* Z_OpenWrite(const char* filename);
// Aborts if cannot open.
FILE* Z_OpenReadAbort(const char* filename);
FILE* Z_OpenWriteAbort(const char* filename);
// Reading/Writing.
gbool Z_ReadFile(void* buffer, int size, FILE* fp);
gbool Z_WriteFile(void* buffer, int size, FILE* fp);
// Aborts if write failed.
void Z_ReadFileAbort(void* buffer, int size, FILE* fp);
void Z_WriteFileAbort(void* buffer, int size, FILE* fp);

// Bit o' file info.
int FileSize(FILE* fp);
gbool FileExists(const char* filename);
int LoadFile(const char* filename, void** buffer);
int LoadFileAbort(const char* filename, void** buffer);
int LoadScriptFileAbort(const char* filename, void** buffer);

double GetTime(void);

// Filename searching.
gbool StringHasWildCard(char* string);
gbool StringHasChar(char* string, char c);
// Include the '.', ex ChangeFileExtension(dst, src, ".EXT");
void ChangeFileExtension(char* dst, char* src, char* ext);
// Returns the filename from a path.
void GetFileNameFromPath(char* file, char* path);
// Returns the path from the filename
void GetPathFromFileName(char* path, char* file);

gbool CompareExt(const char* szString, const char* szExt);
void ConvertUnixPath(char* szString);
void StringReplaceChar( char* string, char src, char dst );

gbool FindCommandLineParm( const char* parm, int argc, const char* argv[] );
gbool GetCommandLineParm( const char* parm, char* value, int argc, const char* argv[] );

// Token Parsing.
#define CMD_MAX_TOKEN_LEN 65535
extern gbool com_eof;
extern char com_token[CMD_MAX_TOKEN_LEN];
extern char token[CMD_MAX_TOKEN_LEN];
extern int scriptline;

void UngetToken(void);
gbool GetToken(void);
gbool GetStringToken(char* string);
void BeginTokenParsing(char* text);

// Strings.
// Upper/Lower.
void StrUpper(char* in);
void StrLower(char* in);

#ifdef __cplusplus
}
#endif

#endif