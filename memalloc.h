// MEMALLOC.H - Memory allocation routines.
// Copyright © 1997 Joe Riedel.
// Author: Joe Riedel.

// These routines are provided for increased portability,
// and I suspect that when using MFC malloc only has a 64K window. 
// This allows you to easily change malloc calls to GlobalAlloc
// calls, without going through thousands of lines of code.

#ifndef MEMALLOC_H
#define MEMALLOC_H

#ifdef __cplusplus
extern "C" {
#endif

void* MemAlloc(unsigned int size);	// Allocates mem, NOTE: Initializes it.
void  MemFree(void* data);			// frees mem.
void* MemRealloc(void* data, unsigned int size);
void* Q_MemAlloc(unsigned int size);	// Quick memalloc.

#ifdef __cplusplus
}
#endif

#endif