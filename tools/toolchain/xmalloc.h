/*
 * Copyright (C) 1999-2001 Affinix Software, LLC
 *
 * This file is part of Infinity.
 *
 * This file may be used under the terms of the Creative Commons Attribution-
 * NonCommercial-ShareAlike 4.0 International License as published by Creative
 * Commons.
 *
 * Alteratively, this file may be used under the terms of the GNU General
 * Public License as published by the Free Software Foundation, either version
 * 3 of the License, or (at your option) any later version.
 *
 * In addition, as a special exception, Affinix Software gives you certain
 * additional rights. These rights are described in the LICENSE file in this
 * package.
 */

#include<stdio.h>
#include<string.h>
#include<malloc.h>

void *xrealloc(void *ptr, int size)
{
	ptr = ptr ? realloc(ptr, size) : malloc(size);
	if(ptr == NULL) {
		printf("xrealloc: out of memory.\n");
		exit(1);
	}
	return ptr;
}

char *xstrdup(char *ptr)
{
	ptr = strdup(ptr);
	if(ptr == NULL) {
		printf("xstrdup: out of memory.\n");
		exit(1);
	}
	return ptr;
}
