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

#define BCUR_NUM     2

#define BCUR_OFF     0
#define BCUR_FREEZE	1
#define BCUR_WIGGLE	2
#define BCUR_LOCKON	3

UBYTE bcur_slot;
WORD bcur_destx;
WORD bcur_desty;
UBYTE bcur_dist;

struct BCURSOR {
   WORD x,y;
   UBYTE dist;
   UBYTE ddist;
	UBYTE mode;
} bcur[BCUR_NUM];

void bcur_init()
{
	static UBYTE i;

   for(i=0;i<BCUR_NUM;i++)
      bcur[i].mode=BCUR_OFF;
}

void bcur_dest()
{
   bcur[bcur_slot].x=bcur_destx,bcur[bcur_slot].y=bcur_desty;
}

void bcur_on()
{
   bcur[bcur_slot].mode=BCUR_LOCKON;

   bcur[bcur_slot].dist=bcur_dist*2;
   bcur[bcur_slot].ddist=8;

   bcur_dest();
}

void bcur_off()
{
   bcur[bcur_slot].mode=BCUR_OFF;

   bspr_8set(0,0,0,-1,B_WDPAL);
   bspr_8set(1,0,0,-1,B_WDPAL);
   bspr_8set(2,0,0,-1,B_WDPAL);
   bspr_8set(3,0,0,-1,B_WDPAL);
}

void bcur_lock()
{
   bcur[bcur_slot].mode=BCUR_WIGGLE;

   bcur[bcur_slot].dist=16;
   bcur[bcur_slot].ddist=8;

   bcur_dest();
}

void bcur_update()
{
   static UBYTE i,w,z;

   for(i=0;i<BCUR_NUM;i++) {
      if(bcur[i].mode==BCUR_OFF)
			continue;

      bcur[i].dist=w=(BYTE)( (((WORD)bcur[i].dist)*3+(WORD)bcur[i].ddist+((bcur[i].dist<bcur[i].ddist)?3:0)) >>2);

      if(w==bcur[i].ddist)
          bcur[i].ddist=22-w;

      z=w+7;
      w++;

      bspr_8set(0,bcur[i].x-w+2,bcur[i].y-z,0,B_WDPAL);
      bspr_8set(1,bcur[i].x-w+2,bcur[i].y+w,1,B_WDPAL);
      bspr_8set(2,bcur[i].x+w  ,bcur[i].y-z,2,B_WDPAL);
      bspr_8set(3,bcur[i].x+w  ,bcur[i].y+w,3,B_WDPAL);
	}
}
