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
#include<unistd.h>

#include<qapplication.h>
#include"frm_enemyImpl.h"
#include"frm_zoneImpl.h"
#include"enemy.h"

void enemy_reset(struct ENEMIES *e)
{
        e->ptr = NULL;
        e->num = 0;
}

void enemy_blank(struct ENEMY_STRUCT *en, char *name)
{
        strcpy(en->name, name);
        en->level = 1;
        en->hp = 10;
        en->sp = 0;

        en->str = 1;
        en->end = 0;
        en->agl = 1;
        en->wis = 0;

        en->att = 1;
        en->def = 1;
        en->spd = 1;
        en->mgd = 0;

        en->fire = 1;
        en->water = 1;
        en->wind = 1;
        en->weapon = 1;
}

void enemy_add(struct ENEMIES *e, struct ENEMY_STRUCT *a)
{
        e->ptr = (struct ENEMY_STRUCT *)realloc(e->ptr, sizeof(struct ENEMY_STRUCT) * (e->num+1));
        memcpy(&e->ptr[e->num], a, sizeof(struct ENEMY_STRUCT));
        ++e->num;
}

void enemy_del(struct ENEMIES *e, int x)
{
        int n;

        --e->num;
        for(n = x; n < e->num; ++n)
                memcpy(&e->ptr[n], &e->ptr[n+1], sizeof(struct ENEMY_STRUCT));
        if(e->num == 0) {
                free(e->ptr);
                e->ptr = NULL;
        }
        else
                e->ptr = (struct ENEMY_STRUCT *)realloc(e->ptr, sizeof(struct ENEMY_STRUCT) * e->num);
}

void enemy_update(struct ENEMIES *e, int x, struct ENEMY_STRUCT *a)
{
        memcpy(&e->ptr[x], a, sizeof(struct ENEMY_STRUCT));
}

void enemy_dup(struct ENEMIES *e1, struct ENEMIES *e2)
{
        int size;

        size = sizeof(struct ENEMY_STRUCT) * e1->num;
        e2->ptr = (struct ENEMY_STRUCT *)malloc(size);
        memcpy(e2->ptr, e1->ptr, size);
        e2->num = e1->num;
}

void enemy_free(struct ENEMIES *e)
{
        free(e->ptr);
        e->ptr = NULL;
        e->num = 0;
}


void zones_reset(struct ZONES *z)
{
        z->ptr = NULL;
        z->num = 0;
}

void zone_blank(struct ZONE_STRUCT *zone, char *name)
{
        int n;

        strcpy(zone->name, name);
        zone->ptr = NULL;
        zone->numforms = 0;
}

void zone_dup(struct ZONE_STRUCT *zone1, struct ZONE_STRUCT *zone2)
{
        int size;

        size = sizeof(struct FORM_STRUCT) * zone1->numforms;
        zone2->ptr = (struct FORM_STRUCT *)malloc(size);
        memcpy(zone2->ptr, zone1->ptr, size);
        strcpy(zone2->name, zone1->name);
        zone2->numforms = zone1->numforms;
}

void zones_dup(struct ZONES *z1, struct ZONES *z2)
{
        int n, size;

        size = sizeof(struct ZONE_STRUCT) * z1->num;
        z2->ptr = (struct ZONE_STRUCT *)malloc(size);
        for(n = 0; n < z1->num; ++n)
                zone_dup(&z1->ptr[n], &z2->ptr[n]);
        z2->num = z1->num;
}

void zones_add(struct ZONES *z, struct ZONE_STRUCT *zone)
{
        z->ptr = (struct ZONE_STRUCT *)realloc(z->ptr, sizeof(struct ZONE_STRUCT) * (z->num+1));
        memcpy(&z->ptr[z->num], zone, sizeof(struct ZONE_STRUCT));
        ++z->num;
}

void zones_del(struct ZONES *z, int x)
{
        int n;

        --z->num;
        for(n = x; n < z->num; ++n)
                memcpy(&z->ptr[n], &z->ptr[n+1], sizeof(struct ZONE_STRUCT));
        if(z->num == 0) {
                free(z->ptr);
                z->ptr = NULL;
        }
        else
                z->ptr = (struct ZONE_STRUCT *)realloc(z->ptr, sizeof(struct ZONE_STRUCT) * z->num);
}

void zones_free(struct ZONES *z)
{
        free(z->ptr);
        z->ptr = NULL;
        z->num = 0;
}

void form_blank(struct FORM_STRUCT *form, char *name)
{
        int n;

        strcpy(form->name, name);
        for(n = 0; n < 12; ++n) {
                form->guy_active[n] = 0;
                form->guy_type[n] = 0;
        }
        form->priority = 5;
        form->map = 0;
}

void form_add(struct ZONE_STRUCT *zone, struct FORM_STRUCT *form)
{
        zone->ptr = (struct FORM_STRUCT *)realloc(zone->ptr, sizeof(struct FORM_STRUCT) * (zone->numforms+1));
        memcpy(&zone->ptr[zone->numforms], form, sizeof(struct FORM_STRUCT));
        ++zone->numforms;
}

void form_del(struct ZONE_STRUCT *zone, int x)
{
        int n;

        --zone->numforms;
        for(n = x; n < zone->numforms; ++n)
                memcpy(&zone->ptr[n], &zone->ptr[n+1], sizeof(struct FORM_STRUCT));
        if(zone->numforms == 0) {
                free(zone->ptr);
                zone->ptr = NULL;
        }
        else
                zone->ptr = (struct FORM_STRUCT *)realloc(zone->ptr, sizeof(struct FORM_STRUCT) * zone->numforms);
}

void form_free(struct ZONE_STRUCT *zone)
{
        free(zone->ptr);
        zone->ptr = NULL;
        zone->numforms = 0;
}

int num_maps = 0;
struct MAP *maps = NULL;

void maps_add(char *name, char array[][11])
{
        maps = (struct MAP *)realloc(maps, (num_maps+1) * sizeof(struct MAP));
        strcpy(maps[num_maps].name, name);
        memcpy((char *)maps[num_maps].map, (char *)array, 99);
        ++num_maps;
}

main(int argc, char **argv)
{
        int n, n2, x, y;
        struct ENEMIES e, temp;
        struct ZONES z, ztemp;
        struct ENEMY_STRUCT en;
        struct ZONE_STRUCT zone;
        struct FORM_STRUCT form;
        char name[256], fname[256], line[256];
        char *p;
        FILE *f, *f2;
        char sig[16];
        char array[9][11] = {
                { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
                { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
                { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
                { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
                { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
                { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
                { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
                { 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1 },
                { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
        };

        if(argc < 2) {
                printf("Infinity Enemy Manager v1.0\n\n");
                printf("  usage: infenemy <.afz file>\n\n");
                return 1;
        }

        // load in maps
        f = fopen("hitfiles.txt", "r");
        if(!f) {
                printf("Couldn't open hitfiles.txt\n");
                printf("File format is \"name:file.hit\" on each line.\n");
                return 1;
        }

        while(1) {
                fgets(line, 255, f);
                if(feof(f))
                        break;

                p = strtok(line, ":\r\n ");
                if(!p)
                        continue;
                strcpy(name, p);
                p = strtok(NULL, ":\r\n ");
                if(!p)
                        continue;
                strcpy(fname, p);

                if(strlen(fname) < 1 || strlen(name) < 1)
                        continue;

                f2 = fopen(fname, "rb");
                if(!f2) {
                        printf("Unable to load map: %s\n", fname);
                        fclose(f);
                        return 1;
                }
                fread(array, 99, 1, f2);
                fclose(f2);

                maps_add(name, array);
        }
        fclose(f);

        enemy_reset(&e);
        zones_reset(&z);

        f = fopen(argv[1], "r+b");
        if(f) {
                printf("Opened: [%s]\n", argv[1]);

                fread(sig, 3, 1, f);
                if(strncmp(sig, "AFZ", 3)) {
                        printf("[%s] has an invalid format.\n", argv[1]);
                        fclose(f);
                        return 1;
                }

                // read enemies
                fread(&x, 4, 1, f);
                for(n = 0; n < x; ++n) {
                        fread(&en, sizeof(ENEMY_STRUCT), 1, f);
                        enemy_add(&e, &en);
                }

                // read zones
                fread(&x, 4, 1, f);
                printf("Zones: %d\n", x);
                for(n = 0; n < x; ++n) {
                        fread(&zone, sizeof(ZONE_STRUCT), 1, f);
                        y = zone.numforms;
                        zone.numforms = 0;
                        zone.ptr = NULL;

                        // read formations
                        printf("Zone [%s]:\n", zone.name);
                        printf("  Formations: %d\n", y);
                        for(n2 = 0; n2 < y; ++n2) {
                                fread(&form, sizeof(FORM_STRUCT), 1, f);
                                printf("    Form [%s]\n", form.name);
                                form_add(&zone, &form);
                        }

                        zones_add(&z, &zone);
                }

                fread(sig, 3, 1, f);
                if(strncmp(sig, "AFZ", 3)) {
                        printf("[%s] has an invalid format.\n", argv[1]);
                        fclose(f);
                        return 1;
                }

                // close
                fclose(f);
        }
        else {
                enemy_blank(&en, "Yak");
                en.level = 1;
                en.hp = 10;
                en.str = 1;
                en.end = 0;
                enemy_add(&e, &en);

                printf("New file: [%s]\n", argv[1]);
        }

        enemy_dup(&e, &temp);
        zones_dup(&z, &ztemp);

        // Fire up the enemy editor
        QApplication *app = new QApplication(argc, argv);
        frm_zoneImpl myform(0, 0, TRUE);
        n = myform.load(&temp, &ztemp, maps, num_maps);
        // Save changes?
        if(n == TRUE) {
                enemy_free(&e);
                zones_free(&z);
                e = temp;
                z = ztemp;
                enemy_reset(&temp);
                zones_reset(&ztemp);

                f = fopen(argv[1], "wb");

                // write signature
                fwrite("AFZ", 3, 1, f);

                // write enemies
                fwrite(&e.num, 4, 1, f);
                for(n = 0; n < e.num; ++n) {
                        fwrite(&e.ptr[n], sizeof(ENEMY_STRUCT), 1, f);
                }

                // write zones
                fwrite(&z.num, 4, 1, f);
                for(n = 0; n < z.num; ++n) {
                        fwrite(&z.ptr[n], sizeof(ZONE_STRUCT), 1, f);
                        for(n2 = 0; n2 < z.ptr[n].numforms; ++n2) {
                                fwrite(&z.ptr[n].ptr[n2], sizeof(FORM_STRUCT), 1, f);
                        }
                }
                fwrite("AFZ", 3, 1, f);
                fclose(f);

                printf("Changes saved.\n");
        }
        else {
                printf("No changes saved.\n");
        }

        return 0;
}

