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
