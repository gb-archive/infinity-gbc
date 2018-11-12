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

struct ITEM
{
	char active;
	char name[256];
	char icon;
	char flag;
	int att, def, spd, wis;
	int equip;
	int value;
};

struct ITEM item[100];

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

char *memstr(char *ptr, char *str, int bytes)
{
	int n, len;

	len = strlen(str);
	for(n = 0; n < bytes - len; ++n) {
		if(!memcmp(ptr + n, str, len))
			return ptr + n;
	}

	return NULL;
}

int get_type(char icon)
{
	char chart[] = { 0, 5, 2, 2, 2, 2, 4, 3, 4, 4, 4, 4, 4, 0, 2, 3 };
	return chart[(int)icon];
}

char *equip2str(int x)
{
	static char str[7];

	strcpy(str, "------");
	if(x & 1)  str[0] = 'C';
	if(x & 2)  str[1] = 'R';
	if(x & 4)  str[2] = 'E';
	if(x & 8)  str[3] = 'V';
	if(x &16)  str[4] = 'O';
	if(x &32)  str[5] = 'A';

	return str;
}

int get_equip(char *s)
{
	int n, n2, len;
	char types[] = "-CREVOA";
	int val = 0;

	for(n = 0; n < strlen(s); ++n) {
		len = strlen(types);
		for(n2 = 0; n2 < len; ++n2) {
			if(s[n] == types[n2])
				break;
		}
		if(n2 >= len)
			return -1;
		if(n2 == 0)
			continue;
		--n2;

		val += (1 << n2);
	}

	return val;
}

char get_attrib(char *s)
{
	int n;

	n = strlen(s) - 1;
	if(s[n] == 'F')
		return 1;
	if(s[n] == 'W')
		return 2;
	if(s[n] == 'A')
		return 3;
	return 0;
}

int getvals(char *inbuf, int *index, int *icon, char *name)
{
	char *p;
	int a_index, a_icon;
	char a_name[256];

	p = getToken(&inbuf, ":");
	if(!p)
		return 0;
	a_index = atoi(p);
	p = getToken(&inbuf, ":");
	if(!p)
		return 0;
	if(p[0] < 'A' || p[0] > 'P')
		return 0;
	a_icon = p[0] - 'A';
	p = getToken(&inbuf, ":");
	if(!p)
		return 0;
	strcpy(a_name, p);

	*index = a_index;
	*icon  = a_icon;
	strcpy(name, a_name);

	return 1;
}

int main(int argc, char *argv[])
{
	FILE *f;
	char *p, *p2;
	char line[256];
	int n, x;
	int index, icon;
	char name[256];
	char dat[256];
	int att, def, spd, wis, flag, equip, value;
	char *rom;
	int size;
	int mode;
	char fname[256];

	if(argc < 3) {
		printf("usage: itemconv --defs <items.ref file>\n");
		printf("       itemconv <items.ref file> <infinity gb file>\n");
		return 1;
	}

	if(!strcmp(argv[1], "--defs")) {
		mode = 1;
		strcpy(fname, argv[2]);
	}
	else {
		mode = 0;
		strcpy(fname, argv[1]);
	}

	for(n = 0; n < 100; ++n) {
		item[n].active = 0;
	}

	f = fopen(fname, "r");
	if(!f) {
		printf("Can't open [%s]\n", argv[1]);
		return 1;
	}
	while(1) {
		fgets(line, 255, f);
		if(feof(f))
			break;

		p = strchr(line, '#');
		if(p)
			*p = 0;
		p = line;
		p2 = strtok(p, " \t\r\n");
		if(!p2)
			continue;
		if(!getvals(p2, &index, &icon, name))
			continue;

		p2 = strtok(NULL, " \t\r\n"); if(!p2) continue;
		flag = get_attrib(p2);
		att = atoi(p2);
		p2 = strtok(NULL, " \t\r\n"); if(!p2) continue;
		def = atoi(p2);
		p2 = strtok(NULL, " \t\r\n"); if(!p2) continue;
		spd = atoi(p2);
		p2 = strtok(NULL, " \t\r\n"); if(!p2) continue;
		wis = atoi(p2);
		p2 = strtok(NULL, " \t\r\n"); if(!p2) continue;
		equip = get_equip(p2);
		p2 = strtok(NULL, " \t\r\n"); if(!p2) continue;
		value = atoi(p2);

		/*x = 0;
		if(att < -64 || att > 64)
			x = 1;
		if(def < -64 || def > 64)
			x = 1;
		if(spd < -64 || spd > 64)
			x = 1;
		if(wis < -64 || wis > 64)
			x = 1;*/
		if(equip == -1 || index < 20 || index > 99) {
			printf("error in [%s]\n", name);
			return 1;
		}

		item[index].active = 1;
		strcpy(item[index].name, name);
		item[index].icon = icon;
		item[index].flag = flag;
		item[index].att = att;
		item[index].def = def;
		item[index].spd = spd;
		item[index].wis = wis;
		item[index].equip = equip;
		item[index].value = value;
	}
	fclose(f);

	x = 0;
	for(n = 0; n < 100; ++n) {
		if(!item[n].active)
			continue;

		++x;
		//printf("Name:  %s (%d)\n", item[n].name, item[n].icon + 'A');
		//printf("Stats: %3d %3d %3d %3d\n", item[n].att, item[n].def, item[n].spd, item[n].wis);
		//printf("Equip: %s (%d)\n", equip2str(item[n].equip), item[n].equip);
		//printf("Value: %d\n\n", item[n].value);
	}
	printf("Found %d complete items.\n", x);

	if(mode == 0) {
		f = fopen(argv[2], "r+b");
		if(!f) {
			printf("unable to open [%s]\n", argv[2]);
			return 1;
		}
		fseek(f, 0l, SEEK_END);
		size = ftell(f);
		rewind(f);
		rom = malloc(size);
		if(!rom) {
			printf("unable to allocate enough memory!\n");
			return 1;
		}
		fread(rom, size, 1, f);
		rewind(f);

		printf("Read %d bytes of ROM\n", size);

		p = memstr(rom, "InfinityItemsNames", size);
		if(!p) {
			printf("unable to locate the Infinity Item List in this rom!\n");
			return 1;
		}
		printf("Located at offset: [%ld]\n", p - rom);
		p += 19;

		for(n = 0; n < 100; ++n) {
			if(!item[n].active)
				continue;
			p2 = p + 20 * n;
			memset(dat, 0, 20);
			dat[0] = item[n].icon + 'A';
			strcpy(dat + 1, item[n].name);
			memcpy(p2, dat, 20);
		}

		p = memstr(rom, "InfinityItemsData", size);
		if(!p) {
			printf("unable to locate the Infinity Item List in this rom!\n");
			return 1;
		}
		printf("Located at offset: [%ld]\n", p - rom);
		p += 18;

		for(n = 0; n < 100; ++n) {
			if(!item[n].active)
				continue;
			p2 = p + 7 * n;
			memset(dat, 0, 7);
			dat[0] = get_type(item[n].icon);
			dat[1] = item[n].att;
			dat[2] = item[n].def;
			dat[3] = item[n].spd;
			dat[4] = item[n].wis;
			dat[5] = item[n].flag;
			dat[6] = item[n].equip;
			memcpy(p2, dat, 7);
		}

		p = memstr(rom, "InfinityItemsValue", size);
		if(!p) {
			printf("unable to locate the Infinity Item List in this rom!\n");
			return 1;
		}
		printf("Located at offset: [%ld]\n", p - rom);
		p += 19;

		for(n = 0; n < 100; ++n) {
			if(!item[n].active)
				continue;
			p2 = p + 2 * n;
			memcpy(p2, &item[n].value, 2);
		}

		fwrite(rom, size, 1, f);
		fclose(f);

		printf("rom file has been updated successfully.\n");
	}
	else {
		f = fopen("itemdefs.h", "w");
		for(n = 0; n < 100; ++n) {
			if(!item[n].active)
				continue;
			fprintf(f, "#define IT_%c%s\t%d\n", item[n].icon + 'A', item[n].name, n);
		}
		fclose(f);

		printf("wrote itemdefs.h\n");
	}

	return 0;
}
