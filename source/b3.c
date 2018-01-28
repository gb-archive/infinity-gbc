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

// battle page 3
#include"defs.h"
#include"vars.h"

extern UBYTE pal_bak[];
extern UBYTE pal_cur[];
extern UWORD pal[];
extern UWORD window_ptr;

struct BOSSDATA bossdata[] = {
   { { 0x0000, 0x0057, 0x2529, 0x0000,   0x0000, 0x0d9e, 0x56d1, 0x0000 }, BOSS_COMPACT }, // big wolf
   { { 0x0000, 0x0000, 0x0000, 0x0000,   0x0000, 0x0000, 0x0000, 0x0000 }, BOSS_COMPACT }, // evil tree
   { { 0x0000, 0x023f, 0x05a6, 0x0000,   0x0000, 0x0f9f, 0x132c, 0x0000 }, 0 },            // exanima
   { { 0x0000, 0x0014, 0x023F, 0x0000,   0x0000, 0x0014, 0x023F, 0x0000 }, BOSS_COMPACT }, // golem omega
   { { 0x0000, 0x0014, 0x023F, 0x0000,   0x0000, 0x0014, 0x023F, 0x0000 }, BOSS_COMPACT }, // golem omega 2
   { { 0x0000, 0x0014, 0x023F, 0x0000,   0x0000, 0x0014, 0x023F, 0x0000 }, BOSS_COMPACT }, // golem omega 3
};

extern UBYTE rotpal;
void gemcheck(UBYTE target);
void gem_animate(UBYTE guy);
void bpause();
extern UBYTE pal_bak[];
extern UBYTE pal_cur[];

void bcoloron(UBYTE color)
{
   UBYTE n;

   pfs_targetspr = 0;
   if(!bcoloractive) {
      bcoloractive = 1;
      fadeto(color, 32);
      //setwindow(0, 144);
      for(n = 0; n != 20; ++n) {
         w4vbl_done();
         fade_step();
      }
      //setwindow(0, 112);
   }
}

void bcoloroff()
{
   UBYTE n;

   if(bcoloractive) {
      //setwindow(0, 144);
      for(n = 0; n != 20; ++n) {
         w4vbl_done();
         fade_backstep();
      }
      bcoloractive = 0;
      pfs_targetspr = 252;
      //setwindow(0, 112);
   }
}

UBYTE bchoose_bad;

void bchoose_prev()
{
   UBYTE n;

   bchoose_bad = 0;
   for(n = 0; n != bnumdudes; ++n) {
      if(bchoosetarget == 0)
         bchoosetarget = bnumdudes - 1;
      else
         --bchoosetarget;

      if(!bch[bchoosetarget].active)
         continue;
      if(bch[bchoosetarget].st.hp == 0 && bch[bchoosetarget].control == BCPU)
         continue;

      if(bch[bchoosetarget].control == bchoosetype)
         break;
   }
   if(n != bnumdudes)
      return;

   bchoose_bad = 1;
}

void bchoose_next()
{
   UBYTE n;

   bchoose_bad = 0;
   for(n = 0; n != bnumdudes; ++n) {
      if(bchoosetarget == bnumdudes - 1)
         bchoosetarget = 0;
      else
         ++bchoosetarget;

      if(!bch[bchoosetarget].active)
         continue;
      if(bch[bchoosetarget].st.hp == 0 && bch[bchoosetarget].control == BCPU)
         continue;

      if(bch[bchoosetarget].control == bchoosetype)
         break;
   }
   if(n != bnumdudes)
      return;

   bchoose_bad = 1;
}

UBYTE setbchoose(UBYTE icon, UBYTE guy)
{
   UBYTE n;
   bchooseicon = icon;

   // set mode
   bchoose_bad = 0;
   if(skl_type == 0) {
      bchoosetarget = bnumdudes - 1;
      bchoosetype = BCPU;
      bchoose_next();
      if(bchoose_bad)
         return 0;

      binputmode = 2;
   }
   else if(skl_type == 1) {
      bchoosetarget = guy;
      bchoosetype = BPLAYER;
      binputmode = 2;
   }
   else {
                bchoosetarget = guy;
      binputmode = 3;
   }

   // bchoose menu
   //bmenu = 5;
   //bmenusquish = 0;
   bicon_expand(bchooseicon+1);

   // set the text
   //bloadboxes();
   point_str(btextdesc, 10);
   pstr(14, 1, 5, (char *)skl_name);
   pstr(14, 2, 2, deci_16((UWORD)skl_cost, 3));
   pstr(16, 2, 1, "/");
   if(skl_gem) {
      // no error check-- you can't get here without being in party and having a gem
      st = get_party(bch[guy].rep);
      n = st->e[3] - 1;
      n = gems_charge[n];
      pstr(17, 2, 2, deci_16((UWORD)n, 3));
   }
   else if(skl_item) {
                // item type, not strength
      bitem_item = (skl_item - 1) & 0x03;
      pstr(17, 2, 2, deci_16(f_bitem_have(), 3));
   }
   else
      pstr(17, 2, 2, deci_16(bch[guy].st.sp, 3));

   return 1;
}

BYTE get_player_input(UBYTE guy)
{
   BYTE even;
   BYTE req;
// UBYTE n, n2;
   UBYTE u, u2;
   char *p;
   UBYTE cost, type;
   UBYTE ok;

   req = -1;

   // update menu
   /*if(bmenu) {
      bspr_8set(8,  16 + 16 - bmenucount, 24 - bmenusquish, 64, bboxpal[0] | B_OVERLAY);
      bspr_8set(9,  24 + 16 - bmenucount, 24 - bmenusquish, 65, bboxpal[0] | B_OVERLAY);

      bspr_8set(10, 16 - bmenucount - bmenusquish, 24 + 16, 66, bboxpal[1] | B_OVERLAY);
      bspr_8set(11, 24 - bmenucount - bmenusquish, 24 + 16, 67, bboxpal[1] | B_OVERLAY);

      bspr_8set(12, 16 + 32 - bmenucount + bmenusquish, 24 + 16, 68, bboxpal[2] | B_OVERLAY);
      bspr_8set(13, 24 + 32 - bmenucount + bmenusquish, 24 + 16, 69, bboxpal[2] | B_OVERLAY);

      bspr_8set(14, 16 + 16 - bmenucount, 24 + 32 + bmenusquish, 70, bboxpal[3] | B_OVERLAY);
      bspr_8set(15, 24 + 16 - bmenucount, 24 + 32 + bmenusquish, 71, bboxpal[3] | B_OVERLAY);

      // slide in
      if(bmenu == 1) {
         if(bmenucount > 0)
            bmenucount -= 16;
      }
      // slide out
      else if(bmenu == 2) {
         if(bmenucount < 64)
            bmenucount += 16;
         else
            bmenu = 0;
      }
      // expand
      else if(bmenu == 3) {
         if(bmenusquish < 16)
            bmenusquish += 4;
         else {
            bloadboxes();
            bmenu = 4;
         }
      }
      // squish
      else if(bmenu == 4) {
         if(bmenusquish > 0)
            bmenusquish -= 4;
         else
            bmenu = 1;
      }
      // explode
      else if(bmenu == 5) {
         if(bmenusquish < 16)
            bmenusquish += 4;
         else
            bmenu = 0;
      }
      // restore
      else if(bmenu == 6) {
         if(bmenusquish > 0)
            bmenusquish -= 4;
         else
            bmenu = 1;
      }

      // menu toast?  poof!
      if(bmenu == 0)
         f_wipe_menu();
   }*/

   // update sliding gem
   if(bch[guy].gem) {
      bspr_8set(16, 140 + bgemslide,  88, 72, bboxpal[4] | B_OVERLAY);
      bspr_8set(17, 148 + bgemslide,  88, 73, bboxpal[4] | B_OVERLAY);
      st = get_party(bch[guy].rep);
      n = gems_charge[st->e[3]-1];
      bgemcharge1 = (n / 10) + 1;
      bgemcharge2 = (n % 10) + 1;
      bspr_8set(18, 140 + bgemslide, 104, bgemcharge1 + 15, 6 | B_OVERLAY);
      bspr_8set(19, 148 + bgemslide, 104, bgemcharge2 + 15, 6 | B_OVERLAY);

      if(bgemslide != 0)
         bgemslide -= 8;
   }

   if(binputmode == 0) {
      bselect = -1;
      if(key[UP] && !key[LEFT] && !key[RIGHT])
         bselect = BDIR_UP;
      if(key[UP] && key[RIGHT])
         bselect = BDIR_UPRIGHT;
      if(key[DOWN] && key[RIGHT])
         bselect = BDIR_DOWNRIGHT;
      if(key[DOWN] && !key[LEFT] && !key[RIGHT])
         bselect = BDIR_DOWN;
      if(key[DOWN] && key[LEFT])
         bselect = BDIR_DOWNLEFT;
      if(key[UP] && key[LEFT])
         bselect = BDIR_UPLEFT;

      if(bselect != -1) {
         f_get_dxdy(bch[guy].x, bch[guy].y, bselect);

         icon = 0;
         for(n = 0; n != bnumdudes; ++n) {
            if(!bch[n].active)
               continue;

            if(bch[n].x == dx && bch[n].y == dy) {
               if(bch[n].control == BCPU) {
                  icon = 2;
                  bch[guy].target = n;
               }
               if(bch[n].control == BPLAYER && bch[n].st.hp > 0)
                  icon = 1;

               break;
            }
         }

         if(!(!bmap[dy][dx] && dy >= 0 && dy < 9 && dx >= 0 && dx < 11) && icon != 2)
            icon = 1;

         even = ~dx & 1;

         px = (WORD)dx * 24;
         py = (WORD)dy * 24;
         py += (WORD)even * 12;

         px += 8;
         py += 8;


         // display icon
         bspr_8set(4, px  -4, py-4 + icon_wiggle, icon * 2 + 4, B_WDPAL);
         bspr_8set(5, px+8-4, py-4 + icon_wiggle, icon * 2 + 5, B_WDPAL);
      }
      else {
         // delete icon
         bspr_8set(4, 0, 0, -1, 0);
         bspr_8set(5, 0, 0, -1, 0);
      }

      if(key[ENTER]) {
         if(bselect == -1) {
            //bmenu = 1;
            //bmenucount = 64;
            binputmode = 1;

            bicon_reset();

            bbox[0] = 0;
            bbox[1] = 1;
            bbox[2] = 2;
            bbox[3] = 3;
            bicon_slidein();

            bboxmenu = 0;
         }
         else {
            if(icon == 0)
               req = bselect;
            if(icon == 2)
               req = bselect + 6;
         }
      }
      else if(key[CANCEL]) {
                        if(options[0]) {
                                for(n = 0, n2 = guy + 1; n != 3; ++n, ++n2) {
                                        n2 %= 4;
                                        if(!bch[n2].active)
                                                continue;
                                        if(n2 == guy)
                                                continue;

                                        if(bch[n2].control == BPLAYER) {
                                                if(bch[n2].points >= 1 && bch[n2].st.hp > 0 && !f_binqueue(n2)) {
                                                        play_sfx(SFX_MENUCURSOR);
                                                        f_player_cancel();
                                                        f_player_control(n2);
                                                        break;
                                                }
                                        }
                                }
                        }

                        // strategy mode?  skip to the next "turn"
                        else {
                                // find guy with the least ticks
                                u = 255;
                                for(n = 0; n < bnumdudes; ++n) {
                                        if(!bch[n].active)
                                                continue;
                                        if(n == guy || bch[n].st.hp == 0)
                                                continue;

                                        if(bch[n].recover < u && bch[n].recover != 0)
                                                u = bch[n].recover;
                                }

                                bch[guy].points = 0;
                                bch[guy].recover = u + 1;

                                play_sfx(SFX_MENUCURSOR);
                                f_player_cancel();
                        }
      }
   }
   else if(binputmode == 1) {
      // main menu
      if(bboxmenu == 0) {
         n = bboxmenu;

         // --> skills
         if(key[UP]) {
            for(n2 = 0; n2 != 4; ++n2) {
               if(bch[guy].skl[n2])
                  bbox[n2] = bch[guy].skl[n2] - 1 + 20;
               else
                  bbox[n2] = 4;
            }

            bboxmenu = 1;
         }

         // --> run!
         else if(key[DOWN]) {
            if(bflag_run) {
               play_sfx(SFX_MENUCURSOR);

               binputmode = 4;
               bicon_expand(4);

               point_str(btextdesc, 10);
               pstr(14, 1, 5, "Run?");
            }
            else
               play_sfx(SFX_MENUERROR);
         }

         // --> gem
         else if(key[LEFT]) {
            st = get_party(bch[guy].rep);
            u = st->e[3];
            if(u) {
               --u;
               u = gems_list[u] - 1;
               u2 = (UBYTE)item_info2(u + GEMS_INDEX)[5];
               cost = u2 & 63;
               type = u2 & 64 ? 1: 0;
               if(type) {
                  if(u == GEM_EMERALD || u == GEM_DIAMOND) {
                     bgemselect = st->e[3] - 1;
                     f_load_skill(u + 30);
                     if(setbchoose(1, guy)) {
                        play_sfx(SFX_MENUCURSOR);

                        bbox[1] = 40 + u;
                        bicon_refresh(1);
                        //bloadboxes();
                     }
                     else
                        play_sfx(SFX_MENUERROR);
                  }
                  else if(u == GEM_GARNET) {
                     bgemselect = st->e[3] - 1;
                     if(gems_charge[bgemselect] >= cost) {
                        play_sfx(SFX_MENUCURSOR);
                        gems_charge[bgemselect] -= cost;
                        req = 81;
                        goto input_end;
                     }
                     else
                        play_sfx(SFX_MENUERROR);
                  }
                  // activation type
                  else {
                     play_sfx(SFX_MENUCURSOR);
                     if(!bch[guy].gem) {
                        bch[guy].gem = 1;
                        bch[guy].gemcount = 0;
                        bgemslide = 32;
                     }
                     else {
                        bch[guy].gem = 0;
                        f_wipe_gem();
                     }
                     //bmenu = 2;
                     bicon_slideout();
                     binputmode = 0;
                  }
               }
            }
            else
               play_sfx(SFX_MENUERROR); // no gem?
         }

         // --> items
         else if(key[RIGHT]) {
            for(u = 0; u != 4; ++u) {
               bitem_item = u;
               bbox[u] = f_bitem_have() ? f_bitem_type() + 6: 4;
            }
            //bbox[1] = 4;
            //bbox[2] = 4;
            //bbox[3] = 4;
            bboxmenu = 2;
         }

         if(bboxmenu != n) {
            play_sfx(SFX_MENUCURSOR);
            bicon_expand(0);
            //bmenu = 3;
         }
      }
      else if(bboxmenu == 1) {
         n = 0;
         if(key[UP])
            n = 1;
         else if(key[DOWN])
            n = 4;
         else if(key[LEFT])
            n = 2;
         else if(key[RIGHT])
            n = 3;

         // no skill? cancel
         if(n) {
            if(!bch[guy].skl[n-1]) {
               play_sfx(SFX_MENUERROR);
               n = 0;
            }
         }

         if(n) {
            --n;
            //bchooseicon = n - 1;
            f_load_skill(bch[guy].skl[n]-1);
            if(setbchoose(n, guy))
               play_sfx(SFX_MENUCURSOR);
            else
               play_sfx(SFX_MENUERROR);
         }
      }
      else if(bboxmenu == 2) {
         n = 0;
         if(key[UP])
            n = 1;
         else if(key[DOWN])
            n = 4;
         else if(key[LEFT])
            n = 2;
         else if(key[RIGHT])
            n = 3;

         // no item? cancel
         if(n) {
            bitem_item = n - 1;
            if(!f_bitem_have()) {
               play_sfx(SFX_MENUERROR);
               n = 0;
            }
         }

         if(n) {
            --n;
            bitem_item = n;
            f_load_skill(f_bitem_type() + 50);
            if(setbchoose(n, guy))
               play_sfx(SFX_MENUCURSOR);
            else
               play_sfx(SFX_MENUERROR);

         }
      }

      if(key[CANCEL]) {// && bmenusquish == 0) {
         if(bboxmenu == 0) {
            //bmenu = 2;
            bicon_slideout();

            binputmode = 0;
         }
         else {
            bbox[0] = 0;
            bbox[1] = 1;
            bbox[2] = 2;
            bbox[3] = 3;
            bboxmenu = 0;
            //bmenu = 3;
            bicon_expand(0);
         }
      }
   }
   else if(binputmode == 2 || binputmode == 3) {
      //n = bchooseicon << 1;
      //bspr_8set(n + 8, 140, 24, 64 + n, bboxpal[bchooseicon] | B_OVERLAY);
      //bspr_8set(n + 9, 148, 24, 65 + n, bboxpal[bchooseicon] | B_OVERLAY);

      if(binputmode == 2) {
         if(key[UP] || key[LEFT]) {
            bchoose_next();
         }
         if(key[DOWN] || key[RIGHT]) {
            bchoose_prev();
         }
      }
      else {
         //rotpal &= 3;
         winpal(14, 1, 5, 1, rotpal);
         //++rotpal;
      }

      //if(skl_cost == 5 && skl_gem == 3 && bgemselect == 2)
      // while(1);
      //disable_interrupts();
      //while(1);

      if(key[CANCEL] || !bch[bchoosetarget].active || (bch[bchoosetarget].st.hp == 0 && bch[bchoosetarget].control == BCPU)) {
         f_wipe_text();
         //bmenu = 6;
         //bmenusquish = 16;
         binputmode = 1;
         if(bboxmenu == 0) {
            bbox[0] = 0;
            bbox[1] = 1;
            bbox[2] = 2;
            bbox[3] = 3;
            //bloadboxes();
         }
         bicon_shrink();
      }
      else if(key[ENTER]) {
         // Skill or GEM or Item
         if(bboxmenu == 0 || bboxmenu == 1 || bboxmenu == 2) {
            // --> gem?
            if(bboxmenu == 0) {
               // enough GP?
               if(gems_charge[bgemselect] >= skl_cost) {
                  play_sfx(SFX_MENUACCEPT);
                  f_wipe_text();

                  bch[guy].target = bchoosetarget;
                  req = skl_gem-1;
                  req += 46;  // req >= 46 == gem
               }
               else
                  play_sfx(SFX_MENUERROR);
            }
            // --> skill
            if(bboxmenu == 1) {
               ok = 1;
                                        n = bch[guy].skl[bchooseicon] - 1;

               if(n == 15 || n == 16) {
                  st = get_party(bch[guy].rep);
                  if(!st->e[0])
                     ok = 0;
               }
               // enough SP?
               if(bch[guy].st.sp >= skl_cost && ok) {
                  play_sfx(SFX_MENUACCEPT);
                  f_wipe_text();

                  bch[guy].target = bchoosetarget;
                  req = bch[guy].skl[bchooseicon] - 1;
                  req += 16;  // req >= 16 == skill
               }
               else
                  play_sfx(SFX_MENUERROR);
            }
            // --> item
            if(bboxmenu == 2) {
               play_sfx(SFX_MENUACCEPT);

                                        // remove the item.  select type (hence &'ing with 0x03), not strength
                                        bitem_item = (skl_item - 1) & 0x03;
                                        f_bitem_del();

               f_wipe_text();
               bch[guy].target = bchoosetarget;
               req = skl_item-1;
               req += 66;  // req >= 66 == item
            }
         }
      }
   }
   else if(binputmode == 4) {
      winpal(14, 1, 5, 1, rotpal);

      if(key[CANCEL]) {
         f_wipe_text();
         binputmode = 1;
         bicon_shrink();
      }
      else if(key[ENTER]) {
         play_sfx(SFX_MENUACCEPT);

         f_wipe_text();
         req = 80;
         goto input_end;
      }
   }

        if(key[MENU]) {
                play_sfx(SFX_MENUACCEPT);
                bpause();
        }

input_end:
   // lock keys
   if(binputmode) {
      for(n = 0; n != 4; ++n) {
         if(key[n])
            keylock[n] = 1;
      }
   }
   for(n = 4; n != 8; ++n) {
      if(key[n])
         keylock[n] = 1;
   }

   if(req != -1) {
      f_wipe_menu();
      f_wipe_gem();
      binputmode = 0;
   }

   return req;
}


// far call
void bmapsetup()
{
   UBYTE n, n2;
   WORD px, py;
   BYTE even;

   for(n2 = 0; n2 != 9; ++n2) {
      for(n = 0; n != 11; ++n) {
         bmap[n2][n] = 0;

         even = n % 2;
         even = even ? 0: 1;

         px = n;
         px *= 24;
         py = n2;
         py *= 24;
         py += (WORD)even * 12;

         if(owe_blocked(1, px >> 4, py >> 4)) {
            bmap[n2][n] = 1;
            continue;
         }

         px += 15;
         if(owe_blocked(1, px >> 4, py >> 4)) {
            bmap[n2][n] = 1;
            continue;
         }

         px -= 15;
         py += 15;
         if(owe_blocked(1, px >> 4, py >> 4)) {
            bmap[n2][n] = 1;
            continue;
         }

         px += 15;
         if(owe_blocked(1, px >> 4, py >> 4)) {
            bmap[n2][n] = 1;
            continue;
         }
      }
   }

   bmap[8][0] = 1;
   bmap[8][2] = 1;
   bmap[8][4] = 1;
   bmap[8][6] = 1;
   bmap[8][8] = 1;
   bmap[8][10] = 1;
}

void setbosspal(UBYTE x)
{
   if(x == 0) {
      setwinpal();
      pfs_targetspr = 0xFC;
      return;
   }

   // load the boss palette
   memcpy(pal + 60, bossdata[x-1].pal + (_gba ? 4:0), 8);
   pfs_targetspr = 0xFD;
}

void beadd(UBYTE type, UBYTE x, UBYTE y)
{
   UBYTE c;
   UBYTE n;

   for(c = 0; bch[c].active; ++c);

   load_enemy(type, (UBYTE *)&bch[c].st);

   bch[c].type = type + SPRBASE_ENEMY;
   bch[c].x = x;
   bch[c].y = y;
   bch[c].rz = 0;
   bch[c].action = BACT_NORM;
   bch[c].control = BCPU;
   bch[c].dir = BDIR_DOWN;
   bch[c].recover = 0;
   bch[c].points = 0;
   bch[c].damage = 0;
   bch[c].poof = 0;
   bch[c].active = 1;
   bch[c].gem = 0;
        bch[c].gemshow = 0;
   bch[c].spawnlord = 0;
   bch[c].slider = 0;
   bch[c].ail = 0;
   bch[c].spflag = 0;

   if(bch[c].type-SPRBASE_ENEMY < BOSS) {
      bch[c].boss = 0;
      bch[c].flags = 0;
   }
   else {
      n = bch[c].type-SPRBASE_ENEMY-BOSS;
      bch[c].boss = n + 1;

      // load the boss palette
      memcpy(pal + 60, bossdata[n].pal + (_gba ? 4:0), 8);
      pfs_targetspr = 0xFD;

      // and info
      bch[c].flags = bossdata[n].flags;
   }

   if(bch[c].type-SPRBASE_ENEMY == 51)
      bch[c].spawnlord = 1;
   if(bch[c].type-SPRBASE_ENEMY == 22)
      bch[c].slider = 1;

   ++bennum;
}

void bpadd(UBYTE type, UBYTE x, UBYTE y)
{
   UBYTE c, n, wepattr;
   struct BDUDE *b;

   st = get_party(type);

   // find the next open slot
   for(c = 0; bch[c].active; ++c) {}

   b = &bch[c];

   b->rep = type;
   b->type = st->type;
   b->x = x;
   b->y = y;
   b->rz = 0;
   b->action = BACT_NORM;
   b->control = BPLAYER;
   b->dir = BDIR_DOWN;
   b->recover = 0;
   b->points = 0;
   b->damage = 0;
   b->poof = 0;
   b->active = 1;
   b->gem = 0;
   b->gemshow = 0;
   b->spawnlord = 0;
   b->boss = 0;
   b->slider = 0;
   b->ail = 0;
   b->spflag = 0;

   b->st.level = st->level;
   b->st.hp = st->hp;
   b->st.hpmax = st->hpmax;
   b->st.sp = st->sp;
   b->st.spmax = st->spmax;
   b->st.str = st->str;
   b->st.end = st->end;
   b->st.agl = st->agl;
   b->st.wis = st->wis;
   b->st.att = st->att;
   b->st.def = st->def;
   b->st.spd = st->spd;
   b->st.man = st->man;

   memcpy(b->skl, st->skl, (UWORD)4);

   for(n = 0; n < 4; ++n)
      b->st.attrib[n] = 1;

   /*
     character elemental attributes?

     Victor: Earth   (Weak vs. Air)
     Connor: Fire   (Weak vs. Water)
     Elya: Water   (Weak vs. Fire)
     Ryan: Water
     Roland: Air
     Alutha: Earth

     1,2,3 = fire,water,air
     0,1,2 = weak, normal, strong
   */

   n = b->type;
   if(n == CONNOR || n == ALUTHA) {
      // strong fire, weak water
      b->st.attrib[1] = BSTRONG;
      b->st.attrib[2] = BWEAK;
   }
   else if(n == RYAN) {
      // strong water
      b->st.attrib[2] = BSTRONG;
   }
   else if(n == ELYA) {
      // strong water
      b->st.attrib[2] = BSTRONG;
   }
   else if(n == VICTOR || n == VICTOR_WOLF) {
      // weak air
      b->st.attrib[3] = BWEAK;
   }
   else if(n == ROLAND) {
      // strong air
      b->st.attrib[3] = BSTRONG;
   }

   for(c = 1; c < 2; ++c) {
      n = st->e[c];
      if(n) {
         wepattr = item_info2(n-1)[5];
         if(wepattr & 1) {
            ++(b->st.attrib[1]);
            if(b->st.attrib[1] > BSTRONG)
               b->st.attrib[1] = BSTRONG;
         }
         if(wepattr & 2) {
            ++(b->st.attrib[2]);
            if(b->st.attrib[2] > BSTRONG)
               b->st.attrib[2] = BSTRONG;
         }
         if(wepattr & 4) {
            ++(b->st.attrib[2]);
            if(b->st.attrib[2] > BSTRONG)
               b->st.attrib[2] = BSTRONG;
         }
      }
   }
}

void bgetrealv(struct BDUDE *bch, WORD *vx, WORD *vy)
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

   *vx = px;
   *vy = py;
}



BYTE gem_animtab[][2] = {
   {   0, 24 },
   {   4, 23 },
   {   9, 22 },
   {  13, 19 },
   {  16, 16 },
   {  19, 13 },
   {  22,  9 },
   {  23,  4 },
   {  24,  0 },
   {  23, -5 },
   {  22,-10 },
   {  19,-14 },
   {  16,-17 },
   {  13,-20 },
   {   9,-23 },
   {   4,-24 },

   {   0,-24 },
   {  -5,-24 },
   { -10,-23 },
   { -14,-20 },
   { -17,-17 },
   { -20,-14 },
   { -23,-10 },
   { -24, -5 },
   { -24,  0 },
   { -24,  4 },
   { -23,  9 },
   { -20, 13 },
   { -17, 16 },
   { -14, 19 },
   { -10, 22 },
   {  -5, 23 },
   {   0, 24 },
};


void bpause()
{
        UBYTE oldsetting, oldsetting2;
        UBYTE n;

        // backup old settings
        oldsetting = pfs_targetspr;
        oldsetting2 = pfs_targetbgr;

        // backup the current palette
        memcpy(pal_bak, pal_cur, 128);
        setwindow(0, 144);
        do_delay(1);

        // darken pal_cur
        pfs_targetspr = 252;
        pfs_targetbgr = 254;
        pfs_setup((UWORD *)pal_cur, (UWORD *)COLOR_BLACK, 32);
        for(n = 0; n != 16; ++n)
                fade_step();
        pfs_setpal((UWORD *)pal_cur);

        // loop
        while(key[MENU])
                input();
        while(1) {
                w4vbl_done();
                input();

                if(key[UP] && bdsy > 0)
                        --bdsy;
                if(key[DOWN] && bdsy < 263)
                        ++bdsy;
                if(key[LEFT] && bdsx > 0)
                        --bdsx;
                if(key[RIGHT] && bdsx < 215)
                        ++bdsx;
                bspr_scrollto(bdsx, bdsy);

                if(key[MENU])
                        break;
        }

        // restore the old palette
        restorepal();
        setwindow(0, 112);
        do_delay(1);

        // restore old settings
        pfs_targetspr = oldsetting;
        pfs_targetbgr = oldsetting2;

        // reset timer since it's out of whack now
        vblank_timer = 0;
        bloops = 0;
}

void gemcheck(UBYTE target)
{
        UBYTE oldsetting;

        if(bch[target].gemshow) {
                gem_animate(target);
                bch[target].gemshow = 0;
                bcritical = 0;
        }
        if(bch[turn].gemshow) {
                gem_animate(turn);
                bch[turn].gemshow = 0;
                bcritical = 0;
        }
        if(bcritical) {
                oldsetting = pfs_targetspr;
                memcpy(pal_bak, pal_cur, (UWORD)128);
                setwindow(0, 144);
                do_delay(1);
                pfs_targetspr = 252;
                whiteout();
                do_delay(5);
                restorepal();
                do_delay(1);
                setwindow(0, 112);
                pfs_targetspr = oldsetting;

                vblank_timer = 0;
                bloops = 0;
        }

        bcritical = 0;
}

// block everything and do a cool gem animation
void gem_animate(UBYTE guy)
{
   struct DUDESTAT *st;
   UBYTE n, n2, c;
   WORD px, py;
   UBYTE oldsetting;

   oldsetting = pfs_targetspr;
   memcpy(pal_bak, pal_cur, (UWORD)128);

   bspr_scrollto(bch[guy].rx, bch[guy].ry);

   st = get_party(bch[guy].rep);
   n = st->e[3] - 1;
   n = gems_list[n] - 1;
   bbox[5] = n + 40;
   bicon_refresh(5);
   //bloadboxes();

   for(n = 0; n < 33; n += 1) {
      px = bch[guy].rx - 3;
      py = bch[guy].ry - 3;
      n2 = n % 3;
      if(n2 == 0)
         c = n;
      else if(n2 == 1)
         c = n - 1;
      else if(n2 == 2)
         c = n - 2;
      c = n;
      px += gem_animtab[c][0];
      py += -gem_animtab[c][1];

      bspr_8set(20, px,     py, 74, bboxpal[5]);
      bspr_8set(21, px + 8, py, 75, bboxpal[5]);
      do_delay(1);
   }
   setwindow(0, 144);
   do_delay(1);
   pfs_targetspr = 0xFC;
   whiteout();
   do_delay(5);
   //memcpy(pal_cur, pal_bak, (UWORD)128);
   //pfs_targetspr = 0;
   restorepal();
   do_delay(1);
   setwindow(0, 112);

   bspr_8set(20, 0, 0, -1, 0);
   bspr_8set(21, 0, 0, -1, 0);

   vblank_timer = 0;
   bloops = 0;

   pfs_targetspr = oldsetting;
}

UBYTE bshowail_conv[4] = { 6, 7,20,21 };
UBYTE bshowail_pal[4]  = { 0, 0, 0, 0 };
BYTE bshowail_x[2] = { -2, 3 };

void bshowail(UBYTE guy, UBYTE flop)
{
   UBYTE base, ail;
   BYTE offset;

   ail = bch[guy].ail;
   base = bshowail_conv[guy];

   if(ail) {
      --ail;
      offset = bshowail_x[(ail << 1) + flop];
      bspr_8set(base, bch[guy].rx + offset, bch[guy].ry - 19, (ail<<1) + flop + 10, bshowail_pal[ail]);
   }
   else
      bspr_8set(base, 0, 0, -1, 0);
}
