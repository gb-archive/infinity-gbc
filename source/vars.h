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

extern UBYTE debug_on;
extern UBYTE reset_ok, _gba;
extern UBYTE demo_key;

extern BYTE i, i2;
extern WORD s, s2;
extern UBYTE n, n2;
extern UWORD u, u2;

extern UBYTE k, k2;


extern UBYTE protect_rampages;

// temp item list (for equipping)
extern UBYTE temp_num;
extern UBYTE temp_list_type[];
extern UBYTE temp_list_num[];

#define treasurestr ((char *)temp_list_type)
#define bvicstr temp_list_type
#define bdata temp_list_type
#define zoneinfo temp_list_type

extern UBYTE curzone, treasuremode;
//extern char treasurestr[32];

extern UBYTE gametime[];
extern UBYTE vblank_timer;
extern UBYTE barg1, barg2, barg3, barg4, barg5;
extern UWORD warg1, warg2, warg3, warg4, warg5;
extern UBYTE pfs_targetbgr, pfs_targetspr;
extern UBYTE flags[];
extern UBYTE grad_var1, grad_var2;
extern UBYTE grad_rgb1[3];
extern UBYTE grad_rgb2[3];
extern UBYTE grad_every;

extern UBYTE pstr_curtile;

extern UBYTE realdirs[];

extern UBYTE text_auto;
extern UBYTE vars[];
extern UBYTE regs[];
extern UBYTE area, lastsaveslot;
extern UBYTE partysize;
extern UBYTE party[4];
extern UBYTE realparty[4];
extern struct DUDESTAT stats[];
extern struct DUDESTAT *st;
extern BYTE key[10];			// key array
extern UBYTE tileset_cur;
extern UBYTE map_cur;
extern UBYTE ch_num, en_num, npc_num;
extern UWORD ch_base, en_base, npc_base;
extern UBYTE defaultcolors[];

extern UBYTE gold[3];
extern UBYTE mostgold[];
extern UBYTE encounter_zone, encounter_protect, encounter_steps, encounter_check;
extern UBYTE boat_x, boat_y, boat_dir, on_boat;
extern UBYTE map_state, hit_state;

extern UBYTE skl_name[10];
extern UBYTE skl_cost, skl_type, skl_gem, skl_item;
extern UBYTE gl_hp, gl_sp, gl_str, gl_end, gl_agl, gl_wis;

extern char menu_blank[];
extern char menu_dequip[];
extern char menu_cantsell[];
extern UBYTE sl_borderloc;
extern UBYTE scroll_location;
extern unsigned char menu_backup[];

extern UBYTE bcur_slot;
extern WORD bcur_destx;
extern WORD bcur_desty;
extern UBYTE bcur_dist;


// item list
extern UBYTE item_num;
extern UBYTE item_list_type[100];
extern UBYTE item_list_num[100];
extern UBYTE item_shop_at;
extern UBYTE item_shop[16];
extern UWORD item_cost[16];
extern UBYTE optionmode;
extern UBYTE options[7];

// gem list
extern UBYTE gems_list[9];
extern UBYTE gems_charge[9];
extern UBYTE gems_rel[9];
extern UBYTE gems_preorder[9];

// battle vars
extern UBYTE icon, icon_wiggle, icon_count;
extern UBYTE turn, inturn, next_turn, moved;
extern UBYTE pending;
extern UBYTE actions;
extern UWORD actioncount;
extern BYTE dx, dy;
extern UBYTE bmenu, bmenucount;
extern UBYTE flop;
extern UBYTE bframe;
extern BYTE req, lastreq, reqcount;
extern BYTE bselect;
extern UBYTE bloops, draw_gfx;
extern UBYTE bin_fade, bin_popup;

extern UBYTE b_target, b_dir, b_attrib;
extern WORD b_damage;

extern BYTE player_req;
extern UBYTE player_turn;
extern UBYTE player_tmode;

extern UWORD curframe;
extern WORD bsx, bsy, bdsx, bdsy;
extern WORD bcx, bcy, bdcx, bdcy;
extern UBYTE bmap[9][11];
extern UBYTE bnumdudes;

extern struct BDUDE *bch;
extern struct BCHAIN *bchain_table;
extern struct BPRO *bpro;
//extern UBYTE *bdata;

extern WORD px, py;

extern UBYTE bchain_active;
extern UBYTE bchain_steps;
extern BYTE bchain_at;

extern UBYTE bbox[];
extern UBYTE bboxpal[];

extern UBYTE bmenusquish, binputmode;
extern UBYTE btextdesc;
extern UBYTE bboxmenu;
extern UBYTE bchoosetarget;
extern UBYTE bchooseicon;
extern UBYTE bchoosetype;

extern UBYTE keylock[8];
extern BYTE bfocus;
extern UBYTE done, odone;
extern UBYTE badvance;

extern UBYTE bgemslide;
extern UBYTE bgemcharge1;
extern UBYTE bgemcharge2;
extern UBYTE bgemselect;
extern UBYTE bskillselect;
extern UBYTE bfreeze;
extern UBYTE bchnum, bennum;
extern UBYTE bformation;
extern UBYTE bcoloractive;
extern UBYTE bearly;
extern UBYTE bcritical;

extern UBYTE bflag_run, bflag_lose, bflag_sp_guy, bflag_sp_hp;
extern UBYTE bitem_tab[12], bitem_item;
extern UBYTE bwon_item, bwon_gp;
extern UBYTE brun_counter;

extern UBYTE bspr_quake;
extern UBYTE bfastestguy, bfastestspd;

extern UBYTE bsong;

// for bcalcmod()
extern UBYTE bcm_guy, bcm_target;
extern WORD bcm_damage;
