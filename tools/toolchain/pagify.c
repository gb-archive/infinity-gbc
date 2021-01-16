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
#include<string.h> // memset

#if defined(__MACH__) && defined(__APPLE__)
#include<stdlib.h>
#else
#include<malloc.h>
#endif

//#include<mem.h>

int main(int argc, char *argv[])
{
	FILE *in, *out;
	char *buf;
	char str[256];
	int n, x;

	buf = malloc(16384);
	if(!buf)
		return 1;

	in = fopen(argv[1], "rb");
	for(n = 0; !feof(in); ++n) {
		memset(buf, 0, 16384);
		x = fread(buf, 1, 16384, in);
		if(x < 0)
			continue;
		sprintf(str, "%s%02d.pag", argv[2], n);
		out = fopen(str, "wb");
		fwrite(buf, 16384, 1, out);
		fclose(out);
		printf("wrote %s.\n", str);
	}
	fclose(in);
}
