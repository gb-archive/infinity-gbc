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

#ifndef ENEMY_H
#define ENEMY_H

struct ENEMY_STRUCT
{
        char name[32];
        int level, hp, sp;
        int str, end, agl, wis;
        int att, def, spd, mgd;
        char fire, water, wind, weapon;
	int exp, au, item;
	char ai[16];
};

struct ENEMIES
{
        struct ENEMY_STRUCT *ptr;
        int num;
};

void enemy_reset(struct ENEMIES *e);
void enemy_blank(struct ENEMY_STRUCT *en, char *name);
void enemy_add(struct ENEMIES *e, struct ENEMY_STRUCT *a);
void enemy_del(struct ENEMIES *e, int x);
void enemy_update(struct ENEMIES *e, int x, struct ENEMY_STRUCT *a);
void enemy_dup(struct ENEMIES *e1, struct ENEMIES *e2);
void enemy_free(struct ENEMIES *e);


struct FORM_STRUCT
{
        char name[32];
        int priority, map;

        char guy_active[12];
        char guy_type[12];
        char guy_x[12];
        char guy_y[12];
};

#define ZONE_STRUCT_SIZE 40

struct ZONE_STRUCT
{
        char name[32];
        int dummy; // 4 bytes of unused data
        int numforms;
        struct FORM_STRUCT *ptr;
};

struct ZONES
{
        struct ZONE_STRUCT *ptr;
        int num;
};

void zones_reset(struct ZONES *z);
void zone_blank(struct ZONE_STRUCT *zone, char *name);
void zone_dup(struct ZONE_STRUCT *zone1, struct ZONE_STRUCT *zone2);
void zones_add(struct ZONES *z, struct ZONE_STRUCT *zone);
void zones_del(struct ZONES *z, int x);
void zones_free(struct ZONES *z);
void zones_dup(struct ZONES *z1, struct ZONES *z2);

void form_blank(struct FORM_STRUCT *form, char *name);
void form_add(struct ZONE_STRUCT *zone, struct FORM_STRUCT *form);
void form_del(struct ZONE_STRUCT *zone, int x);
void form_free(struct ZONE_STRUCT *zone);

struct MAP {
        char name[32];
        char map[9][11];
};

#endif
