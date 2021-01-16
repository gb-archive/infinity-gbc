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

// battle page 2
#include"defs.h"
#include"vars.h"

UBYTE tu, tu2;
WORD tpx, tpy;
WORD tqueue[32][2];
BYTE tz, tv, tw;

extern BYTE tab_slurptab[];
extern UBYTE tab_jolttab[][3];
extern BYTE tab_spiral_rel[][4];
extern BYTE tab_panel_rel[][32][4];

BYTE (*spiral_rel)[4] = (BYTE (*)[4])0xDC00;
//BYTE (*panel_rel)[32][2] = (BYTE (*)[32][2])0xDC00;
//UBYTE (*jolttab)[3] = (UBYTE (*)[3])0xDC00;

BYTE sword_rel[4][8] = {
   {   0,-5,       -8, 1,     -5, 6,       4,12 },
   {   7,-3,       10, 2,     -4, 8,      -5,-6 },
   {  -7,-3,      -10, 2,      4, 8,       5,-6 },
   {  11,-4,       12,-6,     -4,-7,      -9,-2 },
};

BYTE staff_rel[4][8] = {
   {   8,-3,       12, 3,     12,12,       4,14 },
   {   7,-1,        8, 6,      1,11,      -4, 4 },
   {   2,-1,        1, 6,      8,11,      13, 4 },
   {  -4,-2,        0,-5,     12,-4,      13, 6 },
};

BYTE scratch_rel[8] = {
    4,-4,        0, 0,      -4, 4,      0, 0,
};

BYTE spark_rel[8] = {
   -4, 4,
    0,-5,
    3, 0,
   -2,-1,
};

UBYTE get_4waydir(UBYTE bdir);               // need
void text_init(WORD x, WORD y, WORD value, UBYTE mode);
void text_update();                 // need

// actions
void baction_walk(UBYTE guy, WORD x, WORD y);
void baction_attk(UBYTE guy, WORD x, WORD y);
void baction_jump(UBYTE guy, WORD x, WORD y, WORD dist); // guy bit 7 = leap sfx
void baction_scrt(UBYTE guy);
void baction_poof(UBYTE guy);
void baction_cast(UBYTE guy);
void baction_pullbow(UBYTE guy);

void baction_cure(UBYTE guy);
void baction_sparkle(UBYTE guy);
void baction_fire(UBYTE guy, WORD x, WORD y);
void baction_jolt(UBYTE guy);
void baction_tidal(UBYTE guy);
void baction_quake(UBYTE guy);
void baction_panel(UBYTE guy);               // guy bit 7 = sacrifice sfx
void baction_throw(UBYTE guy, WORD x, WORD y, UBYTE type);
void baction_arrow(UBYTE guy, WORD x, WORD y, UBYTE dir);
void baction_wake(UBYTE guy);
void baction_slurp(UBYTE guy, WORD x, WORD y, UBYTE src);
void baction_spawn(UBYTE guy);
void baction_charm(UBYTE guy, WORD x, WORD y);
void baction_slash(UBYTE guy, WORD x, WORD y);
void baction_whirl(UBYTE guy);
void baction_damage(UBYTE guy);
void baction_effect(UBYTE guy, UBYTE type);

UBYTE baction_next(UBYTE guy);

// need
UBYTE bchain_do();
void bchain_stop();
void bchain_reset();
void bchain_add(UBYTE guy, UBYTE action, UBYTE dir, WORD arg1, WORD arg2, WORD arg3);
void bchain_insert(UBYTE guy, UBYTE action, UBYTE dir, WORD arg1, WORD arg2, WORD arg3);

// bline
#define SGN(num) (num<0?-1:1)
#define ABS(num) (num<0?-num:num)

void bline_set(UBYTE slot, WORD x1, WORD y1, WORD x2, WORD y2, UBYTE steps)
{
   brw_setup(&bch[slot].brw_x, x1, x2, steps);
   brw_setup(&bch[slot].brw_y, y1, y2, steps);
}

void bline_next(UBYTE slot)
{
   bch[slot].rx = brw_step(&bch[slot].brw_x);
   bch[slot].ry = brw_step(&bch[slot].brw_y);
}

void bpro_set(UBYTE slot, WORD x1, WORD y1, WORD x2, WORD y2, UBYTE steps)
{
   brw_setup(&bpro[slot].brw_x, x1, x2, steps);
   brw_setup(&bpro[slot].brw_y, y1, y2, steps);
}

void bpro_next(UBYTE slot)
{
   bpro[slot].rx = brw_step(&bpro[slot].brw_x);
   bpro[slot].ry = brw_step(&bpro[slot].brw_y);
}


void bchain_stop()
{
   bchain_active = 0;
   bfocus = -1;
}

void bchain_reset()
{
   bearly = 0;
   bchain_active = 1;
   bchain_steps = 0;
   bchain_at = -1;
   bfocus = -1;
}

void bchain_add(UBYTE guy, UBYTE action, UBYTE dir, WORD arg1, WORD arg2, WORD arg3)
{
   bchain_table[bchain_steps].guy = guy;
   bchain_table[bchain_steps].action = action;
   bchain_table[bchain_steps].dir = dir;
   bchain_table[bchain_steps].arg1 = arg1;
   bchain_table[bchain_steps].arg2 = arg2;
   bchain_table[bchain_steps].arg3 = arg3;
   ++bchain_steps;
}

void bchain_insert(UBYTE guy, UBYTE action, UBYTE dir, WORD arg1, WORD arg2, WORD arg3)
{
   UBYTE n;

   for(n = bchain_steps; n > bchain_at; --n) {
      memcpy(&bchain_table[n], &bchain_table[n-1], (UWORD)sizeof(struct BCHAIN));
   }
   n = bchain_at;

   bchain_table[n].guy = guy;
   bchain_table[n].action = action;
   bchain_table[n].dir = dir;
   bchain_table[n].arg1 = arg1;
   bchain_table[n].arg2 = arg2;
   bchain_table[n].arg3 = arg3;
   ++bchain_steps;
}

void bchain_donext()
{
   UBYTE u;
        struct BCHAIN *bp;

   if(bchain_at >= bchain_steps) {
      bchain_active = 0;
      return;
   }

        bp = &bchain_table[bchain_at];

   u = bp->action;
   switch(u) {
      case BACT_WALK:
         baction_walk(bp->guy, bp->arg1, bp->arg2);
         break;
      case BACT_JUMP:
         baction_jump(bp->guy, bp->arg1, bp->arg2, bp->arg3);
         break;
      case BACT_ATTACK:
         baction_attk(bp->guy, bp->arg1, bp->arg2);
         break;
      case BACT_SCRATCH:
         baction_scrt(bp->guy);
         b_dir = bp->dir;
         break;
      case BACT_POOF:
         baction_poof(bp->guy);
         break;
      case BACT_CURE:
         baction_cure(bp->guy);
         break;
      case BACT_SPARKLE:
         baction_sparkle(bp->guy);
         break;
      case BACT_CAST:
         baction_cast(bp->guy);
         break;
      case BACT_PULLBOW:
         baction_pullbow(bp->guy);
         break;
      case BACT_FIRE:
         baction_fire(bp->guy, bp->arg1, bp->arg2);
         break;
      case BACT_JOLT:
         baction_jolt(bp->guy);
         break;
      case BACT_TIDAL:
         baction_tidal(bp->guy);
         break;
      case BACT_QUAKE:
         baction_quake(bp->guy);
         break;
      case BACT_PANEL:
         baction_panel(bp->guy);
         break;
      case BACT_THROW:
         baction_throw(bp->guy, bp->arg1, bp->arg2, bp->arg3);
         break;
      case BACT_ARROW:
         baction_arrow(bp->guy, bp->arg1, bp->arg2, bp->dir);
         break;
      case BACT_WAKE:
         baction_wake(bp->guy);
         break;
      case BACT_SLURP:
         baction_slurp(bp->guy, bp->arg1, bp->arg2, bp->arg3);
         break;
      case BACT_SPAWN:
         baction_spawn(bp->guy);
         break;
      case BACT_CHARM:
         baction_charm(bp->guy, bp->arg1, bp->arg2);
         break;
      case BACT_SLASH:
         baction_slash(bp->guy, bp->arg1, bp->arg2);
         break;
      case BACT_WHIRL:
         baction_whirl(bp->guy);
         break;
      case BACT_DAMAGE:
         baction_damage(bp->guy);
         break;
      case BACT_EFFECT:
         baction_effect(bp->guy, bp->arg1);
         break;
   }

   if(u == BACT_WALK || u == BACT_JUMP || u == BACT_ATTACK || u == BACT_PULLBOW) {
      bch[bp->guy & 127].dir = bp->dir;
   }

        // strange, but should work
        //if(u == BACT_ARROW)
        //        bch[turn].dir = bchain_table[bchain_at].dir;

   ++bchain_at;
}

UBYTE bchain_do()
{
   if(!bchain_active) {
      bfocus = -1;
      bspr_quake_off();
      f_bcoloroff();
      return 0;
   }

   if(bchain_at == -1) {
      bchain_at = 0;
      bchain_donext();
      return 1;
   }

   if(!baction_next(bchain_table[bchain_at - 1].guy & 127)) {
      bchain_donext();
   }

   return 1;
}


void baction_walk(UBYTE guy, WORD x, WORD y)
{
   bline_set(guy, bch[guy].rx, bch[guy].ry, x, y, BMOVESPEED);
   bch[guy].action = BACT_WALK;
   bch[guy].count  = BMOVESPEED;
   bch[guy].progress = 1;
}

void baction_attk(UBYTE guy, WORD x, WORD y)
{
        // 7th bit set if no damage
        tu = guy & 128;
        guy &= 127;

   bch[guy].action = BACT_ATTACK;
   bch[guy].count = 1;
   bch[guy].progress = 5;

   tpx = x;
   tpy = y;

   // elya
   if(bch[guy].type == ELYA)
      load_bgfx(10);
   // victor
   else if(bch[guy].type == VICTOR || bch[guy].type == VICTOR_WOLF)
      load_bgfx(1); // scratch graphics
   // conner, roland, alutha
   else
      load_bgfx(0);

   // ryan does not have a regular attack
}

void baction_scrt(UBYTE guy)
{
   bch[guy].action = BACT_SCRATCH;
   bch[guy].count = 1;
   bch[guy].progress = 4;
   load_bgfx(1);
}

void baction_poof(UBYTE guy)
{
   bch[guy].action = BACT_POOF;
   bch[guy].count = 1;
   bch[guy].progress = 5;
   load_bgfx(2);
}

void baction_jump(UBYTE guy, WORD x, WORD y, WORD dist)
{
   UBYTE d;
   UBYTE jumpspeed;

        // 7th bit indicates sound
   tu2 = guy & 128;
   guy &= 127;

   tu = dist;
   jumpspeed = BJUMPSPEED * tu;
   if(!(jumpspeed & 1))
      ++jumpspeed;
   tw = jumpspeed;

   d = get_4waydir(bch[guy].dir);
   bline_set(guy, bch[guy].rx, bch[guy].ry, x, y, jumpspeed);

   bch[guy].action = BACT_JUMP;
   bch[guy].count  = jumpspeed;
   bch[guy].dz = jumpspeed / 2;
   bch[guy].progress = 1;

   if(bch[guy].control == BPLAYER)
      bch[guy].frame = d + 8;
   else {
      if(bch[guy].flags & BOSS_COMPACT)
         bch[guy].frame = flop;
      else
         bch[guy].frame = (d << 1) + flop;
   }
}

void baction_cure(UBYTE guy)
{
   bch[guy].action = BACT_CURE;
   bch[guy].count = 32;
   bch[guy].progress = 5;
   load_bgfx(3);

   f_memcpy((void *)0xDC00, (void *)tab_spiral_rel, BATTLE4_PAGE, 128);
}

void baction_sparkle(UBYTE guy)
{
   bch[guy].action = BACT_CURE;
   bch[guy].count = 0;
   bch[guy].progress = 4;
   load_bgfx(3);

   f_memcpy((void *)0xDC00, (void *)tab_spiral_rel, BATTLE4_PAGE, 128);
}

void baction_cast(UBYTE guy)
{
   bch[guy].action = BACT_CAST;
   bch[guy].count = 1;
   bch[guy].progress = 5;
}

void baction_pullbow(UBYTE guy)
{
   bch[guy].action = BACT_PULLBOW;
   bch[guy].count = 1;
   bch[guy].progress = 5;
}

void baction_fire(UBYTE guy, WORD x, WORD y)
{
   UBYTE n;

   bpro_set(0, x, y, bch[guy].rx, bch[guy].ry, 24);

   bch[guy].action = BACT_FIRE;
   bch[guy].count = 24;
   bch[guy].progress = 2;
   load_bgfx(4);

   for(n = 0; n != 5; ++n) {
      tqueue[n][0] = x;
      tqueue[n][1] = y;
   }
}

void baction_jolt(UBYTE guy)
{
   bch[guy].action = BACT_JOLT;
   bch[guy].count = 12;
   bch[guy].progress = 13;
   load_bgfx(5);

   f_memcpy((void *)0xDC00, (void *)tab_jolttab, BATTLE4_PAGE, 128);
}

void baction_tidal(UBYTE guy)
{
   bch[guy].action = BACT_TIDAL;
   bch[guy].count = 24;
   bch[guy].progress = 1;
   load_bgfx(6);
}

void baction_quake(UBYTE guy)
{
   bch[guy].action = BACT_QUAKE;
   bch[guy].count = 16;
   bch[guy].progress = 1;

   bspr_quake_on();
}

void baction_panel(UBYTE guy)
{
   tu2 = guy & 128;
   guy &= 127;

   bch[guy].action = BACT_PANEL;
   bch[guy].count = 32;
   bch[guy].progress = 1;
   load_bgfx(7);

   f_memcpy((void *)0xDC00, (void *)tab_panel_rel, BATTLE4_PAGE, 384);
}

void baction_throw(UBYTE guy, WORD x, WORD y, UBYTE type)
{
   UBYTE n;

   bpro_set(0, x, y, bch[guy].rx, bch[guy].ry, 26);

   bch[guy].action = BACT_THROW;
   bch[guy].count = 26;
   bch[guy].progress = 1;
   tu = load_bgfx_item(type);

   tz = 0;
   tv = -6;
}

void baction_arrow(UBYTE guy, WORD x, WORD y, UBYTE dir)
{
   UBYTE n;

   bpro_set(0, x, y, bch[guy].rx, bch[guy].ry, 8);

   bch[guy].action = BACT_ARROW;
   bch[guy].count = 8;
   bch[guy].progress = 1;
   load_bgfx(8);

   tu = get_4waydir(dir);
   //tz = 0;
   //tv = -6;
}

void baction_wake(UBYTE guy)
{
   bch[guy].action = BACT_WAKE;
   bch[guy].count = 64;
   bch[guy].progress = 1;
   load_bgfx(9);
}

void baction_slurp(UBYTE guy, WORD x, WORD y, UBYTE src)
{
   tu = src;
   bch[guy].action = BACT_SLURP;
   if(tu & 128)
      bch[guy].count = 40;
   else
      bch[guy].count = 52;
   bch[guy].progress = 1;
   load_bgfx(11);

   bpro_set(0, bch[guy].rx, bch[guy].ry, x, y, 40);

   f_memcpy((void *)0xDC00, (void *)tab_slurptab, BATTLE4_PAGE, 64);
}

void baction_spawn(UBYTE guy)
{
   bch[guy].action = BACT_SPAWN;
   bch[guy].progress = 5;
   bch[guy].count = 4;

   load_bgfx(12);
   tu = 3;
}

void baction_charm(UBYTE guy, WORD x, WORD y)
{
   bpro_set(0, x, y, bch[guy].rx, bch[guy].ry, 48);

   bch[guy].action = BACT_CHARM;
   bch[guy].count = 48 + 19;
   bch[guy].progress = 1;
   load_bgfx(3);

   for(n = 0; n != 19; ++n) {
      tqueue[n][0] = x;
      tqueue[n][1] = y;
   }
   tv = -1;
   tz = 3;
   tw = 0;
}

void baction_slash(UBYTE guy, WORD x, WORD y)
{
        bpro_set(0, bch[guy].rx, bch[guy].ry, x, y, 56);

        bch[guy].action = BACT_SLASH;
        bch[guy].count = 56;
        bch[guy].progress = 1;
        load_bgfx(13);

        /*for(n = 0; n != 19; ++n) {
                tqueue[n][0] = x;
                tqueue[n][1] = y;
        }
        tv = -1;
        tz = 3;
        tw = 0;*/
}

void baction_whirl(UBYTE guy)
{
        bch[guy].action = BACT_WHIRL;
        bch[guy].count = 32;
        bch[guy].progress = 1;

        // load the big sword whirl graphics
        load_bgfx(14);
}

void baction_damage(UBYTE guy)
{
        bch[guy].action = BACT_DAMAGE;
        bch[guy].count = 32;
        bch[guy].progress = 1;
}

void baction_effect(UBYTE guy, UBYTE type)
{
        tu = type;
        bch[guy].action = BACT_EFFECT;
        bch[guy].count = 1;
        bch[guy].progress = 1;
}


UBYTE curaction;
UBYTE nextguy;
extern UBYTE brun;

UBYTE baction_next(UBYTE guy)
{
   UBYTE u, u2;
   UBYTE a1, a2;
   UBYTE d;
   BYTE s;
   UBYTE pal;


   curaction = bch[guy].action;

   if(curaction == BACT_NORM)
      return 0;

   d = get_4waydir(bch[guy].dir);

   if(curaction == BACT_JUMP) {
      u = bch[guy].count;
      if(u == 1) {
         if(brun)
            play_sfx(SFX_ESCAPE);
         if(tu2)
            play_sfx(SFX_LAND);
      }
      else if(u == tw - 1) { // first frame
         if(tu2)
            play_sfx(SFX_LEAP);
      }

      bline_next(guy);
      u = bch[guy].dz;
      //u >>= tu;
      bch[guy].rz += u;
      --bch[guy].dz;
   }
   else if(curaction == BACT_WALK) {
      bline_next(guy);
      if(bch[guy].flags & BOSS_COMPACT)
         bch[guy].frame = flop;
      else
         bch[guy].frame = (d << 1) + flop;
   }
   else if(curaction == BACT_SCRATCH) {
      // only act when progress changes
      if(bch[guy].count == 0) {
         u = 3 - bch[guy].progress;
         if(u == 1)
            play_sfx(SFX_CLAW);

         u2 = 32 + (u << 1);
         bch[guy].count = 6;

         px = bch[guy].rx - 3;
         py = bch[guy].ry - 3;

         px += scratch_rel[0 + (u<<1)];
         py += scratch_rel[1 + (u<<1)];

         bspr_8set(22, px,     py, u2,   5);
         bspr_8set(23, px + 8, py, u2+1, 5);

         if(u == 2) {
            px = bch[guy].rx;
            py = bch[guy].ry;
            f_gemcheck(guy);
            text_init(px, py, bch[guy].damage, b_dir);
            f_bresolve(guy);
         }
      }
      if(bch[guy].control == BPLAYER)
         bch[guy].frame = 48;
   }
   else if(curaction == BACT_ATTACK) {
      // only act when progress changes
      if(bch[guy].count == 0) {
         u = 4 - bch[guy].progress;
         if(u == 1) {
                                // staff
            if(bch[guy].type == 2) {
               play_sfx(SFX_STAFF);
            }
                                // anything but the slash skill
            else if(!tu) {
               play_sfx(SFX_SWORD);
                                }
                                // slash skill
                                else {
                                        play_sfx(SFX_SWORD);
                                }
         }

         u2 = 32 + (d<<3);
         u2 += (u<<1);

         if(u == 0 || u == 3)
            bch[guy].count = 8;
         else
            bch[guy].count = 3;

         bch[guy].frame = (d << 2) + BFRAME_ATTACK;
         bch[guy].frame += u;

         px = bch[guy].rx - 3;
         py = bch[guy].ry - 3;

         // staff attack
         if(bch[guy].type == ELYA) {
            px += staff_rel[d][0 + (u<<1)];
            py += staff_rel[d][1 + (u<<1)];
            u2 = 32;
            pal = 1;
         }
         // claw
         else if(bch[guy].type == VICTOR || bch[guy].type == VICTOR_WOLF) {
            px = tpx - 3;
            py = tpy - 3;

            if(u == 0) {
               u2 = 32;
               pal = 255;
            }
            else {
               --u;
               px += scratch_rel[0 + (u<<1)];
               py += scratch_rel[1 + (u<<1)];
               u2 = 32 + (u << 1);
               pal = 0;
               ++u;
            }
         }
         // sword attack
         else {
            px += sword_rel[d][0 + (u<<1)];
            py += sword_rel[d][1 + (u<<1)];
            pal = 4;
         }

         if(pal != 255) {
            bspr_8set(22, px,     py, u2,   pal);
            bspr_8set(23, px + 8, py, u2+1, pal);
         }

         if(u == 3 && !tu) {
            px = bch[b_target].rx;
            py = bch[b_target].ry;
            f_gemcheck(b_target);
            text_init(px, py, bch[b_target].damage, bch[guy].dir);
            f_bresolve(b_target);
         }
      }
   }
   else if(curaction == BACT_POOF) {
      // only act when progress changes
      if(bch[guy].count == 0) {
         u = 4 - bch[guy].progress;
         if(u == 0)
            play_sfx(SFX_POOF);
         u2 = 32 + (u << 1);
         bch[guy].count = 4;

         px = bch[guy].rx - 3;
         py = bch[guy].ry - 3;

         bspr_8set(22, px,     py, u2,   5);
         bspr_8set(23, px + 8, py, u2+1, 5);
      }
   }
   else if(curaction == BACT_CURE) {
      if(bch[guy].count == 0) {
         bch[guy].count = 7;
         if(bch[guy].progress == 4) {
            bspr_8set(24, 0, 0, -1, 0);
            bspr_8set(25, 0, 0, -1, 0);
            play_sfx(SFX_SPARKLE);
         }

         u = 4 - bch[guy].progress;
         if(u == 2) {
            px = bch[guy].rx;
            py = bch[guy].ry;
            f_gemcheck(guy);
            a1 = BDIR_UP;
            if(bch[guy].spflag)
               a1 |= 128;
            text_init(px, py, bch[guy].damage, a1);
            f_bresolve(guy);
         }

         u <<= 1;
         tpx = bch[guy].rx - 3 + spark_rel[u];
         tpy = bch[guy].ry - 3 + spark_rel[u+1];
      }

      // spiral
      if(bch[guy].progress == 5) {
         u = 32 - bch[guy].count;
         if(u == 0)
            play_sfx(SFX_CURE);

         px = bch[guy].rx - 3 + spiral_rel[u][0];
         py = bch[guy].ry - 3 + spiral_rel[u][1];

         bspr_8set(22, px,     py,  32 + 8, 1);
         bspr_8set(23, px + 8, py,  32 + 9, 1);

         px = bch[guy].rx - 3 + spiral_rel[u][2];
         py = bch[guy].ry - 3 + spiral_rel[u][3];

         bspr_8set(24, px,     py,  32 + 8, 1);
         bspr_8set(25, px + 8, py,  32 + 9, 1);
      }
      // sparks
      else {
         u = 7 - bch[guy].count;
         if(u > 3)
            u = 6 - u;
         u <<= 1;

         bspr_8set(22, tpx,     tpy,  32 + u, 1);
         bspr_8set(23, tpx + 8, tpy,  33 + u, 1);
      }
      bfocus = guy;
   }
   else if(curaction == BACT_CAST) {
      // only act when progress changes
      if(bch[guy].count == 0) {
         u = 4 - bch[guy].progress;
         if(bch[guy].control == BPLAYER)
            bch[guy].frame = 32 + u;
         else {
            if(bch[guy].flags & BOSS_COMPACT)
               bch[guy].frame = 2;
            else
               bch[guy].frame = 8;
         }
         bch[guy].count = 4;
         if(bch[guy].progress == 1 && bearly) {
            bearly = 0;
            f_gemcheck(guy);
         }
      }
      bfocus = guy;
   }
   else if(curaction == BACT_PULLBOW) {
      // only act when progress changes
      if(bch[guy].count == 0) {
         u = 4 - bch[guy].progress;
         if(u == 3)
            play_sfx(SFX_ARROWSHOT);

         bch[guy].frame = 16 + u + (d<<2);
         bch[guy].count = 4;
         if(bch[guy].progress == 1 && bearly) {
            bearly = 0;
            f_gemcheck(guy);
         }
      }
      bfocus = guy;
   }
   else if(curaction == BACT_FIRE) {
      if(bch[guy].count == 0) {
         bspr_8set(22, 0, 0, -1, 0);
         bspr_8set(23, 0, 0, -1, 0);
         bspr_8set(24, 0, 0, -1, 0);
         bspr_8set(25, 0, 0, -1, 0);
         bspr_8set(26, 0, 0, -1, 0);
         bspr_8set(27, 0, 0, -1, 0);

         bch[guy].count = 40;
      }

      if(bch[guy].progress == 2) {
         u = 24 - bch[guy].count;
         if(u == 0)
            play_sfx(SFX_ENGULF);

         u >>= 1;
         u &= 1;
         u <<= 1;

         bpro_next(0);
         px = bpro[0].rx - 3;
         py = bpro[0].ry - 3;

         for(u2 = 0; u2 != 4; ++u2) {
            tqueue[u2][0] = tqueue[u2+1][0];
            tqueue[u2][1] = tqueue[u2+1][1];
         }
         tqueue[4][0] = px;
         tqueue[4][1] = py;

         bspr_8set(22, px,     py, 42 - u, 0);
         bspr_8set(23, px + 8, py, 43 - u, 0);

         px = tqueue[2][0];
         py = tqueue[2][1];
         bspr_8set(24, px,     py, 38 - u, 0);
         bspr_8set(25, px + 8, py, 39 - u, 0);

         px = tqueue[0][0];
         py = tqueue[0][1];
         bspr_8set(26, px,     py, 34 - u, 0);
         bspr_8set(27, px + 8, py, 35 - u, 0);
      }
      else {
         u = 40 - bch[guy].count;
         if(u == 5)
            play_sfx(SFX_ENGULFFLAME);

         if(u == 24) {
            px = bch[guy].rx;
            py = bch[guy].ry;
            f_gemcheck(guy);
            text_init(px, py, bch[guy].damage, BDIR_UP);
            f_bresolve(guy);
         }
         if(u < 32) {
            px = bch[guy].rx - 3;
            py = bch[guy].ry - 3;

            // rise up
            if(u < 8) {
               u &= 6;
            }
            else if(u < 24) {
               u &= 1;
               u <<= 1;
               u += 8;
            }
            else {
               u -= 24;
               u &= 6;
               u += 12;
            }

            py += 2;

            bspr_8set(22, px,     py, 44 + u, 0);
            bspr_8set(23, px + 8, py, 45 + u, 0);
         }
         else {
            bspr_8set(22, 0, 0, -1, 0);
            bspr_8set(23, 0, 0, -1, 0);
            bch[guy].count = 1;
         }
      }
      bfocus = guy;
   }
   else if(curaction == BACT_QUAKE) {
      if(bch[guy].count == 16) {
         px = bch[guy].rx;
         py = bch[guy].ry;
         f_gemcheck(guy);
         text_init(px, py, bch[guy].damage, BDIR_UP);
         f_bresolve(guy);
         bfocus = guy;
      }
   }
   else {
      nextguy = guy;
      f_baction_nextmore();
   }

   // count down, change progress
   --bch[guy].count;
   if(bch[guy].count == 0) {
      --bch[guy].progress;
   }

   // if done, clean up and bail
   if(bch[guy].progress == 0) {
      for(n = 0; n != 6; ++n)
         bspr_8set(22 + n, 0, 0, -1, 0);
      do_delay(1);

      if(curaction == BACT_POOF) {
         bch[guy].poof = 2;
      }
      if(!bch[guy].poof) {
         if(bch[guy].flags & BOSS_COMPACT)
            bch[guy].frame = flop;
         else
            bch[guy].frame = (d << 1) + flop;
         bch[guy].action = BACT_NORM;
      }
      return 0;
   }

   return 1;
}

UBYTE text_active, text_mode;
WORD text_x, text_y;
BYTE text_x2, text_y2;
BYTE text_x3, text_y3;
UBYTE text_c1, text_c2, text_c3;
extern UBYTE a1, a2, a3, a4;
UBYTE text_miss, text_frame;
UBYTE text_pal;

void text_show()
{
   if(text_miss) {
      text_frame = (text_active >> 3) & 0x03;
      bspr_8set(28, text_x,     text_y, 80 + (text_frame << 1), 6);
      bspr_8set(29, text_x + 8, text_y, 81 + (text_frame << 1), 6);
   }
   else {
      bspr_8set(28, text_x,           text_y,           text_c1 + 15, text_pal);
      bspr_8set(29, text_x + text_x2, text_y + text_y2, text_c2 + 15, text_pal);
      bspr_8set(30, text_x + text_x3, text_y + text_y3, text_c3 + 15, text_pal);
   }
}

void text_init(WORD x, WORD y, WORD value, UBYTE mode)
{
   UWORD n;
   UBYTE spmode;

   text_x = x;
   text_y = y;
   text_x2 = 0;
   text_y2 = 0;
   text_x3 = 0;
   text_y3 = 0;

   text_pal = 6;
   text_miss = 0;

   spmode = 0;
   if(mode & 128) {
      mode &= 127;
      spmode = 1;
   }

   if(value < 0) {
      value = -value;
      text_pal = spmode ? 0: 1;
   }
   else if(value == 0)
      text_miss = 1;

   // get the 3 digits
   n = value / 100;
   value %= 100;
   a1 = n;
   n = value / 10;
   value %= 10;
   a2 = n;
   a3 = value;

   // deal with zeros
   ++a3;
   if(a1 != 0)
      ++a1;
   if(a2 != 0)
      ++a2;
   if(a2 == 0 && a1 != 0)
      ++a2;


   // assign the sprites
   if(mode == BDIR_UP || mode == BDIR_DOWN) {
      text_c1 = a2;
      text_c2 = a1;
      text_c3 = a3;
   }
   else if(mode == BDIR_UPRIGHT || mode == BDIR_DOWNRIGHT) {
      text_c1 = a1;
      text_c2 = a2;
      text_c3 = a3;
   }
   else if(mode == BDIR_UPLEFT || mode == BDIR_DOWNLEFT) {
      text_c1 = a3;
      text_c2 = a2;
      text_c3 = a1;
   }

   text_show();
   text_active = 1;
   text_mode = mode;
}

void text_off()
{
   bspr_8set(28, 0, 0, -1, 0);
   bspr_8set(29, 0, 0, -1, 0);
   bspr_8set(30, 0, 0, -1, 0);

   text_active = 0;
}

void text_update()
{
   if(!text_active)
      return;

   if(text_active >= (32 << text_miss)) {
      text_off();
      return;
   }

   if(text_mode == BDIR_UP) {
      if(text_active < 8) {
         text_y -= 2;
      }
      else if(text_active < 12) {
         text_x2 -= 2;
         text_x3 += 2;
      }
   }
   else if(text_mode == BDIR_UPRIGHT) {
      if(text_active < 8) {
         text_x += 2;
         text_y -= 2;
      }
      else if(text_active < 12) {
         text_x2 += 2;
         text_x3 += 2;
      }
      else if(text_active < 16) {
         text_x3 += 2;
      }
   }
   else if(text_mode == BDIR_DOWNRIGHT) {
      if(text_active < 8) {
         text_x += 2;
         text_y += 2;
      }
      else if(text_active < 12) {
         text_x2 += 2;
         text_x3 += 2;
      }
      else if(text_active < 16) {
         text_x3 += 2;
      }
   }
   else if(text_mode == BDIR_DOWN) {
      if(text_active < 8) {
         text_y += 2;
      }
      else if(text_active < 12) {
         text_x2 -= 2;
         text_x3 += 2;
      }
   }
   else if(text_mode == BDIR_DOWNLEFT) {
      if(text_active < 8) {
         text_x -= 2;
         text_y += 2;
      }
      else if(text_active < 12) {
         text_x2 -= 2;
         text_x3 -= 2;
      }
      else if(text_active < 16) {
         text_x3 -= 2;
      }
   }
   else if(text_mode == BDIR_UPLEFT) {
      if(text_active < 8) {
         text_x -= 2;
         text_y -= 2;
      }
      else if(text_active < 12) {
         text_x2 -= 2;
         text_x3 -= 2;
      }
      else if(text_active < 16) {
         text_x3 -= 2;
      }
   }

   ++text_active;
   text_show();
}

UBYTE get_4waydir(UBYTE bdir)
{
   if(bdir == BDIR_UP)
      return 3;
   else if(bdir == BDIR_UPRIGHT)
      return 2;
   else if(bdir == BDIR_DOWNRIGHT)
      return 2;
   else if(bdir == BDIR_DOWN)
      return 0;
   else
      return 1;
}
