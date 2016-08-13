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

#include"defs.h"
#include"vars.h"

UBYTE randpick[16];
extern UBYTE bhi, blo;

WORD b6_hplo[3]  = { 40, 100, 200 };
WORD b6_splo[3]  = { 15,  30,  50 };
WORD b6_hpall[3] = { 30,  80, 150 };


// returns 1 if (x,y) is totally free and available
UBYTE freespace(UBYTE x, UBYTE y)
{
   UBYTE n, n2;

   if(x > 10) // || x < 0, unsigned
      return 0;
   if(y > 7 + (x & 1)) // || y < 0, unsigned
      return 0;

   if(bmap[y][x])
      return 0;

   for(n = 0; n != bnumdudes; ++n) {
      if(!bch[n].active)
         continue;

      if(bch[n].x == x && bch[n].y == y)
         return 0;
   }

   return 1;
}

// returns guy+1 if there is a guy at the space
UBYTE isguy(UBYTE x, UBYTE y)
{
   UBYTE n;

   for(n = 0; n != bnumdudes; ++n) {
      if(!bch[n].active)
         continue;

      if(bch[n].x == x && bch[n].y == y)
         return n + 1;
   }

   return 0;
}


UBYTE bfliptab[] = { BDIR_DOWN, BDIR_DOWNLEFT, BDIR_UPLEFT, BDIR_UP, BDIR_UPRIGHT, BDIR_DOWNRIGHT };
UBYTE bflipdir(UBYTE bdir)
{
   return bfliptab[bdir];
}

void b6_bgetreal(struct BDUDE *bch)
{
   WORD px, py;
   BYTE even;

   //even = ~bch->x & 1;
   even = bch->x % 2;
   even = even ? 0: 1;

   px = bch->x;
   px *= 24;
   py = bch->y;
   py *= 24;
   py += (WORD)even * 12;

   px += 8;
   py += 8;

   bch->rx = px;
   bch->ry = py;
}

void b6_bgetrealv(struct BDUDE *bch, WORD *x, WORD *y)
{
   WORD px, py;
   BYTE even;

   //even = ~bch->x & 1;
   even = bch->x % 2;
   even = even ? 0: 1;

   px = bch->x;
   px *= 24;
   py = bch->y;
   py *= 24;
   py += (WORD)even * 12;

   px += 8;
   py += 8;

   *x = px;
   *y = py;
}

UBYTE finddir(WORD x1, WORD y1, WORD x2, WORD y2)
{
   WORD dx, dy;
   UBYTE r, l, d;

   dx = x2 - x1;
   dy = y2 - y1;
   r = dy >= dx * 2;
   l = dy >= dx * -2;
   d = dy >= 0;

   if(r ^ l ^ 1)
      return d ? BDIR_UP: BDIR_DOWN;
   if(r)
      return d ? BDIR_UPRIGHT: BDIR_DOWNRIGHT;

   return d ? BDIR_UPLEFT: BDIR_DOWNLEFT;
}

void magic_damage(UBYTE turn, UBYTE b_target, UBYTE base, UBYTE multiplier)
{
   UBYTE v, v2, v3;

   v  = bch[turn].st.wis     + bch[turn].st.man;
   v2 = bch[b_target].st.wis + bch[b_target].st.man;
   b_damage = (WORD)base + (((WORD)v * (WORD)multiplier) >> 4) - (WORD)v2;

   // make sure b_damage isn't negative
   f_bdamagecap();

   // enemies do 80% magic damage
   if(bch[turn].control == BCPU) {
      b_damage = (b_damage << 2) / 5;
   }

   // mod damage
   bcm_damage = b_damage;
   bcm_guy = turn;
   bcm_target = b_target;
   f_bcalcmod();
   b_damage = bcm_damage;

   f_bdamageflux();
   f_bdamagecap();
}

extern WORD bw, bw2, bw3;
void start_skill()
{
   UBYTE dir;
   UBYTE d;
   UBYTE numguys;
   UBYTE type;
   UBYTE x;

   WORD w, w2, w3;
   UBYTE v, v2, v3;

   if(!skl_gem) {
      if(bch[turn].st.sp >= skl_cost)
         bch[turn].st.sp -= skl_cost;
      else
         return;
   }
   else {
      if(gems_charge[bgemselect] >= skl_cost)
         gems_charge[bgemselect] -= skl_cost;
      else
         return;
   }

      f_bchain_reset();

      // enemies get a panel animation
      if(bch[turn].control == BCPU)
         f_bchain_add(turn | 128, BACT_PANEL, dir, 0, 0, 0);

      // base / rumble / heals / wildcall
      if(bskillselect <= 9 || bskillselect == 11 || (bskillselect >= 20 && bskillselect <= 29) || bskillselect == 17) {
         f_bchain_add(turn, BACT_CAST, dir, 0, 0, 0);
      }
                // specials: sacrifice, whirl, inversion
      if(bskillselect == 10 || bskillselect == 18 || bskillselect == 19 || bskillselect == 12) {
         f_bchain_add(turn | 128, BACT_PANEL, dir, 0, 0, 0);
      }
                // items
      if(bskillselect >= 50) {
         // if HealAll, then throw at yourself
         if(bskillselect == 52 || bskillselect == 56 || bskillselect == 60)
            b_target = turn;
         else
            b_target = bch[turn].target;

         f_bchain_add(b_target, BACT_THROW, dir, bch[turn].rx, bch[turn].ry, bskillselect - 50);
      }

      // Engulf / Jolt / Tidal
      if(bskillselect < 3) {
         bearly = 1;
         switch(bskillselect) {
            case 0:  b_attrib = BATR_FIRE;  type = BACT_FIRE;  break;
            case 1: b_attrib = BATR_WIND;  type = BACT_JOLT;  break;
            case 2: b_attrib = BATR_WATER; type = BACT_TIDAL; break;
         }


         //SpellDamage*(Attacker(Wis+Mana) / Defender(Wis+Mana))

         //current--> Spell Damage = 20 + AttackerWis*1.5 - DefenderWis

         magic_damage(turn, b_target, 10, 28); // 1.75x

         // commenting out. dunno why this was here
         //if(bch[turn].type == ROLAND)
         //       b_damage = (b_damage * 3) >> 1;

         bch[b_target].damage += b_damage;

         /*b_target = bch[turn].target;
         v  = bch[turn].st.wis+1     + bch[turn].st.man+1;
         v2 = bch[b_target].st.wis+1 + bch[b_target].st.man+1;
         b_damage = (WORD)20 + (WORD)v + ((WORD)v/2) - (WORD)v2;

         f_bdamagecap();

         // mod damage
         bcm_damage = b_damage;
         bcm_guy = turn;
         bcm_target = b_target;
         f_bcalcmod();
         b_damage = bcm_damage;

         f_bdamageflux();
         f_bdamagecap();
         bch[b_target].damage += b_damage;*/

         px = bch[b_target].rx;
         py = bch[b_target].ry;
         dir = finddir(px, py, bch[turn].rx, bch[turn].ry);
         bch[turn].dir = dir;

         f_bchain_add(b_target, type, dir, bch[turn].rx, bch[turn].ry, 0);
      }
      // Blaze / Storm / Tsunam
      else if(bskillselect < 6) {
         bearly = 1;
         switch(bskillselect) {
            case 3:  b_attrib = BATR_FIRE;  type = BACT_FIRE;  f_bcoloron(COLOR_RED);    break;
            case 4: b_attrib = BATR_WIND;  type = BACT_JOLT;  f_bcoloron(COLOR_YELLOW); break;
            case 5: b_attrib = BATR_WATER; type = BACT_TIDAL; f_bcoloron(COLOR_BLUE);   break;
         }

         /*numguys = 0;
         for(n = 0; n != bnumdudes; ++n) {
            randpick[n] = 0;
            if(bch[n].active && bch[n].control != bch[turn].control)
               ++numguys;
         }
         n2 = (numguys+1) / 2;
         for(n = 0; n != n2; ++n) {
            k = (UBYTE)randfunc() % bnumdudes;
            while(!bch[k].active || bch[k].control == bch[turn].control || randpick[k]) {
               ++k;
               k %= bnumdudes;
            }
            randpick[k] = 1;

            magic_damage(turn, k);

            bch[k].damage += b_damage;
            f_bchain_add(k, type, dir, bch[k].rx - 100, bch[k].ry - 100, 0);
         }*/

         for(n = 0; n != bnumdudes; ++n) {
            k = n;
            if(bch[k].active && bch[k].control != bch[turn].control && bch[k].st.hp > 0) {
               magic_damage(turn, k, 15, 32); // 2x
               bch[k].damage += b_damage;
               f_bchain_add(k, type, dir, bch[k].rx - 100, bch[k].ry - 100, 0);
            }
         }
      }
      // Rumble
      else if(bskillselect == 6) {
         bearly = 1;

         for(k = 0; k != bnumdudes; ++k) {
            if(!bch[k].active)
               continue;

            if(bch[k].control != bch[turn].control) {
               magic_damage(turn, k, 20, 24); // 1.5x
               bch[k].damage += b_damage;
               f_bchain_add(k, BACT_QUAKE, dir, 0, 0, 0);
            }
         }
      }

      // Embrace
      else if(bskillselect == 7) {
         bearly = 1;
         b_attrib = BATR_HEAL;
         b_target = bch[turn].target;

         //Cure   = (1+(Wis/FixedNumber))*(Percentage To Heal*MaxHPTarget)+Wis

                        b_damage = 0;
                        if(bch[b_target].st.hp > 0) {
                                w = bch[turn].st.wis+1 + bch[turn].st.man+1;
                                w += w >> 1;
                                b_damage = w - 10;

                                f_bdamagecap();
                                f_bdamageflux();
                                f_bdamagecap();
                                b_damage *= -1;
                        }

         bch[b_target].damage += b_damage;

         f_bchain_add(bch[turn].target, BACT_CURE, dir, 0, 0, 0);
      }
      // Hope
      else if(bskillselect == 8) {
         bearly = 1;
         b_attrib = BATR_HEAL;

         w = bch[turn].st.wis+1 + bch[turn].st.man+1;
         w = w * 2 / 3;
         b_damage = w + 15;
         f_bdamagecap();

         numguys = 0;
         for(n = 0; n != bnumdudes; ++n) {
            randpick[n] = 0;
            if(bch[n].active && bch[n].control == bch[turn].control)
               ++numguys;
         }
         n2 = numguys - 1;
         if(n2 == 0)
            n2 = 1;
         for(n = 0; n != n2; ++n) {
            k = (UBYTE)randfunc() % bnumdudes;
            while(!bch[k].active || bch[k].control != bch[turn].control || randpick[k]) {
               ++k;
               k %= bnumdudes;
            }
            randpick[k] = 1;

            bch[k].damage = 0;
            if(bch[k].st.hp > 0)
               bch[k].damage -= b_damage;
            f_bchain_add(k, BACT_CURE, dir, bch[k].rx - 100, bch[k].ry - 100, 0);
         }
         f_bcoloron(COLOR_WHITE);
      }
      // Soulslurp
      else if(bskillselect == 9) {
         b_target = bch[turn].target;
         b_attrib = BATR_NONE;

         magic_damage(turn, b_target, 10, 28); // 1.75x

         // if Victor, do 2x
         if(bch[turn].type == VICTOR || bch[turn].type == VICTOR_WOLF)
            b_damage <<= 1;

         if(b_damage > bch[b_target].st.hp)
            b_damage = bch[b_target].st.hp;

         bch[b_target].damage += b_damage;
         bch[turn].damage -= b_damage;

         px = bch[b_target].rx;
         py = bch[b_target].ry;
         dir = finddir(px, py, bch[turn].rx, bch[turn].ry);
         bch[turn].dir = dir;

         f_bchain_add(b_target, BACT_SLURP, dir, bch[turn].rx, bch[turn].ry, turn);
         f_bchain_add(b_target, BACT_SLURP, dir, bch[turn].rx, bch[turn].ry, turn | 128);
         f_bchain_add(turn, BACT_SPARKLE, dir, 0, 0, 0);
      }
      // Sacrifice
      else if(bskillselect == 10) {
         b_damage = bch[turn].st.hp / 2;
         b_attrib = BATR_HEAL;

         // find number of other alive team members
         v = 0;
         for(k = 0; k != bnumdudes; ++k) {
            if(!bch[k].active || k == turn)
               continue;
            if(bch[k].control == bch[turn].control)
               ++v;
         }

         bch[turn].damage = b_damage;
         f_bchain_add(turn, BACT_SCRATCH, BDIR_UP, 0, 0, 0);

         if(v > 0) {
            w = bch[turn].st.hp / v;

            for(k = 0; k != bnumdudes; ++k) {
               if(!bch[k].active || k == turn)
                  continue;

               if(bch[k].control == bch[turn].control) {
                  bch[k].damage = 0;
                  if(bch[k].st.hp > 0)
                     bch[k].damage -= w;
                  f_bchain_add(k, BACT_CURE, dir, 0, 0, 0);
               }
            }
         }
      }
      // Wake (or Diamond) (or Life item)
      else if(bskillselect == 11 || bskillselect == 33 || bskillselect == 53 || bskillselect == 57 || bskillselect == 61) {
         if(bskillselect == 11 || bskillselect == 33)
            v = 2;
         else
            v = (bskillselect - 53) >> 2;

         // v = power (0,1,2,3)
         // todo: affect hp based on v


         b_attrib = BATR_HEAL;
         b_target = bch[turn].target;
         b_damage = 0;
         if(bch[b_target].st.hp == 0) {
                                if(v == 0)
                                        w = 1;
                                else if(v == 1)
                                        w = bch[b_target].st.hpmax / 10;
                                else
                                        w = bch[b_target].st.hpmax / 4;

            b_damage -= w;
            bch[b_target].points = 0;
            bch[b_target].recover = getcount(bch[b_target].st.spd, 0) / 2;
         }
         bch[b_target].damage += b_damage;

         f_bchain_add(bch[turn].target, BACT_WAKE, dir, 0, 0, 0);
         f_bchain_add(bch[turn].target, BACT_SPARKLE, dir, 0, 0, 0);
      }
      // Inversion
      else if(bskillselect == 12) {
                        bch[turn].damage = 999;
                        f_bchain_add(turn, BACT_SCRATCH, BDIR_UP, 0, 0, 0);

                        for(k = 0; k != bnumdudes; ++k) {
                                if(!bch[k].active || turn == k || bch[k].control != bch[turn].control)
                                        continue;

                                w = bch[k].st.hpmax >> 1;
                                if(bch[k].st.hp > 0) {
                                        b_damage = 0;
                                        if(bch[k].st.hp < w)
                                                b_damage -= (w - bch[k].st.hp);

                                        bch[k].damage += b_damage;
                                        f_bchain_add(k, BACT_CURE, dir, 0, 0, 0);
                                }
                                else {
                                        b_damage = -w;
                                        bch[k].damage += b_damage;

                                        f_bchain_add(k, BACT_WAKE, dir, 0, 0, 0);
                                        f_bchain_add(k, BACT_SPARKLE, dir, 0, 0, 0);

                                        bch[k].points = 0;
                                        bch[k].recover = getcount(bch[k].st.spd, 0) / 2;
                                }
                        }

                        f_bcoloron(COLOR_WHITE);
      }
      // Justice
      else if(bskillselect == 13) {
                        f_bchain_add(turn | 128, BACT_PANEL, dir, 0, 0, 0);
      }
      // Leap
      else if(bskillselect == 14) {
         b_target = bch[turn].target;

         // damage
         b_damage = f_bwepdamage(turn, b_target);
         bch[b_target].damage += b_damage;

         px = bch[b_target].rx;
         py = bch[b_target].ry;
         dir = finddir(px, py, bch[turn].rx, bch[turn].ry);
         bch[turn].dir = dir;
         d = f_get_4waydir(dir);

         if(d == RLEFT)
            px += 12;
         if(d == RRIGHT)
            px -= 12;
         if(d == RDOWN)
            py -= 12;
         if(d == RUP)
            py += 12;

         b_attrib = BATR_HAND;
         f_bchain_add(turn | 128, BACT_JUMP, dir, px, py, 2);

         // enemy or no weapon?  then scratch
         v = 1;
         if(bch[turn].control == BCPU) {
            v = 0;
         }
         else {
            st = get_party(turn);
            if(!st->e[0])
               v = 0;
         }


         // look for a free space to land, put the coords in w,w2
         k = b_target;
         n2 = 0;
         for(n = 0; n != 6; ++n) {
            // get the space
            f_get_dxdy(bch[k].x, bch[k].y, n);

            // if the caster is there, then go there
            if(dx == bch[turn].x && dy == bch[turn].y) {
               n2 = n + 1;
               break;
            }

            // is it a free space?
            if(freespace(dx, dy))
               n2 = n + 1;
         }

         if(!n2) {
            // ack!  no room.  we jump back then
            w = bch[turn].rx;
            w2 = bch[turn].ry;
         }
         else {
            // get direction, and set dx/dy
            n = n2 - 1;
            f_get_dxdy(bch[k].x, bch[k].y, n);

            // found a new place, set the guy there
            bch[turn].x = dx;
            bch[turn].y = dy;
            //dir = bflipdir(n2-1);
            b6_bgetrealv(&bch[turn], &w, &w2);
         }

         if(v)
            f_bchain_add(turn, BACT_ATTACK, dir, bch[b_target].rx, bch[b_target].ry, 0);
         else
            f_bchain_add(b_target, BACT_SCRATCH, dir, 0, 0, 0);

         // no room?  jump back high (bit 7 for sound)
         if(!n2) {
            f_bchain_add(turn | 128, BACT_JUMP, dir, w, w2, 2);
         }
         else {
            f_bchain_add(turn, BACT_JUMP, dir, w, w2, 1);
         }
      }
      // Longshot
      else if(bskillselect == 15) {
         bearly = 1;
         b_target = bch[turn].target;

         // damage
         b_damage = f_bwepdamage(turn, b_target);
         bch[b_target].damage += b_damage;

         px = bch[b_target].rx;
         py = bch[b_target].ry;
         dir = finddir(px, py, bch[turn].rx, bch[turn].ry);
         bch[turn].dir = dir;

         b_attrib = BATR_HAND;
         if(bch[turn].control == BPLAYER)
            f_bchain_add(turn, BACT_PULLBOW, dir, 0, 0, 0);

         f_bchain_add(b_target, BACT_ARROW, dir, bch[turn].rx, bch[turn].ry, 0);
      }
      // Spray
      else if(bskillselect == 16) {
         //if(bch[turn].control == BPLAYER)
         // f_bchain_add(turn, BACT_PULLBOW, bch[turn].dir, 0, 0, 0);

         // initialize and count number of possible targets
         numguys = 0;
         for(n = 0; n != bnumdudes; ++n) {
            randpick[n] = 0;
            if(bch[n].active && bch[n].control != bch[turn].control && bch[n].st.hp)
               ++numguys;
         }
         // hit at most, 5 guys
         if(numguys > 5)
            n2 = 5;
         else
            n2 = numguys;

         // loop through and smack
         for(n = 0; n != n2; ++n) {
            k = (UBYTE)randfunc() % bnumdudes;
            while(!bch[k].active || bch[k].control == bch[turn].control || !bch[k].st.hp || randpick[k]) {
               ++k;
               k %= bnumdudes;
            }
            randpick[k] = 1;

            // damage
            b_damage = f_bwepdamage(turn, k) / 2;
            f_bdamagecap();
            bch[k].damage += b_damage;

            dir = finddir(bch[k].rx, bch[k].ry, bch[turn].rx, bch[turn].ry);
            bch[turn].dir = dir;

            if(bch[turn].control == BPLAYER)
               f_bchain_add(turn, BACT_PULLBOW, dir, 0, 0, 0);
            f_bchain_add(k, BACT_ARROW, dir, bch[turn].rx, bch[turn].ry, 0);
         }
      }
      // Wildcall
      else if(bskillselect == 17) {
         f_bchain_add(turn, BACT_EFFECT, bch[turn].dir, BAIL_WOLF, 0, 0);
      }
      // Slash
      else if(bskillselect == 18) {
         b_target = bch[turn].target;

         // get target x,y and face direction
         px = bch[b_target].rx;
         py = bch[b_target].ry;
         dir = finddir(px, py, bch[turn].rx, bch[turn].ry);
         bch[turn].dir = dir;

         // 7th bit indicates "swing" only
         f_bchain_add(turn | 128, BACT_ATTACK, dir, 0, 0, 0);

         // do the slash effect
         f_getdistxy(px - bch[turn].rx, py - bch[turn].ry, 127);
         px += bch[turn].rx;
         py += bch[turn].ry;
         f_bchain_add(turn, BACT_SLASH, dir, px, py, 0);

         // damage each enemy
         for(n = 0; n < bnumdudes; ++n)
                 randpick[n] = 0;
         f_bpro_set(0, bch[turn].rx, bch[turn].ry, px, py, 56);
         v = 96;
         w3 = 48;
         for(n = 0; n < 56; ++n, --v) {
            f_bpro_next(0);
            px = bpro[0].rx;
            py = bpro[0].ry;

            // see if an enemy is in the way
            for(n2 = 0; n2 < bnumdudes; ++n2) {
               if(!bch[n2].active)
                  continue;
               if(randpick[n2])
                  continue;
               if(bch[n2].control == bch[turn].control)
                  continue;
               if(bch[n2].st.hp == 0)
                  continue;

               w = bch[n2].rx;
               w2 = bch[n2].ry;
               k = bch[n2].boss ? 16+8+2: 8+8+2;
               if(px >= w - k && px < w + k && py >= w2 - k && py < w2 + k)
                  break;
            }
            // found one?
            if(n2 < bnumdudes) {
               randpick[n2] = 1;
               //b_damage = f_bwepdamage(turn, n2) * (WORD)v / w3;
               b_damage = (f_bwepdamage(turn, n2) * 3) >> 1;
               f_bdamagecap();
               bch[n2].damage += b_damage;
               f_bchain_add(n2, BACT_DAMAGE, bch[n2].dir, 0, 0, 0);
               w3 += v;
               w3 >>= 1;
            }
         }
      }
      // Whirlwind
      else if(bskillselect == 19) {
         f_bchain_add(turn, BACT_WHIRL, dir, 0, 0, 0);

         // check nearby squares
         for(n = 0; n < 6; ++n) {
            f_get_dxdy(bch[turn].x, bch[turn].y, n);
            n2 = isguy(dx, dy);
            if(!n2)
               continue;
            --n2;

            // don't hit dead, same team, etc.
            if(!bch[n2].active || bch[n2].st.hp == 0 || bch[n2].control == bch[turn].control)
               continue;

            // damage!
            b_damage = f_bwepdamage(turn, n2);
            bch[n2].damage += b_damage;

            f_bchain_add(n2, BACT_DAMAGE, bch[n2].dir, 0, 0, 0);
         }
      }

      // Spawn
      else if(bskillselect == 20) {
         // see if slot 4-6 is inactive
         for(n = 0; n != 3; ++n) {
            n2 = bchnum + n;
            if(!bch[n2].active) {
               f_bchain_add(n2, BACT_SPAWN, dir, 0, 0, 0);
               break;
            }
         }
         if(n == 3) {
            // cancel move
            f_bchain_stop();
            done = 1;
            return;
         }
         k = n2;

         // original square still available?
         if(!freespace(bch[k].x, bch[k].y)) {
            // if not, look for one around there
            for(n = 0; n != 6; ++n) {
               f_get_dxdy(bch[k].x, bch[k].y, n);
               if(freespace(dx, dy))
                  break;
            }
            if(n == 6) {
               // ack!  no room.  we bail
               f_bchain_stop();
               done = 1;
               return;
            }

            // found a new place, set the guy there
            bch[k].x = dx;
            bch[k].y = dy;
            b6_bgetreal(&bch[k]);
         }
      }

      // Charm
      else if(bskillselect == 21) {
         f_bchain_add(bch[turn].target, BACT_CHARM, dir, bch[turn].rx, bch[turn].ry, 0);
      }

      // Emerald
      else if(bskillselect == 32) {
         for(k = 0; k != bnumdudes; ++k) {
            if(!bch[k].active)
               continue;

            if(bch[k].control == BPLAYER) {
               //bch[k].damage += 16 * k;
               //bch[k].damage = 12;
               b_damage = 0;
               if(bch[k].st.hp > 0)
                  b_damage -= 30;
               bch[k].damage += b_damage;
               f_bchain_add(k, BACT_CURE, dir, 0, 0, 0);
            }
         }
      }

      // HPUP
      else if(bskillselect == 50 || bskillselect == 54 || bskillselect == 58) {
         v = (bskillselect - 50) >> 2;

         b_target = bch[turn].target;
         b_damage = 0;
         if(bch[b_target].st.hp > 0)
                                b_damage -= b6_hplo[v];
         bch[b_target].damage += b_damage;

         f_bchain_add(bch[turn].target, BACT_SPARKLE, dir, 0, 0, 0);
      }

      // SPUP
      else if(bskillselect == 51 || bskillselect == 55 || bskillselect == 59) {
         v = (bskillselect - 51) >> 2;

         b_target = bch[turn].target;
         b_damage = 0;
         if(bch[b_target].st.hp > 0)
            b_damage -= b6_splo[v];
         bch[b_target].damage += b_damage;
         bch[b_target].spflag = 1;

         f_bchain_add(bch[turn].target, BACT_SPARKLE, dir, 0, 0, 0);
      }

      // ALUP
      else if(bskillselect == 52 || bskillselect == 56 || bskillselect == 60) {
         v = (bskillselect - 52) >> 2;

         for(n = 0; n < bchnum; ++n) {
            b_damage = 0;
            if(bch[n].st.hp > 0)
               b_damage -= b6_hpall[v];
            bch[n].damage += b_damage;

            f_bchain_add(n, BACT_CURE, dir, 0, 0, 0);
         }
      }
      else {
         disable_interrupts();
         *((UBYTE *)0xCFFF) = 97;
         while(1);
      }

      done = !f_bchain_do();
}

