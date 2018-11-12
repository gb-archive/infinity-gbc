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
#include<string.h>
#include<stdlib.h>

FILE *_log;

void mem_error()
{
	printf("memory error!\n");
	exit(1);
}

int highest;
short offsets[1024];
int dataindex;
char *data;

int allocpages;

int numtexts;
int bufsize;
int page, deep;

struct TEXT_BLOCK
{
	int at;
	char *str;
	int len;
};

struct TEXT_BLOCK *text = NULL;

void proc_line(char *s)
{
	char *p;
	int n;
	char str[1024];

	p = strchr(s, ':');
	if(!p)
		return;

	*(p++) = 0;
//	n = atoi(s);
	n = highest + 1;
	fprintf(_log, "#define %s %d\n", s, n);

	strcpy(str, p);

	if(n > highest)
		highest = n;

	text = realloc(text, sizeof(struct TEXT_BLOCK) * (numtexts + 1));
	if(!text)
		mem_error();
	text[numtexts].str = strdup(str);
	if(!text[numtexts].str)
		mem_error();

	text[numtexts].at = n;
	text[numtexts].len = strlen(str) + 1;

	++numtexts;

//	offsets[n] = dataindex;
//	data = realloc(data, dataindex + strlen(str) + 1);
//	strcpy(data + dataindex, str);
//	dataindex += strlen(str) + 1;

//	printf("index = %d\n[%s]\n", n, str);
//	++numtexts;
}

int main(int argc, char *argv[])
{
	FILE *in, *out;
	char *p, *p2;
	char *buf, line[8192];
	int size;
	int n;
	int len;
	int at;

	_log = fopen("etxt.h", "w");

	highest = -1;
	numtexts = 0;
	dataindex = 0;
	data = NULL;

	if(argc < 3)
		exit(0);

	in = fopen(argv[1], "r");
	out = fopen(argv[2], "wb");

	fseek(in, 0l, SEEK_END);
	size = ftell(in);
	rewind(in);
	buf = malloc(size+1);
	if(!buf) {
		printf("mem\n");
		exit(0);
	}
	size = fread(buf, 1, size, in);
	fclose(in);
	buf[size] = 0;

	p = buf;
	while(1) {
		p = strchr(p, '[');
		if(!p)
			break;
		p2 = strchr(p, ']');
		if(!p2)
			break;
		++p;
		len = p2 - p;
		strncpy(line, p, len);
		line[len] = 0;

		proc_line(line);

		p = p2 + 1;
	}
	free(buf);

	++highest;
	bufsize = highest * 3;

	buf = NULL;
	allocpages = 0;
	for(n = 0; n < numtexts; ++n) {
		page = bufsize / 16384;
		deep = bufsize % 16384;

		if(deep + text[n].len > 16384) {
			++page;
			deep = 0;
			bufsize = page * 16384;
		}

		if(allocpages <= page) {
			buf = realloc(buf, (page+1) * 16384);
			if(!buf)
				mem_error();
			memset(&buf[allocpages*16384],0,(page+1-allocpages)*16384);
			allocpages = page + 1;
		}

		at = text[n].at;
		buf[at * 3 + 0] = page;
		buf[at * 3 + 1] = deep & 0xff;
		buf[at * 3 + 2] = (deep >> 8) & 0xff;
		memcpy(buf + bufsize, text[n].str, text[n].len);
		bufsize += text[n].len;
	}

	fwrite(buf, bufsize, 1, out);
	fclose(out);

	printf("Success!  Wrote [%d] strings in a [%d] entry table\n", numtexts, highest);
}
