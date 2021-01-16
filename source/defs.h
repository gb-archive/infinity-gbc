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

#include<gb.h>
void memcpy(void *dest, void *src, UWORD size);
void memset(void *dest, UBYTE c, UWORD size);
UBYTE rom_page(UBYTE page);
UBYTE ram_page(UBYTE page);

#define play_sfx(num) SOUNDLIB_sfx(num)

#include"eve/sfx.h"
#include"eve/itemdefs.h"

//#define NO_MUSIC        1

#define IT_HPLo         1
#define IT_SPLo         2
#define IT_HALo         3
#define IT_LFLo         4
#define IT_HPMd         5
#define IT_SPMd         6
#define IT_HAMd         7
#define IT_LFMd         8
#define IT_HPHi         9
#define IT_SPHi         10
#define IT_HAHi         11
#define IT_LFHi         12

#define SPRBASE_HERO 0
#define SPRBASE_ENEMY   6

#define  OWE_PAGE 1
#define OWE2_PAGE       2
#define DATA_PAGE 3
#define DATA2_PAGE   4
#define MENU_PAGE 5
#define MENU2_PAGE   6
#define BATTLE_PAGE  7
#define BATTLE2_PAGE 8
#define BATTLE3_PAGE 9
#define BATTLE4_PAGE 10
#define BATTLE5_PAGE 11
#define BATTLE6_PAGE 12
#define DFS_PAGE  13
#define TITLE_PAGE   14
#define ZONE_PAGE 15
#define EFORM_PAGE   16
#define EVENT_PAGE   17
#define TEXT_PAGE 21
#define TABLE_PAGE   28

#define MGS_EX    4
#define MGS_CH    16
#define MGS_NUM      20

#define GEMS_INDEX   100
#define GOLD_INDEX   116

#define CONNOR    0
#define RYAN      1
#define ELYA      2
#define VICTOR    3
#define ROLAND    4
#define ALUTHA          5
#define VICTOR_WOLF  6

#define BOSS      100

#define TYPE_BOAT 172

// NOTE! opal not used
#define GEM_RUBY  0
#define GEM_CORAL 1
#define GEM_EMERALD  2
#define GEM_DIAMOND  3
#define GEM_SAPPHIRE 4
#define GEM_OPAL  5
#define GEM_AMETHYST 6
#define GEM_GARNET   7
#define GEM_CRYSTAL  8
#define GEM_JADE  9
#define GEM_ONYX  10
#define GEM_PEARL 11
#define GEM_AQUAMARINE  12
#define GEM_TURQUIOSE   13
#define GEM_CARNELIAN   14
#define GEM_MOONSTONE   15

#define COLOR_BLACK  0
#define COLOR_WHITE  1
#define COLOR_RED 2
#define COLOR_BLUE   3
#define COLOR_YELLOW 4
#define COLOR_BLUEDARK  5
#define COLOR_GRDKISLE  0xFF

#define I_HPLO    1
#define I_HPHI    2
#define I_SPLO    3
#define I_HPSP    4

#define ICON_BASE 24

#define NUMKEYS      8
#define KEYDELAY  8

/* control defines */
#define DELAY           1
#define MENUDELAY 10
#define MOVESPEED       2
//#define MOVESPEED  1

#define WID             8
#define ANIMSPEED       4

#define NUM_MOVES       4
#define NUM_MOVES_NPC   8

//#define NUM_MOVES  8
//#define NUM_MOVES_NPC 16

/* defines */
#define UP              0
#define DOWN            1
#define LEFT            2
#define RIGHT           3

#define ENTER           4
#define CANCEL          5
#define MENU      6
#define SELECT    7

#define NORM            0
#define MOVE            1
#define STILL     2
#define FRAME     3

#define PLAYER          0
#define PARTY           1
#define WALK            2
#define STAND           3
#define BOAT      4


// search flags
#define SR_UP     1
#define SR_DOWN      2
#define SR_LEFT      4
#define SR_RIGHT  8


#define BATTLE_WIN      0
#define BATTLE_LOSE     1
#define BATTLE_RESCUE   2
#define BATTLE_DEMOSKIP 3


// must match edefs.h
#define VAR_EVEPROG 0


/* structs */

struct DUDESTAT {
   UBYTE name[9];
   UBYTE class[9];
   UBYTE type;
   WORD hp, hpmax;
   WORD sp, spmax;

   UBYTE level;
   UBYTE exp[3];

   UBYTE wolfpow;
   UBYTE pad[2];

   UBYTE att, def, spd, man;
   UBYTE str, end, agl, wis;

   UBYTE e[4]; // wep, arm, acc, gem
   UBYTE skl[4];
};

struct DUDESTRUCT {
   UBYTE active;  // is this entry active?
        WORD gx,gy;  // pixel x, y, z
        BYTE gz;
        BYTE pad;
 //       WORD x, y; // tile x, y
   UBYTE x, y;
        BYTE dir; // direction facing
        WORD frame;  // current frame
        UBYTE mode;  // current mode
        WORD count;  // allpurpose counter
        BYTE req; // mode change being requested
        UBYTE type;  // dude type
        BYTE control;   // PLAYER, PARTY, etc
   UBYTE framemode, animmode;
};

struct BRE {
   UBYTE current;
   UBYTE error;
   UBYTE step_whole;
   UBYTE step_error;
   UBYTE sign;
   UBYTE steps;
};

struct BRW {
   UBYTE part, error;
   WORD sign, whole, current;
};

/* OWE (overworld engine) */
void owe_init(UBYTE set, UBYTE type, UWORD x, UWORD y);
void owe_scrollto(UWORD x, UWORD y);
void owe_spr(UBYTE slot, WORD x, WORD y, BYTE z, UWORD frame);
void owe_spr_reset();
void owe_on();
void owe_off();
void owe_off2();

void owe_update(UBYTE x, UBYTE y);
void owe_redraw(UWORD x, UWORD y);

UBYTE owe_clip(UBYTE x, UBYTE y, UBYTE dir);
UBYTE owe_blocked(UBYTE type, UBYTE x, UBYTE y);

void owe_dscroll_on();
void owe_dscroll_off();

void owe_panspeed(UBYTE speed);

UBYTE map_gettype(UBYTE x, UBYTE y);
void map_settype(UBYTE x, UBYTE y, UBYTE type);
UBYTE map_getzone(UBYTE x, UBYTE y);

/* SLM (save/load menu) */
void slm_setx(UBYTE window, UBYTE x);
void slm_init();
void slm_off();


/* BX (battle transitions) */
void bx_init();
void bx_off();
void bx_setstep(UBYTE step);

/* Title */
void title_init();
void title_screen();

/* MENU */
void show_menu(UBYTE num);
/*UBYTE item_list_type(UBYTE x);
UBYTE item_list_num(UBYTE x);
void item_list_stype(UBYTE x, UBYTE val);
void item_list_snum(UBYTE x, UBYTE val);*/

/* MENU2 */
BYTE do_gemsmenu();

void w4vbl_done();
void w4vbl_done2();
void w4vbl_done3();
void w4line(UBYTE line);

UBYTE getrand(UBYTE n);
void showbuf();
void render_map();
void render_map_full();
void render_dudes();
void backup_tiles(UBYTE x, UBYTE y, UBYTE w, UBYTE h, unsigned char *mem);
void restore_tiles(UBYTE x, UBYTE y, UBYTE w, UBYTE h, unsigned char *mem);
void reset_window();
void draw_menu(UBYTE c);
void init_input();
void init();
void init_game();
void deinit();
void fix_camera();
void openwindow();
void closewindow();
UBYTE checkwindowlen();
void resetupwindow();
void win_pchar(UBYTE c);
void win_newline();

UBYTE loadmapbase(UBYTE num);

void loadmap(UBYTE num, WORD xpos, WORD ypos);
void loaddude(UBYTE slot, UBYTE xpos, UBYTE ypos, UBYTE type, UBYTE dir, UBYTE control);
void deldude(UBYTE slot);
void bloadmap(UBYTE num);

void pchar(UBYTE x, UBYTE y, UBYTE c);
void ptile(UBYTE x, UBYTE y, UBYTE c);

void pstr(UBYTE x, UBYTE y, UBYTE len, char *s);
void ppstr(UBYTE x, UBYTE y, UBYTE xofs, UBYTE len, char *s);
void ipstr(UBYTE xofs, UBYTE len, char *s);
void fpstr(UBYTE x, UBYTE y, UBYTE len, char *s, UBYTE page);
void fipstr(UBYTE xofs, UBYTE len, char *s, UBYTE page);

void do_delay(UBYTE n);
void input();
BYTE inputb();

void win_tiles(UBYTE x);

void reset_str();
void restore_str();
UBYTE save_str();
UBYTE read_str();
void point_str(UBYTE x, UBYTE len);
void clear_str();
UBYTE *lead(UBYTE *p);

void mgs_init();
void mgs_slm();
void mgs_setwin(UBYTE    x, UBYTE y         );
void mgs_sput  (UBYTE slot, UBYTE x, UBYTE y);
void mgs_shide (UBYTE slot                  );
void mgs_update(UBYTE slot                  );
void mgs_schg(UBYTE slot, UBYTE n);
void mgs_sload(UBYTE slot, UBYTE type, UBYTE dir, UBYTE frame);   // load character
void mgs_wload(UBYTE slot, UBYTE n, UBYTE pal);       // load wingdings
void mgs_gload(UBYTE slot, UBYTE n);            // load gem
void mgs_shift(UBYTE zone, UBYTE dist, UBYTE y);
void mgs_setpal(UBYTE slot, UBYTE pal);
UBYTE mgs_getpal(UBYTE slot);

char *deci_24(UBYTE *num, UBYTE digits);
char *deci_16(UWORD num, UBYTE digits);
void add_24(UBYTE *num_dest, UBYTE *num_src);
void sub_24(UBYTE *num_dest, UBYTE *num_src);
BYTE cmp_24(UBYTE *num_1, UBYTE *num_2);
UBYTE set_24(UBYTE *num, UWORD val);

void menu_drawbox(UBYTE x, UBYTE y, UBYTE w, UBYTE h);
void menu_clearbox(UBYTE x, UBYTE y, UBYTE w, UBYTE h);
void f_menu_scrollinit(UBYTE a, UBYTE b, UBYTE numsize);
void f_menu_redraw(UBYTE x, UBYTE y, UBYTE deep, UBYTE at, char *s, char *s2);
void f_menu_scroll(UBYTE dir, UBYTE x, UBYTE x2, UBYTE y, UBYTE entries, char *s, char *s2);

void put_tile(UBYTE x, UBYTE y, UBYTE tile);
UBYTE get_tile(UBYTE x, UBYTE y);

void flicker_init();
void flicker_on();
void flicker_off();
void flicker_int();
void flicker_build_invert();
void flicker_ghost_on(UBYTE num);
void flicker_ghost_off();

void pfs_setpal(UWORD *pal);
void pfs_setup(UWORD *src, UWORD *dest, UBYTE fadelength);
void pfs_step();
void pfs_backstep();

void fadein_title(UBYTE len);
void fadeout_title(UBYTE len);
void fadeto(UBYTE type, UBYTE len);
void fadein(UBYTE len);
void fadeout(UBYTE len);
void fadeoutgrd(UBYTE len);
void fade_step();

void grad_op_enable();
void grad_op_disable();
void grad_init();

void winbkg_on();
void winbkg_off();
void scl_init();
void scl_pixelate(UBYTE x);
void scl_stretch(UBYTE x);
void scl_roll(UBYTE x);
void scl_prophecy(UBYTE x);
void scl_off();

void play_song(UBYTE songnum);
void play_sfx(UBYTE num);
void update_sound();
void SOUNDLIB_Vol(UBYTE vol);

void auto_proc(void (*func)());

void inputb_handler();

// misc far
void f_items_reset();
void f_items_add(UBYTE type);
void f_items_del(UBYTE type);
UBYTE f_items_have(UBYTE type);
void f_items_set(UBYTE type, UBYTE value);
void f_stats_recalc(struct DUDESTAT *guy);
void f_catchup_level(struct DUDESTAT *guy);
UBYTE f_do_slmenu(UBYTE type);
void f_curflick_on(UBYTE x, UBYTE y);
void f_curflick_off();
void f_menu_error();
void f_memcpy(UBYTE *dest, UBYTE *src, UBYTE page, UWORD size);
void f_prophecy();
void f_poem();


// bre
void bre_setup(UBYTE src, UBYTE dest, UBYTE steps, struct BRE *bre);
UBYTE bre_step(struct BRE *bre);

void brw_setup(struct BRW *brw, WORD src, WORD dest, UBYTE steps);
WORD brw_step(struct BRW *brw);

// misc
struct DUDESTAT *get_party(UBYTE x);
struct DUDESTAT *get_realparty(UBYTE x);
UBYTE get_event_byte(UWORD ptr);
void dialog_setup(UWORD text);
UBYTE get_dialog_byte();
UBYTE get_dialog_strlen();
void load_eform(UBYTE x, UBYTE *dest);
void load_enemy(UBYTE x, UBYTE *dest);
void load_gemgfx();
void copy_page(UBYTE src, UBYTE dest);
void winpal(UBYTE x, UBYTE y, UBYTE w, UBYTE h, UBYTE pal);
void setdudepal();
void blackout();
void whiteout();
void setcurpal();
void restorepal();
void load_graypal();
void load_gradcolors(UBYTE *colors);
UWORD getbaseframe(UBYTE type);
void clear_vram();

UBYTE *item_info(UBYTE num);
BYTE *item_info2(UBYTE num);
UWORD item_buycost(UBYTE num);
UWORD item_sellcost(UBYTE num);

// battle
void load_bgfx(UBYTE x);
UBYTE load_bgfx_item(UBYTE x);
void bloadboxes();



// battle defs
#define RUP    3
#define RDOWN     0
#define RLEFT     1
#define RRIGHT    2

#define BDIR_UP      0
#define BDIR_UPRIGHT 1
#define BDIR_DOWNRIGHT  2
#define BDIR_DOWN 3
#define BDIR_DOWNLEFT   4
#define BDIR_UPLEFT  5

#define BACT_NORM 0
#define  BACT_WALK   1
#define BACT_ATTACK  2
#define BACT_HURT 3
#define BACT_JUMP 5
#define BACT_SCRATCH 6
#define BACT_POOF 7
#define BACT_CLOSE   8
#define BACT_MISS 9
#define BACT_CAST 10
#define BACT_CURE 11
#define BACT_FIRE 12
#define BACT_QUAKE   13
#define BACT_LEAP 14
#define BACT_JOLT 15
#define BACT_TIDAL   16
#define BACT_PANEL   17
#define BACT_THROW   18
#define BACT_ARROW   19
#define BACT_PULLBOW 20
#define BACT_SPARKLE 21
#define BACT_WAKE 22
#define BACT_SLURP   23
#define BACT_SPAWN   24
#define BACT_CHARM   25
#define BACT_SLASH      26
#define BACT_WHIRL      27
#define BACT_DAMAGE     28
#define BACT_EFFECT     29


#define BATR_NONE 0
#define BATR_FIRE 1
#define BATR_WATER   2
#define BATR_WIND 3
#define BATR_HEAL 4
#define BATR_HAND 5

#define BAIL_CHARM   1
#define BAIL_POISON  2
#define BAIL_WOLF       3

#define BMOVESPEED   16
#define BATTACKSPEED 4
#define BJUMPSPEED   9

#define BPLAYER      0
#define BCPU      1

//#define MAXACTIONS 10

#define B_OVERLAY 128
#define B_BANK1      64
#define B_WDPAL      6

#define BFRAME_ATTACK   16

#define BWEAK     0
#define BSTRONG      2

struct BSTAT {
   UBYTE level;
   WORD hp, hpmax;
   WORD sp, spmax;
   UBYTE str, end, agl, wis;
   UBYTE att, def, spd, man;
   UBYTE item;
   UBYTE ai[16];
   UBYTE attrib[4];
};

struct BDUDE {
   UBYTE rep;
   UBYTE active;

   WORD rx, ry;
   UBYTE rz;
   BYTE x, y;
   UBYTE type;
   UBYTE frame;
   UBYTE action;
   UBYTE count;
   UBYTE progress;
   UBYTE control;
   UBYTE dir;

   // stat stuff
   struct BSTAT st;
   WORD damage;

   // bline
   struct BRW brw_x;
   struct BRW brw_y;

   // jumping
   BYTE dz;

   // backup
   UBYTE olddir;


   // active battle
   UBYTE recover;
   BYTE points;
   UBYTE target;     // who to hit?
   UBYTE poof;    // flagged for poof?
   UBYTE boss;    // 2x2 big character?
   UBYTE spawnlord;
   UBYTE slider;
   UBYTE ail;
   UWORD ailtime;

   UBYTE skl[4];
   UBYTE gem, gemcount, gemshow;
   UBYTE spflag;
   UBYTE flags;            // for bosses
};

struct BCHAIN {
   UBYTE guy;
   UBYTE action;
   UBYTE dir;
   WORD arg1, arg2, arg3;
};

struct BPRO {
   WORD rx, ry;
   struct BRW brw_x;
   struct BRW brw_y;
};

// chain system
UBYTE f_bchain_do();
void f_bchain_stop();
void f_bchain_reset();
void f_bchain_add(UBYTE guy, UBYTE action, UBYTE dir, WORD arg1, WORD arg2, WORD arg3);
void f_bchain_insert(UBYTE guy, UBYTE action, UBYTE dir, WORD arg1, WORD arg2, WORD arg3);

// more
UBYTE f_get_4waydir(UBYTE bdir);
void f_text_update();
UBYTE f_gain_level(struct DUDESTAT *st);
void f_load_skill(UBYTE x);
//UBYTE f_get_4waydir(UBYTE bdir);
void f_baction_poof(UBYTE guy);

// yet more
BYTE f_get_player_input(UBYTE guy);
void f_wipe_menu();
void f_wipe_text();
void f_wipe_gem();
void f_get_dxdy(BYTE x, BYTE y, UBYTE req);
void f_player_control(UBYTE guy);
void f_player_cancel();
void f_bresolve(UBYTE guy);
void f_start_skill();
void f_shop_reset();
void f_shop_add(UBYTE type, UWORD price);
void f_do_shopmenu();

void f_slide_in_menu(UBYTE num);
void f_slide_out_menu();
void f_draw_shopmenu();
UBYTE f_do_yesnomenu();
UBYTE f_do_useitem(UBYTE mode, UBYTE type);
void f_show_menu(UBYTE num);

UBYTE f_touch_guy(UBYTE x, UBYTE n, BYTE dir);

void f_bmapsetup();
void f_gem_animate(UBYTE guy);
void f_gemcheck(UBYTE target);
void f_reset_data();

void f_bcoloron(UBYTE color);
void f_bcoloroff();
void f_colorshift(UBYTE color);

void f_bautoproc();
void f_bvictory();

BYTE f_b_ai(UBYTE guy, UBYTE style);
BYTE f_b_ai_charm(UBYTE guy);

UWORD f_bgetrecover(struct BDUDE *b);

// even more
UBYTE battle();

// save game I/O
void fopen(UBYTE slot);
void fclose(UBYTE slot);

void spr_load(UBYTE dest, UWORD frame);
void spr_load2(UBYTE dest, UWORD frame);

UBYTE dfs_pschar(UBYTE tile, UBYTE x, UBYTE c);

void flag_set(UBYTE num);
UBYTE flag_get(UBYTE num);

UBYTE getcount(UBYTE speed, UBYTE action);

/**** bspr ****/
void bspr_init();
void bspr_scrollto(WORD x, WORD y);
void bspr_scrollset(WORD x, WORD y);
void bspr_16set(UBYTE slot, WORD x, WORD y, UBYTE z, WORD frame, BYTE pal);
void bspr_8set(UBYTE slot, WORD x, WORD y, BYTE frame, UBYTE mode);
void bspr_off();
void bspr_quake_on();
void bspr_quake_off();

void bicon_reset();
void bicon_slidein();
void bicon_slideout();
void bicon_expand(UBYTE x);
void bicon_shrink();
void bicon_refresh(UBYTE x);

// bcur
#define BCUR_NUM     2

#define BCUR_OFF        0
#define BCUR_FREEZE  1
#define BCUR_WIGGLE  2
#define BCUR_LOCKON  3

void f_bcur_init();
void f_bcur_dest();
void f_bcur_on();
void f_bcur_off();
void f_bcur_lock();
void f_bcur_update();

void f_bpro_set(UBYTE slot, WORD x1, WORD y1, WORD x2, WORD y2, UBYTE steps);
void f_bpro_next(UBYTE slot);

void f_setbosspal(UBYTE x);
UBYTE f_binqueue(UBYTE guy);
void f_bshowail(UBYTE guy, UBYTE frame);

void f_baction_nextmore();
void f_text_init(WORD x, WORD y, WORD value, UBYTE mode);

UBYTE f_do_effect(UBYTE mode, UBYTE type, UBYTE guy);

void f_get_dotext();
void f_get_skill(struct DUDESTAT *st);
void f_get_item(UBYTE item);
UBYTE f_randbattle(UBYTE *info);
UBYTE f_brun_check();
WORD f_bwepdamage(UBYTE guy, UBYTE target);

UBYTE *f_get_expnext(UBYTE level);
void f_copyright();
UBYTE randfunc();
UWORD randword();

void f_map_restate();

void f_bcalcmod();

void f_bdamageflux();
void f_bdamagecap();

UBYTE f_bitem_have();
UBYTE f_bitem_type();
void f_bitem_del();

UBYTE f_battlestart(UBYTE formation);
void f_sync_party();
void f_draw_dudes();
void f_resync();
void f_lose();


#define BOSS_COMPACT 0x01

struct BOSSDATA
{
   UWORD pal[8];
   UBYTE flags;
};


WORD getsin(BYTE angle);
WORD getcos(BYTE angle);
BYTE getarctan(WORD ratio);

void f_getdistxy(WORD xlen, WORD ylen, UBYTE dist);

BYTE f_get_action2();


void end_init();
void end_next();
void end_deinit();

void f_setbosspal();
void f_setaluthapal(UBYTE map);

