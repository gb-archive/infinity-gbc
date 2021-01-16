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
#include<stdlib.h>
#include<string.h>

void main()
{
	int n, atcolor;
	char in_fname[256], out_fname[256], str[256], target[256];
	short color;
	FILE *in, *out;
	char *p;

	printf("Input .H file from GBTD: "); fflush(stdout);
	gets(in_fname);
	printf("Output file: "); fflush(stdout);
	gets(out_fname);

	in = fopen(in_fname, "r");
	out = fopen(out_fname, "wb");
	n = 0;
	atcolor = 0;
	while(n < 8) {
		fgets(str, 255, in);
		if(feof(in))
			break;
		sprintf(target, "CGBPal%dc%d", n, atcolor);
		p = strstr(str, target);	if(!p) continue;
		p = strchr(p, ' ');		if(!p) continue;
		p = strtok(p, " \r\n\t");	if(!p) continue;

		color = atoi(p);

		++atcolor;
		if(atcolor > 3) {
			atcolor = 0;
			++n;
		}
		fwrite(&color, 2, 1, out);
	}

	fclose(in);
	fclose(out);

	if(n < 8) {
		printf("Incomplete .H file.  Only wrote %d/8 palettes.\n", n);
	}
}
