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
#include<ctype.h>
#include"xmalloc.h"

void readerror()
{
	printf("Error reading line!\n");
	exit(0);
}

int allnumbers(char *s)
{
	int n;

	for(n = 0; n < strlen(s); ++n) {
		if(!isdigit(s[n]))
			return 0;
	}
	return 1;
}

void caps(char *s)
{
	int n;

	for(n = 0; n < strlen(s); ++n) {
		s[n] = toupper(s[n]);
	}
}


struct CNX_LINK {
	int type;
	int map1, map2;
	int x1, y1, x2, y2;
	char *label;
};

struct CNX_EXIT {
	char *label;
	int x, y;
};

struct CNX_LINK *cnx_link = NULL;
struct CNX_EXIT *cnx_exit = NULL;
int cnx_numlinks = 0;
int cnx_numexits = 0;
char cnx_base[256];
char cnx_map[256];

void cnx_setbase(char *base)
{
	printf("base set to [%s]\n", base);
	strcpy(cnx_base, base);
	strcpy(cnx_map, base);
	caps(cnx_map);
}

void cnx_addexit(char *label, int x, int y)
{
	printf("exit: label=[%s], xy=[%02x,%02x]\n", label, x, y);

	cnx_exit = xrealloc(cnx_exit, sizeof(struct CNX_EXIT) * (cnx_numexits + 1));
	cnx_exit[cnx_numexits].label = strdup(label);
	cnx_exit[cnx_numexits].x = x;
	cnx_exit[cnx_numexits].y = y;
	++cnx_numexits;
}

void cnx_addlink(int map1, int x1, int y1, int map2, int x2, int y2)
{
	printf("link: map%d [%02x,%02x] <--> map%d [%02x,%02x]\n", map1, x1, y1, map2, x2, y2);

	cnx_link = xrealloc(cnx_link, sizeof(struct CNX_LINK) * (cnx_numlinks + 1));
	cnx_link[cnx_numlinks].type = 0;
	cnx_link[cnx_numlinks].map1 = map1;
	cnx_link[cnx_numlinks].x1 = x1;
	cnx_link[cnx_numlinks].y1 = y1;
	cnx_link[cnx_numlinks].map2 = map2;
	cnx_link[cnx_numlinks].x2 = x2;
	cnx_link[cnx_numlinks].y2 = y2;
	++cnx_numlinks;
}

void cnx_addelink(int map, int x, int y, char *label)
{
	printf("link: map%d [%02x,%02x] <--> %s\n", map, x, y, label);

	cnx_link = xrealloc(cnx_link, sizeof(struct CNX_LINK) * (cnx_numlinks + 1));
	cnx_link[cnx_numlinks].type = 1;
	cnx_link[cnx_numlinks].map1 = map;
	cnx_link[cnx_numlinks].x1 = x;
	cnx_link[cnx_numlinks].y1 = y;
	cnx_link[cnx_numlinks].label = strdup(label);
	++cnx_numlinks;
}

void cnx_addtres(int map, int x, int y)
{
	printf("tres: map%d [%02x,%02x]\n", map, x, y);

	cnx_link = xrealloc(cnx_link, sizeof(struct CNX_LINK) * (cnx_numlinks + 1));
	cnx_link[cnx_numlinks].type = 2;
	cnx_link[cnx_numlinks].map1 = map;
	cnx_link[cnx_numlinks].x1 = x;
	cnx_link[cnx_numlinks].y1 = y;
	++cnx_numlinks;
}


main(int argc, char *argv[])
{
	FILE *in, *out;
	char line[256];
	char str[256];
	char label[64], label2[64];
	char *p;
	int map1, map2, x1, y1, x2, y2;
	int n, n2, at, ln;

	if(argc < 3) {
		printf("usage: connex <.cnx> <.eve>\n");
		exit(0);
	}

	// read in the data
	in = fopen(argv[1], "r");
	while(1) {
		fgets(line, 255, in);
		if(feof(in))
			break;

		// clean
		p = strtok(line, "\r\n");
		if(!p)
			readerror();
		if(strlen(p) == 0)
			continue;
		strcpy(str, p);

		// check type
		p = strtok(str, ":");
		if(!p)
			readerror();
		if(!strcmp(p, "base")) {
			p = strtok(NULL, ":");
			cnx_setbase(p);
		}
		else if(allnumbers(p)) {
			strcpy(label, p);
			p = strtok(NULL, ",");
			x1 = strtol(p, NULL, 16);
			p = strtok(NULL, "\t ");
			y1 = strtol(p, NULL, 16);

			p = strtok(NULL, " ");
			strcpy(str, p);

			if(!strchr(str, ':')) {
				// treasure or exit
				map1 = atoi(label);
				if(!strcmp(str, "tres")) {
					cnx_addtres(map1, x1, y1);
				}
				else {
					cnx_addelink(map1, x1, y1, str);
				}
			}
			else {
				// link
				p = strtok(str, ":");
				strcpy(label2, p);
				p = strtok(NULL, ",");
				x2 = strtol(p, NULL, 16);
				p = strtok(NULL, ",");
				y2 = strtol(p, NULL, 16);
				map1 = atoi(label);
				map2 = atoi(label2);
				cnx_addlink(map1, x1, y1, map2, x2, y2);
				// backlink
				cnx_addlink(map2, x2, y2, map1, x1, y1);
			}
		}
		else {
			strcpy(label, p);
			p = strtok(NULL, ",");
			x1 = atoi(p);
			p = strtok(NULL, ",");
			y1 = atoi(p);
			cnx_addexit(label, x1, y1);
		}
	}
	fclose(in);

	// find first reference to the first exit
	n2 = -1;
	for(n = 0; n < cnx_numlinks; ++n) {
		if(cnx_link[n].type == 1) {
			n2 = cnx_link[n].map1;
			break;
		}
	}
	if(n2 == -1) {
		printf("no exit!\n");
	}

	out = fopen(argv[2], "w");
	//out = stdout;

	fprintf(out, "O_%s:\n", cnx_map);
	fprintf(out, "   fadeout(X_ROLL)\n");
	fprintf(out, "   face(0,UP)\n");
	fprintf(out, "   set(D,$%02x)\n", cnx_link[n].x1);
	fprintf(out, "   set(E,$%02x)\n", cnx_link[n].y1 - 1);
	fprintf(out, "   set(G,X_ROLL)\n");
	fprintf(out, "   song()\n");
	fprintf(out, "   goto(%s_part%d_nf)\n\n", cnx_base, n2);

	at = -1;
	while(1) {
		// find lowest map#
		n2 = 99;
		for(n = 0; n < cnx_numlinks; ++n) {
			if(cnx_link[n].type == 0) {
				if(cnx_link[n].map1 > at && cnx_link[n].map1 < n2) {
					n2 = cnx_link[n].map1;
				}
			}
		}
		// done
		if(n2 == 99)
			break;

		at = n2;
		fprintf(out, "%s_part%d:\n", cnx_base, at);
		fprintf(out, "   fadeout(X_FADE)\n");
		if(at == 1) {
			fprintf(out, "   set(G,X_FADE)\n");
			fprintf(out, "%s_part%d_nf:\n", cnx_base, at);
		}

		fprintf(out, "   lmap(MAP_%s_P%d, D,E)\n", cnx_map, at);

		ln = 0;
		for(n = 0; n < cnx_numlinks; ++n) {
			if(cnx_link[n].type == 0 || cnx_link[n].type == 1) {
				if(cnx_link[n].map1 == at) {
					fprintf(out, "   cstep(%s_part%d%c, $%02x,$%02x)\n", cnx_base, at, ln + 'a', cnx_link[n].x1, cnx_link[n].y1);
					++ln;
				}
			}
			if(cnx_link[n].type == 2) {
				if(cnx_link[n].map1 == at) {
					fprintf(out, "   ctres(XTRES,IT_HPLO, $%02x,$%02x,,SFX_CHEST)\n", cnx_link[n].x1, cnx_link[n].y1);
				}
			}
		}

		if(at == 1)
			fprintf(out, "   fadein(G)\n");
		else
			fprintf(out, "   fadein(X_FADE)\n");
		fprintf(out, "   end\n");

		ln = 0;
		for(n = 0; n < cnx_numlinks; ++n) {
			if(cnx_link[n].type == 0) {
				if(cnx_link[n].map1 == at) {
					fprintf(out, "%s_part%d%c:\n", cnx_base, at, ln + 'a');
					fprintf(out, "   set(D,$%02x)\n", cnx_link[n].x2);
					fprintf(out, "   set(E,$%02x)\n", cnx_link[n].y2);
					fprintf(out, "   goto(%s_part%d)\n", cnx_base, cnx_link[n].map2);
					++ln;
				}
			}
			if(cnx_link[n].type == 1) {
				if(cnx_link[n].map1 == at) {
					// find the label
					for(n2 = 0; n2 < cnx_numexits; ++n2) {
						if(!strcmp(cnx_link[n].label, cnx_exit[n2].label))
							break;
					}
					if(n2 >= cnx_numexits) {
						printf("can't locate exit: [%s]\n", cnx_link[n].label);
						exit(1);
					}

					fprintf(out, "%s_part%d%c:\n", cnx_base, at, ln + 'a');
					fprintf(out, "   set(B,$%02x)\n", cnx_exit[n2].x);
					fprintf(out, "   set(C,$%02x)\n", cnx_exit[n2].y);
					fprintf(out, "   goto(OVER_WARP)\n");
					++ln;
				}
			}
		}
		fprintf(out, "\n");
	}
	fclose(out);

	printf("Wrote: [%s]\n", argv[2]);
}
