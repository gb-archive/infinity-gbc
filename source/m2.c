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

#include "defs.h"
#include "vars.h"
#include "d1.h"

#define STATMENU  0
#define ITEMMENU  1
#define EQUIPMENU 2
#define GEMSMENU  3
#define SKILMENU  4
#define SHOPMENU  5

#define MGS_EX    4
#define MGS_CH    16

// menu vars
extern BYTE index;
extern UBYTE cur_x, cur_y;
extern UBYTE atmenu;
extern UBYTE stat_selection;
extern UBYTE menu_ptr;
extern UBYTE gems_dotrick;

extern UBYTE gems_list[];
extern UBYTE gems_charge[];
extern UBYTE gems_rel[];

void index_to_cur(UBYTE index, UBYTE *x, UBYTE *y)
{
   UBYTE n, n2;

   n2 = index % 3;
   *x = 40 + n2 * 40;

   n = index / 3;
   *y = 48 + n * 32 - (n2 * 8);
}


struct BRE gembx[9];
struct BRE gemby[9];

UBYTE gem_xs[9], gem_ys[9];
UBYTE gem_xc[9], gem_yc[9], gem_zc[9];
UBYTE gem_xe[9], gem_ye[9];

UBYTE gem_xdist[9], gem_ydist[9];
UBYTE gem_atstep[9], gem_steps[9];

UBYTE gem_wait[9];
BYTE gem_zd[9];

void show_gems_guys()
{
   UBYTE n, x;
   UBYTE cx, cy;

   for(n = 0; n != 9; ++n) {
      index_to_cur(n, &cx, &cy);
      if(gems_list[n])
         mgs_sput(MGS_EX+n, cx, cy);
   }

   for(n = 0; n != 4; ++n) {
      st = get_party(n);
      x = st->e[3];
      if(x) {
         index_to_cur(x - 1, &cx, &cy);
         cx += 4;
         cy -= 8;
         mgs_sput(n + 16, cx, cy);
      }
   }
}

UBYTE gems_guy(UBYTE x)
{
   UBYTE n;

   // check only the party
   for(n = 0; n != 4; ++n) {
      st = get_party(n);
      if(!st)
         continue;

      if(st->e[3] == x + 1)
         return n + 1;
   }

   return NULL;
}

struct DUDESTAT *gems_guy2(UBYTE x)
{
   UBYTE n;

   // check all PC's, not just party
   for(n = 0; n != 6; ++n) {
      if(stats[n].e[3] == x + 1)
         return &stats[n];
   }

   return NULL;
}


void update_gemsmenu(UBYTE pos, UBYTE index)
{
   point_str(pos, 10);
   if(gems_list[index]) {
      ipstr(0, 6, (char *)item_info(gems_list[index]-1 + GEMS_INDEX) + 1);
      ipstr(0, 2, deci_16(gems_charge[index], 3));
      ipstr(0, 2, "GP");
   }
}

BYTE do_gemsmenu_real()
{
   UBYTE n, n2;
   BYTE c, r;
   UBYTE index, oindex, update;
   UBYTE cx, cy;
   UBYTE hold;
   UBYTE hold_index;
   UBYTE x;
   UBYTE ok;
   UBYTE pos;
   UBYTE cost, type;
   char *p, *p2;
   UBYTE all;
   UBYTE ret;
        struct DUDESTAT *st2;

   ret = CANCEL;

   // set initial coords / load gems
   gem_wait[0] = 6;
   gem_wait[1] = 7;
   gem_wait[2] = 8;
   gem_wait[3] = 5;
   gem_wait[4] = 4;
   gem_wait[5] = 3;
   gem_wait[6] = 0;
   gem_wait[7] = 1;
   gem_wait[8] = 2;

   for(n = 0; n != 9; ++n) {
      gem_xs[n] = 160;
      gem_ys[n] = 16;

      gem_atstep[n] = 0;
      gem_steps[n] = 12;
      gem_zc[n] = -(BYTE)(gem_steps[n]>>1);
      if(gems_list[n])
         mgs_gload(MGS_EX+n, gems_list[n]-1);
   }

   // set ending coords
   for(n = 0; n != 9; ++n) {
      index_to_cur(n, &cx, &cy);
      gem_xe[n] = cx;
      gem_ye[n] = cy;

      gem_zd[n] = gem_steps[n]>>1;

      gem_xc[n] = gem_xs[n];
      gem_yc[n] = gem_ys[n];

      bre_setup(gem_xs[n], gem_xe[n], gem_steps[n], &gembx[n]);
      bre_setup(gem_ys[n], gem_ye[n], gem_steps[n], &gemby[n]);
   }

   // cancel trick if no gems
   for(n = 0; n != 9; ++n) {
      if(gems_list[n])
         break;
   }
   if(n == 9)
      gems_dotrick = 0;

   if(!gems_dotrick)
      goto notrick;

   //mgs_sput(16, 16, 56);

   for(;;) {
      w4vbl_done();
      // move gems
      ok = 1;
      for(n = 0; n != 9; ++n) {
         if(gem_atstep[n] == gem_steps[n]) {
            continue;
         }
         ok = 0;

         if(gem_wait[n]) {
            --gem_wait[n];
            continue;
         }

         gem_xc[n] = bre_step(&gembx[n]);
         gem_yc[n] = bre_step(&gemby[n]);


         gem_zc[n] += gem_zd[n];
         --gem_zd[n];

         ++gem_atstep[n];
      }

      // update each gem
      for(n = 0; n != 9; ++n) {
         if(gems_list[n]) {
            mgs_sput(MGS_EX+n, gem_xc[n], gem_yc[n] - gem_zc[n]);
         }
      }

      if(ok)
         break;
   }

   gems_dotrick = 0;

notrick:
   show_gems_guys();

again:
   save_str();

   mgs_shide(0);
   for(;;) {
      do_delay(2);
      c = inputb();
      if(c == ENTER) {
         play_sfx(SFX_MENUACCEPT);
         break;
      }
      if(c == CANCEL || c == MENU || c == LEFT || c == RIGHT) {
         ret = c;
         goto end;
      }
   }


   mgs_sput(0, 32, 56);
   do {
      input();
   } while(key[ENTER]);

   index = 0;
   oindex = 0;
   cx = 0;
   cy = 0;
   hold = 0;

   pos = read_str();
after_usage:
   update = 1;
   pstr(2, 3, 6, ""); //p
   pstr(2, 4, 2, ""); //p2
   pstr(4, 4, 2, "");

   /*if(gems_list[index]) {
      p = (char *)item_info(gems_list[index]-1 + GEMS_INDEX) + 1;
      p2 = deci_16(gems_charge[index], 3);
   }
   else {
      p = menu_blank;
      p2 = menu_blank;
   }*/

   for(;;) {
      do_delay(2);

      // moved?  update screen
      if(index != oindex)
         update = 1;

      if(update) {
         update_gemsmenu(pos, index);
         update = 0;
      }

      index_to_cur(index, &cur_x, &cur_y);
      cur_x -= 8;
      cur_y += 8;
      mgs_sput(0, cur_x, cur_y);

      c = inputb();
      if(c == CANCEL || c == MENU) {
         f_curflick_off();
         ret = c;
         if(c == CANCEL)
            break;

         goto end;
      }

      if(key[ENTER]) {
         if(!hold) {
            hold = 1;
            hold_index = index;
            f_curflick_on(cur_x + 4, cur_y);
         }
      }
      else {
         if(hold) {
            f_curflick_off();
            hold = 0;

            if(hold_index == index) {
               n = gems_list[index];
               if(!n)
                  f_menu_error();
               else {
                  --n;
                  n2 = item_info2(n + GEMS_INDEX)[5];
                  cost = n2 & 63;
                  type = n2 & 128 ? 1: 0;

                  if(type && gems_charge[index] >= cost) {
                     if(n == GEM_CORAL && !encounter_protect) {
                        play_sfx(SFX_MENUACCEPT);
                        gems_charge[index] -= cost;
                        update_gemsmenu(pos, index);
                        mgs_shide(0);
                        f_curflick_on(cur_x + 4, cur_y);
                        do_delay(30);
                        f_curflick_off();

                        encounter_protect = 2;
                        ret = MENU;
                        goto end;
                     }
                     else if(n == GEM_MOONSTONE) {
                        play_sfx(SFX_MENUACCEPT);
                        gems_charge[index] -= cost;
                        update_gemsmenu(pos, index);
                        mgs_shide(0);
                        f_curflick_on(cur_x + 4, cur_y);
                        do_delay(30);
                        f_curflick_off();

                        encounter_protect = 3;  // re-using var for warp
                        ret = MENU;
                        goto end;
                     }
                     else if(n == GEM_EMERALD || n == GEM_DIAMOND) {
                        play_sfx(SFX_MENUACCEPT);
                        all = 0;
                        if(n == GEM_EMERALD)
                           all = 1;

                        // hide all
                        for(n2 = 0; n2 != 16; ++n2)
                           mgs_shide(n2);
                        save_str();
                        r = f_do_useitem(64+(all?128:0), index);
                        if(r == MENU) {
                           ret = MENU;
                           goto end;
                        }
                        f_show_menu(GEMSMENU);
                        restore_str();
                        show_gems_guys();
                        point_str(pos, 10);
                        goto after_usage;
                     }
                     else
                        f_menu_error();
                  }
                  else
                     f_menu_error();
               }
            }
            else {
               // do cool exchange animation

               // erase involved guys first
               n = gems_guy(hold_index);
               if(n)
                  mgs_shide(MGS_CH+n-1);
               n = gems_guy(index);
               if(n)
                  mgs_shide(MGS_CH+n-1);

               // change the gem equipped
               st = gems_guy2(hold_index);
               st2 = gems_guy2(index);
               if(st)
                  st->e[3] = index + 1;
               if(st2)
                  st2->e[3] = hold_index + 1;

               // start positions
               index_to_cur(hold_index, &cur_x, &cur_y);
               gem_xs[0] = cur_x;
               gem_ys[0] = cur_y;
               index_to_cur(index, &cur_x, &cur_y);
               gem_xs[1] = cur_x;
               gem_ys[1] = cur_y;

               // end positions
               gem_xe[0] = gem_xs[1];
               gem_xe[1] = gem_xs[0];
               gem_ye[0] = gem_ys[1];
               gem_ye[1] = gem_ys[0];

               // rest of init
               for(n = 0; n != 2; ++n) {
                  if(n == 0)
                     n2 = hold_index;
                  else
                     n2 = index;
                  if(gems_list[n2])
                     mgs_gload(MGS_EX+n2, gems_list[n2]-1);

                  gem_atstep[n] = 0;
                  gem_steps[n] = 16;

                  gem_zc[n] = -(BYTE)(gem_steps[n]>>1);
                  if(!n)
                     gem_zc[n] = 0;
                  gem_zd[n] = gem_steps[n]>>1;

                  gem_xc[n] = gem_xs[n];
                  gem_yc[n] = gem_ys[n];

                  bre_setup(gem_xs[n], gem_xe[n], gem_steps[n], &gembx[n]);
                  bre_setup(gem_ys[n], gem_ye[n], gem_steps[n], &gemby[n]);
               }

               // do it!
               for(x = 0; x != 16; ++x) {
                  w4vbl_done();

                  // move gems
                  for(n = 0; n != 2; ++n) {
                     gem_xc[n] = bre_step(&gembx[n]);
                     gem_yc[n] = bre_step(&gemby[n]);

                     if(n)
                        gem_zc[n] += gem_zd[n];
                     --gem_zd[n];

                     ++gem_atstep[n];
                  }

                  // update each gem
                  for(n = 0; n != 2; ++n) {
                     if(n == 0)
                        n2 = hold_index;
                     else
                        n2 = index;
                     if(gems_list[n2])
                        mgs_sput(MGS_EX+n2, gem_xc[n], gem_yc[n] - gem_zc[n]);
                  }
               }

               // do a quick switch and hope no one notices
               x = gems_list[index];
               gems_list[index] = gems_list[hold_index];
               gems_list[hold_index] = x;
               x = gems_charge[index];
               gems_charge[index] = gems_charge[hold_index];
               gems_charge[hold_index] = x;

               w4vbl_done();
               if(gems_list[hold_index]) {
                  mgs_sput(MGS_EX+hold_index, gem_xs[0], gem_ys[0]);
                  mgs_gload(MGS_EX+hold_index, gems_list[hold_index]-1);
               }
               else
                     mgs_shide(MGS_EX+hold_index);
               if(gems_list[index]) {
                  mgs_sput(MGS_EX+index,      gem_xs[1], gem_ys[1]);
                  mgs_gload(MGS_EX+index,      gems_list[index]-1);
               }
               else
                  mgs_shide(MGS_EX+index);

               show_gems_guys();

               update = 1;
            }
         }
      }

      oindex = index;
      if(c == UP && cy > 0) {
         --cy;
      }
      if(c == DOWN && cy < 2) {
         ++cy;
      }
      if(c == LEFT && cx > 0) {
         --cx;
      }
      if(c == RIGHT && cx < 2) {
         ++cx;
      }
      index = cy * 3 + cx;
   }
   restore_str();
   goto again;

end:
   restore_str();
   for(n = 0; n < 4; ++n)
      mgs_shide(MGS_CH+n);

   return ret;
}


UBYTE do_yesnomenu()
{
   UBYTE x;
   BYTE c;

   point_str(128, 4);

   menu_drawbox(14, 0, 5, 4);
   pstr(16, 1, 2, "Yes");
   pstr(16, 2, 2, "No");

   x = 1;
   while(1) {
      do_delay(2);
      put_tile(15, 1,  x ? 25: 4);
      put_tile(15, 2, !x ? 25: 4);

      c = inputb();
      if(c == UP)
         x = 1;
      if(c == DOWN)
         x = 0;
      if(c == ENTER) {
         play_sfx(SFX_MENUACCEPT);
         break;
      }
      if(c == CANCEL) {
         x = 0;
         break;
      }
   }

   return x;
}

void opmark(UBYTE x, UBYTE val, UBYTE type)
{
   if(x == 0) {
      if(val == 0)
         put_tile(2, 4, type);
      else
         put_tile(9 + (val << 1), 5, type);
   }
   else if(x < 4)
      put_tile(val + 2, x + 7, type);
   else if(x < 8)
      put_tile(val + 2, x + 10, type);
}

UBYTE old_options[7];

UBYTE oprange[7] = { 5, 16, 16, 16, 16, 16, 16 };
void do_opmenu()
{
   BYTE c;
   UBYTE index, n, ok;

   for(n = 0; n != 7; ++n)
      old_options[n] = 255;

   index = 0;
   while(1) {
      // show option changes
      ok = 1;
      for(n = 0; n != 7; ++n) {
         if(old_options[n] != options[n]) {
            ok = 0;

            // erase the old tile
            if(old_options[n] != 255)
               opmark(n, old_options[n], 4);

            // mark the new spot
            opmark(n, options[n], 25);
         }
      }
      if(!ok) {
         load_gradcolors(options + 1);
         w4line(72);
         grad_init();
         grad_every = 0;
      }
      memcpy(old_options, options, (UWORD)7);

      do_delay(2);

      cur_x = 0; //8;
      if(index == 0)
         cur_y = 36;
      else {
         cur_x = 0;
         if(index < 4)
            cur_y = (index<<3) + 60;
         else if(index < 8)
            cur_y = (index<<3) + 84;
      }
      mgs_sput(0, cur_x, cur_y);

      c = inputb();
      if(c == UP && index > 0)
         --index;
      if(c == DOWN && index < 6)
         ++index;
      if(c == RIGHT) {
         if(options[index] < oprange[index] - 1)
            ++options[index];
      }
      if(c == LEFT) {
         if(options[index] > 0)
            --options[index];
      }
      if(c == CANCEL || c == SELECT)
         break;
   }
}

unsigned char shop_menu_tiles[] = {
   0, 1, 1, 1, 1, 2,19,19,19,19,19,19,19,19,19,19,19,19,19,19,
   3, 4, 4, 4, 4, 5,19,19,19,19,19,19,19,19,19,19,19,19,19,19,
   3, 4, 4, 4, 4,12, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2,
   3, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 5,
   3, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 5,
   3, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 5,
   3, 9, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,10,11, 5,
   3, 5, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 3,15, 5,
   3, 5, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 3,15, 5,
   3, 5, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 3,15, 5,
   3, 5, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 3,15, 5,
   3, 5, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 3,15, 5,
   3, 5, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 3,15, 5,
   3, 5, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 3,15, 5,
   3, 5, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 3,15, 5,
   3, 5, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 3,15, 5,
   3,12, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,13,14, 5,
   6, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 8,
};

char opmask[3] = { 'R', 'G', 'B' };
void draw_opgrad(UBYTE base)
{
   UBYTE n;
   char str[4] = "R:";

   for(n = 0; n != 3; ++n) {
      str[0] = opmask[n];
      pstr(1, base + n, 3, str);
      pstr(18, base + n, 1, "]");
   }
}

void draw_shopmenu()
{
   restore_tiles(0, 0, 20, 18, shop_menu_tiles);
   if(!optionmode) {
      ppstr(1, 1, 6, 4, "Shop");
      pstr(2, 4, 3, "Buy");
      pstr(6, 4, 3, "Sell");
      pstr(16, 3, 3, "Aurum");
   }
   else {
      menu_clearbox(1, 6, 18, 11);
      pstr(1, 1, 4, "Options");
      pstr(1, 3, 6, "Battle Mode");
      pstr(3, 4, 5, "Wait");
      pstr(10, 4, 4, "Action");
      pstr(12, 5, 1, "1");
      pstr(14, 5, 1, "2");
      pstr(16, 5, 1, "3");
      pstr(18, 5, 1, "4");

      pstr(1, 7, 10, "Window Color: Top");
      pstr(1,13, 12, "Window Color: Bottom");
      draw_opgrad(8);
      draw_opgrad(14);
   }
}

void draw_shopbuy(UBYTE start)
{
   UBYTE n;
   char *s;

   for(n = 0; n != 9 && start != item_shop_at; ++n, ++start) {
      put_tile(3, 7 + n, ICON_BASE + item_info(item_shop[start])[0] - 'A');
      pstr( 4, 7 + n, 6, (char *)item_info(item_shop[start]) + 1);
      if(!item_shop[start])
         s = menu_blank;
      else
         s = deci_16((UWORD)item_cost[start], 6);
      pstr(12, 7 + n, 4, s);
   }
}

void draw_shopsell(UBYTE start)
{
   UBYTE n;
   char *s;
   UBYTE w;

   for(n = 0; n != 9 && start != item_num; ++n, ++start) {
      put_tile(3, 7 + n, ICON_BASE + item_info(item_list_type[start])[0] - 'A');
      pstr( 4, 7 + n, 6, (char *)item_info(item_list_type[start]) + 1);
      if(!item_list_type[start])
         s = menu_blank;
      else {
         w = item_sellcost(item_list_type[start]);
         if(w == 0)
            s = menu_cantsell;
         else
            s = deci_16(w, 6);
      }
      pstr(12, 7 + n, 4, s);
   }
}

void shop_reset()
{
   item_shop_at = 0;
}

void shop_add(UBYTE type, UWORD price)
{
   item_shop[item_shop_at] = type;
   item_cost[item_shop_at] = price;
   ++item_shop_at;
}

UBYTE shop_len;
UBYTE pbak[2];

void shop_youhave(UBYTE x)
{
   UBYTE lastpos;

   lastpos = read_str();
   point_str(pbak[1], 2);
   ipstr(0, 2, deci_16((UWORD)x, 3));
   point_str(lastpos, 0);
}

void shop_gold()
{
   UBYTE lastpos;

   lastpos = read_str();
   point_str(pbak[0], 4);
   ipstr(0, 4, deci_24(gold, 6));
   point_str(lastpos, 0);
}

void do_shopmenu()
{
   UBYTE n;
   BYTE c;
   UBYTE choose, index, oindex, scrolled, bary;
   BYTE at, scroll;
   UWORD x;
   char *s;
   UBYTE *p;
   UBYTE price[3];
   UWORD w;

   f_slide_in_menu(SHOPMENU);

   if(optionmode) {
      do_opmenu();
      goto shopdone;
   }

   pbak[0] = read_str();
   pstr(11, 3, 4, deci_24(gold, 6));

   choose = 0;
   while(1) {
           save_str();
      // buy or sell?
      while(1) {
         do_delay(2);
         cur_x = choose ? 36: 4;
         cur_y = 36;
         mgs_sput(0, cur_x, cur_y);

         c = inputb();
         if(c == LEFT)
            choose = 0;
         if(c == RIGHT)
            choose = 1;
         if(c == ENTER || c == CANCEL)
            break;
      }
      if(c == CANCEL)
         break;

      play_sfx(SFX_MENUACCEPT);

      mgs_shide(0);
      f_curflick_on(cur_x, cur_y);

      pstr(11, 5, 5, "You have:");
      pbak[1] = read_str();
      pstr(16, 5, 2, menu_blank);

      // what to buy or sell
      index = 0;
      oindex = !index;
      scroll = 0;
      scrolled = 0;
      scroll_location = read_str();
      point_str(scroll_location, 90);

      if(choose == 0) {
         shop_len = item_shop_at;
         p = item_shop;
         draw_shopbuy(0);
      }
      else {
         shop_len = item_num;
         p = item_list_type;
         draw_shopsell(0);
      }
      f_menu_scrollinit(scroll_location, scroll_location + 90, 4);

      while(1) {
         // sync up
         do_delay(2);
         at = index - scroll;

         cur_x = 12;
         cur_y = at * 8 + 60;
         mgs_sput(0, cur_x, cur_y);

         if(shop_len < 2) {
            bary = 0;
         }
         else {
            x = (UWORD)64 * index / (shop_len-1);
            bary = (UBYTE)x;
         }
         mgs_sput(2, 144, bary + 56);

         if(oindex != index) {
            shop_youhave(f_items_have(p[index]));
         }
         oindex = index;

         c = inputb();
         if(c == DOWN && index < shop_len - 1) {
            index += 1;

            if(at == 8) {
               if(!p[index])
                  s = menu_blank;
               else {
                  if(choose == 0)
                     s = deci_16((UWORD)item_cost[index], 6);
                  else {
                     w = item_sellcost(p[index]);
                     if(!w)
                        s = menu_cantsell;
                     else
                        s = deci_16(w, 6);
                  }
               }

               f_menu_scroll(0, 3, 12, 7, 9,
                  (char *)item_info(p[index]), s);

               scrolled = 1;
               ++scroll;
            }
         }
         if(c == UP && index > 0) {
            index -= 1;

            if(at == 0) {
               if(!p[index])
                  s = menu_blank;
               else {
                  if(choose == 0)
                     s = deci_16((UWORD)item_cost[index], 6);
                  else {
                     w = item_sellcost(p[index]);
                     if(!w)
                        s = menu_cantsell;
                     else
                        s = deci_16(w, 6);
                  }
               }

               f_menu_scroll(1, 3, 12, 7, 9,
                  (char *)item_info(p[index]), s);

               scrolled = 1;
               --scroll;
            }
         }

         if(c == ENTER) {
            if(p[index]) {
               if(choose == 0) {
                  set_24(price, item_cost[index]);
                  if(cmp_24(gold, price) >= 0 && f_items_have(p[index]) < 100) {
                     sub_24(gold, price);
                     play_sfx(SFX_BUY);
                     f_items_add(p[index]);
                     shop_youhave(f_items_have(p[index]));
                     shop_gold();
                  }
                  else
                     f_menu_error();
               }
               else {
                  if(item_sellcost(p[index])) {
                     set_24(price, item_sellcost(p[index]));
                     add_24(gold, price);
                     if(cmp_24(gold, mostgold) > 0) {
                        set_24(gold, 0);
                        add_24(gold, mostgold);
                     }
                     play_sfx(SFX_BUY);
                     f_items_del(p[index]);
                     if(f_items_have(p[index]) == 0) {
                        f_menu_redraw(3, 7, 9, at, (char *)item_info(p[index]), "");
                     }
                     shop_youhave(f_items_have(p[index]));
                     shop_gold();
                  }
                  else
                     f_menu_error();
               }
            }
            else
               f_menu_error();
         }
         if(c == CANCEL)
            break;
      }
      f_curflick_off();

      mgs_shide(2);
                // clear the whole darn thing (width=14)
      menu_clearbox(3, 7, 14, 9);
      restore_str();
   }

shopdone:
   f_slide_out_menu();
}

UBYTE *fileptr;
void write(void *block, UBYTE size)
{
   memcpy(fileptr, block, (UWORD)size);
   fileptr += size;
}

void read(void *block, UBYTE size)
{
   memcpy(block, fileptr, (UWORD)size);
   fileptr += size;
}

UBYTE ss_active;
UBYTE ss_level, ss_area, ss_hour, ss_min;
UBYTE ss_num;
UBYTE ss_type[4];
UBYTE ss_options[7];
UBYTE savesig[8];
char slsig[8] = "BONJAHL";

void load_saveinfo(UBYTE slot)
{
   UBYTE lastpage;
   UBYTE gametime[4];

   fopen(slot);
   lastpage = ram_page(5);
   fileptr = (UBYTE *)0xD800;

   read(savesig, 7);
   savesig[7] = 0;
   if(strcmp((UBYTE *)savesig, slsig)) {
      ss_active = 0;
      ram_page(lastpage);
      return;
   }
   ss_active = 1;

   read(&ss_num, 1);
   if(ss_num > 4)
      ss_num = 4;
   read(&ss_type, 4);
   read(&ss_level, 1);
   read(&ss_area, 1);
   read(gametime, 4);
   read(ss_options, 7);

   ss_hour = gametime[0];
   ss_min = gametime[1];

   ram_page(lastpage);
}


void savegame(UBYTE slot)
{
   UBYTE a[4];
   UBYTE n, at;
   UBYTE lastpage;
   UBYTE level;

   area = regs[0];

   fopen(slot);
   lastpage = ram_page(5);
   fileptr = (UBYTE *)0xD800;

   write(slsig, 7);
   for(at = 0, n = 0; n != 4; ++n) {
      st = get_realparty(n);
      if(st) {
         if(at == 0)
            level = st->level;
         a[at++] = st->type;
      }
   }
   write(&at, 1);
   write(a, 4);
   write(&level, 1);
   write(&area, 1);
   disable_interrupts();
   a[0] = gametime[0];
   a[1] = gametime[1];
   a[2] = gametime[2];
   a[3] = gametime[3];
   enable_interrupts();
   write(a, 4);
   write(options, 7);
   write(&lastsaveslot, 1);
   write(party, 4);
   write(realparty, 4);
   for(n = 0; n != 6; ++n)
      write(&stats[n], sizeof(struct DUDESTAT));
   write(vars, 96);
   write(flags, 32);
   write(regs, 8);
   write(item_list_type, 100);
   write(item_list_num, 100);
   write(gems_list, 9);
   write(gems_charge, 9);
   write(gold, 3);
   write(&boat_x, 1);
   write(&boat_y, 1);
   write(&boat_dir, 1);

   ram_page(lastpage);

   fclose(slot);
}

void loadgame(UBYTE slot)
{
   UBYTE a[4];
   UBYTE n, at;
   UBYTE lastpage;
   UBYTE level;
   UBYTE c;

   fopen(slot);
   lastpage = ram_page(5);
   fileptr = (UBYTE *)0xD800;

   read(slsig, 7);
   read(&at, 1);
   read(a, 4);
   read(&level, 1);
   read(&area, 1);
   read(a, 4);
   disable_interrupts();
   gametime[0] = a[0];
   gametime[1] = a[1];
   gametime[2] = a[2];
   gametime[3] = a[3];
   enable_interrupts();
   read(options, 7);
   read(&lastsaveslot, 1);
   read(party, 4);
   read(realparty, 4);
   for(n = 0; n != 6; ++n)
      read(&stats[n], sizeof(struct DUDESTAT));
   read(vars, 96);
   read(flags, 32);
//   for(n = 0; n != 32; ++n)
//      flags[n] = 0;
   read(regs, 8);
   read(item_list_type, 100);
   read(item_list_num, 100);
   read(gems_list, 9);
   read(gems_charge, 9);
   read(gold, 3);
   read(&boat_x, 1);
   read(&boat_y, 1);
   read(&boat_dir, 1);

   ram_page(lastpage);

   fclose(slot);
}

UBYTE grad_slm_table[3] = { 24, 64, 104 };

void start_gradient_slm_real()
{
   UBYTE n;

   grad_var1 = 0;
   grad_var2 = 24;
   grad_init();

   for(n = 0; n != 3; ++n) {
      load_saveinfo(n);
      if(ss_active)
         load_gradcolors(ss_options + 1);
      else
         load_gradcolors(defaultcolors);
      grad_var1 = grad_slm_table[n];
      grad_var2 = 40;
      grad_init();
   }
   load_gradcolors(options + 1);
}

// save / load menu
UBYTE slpals[12];
UBYTE slx[4] = { 28, 60, 92, 124 };
UBYTE slwin[5];
UBYTE oslwin[5];
char empty_str[] = "Empty";
UBYTE sl_textbase;

void sl_redraw(UBYTE x)
{
   UBYTE n;
   UBYTE offset;
   UBYTE a1, a2;
   UBYTE timestr[6];

   menu_drawbox(0, x * 5 + 3, 19, 5);
   point_str(sl_textbase + x * 16, 16);
   start_gradient_slm_real();

   load_saveinfo(x);
   //if(!ss_active)
   // while(1);

   a1 = x * 5 + 4;

   pstr( 1, a1, 3, "Level");
   pstr( 4, a1, 2, deci_16(ss_level, 3));
   fpstr( 7, a1, 8, area_name[ss_area-1], DATA_PAGE);
   strcpy(timestr, deci_16(ss_hour, 2));
   strcat(timestr, ":");
   strcat(timestr, deci_16(ss_min, 2));
   if(ss_min < 10)
      timestr[3] = '0';
   pstr(15, a1, 3, (char *)timestr);

   for(n = 0; n != 4; ++n) {
      a2 = x * 4 + n;
      a1 = a2 + 4;

      if(n < ss_num && ss_active)
         mgs_sload(a1|128, ss_type[n], DOWN, 0);
      else
         mgs_sload(a1|128, 0, DOWN, 63);

      //slpals[a2] = mgs_getpal(a1);
      //mgs_setpal(a1, 6);
      mgs_sput(a1, slx[n], (x+1) * 40);
   }
}

extern UBYTE slm_var;
UBYTE do_slmenu(UBYTE type)
{
   UBYTE n, n2;
   BYTE c;
   UBYTE index, oindex;
   UBYTE flick;
   UBYTE count;
   UBYTE x;
   UBYTE offset;
   UBYTE a1, a2;
   UBYTE timestr[6];
   UBYTE active[3];
   UBYTE saveok;

   load_graypal();
   mgs_slm();
   mgs_wload(0, 0, 7);
   mgs_wload(1, 0, 7);

   offset = 0;
   if(type == 1) {
      // note: top menu is drawn before calling this function
      menu_drawbox(0, 3, 19, 5);
      menu_drawbox(0, 8, 19, 5);
      menu_drawbox(0,13, 19, 5);
      offset = 3;
   }

   // slide in the windows
   slwin[0] = (UBYTE)-72;
   slwin[1] = 160;
   slwin[2] = 160;
   slwin[3] = 160;

   // set cursor position
   if(type == 0)
      index = 0;
   else
      index = (lastsaveslot%3) + 1;

   // note: if type is 1 then reset_str() and the caption are predrawn
   if(type == 0) {
      reset_str(0);
      ipstr(9, 7, "New Game");
   }
   sl_textbase = read_str();

   // read in the save games
   for(n2 = 0; n2 != 3; ++n2) {
      load_saveinfo(n2);
      active[n2] = ss_active;

      point_str(sl_textbase + n2 * 16, 16);
      a1 = n2 * 5 + 1 + offset;
      if(ss_active) {
         pstr( 1, a1, 3, "Level");
         pstr( 4, a1, 2, deci_16(ss_level, 3));
         fpstr( 7, a1, 8, area_name[ss_area-1], DATA_PAGE);
         strcpy(timestr, deci_16(ss_hour, 2));
         strcat(timestr, ":");
         strcat(timestr, deci_16(ss_min, 2));
         if(ss_min < 10)
            timestr[3] = '0';
         pstr(15, a1, 3, (char *)timestr);
      }
      else {
         ++a1;
         pstr( 8, a1, 4, empty_str);
      }
      for(n = 0; n != 4; ++n) {
         a2 = n2 * 4 + n;
         a1 = a2 + 4;

         if(n < ss_num && ss_active)
            mgs_sload(a1|128, ss_type[n], DOWN, 0);
         else
            mgs_sload(a1|128, 0, DOWN, 63);

         slpals[a2] = mgs_getpal(a1);

         mgs_setpal(a1, (n2 == lastsaveslot && type == 1) ? slpals[a2]: 6);
         mgs_sput(a1, slx[n] + 152, (UBYTE)-16);
      }
   }

   // no savegames on loading?  autoselect "new"
   if(!active[0] && !active[1] && !active[2] && type == 0)
      return 0;

   for(x = 120; ; ++x) {
      oslwin[1] = slwin[1];
      oslwin[2] = slwin[2];
      oslwin[3] = slwin[3];

      if(x >= 120 && x < 156)
         slwin[0] += 2;
      if(x >= 120)
         slwin[1] = ((UWORD)slwin[1] * 15 + 8) / 16;
      if(x >= 135)
         slwin[2] = ((UWORD)slwin[2] * 15 + 8) / 16;
      if(x >= 150)
         slwin[3] = ((UWORD)slwin[3] * 15 + 8) / 16;

      oslwin[1] -= slwin[1];
      oslwin[2] -= slwin[2];
      oslwin[3] -= slwin[3];
      mgs_shift(1, oslwin[1], 40);
      mgs_shift(2, oslwin[2], 80);
      mgs_shift(3, oslwin[3],120);

      // show cursor ?
      if(index) {
         cur_x = slwin[index] - 4;
         cur_y = (index-1) * 40 + 24 + 19;
         mgs_sput(0, cur_x, cur_y);
      }

      for(n = 0; n < 4; ++n) {
         slm_setx(n, slwin[n]);
      }
      w4vbl_done();

      if(slwin[3] == 8)
         break;
   }


   // make a choice
   oindex = 0;

   while(1) {
      do_delay(2);

      if(index == 0) {
         cur_x = 0;
         cur_y = 11;
      }
      else {
         cur_x = 4;
         cur_y = (index-1) * 40 + 24 + 19;
      }

      mgs_sput(0, cur_x, cur_y);

      if(oindex != index) {
         for(n2 = 1; n2 != 4; ++n2) {
            a1 = n2 << 2;
            a2 = a1 + 4;
            for(n = a1; n != a2; ++n) {
               mgs_setpal(n, n2 == index ? slpals[n-4]: 6);
            }
         }
      }
      oindex = index;

      c = inputb();
      if(c == UP && index > 0) {
         index -= 1;
      }
      if(c == DOWN && index < 3) {
         index += 1;
      }

      if(type == 1 && index == 0)
         index = 1;


      if(c == ENTER || c == MENU) {
         // new game
         if(index == 0) {
            play_sfx(SFX_MENUACCEPT);
            break;
         }
         else {
            // load game
            if(type == 0) {
               if(active[index-1]) {
                  play_sfx(SFX_MENUACCEPT);
                  loadgame(index-1);
                  break;
               }
            }
            // save game
            else {
               play_sfx(SFX_MENUACCEPT);
               n = index - 1;
               saveok = 1;
               if(n != lastsaveslot && active[n]) {
                  point_str(sl_borderloc, 18);
                  ipstr(  0, 0, "Save OK?");
                  ipstr(112, 0, "Yes   No");
                  n2 = 0;
                  flick = 0;
                  while(1) {
                     flick &= 3;
                     ++flick;

                     do_delay(2);
                     mgs_sput(0, n2 ? 108: 128, 12);
                     if(flick & 2)
                        mgs_sput(1, cur_x, cur_y);
                     else
                        mgs_shide(1);

                     c = inputb();
                     if(c == LEFT)
                        n2 = 1;
                     if(c == RIGHT)
                        n2 = 0;
                     if(c == ENTER) {
                        play_sfx(SFX_MENUACCEPT);
                        break;
                     }
                     if(c == CANCEL) {
                        n2 = 0;
                        break;
                     }
                  }
                  saveok = n2;
                  c = -1;

                  mgs_sput(0, cur_x, cur_y);
                  mgs_shide(1);
                  point_str(sl_borderloc, 18);
                  ipstr(52, 0, "Save Game");
               }

               if(saveok) {
                  lastsaveslot = index - 1;
                  savegame(lastsaveslot);
                  sl_redraw(lastsaveslot);
                  break;
               }
            }
         }
      }

      // only cancel if saving
      if(c == CANCEL && type == 1)
         break;
   }

   if(c != CANCEL) {
      flick = 0;
      for(n = 0; n != 15; ++n) {
         flick &= 3;
         ++flick;

         do_delay(2);
         if(flick & 2)
            mgs_sput(0, cur_x, cur_y);
         else
            mgs_shide(0);
      }
   }
   mgs_shide(0);


   // slide them out
   n2 = 0;
   for(x = 120; ; ++x) {
      oslwin[1] = slwin[1];
      oslwin[2] = slwin[2];
      oslwin[3] = slwin[3];

      if(x >= 120 && x <= 156)
         slwin[0] -= 2;
      if(x >= 145 && slwin[1] < 160)
         slwin[1] += n2>>1;
      if(x >= 137 && slwin[2] < 160)
         slwin[2] += n2>>1;
      if(x >= 120 && slwin[3] < 160)
         slwin[3] += n2>>1;

      oslwin[1] -= slwin[1];
      oslwin[2] -= slwin[2];
      oslwin[3] -= slwin[3];
      mgs_shift(1, oslwin[1], (UBYTE)-16);
      mgs_shift(2, oslwin[2], (UBYTE)-16);
      mgs_shift(3, oslwin[3], (UBYTE)-16);

      w4vbl_done();
      for(n = 0; n < 4; ++n) {
         slm_setx(n, slwin[n]);
      }

      if(slwin[0] < (UBYTE)-72)
         break;
      ++n2;
   }

   setwinpal();

   //do_delay(240);

   return index;
}


// extra stuff

#include "eve/exptab.h"

UBYTE expnext[3];
UBYTE *get_expnext(UBYTE level)
{
   memcpy(expnext, (UBYTE *)exptab + (level * 3), (UWORD)3);
   return expnext;
}

/*
P indicates primary stat
S indicates secondary

I would do it like this:
P increase = 3 to 4
S increase = 2 to 3
Other inc  = 1 to 3

That way, you don't always gain the exact same in your primary and secondary

      HP SP STR   END   AGL   WIS      LEVEL
Connor      40 20 6P 4  5  6S    0
Ryan     30 10 5S 3  8P 3     +1
Elya     20 30 3  3S 2  9P    -1
Victor      70 10 8P 5  9S 3     0
Roland      50 10 9P 7S 3  4     +2
Alutha      80 20 9P 7S 4  6     +2

I would do HP gained per level as (4*End/5) to (5*End/4)
sp gain: (4*Wis/15) to (5*Wis/12)

I don't know what the max level will be in the game, but at level 20, that gives the highest possible HP gained by someone
who has gained the max END every level to be 100. Lowest, then, would be 64.  It sets the spread for Connor on level 1to
be 8 to 10.  It's a pretty wide spread, we can narrow it if you like.  Also, if you want it to go up faster, we can do that too.

SP increase would be the same, divided by 3.  That way, most chars won't go crazy with tons of MP by the end of the game.



BTW, new HP gain system is:

Elya gets 3-6 per level
Connor 4-8
Roland 5-10
Alutha 5-10
Victor 4-8
Ryan 4-7

They then get +1 hp per level per 5 levels of experience

New MP gain is

E  5-10
C 4-8
Ro 3-6
Vi 2-4
Alu 3-6
Ryan 2-4

Same +1 bonus

End will now be used in the damage eeq rather than HP gain

*/

UBYTE levhp_base[6]  = { 4, 4, 3, 4, 5, 5 };
UBYTE levhp_range[6] = { 4, 3, 3, 4, 5, 5 };
//UBYTE levsp_base[6]  = { 4, 2, 5, 2, 3, 3 };
UBYTE levsp_base[6]  = { 2, 1, 2, 1, 1, 1 };
UBYTE levsp_range[6] = { 4, 2, 5, 2, 3, 3 };

extern struct DUDESTAT temp_guy;

UBYTE lev_primary[6]   = { 0, 2, 3, 0, 0, 0 };
UBYTE lev_secondary[6] = { 3, 0, 1, 2, 1, 1 };
UBYTE lev_third[6]     = { 1, 1, 2, 1, 2, 2 };
UBYTE lev_fourth[6]    = { 2, 3, 0, 3, 3, 3 };

 BYTE lev_catchup[6]   = { 0, 1,-1, 0, 0, 2 };

void gain_stat(struct DUDESTAT *guy, UBYTE stat_num, UBYTE amount)
{
   switch(stat_num) {
      case 0: guy->str += amount; break;
      case 1: guy->end += amount; break;
      case 2: guy->agl += amount; break;
      case 3: guy->wis += amount; break;
   }
}

UBYTE gain_level(struct DUDESTAT *guy)
{
   UBYTE tmp[3];
   UBYTE n;
   BYTE i;
   UBYTE type;
   UBYTE lo, hi, diff;

   type = guy->type;
   if(type == VICTOR_WOLF) // victor wolf
      type = 3;

   // copy over
   memcpy(&temp_guy, guy, sizeof(struct DUDESTAT));

   n = temp_guy.level;
   memcpy(tmp, get_expnext(n), (UWORD)3);
   i = cmp_24(temp_guy.exp, tmp);
   if(i < 0)
      return 0;

   ++temp_guy.level;
   if(temp_guy.level == 100)
      memcpy(temp_guy.exp, tmp, (UWORD)3);

   gain_stat(&temp_guy, lev_primary[type], 3 + (randfunc() % 2));
   gain_stat(&temp_guy, lev_secondary[type], 2 + (randfunc() % 2));
   gain_stat(&temp_guy, lev_third[type], 1 + (randfunc() % 3));
   gain_stat(&temp_guy, lev_fourth[type], 1 + (randfunc() % 3));

   lo = levhp_base[type];
   diff = levhp_range[type] + 1;
   hi = temp_guy.level / 5;
   temp_guy.hpmax += (randfunc() % diff) + lo + hi;

   /*lo = 4 * temp_guy.end / 5;
   hi = 5 * temp_guy.end / 4;
   diff = hi - lo;
   temp_guy.hpmax += (randfunc() % diff) + lo;*/

   lo = levsp_base[type];
   /*diff = levsp_range[type] + 1;
   hi = temp_guy.level / 5;
   temp_guy.spmax += (randfunc() % diff) + lo + hi;*/
   temp_guy.spmax += lo;

   /*lo = (UWORD)4 * (UWORD)temp_guy.wis / 40;
   hi = (UWORD)5 * (UWORD)temp_guy.wis / 30;
   diff = hi - lo;
   temp_guy.spmax += (randfunc() % diff) + lo;*/

   gl_str = temp_guy.str - guy->str;
   gl_end = temp_guy.end - guy->end;
   gl_agl = temp_guy.agl - guy->agl;
   gl_wis = temp_guy.wis - guy->wis;

   memcpy(guy, &temp_guy, sizeof(struct DUDESTAT));

   return 1;
}

void catchup_level(struct DUDESTAT *guy)
{
   UBYTE type;
   BYTE herolevel, guylevel;
   UBYTE tmp[3];

   type = guy->type;
   if(type == VICTOR_WOLF) // victor wolf
      type = 3;

   herolevel = stats[0].level + lev_catchup[type];
   guylevel = guy->level;

   if(guylevel >= herolevel)
      return;

   // get exp of herolevel
   memcpy(&guy->exp, get_expnext(herolevel-1), (UWORD)3);
   while(gain_level(guy));

   guy->hp = guy->hpmax;
   guy->sp = guy->spmax;
   f_stats_recalc(guy);
}

// skills
char skill_names[][10] = {
   "Engulf",
   "Jolt",
   "Tidal",
   "Blaze",
   "Storm",
   "Tsunam",
   "Rumble",

   "Embrace",
   "Hope",
   "Soulslurp",
   "Sacrifice",
   "Wake",
   "Inversion",
   "Justice",
   "Leap",
   "Longshot",
   "Spray",
   "Wildcall",
   "Slash",
   "Whirlwind",

   "Spawn",
   "Charm",
};

UBYTE skill_cost[] = {
   4,
   4,
   4,
   12,
   12,
   12,
   8,

   5,
   15,
   4,
   8,
   8,
   8,
   8,
   3,
   1,
   8,
   8,
   8,
   8,

   0,
   0,
};


// 0 = Single enemy
// 1 = Single hero
// 2 = Multi enemy
// 3 = Multi hero
// 4 = Self

UBYTE skill_type[] = {
   0,
   0,
   0,
   2,
   2,
   2,
   2,

   1,
   3,
   0,
   3,
   1,
   3,
   0,
   0,
   0,
   2,
   4,
   0,
   2,

   2,
   0,
};

UBYTE iskill_type[] = {
   1,
   1,
   3,
   1,

   1,
   1,
   3,
   1,

   1,
   1,
   3,
   1,
};

void load_skill(UBYTE x)
{
   if(x < 30) {
      memcpy(skl_name, skill_names[x], (UWORD)10);
      skl_cost = skill_cost[x];
      skl_type = skill_type[x];
      skl_gem = 0;
      skl_item = 0;
   }
   // gem skill emulation sort of
   else if(x < 50) {
      x = x - 30;
      memcpy(skl_name, item_info(x + GEMS_INDEX) + 1, (UWORD)10);
      skl_cost = item_info2(x + GEMS_INDEX)[5] & 63;
      if(x == GEM_EMERALD)
         skl_type = 3;
      else if(x == GEM_DIAMOND)
         skl_type = 1;
      else
         while(1);

      skl_gem = x + 1;
      skl_item = 0;
   }
   // item skill emulation sort of
   else {
      x = x - 50;
      memcpy(skl_name, item_info(x + 1) + 1, (UWORD)10);
      skl_cost = 1;
      skl_type = iskill_type[x];
      skl_gem = 0;
      skl_item = x + 1;
   }
}


UBYTE menu_newtile;
UBYTE menu_basetile;
UBYTE menu_curtile;
UBYTE menu_numsize;

void menu_scrollinit(UBYTE a, UBYTE b, UBYTE numsize)
{
   menu_basetile = a;
   menu_newtile = b;
   menu_curtile = menu_basetile;
   menu_numsize = numsize;
}

UBYTE ms_len;
UBYTE ms_numsize;
UWORD ms_total;
void menu_redraw(UBYTE x, UBYTE y, UBYTE deep, UBYTE at, char *s, char *s2)
{
   UBYTE a;
   UBYTE c;
   UBYTE n;

   ms_numsize = menu_numsize;
   ms_len = ms_numsize + 6;

   c = menu_curtile;
   for(n = 0; n != at; ++n) {
      c += ms_len;
      if(c > menu_basetile + ((UWORD)deep * ms_len))
         c = menu_basetile;
   }

   pstr_curtile = c;
   dfs_clear(pstr_curtile, ms_len);

   ipstr(0, 6, s+1);
   ipstr(0, ms_numsize, s2);

   put_tile(x, y + at, *s + 24 - 'A');
}

void menu_scroll(UBYTE dir, UBYTE x, UBYTE x2, UBYTE y, UBYTE entries, char *s, char *s2)
{
   UBYTE n;
   UBYTE c;
   UBYTE i;

   ms_numsize = menu_numsize;
   ms_len = ms_numsize + 6;
   ms_total = (UWORD)entries * ms_len;

   dfs_clear(menu_newtile, ms_len);

   pstr_curtile = menu_newtile;
   ipstr(0, 6, s+1);
   ipstr(0, ms_numsize, s2);

   //pflush();

   c = menu_curtile;
   if(dir == 0) {
      for(i = 0; i < entries; ++i) {
         n = i;

         c += ms_len;
         if(c > menu_basetile + ms_total)
            c = menu_basetile;

         barg1 = x + 1;
         barg2 = y + n;
         barg3 = c;
         barg4 = 6;

         dfs_line();

         barg1 = x2;
         barg2 = y + n;
         barg3 = c + 6;
         barg4 = ms_numsize;

         dfs_line();

         if(n != entries - 1)
            put_tile(x, y + n, get_tile(x, y + n + 1));
         else
            put_tile(x, y + n, *s + 24 - 'A');
      }
   }
   else {
      barg1 = x + 1;
      barg2 = y;
      barg3 = menu_newtile;
      barg4 = 6;

      dfs_line();

      barg1 = x2;
      barg2 = y;
      barg3 = menu_newtile + 6;
      barg4 = ms_numsize;

      dfs_line();

      for(i = 1; i < entries; ++i) {
         n = i;

         if(c > menu_basetile + ms_total)
            c = menu_basetile;

         barg1 = x + 1;
         barg2 = y + n;
         barg3 = c;
         barg4 = 6;

         dfs_line();

         barg1 = x2;
         barg2 = y + n;
         barg3 = c + 6;
         barg4 = ms_numsize;

         dfs_line();

         put_tile(x, y + entries - n, get_tile(x, y + entries - n - 1));

         c += ms_len;
      }

      put_tile(x, y, *s + 24 - 'A');
   }


   if(dir == 0) {
      menu_newtile += ms_len;
      if(menu_newtile > menu_basetile + ms_total)
         menu_newtile = menu_basetile;

      menu_curtile += ms_len;
      if(menu_curtile > menu_basetile + ms_total)
         menu_curtile = menu_basetile;
   }
   else {
      menu_newtile -= ms_len;
      if(menu_newtile < menu_basetile)
         menu_newtile = menu_basetile + ms_total;

      menu_curtile -= ms_len;
      if(menu_curtile < menu_basetile)
         menu_curtile = menu_basetile + ms_total;
   }
}

//void menu_usenew()
//{
// pstr_curtile = menu_curtile;
//}
