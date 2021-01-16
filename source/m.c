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

// the menu!
//
//

#include "defs.h"
#include "vars.h"

// menu vars
BYTE index;
UBYTE cur_x, cur_y;
UBYTE atmenu;
UBYTE stat_selection;
UBYTE menu_ptr;
UBYTE gems_dotrick;
UBYTE dfs_array[17];

#define STATMENU  0
#define ITEMMENU  1
#define EQUIPMENU 2
#define GEMSMENU  3
#define SKILMENU  4
#define SHOPMENU  5

void show_menu(UBYTE num);

void items_reset();
void items_add(UBYTE type);
void items_del(UBYTE type);

void gems_add(UBYTE type, UBYTE charge);

void items_reset()
{
   item_num = 100;

   for(n = 0; n < 100; ++n) {
      item_list_type[n]   = 0;
      item_list_num[n]    = 0;
   }
   for(n = 0; n < 9; ++n) {
      gems_list[n] = 0;
      gems_charge[n] = 0;
   }

   // gem #5 [6] is unused! (Opal)

/* gems_add(12, 99);
   gems_add( 2, 99);
   gems_add( 3, 99);
   gems_add( 4, 99);
   gems_add(11, 99);*/

// gems_add( 7, 99);
// gems_add( 8, 99);
// gems_add( 9, 99);
// gems_add(10, 99);
// gems_add(11, 99);
// gems_add(12, 99);
// gems_add(13, 99);
// gems_add(14, 99);
// gems_add(16, 99);

/* items_add(1);
   items_add(6);

   items_add(1);
   items_add(2);
   items_add(3);
   items_add(4);
   items_add(5);
   items_add(6);
   items_add(7);
   items_add(7);
   items_add(7);

   gems_add(1);
   gems_add(2);
   gems_add(3);
   gems_add(4);
   gems_add(5);
   gems_add(6);
   gems_add(7);
   gems_add(8);
   gems_add(9);*/
}

void gold_give(UWORD amount)
{
   UBYTE ask[3];

   set_24(ask, amount);
   add_24(gold, ask);
}

UBYTE items_findslot(UBYTE type)
{
   UBYTE n;

   for(n = 0; n < item_num; ++n) {
      if(item_list_type[n] == type) {
         return n;
      }
   }

   return 255;
}

UWORD au_tab[] = { 50, 100, 200, 500, 1000, 2000, 5000 };

void items_add(UBYTE type)
{
   UBYTE n, n2;

   if(type == 0)
      return;

   // gold?
   if(type >= GOLD_INDEX) {
      gold_give(au_tab[type - GOLD_INDEX]);
      return;
   }
   if(type >= GEMS_INDEX) {
      gems_add(type-GEMS_INDEX+1, 0);
      return;
   }

   // does the player have the item already?
   n = items_findslot(type);
   if(n != 255) {
      ++item_list_num[n];
      return;
   }

   // search for a new slot
   for(n = 0; n < item_num; ++n) {
      if(item_list_type[n] == 0) {
         break;
      }
   }

   // if there's no free slot, create one at the end
   if(n >= item_num) {
      ++item_num;
   }

   // add the item
   item_list_type[n] = type;
   item_list_num[n] = 1;
}

void items_del(UBYTE type)
{
   UBYTE n, n2;

   if(type == 0)
      return;

   // find the item
   n = items_findslot(type);
   if(n == 255)
      return;

   if(item_list_num[n] > 1) {
      --item_list_num[n];
   }
   else {
      item_list_type[n] = 0;
      item_list_num[n] = 0;
   }
}

UBYTE items_have(UBYTE type)
{
   UBYTE n;

   // find the item
   n = items_findslot(type);
   if(n == 255)
      return 0;
   return (item_list_num[n]);
}

void items_set(UBYTE type, UBYTE value)
{
   UBYTE n;

   n = items_findslot(type);
   if(n == 255)
      return;

   item_list_num[n] = value;
   if(value == 0)
      item_list_type[n] = 0;
}

void gems_add(UBYTE type, UBYTE charge)
{
   UBYTE n;

   for(n = 0; n < 9; ++n) {
      if(!gems_list[n])
         break;
   }
   if(n >= 9)
      return;

   gems_list[n] = type;
   gems_charge[n] = charge;
}

UBYTE gems_inuse(UBYTE x)
{
   UBYTE n;
   struct DUDESTAT *st;

   for(n = 0; n != 4; ++n) {
      st = get_party(n);
      if(st && st->e[3] == x+1)
         return 1;
   }
   return 0;
}

void temp_generate(UBYTE guy, UBYTE type, UBYTE have)
{
   UBYTE n;
   UBYTE guynum;

   guynum = party[guy] - 1;

   if(have) {
      temp_num = 1;
      temp_list_type[0] = 0;
      temp_list_num[0] = 0;
      gems_rel[0] = 0;
   }
   else {
      temp_num = 0;
   }

   if(type == 5) {
      for(n = 0; n != 9; ++n) {
         if(gems_list[n] && !gems_inuse(n)) {
            temp_list_type[temp_num] = gems_list[n]-1 + GEMS_INDEX;
            temp_list_num[temp_num] = gems_charge[n];
            gems_rel[temp_num] = n + 1;
            ++temp_num;
         }
      }
   }
   else {
      for(n = 0; n != item_num; ++n) {
         if(item_info2(item_list_type[n])[0] == type &&
            item_info2(item_list_type[n])[6] & (1<<guynum)) {
            temp_list_type[temp_num] = item_list_type[n];
            temp_list_num[temp_num] = item_list_num[n];
            ++temp_num;

            // safety??
            if(temp_num == 50)
               break;
         }
      }
   }
}

struct DUDESTAT temp_guy;

void stats_recalc(struct DUDESTAT *guy)
{
   UBYTE n;
   BYTE att, def, spd, man;

   att = 1;
   def = 1;
   spd = 0;
   man = 0;

   // calculate speed
   spd += guy->agl;

   for(n = 0; n != 3; ++n) {
      att += item_info2(guy->e[n])[1];
      def += item_info2(guy->e[n])[2];
      spd += item_info2(guy->e[n])[3];
      man += item_info2(guy->e[n])[4];
   }

   if(guy->type == VICTOR_WOLF) {
      n = guy->wolfpow * 3;
      att += n;
      def += n;
   }

   if(att < 0)
      att = 0;
   if(def < 0)
      def = 0;
   if(spd < 0)
      spd = 0;
   if(man < 0)
      man = 0;

   guy->att = att;
   guy->def = def;
   guy->spd = spd;
   guy->man = man;
}


// note to self:
// 0-8 = edges, 4=blank, 19=black
// 20-23 = equip icons
// 24-27 = stat icons
// 28+   = item icons
// 48-55 = new equip icons

unsigned char equip_menu_tiles[] = {
   3, 4, 4, 4, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 5,
   3, 0, 1, 1,13, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 5, 5,
   3, 3, 4, 4, 4,48,49, 4, 4, 4, 4, 4, 4, 4,20, 4, 4, 4, 5, 5,
   3, 3, 4, 4, 4,50,51, 4, 4, 4, 4, 4, 4, 4,21, 4, 4, 4, 5, 5,
   3, 6, 7, 7,10,52,53, 4, 4, 4, 4, 4, 4, 4,22, 4, 4, 4, 5, 5,
   3, 3, 4, 4, 3,54,55, 4, 4, 4, 4, 4, 4, 4,23, 4, 4, 4, 5, 5,
   3, 3, 4, 4, 3, 9, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,10,11, 5, 5,
   3, 6, 7, 7, 3, 5, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 3,15, 5, 5,
   3, 3, 4, 4, 3, 5, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 3,15, 5, 5,
   3, 3, 4, 4, 3, 5, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 3,15, 5, 5,
   3, 6, 7, 7, 3, 5, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 3,15, 5, 5,
   3, 3, 4, 4, 3, 5, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 3,15, 5, 5,
   3, 3, 4, 4, 3,12, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,13,14, 5, 5,
   3, 6, 7, 7, 6, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 8, 5,
   6, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 8,
};

unsigned char gemsmenu[] = {
   3, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 9, 7, 7,10, 4, 5,
   3, 4, 4, 4, 4, 4, 4, 4, 4, 9, 7, 7,10, 4, 5, 4, 4, 3, 4, 5,
   3, 4, 4, 4, 9, 7, 7,10, 4, 5, 4, 4, 3, 4, 5, 4, 4, 3, 4, 5,
   3, 4, 4, 4, 5, 4, 4, 3, 4, 5, 4, 4, 3, 4,12, 1, 1,13, 4, 5,
   3, 4, 4, 4, 5, 4, 4, 3, 4,12, 1, 1,13, 4, 9, 7, 7,10, 4, 5,
   3, 4, 4, 4,12, 1, 1,13, 4, 9, 7, 7,10, 4, 5, 4, 4, 3, 4, 5,
   3, 4, 4, 4, 9, 7, 7,10, 4, 5, 4, 4, 3, 4, 5, 4, 4, 3, 4, 5,
   3, 4, 4, 4, 5, 4, 4, 3, 4, 5, 4, 4, 3, 4,12, 1, 1,13, 4, 5,
   3, 4, 4, 4, 5, 4, 4, 3, 4,12, 1, 1,13, 4, 9, 7, 7,10, 4, 5,
   3, 4, 4, 4,12, 1, 1,13, 4, 9, 7, 7,10, 4, 5, 4, 4, 3, 4, 5,
   3, 4, 4, 4, 9, 7, 7,10, 4, 5, 4, 4, 3, 4, 5, 4, 4, 3, 4, 5,
   3, 4, 4, 4, 5, 4, 4, 3, 4, 5, 4, 4, 3, 4,12, 1, 1,13, 4, 5,
   3, 4, 4, 4, 5, 4, 4, 3, 4,12, 1, 1,13, 4, 4, 4, 4, 4, 4, 5,
   3, 4, 4, 4,12, 1, 1,13, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 5,
   6, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 8,
};

unsigned char skilmenu_top[] = {
   0, 1, 1, 1, 1, 2,
   3, 4, 4, 4, 4, 5,
   3, 4, 4, 4, 4,12,
};

void draw_itemlist(UBYTE start)
{
   UBYTE n;
   char *s;

   for(n = 0; n != 12 && start != item_num; ++n, ++start) {
      put_tile(3, 4 + n, ICON_BASE + item_info(item_list_type[start])[0] - 'A');
      pstr( 4, 4 + n, 6, (char *)item_info(item_list_type[start]) + 1);
      if(!item_list_type[start])
         s = menu_blank;
      else
         s = deci_16((UWORD)item_list_num[start], 3);
      pstr(14, 4 + n, 2, s);
   }
}

void draw_equiplist(UBYTE start, UBYTE e_num)
{
   UBYTE n;
   char *s1, *s2;

   for(n = 0; n != 5 && start != temp_num; ++n, ++start) {
      if(!temp_list_type[start]) {
         s1 = menu_dequip;
         s2 = menu_blank;
      }
      else {
         s1 = (char *)item_info(temp_list_type[start]);
         s2 = deci_16((UWORD)temp_list_num[start], 3);
      }

      put_tile(6, 10 + n, *s1 + 24 - 'A');
      pstr( 7, 10 + n, 6, s1 + 1);
      pstr(14, 10 + n, 2, s2);
   }
}

UBYTE menu_first;

void menu_header()
{
   ppstr(1, 1, 0, 3, "Ability");
   ppstr(5, 1, 2, 5, "Backpack");
   ppstr(11,1, 1, 3, "Equip");
   ppstr(15,1, 6, 4, "Gems");
}

void menu_error()
{
   UBYTE n;

   play_sfx(SFX_MENUERROR);

   for(n = 1; n != 8; ++n) {
      do_delay(1);
      scl_pixelate(n);
   }
   for(n = 6; n != 0; --n) {
      do_delay(1);
      scl_pixelate(n);
   }
}

UBYTE auto_var;
UBYTE auto_animcur;
UBYTE auto_animframe;
UBYTE auto_last;

UBYTE cf_x, cf_y, cf_count, cf_f, cf_active;

void auto_slidein()
{
   UBYTE n;
   struct DUDESTAT *st;

// inputb_handler();


   if(auto_animcur < 4) {
      st = get_party(auto_animcur);
      n = auto_animframe;
      if(st) {
         if(st->hp == 0)
            n = 47;
                        else if(st->hp <= (st->hpmax >> 2))
                                n += 40;
      }
      mgs_sload(auto_animcur + MGS_CH, st->type, DOWN, n);
   }

   if(auto_last) {
      mgs_setwin(0, 0);
      auto_last = 0;
   }

   if(auto_var) {
      auto_var -= 10;
      if(auto_var == 0)
         auto_last = 1;
      mgs_setwin(auto_var, 0);
   }

   ++auto_animcur;
   if(auto_animcur == 24) {
      auto_animframe = auto_animframe ? 0: 1;
      auto_animcur = 0;
   }

   if(cf_active) {
      ++cf_count;
      if(cf_count == 4) {
         cf_count = 0;
         cf_f = cf_f ? 0: 1;

         if(cf_f)
            mgs_sput(1, cf_x, cf_y);
         else
            mgs_shide(1);
      }
   }
}

void curflick_on(UBYTE x, UBYTE y)
{
   cf_x = x;
   cf_y = y;
   cf_f = 0;
   cf_count = 0;
   cf_active = 1;
}

void curflick_off()
{
   cf_active = 0;
   mgs_shide(1);
}

void auto_forceupdate()
{
   auto_animcur = 0;
}


UBYTE newnum[3];

void slide_in_menu(BYTE num)
{
   UBYTE u;
   UBYTE n;
   BYTE i;

   // put the window at far right
   mgs_init();
   mgs_setwin(160, 0);

   start_gradient_full();
   flicker_off();

   //if(num != SHOPMENU)
      menu_drawbox(0, 0, 20, 18);

   // hide the sprites
   for(n = 0; n != MGS_NUM; ++n) {
      mgs_shide(n);
   }
   // make sure dude pal is up
   setdudepal();

   reset_str(0);
   if(num != SHOPMENU)
      menu_header();
   save_str();

   // change to the cursor
   mgs_wload(0, 0, 7);
   mgs_wload(1, 0, 7);
   mgs_wload(2, 1, 7);

   // start the auto window slider
   auto_var = 160;
   auto_animcur = 0;
   auto_animframe = 0;
   auto_last = 0;
   auto_proc(auto_slidein);

   menu_first = 1;
   stat_selection = 0;
   show_menu(num);

   while(auto_var);

   winbkg_on();
}

void slide_out_menu()
{
   UBYTE u;
   UBYTE n;

   auto_proc(0);

   w4vbl_done();
   mgs_setwin(0, 0);
   w4vbl_done();

// owe_dscroll_on();
   winbkg_off();

   // hide the menuwings
   for(n = 0; n != 16; ++n)
      mgs_shide(n);

   // slide the window out
   u = 10;
   while(1) {
      mgs_setwin(u, 0);
      do_delay(1);

      if(u == 160)
         break;
      u += 10;
   }

   mgs_setwin(u, 0);

   flicker_on();
   stop_gradient();
}

UBYTE reshow_stat_tab1[12] = { 68,69,64,65,66,67 };
UBYTE reshow_stat_tab2[6] =  { 16,12, 4, 5,12,13 };

void reshow_stat(UBYTE guy)
{
   struct DUDESTAT *st;
   UBYTE u, u2, n;
   char *s;
   UBYTE tile;

   st = get_party(guy);

   if(st) {
      // update the frame
      mgs_sload(MGS_CH + guy, st->type, DOWN, 0);
   }
   do_delay(4);

   // refresh the guys
   mgs_sput(MGS_CH+0, 12,  40);
   mgs_sput(MGS_CH+1, 12,  64);
   mgs_sput(MGS_CH+2, 12,  88);
   mgs_sput(MGS_CH+3, 12, 112);

   u = guy + (guy << 1) + 5;
   point_str(dfs_array[guy], 19);

   n = st->e[3];
   for(u2 = 0; u2 < 6; ++u2) {
      if(u2 == 2)
         ++u;

      tile = reshow_stat_tab1[u2];
      if(!st)
         tile = 4;
      if(u2 == 0 && !n)
         tile = 4;

      put_tile(reshow_stat_tab2[u2], u, tile);
   }

   if(st) {
      ipstr(0, 5, (char *)st->name);
      ipstr(0, 2, deci_16((UWORD)st->level, 3));
      if(n)
         s = deci_16(gems_charge[n-1], 3);
      else
         s = "";
      ipstr(0, 2, s);

      ipstr(0, 2, deci_16((UWORD)st->hp, 3));
      ipstr(1, 1, "/");
      ipstr(0, 2, deci_16((UWORD)st->hpmax, 3));

      ipstr(0, 2, deci_16((UWORD)st->sp, 3));
      ipstr(1, 1, "/");
      ipstr(0, 2, deci_16((UWORD)st->spmax, 3));
   }
}

void show_menu(UBYTE num)
{
   UBYTE n, n2, u, u2;
   UBYTE *p;
   char *s;

   auto_forceupdate();
   for(n = 0; n != MGS_NUM; ++n) {
      mgs_shide(n);
   }

   if(!menu_first) {
      restore_str();
      save_str();
   }
   menu_first = 0;

   scroll_location = read_str();

   if(num != STATMENU && num != SKILMENU)
      stat_selection = 0;

   if(num == STATMENU) {
      // draw the window
      draw_menu(num);
      menu_clearbox(1, 3, 18, 14);

      // show the sprites
      mgs_sput(MGS_CH+0, 12,  40);
      mgs_sput(MGS_CH+1, 12,  64);
      mgs_sput(MGS_CH+2, 12,  88);
      mgs_sput(MGS_CH+3, 12, 112);

      pstr(11, 3, 4, deci_24(gold, 6));
      pstr(16, 3, 3, "Aurum");

      for(n = 0; n < 4; ++n) {
         u = n * 3;
         dfs_array[n] = read_str();

         st = get_party(n);
         if(!st) {
            u2 = u + 5;
            pstr( 4, u2, 5, "");
            pstr(13, u2, 2, "");
            pstr(17, u2, 2, "");

            ++u2;
            pstr( 6, u2, 2, "");
            pstr( 8, u2, 1, "");
            pstr( 9, u2, 2, "");

            pstr(14, u2, 2, "");
            pstr(16, u2, 1, "");
            pstr(17, u2, 2, "");
         }
         else {
            u2 = u + 5;
            pstr(     4,u2, 5, (char *)st->name);
            put_tile(12,u2, 69);
            pstr(    13,u2, 2, deci_16((UWORD)st->level, 3));

            n2 = st->e[3];
            if(n2) {
               put_tile(16,u2, 68);
               pstr(    17,u2, 2, deci_16(gems_charge[n2-1], 3));
            }
            else
               pstr(    17,u2, 2, "");

            ++u2;
            put_tile( 4,u2, 64);
            put_tile( 5,u2, 65);
            ppstr(    6,u2, 0, 2, deci_16((UWORD)st->hp, 3));
            ppstr(    8,u2, 1, 1, "/");
            ppstr(    9,u2, 0, 2, deci_16((UWORD)st->hpmax, 3));

            put_tile(12,u2, 66);
            put_tile(13,u2, 67);
            ppstr(   14,u2, 0, 2, deci_16((UWORD)st->sp, 3));
            ppstr(   16,u2, 1, 1, "/");
            ppstr(   17,u2, 0, 2, deci_16((UWORD)st->spmax, 3));
         }
      }
   }
   else if(num == ITEMMENU) {
      draw_menu(num);

      draw_itemlist(0);
   }
   else if(num == EQUIPMENU) {
      draw_menu(num);
      restore_tiles(0, 3, 20, 15, equip_menu_tiles);

      // show the sprites
      mgs_sput(MGS_CH+0, 16,  40);
      mgs_sput(MGS_CH+1, 16,  64);
      mgs_sput(MGS_CH+2, 16,  88);
      mgs_sput(MGS_CH+3, 16, 112);
   }
   else if(num == GEMSMENU) {
      draw_menu(num);
      restore_tiles(0, 3, 20, 15, gemsmenu);
   }
   else if(num == SKILMENU) {
      backup_tiles(0, 0, 20, 3, menu_backup);

      menu_clearbox(1, 3, 18, 14);
      restore_tiles(0, 0, 6, 3, skilmenu_top);


      // show the one guy
      mgs_sput(MGS_CH+stat_selection, 16, 6);

      n = stat_selection;
      st = get_party(n);

      // show the skills
      for(n = 0; n != 4; ++n) {
         if(st->skl[n])
            n2 = st->skl[n] - 1 + 20;
         else
            n2 = 12;
         mgs_wload(MGS_EX+n, n2, 6);
      }
      mgs_sput(MGS_EX+0, 120, 88);
      mgs_sput(MGS_EX+1, 104,104);
      mgs_sput(MGS_EX+2, 136,104);
      mgs_sput(MGS_EX+3, 120,120);

      // the gem
      n = st->e[3];
      if(n) {
         mgs_gload(MGS_EX+4, gems_list[n-1] - 1);
         mgs_sput(MGS_EX+4,  80, 44);
      }

      ppstr(1, 4, 0, 5, (char *)st->name);
      ppstr(8, 4, 0, 5, (char *)st->class);

      ppstr(1, 5, 4, 2, "HP");
      ppstr(3, 5, 0, 2, deci_16(st->hp, 3));
      ppstr(5, 5, 1, 1, "/");
      ppstr(6, 5, 0, 2, deci_16(st->hpmax, 3));

      ppstr(1, 6, 4, 2, "SP");
      ppstr(3, 6, 0, 2, deci_16(st->sp, 3));
      ppstr(5, 6, 1, 1, "/");
      ppstr(6, 6, 0, 2, deci_16(st->spmax, 3));

      ppstr(14, 4, 0, 2, "Lv");
      ppstr(16, 4, 0, 2, deci_16((UWORD)st->level, 3));

      ppstr(14, 6, 0, 2, "EXP");
      ppstr(15, 7, 0, 4, deci_24(st->exp, 6));
      if(st->level < 100) {
         ppstr(14, 8, 0, 3, "Next");
         ppstr(15, 9, 0, 4, deci_24(f_get_expnext(st->level), 6));
      }

      ppstr(1, 8, 0, 5, "Strength");
      ppstr(9, 8, 0, 2, deci_16((UWORD)st->str, 3));
      ppstr(1, 9, 0, 5, "Endurance");
      ppstr(9, 9, 0, 2, deci_16((UWORD)st->end, 3));
      ppstr(1,10, 0, 3, "Agility");
      ppstr(9,10, 0, 2, deci_16((UWORD)st->agl, 3));
      ppstr(1,11, 0, 4, "Wisdom");
      ppstr(9,11, 0, 2, deci_16((UWORD)st->wis, 3));

      menu_ptr = read_str();

      pstr(1,13, 4, "Attack");
      pstr(9,13, 2, deci_16((UWORD)st->att, 3));
      pstr(1,14, 4, "Defense");
      pstr(9,14, 2, deci_16((UWORD)st->def, 3));
      pstr(1,15, 5, "Speed");
      pstr(9,15, 2, deci_16((UWORD)st->spd, 3));
      pstr(1,16, 5, "Mana");
      pstr(9,16, 2, deci_16((UWORD)st->man, 3));
   }
   else if(num == SHOPMENU) {
      f_draw_shopmenu();
   }

   //pflush();
}


BYTE do_menu(BYTE num)
{
   BYTE n;

   n = -1;

   if(num == STATMENU) {
      n = do_statmenu();
   }
   else if(num == 1) {
      n = do_itemmenu();
   }
   else if(num == EQUIPMENU) {
      n = do_equipmenu();
   }
   else if(num == 3) {
      n = do_gemsmenu();
   }
   else if(num == 4) {
      n = do_skilmenu();
   }

   return n;
}

BYTE do_statmenu()
{
   UBYTE n;
   WORD c;
   UBYTE flick;
   UBYTE hold, hold_index;
   BYTE at;
   UBYTE index;

   // menu code
   index = stat_selection;
   hold = 0;
   flick = 0;

   while(1) {
      // sync up
      do_delay(2);

      ++flick;
      flick &= 3;

      if(hold && (flick&2)) {
         cur_x = 8;
         cur_y = hold_index * 24 + 40 + 8;
         mgs_sput(1, cur_x, cur_y);
      }
      else {
         mgs_shide(1);
      }

      // place cursor
      cur_x = 4;
      cur_y = index * 24 + 40 + 8;
      mgs_sput(0, cur_x, cur_y);

      // get input
      c = inputb();
      if(c == CANCEL)
         break;
      if(c == MENU)
         return MENU;
      if(c == UP && index > 0) {
         index -= 1;
      }
      if(c == DOWN && index < 3) {
         index += 1;
      }
      if(c == LEFT && !hold) {
         return LEFT;
      }
      if(c == RIGHT && !hold) {
         return RIGHT;
      }

      if(key[ENTER]) {
         if(!hold) {
            hold = 1;
            hold_index = index;
         }
      }
      else {
         if(hold) {
            mgs_shide(1);

            hold = 0;
            if(hold_index == index) {
               if(get_party(index)) {
                  stat_selection = index;
                  play_sfx(SFX_MENUACCEPT);
                  return ENTER;
               }
               else {
                  menu_error();
               }
            }
            else {
               mgs_shide(MGS_CH + hold_index);
               mgs_shide(MGS_CH + index);

               n = party[hold_index];
               party[hold_index] = party[index];
               party[index] = n;

               auto_animcur = 0;
               reshow_stat(hold_index);
               reshow_stat(index);
            }
         }
      }
   }
   return CANCEL;
}

UBYTE next_indexup(UBYTE cur)
{
   BYTE x;

   for(x = cur - 1; x >= 0; --x) {
      if(get_party(x))
         return x;
   }
   return cur;
}

UBYTE next_indexdown(UBYTE cur)
{
   BYTE x;

   for(x = cur + 1; x <= 3; ++x) {
      if(get_party(x))
         return x;
   }
   return cur;
}

UBYTE gemcost, gemnum;
// mode: 0 = item
//       1+ = spell (caster + 1)
//       64+ = gem (caster + 64)
//       bit7 = all
BYTE do_useitem(UBYTE mode, UBYTE type)
{
   UBYTE n, n2;
   UBYTE x, y;
   UBYTE index;
   BYTE c;
   UBYTE num;
   UBYTE ok;
   UBYTE all, allflash[4];
   UBYTE usemode;
        UBYTE ondead;

   index = next_indexdown((UBYTE)-1);

   all = 0;
        ondead = 0;
   if(mode & 128) {
      all = 1;
      mode &= 127;

      for(n = 0; n != 4; ++n) {
         if(get_party(n))
            allflash[n] = 1;
         else
            allflash[n] = 0;
      }
   }

   menu_drawbox(1, 3, 18, 14);
   if(mode == 0) {
      num = items_have(type);
      usemode = 0;
                if(item_info2(type)[2])
                        ondead = 1;
   }
   else if(mode < 64) {
      f_load_skill(type);
      num = skl_cost;
      usemode = 1;
                if(type == 11)
                        ondead = 1;
   }
   else {
      gemnum = type;
      type = gems_list[type] - 1;
      gemcost = item_info2(type + GEMS_INDEX)[5];
      gemcost &= 63;
      num = gems_charge[gemnum];
      usemode = 2;
   }

   restore_str();
   save_str();

   pstr(2, 4, 2, "Use:");
   if(mode == 0) {
      pstr(5, 4, 6, (char *)item_info(type) + 1);
      put_tile(4, 4, item_info(type)[0] + 24 - 'A');
   }
   else if(mode < 64) {
      pstr(5, 4, 6, (char *)skl_name);
      pstr(13, 4, 2, "SP");
   }
   else {
      pstr(5, 4, 6, (char *)item_info(type + GEMS_INDEX) + 1);
      put_tile(4, 4, item_info(type + GEMS_INDEX)[0] + 24 - 'A');
      pstr(13, 4, 2, "GP");
   }

   dfs_array[16] = read_str();
   pstr(11, 4, 2, deci_16((UWORD)num, 3));

   ok = 1;
   for(n = 0; n < 4; ++n) {
      st = get_party(n);
      if(!st)
         continue;

      y = 5 + (n * 3);
      mgs_sput(MGS_CH+n, 24, 40 + (n * 24));

      ppstr(14,   y, 1, 1, "/");
      ppstr(14, y+1, 1, 1, "/");
      ppstr(10,   y, 5, 2, "HP");
      ppstr(10, y+1, 5, 2, "SP");

      ppstr( 5, y+1, 0, 5, (char *)st->name);

      n2 = n << 2;
      dfs_array[n2] = read_str();
      ppstr(12,   y, 0, 2, deci_16((UWORD)st->hp, 3));
      dfs_array[n2+1] = read_str();
      ppstr(15,   y, 0, 2, deci_16((UWORD)st->hpmax, 3));
      dfs_array[n2+2] = read_str();
      ppstr(12, y+1, 0, 2, deci_16((UWORD)st->sp, 3));
      dfs_array[n2+3] = read_str();
      ppstr(15, y+1, 0, 2, deci_16((UWORD)st->spmax, 3));
   }

   do {
      if(!all) {
         while(1) {
            // slow down everything
            do_delay(2);

            // place cursor
            cur_x = 16;
            cur_y = index * 24 + 40 + 8;
            mgs_sput(0, cur_x, cur_y);

            // get input
            c = inputb();
            if(c == CANCEL)
               return CANCEL;
            if(c == MENU)
               return MENU;
            if(c == UP && index > 0) {
               index = next_indexup(index);
            }
            if(c == DOWN && index < 3) {
               index = next_indexdown(index);
            }
            if(c == ENTER) {
               st = get_party(index);
               if(st->hp == 0 && !ondead)
                  menu_error();
               else {
                  //play_sfx(SFX_ACCEPT);
                  break;
               }
            }
         }
      }
      else {
         n = 0;
         while(1) {
            do_delay(2);

            cur_x = 16;
            if(allflash[n])
               cur_y = n * 24 + 40 + 8;
            else
               cur_y = (UBYTE)-16;
            mgs_sput(0, cur_x, cur_y);

            c = inputb();
            if(c == CANCEL)
               return CANCEL;
            if(c == MENU)
               return MENU;
            if(c == ENTER) {
               //play_sfx(SFX_ACCEPT);
               break;
            }

            ++n;
            if(n == 4)
               n = 0;
         }
      }

      if(f_do_effect(mode, type, index)) {
         play_sfx(SFX_SPARKLE);
         if(mode == 0) {
            num = items_have(type);
            if(num == 0)
               ok = 0;
         }
         else if(mode < 64) {
            st = get_party(mode - 1);
            if(st->sp < skl_cost)
               ok = 0;
         }
         else {
            num = gems_charge[gemnum];
            if(gems_charge[gemnum] < gemcost)
               ok = 0;
         }

         if(!all) {
                                // reshow hp
            st = get_party(index);
            n = index << 2;
            point_str(dfs_array[n], 2);
            ipstr(0, 2, deci_16((UWORD)st->hp, 3));

                                // reshow sp
            point_str(dfs_array[n+2], 2);
            ipstr(0, 2, deci_16((UWORD)st->sp, 3));
         }
         else {
            for(n2 = 0; n2 != 4; ++n2) {
               st = get_party(n2);
               if(!st)
                  continue;
               n = n2 << 2;
               point_str(dfs_array[n], 2);
               ipstr(0, 2, deci_16((UWORD)st->hp, 3));
            }
         }

         // if it was an item, show the decrease in item number
         if(mode == 0) {
            point_str(dfs_array[16], 2);
            ipstr(0, 2, deci_16((UWORD)num, 3));
         }
         // if it was a skill, reshow caster's SP
         else if(mode < 64) {
            st = get_party(mode - 1);
            n = (mode - 1) << 2;
            point_str(dfs_array[n+2], 2);
            ipstr(0, 2, deci_16((UWORD)st->sp, 3));
         }
         // if it was a gem, show the decrease in gem charge
         else {
            point_str(dfs_array[16], 2);
            ipstr(0, 2, deci_16((UWORD)num, 3));
         }
      }
      else {
         //mgs_shide(1);
         menu_error();
      }
   } while(ok);

   return CANCEL;
}

BYTE do_itemmenu()
{
   BYTE c;
   UBYTE n, n2;
   UBYTE u;
   BYTE a;
   BYTE scroll, at, at2;
   UBYTE bary;
   UWORD x;
   UBYTE flick;
   UBYTE hold;
   UBYTE hold_index;
   UBYTE count;
   UBYTE scrolled;
   UBYTE tmp_type;
   UBYTE tmp_num;
   char *s;


   index = 0;
   scroll = 0;
   flick = 0;
   hold = 0;
   count = 0;
   scrolled = 0;

   f_menu_scrollinit(scroll_location, scroll_location + 96, 2);

   while(1) {
      // sync up
      if(!scrolled)
         do_delay(2);
      scrolled = 0;

      at = index - scroll;
      ++flick;
      flick &= 3;

      cur_x = 12;
      cur_y = at * 8 + 32 + 4;
      mgs_sput(0, cur_x, cur_y);

      if(item_num < 2) {
         bary = 0;
      }
      else {
         x = (UWORD)88 * index / (item_num-1);
         bary = (UBYTE)x;
      }

      mgs_sput(2, 144, bary + 32);

      at2 = hold_index - scroll;
      if(hold && (flick&2) && at2 >= 0 && at2 <= 11) {
         cur_x = 16;
         cur_y = at2 * 8 + 32 + 4;
         mgs_sput(1, cur_x, cur_y);
      }
      else {
         mgs_shide(1);
      }

//    ++count;
//    if(count < 2)
//       continue;
//    count = 0;

      c = inputb();
      if(c == LEFT && !hold)
         return LEFT;
      if(c == RIGHT && !hold)
         return RIGHT;
      if(c == CANCEL)
         break;
      if(c == MENU)
         return MENU;
      if(c == UP && index > 0) {
         index -= 1;

         if(at == 0) {
            if(!item_list_type[index])
               s = menu_blank;
            else
               s = deci_16((UWORD)item_list_num[index], 3);

            f_menu_scroll(1, 3, 14, 4, 12,
               (char *)item_info(item_list_type[index]), s);

            scrolled = 1;

            --scroll;
         }
      }

      if(c == DOWN && index < item_num - 1) {
         index += 1;

         if(at == 11) {
            if(!item_list_type[index])
               s = menu_blank;
            else
               s = deci_16((UWORD)item_list_num[index], 3);

            f_menu_scroll(0, 3, 14, 4, 12,
               (char *)item_info(item_list_type[index]), s);

            scrolled = 1;

            ++scroll;
         }
      }

      at = index - scroll;

      if(key[ENTER]) {
         if(!hold) {
            hold = 1;
            hold_index = index;
         }
      }
      if(!key[ENTER]) {
         if(hold) {
            hold = 0;

            if(index != hold_index) {
               n = index;
               n2 = hold_index;

               // swap the items
               tmp_type    = item_list_type[n];
               tmp_num     = item_list_num[n];
               item_list_type[n] = item_list_type[n2];
               item_list_num[n]  = item_list_num[n2];
               item_list_type[n2]   = tmp_type;
               item_list_num[n2] = tmp_num;

               if(!item_list_type[n])
                  s = menu_blank;
               else
                  s = deci_16((UWORD)item_list_num[n], 3);
               if(at >= 0 && at <= 11)
                  f_menu_redraw(3, 4, 12, at, (char *)item_info(item_list_type[n]), s);


               if(!item_list_type[n2])
                  s = menu_blank;
               else
                  s = deci_16((UWORD)item_list_num[n2], 3);
               if(at2 >= 0 && at2 <= 11)
                  f_menu_redraw(3, 4, 12, at2, (char *)item_info(item_list_type[n2]), s);
            }
            else if(item_info2(item_list_type[index])[0] == 1) {
               play_sfx(SFX_MENUACCEPT);
               for(n = 0; n != MGS_NUM; ++n)
                  mgs_shide(n);
               restore_str();
               save_str();

                                        n = 0;
                                        if(item_info2(item_list_type[index])[1] == 1)
                                                n |= 128;

               a = do_useitem(n, item_list_type[index]);
               if(a == MENU)
                  return MENU;
               for(n = 0; n != MGS_NUM; ++n)
                  mgs_shide(n);

               restore_str();
               save_str();

               draw_menu(ITEMMENU);
               draw_itemlist(index - at);

               f_menu_scrollinit(scroll_location, scroll_location + 96, 2);
            }
            else if(item_list_type[index]) {
               mgs_shide(1);
               menu_error();
            }
         }
      }
   }
   return CANCEL;
}

UBYTE equip_tab_data[4][52] = {
   {
      0, 1, 1,13,
      3, 4, 4, 4,
      3, 4, 4, 4,
      6, 7, 7,10,

      3, 4, 4, 3,
      3, 4, 4, 3,
      6, 7, 7, 3,

      3, 4, 4, 3,
      3, 4, 4, 3,
      6, 7, 7, 3,

      3, 4, 4, 3,
      3, 4, 4, 3,
      6, 7, 7, 6,
   },

   {
      0, 1, 1, 3,
      3, 4, 4, 3,
      3, 4, 4, 3,
      0, 1, 1,13,

      3, 4, 4, 4,
      3, 4, 4, 4,
      6, 7, 7,10,

      3, 4, 4, 3,
      3, 4, 4, 3,
      6, 7, 7, 3,

      3, 4, 4, 3,
      3, 4, 4, 3,
      6, 7, 7, 6,
   },

   {
      0, 1, 1, 3,
      3, 4, 4, 3,
      3, 4, 4, 3,
      0, 1, 1, 3,

      3, 4, 4, 3,
      3, 4, 4, 3,
      0, 1, 1,13,

      3, 4, 4, 4,
      3, 4, 4, 4,
      6, 7, 7,10,

      3, 4, 4, 3,
      3, 4, 4, 3,
      6, 7, 7, 6,
   },

   {
      0, 1, 1, 3,
      3, 4, 4, 3,
      3, 4, 4, 3,
      0, 1, 1, 3,

      3, 4, 4, 3,
      3, 4, 4, 3,
      0, 1, 1, 3,

      3, 4, 4, 3,
      3, 4, 4, 3,
      0, 1, 1,13,

      3, 4, 4, 4,
      3, 4, 4, 4,
      6, 7, 7, 7,
   },
};

void show_equip_tab(UBYTE x)
{
   restore_tiles(1, 4, 4, 13, (unsigned char *)equip_tab_data[x]);
}


void show_equip_items(UBYTE x)
{
   UBYTE n, i;

   st = get_party(x);

   pstr(6, 4, 6, (char *)st->name);

   for(n = 0; n != 4; ++n) {
      dfs_array[n] = read_str();
      i = st->e[n];
      if(i) {
         if(n == 3) {
            i = gems_list[i-1]-1 + GEMS_INDEX;
         }

         put_tile(7, 5+n, ICON_BASE + item_info(i)[0] - 'A');
         pstr(8, 5+n, 6, (char *)item_info(i) + 1);
      }
      else
         pstr(8, 5+n, 6, "");
   }
}

void show_equip_stats(UBYTE x)
{
   struct DUDESTAT *st;
   st = get_party(x);

   dfs_array[4] = read_str();
   pstr(15, 5, 2, deci_16((UWORD)st->att, 3));
   dfs_array[5] = read_str();
   pstr(15, 6, 2, deci_16((UWORD)st->def, 3));
   dfs_array[6] = read_str();
   pstr(15, 7, 2, deci_16((UWORD)st->spd, 3));
   dfs_array[7] = read_str();
   pstr(15, 8, 2, deci_16((UWORD)st->man, 3));
}

void reshow_equip_stats(struct DUDESTAT *guy)
{
   UBYTE lastpos;

   lastpos = read_str();

   point_str(dfs_array[4], 2);
   ipstr(0, 2, deci_16((UWORD)guy->att, 3));
   point_str(dfs_array[5], 2);
   ipstr(0, 2, deci_16((UWORD)guy->def, 3));
   point_str(dfs_array[6], 2);
   ipstr(0, 2, deci_16((UWORD)guy->spd, 3));
   point_str(dfs_array[7], 2);
   ipstr(0, 2, deci_16((UWORD)guy->man, 3));

   point_str(lastpos, 0);
}

void show_equip_arrows()
{
   if(temp_guy.att > st->att)
      put_tile(17, 5, 17);
   else if(temp_guy.att < st->att)
      put_tile(17, 5, 18);
   else
      put_tile(17, 5, 4);

   if(temp_guy.def > st->def)
      put_tile(17, 6, 17);
   else if(temp_guy.def < st->def)
      put_tile(17, 6, 18);
   else
      put_tile(17, 6, 4);

   if(temp_guy.spd > st->spd)
      put_tile(17, 7, 17);
   else if(temp_guy.spd < st->spd)
      put_tile(17, 7, 18);
   else
      put_tile(17, 7, 4);

   if(temp_guy.man > st->man)
      put_tile(17, 8, 17);
   else if(temp_guy.man < st->man)
      put_tile(17, 8, 18);
   else
      put_tile(17, 8, 4);
}

BYTE do_equipmenu_sel(UBYTE guy, UBYTE e_num)
{
   UBYTE index, scrolled;
   BYTE at, scroll;
   BYTE c, r;
   char *s1, *s2;
   UBYTE bary;
   UWORD x;
   UBYTE n, n2, old_item, new_item;
   UBYTE oindex;
   struct DUDESTAT *st;

   scroll_location = read_str();

   draw_equiplist(0, e_num);

   f_menu_scrollinit(scroll_location, scroll_location + 40, 2);
   st = get_party(guy);

   index = 0;
   oindex = !index;
   scroll = 0;

   for(;;) {
      // sync up
      do_delay(2);

      at = index - scroll;

      cur_x = 38;
      cur_y = at * 8 + 84;
      mgs_sput(0, cur_x, cur_y);


      if(temp_num < 2) {
         bary = 0;
      }
      else {
         x = (UWORD)32 * index / (temp_num-1);
         bary = (UBYTE)x;
      }

      mgs_sput(2, 136, bary + 80);

      if(oindex != index) {
         memcpy(&temp_guy, st, (UWORD)sizeof(struct DUDESTAT));
         temp_guy.e[e_num] = temp_list_type[index];
         stats_recalc(&temp_guy);
         reshow_equip_stats(&temp_guy);
         show_equip_arrows();
      }

      oindex = index;

      scrolled = 0;
      c = inputb();
      if(c == CANCEL)
         break;
      if(c == MENU)
         return MENU;
      if(c == UP && index > 0) {
         index -= 1;

         if(at == 0) {
            if(!temp_list_type[index]) {
               s1 = menu_dequip;
               s2 = menu_blank;
            }
            else {
               s1 = (char *)item_info(temp_list_type[index]);
               s2 = deci_16((UWORD)temp_list_num[index], 3);
            }

            f_menu_scroll(1, 6, 14, 10, 5, s1, s2);
            scrolled = 1;
            --scroll;
         }
      }
      if(c == DOWN && index < temp_num - 1) {
         index += 1;

         if(at == 4) {
            if(!temp_list_type[index]) {
               s1 = menu_dequip;
               s2 = menu_blank;
            }
            else {
               s1 = (char *)item_info(temp_list_type[index]);
               s2 = deci_16((UWORD)temp_list_num[index], 3);
            }

            f_menu_scroll(0, 6, 14, 10, 5, s1, s2);
            scrolled = 1;
            ++scroll;
         }
      }
      if(c == ENTER) {
                        // if it's an empty gem then error
                        if(e_num == 3 && temp_list_num[index] == 0 && temp_list_type[index]) {
                                play_sfx(SFX_MENUERROR);
                                continue;
                        }

         play_sfx(SFX_MENUACCEPT);
         new_item = temp_list_type[index];

         if(e_num != 3) {
            old_item = st->e[e_num];
            st->e[e_num] = new_item;

            items_del(new_item);
            items_add(old_item);
         }
         else {
                                // any non-party PC using this gem?  dequip it!
                                for(n = 0; n < 6; ++n) {
                                        if(stats[n].e[3] == gems_rel[index])
                                                stats[n].e[3] = 0;
                                }

                                // equip the gem
            st->e[e_num] = gems_rel[index];
         }

         point_str(dfs_array[e_num], 6);
         ipstr(0, 6, (char *)item_info(new_item) + 1);
         put_tile(7, 5+e_num, ICON_BASE + item_info(new_item)[0] - 'A');

         stats_recalc(st);

         return ENTER;
      }
   }

   return CANCEL;
}

BYTE do_equipmenu_cur(UBYTE guy)
{
   UBYTE index;
   BYTE c;
   UBYTE n, n2;
   BYTE a;
   BYTE r;
   UBYTE have_item;
   struct DUDESTAT *st;

   index = 0;

   while(1) {
      // slow down everything
      do_delay(2);

      cur_x = 28;
      cur_y = index * 8 + 44;
      mgs_sput(0, cur_x, cur_y);

      c = inputb();
      if(c == CANCEL)
         break;
      if(c == MENU)
         return MENU;
      if(c == UP && index > 0) {
         index -= 1;
      }
      if(c == DOWN && index < 3) {
         index += 1;
      }
      if(c == ENTER) {
         st = get_party(guy);
         if(st->e[index])
            have_item = 1;
         else
            have_item = 0;

         if(st->type == VICTOR_WOLF && index < 3) {
            menu_error();
            continue;
         }

         temp_generate(guy, index+2, have_item);
         if(temp_num) {
            play_sfx(SFX_MENUACCEPT);
            save_str();
            curflick_on(cur_x+4, cur_y);
            r = do_equipmenu_sel(guy, index);

            reshow_equip_stats(st);
            curflick_off();

            // erase equip selection window
            menu_clearbox(6, 10, 10, 5);

            // erase any arrows
            put_tile(17, 5, 4);
            put_tile(17, 6, 4);
            put_tile(17, 7, 4);
            put_tile(17, 8, 4);

            // erase the scrollbar
            mgs_shide(2);

            if(r == MENU)
               return MENU;

            restore_str();

            // erase item icons
            for(n = 0; n != 5; ++n)
               put_tile(6, 10+n, 4);
         }
         else {
            menu_error();
         }
      }
   }
   return CANCEL;
}

BYTE do_equipmenu()
{
   UBYTE n;
   WORD c;
   UBYTE last;
   BYTE r;
   UBYTE index;

   // menu code
   index = next_indexdown((UBYTE)-1);
   last = 99;

   while(1) {
      // slow down everything
      do_delay(2);

      // place cursor
      cur_x = 8;
      cur_y = index * 24 + 40 + 8;
      mgs_sput(0, cur_x, cur_y);

      // refresh the equipped items
      if(index != last) {
         show_equip_tab(index);

         put_tile(7, 5, 4);
         put_tile(7, 6, 4);
         put_tile(7, 7, 4);
         put_tile(7, 8, 4);

         restore_str();
         save_str();
         show_equip_items(index);
         show_equip_stats(index);

         //pflush();
      }
      last = index;

      // get input
      c = inputb();
      if(c == CANCEL)
         break;
      if(c == MENU)
         return MENU;
      if(c == UP && index > 0) {
         index = next_indexup(index);
      }
      if(c == DOWN && index < 3) {
         index = next_indexdown(index);
      }
      if(c == LEFT) {
         return LEFT;
      }
      if(c == RIGHT) {
         return RIGHT;
      }
      if(c == ENTER) {
         play_sfx(SFX_MENUACCEPT);
         r = do_equipmenu_cur(index);
         if(r == MENU)
            return MENU;
      }
   }
   return CANCEL;
}

BYTE do_skilmenu()
{
   UBYTE n;
   BYTE c;
   UBYTE index, oindex;
   char *p;
   BYTE r;
   struct DUDESTAT *st;

   index = 0;
   oindex = index;
   st = get_party(stat_selection);

   while(1) {
      // sync up
      do_delay(2);

      switch(index) {
         case 0:  cur_x = 160;   cur_y = 144;   break;
         case 1:  cur_x = 128;   cur_y =  96;   break;
         case 2:  cur_x = 112;   cur_y = 112;   break;
         case 3:  cur_x = 144;   cur_y = 112;   break;
         case 4:  cur_x = 128;   cur_y = 128;   break;
      }
      mgs_sput(0, cur_x - 16, cur_y);

      // selected something?
      if(index != oindex) {
         point_str(menu_ptr, 0);
         clear_str();
         menu_drawbox(1, 13, 11, 4);
         n = st->skl[index - 1];
         if(n) {
            f_load_skill(n - 1);
            pstr( 2,14, 5, (char *)skl_name);
            pstr( 7,14, 2, deci_16((UWORD)skl_cost, 2));
            pstr( 9,14, 2, "SP");
            pstr( 2,15, 3, "Type:");
            if(skl_type == 0)
               p = "Enemy";
            else if(skl_type == 1)
               p = "Ally";
            else if(skl_type == 2)
               p = "Enemy All";
            else if(skl_type == 3)
               p = "Ally All";
                                else
                                        p = "Self";
            pstr( 5,15, 5, p);
         }
      }

      oindex = index;
      // get input
      c = inputb();
      if(c == CANCEL)
         break;
      if(c == MENU)
         return MENU;
      if(c == UP)
         index = 1;
      if(c == DOWN)
         index = 4;
      if(c == LEFT)
         index = 2;
      if(c == RIGHT)
         index = 3;
      if(c == ENTER && index) {
         f_load_skill(st->skl[index - 1] - 1);
         if(st->sp < skl_cost || st->hp == 0)
            menu_error();
         else {
            // single ally
            if(skl_type == 1) {
               play_sfx(SFX_MENUACCEPT);

               for(n = 0; n != MGS_NUM; ++n)
                  mgs_shide(n);
               restore_str();
               save_str();

               restore_tiles(0, 0, 20, 3, menu_backup);

               r = do_useitem(1 + stat_selection, st->skl[index - 1] - 1);
               if(r == MENU)
                  return MENU;
               restore_str();
               save_str();

               show_menu(SKILMENU);
            }
            //// ally all
            //else if(skl_type == 3) {
            //}
            else
               menu_error();
         }
      }
      if(index != oindex) {
         if(!st->skl[index - 1])
            index = oindex;
      }
   }
   restore_tiles(0, 0, 20, 3, menu_backup);

   return CANCEL;
}

void real_menu()
{
   atmenu = 0;
   gems_dotrick = 1;
   slide_in_menu(atmenu);

   while(1) {
      s = do_menu(atmenu);
      if(s == RIGHT) {
         if(atmenu == 0 || atmenu == 1 || atmenu == 2)
            ++atmenu;
         else
            atmenu = 0;
      }
      else if(s == LEFT) {
         if(atmenu == 1 || atmenu == 2 || atmenu == 3)
            --atmenu;
         else
            atmenu = 3;
      }
      else if(s == CANCEL) {
         if(atmenu == 4)
            atmenu = 0;
         else
            break;
      }
      else if(s == MENU) {
         break;
      }
      else if(s == ENTER) {
         if(atmenu == 0)
            atmenu = 4;
      }
      show_menu(atmenu);
   }

   slide_out_menu();
}
