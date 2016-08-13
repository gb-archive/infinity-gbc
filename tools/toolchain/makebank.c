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
#include<malloc.h>

char *tiles;
char pal_indexes[1024];
int num;
char buf[16384];

void main()
{
	FILE *out, *f;
	char outfile[256], str[256];
	int size;
	int n;
	int total;
	int pages;
	char fname[256];
	int ftype;

	tiles = malloc(16384 * 64);
	if(!tiles)
		exit(0);

	total = 0;
	while(1) {
		printf("[Type] [BIN file] (q to quit): "); fflush(stdout);
		gets(str);

		p = strtok(str, " \t\n");
		if(!p)
			continue;
		if(!strcmp(p, "q"))
			break;
		else if(!strcmp(p, "c"))
			ftype = 1;
		else if(!strcmp(p, "e"))
			ftype = 2;
		else if(!strcmp(p, "n"))
			ftype = 3;
		else {
			printf("bad type (use c, e, or n)\n");
			continue;
		}
		p = strtok(NULL, "\t\n");
		if(!p)
			continue;
		strcpy(fname, p);

		// open the file
		f = fopen(fname, "rb");
		if(!f) {
			printf("can't open [%s]!\n", fname);
			continue;
		}

		fseek(f, 0l, SEEK_END);
		size = ftell(f);
		rewind(f);

		// characters
		if(ftype == 1) {
			num = size / 65;
			fread(pal_indexes + total, num, 1, f);

			fread(tiles + total * 64, num * 64, 1, f);
			fclose(f);

			total += 40;
		}
		if(ftype == 2) {
			num = size / 65;
			fread(pal_indexes + total, num, 1, f);

			fread(tiles + total * 64, num * 64, 1, f);
			fclose(f);

			total += 40;
		}
		if(ftype == 3) {
			num = size / 65;
			fread(pal_indexes + total, num, 1, f);

			fread(tiles + total * 64, num * 64, 1, f);
			fclose(f);

			total += 40;
		}
	}
	num = total;

	out = fopen("table.pag", "wb");
	buf[0] = 6;
	buf[1] = 64;
	buf[2] = 0;
	memcpy(buf+64, pal_indexes, 1024);
	fwrite(buf, 16384, 1, out);
	fclose(out);

	printf("wrote table.pag\n");

	pages = num / 256;
	if(num % 256 > 0)
		++pages;

	for(n = 0; n < pages; ++n) {
		sprintf(str, "spr%02d.pag", n);
		out = fopen(str, "wb");
		fwrite(tiles + n * 16384, 16384, 1, out);
		fclose(out);

		printf("wrote %s\n", str);
	}
}
