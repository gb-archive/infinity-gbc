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
#include<ctype.h> // isalnum
#include"enemy.h"

int base_enemy = 0;
int base_map   = 180;

int errors = 0;

struct ENEMYREF
{
	char *name;
	int index;
	int exp, au, item;
	char ai[16];
};

struct ZONEREF
{
	int num;
	int index;
};

struct ENEMYREF *enemyref = NULL;
int enemyref_num = 0;

struct ZONEREF *zoneref = NULL;
int zoneref_num = 0;

static char* getToken(char** str, const char* delims)
{
    char* token;

    if (*str==NULL) {
        /* No more tokens */
        return NULL;
    }

    token=*str;
    while (**str!='\0') {
        if (strchr(delims,**str)!=NULL) {
            **str='\0';
           (*str)++;
            return token;
        }
        (*str)++;
    }
    /* There is no other token */
    *str=NULL;
    return token;
}

int findref_enemy(char *name)
{
	int n;

	for(n = 0; n < enemyref_num; ++n) {
		if(!strcmp(enemyref[n].name, name))
			return n;
	}
	return -1;
}

int xfindref_enemy(int x)
{
	int n;

	for(n = 0; n < enemyref_num; ++n) {
		if(enemyref[n].index == x)
			return n;
	}
	return -1;
}

int xref_zone(int x)
{
	int n;

	for(n = 0; n < zoneref_num; ++n) {
		if(zoneref[n].num == x)
			return zoneref[n].index;
	}
	return -1;
}

int xrevref_zone(int x)
{
	int n;

	for(n = 0; n < zoneref_num; ++n) {
		if(zoneref[n].index == x)
			return zoneref[n].num;
	}
	return -1;
}

void addref_enemy(int index, char *name, int exp, int au, int item, char *ai)
{
	if(index >= 150) {
		printf("ERROR: [%s] is set out of range (%d).\n", name, index);
		exit(1);
	}

	enemyref = realloc(enemyref, sizeof(struct ENEMYREF) * (enemyref_num+1));
	enemyref[enemyref_num].name = strdup(name);
	enemyref[enemyref_num].index = index;
	enemyref[enemyref_num].exp   = exp;
	enemyref[enemyref_num].au    = au;
	enemyref[enemyref_num].item  = item;

	memcpy(enemyref[enemyref_num].ai, ai, 16);

	++enemyref_num;
}

void addref_zone(int num, int index)
{
	zoneref = realloc(zoneref, sizeof(struct ZONEREF) * (zoneref_num+1));
	zoneref[zoneref_num].num   = num;
	zoneref[zoneref_num].index = index;

	++zoneref_num;
}

int readrefs_enemy(char *fname)
{
	FILE *f;
	char *p, *p2;
	char name[256];
	int index, exp, au, item;
	char line[256];
	char ai[16];
	int n;

	f = fopen(fname, "r");
	if(!f)
		return 0;
	while(1) {
		fgets(line, 255, f);
		if(feof(f))
			break;
		p = line;
		p2 = getToken(&p, ",:\r\n");
		if(!p2)
			continue;
		index = atoi(p2);
		p2 = getToken(&p, ",:\r\n");
		if(!p2)
			continue;
		strcpy(name, p2);
		p2 = getToken(&p, ",:\r\n");
		if(!p2)
			continue;
		exp = atoi(p2);
		p2 = getToken(&p, ",:\r\n");
		if(!p2)
			continue;
		au = atoi(p2);
		p2 = getToken(&p, ",:\r\n");
		if(!p2)
			continue;
		item = atoi(p2);

		memset(ai, 0, 16);
		// check for ai
		for(n = 15; n >= 0; --n) {
			p2 = getToken(&p, ",:\r\n");
			if(!p2)
				continue;
			ai[n] = atoi(p2);
		}

		addref_enemy(index, name, exp, au, item, ai);
	}
	fclose(f);

	return 1;
}

int readrefs_zone(char *fname)
{
	FILE *f;
	char *p, *p2;
	int num, index;
	char line[256];

	f = fopen(fname, "r");
	if(!f)
		return 0;
	while(1) {
		fgets(line, 255, f);
		if(feof(f))
			break;
		p = line;
		p2 = getToken(&p, ",:\r\n");
		if(!p2)
			continue;
		num = atoi(p2);
		p2 = getToken(&p, ",:\r\n");
		if(!p2)
			continue;
		index = atoi(p2);

		addref_zone(num, index);
	}
	fclose(f);

	return 1;
}

char *zonestr(int x, struct ZONES *z)
{
	int n, at, num;

	at = 0;
	for(n = 0; n < z->num; ++n) {
		num = z->ptr[n].numforms;
		if(at + num > x) {
			return z->ptr[n].name;
		}
		at += num;
	}

	return NULL;
}

struct FORM_STRUCT *findform(int x, struct ZONES *z)
{
	int n, at, num;
	struct FORM_STRUCT *frm_ptr;

	at = 0;
	for(n = 0; n < z->num; ++n) {
		num = z->ptr[n].numforms;
		if(at + num > x) {
			frm_ptr = &z->ptr[n].ptr[x - at];
			return frm_ptr;
		}
		at += num;
	}

	return NULL;
}

int xfindform(int x, int zone, struct ZONES *z)
{
	int n, at, num;

	at = 0;
	for(n = 0; n < z->num; ++n) {
		num = z->ptr[n].numforms;
		if(n == zone) {
			return at + x;
		}
		at += num;
	}

	return 0;
}

void fix(char *s)
{
	int n;

	for(n = 0; n < strlen(s); ++n) {
		if(!isalnum(s[n]))
			s[n] = '_';
	}
}

int genformdefs(char *fname, struct ZONES *z)
{
	FILE *f;
	int n, n2, at, num;
	char zone[256], form[256];

	f = fopen(fname, "w");
	if(!f)
		return 0;
	at = 0;
	for(n = 0; n < z->num; ++n) {
		num = z->ptr[n].numforms;
		strcpy(zone, z->ptr[n].name);
		fix(zone);
		for(n2 = 0; n2 < num; ++n2) {
			strcpy(form, z->ptr[n].ptr[n2].name);
			fix(form);
			fprintf(f, "#define %s_%s %d\n", zone, form, at);
			++at;
		}
	}
	fclose(f);

	return 1;
}


int main(int argc, char *argv[])
{
	struct FORM_STRUCT form, *formp;
	struct ENEMY_STRUCT en;
	struct ZONE_STRUCT zone;
	struct ENEMIES e;
	struct ZONES z;
	FILE *f;
	char sig[16];
	int n, n2, x, y;
	int at;
	int num, exp, au;
	FILE *out;

	if(argc < 2) {
		printf("usage: afzconv <.afz file> [cmd] [arg1] [arg2]\n");
		return 1;
	}

	enemy_reset(&e);
	zones_reset(&z);

	f = fopen(argv[1], "rb");
	if(!f) {
		printf("ERROR: Couldn't open [%s]\n", argv[1]);
		return 1;
	}

	fread(sig, 3, 1, f);
	if(strncmp(sig, "AFZ", 3)) {
		printf("ERROR: [%s] has an invalid format.\n", argv[1]);
		fclose(f);
		return 1;
	}

	// read enemies
	fread(&x, 4, 1, f);
	printf("Enemies: %d\n", x);
	for(n = 0; n < x; ++n) {
		fread(&en, sizeof(struct ENEMY_STRUCT) - 12 - 16, 1, f);
		enemy_add(&e, &en);
	}

	// read zones
	fread(&x, 4, 1, f);
	printf("Zones: %d\n", x);
	for(n = 0; n < x; ++n) {
		fread(&zone, ZONE_STRUCT_SIZE, 1, f);
		y = zone.numforms;
		zone.numforms = 0;
		zone.ptr = NULL;

		// read formations
		printf("Zone [%s]:\n", zone.name);
		printf("  Formations: %d\n", y);
		for(n2 = 0; n2 < y; ++n2) {
			fread(&form, sizeof(struct FORM_STRUCT), 1, f);
			printf("    Form [%s]\n", form.name);
			form_add(&zone, &form);
		}

		zones_add(&z, &zone);
	}
	fread(sig, 3, 1, f);
	if(strncmp(sig, "AFZ", 3)) {
		printf("ERROR: [%s] has an invalid format.\n", argv[1]);
		fclose(f);
		return 1;
	}
	fclose(f);

	printf("done.\n");

	if(argc > 2) {
		if(!strcmp(argv[2], "renzone")) {
			for(n = 0; n < z.num; ++n) {
				if(!strcmp(z.ptr[n].name, argv[3]))
					break;
			}
			if(n >= z.num) {
				printf("no such zone: %s\n", argv[3]);
				return 1;
			}

			// rename the zone
			strcpy(z.ptr[n].name, argv[4]);
			printf("Zone is now %d:%s\n", n+1, z.ptr[n].name);
		}
		else if(!strcmp(argv[2], "swapzone")) {
			for(n = 0; n < z.num; ++n) {
                                if(!strcmp(z.ptr[n].name, argv[3]))
                                        break;
                        }
                        if(n >= z.num) {
                                printf("no such zone: %s\n", argv[3]);
                                return 1;
                        }

			for(n2= 0; n2 < z.num; ++n2) {
                                if(!strcmp(z.ptr[n2].name, argv[4]))
                                        break;
                        }
                        if(n2 >= z.num) {
                                printf("no such zone: %s\n", argv[4]);
                                return 1;
                        }
 
                        // swap the zones
			zone = z.ptr[n];
			z.ptr[n] = z.ptr[n2];
			z.ptr[n2] = zone;
			
                        printf("Zones are now swapped.\n");
		}
		else if(!strcmp(argv[2], "renform") || !strcmp(argv[2], "delform")) {
			for(n = 0; n < z.num; ++n) {
				if(!strcmp(z.ptr[n].name, argv[3]))
					break;
			}
			if(n >= z.num) {
				printf("no such zone: %s\n", argv[3]);
				return 1;
			}

			for(n2 = 0; n2 < z.ptr[n].numforms; ++n2) {
				if(!strcmp(z.ptr[n].ptr[n2].name, argv[4]))
					break;
			}
			if(n2 >= z.ptr[n].numforms) {
				printf("no such form: %s\n", argv[4]);
				return 1;
			}

			if(!strcmp(argv[2], "renform")) {
				// rename the formation
				strcpy(z.ptr[n].ptr[n2].name, argv[5]);
				printf("Formation is now %d:%s:%s\n", n+1, z.ptr[n].name, z.ptr[n].ptr[n2].name);
			}
			else {
				// delete the formation
				printf("Formation %d:%s:%s deleted.\n", n+1, z.ptr[n].name, z.ptr[n].ptr[n2].name);
				form_del(&z.ptr[n], n2);
			}
		}
		else {
			printf("no such command: [%s]\n", argv[2]);
		}

		// save back, and exit
		printf("Press enter to save.\n");
		getchar();


		f = fopen("result.afz", "wb");
 
                // write signature
                fwrite("AFZ", 3, 1, f);
 
                // write enemies
                fwrite(&e.num, 4, 1, f);
                for(n = 0; n < e.num; ++n) {
                        fwrite(&e.ptr[n], sizeof(struct ENEMY_STRUCT) - 12 - 16, 1, f);
                }
 
                // write zones
                fwrite(&z.num, 4, 1, f);
                for(n = 0; n < z.num; ++n) {
                        fwrite(&z.ptr[n], sizeof(struct ZONE_STRUCT), 1, f);
                        for(n2 = 0; n2 < z.ptr[n].numforms; ++n2) {
                                fwrite(&z.ptr[n].ptr[n2], sizeof(struct FORM_STRUCT), 1, f);
                        }
                }
                fwrite("AFZ", 3, 1, f);
                fclose(f);
 
                printf("Changes saved.\n");
		return 1;
	}

	if(!readrefs_enemy("enemies.ref")) {
		printf("ERROR: trying to read enemies.ref\n");
		return 1;
	}
	if(!readrefs_zone("zones.ref")) {
		printf("ERROR: trying to read zones.ref\n");
		return 1;
	}

	printf("read %d enemy references.\n", enemyref_num);
	for(n = 0; n < enemyref_num; ++n) {
		printf("[%d] = [%s]\n", enemyref[n].index, enemyref[n].name);
	}

	printf("read %d zone references.\n", zoneref_num);
	for(n = 0; n < zoneref_num; ++n) {
		printf("[zone%d] -> [%d]\n", zoneref[n].num, zoneref[n].index);
	}

	// verify enemy references
	for(n = 0; n < e.num; ++n) {
		x = findref_enemy(e.ptr[n].name);
		if(x == -1) {
			printf("ERROR: no reference for enemy %d: [%s]\n", n, e.ptr[n].name);
			return 1;
		}

		e.ptr[n].exp  = enemyref[x].exp;
		e.ptr[n].au   = enemyref[x].au;
		e.ptr[n].item = enemyref[x].item;
		memcpy(e.ptr[n].ai, enemyref[x].ai, 16);
		printf("[%s]->[%s]\n", e.ptr[n].name, enemyref[x].name);
	}

	// verify zone references
	for(n = 0; n < z.num; ++n) {
		x = xref_zone(n+1);
		if(x == -1) {
			printf("ERROR: no reference for zone:%d\n", n+1);
			return 1;
		}
	}

	//out = stdout;
	out = fopen("eform.txt", "w");

	fprintf(out, "; ----------------------------------------\n");
	fprintf(out, ";\n");
	fprintf(out, "; Enemy formation info / Enemy info\n");
	fprintf(out, ";\n");
	fprintf(out, "; ----------------------------------------\n");
	fprintf(out, "#define b(label)                      dw label\n");
	fprintf(out, "#define info(map,enum,exp,gold)       db map, enum __NL__ dw exp, gold\n");
	fprintf(out, "#define bp(x,y)                       db x, y\n");
	fprintf(out, "#define be(type,x,y)                  db type, x, y\n");
	fprintf(out, "#define  e(lvl,hp,sp,str,end,agl,wis) db lvl __NL__ dw hp,hp,sp,sp __NL__ db str,end,agl,wis\n");
	fprintf(out, "#define  a(att,def,spd,md)            db att,def,spd,md\n");
	fprintf(out, "#define  z(item)                      db item\n");
	fprintf(out, "#define eblank                        db 0 __NL__ dw 0,0,0,0 __NL__ db 0,0,0,0, 0,0,0,0 __NL__ db 0 __NL__ dw 0,0,0,0,0,0,0,0 __NL__ db 0,0,0,0 \n");
	fprintf(out, "; ----------------------------------------\n");
	fprintf(out, "\n");
	fprintf(out, "    dw	zones\n");
	fprintf(out, "    dw	formations\n");
	fprintf(out, "    dw	enemies\n");
	fprintf(out, "\n");

	// skip defines..

	// zones
	fprintf(out, "zones:\n");
	at = 0;
	for(n = 0; n < 64; ++n) {
		x = xrevref_zone(n+1);
		if(x == -1) {
			fprintf(out, "    db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 ; blank\n");
		}
		else {
			--x;
			printf("[%d]->[%d]\n", n, x);

			fprintf(out, "    db %d", z.ptr[x].numforms);
			for(n2 = 0; n2 < z.ptr[x].numforms; ++n2) {
				fprintf(out, ",%2d,%2d", xfindform(n2, x, &z), z.ptr[x].ptr[n2].priority);
				++at;
			}
			for(; n2 < 12; ++n2) {
				fprintf(out, ", 0, 0");
			}
			fprintf(out, "\n");
		}
	}
	fprintf(out, "\n");

	// make formdefs.h
	genformdefs("formdefs.h", &z);

	// formations
	fprintf(out, "formations:\n");
	for(n = 0; n < at; ++n) {
		fprintf(out, "    b( form%03d )\n", n + 1);
	}
	fprintf(out, "\n");

	// enemies:
	fprintf(out, "enemies:\n");
	for(n = 0; n < 150; ++n) {
		x = xfindref_enemy(n);
		if(x == -1) {
			fprintf(out, "    eblank\n");
		}
		else {
			// match the enemy
			for(n2 = 0;; ++n2) {
				if(!strcmp(enemyref[x].name, e.ptr[n2].name))
					break;
			}
			x = n2;

			fprintf(out, "    ; %s - %d exp, %d au\n", e.ptr[x].name, e.ptr[x].exp, e.ptr[x].au);
			fprintf(out, "    e(%2d,%2d,%2d,  %2d,%2d,%2d,%2d)\n", e.ptr[x].level, e.ptr[x].hp, e.ptr[x].sp, e.ptr[x].str, e.ptr[x].end, e.ptr[x].agl, e.ptr[x].wis);
			fprintf(out, "    a(%2d,%2d,%2d,%2d)\n", e.ptr[x].att, e.ptr[x].def, e.ptr[x].spd, e.ptr[x].mgd);
			fprintf(out, "    z(%2d)\n", e.ptr[x].item);
			fprintf(out, "    db ");
			for(n2 = 0; n2 < 16; ++n2) {
				if(n2 != 0)
					fprintf(out, ",");
				fprintf(out, "%2d", e.ptr[x].ai[n2]);
			}
			fprintf(out, "\n");
			fprintf(out, "    db %d,%d,%d,%d\n", e.ptr[x].weapon, e.ptr[x].fire, e.ptr[x].water, e.ptr[x].wind);

			fprintf(out, "\n");
		}
	}
	fprintf(out, "\n");

	fprintf(out, "    ; Formation data\n");
	for(n = 0; n < at; ++n) {
		formp = findform(n, &z);
		fprintf(out, "form%03d:\n", n + 1);
		fprintf(out, "    ; %s\n", formp->name);
		num = 0;
		exp = 0;
		au = 0;
		y = 0;
		for(n2 = 0; n2 < 8; ++n2) {
			if(formp->guy_active[n2+4]) {
				x = formp->guy_type[n2+4];
				if(x == 0) {
					if(errors >= 10) {
						printf("Quitting because of too many errors.\n");
						return 1;
					}
					printf("ERROR: uninitialized enemy in [%s]:[%s] at slot %d\n", zonestr(n, &z), formp->name, n2+4+1);
					++errors;
				}
				x = x - 1 + base_enemy;

				exp += e.ptr[x].exp;
				au  += e.ptr[x].au;
				y   += e.ptr[x].level;
				++num;
			}
		}
		y /= 2;
		exp += y;

		fprintf(out, "    info(%2d,%2d,  %d,%d)\n", formp->map + base_map, num, exp, au);
		for(n2 = 0; n2 < 4; ++n2)
			fprintf(out, "    bp(%2d,%2d)\n", formp->guy_x[n2], formp->guy_y[n2]);

		for(n2 = 4; n2 < 12; ++n2) {
			if(formp->guy_active[n2]) {
				x = formp->guy_type[n2]-1;
				x = findref_enemy(e.ptr[x].name);

				fprintf(out, "    be(%2d, %2d,%2d)\n", enemyref[x].index + base_enemy, formp->guy_x[n2], formp->guy_y[n2]);
			}
		}
		fprintf(out, "\n");
	}
	fprintf(out, "\n");
	fclose(out);

	if(errors)
		return 1;

	printf("Success.\n");
	return 0;
}
