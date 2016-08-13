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
#include<stdlib.h>
#include<string.h>
#include"xmalloc.h"

int tres_at;

char *insert(char *buf, char *p1, char *p2, char *str)
{
	int len, len2, offset, deep, size;
	int x1, x2;

	x1 = p1 - buf;
	x2 = p2 - buf;
	size = strlen(buf) + 1;

	len = x2 - x1;
	len2 = strlen(str);
	offset = abs(len2 - len);
	deep = x1;

	/* alter the size of the buffer */
	if(len > len2) {
		memmove(p1 + len2, p2, size - len2 - x1);
		buf = xrealloc(buf, size - offset);
		if(!buf)
			return NULL;
		p1 = buf + x1;
		p2 = buf + x2;
		size -= offset;
	}
	else if(len < len2) {
		buf = xrealloc(buf, size + offset);
		if(!buf)
			return NULL;
		p1 = buf + x1;
		p2 = buf + x2;
		memmove(p1 + offset, p1, size - deep);
		size += offset;
	}
	p1 = buf + x1;

	strncpy(p1, str, len2);
	p1 += len2;

	return buf;
}

void parse(char *fname)
{
	FILE *f;
	char **line = NULL;
	char str[256], old[256];
	int num = 0;
	int n, len;
	int changed;
	char *p, *p2;

	printf("scanning: [%s]\n", fname);

	// read in the lines
	f = fopen(fname, "r");
	if(!f) {
		printf("Unable to open: [%s]\n", fname);
		exit(0);
	}
	while(1) {
		fgets(str, 255, f);
		if(feof(f))
			break;
		if(str[strlen(str)-1] == '\n')
			str[strlen(str)-1] = 0;
		line = xrealloc(line, sizeof(char *) * (num + 1));
		line[num] = xstrdup(str);
		++num;
	}
	fclose(f);
	changed = 0;

	// scan for ctres() usage
	for(n = 0; n < num; ++n) {
		if(strstr(line[n], "#define"))
			continue;

		p = strstr(line[n], "ctres(");
		if(p) {
			p += 6;
			p2 = strchr(p, ',');
			if(!p2) {
				printf("line %4d: [%s]\n", n, line[n]);
				printf("error: no comma?\n");
				exit(0);
			}

			len = p2 - p;
			strncpy(old, p, len);
			old[len] = 0;

			sprintf(str, "$%02x", tres_at);
			++tres_at;

			if(strcmp(old, str)) {
				line[n] = insert(line[n], p, p2, str);
				changed = 1;
			}
			continue;
		}
		p = strstr(line[n], "#include");
		if(p) {
			p += 8;
			strcpy(str, p);
			p = strtok(str, "\" ");
			if(!p) {
				printf("line %4d: [%s]\n", n, line[n]);
				printf("error: include file name\n");
				exit(0);
			}
			parse(p);
			continue;
		}
	}

	// save the file
	if(changed) {
		f = fopen(fname, "w");
		for(n = 0; n < num; ++n)
			fprintf(f, "%s\n", line[n]);
		fclose(f);
		printf("saved: [%s]\n", fname);
	}

	// free the data
	for(n = 0; n < num; ++n) {
		free(line[n]);
	}
	free(line);
	line = NULL;
}

main(int argc, char *argv[])
{
	if(argc < 2) {
		printf("tres <.eve>\n");
		exit(0);
	}

	tres_at = 0;
	parse(argv[1]);

	printf("Treasure chests: %d\n", tres_at);
}
