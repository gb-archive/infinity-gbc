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

char tiles[64 * 64];
char pal_indexes[64];
char pal[64];
int num;

void main()
{
	FILE *out, *f;
	char outfile[256], str[256];
	int size;
	int n;
	int total;

	printf("Filename (name it a .SET) : "); fflush(stdout);
	gets(outfile);

	/*printf("Number of tiles in the set: "); fflush(stdout);
	gets(str);
	num = atoi(str);*/

	total = 0;
	while(1) {
		printf(".BIN file of the tiles (q to quit): "); fflush(stdout);
		gets(str);
		if(!strcmp(str, "q"))
			break;
		f = fopen(str, "rb");
		fseek(f, 0l, SEEK_END);
		size = ftell(f);
		rewind(f);

		num = size / 65;

		fread(pal_indexes + total, num, 1, f);
		fread(tiles + total * 64, num * 64, 1, f);
		fclose(f);

		total += num;
	}
	num = total;

	printf("Palette file for this set (that you made with MAKEPAL): "); fflush(stdout);
	gets(str);
	f = fopen(str, "rb");
	fread(pal, 64, 1, f);
	fclose(f);
	/*for(n = 0; n < 64; ++n) {
		pal[n] |= 8;
	}*/

	out = fopen(outfile, "wb");
	fwrite(pal, 64, 1, out);
	fwrite(pal_indexes, 64, 1, out);
	fwrite(tiles, 64 * 64, 1, out);
	fclose(out);
}
