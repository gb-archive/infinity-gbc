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
#include<ctype.h> // tolower

#if !defined(__MACH__) && !defined(__APPLE__)
#include<malloc.h>
#endif

//#include<bios.h>

#define PAGEBASE	28
#define CHSIZE		64
#define NPCSIZE		8
#define ENSIZE		9


void mem_error()
{
	printf("mem error!\n");
	exit(0);
}

struct J2W_HEADER {
	char id[3];
	int ch_num;
	int en_num;
	int npc_num;
	int set_num;
	int map_num;
	int song_num;
} hed;

struct J2W_CH
{
	char label[64];
	int pal;
	char data[64];
} *ch;

struct J2W_EN
{
	char label[64];
	int pal;
	char data[64];
} *en;

struct J2W_NPC
{
	char label[64];
	int pal;
	char data[64];
} *npc;

struct J2W_SET
{
	char label[64];
	int num_tile;
	int num_meta;
	char pal[144];
	char meta[2048];
	char data[4096];
} *set;

struct J2W_MAP
{
	char label[64];
	int x;
	int y;
	int tileset;
	int info;
	char data[20000];
} *map;

struct J2W_SONG
{
	int len;
	char data[16384];
} *song;


void j2w_addch(char *label, int pal, char *data)
{
	int newsize;

	newsize = sizeof(struct J2W_CH) * (hed.ch_num + 1);
	ch = realloc(ch, newsize);
	ch[hed.ch_num].pal = pal;
	memcpy(ch[hed.ch_num].data, data, 64);
	++hed.ch_num;
}

void j2w_adden(char *label, int pal, char *data)
{
	en = realloc(en, sizeof(struct J2W_EN) * (hed.en_num + 1));
	en[hed.en_num].pal = pal;
	memcpy(en[hed.en_num].data, data, 64);
	++hed.en_num;
}

void j2w_addnpc(char *label, int pal, char *data)
{
	npc = realloc(npc, sizeof(struct J2W_NPC) * (hed.npc_num + 1));
	npc[hed.npc_num].pal = pal;
	memcpy(npc[hed.npc_num].data, data, 64);
	++hed.npc_num;
}

void j2w_addset(char *label, int num_tile, int num_meta, char *pal, char *meta, char *tile)
{
	set = realloc(set, sizeof(struct J2W_SET) * (hed.set_num + 1));

	strcpy(set[hed.set_num].label, label);
	set[hed.set_num].num_tile = num_tile;
	set[hed.set_num].num_meta = num_meta;
	memcpy(set[hed.set_num].pal, pal, 144);
	memcpy(set[hed.set_num].meta, meta, num_meta * 8);
	memcpy(set[hed.set_num].data, tile, num_tile * 16);
	++hed.set_num;
}

void j2w_addmap(char *label, int x, int y, char *data, int tileset, int info)
{
	map = realloc(map, sizeof(struct J2W_MAP) * (hed.map_num + 1));

	strcpy(map[hed.map_num].label, label);
	map[hed.map_num].x = x;
	map[hed.map_num].y = y;
	map[hed.map_num].tileset = tileset;
	map[hed.map_num].info = info;
	memcpy(map[hed.map_num].data, data, x * y);
	++hed.map_num;
}

void j2w_addsong(int len, char *data)
{
	song = realloc(song, sizeof(struct J2W_SONG) * (hed.song_num + 1));
	if(!song)
		mem_error();
	song[hed.song_num].len = len;
	memcpy(song[hed.song_num].data, data, len);
	++hed.song_num;
}

char zonemap[16384];

int j2w_add(char *fname, int ftype, char *flabel, int info)
{
	FILE *f, *out;
	int num, size, n;
	char pal[144];
	char pals[144];
	char meta[2048];
	char tiles[16384];
	int x, y;
	char id[3];

	f = fopen(fname, "rb");
	if(!f)
		return 0;

	fseek(f, 0l, SEEK_END);
	size = ftell(f);
	rewind(f);

	// characters
	if(ftype == 1) {
		num = size / 65;
		if(num > CHSIZE) {
			printf("Frames > %d!\n", CHSIZE);
			return 0;
		}

		fread(pals, num, 1, f);
		fread(tiles, num * 64, 1, f);
		fclose(f);

		for(n = 0; n < num; ++n)
			j2w_addch(flabel, pals[n], tiles + n * 64);
		for(n = num; n < CHSIZE; ++n) {
			memset(tiles, 0, 64);
			j2w_addch(flabel, 0, tiles);
		}
	}
	if(ftype == 2) {
		num = size / 65;
		fread(pals, num, 1, f);
		fread(tiles, num * 64, 1, f);
		fclose(f);

		for(n = 0; n < num; ++n)
			j2w_adden(flabel, pals[n], tiles + n * 64);
	}
	if(ftype == 3) {
		num = size / 65;
		fread(pals, num, 1, f);
		fread(tiles, num * 64, 1, f);
		fclose(f);

		for(n = 0; n < num; ++n)
			j2w_addnpc(flabel, pals[n], tiles + n * 64);
		for(n = num; n < NPCSIZE; ++n) {
			memset(tiles, 0, 64);
			j2w_addnpc(flabel, 0, tiles);
		}
	}
	if(ftype == 4) {
		fread(id, 3, 1, f);

		fread(&num, 4, 1, f);
		fread(&x, 4, 1, f);

		fread(pal, 144, 1, f);
		fseek(f, num, SEEK_CUR);
		fread(meta, x * 8, 1, f);
		fread(tiles, num * 16, 1, f);
		fclose(f);

		j2w_addset(flabel, num, x, pal, meta, tiles);
	}
	if(ftype == 5) {
		fread(id, 3, 1, f);

		fread(&x, 4, 1, f);
		fread(&y, 4, 1, f);
		n = 0;
		fread(&n, 1, 1, f);
		fread(tiles, x * y, 1, f);
		if((n & 3) == 3) {
			fread(zonemap, x * y, 1, f);
			out = fopen("zonemap.pag", "wb");
			fwrite(zonemap, x * y, 1, out);
			fclose(out);
		}

		fclose(f);

		j2w_addmap(flabel, x, y, tiles, info, n);
	}
	if(ftype == 6) {
		fread(id, 3, 1, f);
		n = size - 3;
		fread(tiles, n, 1, f);
		fclose(f);

		printf("adding song: %d\n", n);
		j2w_addsong(n, tiles);
	}
	if(ftype == 7) {
		num = 36;
		fread(pals, num, 1, f);
		fread(tiles, num * 64, 1, f);
		fclose(f);

		for(n = 0; n < num; ++n)
			j2w_adden(flabel, pals[n], tiles + n * 64);
	}
	return 1;
}

int j2w_save(char *fname)
{
	FILE *f;
	int n;

	f = fopen(fname, "wb");
	fwrite(&hed, sizeof(struct J2W_HEADER), 1, f);

	for(n = 0; n < hed.ch_num; ++n)
		fwrite(&ch[n], sizeof(struct J2W_CH), 1, f);
	for(n = 0; n < hed.en_num; ++n)
		fwrite(&en[n], sizeof(struct J2W_EN), 1, f);
	for(n = 0; n < hed.npc_num; ++n)
		fwrite(&npc[n], sizeof(struct J2W_NPC), 1, f);
	for(n = 0; n < hed.set_num; ++n)
		fwrite(&set[n], sizeof(struct J2W_SET), 1, f);
	for(n = 0; n < hed.map_num; ++n)
		fwrite(&map[n], sizeof(struct J2W_MAP), 1, f);
	for(n = 0; n < hed.song_num; ++n)
		fwrite(&song[n], sizeof(struct J2W_SONG), 1, f);

	fclose(f);

	return 0;
}

int j2w_load(char *fname)
{
	FILE *f;
	char tile[64];
	int set_num_tile;
	int set_num_meta;
	char set_tiles[4096];
	char set_meta[2048];
	char set_pal[144];
	int pal;
	int n, i, i2;
	int x, y;
	char *buf;
	struct J2W_HEADER newhed;
	char label[64];

	buf = malloc(20000);
	if(!buf)
		mem_error();

	f = fopen(fname, "rb");
	if(f) {
		fread(&newhed, sizeof(struct J2W_HEADER), 1, f);
		if(strncmp(newhed.id, "J2W", 3)) {
			printf("Invalid J2W file!\n");
			fclose(f);
			exit(0);
		}

		for(n = 0; n < newhed.ch_num; ++n) {
			fread(label, 64, 1, f);
			fread(&pal, 4, 1, f);
			fread(tile, 64, 1, f);
			j2w_addch(label, pal, tile);
		}
		for(n = 0; n < newhed.en_num; ++n) {
			fread(label, 64, 1, f);
			fread(&pal, 4, 1, f);
			fread(tile, 64, 1, f);
			j2w_adden(label, pal, tile);
		}
		for(n = 0; n < newhed.npc_num; ++n) {
			fread(label, 64, 1, f);
			fread(&pal, 4, 1, f);
			fread(tile, 64, 1, f);
			j2w_addnpc(label, pal, tile);
		}

		for(n = 0; n < newhed.set_num; ++n) {
			fread(label, 64, 1, f);
			fread(&set_num_tile, 4, 1, f);
			fread(&set_num_meta, 4, 1, f);

			fread(set_pal, 144, 1, f);
			fread(set_meta, 2048, 1, f);
			fread(set_tiles, 4096, 1, f);
			j2w_addset(label, set_num_tile, set_num_meta, set_pal, set_meta, set_tiles);
		}

		for(n = 0; n < newhed.map_num; ++n) {
			fread(label, 64, 1, f);
			fread(&x, 4, 1, f);
			fread(&y, 4, 1, f);
			fread(&i, 4, 1, f);
			fread(&i2, 4, 1, f);
			fread(buf, 20000, 1, f);
			j2w_addmap(label, x, y, buf, i, i2);
		}

		for(n = 0; n < newhed.song_num; ++n) {
			fread(&x, 4, 1, f);
			fread(buf, 16384, 1, f);
			j2w_addsong(x, buf);
		}

		memcpy(&hed, &newhed, sizeof(struct J2W_HEADER));
		return 1;
	}
	else {
		return 0;
	}
}

void j2w_reset()
{
	hed.id[0] = 'J';
	hed.id[1] = '2';
	hed.id[2] = 'W';
	hed.ch_num = 0;
	hed.en_num = 0;
	hed.npc_num = 0;
	hed.set_num = 0;
	hed.map_num = 0;
	hed.song_num = 0;

	ch = NULL;
	en = NULL;
	npc = NULL;
	set = NULL;
	map = NULL;
	song = NULL;
}

void usage()
{
	printf("J2W v1.0 - Joltima 2 wad file manager\n");
	printf(" usage: j2w <.j2w file> <.lst config file>\n");
}

int main(int argc, char *argv[])
{
	FILE *out, *lst;
	char buf[16384];
	char fname[256], fadd[256];
	int ftype;
	char *p;
	int key;
	int num, pages;
	char str[256];
	int n, i;
	int at;
	char *tiles;
	int atpage;
	//int repeat;

	int set_pages;
	int spr_pages;
	int map_pages;
	int song_pages;

	int cur_seg, cur_off;

	int stb_off[64];
	int stb_seg[64];
	int stb_size[64];

	int map_off[256];
	int map_seg[256];
	int map_size[256];

	int song_off[64];
	int song_seg[64];
	int song_size[64];
	char flabel[64];
	int info = 0;

	j2w_reset();

	if(argc < 2) {
		usage();
		exit(0);
	}
	strcpy(fname, argv[1]);

	// use list file
	if(argc > 2) {
		lst = fopen(argv[2], "r");
		if(!lst) {
			printf("can't read [%s]\n", argv[2]);
			return 1;
		}
		// read the contents
		while(1) {
			fgets(str, 255, lst);
			if(feof(lst))
				break;

			// locate comment
			p = strchr(str, '#');
			if(p)
				*p = 0;

			// separate label and file
			p = strtok(str, " \r\n\t");
			if(!p)
				continue;
			strcpy(flabel, p);
			p = strtok(NULL, " \r\n\t");
			if(!p)
				continue;
			strcpy(fadd, p);

			ftype = 0;
			// get type based on label
			if(!strncmp(flabel, "Hero_", 5))
				ftype = 1;
			else if(!strncmp(flabel, "Enemy_", 6))
				ftype = 2;
			else if(!strncmp(flabel, "Boss_", 5))
				ftype = 2;
			else if(!strncmp(flabel, "Npc_", 4))
				ftype = 3;
			else if(!strncmp(flabel, "Tile_", 5))
				ftype = 4;
			else if(!strncmp(flabel, "Map_", 4))
				ftype = 5;

			if(ftype == 0) {
				printf("Unknown type for label \"%s\".\n", flabel);
				return 1;
			}
			// maps need tilesets
			if(ftype == 5) {
				p = strtok(fadd, ",");
				p = strtok(NULL, ",");
				if(!p) {
					printf("No tileset label for map \"%s\".\n", flabel);
					return 1;
				}

				// search through tilesets for this label
				for(n = 0; n < hed.set_num; ++n) {
					if(!strcmp(p, set[n].label))
						break;
				}
				if(n >= hed.set_num) {
					printf("Undefined tileset reference \"%s\".\n", p);
					return 1;
				}
				info = n;
			}

			// add
			printf("Adding: {%s} [%s]", flabel, fadd);
			if(ftype == 5)
				printf(" (%s)%d", p, info);
			printf("\n");

			if(!j2w_add(fadd, ftype, flabel, info)) {
				//printf("Couldn't add [%s]\n", fadd);
				printf("Error adding!\n");
				return 1;
			}
			else {
				//printf("Added [%s]\n", fadd);
			}

		}
		fclose(lst);
		j2w_save(fname);
		printf("Done.\n");
	}

	/*ftype = 0;
	if(argc > 3) {
		strcpy(fadd, argv[2]);
		ftype = atoi(argv[3]);
		if(ftype < 1 || ftype > 6) {
			printf("Bad Type.  1=C, 2=E, 3=NPC, 4=TILESET, 5=MAP, 6=SONG, 7=BOSS\n");
			exit(0);
		}
	}
	if(argc > 4) {
		info = atoi(argv[4]);
	}
	repeat = 1;
	if(argc > 5) {
		repeat = atoi(argv[5]);
		if(repeat < 1)
			repeat = 1;
	}

	if(j2w_load(fname)) {
		printf("Reading:  %s\n", fname);
	}
	else {
		printf("Creating: %s\n", fname);
		j2w_reset();
	}

	if(ftype > 0) {
		for(n = 0; n < repeat; ++n) {
			if(!j2w_add(fadd, ftype, info)) {
				printf("Couldn't add [%s]\n", fadd);
				bioskey(0);
			}
			else {
				printf("Added [%s]\n", fadd);
			}
		}
		j2w_save(fname);
		printf("Done.\n");
	}*/

	else {
		if(j2w_load(fname))
			printf("Reading:  %s\n", fname);
		else
			printf("Error reading [%s]\n", fname);

		printf("--->\n");
		printf("  %3d Character frames (%2d characters)\n", hed.ch_num, hed.ch_num / CHSIZE);
		printf("  %3d Enemy frames (%2d characters)\n", hed.en_num, hed.en_num / ENSIZE);
		printf("  %3d NPC frames (%2d characters)\n", hed.npc_num, hed.npc_num / NPCSIZE);
		printf("  %3d Tilesets\n", hed.set_num);
		printf("  %3d Maps\n", hed.map_num);
		printf("  %3d Songs\n", hed.song_num);

		/*printf("Turn to .PAG files? [Y/N]\n");
		while(1) {
			#if defined(__MACH__) || defined(__unix__)
			key = getchar();
			#else
			key = getch();//bioskey(0);
			#endif

			key = tolower(key);
			if(key == 'y' || key == 'n')
				break;
		}*/
		key = 'y';

		if(key == 'y') {

			// ***** SPRITES *****
			num = hed.ch_num + hed.en_num + hed.npc_num;
			tiles = malloc(num * 64 + 16384);
			if(!tiles) {
				printf("memory!\n");
				exit(0);
			}
			memset(tiles, 0, num * 64 + 16384);
			at = 0;
			for(n = 0; n < hed.ch_num; ++n, ++at) {
				memcpy(tiles + at * 64, ch[n].data, 64);
			}
			for(n = 0; n < hed.en_num; ++n, ++at) {
				memcpy(tiles + at * 64, en[n].data, 64);
			}
			for(n = 0; n < hed.npc_num; ++n, ++at) {
				memcpy(tiles + at * 64, npc[n].data, 64);
			}

			pages = num / 256;
			if(num % 256 > 0)
				++pages;
			spr_pages = pages;

			for(n = 0; n < pages; ++n) {
				sprintf(str, "spr%02d.pag", n);

				out = fopen(str, "wb");
				fwrite(tiles + n * 16384, 16384, 1, out);
				fclose(out);

				printf("Wrote [%s]\n", str);
			}
			// ***** END *****


			// ***** TILESETS *****
			// precalculate the tileset info
			num = hed.set_num;
			cur_seg = 0;
			cur_off = 0;
			for(n = 0; n < num; ++n) {
				i = set[n].num_tile * 16 + set[n].num_meta * 8 + 144;

				if(cur_off + i > 16384) {
					++cur_seg;
					cur_off = 0;
				}

				printf("[%4d:%4d, size=%d]\n", cur_seg, cur_off, i);
				stb_size[n] = i;
				stb_seg[n] = cur_seg;
				stb_off[n] = cur_off;

				cur_off += i;
			}
			pages = cur_seg + 1;
			printf("pages=%d\n", pages);


			set_pages = pages;
			i = 0;
			for(n = 0; n < pages; ++n) {
				sprintf(str, "stb%02d.pag", n);

				memset(buf, 0, 16384);
				at = 0;

				for(; stb_seg[i] == n && i < hed.set_num; ++i) {
					printf("--at=%d {meta=%d, tiles=%d, x=%d}--\n", at, set[i].num_meta, set[i].num_tile, at + 144 + (set[i].num_meta * 8));
					memcpy(buf + at, set[i].pal, 144);
					memcpy(buf + at + 144, set[i].meta, set[i].num_meta * 8);
					memcpy(buf + at + 144 + (set[i].num_meta * 8), set[i].data, set[i].num_tile * 16);
					at += stb_size[i];
				}
				printf("--end--\n");

				out = fopen(str, "wb");
				fwrite(buf, 16384, 1, out);
				fclose(out);
				printf("Wrote [%s]\n", str);
			}
			// ***** END *****


			// ***** MAPS *****
			// do the maps
/*			num = hed.map_num;
			pages = num / 4;
			if(num % 4 > 0)
				++pages;
			map_pages = pages;

			for(n = 0; n < pages; ++n) {
				sprintf(str, "map%02d.pag", n);

				if(n + 1 >= pages)
					num = hed.map_num % 4;
				else
					num = 4;

				memset(buf, 0, 16384);
				at = 0;
				for(n2 = 0; n2 < num; ++n2) {
					i = n * 4 + n2;
					memcpy(buf+at, map[i].data, 4096);
					at += 4096;
				}

				out = fopen(str, "wb");
				fwrite(buf, 16384, 1, out);
				fclose(out);
				printf("Wrote [%s]\n", str);
			}
*/

			// precalculate the map info
			num = hed.map_num;
			cur_seg = 0;
			cur_off = 0;
			for(n = 0; n < num; ++n) {
				i = map[n].x * map[n].y;

				if(cur_off + i > 16384) {
					++cur_seg;
					cur_off = 0;
				}

				printf("[%4d:%4d, size=%d]\n", cur_seg, cur_off, i);
				map_size[n] = i;
				map_seg[n] = cur_seg;
				map_off[n] = cur_off;

				cur_off += i;
			}
			pages = cur_seg + 1;
			printf("pages=%d\n", pages);


			map_pages = pages;
			i = 0;
			for(n = 0; n < pages; ++n) {
				sprintf(str, "map%02d.pag", n);

				memset(buf, 0, 16384);
				at = 0;

				for(; map_seg[i] == n && i < hed.map_num; ++i) {
					memcpy(buf + at, map[i].data, map_size[i]);
					at += map_size[i];
				}
				printf("--end--\n");

				out = fopen(str, "wb");
				fwrite(buf, 16384, 1, out);
				fclose(out);
				printf("Wrote [%s]\n", str);
			}

			// ***** END *****


			// ***** SONGS *****
			// precalculate the song info
			num = hed.song_num;
			cur_seg = 0;
			cur_off = 0;
			for(n = 0; n < num; ++n) {
				i = song[n].len;

				if(cur_off + i > 16384) {
					++cur_seg;
					cur_off = 0;
				}

				printf("[%4d:%4d, size=%d]\n", cur_seg, cur_off, i);
				song_size[n] = i;
				song_seg[n] = cur_seg;
				song_off[n] = cur_off;

				cur_off += i;
			}
			pages = cur_seg + 1;
			printf("pages=%d\n", pages);


			song_pages = pages;
			i = 0;
			for(n = 0; n < pages; ++n) {
				sprintf(str, "song%02d.pag", n);

				memset(buf, 0, 16384);
				at = 0;

				for(; song_seg[i] == n && i < hed.song_num; ++i) {
					memcpy(buf + at, song[i].data, song[i].len);
					at += song_size[i];
				}
				printf("--end--\n");

				out = fopen(str, "wb");
				fwrite(buf, 16384, 1, out);
				fclose(out);
				printf("Wrote [%s]\n", str);
			}
			// ***** END *****



			// ***** BUILD! *****
			printf("Making [table.pag]\n");

			out = fopen("table.pag", "wb");
			memset(buf, 0, 16384);

			atpage = 0;

			// sprites
			buf[0] = atpage + PAGEBASE+1;
			buf[1] = 0;
			buf[2] = 0x10;
			buf[3] = hed.ch_num;

			{
				int ch_num = hed.ch_num / CHSIZE;
				int en_num = hed.en_num / ENSIZE;
				int npc_num = hed.npc_num / NPCSIZE;
				buf[4096] = ch_num;
				buf[4097] = en_num;
				buf[4098] = npc_num;
				printf("ch_num = %d, en_num = %d, npc_num = %d\n", ch_num, en_num, npc_num);
			}

			at = 4;
			for(n = 0; n < hed.ch_num; ++n, ++at) {
				buf[4096+at] = ch[n].pal;
			}
			for(n = 0; n < hed.en_num; ++n, ++at) {
				buf[4096+at] = en[n].pal;
			}
			for(n = 0; n < hed.npc_num; ++n, ++at) {
				buf[4096+at] = npc[n].pal;
			}
			atpage += spr_pages;


			// tilesets
			buf[4] = atpage + PAGEBASE+1;
			buf[5] = 0; //1024
			buf[6] = 4; //1024

			for(n = 0; n < hed.set_num; ++n) {
				buf[1024 + n * 8] = stb_seg[n] & 0xff;
				buf[1025 + n * 8] = stb_off[n] & 0xff;
				buf[1026 + n * 8] = (stb_off[n] >> 8) & 0xff;
				buf[1027 + n * 8] = set[n].num_meta & 0xff;
				buf[1028 + n * 8] = set[n].num_tile & 0xff;
				buf[1029 + n * 8] = stb_size[n] & 0xff;
				buf[1030 + n * 8] = (stb_size[n] >> 8) & 0xff;
			}
			atpage += set_pages;


			// maps
			buf [8] = atpage + PAGEBASE+1;
			buf [9] = 0; //2048
			buf[10] = 8; //2048

			for(n = 0; n < hed.map_num; ++n) {
				buf[2048 + n * 8] = map_seg[n] & 0xff;
				buf[2049 + n * 8] = map_off[n] & 0xff;
				buf[2050 + n * 8] = (map_off[n] >> 8) & 0xff;
				buf[2051 + n * 8] = map[n].x;
				buf[2052 + n * 8] = map[n].y;
				buf[2053 + n * 8] = map[n].tileset;
				buf[2054 + n * 8] = map[n].info;
			}
			atpage += map_pages;


			// song
			buf[12] = atpage + PAGEBASE+1;
			buf[13] =  0; //3072
			buf[14] = 12; //3072

			for(n = 0; n < hed.song_num; ++n) {
				buf[3072 + n * 4] = song_seg[n] & 0xff;
				buf[3073 + n * 4] = song_off[n] & 0xff;
				buf[3074 + n * 4] = (song_off[n] >> 8) & 0xff;
				buf[3075 + n * 4] = song_size[n];
			}
			atpage += song_pages;


			fwrite(buf, 16384, 1, out);
			fclose(out);
		}
		else {
			printf("Doing nothing.\n");
		}
		printf("Done.\n");
	}
	return 0;
}
