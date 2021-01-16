/*
 * Copyright (C) 1999-2001 Affinix Software, LLC
 *
 * This file is part of Infinity.
 *
 * This file may be used under the terms of the Creative Commons Attribution-
 * NonCommercial-ShareAlike 4.0 International License as published by Creative
 * Commons.
 *
 * Alternatively, this file may be used under the terms of the GNU General
 * Public License as published by the Free Software Foundation, either version
 * 3 of the License, or (at your option) any later version.
 *
 * In addition, as a special exception, Affinix Software gives you certain
 * additional rights. These rights are described in the LICENSE file in this
 * package.
 */

#include<stdio.h>
#include<stdlib.h> // exit, atoi

char buf[16384];

int main(int argc, char *argv[])
{
	FILE *f;
	int pagenum;

	if(argc < 4)
		return 1;

	pagenum = atoi(argv[3]);

	f = fopen(argv[2], "rb");
	if(!f)
		return 1;
	fread(buf, 16384, 1, f);
	fclose(f);

	f = fopen(argv[1], "r+b");
	if(!f)
		return 1;
	fseek(f, pagenum * 16384, SEEK_SET);
	fwrite(buf, 16384, 1, f);
	fclose(f);

	printf("Wrote %s into page %d.\n", argv[2], pagenum);
	return 0;
}
