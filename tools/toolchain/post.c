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

void rcr(char *s)
{
	int n;

	n = strlen(s);
	if(s[n-1] == '\n')
		s[n-1] = 0;
}

int main(int argc, char *argv[])
{
	FILE *in, *out;
	char line[256];
	char *p;

	in = stdin;
	out = stdout;

	while(1) {
		fgets(line, 255, in);
		if(feof(in))
			break;
		rcr(line);

		p = strchr(line, ';');
		if(p)
			*p = 0;
		p = strchr(line, '#');
		if(p)
			*p = 0;
		p = strstr(line, "..");
		if(p)
			*p = 0;

		if(strlen(line) < 1)
			continue;

		fprintf(out, "%s\n", line);
	}

	fprintf(out, "\n    .end\n");
	fclose(in);
	fclose(out);
}
