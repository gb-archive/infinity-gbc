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

#include"defs.h"
#include"d1.h"

extern UBYTE bspr_window;
extern UBYTE reset_ok, _gba;

extern UBYTE *map;                      // map pointer
extern WORD sx, sy;                     // camera coordinates
extern struct DUDESTRUCT ch[];          // character structs
extern BYTE numdudes;                   // max characters on the map
extern UBYTE maplenx, mapleny;          // dimensions of the map
extern BYTE control;                    // player has control?
extern UBYTE allow_scroll;

UWORD pal[64];

#define FONTBASE 128
#define FONTSIZE 127

BYTE i, i2;
WORD s, s2;
UBYTE n, n2;
UWORD u, u2;

UBYTE k, k2;


UBYTE protect_rampages;

// temp item list (for equipping)
UBYTE temp_num;
UBYTE temp_list_type[64];  //50
UBYTE temp_list_num[50];

extern UBYTE vblank_timer;
//extern UBYTE realdirs[];

UBYTE curzone, treasuremode;
//char treasurestr[32];

UBYTE mapw, maph;
UBYTE text_auto;
UBYTE vars[96];
UBYTE regs[8];
UBYTE area, lastsaveslot;
UBYTE partysize;
UBYTE party[4];
UBYTE realparty[4];
struct DUDESTAT stats[6];
struct DUDESTAT *st;
BYTE key[8];         // key array
UBYTE tileset_cur;
UBYTE map_cur;
UBYTE ch_num, en_num, npc_num;
UWORD ch_base, en_base, npc_base;
UBYTE defaultcolors[6] = { 0, 15, 15, 0, 0, 7 };
UBYTE gold[3];
UBYTE mostgold[3] = { 15, 66, 63 };
UBYTE encounter_zone, encounter_protect, encounter_steps, encounter_check;
UBYTE boat_x, boat_y, boat_dir, on_boat;
UBYTE map_state, hit_state;

UBYTE last_page;
UWORD x, y;

UBYTE xtile;
UBYTE lsx, lsy;

UBYTE *mapmem;
UBYTE *pagptr;

UBYTE skl_name[10];
UBYTE skl_cost, skl_type, skl_gem, skl_item;
UBYTE gl_hp, gl_sp, gl_str, gl_end, gl_agl, gl_wis;

char menu_blank[] = "  ";
char menu_dequip[] = "ARemove";
char menu_cantsell[] = "   -----";

UBYTE sl_borderloc;
UBYTE scroll_location;
unsigned char menu_backup[96];

// item list
UBYTE item_num;
UBYTE item_list_type[100];
UBYTE item_list_num[100];
UBYTE item_shop_at;
UBYTE item_shop[16];
UWORD item_cost[16];
UBYTE optionmode;
UBYTE options[7];

// gem list
UBYTE gems_list[9];
UBYTE gems_charge[9];
UBYTE gems_rel[9];
UBYTE gems_preorder[9] = { 0, 8 };

// battle vars
UBYTE icon, icon_wiggle, icon_count;
UBYTE turn, inturn, next_turn, moved;
UBYTE pending;
UBYTE actions;
UWORD actioncount;
BYTE dx, dy;
UBYTE bmenu, bmenucount;
UBYTE flop;
UBYTE bframe;
BYTE req, lastreq, reqcount;
BYTE bselect;
UBYTE bloops, draw_gfx;
UBYTE bin_fade, bin_popup;

UBYTE b_target, b_dir, b_attrib;
WORD b_damage;

BYTE player_req;
UBYTE player_turn;
UBYTE player_tmode;

UWORD curframe;
WORD bsx, bsy, bdsx, bdsy;
WORD bcx, bcy, bdcx, bdcy;
UBYTE bmap[9][11];
UBYTE bnumdudes;

struct BDUDE *bch = (struct BDUDE *)0xD000;
struct BCHAIN *bchain_table = (struct BCHAIN *)0xD600;
struct BPRO *bpro = (struct BPRO *)0xD800;
//UBYTE *bdata = (UBYTE *)0xDB00;

WORD px, py;

UBYTE bchain_active;
UBYTE bchain_steps;
BYTE bchain_at;

UBYTE bbox[6];
UBYTE bboxpal[6];

UBYTE bmenusquish, binputmode;
UBYTE btextdesc;
UBYTE bboxmenu;
UBYTE bchoosetarget;
UBYTE bchooseicon;
UBYTE bchoosetype;

UBYTE keylock[8];
BYTE bfocus;
UBYTE done, odone;
UBYTE badvance;

UBYTE bgemslide;
UBYTE bgemcharge1;
UBYTE bgemcharge2;
UBYTE bgemselect;
UBYTE bskillselect;
UBYTE bfreeze;
UBYTE bchnum, bennum;
UBYTE bformation;
UBYTE bcoloractive;
UBYTE bearly;
UBYTE bcritical;

UBYTE bflag_run, bflag_lose, bflag_sp_guy, bflag_sp_hp;
UBYTE bitem_tab[12], bitem_item;
UBYTE bwon_item, bwon_gp;
UBYTE brun_counter;

UBYTE bfastestguy, bfastestspd;

UBYTE bsong;

// for bcalcmod()
UBYTE bcm_guy, bcm_target;
WORD bcm_damage;

/////////////////////////////////


UBYTE getrand(UBYTE n)
{
        return (randfunc() % n);
}

// a prototype
void render_map_full();

extern UBYTE map_lenx;
extern UBYTE *mapsrc_ptr;
extern UBYTE *mapdest_ptr;
extern UBYTE *pal_ptr;
extern UBYTE map_vram_x;
extern UBYTE map_vram_y;
extern UWORD window_ptr;
extern UBYTE barg1, barg2, barg3, barg4, barg5;
extern UWORD warg1, warg2, warg3, warg4, warg5;
extern UBYTE pfs_targetbgr, pfs_targetspr;

extern UBYTE grad_var1, grad_var2;
extern UBYTE grad_rgb1[3];
extern UBYTE grad_rgb2[3];
extern UBYTE grad_every;

extern void render_bg_asm();
extern void render_bg_hline_asm();
extern void render_bg_vline_asm();
extern void asm_init();
extern void w4vbl();

extern void dfs_line();
extern void dfs_clear();
extern void dfs_pstr();
extern void dfs_pchar();
extern void dfs_strlen();
extern void dfs_window();
//extern void dfs_update();

extern void grad_init();
extern void grad_int();
extern void grad_on();
extern void grad_off();
extern void grad_clear();
extern UBYTE grad_issprites;

void pfs_setpal(UWORD *pal);
void pfs_setup(UWORD *src, UWORD *dest, UBYTE fadelength);
void pfs_step();
void pfs_backstep();

//void pflush();

void setwindow(UBYTE x, UBYTE y);

void menu_drawbox(UBYTE x, UBYTE y, UBYTE w, UBYTE h);

void bspr_interrupt();

void SOUNDLIB_Init();
void SOUNDLIB_Update();
void SOUNDLIB_sfx(UBYTE num);
void SOUNDLIB_Play(UBYTE songnum);
void SOUNDLIB_Resume();
void SOUNDLIB_Stop();
void SOUNDLIB_Pause();


WORD oldsx, oldsy;

UBYTE paltable[64];
UBYTE lastsong;

#define blackpal ((UWORD *)0)
#define whitepal ((UWORD *)1)

UBYTE realdirs[4] = { 3, 0, 1, 2 };


// ********* START OF GAME ************
extern void main_game();

void main()
{
   rom_page(OWE_PAGE);
   main_game();
}

void setwinpal();

void load_menugfx()
{
   UBYTE lastpage;

   lastpage = rom_page(DATA_PAGE);
   set_win_data(0, 96, menugfx);
   rom_page(lastpage);
}

void init()
{
   UBYTE last_page;

   asm_init();

   rom_page(DATA_PAGE);
   memcpy(pal + 32, _gba ? dudepal2 : dudepal, (UWORD)64);

   whiteout();
   w4vbl_done3();
   SCX_REG = 0;
   SCY_REG = 0;

   WX_REG = 160+7;
   WY_REG = 0;

   window_ptr = 0x9C00;
   setwinpal();

   DISPLAY_ON;
   enable_interrupts();

   rom_page(OWE_PAGE);
   bspr_off();

   set_interrupts(VBL_IFLAG | LCD_IFLAG);

   lastsong = 255;

   last_page = rom_page(TABLE_PAGE);
   pagptr = (UBYTE *) 0x4000;
   ch_num = pagptr[4096];
   en_num = pagptr[4097];
   npc_num = pagptr[4098];

   en_base = ((UWORD)ch_num << 6);
   npc_base = ((UWORD)ch_num << 6) + (((UWORD)en_num << 3) + (UWORD)en_num);

   load_gradcolors(defaultcolors);

   rom_page(DATA_PAGE);
   load_gemgfx();
   load_menugfx();

   rom_page(last_page);

   pal_ptr = paltable;
   map = (UBYTE *)0xD000;
}

void load_gemgfx()
{
   UBYTE lastpage;

   lastpage = rom_page(DATA_PAGE);
   VBK_REG = 1;
   set_sprite_data(64, 64, (unsigned char *)gemgfx + 16);
   VBK_REG = 0;
   rom_page(lastpage);
}

void init_game()
{
   setwinpal();
   set_w_attr();

   load_gemgfx();
   load_menugfx();
}

void fix_camera()
{
   WORD n, n2;

   if(sx < 0)
      sx = 0;
   else {
      n = (WORD)mapw * 16;
      n -= 160;
      if(sx > n)
         sx = n;
   }
   if(sy < 0)
      sy = 0;
   else {
      n = (WORD)maph * 16;
      n -= 144;
      if(sy > n)
         sy = n;
   }
}

void load_gradcolors(UBYTE *colors)
{
        UBYTE n;

   memcpy(grad_rgb1, colors, 6);
        if(_gba) {
                for(n = 0; n < 6; ++n) {
                        grad_rgb1[n] += 8;
                }
        }
}

void start_gradient_small()
{
   grad_var1 = 88;
   grad_var2 = 56;

   grad_clear();
   grad_issprites = 0;
   grad_init();

   dialogue_on();
}

void start_gradient_full()
{
        //return;

   grad_var1 = 0;
   grad_var2 = 144;

   grad_issprites = 1;
   grad_init();
        grad_every = 0;
   grad_on();
}

void start_gradient_slm_real();
void start_gradient_slm()
{
   UBYTE lastpage;

   grad_issprites = 1;

   lastpage = rom_page(MENU2_PAGE);
   start_gradient_slm_real();
   rom_page(lastpage);
}

void start_gradient_battle()
{
   grad_var1 = 112; //0; //112;
   grad_var2 = 32; //144; //32;

   grad_clear();
   grad_issprites = 0;
   grad_init();
   grad_on();
}

void stop_gradient()
{
   grad_off();
   dialogue_off();
   SHOW_SPRITES;
}

void openbattlewindow()
{
   menu_drawbox(0, 0, 20, 4);

   WY_REG = 144;
   w4vbl_done();
   WX_REG = 7;
}

void closebattlewindow()
{
   WX_REG = 167;
}

BYTE atline;

void openwindow()
{
   UBYTE n;

   menu_drawbox(0, 0, 20, 20);
   menu_drawbox(0, 0, 20,  7);

   dfs_window();
   //pflush();

   barg1 = 0;
   barg2 = 0;

   start_gradient_small();
   atline = 0;
}

void closewindow()
{
   WX_REG = 160+7;
   WY_REG = 0;

   stop_gradient();
   SHOW_SPRITES;
}

void resetupwindow()
{
   dfs_window();
   barg1 = 4;
   barg2 = 0;
   atline = 0;
}

void show_moreicon()
{
   put_tile(18, 6, 16);
}

void hide_moreicon()
{
   put_tile(18, 6, 7);
}

void win_pchar(UBYTE c)
{
   barg4 = c;
   dfs_pchar();
}

void win_newline()
{
   ++atline;

   barg1 = 4;
   barg2 = 14 * atline;
}


UBYTE pstr_curtile;
UBYTE pstr_curtile_stackp;
UBYTE pstr_curtile_stack[8];

void reset_str(UBYTE start)
{
   pstr_curtile_stackp = 0;
   pstr_curtile = FONTBASE + start;

   dfs_clear(pstr_curtile, FONTBASE+FONTSIZE-pstr_curtile);
   //dfs_update();
}

UBYTE read_str()
{
   return pstr_curtile;
}

void point_str(UBYTE x, UBYTE len)
{
   pstr_curtile = x;
   if(len != 0)
      dfs_clear(pstr_curtile, len);
}

void clear_str()
{
   dfs_clear(pstr_curtile, FONTBASE+FONTSIZE-pstr_curtile);
}

UBYTE save_str()
{
   pstr_curtile_stack[pstr_curtile_stackp] = pstr_curtile;
   ++pstr_curtile_stackp;

   return pstr_curtile;
}

void restore_str()
{
   if(pstr_curtile_stackp > 0) {
      --pstr_curtile_stackp;
      pstr_curtile = pstr_curtile_stack[pstr_curtile_stackp];
      clear_str();
      //dfs_update();
   }
   else {
      reset_str(0);
   }
}

void ppstr(UBYTE x, UBYTE y, UBYTE xofs, UBYTE len, char *s)
{
   barg1 = x;
   barg2 = y;
   barg3 = pstr_curtile;
   barg4 = len;

   pstr_curtile += len;

   dfs_line();

   barg1 = xofs;

   warg1 = (UWORD)s;
   dfs_pstr();
}

void ipstr(UBYTE xofs, UBYTE len, char *s)
{
   barg1 = x;
   barg2 = y;
   barg3 = pstr_curtile;
   barg4 = len;

   pstr_curtile += len;

   barg1 = xofs;
   warg1 = (UWORD)s;
   dfs_pstr();
}

void pstr(UBYTE x, UBYTE y, UBYTE len, char *s)
{
   ppstr(x, y, 0, len, s);
}

//void pflush()
//{
// dfs_update();
//}

void fpstr(UBYTE x, UBYTE y, UBYTE len, char *s, UBYTE page)
{
   UBYTE lastpage;

   lastpage = rom_page(page);
   pstr(x, y, len, s);
   rom_page(lastpage);
}

void fipstr(UBYTE xofs, UBYTE len, char *s, UBYTE page)
{
   UBYTE lastpage;

   lastpage = rom_page(page);
   ipstr(xofs, len, s);
   rom_page(lastpage);
}


void winpal(UBYTE x, UBYTE y, UBYTE w, UBYTE h, UBYTE pal)
{
   UBYTE n, n2;
   UBYTE lenx, leny;

   lenx = x + w;
   leny = y + h;

   VBK_REG = 1;
   for(n2 = y; n2 != leny; ++n2) {
      for(n = x; n != lenx; ++n) {
         put_tile(n, n2, pal);
      }
   }
   VBK_REG = 0;
}


// sound
void play_song(UBYTE songnum)
{
   if(lastsong == songnum)
      return;

   SOUNDLIB_Play(songnum);
   lastsong = songnum;
}

void stop_song()
{
   SOUNDLIB_Stop();
   lastsong = 255;
}

UBYTE paused_song;
void resume_song()
{
   if(paused_song == 255) {
      stop_song();
      return;
   }
   SOUNDLIB_Resume();
   lastsong = paused_song;
}

void pause_song()
{
   SOUNDLIB_Pause();
   paused_song = lastsong;
   lastsong = 255;
}

/*void play_sfx(UBYTE num)
{
        //SOUNDLIB_sfx(num);
}*/


//void update_sound()
//{
// SOUNDLIB_Update();
//}


extern UWORD pal_cur[], pal_bak[];


void fadeto(UBYTE type, UBYTE len)
{
   pfs_setup(pal_cur, (UWORD *)type, len);
}

void fadeout(UBYTE len)
{
   pfs_setup(pal_cur, blackpal, len);
}

/*void fadeoutgrd()
{
        UBYTE lastpage;

        lastpage = rom_page(TITLE_PAGE);
        pfs_setup(pal_cur, _gba ? great_dark_fade_pal + 64 : great_dark_fade_pal, len);
        rom_page(lastpage);
}*/

void fadefrom(UBYTE type, UBYTE len)
{
   pfs_setup((UWORD *)type, pal, len);
}

void fadein(UBYTE len)
{
   pfs_setup(blackpal, pal, len);
}

void blackout()
{
   pfs_setpal(blackpal);
}

void whiteout()
{
   pfs_setpal(whitepal);
}

void restorepal()
{
   pfs_setpal(pal_bak);
}

void setcurpal()
{
   pfs_setpal(pal);
}

extern UWORD title_pal[];
extern UWORD title_pal_gba[];
extern UWORD title_pal_full[];
extern UWORD title_pal_full_gba[];

void fadein_title(UBYTE len)
{
   UBYTE lastpage;

   lastpage = rom_page(TITLE_PAGE);
   pfs_setup(blackpal, _gba ? title_pal_gba : title_pal, len);
   rom_page(lastpage);
}

void fadeout_title(UBYTE len)
{
   UBYTE lastpage;

   lastpage = rom_page(TITLE_PAGE);
   pfs_setup(_gba ? title_pal_full_gba : title_pal_full, blackpal, len);
   rom_page(lastpage);
}

void fade_step()
{
   pfs_step();
}

void fade_backstep()
{
   pfs_backstep();
}

//void gfx_load(UBYTE dest, UBYTE *ptr);

extern UBYTE owe_pantile[];

void loadtileset(UBYTE x);
/*{
   static UBYTE set_page;
   static UWORD n;
   static UBYTE set_start;
   static UBYTE num_meta, num_tiles;
   static UBYTE lo, hi;
   static UBYTE lastpage;

   // switch to table page
   lastpage = rom_page(TABLE_PAGE);
   pagptr = (UBYTE *)0x4000;

   // read tileset header
   set_start = pagptr[4];
   lo = pagptr[5];
   hi = pagptr[6];

   // locate tileset table
   n = (UWORD)hi << 8;
   n += lo;
   pagptr = (UBYTE *)0x4000 + n;
   pagptr += (UWORD)x * 8;


   // read in the tileset info
   set_page = set_start + pagptr[0];
   lo = pagptr[1];
   hi = pagptr[2];
   num_meta = pagptr[3];
   num_tiles = pagptr[4];

   // point to the tileset
   rom_page(set_page);
   n = (UWORD)hi << 8;
   n += lo;
   pagptr = (UBYTE *)0x4000 + n;


   // load in the tileset graphics
   VBK_REG = 1;

   // pan tile thing
   memcpy(owe_pantile, pagptr + ((UWORD)num_meta * 8) + 64, (UWORD)64);
   // ***end     ***

   // load in the tiles
   for(n = 0; n < 64; ++n) {
      gfx_load(n, pagptr + ((UWORD)num_meta * 8) + (n * 64) + 64);
   }
   memcpy(pal, pagptr, (UWORD)64);

   VBK_REG = 0;


   // don't set palette, use fade instead
   // set_bkg_palette(0, 7, (UWORD *)tileset_pal);

   // flip back
   rom_page(lastpage);
}*/

UBYTE loadmapbase(UBYTE num);
/*{
   UBYTE *newmap;
   UWORD size;
   UBYTE map_page;
   UWORD n;
   UBYTE map_start;
   UBYTE num_meta, num_tiles;
   UBYTE lo, hi;
   UBYTE lastpage;
   UBYTE xsize, ysize;
   UWORD i, i2;
   UBYTE tileset;
   UBYTE info;
   UBYTE *pagptr;

   // blank out the map
   memset((UBYTE *)0xD000, (UBYTE)0, (UWORD)4096);

   // switch to table page
   lastpage = rom_page(TABLE_PAGE);
   pagptr = (UBYTE *)0x4000;

   // read map header
   map_start = pagptr[8];
   lo = pagptr[9];
   hi = pagptr[10];

   // locate map table
   n = (UWORD)hi << 8;
   n += lo;
   pagptr = (UBYTE *)0x4000 + n;
   pagptr += (UWORD)num * 8;

   // read in the map info
   map_page = map_start + pagptr[0];
   lo = pagptr[1];
   hi = pagptr[2];
   xsize = pagptr[3];
   ysize = pagptr[4];
   tileset = pagptr[5];
   info = pagptr[6];

   // load in the tileset
   loadtileset(tileset);

   if(num == 0)
      goto no_mapload;

   // point to the map
   rom_page(map_page);
   n = (UWORD)hi << 8;
   n += lo;
   pagptr = (UBYTE *)0x4000 + n;


   // load map data / set vars
   if((info & 3) == 0) {
      maplenx = 64;
      mapleny = 64;
   }
   else if((info & 3) == 1) {
      maplenx = 32;
      mapleny = 128;
   }
   else if((info & 3) == 2) {
      maplenx = 128;
      mapleny = 32;
   }

// maplenx = 32;
// mapleny = 32;

   newmap = (UBYTE *)pagptr;
   mapmem = (UBYTE *)0xD000;

   size = (UWORD)maplenx * (UWORD)mapleny;

   // insert the map
   for(i2 = 0; i2 != ysize; ++i2) {
      for(i = 0; i != xsize; ++i) {
         mapmem[i2 * maplenx + i] = newmap[i2 * xsize + i];
      }
   }

no_mapload:
   mapw = xsize;
   maph = ysize;

   rom_page(lastpage);

   if(info & 4)
      allow_scroll = 0;
   else
      allow_scroll = 1;

   tileset_cur = tileset;
   map_cur = num;

   return info;
}*/


void setaluthapal(UBYTE map);

void loadmap(UBYTE num, WORD xpos, WORD ypos)
{
   UBYTE i;
   UBYTE info;
        UBYTE lastpage;

   owe_off2();

   info = loadmapbase(num);

   // set the camera
   sx = xpos * 16 - 72; // - 80 + 8;
   sy = ypos * 16 - 64; // - 72 + 8;

   if(!allow_scroll)
      fix_camera();

   owe_init(tileset_cur, info, sx, sy);
}

void bloadmap(UBYTE num)
{
   UBYTE info;

   owe_off();

   info = loadmapbase(num);

   owe_init(tileset_cur, info, 32, 48);
   owe_deinit();
}

void pchar(UBYTE x, UBYTE y, UBYTE c)
{
   UBYTE ch;

   ch = c - ' ' + FONTBASE; //+128;
   set_win_tiles(x, y, 1, 1, (unsigned char *)&ch);
}

void do_delay(UBYTE n)
{
   UBYTE c;

   for(c = 0; c < n; ++c)
      w4vbl_done();
}

UBYTE get_keypad();

void setwindow(UBYTE x, UBYTE y)
{
   WY_REG = y;
   WX_REG = x+7;

   bspr_window = y;
}


// MENU

UBYTE mgs_stable[MGS_NUM*2];
UBYTE mgs_wx;
UBYTE mgs_wy;

void mgs_setwin(UBYTE    x, UBYTE y         );
void mgs_sput  (UBYTE slot, UBYTE x, UBYTE y);
void mgs_shide (UBYTE slot                  );
void mgs_update(UBYTE slot                  );
void mgs_schg(UBYTE slot, UBYTE n);
void mgs_sload(UBYTE slot, UBYTE type, UBYTE dir, UBYTE frame);
void mgs_wload(UBYTE slot, UBYTE n, UBYTE pal);
void mgs_gload(UBYTE slot, UBYTE n);
void mgs_init();
void mgs_spal(UBYTE slot, UBYTE pal);

void mgs_slm()
{
   UBYTE n;

   mgs_wx = 0;
   mgs_wy = 0;

   for(n = 0; n != MGS_NUM; ++n)
      mgs_shide(n);
}

void mgs_init()
{
   UBYTE n;

   mgs_wx = 160;
   mgs_wy = 0;

   for(n = 0; n != MGS_NUM; ++n)
      mgs_shide(n);
}

void mgs_update(UBYTE slot)
{
   UBYTE cx, cy;

   cx = mgs_stable[ slot << 1     ];
   cy = mgs_stable[(slot << 1) + 1];

   move_sprite((slot<<1)    , cx  +8   +mgs_wx,   cy  +16 +mgs_wy);
   move_sprite((slot<<1) + 1, cx  +8+8 +mgs_wx,   cy  +16 +mgs_wy);
}

void mgs_setwin(UBYTE x, UBYTE y)
{
   UBYTE n;

   WX_REG = mgs_wx+7;
   WY_REG = mgs_wy;

   mgs_wx = x;
   mgs_wy = y;

   for(n = 0; n != MGS_NUM; ++n) {
      mgs_update(n);
   }
}

void mgs_sput(UBYTE slot, UBYTE x, UBYTE y)
{
   struct DUDESTAT *st;

   if(slot >= MGS_CH && slot <= MGS_CH + 3) {
      st = get_party(slot - MGS_CH);
      if(!st) {
         mgs_stable[ slot << 1     ] = (UBYTE)-16;
         mgs_stable[(slot << 1) + 1] = (UBYTE)-16;
         mgs_update(slot);
         return;
      }
   }

   mgs_stable[ slot << 1     ] = x;
   mgs_stable[(slot << 1) + 1] = y;

   mgs_update(slot);
}

void mgs_shide(UBYTE slot)
{
   mgs_sput(slot, (UBYTE)-8, (UBYTE)-16);
}

void mgs_sload(UBYTE slot, UBYTE type, UBYTE dir, UBYTE frame)
{
   UWORD at;

   at = ((UWORD)type << 6) + ((UWORD)realdirs[dir] << 1) + (UWORD)frame;
   if(slot & 128)
      spr_load2(slot&127, at);
   else
      spr_load(slot&127, at);
}

void mgs_wload(UBYTE slot, UBYTE n, UBYTE pal)
{
   UBYTE *data;
   UBYTE lastpage;
   UBYTE n2, n3;

   lastpage = rom_page(DATA_PAGE);
   if(n >= 20) {
      n -= 20;
      data = (UBYTE *)skillgfx + 20 + (64 * (UWORD)n);
      pal = skillgfx[n];
   }
   else if(n >= 8) {
      n += 8;
      data = (UBYTE *)wingding + (64 * (UWORD)n);
   }
   else
      data = (UBYTE *)menuwing + (64 * (UWORD)n);

   n2 = slot<<1;
   n3 = slot<<2;

   set_sprite_data(n3, 4, (unsigned char *)data);
   set_sprite_tile(n2, n3);
   set_sprite_tile(n2+1, n3+2);
   set_sprite_prop(n2, pal);
   set_sprite_prop(n2+1, pal);

   rom_page(lastpage);
}

void mgs_gload(UBYTE slot, UBYTE n)
{
   UBYTE lastpage;
   UBYTE pal;
   UBYTE n2, n3;

   lastpage = rom_page(DATA_PAGE);
   pal = gemgfx[n];

   n2 = slot<<1;
   n3 = n<<2;

   set_sprite_tile(n2, n3 + 64);
   set_sprite_tile(n2+1, n3 + 66);
   set_sprite_prop(n2, pal | 8);
   set_sprite_prop(n2+1, pal | 8);

   rom_page(lastpage);
}

void mgs_setpal(UBYTE slot, UBYTE pal)
{
   set_sprite_prop((slot<<1), pal);
   set_sprite_prop((slot<<1)+1, pal);
}


void load_wingding()
{
   UBYTE u;
   UBYTE lastpage;

   lastpage = rom_page(DATA_PAGE);

   set_sprite_data( 64, 64, (unsigned char*) wingding);
   VBK_REG = 1;
   set_sprite_data( 96, 16, (unsigned char*) missgfx);
   VBK_REG = 0;

   rom_page(lastpage);
}

UBYTE *bgfx_tab[] = {
   (UBYTE *)sword1,
   (UBYTE *)scratch1,
   (UBYTE *)poofgfx,
   (UBYTE *)cure1,
   (UBYTE *)fire1,
   (UBYTE *)jolt1,
   (UBYTE *)tidal1,
   (UBYTE *)fx1,
   (UBYTE *)arrow_w,
   (UBYTE *)wakegfx,
   (UBYTE *)staff1gfx,

   // DATA2
   (UBYTE *)slurpgfx,
   (UBYTE *)spawn1,
        (UBYTE *)slash1,
        (UBYTE *)whirl1,
};

void load_bgfx(UBYTE x)
{
   UBYTE *ptr;
   UBYTE lastpage;
   UBYTE nextpage;

   nextpage = DATA_PAGE;

   ptr = bgfx_tab[x];
   if(x >= 11)
      nextpage = DATA2_PAGE;

   lastpage = rom_page(nextpage);

   VBK_REG = 1;
   set_sprite_data( 0, 64, (unsigned char*) ptr);
   VBK_REG = 0;

   rom_page(lastpage);
}

UBYTE load_bgfx_item(UBYTE x)
{
   UBYTE lastpage;
   UBYTE *ptr;
   UBYTE n;

   lastpage = rom_page(DATA_PAGE);

   ptr = (UBYTE *)itemgfx + 24;
   ptr += (UWORD)x * 64;

   VBK_REG = 1;
   set_sprite_data( 0, 4, (unsigned char*) ptr);
   VBK_REG = 0;

   n = itemgfx[x];

   rom_page(lastpage);
   return n;
}

/*void bre_setup(UBYTE src, UBYTE dest, UBYTE steps, struct BRE *bre)
{
   BYTE delta, absdelta;
   BYTE n;

   delta = dest - src;
   absdelta = (delta < 0x00) ? -delta: delta;
   bre->current = src;
   bre->error = 0;
   n = absdelta / steps;
   bre->sign = (delta < 0x00) ? -1: 1;
   bre->step_whole = (delta < 0x00) ? -n: n;
   bre->step_error = absdelta % steps;
   bre->steps = steps;
}

UBYTE bre_step(struct BRE *bre)
{
   bre->error += bre->step_error;
   bre->current += bre->step_whole;
   if(bre->error >= bre->steps) {
      bre->error -= bre->steps;
      bre->current += bre->sign;
   }

   return bre->current;
}*/

UBYTE real_battle();
void real_menu();

UBYTE battle()
{
   UBYTE c;
   UBYTE lastpage;

   lastpage = rom_page(BATTLE_PAGE);
   c = real_battle();
   rom_page(lastpage);

   return c;
}

void menu()
{
   UBYTE lastpage;

   lastpage = rom_page(MENU_PAGE);
   real_menu();
   rom_page(lastpage);
}


/* MENU2 forwarding :) */
BYTE do_gemsmenu()
{
   UBYTE lastpage;
   UBYTE r;

   lastpage = rom_page(MENU2_PAGE);
   r = do_gemsmenu_real();
   rom_page(lastpage);

   return r;
}


// misc
struct DUDESTAT *get_party(UBYTE x)
{
   UBYTE n;

   n = party[x];
   if(n == 0)
      return 0;
   return (&stats[n-1]);
}

struct DUDESTAT *get_realparty(UBYTE x)
{
   UBYTE n;

   n = realparty[x];
   if(n == 0)
      return 0;
   return (&stats[n-1]);
}

void f_memcpy(UBYTE *dest, UBYTE *src, UBYTE page, UWORD size)
{
   UBYTE lastpage;

   lastpage = rom_page(page);
   memcpy(dest, src, (UWORD)size);
   rom_page(lastpage);
}

void load_eform(UBYTE x, UBYTE *dest)
{
   UBYTE lastpage;
   UWORD *ptr;
   UWORD n;

   lastpage = rom_page(EFORM_PAGE);

   ptr = (UWORD *)0x4000;
   n = ptr[1] + 0x4000;
   ptr = (UWORD *)n;
   n = ptr[x] + 0x4000;
   pagptr = (UBYTE *)n;
   memcpy(dest, pagptr, (UWORD)64); // max formation size

   rom_page(lastpage);
}

void load_enemy(UBYTE x, UBYTE *dest)
{
   UBYTE lastpage;
   UWORD *ptr;
   UWORD n;

   lastpage = rom_page(EFORM_PAGE);

   ptr = (UWORD *)0x4000;
   n = ptr[2] + 0x4000;
   pagptr = (UBYTE *)n + ((UWORD)x * (18+16+4));
   memcpy(dest, pagptr, (UWORD)(18+16+4));

   rom_page(lastpage);
}


void load_skill(UBYTE x);

void f_load_skill(UBYTE x)
{
   UBYTE lastpage;

   lastpage = rom_page(MENU2_PAGE);
   load_skill(x);
   rom_page(lastpage);
}

void f_get_dxdy(BYTE x, BYTE y, UBYTE req)
{
   UBYTE lastpage;

   lastpage = rom_page(BATTLE4_PAGE);
   get_dxdy(x, y, req);
   rom_page(lastpage);

   return;
}

void f_player_control(UBYTE guy)
{
   UBYTE lastpage;

   lastpage = rom_page(BATTLE_PAGE);
   player_control(guy);
   rom_page(lastpage);
}

void f_bchain_insert(UBYTE guy, UBYTE action, UBYTE dir, WORD arg1, WORD arg2, WORD arg3)
{
   UBYTE lastpage;

   lastpage = rom_page(BATTLE2_PAGE);
   bchain_insert(guy, action, dir, arg1, arg2, arg3);
   rom_page(lastpage);
}

void bresolve(UBYTE guy);
void f_bresolve(UBYTE guy)
{
   UBYTE lastpage;

   lastpage = rom_page(BATTLE_PAGE);
   bresolve(guy);
   rom_page(lastpage);
}

UWORD bgetrecover(struct BDUDE *b);
UWORD f_bgetrecover(struct BDUDE *b)
{
   UBYTE lastpage;
   UWORD ret;

   lastpage = rom_page(BATTLE_PAGE);
   ret = bgetrecover(b);
   rom_page(lastpage);

   return ret;
}

UBYTE item_info_buf[16];
BYTE item_info_buf2[16];

UBYTE *item_info(UBYTE num)
{
   UBYTE lastpage;

   lastpage = rom_page(DATA2_PAGE);
   memcpy(item_info_buf, item_name[num], (UWORD)16);
   rom_page(lastpage);

   return item_info_buf;
}

BYTE *item_info2(UBYTE num)
{
   UBYTE lastpage;

   lastpage = rom_page(DATA2_PAGE);
   memcpy(item_info_buf2, item_data[num], (UWORD)16);
   rom_page(lastpage);

   return item_info_buf2;
}

UWORD item_buycost(UBYTE num)
{
   UBYTE lastpage;
   UWORD n;

   lastpage = rom_page(DATA2_PAGE);
   n = item_selldata[num];
   rom_page(lastpage);

   return n;
}

UWORD item_sellcost(UBYTE num)
{
   UBYTE lastpage;
   UWORD n;

   lastpage = rom_page(DATA2_PAGE);
   n = item_selldata[num] >> 1;
   rom_page(lastpage);

   return n;
}

/*UWORD graypal[] = { 6076, 9513, 12684, 4228 };
void load_graypal()
{
   set_sprite_palette(6, 1, (UWORD *)graypal);
}*/

UBYTE *lead(UBYTE *p)
{
   --p;
   while(*(++p) == 16);
   return p;
}

void f_menu_scrollinit(UBYTE a, UBYTE b, UBYTE numsize)
{
   UBYTE lastpage;

   lastpage = rom_page(MENU2_PAGE);
   menu_scrollinit(a, b, numsize);
   rom_page(lastpage);
}

void f_menu_redraw(UBYTE x, UBYTE y, UBYTE deep, UBYTE at, char *s, char *s2)
{
   UBYTE lastpage;

   lastpage = rom_page(MENU2_PAGE);
   menu_redraw(x, y, deep, at, s, s2);
   rom_page(lastpage);
}

void f_menu_scroll(UBYTE dir, UBYTE x, UBYTE x2, UBYTE y, UBYTE entries, char *s, char *s2)
{
   UBYTE lastpage;

   lastpage = rom_page(MENU2_PAGE);
   menu_scroll(dir, x, x2, y, entries, s, s2);
   rom_page(lastpage);
}

UBYTE f_do_useitem(UBYTE mode, UBYTE type)
{
   UBYTE lastpage, n;

   lastpage = rom_page(MENU_PAGE);
   n = do_useitem(mode, type);
   rom_page(lastpage);

   return n;
}

void f_beadd(UBYTE type, UBYTE x, UBYTE y)
{
   UBYTE lastpage;

   lastpage = rom_page(BATTLE3_PAGE);
   beadd(type, x, y);
   rom_page(lastpage);
}

void f_bpadd(UBYTE type, UBYTE x, UBYTE y)
{
   UBYTE lastpage;

   lastpage = rom_page(BATTLE3_PAGE);
   bpadd(type, x, y);
   rom_page(lastpage);
}

void gemcheck(UBYTE target);
void f_gemcheck(UBYTE target)
{
   UBYTE lastpage;

   lastpage = rom_page(BATTLE3_PAGE);
   gemcheck(target);
   rom_page(lastpage);
}

void reset_data();
void f_reset_data()
{
   UBYTE lastpage;

   lastpage = rom_page(BATTLE4_PAGE);
   reset_data();
   rom_page(lastpage);
}

void catchup_level(struct DUDESTAT *guy);
void f_catchup_level(struct DUDESTAT *guy)
{
   UBYTE lastpage;

   lastpage = rom_page(MENU2_PAGE);
   catchup_level(guy);
   rom_page(lastpage);
}

UBYTE binqueue(UBYTE guy);
UBYTE f_binqueue(UBYTE guy)
{
   UBYTE lastpage;
   UBYTE r;

   lastpage = rom_page(BATTLE_PAGE);
   r = binqueue(guy);
   rom_page(lastpage);

   return r;
}

UBYTE brun_check();
UBYTE f_brun_check()
{
   UBYTE lastpage;
   UBYTE r;

   lastpage = rom_page(BATTLE4_PAGE);
   r = brun_check();
   rom_page(lastpage);

   return r;
}

UBYTE do_effect(UBYTE mode, UBYTE type, UBYTE guy);
UBYTE f_do_effect(UBYTE mode, UBYTE type, UBYTE guy)
{
   UBYTE lastpage;
   UBYTE r;

   lastpage = rom_page(BATTLE4_PAGE);
   r = do_effect(mode, type, guy);
   rom_page(lastpage);

   return r;
}

/*void items_set(UBYTE type, UBYTE value);
void f_items_set(UBYTE type, UBYTE value)
{
   UBYTE lastpage;

   lastpage = rom_page(MENU_PAGE);
   items_set(type, value);
   rom_page(lastpage);
}
*/

UWORD getbaseframe(UBYTE type)
{
   if(type < ch_num)
      return (UWORD)type << 6;
   else if(type < ch_num + en_num)
      return (((UWORD)type-ch_num) * 9) + en_base;
   else
      return (((UWORD)type-ch_num-en_num) << 3) + npc_base;
}

void f_bcalcmod()
{
   UBYTE lastpage;

   lastpage = rom_page(BATTLE4_PAGE);
   bcalcmod();
   rom_page(lastpage);
}

UBYTE battlestart(UBYTE formation);
UBYTE f_battlestart(UBYTE formation)
{
   UBYTE lastpage;
   UBYTE r;

   lastpage = rom_page(BATTLE4_PAGE);
   r = battlestart(formation);
   rom_page(lastpage);

   return r;
}

/*void sync_party();
void f_sync_party()
{
   UBYTE lastpage;

   lastpage = rom_page(OWE_PAGE);
   sync_party();
   rom_page(lastpage);
}

void draw_dudes();
void f_draw_dudes()
{
   UBYTE lastpage;

   lastpage = rom_page(OWE_PAGE);
   draw_dudes();
   rom_page(lastpage);
}

void resync();
void f_resync()
{
   UBYTE lastpage;

   lastpage = rom_page(OWE_PAGE);
   resync();
   rom_page(lastpage);
}*/

