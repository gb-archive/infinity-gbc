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

// battle page 4
#include"defs.h"
#include"vars.h"

BYTE tab_slurptab[64] = {
   48,48,47,46,46,44,43,42,40,38,37,35,32,30,28,
   25,23,20,17,14,12,9,6,3,0,-1,-4,-7,-9,-12,-14,
   -16,-18,-20,-22,-24,-25,-27,-28,-29,-30,-30,-31,-31,-31,-31,-30,
   -30,-29,-28,-27,-26,-24,-22,-21,-19,-17,-14,-12,-10,-7,-4,-2,0
};

UBYTE tab_jolttab[12][3] = {
   { 1, 0, 0 },
   { 1, 2, 0 },
   { 1, 2, 3 },
   { 0, 2, 3 },
   { 1, 0, 0 },
   { 1, 2, 0 },
   { 1, 2, 3 },
   { 1, 2, 4 },
   { 1, 2, 3 },
   { 0, 2, 3 },
   { 0, 0, 3 },
   { 0, 0, 0 },
};

BYTE tab_spiral_rel[32][4] = {
  { 32,  0,-32,  0},
  { 28, 11,-28,-11},
  { 21, 21,-21,-21},
  { 11, 26,-11,-26},
  {  0, 28,  0,-28},
  {-10, 25, 10,-25},
  {-18, 18, 18,-18},
  {-23,  9, 23, -9},
  {-24,  0, 24,  0},
  {-21, -8, 21,  8},
  {-15,-15, 15, 15},
  { -8,-19,  8, 19},
  {  0,-20,  0, 20},
  {  7,-17, -7, 17},
  { 12,-12,-12, 12},
  { 15, -6,-15,  6},
  { 16,  0,-16,  0},
  { 13,  5,-13, -5},
  { 10, 10,-10,-10},
  {  5, 12, -5,-12},
  {  0, 12,  0,-12},
  { -4, 10,  4,-10},
  { -7,  7,  7, -7},
  { -8,  3,  8, -3},
  { -8,  0,  8,  0},
  { -6, -2,  6,  2},
  { -4, -4,  4,  4},
  { -2, -4,  2,  4},
  {  0, -4,  0,  4},
  {  1, -2, -1,  2},
  {  1, -1, -1,  1},
  {  1,  0, -1,  0},
};

BYTE tab_panel_rel[6][32][2] = {
{
// 0,0 to 0,-32
{0,-4},
{0,-7},
{0,-10},
{0,-12},
{0,-14},
{0,-16},
{0,-18},
{0,-19},
{0,-21},
{0,-22},
{0,-23},
{0,-24},
{0,-25},
{0,-26},
{0,-26},
{0,-27},
{0,-28},
{0,-28},
{0,-29},
{0,-29},
{0,-29},
{0,-30},
{0,-30},
{0,-30},
{0,-31},
{0,-31},
{0,-31},
{0,-31},
{0,-31},
{0,-31},
{0,-31},
{0,-32},
},
{
// 0,0 to -28,-16
{-3,-2},
{-6,-4},
{-8,-5},
{-11,-6},
{-12,-7},
{-14,-8},
{-16,-9},
{-17,-10},
{-18,-11},
{-19,-11},
{-20,-12},
{-21,-12},
{-22,-13},
{-23,-13},
{-23,-13},
{-24,-14},
{-24,-14},
{-25,-14},
{-25,-15},
{-25,-15},
{-26,-15},
{-26,-15},
{-26,-15},
{-27,-15},
{-27,-16},
{-27,-16},
{-27,-16},
{-27,-16},
{-27,-16},
{-28,-16},
{-28,-16},
{-28,-16},
},
{
// 0,0 to -28,16
{-3,2},
{-6,4},
{-8,5},
{-11,6},
{-12,7},
{-14,8},
{-16,9},
{-17,10},
{-18,11},
{-19,11},
{-20,12},
{-21,12},
{-22,13},
{-23,13},
{-23,13},
{-24,14},
{-24,14},
{-25,14},
{-25,15},
{-25,15},
{-26,15},
{-26,15},
{-26,15},
{-27,15},
{-27,16},
{-27,16},
{-27,16},
{-27,16},
{-27,16},
{-28,16},
{-28,16},
{-28,16},
},
{
// 0,0 to 0,32
{0,4},
{0,7},
{0,10},
{0,12},
{0,14},
{0,16},
{0,18},
{0,19},
{0,21},
{0,22},
{0,23},
{0,24},
{0,25},
{0,26},
{0,26},
{0,27},
{0,28},
{0,28},
{0,29},
{0,29},
{0,29},
{0,30},
{0,30},
{0,30},
{0,31},
{0,31},
{0,31},
{0,31},
{0,31},
{0,31},
{0,31},
{0,32},
},
{
// 0,0 to 28,16
{3,2},
{6,4},
{8,5},
{11,6},
{12,7},
{14,8},
{16,9},
{17,10},
{18,11},
{19,11},
{20,12},
{21,12},
{22,13},
{23,13},
{23,13},
{24,14},
{24,14},
{25,14},
{25,15},
{25,15},
{26,15},
{26,15},
{26,15},
{27,15},
{27,16},
{27,16},
{27,16},
{27,16},
{27,16},
{28,16},
{28,16},
{28,16},
},
{
// 0,0 to 28,-16
{3,-2},
{6,-4},
{8,-5},
{11,-6},
{12,-7},
{14,-8},
{16,-9},
{17,-10},
{18,-11},
{19,-11},
{20,-12},
{21,-12},
{22,-13},
{23,-13},
{23,-13},
{24,-14},
{24,-14},
{25,-14},
{25,-15},
{25,-15},
{26,-15},
{26,-15},
{26,-15},
{27,-15},
{27,-16},
{27,-16},
{27,-16},
{27,-16},
{27,-16},
{28,-16},
{28,-16},
{28,-16},
},
};


void reset_data()
{
   UBYTE n, n2;
   struct DUDESTAT *st;

   set_24(gold, 0);

   // Connor
   st = &stats[0];
   strcpy(st->name, "Connor");
   strcpy(st->class, "Hero");
   st->type = 0;
   st->level = 1;
   set_24(st->exp, 0);
   st->hpmax = 40;
   st->spmax = 20;
   st->str =  6;
   st->end =  4;
   st->agl =  5;
   st->wis =  7;
   st->e[0] =  0;  // (he is given conscript in event code)
   st->e[1] = IT_HTunic;
   st->e[2] = IT_ICloak;
   st->e[3] = 0;
   st->skl[0] = 1; // Engulf
   st->skl[1] = 0;
   st->skl[2] = 0;
   st->skl[3] = 0;
   f_stats_recalc(st);
   st->hp = st->hpmax;
   st->sp = st->spmax;

   // Ryan
   st = &stats[1];
   strcpy(st->name, "Ryan");
   strcpy(st->class, "Guide");
   st->type = 1;
   st->level = 1;
   set_24(st->exp, 0);
   st->hpmax = 30;
   st->spmax = 13;
   st->str =   5;
   st->end =   3;
   st->agl =   8;
   st->wis =   6;
   st->e[0] = IT_DBasic;
   st->e[1] = IT_HLeather;
   st->e[2] = 0;
   st->e[3] = 0;
   st->skl[0] = 16; // Longshot
   st->skl[1] = 3;  // Tidal
   st->skl[2] = 0;
   st->skl[3] = 0;
   f_stats_recalc(st);
   st->hp = st->hpmax;
   st->sp = st->spmax;

   // Elya
   st = &stats[2];
   strcpy(st->name, "Elya");
   strcpy(st->class, "Wizard");
   st->type = 2;
   st->level = 1;
   set_24(st->exp, 0);
   st->hpmax = 22;
   st->spmax = 30;
   st->str =   3;
   st->end =   3;
   st->agl =   2;
   st->wis =   10;
   st->e[0] = IT_EOak;
   st->e[1] = IT_PApprentice;
   st->e[2] = IT_KWizard;
   st->e[3] = 0;
   st->skl[0] =  8; // Embrace
   st->skl[1] =  3; // Tidal
   st->skl[2] =  0;
   st->skl[3] =  0;
   f_stats_recalc(st);
   st->hp = st->hpmax;
   st->sp = st->spmax;

   // Victor
   st = &stats[3];
   strcpy(st->name, "Victor");
   strcpy(st->class, "Wolf");
   st->type = VICTOR_WOLF;
   st->level = 1;
   set_24(st->exp, 0);
   st->wolfpow = 0;
   st->hpmax = 70;
   st->spmax = 10;
   st->str =   8;
   st->end =   5;
   st->agl =   9;
   st->wis =   3;
   st->e[0] = IT_FWolf;
   st->e[1] = IT_HWolf;
   st->e[2] =  0;
   st->e[3] =  0;
   st->skl[0] = 15; // Leap
   st->skl[1] =  0;
   st->skl[2] =  0;
   st->skl[3] =  0;
   f_stats_recalc(st);
   st->hp = st->hpmax;
   st->sp = st->spmax;

   // Roland
   st = &stats[4];
   strcpy(st->name, "Roland");
   strcpy(st->class, "Knight");
   st->type = 4;
   st->level = 1;
   set_24(st->exp, 0);
   st->hpmax = 50;
   st->spmax = 10;
   st->str =   9;
   st->end =   7;
   st->agl =   3;
   st->wis =   4;
   st->e[0] =  IT_OBastard;
   st->e[1] =  IT_HPlate;
   st->e[2] =  0;
   st->e[3] =  0;
   st->skl[0] =  2; // jolt
   st->skl[1] =  19; // slash
   st->skl[2] =  0;
   st->skl[3] =  0;
   f_stats_recalc(st);
   st->hp = st->hpmax;
   st->sp = st->spmax;

   // Alutha
   st = &stats[5];
   strcpy(st->name, "Alutha");
   strcpy(st->class, "Hero");
   st->type = ALUTHA;
   st->level = 1;
   set_24(st->exp, 0);
   st->hpmax = 80;
   st->spmax = 20;
   st->str =   9;
   st->end =   7;
   st->agl =   4;
   st->wis =   6;
   st->e[0] =  0;
   st->e[1] =  0;
   st->e[2] =  0;
   st->e[3] =  0;
   st->skl[0] =  0;
   st->skl[1] =  0;
   st->skl[2] =  0;
   st->skl[3] =  0;
   f_stats_recalc(st);
   st->hp = st->hpmax;
   st->sp = st->spmax;

   party[0] = 0;
   party[1] = 0;
   party[2] = 0;
   party[3] = 0;
   area = 0;
   lastsaveslot = 3;
   for(n = 0; n != 96; ++n)
      vars[n] = 0;
   for(n = 0; n != 8; ++n)
      regs[n] = 0;
   for(n = 0; n != 32; ++n)
      flags[n] = 0;
   disable_interrupts();
   gametime[0] = 0;
   gametime[1] = 0;
   gametime[2] = 0;
   gametime[3] = 0;
   enable_interrupts();

   options[0] = 0;
   memcpy(options + 1, defaultcolors, 7);

   boat_x = 0;
   boat_y = 0;
   boat_dir = 0;
   on_boat = 0;
}


extern UBYTE pal_bak[];
extern UBYTE pal_cur[];
extern UWORD pal[];
extern UWORD window_ptr;

void colorshift(UBYTE color)
{
   UBYTE n;

   pfs_targetspr = 0;
   pfs_setup(pal, (UWORD *)color, 32);
   pfs_targetbgr = 0;
   for(n = 0; n != 16; ++n)
      fade_step();
   memcpy(pal, pal_cur, 64);
   pfs_targetspr = 252;
   pfs_targetbgr = 254;
}


UBYTE pro_sprites[] = {
  0x03,0x00,0x0F,0x00,0x1F,0x06,0x3F,0x0C,
  0x7F,0x29,0x7F,0x4F,0xFF,0x1F,0xFF,0x0E,
  0xFE,0x3B,0xFE,0x1F,0x78,0x1F,0x7C,0x1F,
  0x3C,0x0F,0x1C,0x07,0x0E,0x03,0x03,0x01,
  0xC0,0x00,0xF0,0x00,0xF8,0x10,0xFC,0x48,
  0xFE,0x14,0xFE,0x00,0xE7,0x5A,0xC0,0xBF,
  0x00,0xFF,0x80,0xFF,0xC0,0xFE,0x00,0xFE,
  0x00,0xFC,0x00,0xF8,0x00,0xF0,0x00,0xC0,
  0x00,0x00,0x00,0x00,0x08,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x20,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x08,0x00,0x00,
  0x00,0x00,0x00,0x40,0x02,0x02,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,
};

UWORD pro_pal[] = {
   0x0000,0x5AD6,0x2108,0x520F,
   0x0D02,0x7BBD,0x42B0,0x737A,
};
UWORD pro_pal2[] = {
   0x0000,0x1084,0x0000,0x7FFF,
   0x0D02,0x1DC9,0x0D02,0x7FFF,
};

UBYTE pro_dma[] = {
   32,  8,4,0x86,
   32, 56,4,0x86,
   80, 56,4,0x86,
   64,104,4,0x86,
   56, 40,6,0x86,
   88, 24,6,0x86,
   40, 88,6,0x86,
   72,120,6,0x86,
   40,152,6,0x86,
   72,128,0,0x86,
   72,136,2,0x86
};

void pro_str(UBYTE tile, char *s)
{
   UBYTE n;

   n = 0;
   while(*s) {
      n = dfs_pschar(tile, n, *s);
      ++s;
   }
}

char str[4][4][20] = {
   { "It was an age of", "suffering.", "", "" },
   { "War followed", "plague, and death", "stalked the land.", "" },
   { "Far in the north", "lay the hope for", "peace,", "" },
   { "And yet deep in", "the Earth,", "something dark was", "stirring..." },
};

UBYTE pro_wait[4] = { 128, 152, 152, 176 };

void prophecy()
{
   UBYTE n, n2, at;

   owe_off();
   scl_init();

   window_ptr = 0x8000;
   for(n = 0; n != 127; ++n) {
      put_tile(n, 0, pro_sprites[n]);
   }
   put_tile(n, 0, 0);
   pfs_targetspr = 2;
   pfs_targetbgr = 0;
   pfs_setpal(pro_pal - 56 + (_gba ? 4:0));
   pfs_targetspr = 0;
   pfs_targetbgr = 1;
   pfs_setpal(pro_pal2 - 28 + (_gba ? 4:0));
   pfs_targetspr = 252;
   pfs_targetbgr = 254;
   memcpy((void *)0xC000, pro_dma, 44);

   window_ptr = 0x9800;
   // blank
   for(n2 = 0; n2 != 13; ++n2) {
      for(n = 0; n != 20; ++n) {
         put_tile(n + 1, n2 + 18, 254);
      }
   }
   VBK_REG = 1;
   for(n2 = 0; n2 != 13; ++n2) {
      for(n = 0; n != 20; ++n) {
         put_tile(n + 1, n2 + 18, 7);
      }
   }
   VBK_REG = 0;
   for(n2 = 0; n2 != 4; ++n2) {
      for(n = 0; n != 13; ++n) {
         put_tile(2 + n, 19 + n2 * 3, 128 + (n2 * 26) + (n << 1));
      }
      for(n = 0; n != 13; ++n) {
         put_tile(2 + n, 20 + n2 * 3, 129 + (n2 * 26) + (n << 1));
      }
   }

   for(at = 0; at != 4; ++at) {
      dfs_clear(128, 128);

      pro_str(128, str[at][0]);
      pro_str(154, str[at][1]);
      pro_str(180, str[at][2]);
      pro_str(206, str[at][3]);

      w4vbl_done();
      for(n = 0; n < pro_wait[at]; ++n) {
         scl_prophecy(n);
         do_delay(4);
      }
      scl_prophecy(0);
   }
   window_ptr = 0x9C00;

   scl_off();
   init_game();

   owe_on();
}

extern UWORD bdata_gold, bdata_exp;
//UBYTE bvicstr[40];
UBYTE schlong[3];

// note: this is a duplicate of the method in m.c
UBYTE b_gems_inuse(UBYTE x)
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

void bvictory()
{
   UBYTE k, k2;
   UBYTE i;
   UBYTE n, n2;

   // room for the item still?
   if(bwon_item) {
      if(f_items_have(bwon_item) > 99)
         bwon_item = 0;
      else
         f_items_add(bwon_item);
   }

   k2 = 0;
   if(bwon_gp) {
      for(n = 0; n < 9; ++n) {
         if(gems_list[n] && !b_gems_inuse(n)) {
            if(gems_charge[n] >= 99)
               continue;

            // bwon_gp is guaranteed to not exceed 99
            gems_charge[n] += bwon_gp;
            if(gems_charge[n] > 99)
               gems_charge[n] = 99;

            k2 = 1; // flag that at least one gem got charged
         }
      }
   }
   if(!k2)
      bwon_gp = 0;


   k = 0;   // counter
   i = 0;   // window
   k2 = 0; // ok to press key

   // victory
   while(1) {
      if(k == 4) {
         if(i == 0) {
            strcpy(bvicstr, "Gained ");
            strcat(bvicstr, (char *)lead((UBYTE *)deci_16(bdata_exp, 5)));
            strcat(bvicstr, " experience points!");
            pstr(1, 1, 18, (char *)bvicstr);

            strcpy(bvicstr, "Discovered ");
            strcat(bvicstr, (char *)lead((UBYTE *)deci_16(bdata_gold, 5)));
            strcat(bvicstr, " aurum!");
            pstr(1, 2, 18, (char *)bvicstr);

            set_24(schlong, bdata_exp);
            for(n = 0; n != 4; ++n) {
               st = get_party(n);
               if(!st)
                  continue;
               if(st->hp == 0)
                  continue;
               if(st->level == 100)
                  continue;
               add_24(st->exp, schlong);
            }

                                set_24(schlong, bdata_gold);
                                add_24(gold, schlong);
                                if(cmp_24(gold, mostgold) > 0) {
                                        set_24(gold, 0);
                                        add_24(gold, mostgold);
                                }

            i = 1;

            bin_popup = 32;
            f_bautoproc();
            k = 0;   // reset counter
            k2 = 1;  // ok to hit key
         }
         else if(i <= 4) {
            st = get_party(i-1);
            if(st) {
               if(f_gain_level(st)) {
                  strcpy(bvicstr, (char *)st->name);
                  strcat(bvicstr, " has reached level ");
                  strcat(bvicstr, (char *)lead((UBYTE *)deci_16(st->level,2)));
                  strcat(bvicstr, "!");
                  pstr( 1, 1, 16, (char *)bvicstr);
                  f_stats_recalc(st);

                  bspr_scrollto(bch[i-1].rx, bch[i-1].ry);

                  for(n = 0; n != 4; ++n) {
                     put_tile((n<<2) + 1,2,56+(n<<1));
                     put_tile((n<<2) + 2,2,57+(n<<1));
                  }

                  strcpy(bvicstr, "+ ");
                  bvicstr[1] = gl_str + 0x30;
                  pstr( 3, 2, 2, (char *)bvicstr);
                  bvicstr[1] = gl_end + 0x30;
                  pstr( 7, 2, 2, (char *)bvicstr);
                  bvicstr[1] = gl_agl + 0x30;
                  pstr(11, 2, 2, (char *)bvicstr);
                  bvicstr[1] = gl_wis + 0x30;
                  pstr(15, 2, 2, (char *)bvicstr);

                  play_sfx(SFX_LEVELUP);
                  bin_popup = 32;
                  f_bautoproc();
                  k = 0;   // reset counter
                  k2 = 1;  // ok to hit key
               }
               else
                  ++i;
            }
            else
               i = 5;
         }
         else if(i == 5) {
            if(bwon_item) {
               strcpy(bvicstr, "Found ");
               pstr( 1, 1, 16, (char *)bvicstr);

               n = item_info(bwon_item)[0] - 'A';
               n2 = 4;
               if(n) {
                  put_tile(n2, 1, 24 + n);
                  ++n2;
               }
               pstr(n2, 1, 6, (char *)item_info(bwon_item) + 1);

               play_sfx(SFX_SEARCH);
               bin_popup = 32;
               f_bautoproc();
               k = 0;  // reset counter
               k2 = 1; // ok to hit key
            }
            ++i;
         }
         else if(i == 6) {
            if(bwon_gp) {
               strcpy(bvicstr, "Unused gems gained ");
               strcat(bvicstr, (char *)lead((UBYTE *)deci_16(bwon_gp,2)));
               strcat(bvicstr, " points");
               pstr( 1, 1, 16, (char *)bvicstr);

               play_sfx(SFX_STAFF);
               bin_popup = 32;
               f_bautoproc();
               k = 0;
               k2 = 1;
            }
            ++i;
         }
         else
            break;
      }

      input();
      if(k2 && bin_popup == 0) {
         if(key[ENTER] || key[CANCEL]) {
            play_sfx(SFX_MENUCURSOR);
            // move window offscreen
            for(n = 4; n <= 32; n += 4) {
               w4vbl_done();
               setwindow(0, 112+n);
            }
            w4vbl_done();
            openbattlewindow();
            reset_str(0);

            k2 = 0;
         }
      }

      for(n = 0; n != bnumdudes; ++n) {
         if(!bch[n].active)
            continue;

         if(bch[n].st.hp == 0)
            continue;

         // add sprite base
         if(flop)
            curframe = 44;
         else
            curframe = 0;

         curframe += (UWORD)bch[n].type << 6;
         bspr_16set(n, bch[n].rx, bch[n].ry, bch[n].rz, curframe, -1);
      }
      w4vbl_done();

      ++icon_count;
      if(icon_count == 16) {
         icon_wiggle = 1 - icon_wiggle;
         flop = 1 - flop;
         icon_count = 0;
         if(!k2)
            ++k;
      }
   }
}

#include"bcur.c"

extern struct DUDESTRUCT ch[];
void loaddude(UBYTE slot, UBYTE xpos, UBYTE ypos, UBYTE type, UBYTE dir, UBYTE control)
{
   ch[slot].active = 1;

   if(control == BOAT) {
      // boat is terms of 8x8
      ch[slot].x = xpos;
      ch[slot].y = ypos;
   }
   else {
      ch[slot].x = xpos << 1; // x2
      ch[slot].y = ypos << 1;
   }

   ch[slot].type = type;

   ch[slot].gx = (WORD)ch[slot].x << 3; // x16
   ch[slot].gy = (WORD)ch[slot].y << 3;
   ch[slot].gz = 0;

   ch[slot].frame = 0;
   ch[slot].dir = dir;
   ch[slot].mode = NORM;
   ch[slot].req = NORM;
   ch[slot].count = 0;

   ch[slot].control = control;
   ch[slot].framemode = 0;
        ch[slot].animmode = 0;
}

UBYTE b4_finddir(WORD x1, WORD y1, WORD x2, WORD y2)
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

#define ABS(x) ((x)<0?-(x):(x))

UBYTE aitab[6];
UBYTE pick;

BYTE aireq;
BYTE b_ai(UBYTE guy, UBYTE style)
{
   UBYTE n, n2;
   UBYTE near;
   WORD rx, ry;
   WORD dist;
   WORD trx, try, tdist;
   UBYTE dir, odir;
   UBYTE k, k2;
   UBYTE r;
   UBYTE ok;

   aireq = -1;

   // approach style
   if(style == 0) {
      // build a random 6 direction table
      pick = 63;
      for(k = 0; k < 6; ++k) {
         n = (UBYTE)randfunc();
         for(k2 = 0;; ++k2) {
            r = (n + k2) % 6;
            if(pick & (1 << r))
               break;
         }
         aitab[k] = r;
         pick ^= 1 << r;
      }

      // next to a player?
      for(k = 0; k != 6; ++k) {
         f_get_dxdy(bch[guy].x, bch[guy].y, aitab[k]);
         for(k2 = 0; k2 != bnumdudes; ++k2) {
            if(!bch[k2].active)
               continue;
            if(turn == k2)
               continue;
            if(bch[k2].x == dx && bch[k2].y == dy && bch[k2].control != bch[guy].control) {
               if(bch[k2].st.hp > 0) {
                  bch[guy].target = k2;
                  break;
               }
            }
         }
         if(k2 < bnumdudes)
            break;
      }
      if(k < 6) {
         aireq = aitab[k] + 6;
         return aireq;
      }
      /*else {
         aireq = ((UBYTE)randfunc()&127) % 6;
         f_get_dxdy(bch[guy].x, bch[guy].y, aireq);
         //return aireq;
      }
      return aireq;*/

      // locate nearest player
      near = 0;
      dist = 20000;
      rx = bch[guy].rx >> 2;
      ry = bch[guy].ry >> 2;
      for(n = 0; n < 4; ++n) {
         if(!bch[n].active)
            continue;

         if(bch[n].control != bch[guy].control && bch[n].st.hp > 0) {
            trx = bch[n].rx >> 2;
            try = bch[n].ry >> 2;
            trx = rx - trx;
            try = rx - try;
            trx *= trx;
            try *= try;
            tdist = trx + try;

            if(tdist < dist) {
               dist = tdist;
               near = n;
            }
         }
      }

      // find the direction to move
      r = (UBYTE)randfunc() & 1; // left or right first?
      odir = b4_finddir(bch[near].rx, bch[near].ry, bch[guy].rx, bch[guy].ry);
      dir = odir;
      for(n = 0; n != 3; ++n) {
         ok = 1;
         f_get_dxdy(bch[guy].x, bch[guy].y, dir);

         // make sure no one is in the way
         for(n2 = 0; n2 != bnumdudes; ++n2) {
            if(!bch[n2].active)
               continue;
            if(n2 == turn)
               continue;
            if(bch[n2].x == dx && bch[n2].y == dy && bch[n2].st.hp > 0)
               break;
         }
         if(n2 < bnumdudes)
            ok = 0;

         // make sure there are no obstacles in the map
         if(!(!bmap[dy][dx] && dy >= 0 && dy < 9 && dx >= 0 && dx < 11))
            ok = 0;

         if(ok)
            break;

         if(r == 0)
            dir = (odir + 5) % 6;
         else
            dir = (odir + 1) % 6;
         r ^= 1;
      }
      if(n == 3)
         return 99;  // turn skip

      aireq = dir;
      return aireq;
   }
   // skill style
   else {
      f_load_skill(style-1);
      if(skl_type == 0) {
                        // embrace
                        if(style-1 == 7) {
                                // find enemy with lowest hp
                                ry = 100;
                                n2 = guy; // default to caster
                                for(n = 0; n != bnumdudes; ++n) {
                                        if(!bch[n].active || bch[n].control == bch[guy].control)
                                                continue;
                                        rx = bch[n].st.hp * 100 / bch[n].st.hpmax;
                                        if(rx < ry) {
                                                n2 = n;
                                                ry = rx;
                                        }
                                }
                        }
                        else {
                                do {
                                        n2 = randfunc() % bchnum;
                                } while(!bch[n2].active || bch[n2].st.hp == 0);
                        }
                        bch[guy].target = n2;
      }
      else if(skl_type == 1) {
         do {
            n2 = randfunc() % bennum;
         } while(!bch[n2+bchnum].active || bch[n2+bchnum].st.hp == 0);
         bch[guy].target = n2+bchnum;
      }
      else if(skl_type == 2) {
         // spawn
         if(style-1 == 20) {
            // see if slot 4-6 is inactive
            for(n = 0; n != 3; ++n) {
               n2 = bchnum + n;
               if(!bch[n2].active)
                  break;
            }
            if(n == 3)
               return -1; // do something else
         }
      }

      aireq = style + (16 - 1);
   }

   return aireq;
}

BYTE b_ai_charm(UBYTE guy)
{
   UBYTE aireq;

   aireq = randfunc()%6;
   return aireq;
}

static BYTE mrel[8] = {
        0,-1,
        0, 1,
       -1, 0,
        1, 0
};

UBYTE touch_guy(UBYTE x, UBYTE n, BYTE dir)
{
/*   BYTE rx, ry;

   rx = mrel[(dir<<1)];
   ry = mrel[(dir<<1)+1];

   if(ch[x].x+rx   == ch[n].x || ch[x].x+rx   == ch[n].x+1
   || ch[x].x+rx+1 == ch[n].x || ch[x].x+rx+1 == ch[n].x+1) {
      if(ch[x].y+ry   == ch[n].y || ch[x].y+ry   == ch[n].y+1
      || ch[x].y+ry+1 == ch[n].y || ch[x].y+ry+1 == ch[n].y+1)
         return 1;
   }*/

   BYTE tx, ty;
   tx = ch[x].x + mrel[(dir<<1)];
   ty = ch[x].y + mrel[(dir<<1)+1];

   return tx+1 >= ch[n].x && tx <= ch[n].x+1
       && ty+1 >= ch[n].y && ty <= ch[n].y+1;
}

void get_dxdy(BYTE x, BYTE y, UBYTE req)
{
   UBYTE even;

   even = ~x & 1;

   if(req == 0) {
      dx = x;
      dy = y - 1;
   }
   else if(req == 1) {
      dx = x + 1;
      if(!even)
         dy = y - 1;
      else
         dy = y;
   }
   else if(req == 2) {
      dx = x + 1;
      if(!even)
         dy = y;
      else
         dy = y + 1;
   }
   else if(req == 3) {
      dx = x;
      dy = y + 1;
   }
   else if(req == 4) {
      dx = x - 1;
      if(!even)
         dy = y;
      else
         dy = y + 1;
   }
   else if(req == 5) {
      dx = x - 1;
      if(!even)
         dy = y - 1;
      else
         dy = y;
   }
}

UBYTE randbattle(UBYTE *info)
{
   UBYTE n, priority, x, at, num;
   UBYTE total, form;

   if(info[0] == 0)
      return 0;

   // tally up the priorities
   num = info[0];
   total = 0;
   for(n = 0; n != num; ++n)
      total += info[(n << 1) + 2];
   x = (UBYTE)randfunc() % total;

   // locate the battle
   at = 0;
   for(n = 0; n != num; ++n) {
      form     = info[(n << 1) + 1];
      priority = info[(n << 1) + 2];

      if(x >= at && x < at + priority)
         return form+1;

      at += priority;
   }
   return 0;
}

UBYTE brun_check()
{
/* UWORD bw, bw2, bw3;
   UBYTE n, n2, c;

   // tally up both team's agl
   bw = bw2 = 0;
   n = n2 = 0;
   for(c = 0; c != bnumdudes; ++c) {
      if(!bch[c].active || bch[c].st.hp == 0)
         continue;
      if(bch[c].control == BPLAYER) {
         bw += bch[c].st.agl;
         ++n;
      }
      else {
         bw2 += bch[c].st.agl;
         ++n2;
      }
   }
   bw /= n;
   bw2 /= n2;
   bw3 = bw * 40 / bw2;

   return 1;

   if(randfunc() % 101 > bw3)
      return 1;
*/

        UBYTE lev1, lev2;
        UBYTE live;
        UBYTE n;

        lev1 = bch[turn].st.level;

        // find highest enemy level, and number if live party members
        live = 0;
        lev2 = 0;
        for(n = 0; n < bnumdudes; ++n) {
                if(!bch[n].active || bch[n].st.hp == 0)
                        continue;

                if(bch[n].control == BCPU) {
                        if(bch[n].st.level > lev2)
                                lev2 = bch[n].st.level;
                }
                else {
                        ++live;
                }
        }

        if(lev1 >= lev2 + 10)
                return 1;

        brun_counter += 15 - (live << 1);
        if(brun_counter > 100)
                brun_counter = 100;

        if(randfunc() % 101 > brun_counter)
                return 1;

        return 0;
}

UBYTE heal(UBYTE guy, WORD amount)
{
   WORD hp, hpmax;

   st = get_party(guy);
   if(!st)
      return 0;

   hp = st->hp;
   if(hp == 0)
      return 0;
   hpmax = st->hpmax;
   if(hp == hpmax)
      return 0;

   hp += amount;

   if(hp > hpmax)
      hp = hpmax;

   st->hp = hp;

   return 1;
}

UBYTE healsp(UBYTE guy, WORD amount)
{
   WORD sp, spmax;

   st = get_party(guy);
   if(!st)
      return 0;

   sp = st->sp;
   spmax = st->spmax;
   if(sp == spmax)
      return 0;

   sp += amount;

   if(sp > spmax)
      sp = spmax;

   st->sp = sp;

   return 1;
}

UBYTE revive(UBYTE guy, UBYTE v)
{
        WORD w;

        st = get_party(guy);
        if(!st)
                return 0;
        if(st->hp)
                return 0;

        if(v == 0)
                w = 1;
        else if(v == 1)
                w = st->hpmax / 10;
        else
                w = st->hpmax / 4;

        st->hp = w;
        return 1;
}

WORD b4_hplo[3]  = { 40, 100, 200 };
WORD b4_splo[3]  = { 15,  30,  50 };
WORD b4_hpall[3] = { 40, 100, 200 };

extern UBYTE gemcost, gemnum;
UBYTE do_effect(UBYTE mode, UBYTE type, UBYTE guy)
{
   UBYTE n, n2, v, basetype;
        WORD w;
        struct DUDESTAT *caster;

   if(mode == 0) {
                basetype = type;

                --basetype;
                v = basetype / 4;
                basetype &= 0x03;
                ++basetype;

                if(basetype == IT_HPLo) {
                        if(!heal(guy, b4_hplo[v]))
                                return 0;
                }
                else if(basetype == IT_SPLo) {
                        if(!healsp(guy, b4_splo[v]))
                                return 0;
                }
                else if(basetype == IT_HALo) {
                        n2 = 1;
                        for(n = 0; n != 4; ++n) {
                                if(heal(n, b4_hpall[v]))
                                        n2 = 0;
                        }
                        if(n2)
                                return 0;
                }
                else if(basetype == IT_LFLo) {
                        if(!revive(guy, v))
                                return 0;
                }

      f_items_del(type);
   }
   else if(mode < 64) {
      caster = get_party(mode - 1);

      // embrace
      if(type == 7) {
                        w = caster->wis+1 + caster->man+1;
                        w += w >> 1;
                        w -= 10;
                        if(w <= 0)
                                w = 1;

         if(!heal(guy, w))
            return 0;
      }
                // wake
      else if(type == 11) {
         st = get_party(guy);
         if(!st)
            return 0;
         if(st->hp)
            return 0;
         st->hp = 1;
      }

      caster->sp -= skl_cost;
   }
   else {
      // emerald
      if(type == GEM_EMERALD) {
                        n2 = 0;
                        for(n = 0; n != 4; ++n) {
                                if(!heal(n, 40))
                                        n2 = 1;
                        }
                        if(n2)
                                return 0;
      }
      if(type == GEM_DIAMOND) {
                        if(!revive(guy, 2))
                                return 0;
      }

      gems_charge[gemnum] -= gemcost;
   }

   return 1;
}

extern UBYTE scroll_x;
void map_restate()
{
   UBYTE n;
   UBYTE realx, realy;
   UBYTE offset;

   offset = map_state << 6;
        scroll_x = (scroll_x&31) + (offset >> 1);
   for(n = 0; n < 4; ++n) {
      ch[n].x = (ch[n].x&63) + offset;
      ch[n].gx = (UWORD)ch[n].x << 3;
   }
}

/*
(Level*2+(Str*1.5)+Weapon - TargetEnd+TargetDef), variable by up to 25% up or down?

Weaknesses double damage, strengths halve damage

i.e. fire on a fireweak target does 2x, fire on a fire strong does 1/2

I think the current magic damage eq. is actually pretty good, strangely enough. You have my speed eq.

Running right now seems too good. :) Maybe reduce it by 50%?

new: (Level+Str+Weapon) - (2/3End+Armor)

*/

void bcalcmod();

UWORD bwepdamage(UBYTE guy, UBYTE target)
{
   WORD bw, bw2, bw3;
   WORD b_damage;

   bw = bch[guy].st.level + bch[guy].st.str + bch[guy].st.att;
   bw2 = bch[target].st.end * 2 / 3;
   bw2 += bch[target].st.def;
   b_damage = bw - bw2;
   if(b_damage < 1)
      b_damage = 1;

   bw = b_damage / 8;
   if(bw == 0)
      bw = 1;
   bw2 = b_damage - bw;
   bw3 = b_damage + bw;
   bw = bw3 - bw2;

   bcm_damage = b_damage;
   bcm_guy = guy;
   bcm_target = target;
   bcalcmod();
   b_damage = bcm_damage;

   b_damage = (randword() % bw) + bw2;
   if(b_damage < 1)
      b_damage = 1;
   if(b_damage > 999)
      b_damage = 999;

   return b_damage;
}


BYTE bmod;
void bmodify(UBYTE x)
{
   if(x == BWEAK)
      ++bmod;
   if(x == BSTRONG)
      --bmod;
}

void bcalcmod()
{
   UBYTE n, flag, wepattr;
   BYTE d;
   WORD bw;

   bmod = 0;

   // weapon attack?
   if(b_attrib == BATR_HAND) {
      if(bch[bcm_guy].control == BCPU)
         return;

      n = get_party(bcm_guy)->e[0];
      if(n) {
         wepattr = item_info2(n-1)[5];

         // do weapon attribute
         bmodify(bch[bcm_target].st.attrib[0]);
         // do others
         for(n = flag = 1; n < 4; ++n, flag <<= 1) {
            if(wepattr & flag)
               bmodify(bch[bcm_target].st.attrib[n]);
         }
      }
   }
   // skill?
   else {
      n = 0;

      if(b_attrib == BATR_FIRE)
         n = 1;
      else if(b_attrib == BATR_WATER)
         n = 2;
      else if(b_attrib == BATR_WIND)
         n = 3;

      if(n)
         bmodify(bch[bcm_target].st.attrib[n]);
   }

   // apply mod
   d = bmod > 0 ? 1: -1;
   bw = bcm_damage * d;
   while(bmod) {
      bmod -= d;
      bw /= 2;
      bcm_damage += bw;
   }
}

UBYTE bflux;
void bdamageflux()
{
   WORD bw, bw2;

   bw = b_damage / 10;
   bw2 = bw * 2;
   bw = b_damage - bw;

   if(bw2)
      b_damage += randword() % bw2;
}

void bdamagecap()
{
   if(b_damage < 1)
      b_damage = 1;
   if(b_damage > 999)
      b_damage = 999;
}


// bitem_getitem: returns index to the correct item and strength, based on box chosen
UBYTE bitem_getitem(UBYTE box)
{
        if(bitem_tab[box+8])
                return box+9;
        if(bitem_tab[box+4])
                return box+5;
        if(bitem_tab[box])
                return box+1;

        return 0;
}

// BITEM functions
//  bitem_item is the input, and will always be 0-3, representing the 4 boxes

UBYTE bitem_have()
{
        UBYTE item;

        item = bitem_getitem(bitem_item);
        if(item == 0)
                return 0;
        --item;

        return bitem_tab[item];
}

UBYTE bitem_type()
{
        UBYTE item;

        item = bitem_getitem(bitem_item);
        if(item == 0)
                return 0;
        --item;

   return item;
}

void bitem_del()
{
        UBYTE item;

        item = bitem_getitem(bitem_item);
        if(item == 0)
                return;
        --item;

        if(bitem_tab[item])
                --bitem_tab[item];
}


extern WORD sx, sy;
extern UBYTE evebattle;

UBYTE battlestart(UBYTE formation)
{
   UBYTE n, n2, old_map;
   WORD old_sx, old_sy;
   UBYTE bret;

   //copy_page(1,3);
   //debug_on = 1;

   bformation = formation;

   owe_scrollto(sx, sy);
   do_delay(2);

   f_draw_dudes();

   if(!evebattle) {
      pause_song();
   }

   play_sfx(SFX_STARTBATTLE);

   owe_deinit();
   copy_page(1, 2);

   scl_init();

   fadeout(32);
   for(n = 0; n != 16; n += 2) {
      do_delay(1);
      scl_stretch(n+1);
   }
   for(n -= 2; n != 0; n -= 2) {
      do_delay(1);
      scl_stretch(n+1);
   }
   for(n = 2; n != 32; n += 2) {
      do_delay(1);
      scl_stretch(n+1);
      fade_step();
      fade_step();
   }
   scl_stretch(1);
   fade_step();
   fade_step();
   grad_off();

   scl_off();
   do_delay(60);

   if(evebattle) {
      if(bsong) {
#ifndef NO_MUSIC
         play_song(bsong - 1);
#endif
         bsong = 0;
      }
   }
   else {
#ifndef NO_MUSIC
      play_song(SNG_FIGHT1);
#endif
   }

   old_map = map_cur;
   old_sx = sx;
   old_sy = sy;

   // if not event, then set defaults
   if(!evebattle) {
      bflag_run = 1;    // run-able

      if(stats[CONNOR].level < 2 && vars[VAR_EVEPROG] < 8)
         bflag_lose = 0;      // can't lose
      else
         bflag_lose = 1;      // lose-able

      bflag_sp_guy = 0; // no special exit
   }

   bret = battle();

   /*if(bret == BATTLE_DEMOSKIP) {
      pfs_targetspr &= 252;
      setwinpal();
      load_gemgfx();
      return BATTLE_DEMOSKIP;
   }*/

   if(bret == BATTLE_LOSE) {
      f_lose();
      return BATTLE_LOSE;
   }

   pfs_targetspr &= 252;

   setwinpal();
   load_gemgfx();

        protect_rampages = 1;
   loadmap(old_map, (UBYTE)ch[0].x >> 1, (UBYTE)ch[0].y >> 1);
        protect_rampages = 0;

   f_setaluthapal(old_map);

   copy_page(2, 1);
   sx = old_sx;
   sy = old_sy;
   owe_redraw(sx, sy);
   do_delay(20);

   f_sync_party();
   f_draw_dudes();

   w4vbl_done();

   // event controlled?  otherwise, handle fadein
   if(!evebattle && bret != BATTLE_RESCUE) {
      resume_song();

      // special case -- night
      if(vars[VAR_EVEPROG] == 4) {
         f_colorshift(COLOR_BLUEDARK);
      }

      fadein(32);
      for(n = 0; n != 32; ++n) {
         w4vbl_done();
         fade_step();
      }
      f_resync();
   }

   //debug_on = 1;

   return bret;
}


void getdistxy(WORD xlen, WORD ylen, UBYTE dist)
{
        BYTE angle;

        if(xlen == 0) {
                px = 0;
                py = ylen > 0 ? (WORD)dist: -(WORD)dist;
                return;
        }

        angle = getarctan((ylen << 8) / xlen);

        if(xlen < 0) {
                angle = (WORD)angle + 0x80;
        }

        px = (getcos(angle) * (WORD)dist) >> 8;
        py = (getsin(angle) * (WORD)dist) >> 8;
}
