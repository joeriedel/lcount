// MEMALLOC.CPP - memalloc routines.
// Copyright © 1997 Joe Riedel.
// Author: Joe Riedel.
#include "cmdlib.h"
#include <malloc.h>
#include "memalloc.h"

// Quick mem alloc.
void* Q_MemAlloc(unsigned int size)
{
	return(malloc(size));
}

void* MemAlloc(unsigned int size)
{
	void* data;
	
	data = malloc(size);
	if(!data)
		return(NULL);

	// Initialize it.
	memset(data, 0, size);

	return(data);
}

void MemFree(void* data)
{
	free(data);
}

void* MemRealloc(void* data, unsigned int size)
{
	return(realloc(data, size));
}
