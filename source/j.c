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

/*****
   Infinity - Game Boy Color RPG

   Justin Karneges
   Affinix team
*****/

#include "defs.h"
#include "vars.h"

#define TYPE_INVIS   240

UBYTE omni_save, omni_count;

UBYTE game_loops;
UBYTE in_fade;
UBYTE game_mode;

UWORD frame;
UBYTE c;
UWORD w;
UBYTE evebattle;
UBYTE demo_reset;

BYTE *local_event;
BYTE *local_action;
BYTE act_type;

extern UBYTE atmenu, lastsong;

/* vars */
UBYTE *map;       // map pointer
WORD sx, sy;         // camera coordinates
struct DUDESTRUCT ch[12];  // character structs
UBYTE allow_scroll;
UBYTE numdudes;         // max characters on the map
UBYTE maplenx, mapleny;    // dimensions of the map
UBYTE control;       // player has control?
UBYTE pullparty;     // pull the party with player?
BYTE whotalk;        // who you're attempting to talk to
UBYTE prev_dir;         // use to set a guy's dir back after a talk
UBYTE talk_dir;      // what a guy's dir would be if not in framemode
UBYTE show_realparty;      // show real party arrangment?

UBYTE instep;
BYTE insearch;       // attempting to search?
UBYTE searchx[2], searchy[2]; // the coords

UBYTE scroll_on;
UBYTE scroll_guy;
UBYTE scroll_x, scroll_y;

UBYTE nighttime;

UBYTE maindir;
//UBYTE zoneinfo[25];

BYTE mrel[8] = {
        0,-1,
        0, 1,
       -1, 0,
        1, 0
};

BYTE mrel2[8] = {
        0,              -MOVESPEED,
        0,              MOVESPEED,
        -MOVESPEED,     0,
        MOVESPEED,      0
};

BYTE flip[4] = {
   DOWN, UP, RIGHT, LEFT,
};

UBYTE walklist[] = { 3, 6, 8, 10, 13, 0xFF };

BYTE lastmoves[8];

void reset_lastmoves()
{
   //UBYTE n;

   memset(lastmoves, (UBYTE)-1, 8);

   /*for(n = 0; n != 8; ++n) {
      lastmoves[n] = -1;
   }*/
}


void menu();
void f_loaddude(UBYTE slot, UBYTE xpos, UBYTE ypos, UBYTE type, UBYTE dir, UBYTE control);

void deldude(UBYTE slot)
{
   owe_spr(slot, 0, 0, 0, (UWORD)-1);
   ch[slot].active = 0;
}

void deldudes()
{
   UBYTE i;

   // deactivate dudes
   for(i = 0; i < numdudes; ++i) {
      deldude(i);
   }
}

void resync()
{
   game_loops = 0;
   vblank_timer = 0;
}

void draw_dudes()
{
   UBYTE realdir;
   UBYTE boat_offset;
   struct DUDESTRUCT *ds;

   for(n = 0; n != numdudes; ++n) {
      ds = &ch[n];
      if(!ds->active)
         continue;

      if(ds->type == TYPE_INVIS) {
         owe_spr(n, 0, 0, 0, (UWORD)-1);
      }
      else {
         if(ds->framemode) {
            frame = ds->frame;
         }
                        else if(ds->animmode) {
                                frame = getbaseframe(ds->type);
                                frame += ds->animmode++ - 1;
                                if(ds->animmode == 8+1)
                                        ds->animmode = 1;
                        }
         else {
            realdir = realdirs[ds->dir];

            frame = getbaseframe(ds->type);
            frame += (realdir << 1);
            frame += ds->frame ? 1: 0;
         }
         boat_offset = ds->type == TYPE_BOAT ? 1: 0;

         owe_spr(n, ds->gx, ds->gy + boat_offset, ds->gz, frame);
      }
   }
}

void sync_party()
{
   UBYTE n;
   UBYTE at;

   if(on_boat)
      return;
   if(!ch[0].active)
      return;

   for(at = 0, n = 0; n != 4; ++n) {
      ch[n].type = TYPE_INVIS;
      st = show_realparty ? get_realparty(n): get_party(n);

/*    if(show_realparty) {
         st = get_realparty(n);
      }
      else {
         st = get_party(n);
      }*/

      if(!st)
         continue;
      if(st->hp == 0 && !show_realparty)
         continue;

      ch[at++].type = st->type;
   }
}

void boatmode();
void loadboat()
{
   f_loaddude(4, boat_x, boat_y, TYPE_BOAT, boat_dir, BOAT);
}

void load_party(UBYTE x, UBYTE y)
{
   UBYTE con;
   UBYTE n, n2;

   maindir = ch[0].dir;

   n2 = 0;
   for(n = 0; n != 4; ++n) {
      st = get_party(n);
      if(!st)
         continue;
      if(n2 == 0)
         con = PLAYER;
      else
         con = PARTY;
      f_loaddude(n2++, x, y, st->type, maindir, con);
   }
   for(n = n2; n != 4; ++n) {
      deldude(n);
   }

   // if overworld, load boat
   if(!map_cur) {
      if(on_boat) {
         on_boat = 0;
         boatmode();
      }
      else
         loadboat();
   }

   reset_lastmoves();
}

UBYTE srdir(UBYTE dir, UBYTE flags)
{
   return flags&(1<<dir);
}

UBYTE insquare_guy(UBYTE i, UBYTE x, UBYTE y)
{
   UBYTE tx, ty;

   tx = ch[i].x;
   ty = ch[i].y;

   if(ty >> 1 == y || (ty + 1) >> 1 == y)
      if(tx >> 1 == x || (tx + 1) >> 1 == x)
         return 1;

   return 0;
}

/*UBYTE touch_guy(UBYTE x, UBYTE n, BYTE dir)
{
   BYTE rx, ry;

        rx = mrel[(dir<<1)];
        ry = mrel[(dir<<1)+1];

   if(ch[x].x+rx   == ch[n].x || ch[x].x+rx   == ch[n].x+1
   || ch[x].x+rx+1 == ch[n].x || ch[x].x+rx+1 == ch[n].x+1) {
      if(ch[x].y+ry   == ch[n].y || ch[x].y+ry   == ch[n].y+1
      || ch[x].y+ry+1 == ch[n].y || ch[x].y+ry+1 == ch[n].y+1)
         return 1;
   }

   return 0;
}*/

UBYTE hitd_npc(UBYTE i, BYTE dir)
{
   UBYTE n;
   BYTE rx, ry;
   UBYTE x, y;

   rx = mrel[dir<<1];
   ry = mrel[(dir<<1)+1];
   x = (ch[i].x>>1) + rx;
   y = (ch[i].y>>1) + ry;

   if(owe_blocked(0, x, y))
      return 1;

   for(n = 0; n != numdudes; ++n) {
      if(n == i)
         continue;
      if(ch[n].control == PARTY)
         continue;
      if(insquare_guy(n, x, y))
         return 1;
   }

   return 0;
}

UBYTE hitd(UBYTE x, BYTE dir)
{
   struct DUDESTRUCT *ds;

   for(n = 1; n != numdudes; ++n) {
      ds = &ch[n];
      if(!ds->active)
         continue;
      if(ds->control == PARTY)
         continue;
      if(ds->control == BOAT)
         continue;

      if(f_touch_guy(x, n, dir))
         return 1;
   }
   return 0;
}

UBYTE hitd_boat(UBYTE x, UBYTE y, UBYTE dir)
{
   // only work on overworld
   if(map_cur)
      return 0;

   if(x+(mrel[dir<<1]<<1) == ch[4].x && y+mrel[(dir<<1)+1] == ch[4].y)
         return 1;

   return 0;
}

void do_text(UWORD num);

void get_dotext()
{
   UBYTE a;

   a = treasuremode;
   treasuremode = 1;
   do_text(0);
   treasuremode = a;
   resync();
}

void get_skill(struct DUDESTAT *st)
{
   strcpy(treasurestr, st->name);
   strcat(treasurestr, " gained ");
   strcat(treasurestr, skl_name);
   strcat(treasurestr, " skill!");

   play_sfx(SFX_LEVELUP);
   get_dotext();
}

void strcat_item(UBYTE item)
{
   char c;
   char icon[2];

   c = item_info(item)[0];
   if(c > 'A') {
      icon[0] = 7 - 'A' + c;
      icon[1] = 0;
      strcat(treasurestr, icon);
   }
   strcat(treasurestr, item_info(item) + 1);
}

void get_item(UBYTE item)
{
   f_items_add(item);

   strcpy(treasurestr, "Got ");
   strcat_item(item);
   strcat(treasurestr, "!");

   get_dotext();
}

void full_item(UBYTE item)
{
   strcpy(treasurestr, "Unable to hold ");
   strcat_item(item);
   strcat(treasurestr, "!");

   get_dotext();
}

void boatmode()
{
   UBYTE n;

   if(!on_boat) {
      ch[0].type = TYPE_BOAT;
      for(n = 1; n < 4; ++n)
         ch[n].type = TYPE_INVIS;
      deldude(4);
      on_boat = 1;
   }
   else {
      on_boat = 0;
      sync_party();

      boat_x = ch[0].x;
      boat_y = ch[0].y;
      boat_dir = ch[0].dir;
      loadboat();
   }
}

void set_camera()
{
   // camera alignment (nothing special going on here)
   if(scroll_on == 1) {
      sx = ch[scroll_guy].gx - 80 + 8;
      sy = ch[scroll_guy].gy - 72 + 8;
   }
   else if(scroll_on == 2) {
      sx = (WORD)scroll_x << 4;
      sy = (WORD)scroll_y << 4;
      sx = sx - 80 + 8;
      sy = sy - 72 + 8;
   }
   if(!allow_scroll)
      fix_camera();
}

void lose()
{
   stop_song();
   play_song(SNG_GAMEOVER);
   do_delay(60);
   do_text(0);
   do_delay(120);
}


BYTE a1, a2, a3, a4;
UBYTE b1, b2, b3, b4;

UWORD w1, w2;
UBYTE hi, lo;
UBYTE ok;
UBYTE anim;

UBYTE chx, chy;
extern UWORD rnd_seed;

#include "events.h"

void main_game()
{
// WORD dx, dy;
   BYTE dx, dy;
   UBYTE u;

   WORD x, y;
   UBYTE lx, ly;
        BYTE rx, ry;
   UBYTE n, n2;
   struct DUDESTRUCT *ds;

   //test_IR();
   //disable_interrupts();
   //while(receive_infrared());
   //enable_interrupts();

   init();

   f_copyright();
   
start:
   demo_reset = 0;
   asm_init();
   setwinpal();
   f_items_reset();

   /*input();
   if(key[CANCEL]) {
      goto no_title;
   }*/

   // run title screen
   play_song(SNG_TITLE);

#ifdef  NO_MUSIC
   stop_song();
#endif
   title_init();

   // setup gradient
   start_gradient_slm();
   slm_init();

   //enable_interrupts();

   // fade in the title pic
   fadein_title(60);
   for(n = 0; n != 60; ++n) {
      w4vbl_done2();
      fade_step();
   }

   // wait 1 second
   for(n = 0; n != 60; ++n) {
      w4vbl_done2();
   }

   // start the title screen!
   title_screen();

   setdudepal();
   a1 = f_do_slmenu(0);

   do_delay(60);

   b1 = pfs_targetbgr;
   b2 = pfs_targetspr;
   pfs_targetbgr = 0x1c;
   pfs_targetspr = 0;
   fadeout_title(60);
   for(n = 0; n != 60; ++n) {
      w4vbl_done3();
      fade_step();
   }
   slm_off();
   pfs_targetbgr = b1;
   pfs_targetspr = b2;

   stop_song();

   blackout();
   clear_vram();

no_title:
   reset_ok = 1;

   lastsong = 255;
   init_game();
   protect_rampages = 0;  // flag *only* when loading a map after battle

   if(a1 == 0) {
      f_reset_data();
      boat_x = 0; //162;
      boat_y = 0; //150;
      boat_dir = LEFT;

      //f_items_add(102);
      //f_items_add(107);
      //f_items_add(108);
      //f_items_add(110);
      //f_items_add(112);
      //f_items_add(115);
   }

//removed old init code
//   // forced init
////   stats[0].skl[2] = 4; // blaze
////   stats[0].skl[3] = 14; // justice
//   stats[4].e[0] =  IT_OBastard; // roland's weapons
//   stats[4].e[1] =  IT_HPlate;
//   stats[4].skl[3] =  13; // inversion
//   stats[4].skl[2] =  20; // whirlwind
//   stats[4].skl[0] =  2; // jolt
//   stats[4].skl[1] =  19; // slash
//   //stats[3].skl[3] =  18; // force wildcall on victor
//   //stats[3].type = VICTOR_WOLF;
//   stats[5].type = ALUTHA;
//   /*gems_list[0] = GEM_RUBY + 1;
//   gems_list[1] = GEM_CRYSTAL + 1;
//   gems_charge[0] = 99;
//   gems_charge[1] = 99;
//   for(n = 2; n < 9; ++n) {
//       gems_list[n] = 0;
//       gems_charge[n] = 0;
//   }*/

   load_gradcolors(options + 1);

   // safety for now
   if(options[0] > 4)
      options[0] = 4;

   regs[0] = area;
   numdudes = 12;  // max dudes in a single map
   tbase = 0;
   reset_events();

   owe_panspeed(2);
   allow_scroll = 0;
   control = 0;
   text_auto = 0;
   scroll_on = 1;
   scroll_guy = 0;
   show_realparty = 0;
   encounter_protect = 0;
   encounter_zone = 0;
   encounter_check = 0;
   treasuremode = 0;

   ch[0].dir = DOWN; // default face down

   //f_init_elya();

   // Begin!!
   init_event(0);

   on_boat = 0;

   omni_save = 0;
   omni_count = 0;

   nighttime = 0;

   while(1) {
      // non map
      while(game_mode == 0) {
         do_delay(2);
         if(in_event) {
            cleanup_actions();
            get_actions();
            if(battleresult == BATTLE_LOSE)
               goto start;
         }
      }

           while(game_mode == 1) {
         // keep things random!
              randfunc();

                        if(demo_reset)
                                goto start;

         // lead character locked?  unlock!
         if(ch[0].mode == STILL)
            ch[0].mode = NORM;

         // sync with vblank at 30fps
         while(game_loops > (vblank_timer >> 1))
            do_delay(DELAY);
         if(vblank_timer > 100) {
            game_loops = 0;
            vblank_timer = 0;
         }

         // draw guys
         draw_dudes();

         // position the camera
         owe_scrollto(sx, sy);

         if(in_fade) {
            fade_step();
            --in_fade;
         }

         // animate current guy
                        ds = &ch[anim];
         if(anim < 12) {
            if(ds->active && !ds->framemode && !ds->animmode) {
               ds->frame = ds->frame ? 0: 1;
            }
         }
         if(anim == 11)
            anim = 0;
         else
            ++anim;


         // check for event triggers
         if(!in_event && ch[0].mode == NORM) {
            ok = 0;
            chx = ch[0].x;
            if(map_state)
               chx &= 63;

            for(n = 0; n < numcases; ++n) {
               local_event = event_cases[n];

               n2 = local_event[0];
               if(n2 == CASE_STEP) {
                  b1 = chx;
                  b2 = ch[0].y+1;
                  if(b1&1)
                     continue;
                  b1 = (UBYTE)b1 >> 1;
                  b2 = (UBYTE)b2 >> 1;
                  b3 = local_event[3];
                  b4 = local_event[4];

                  if(b1 == b3 && b2 == b4) {
                     if(!instep) {
                        if(owe_blocked(2, b1, b2))
                           play_sfx(SFX_DOOROPEN);

                        lo = local_event[1];
                        hi = local_event[2];
                        init_event(((UWORD)hi << 8) + lo);
                     }
                     ok = 1;
                  }
               }
               if(n2 == CASE_QSTEP) {
                  b1 = chx;
                  b2 = ch[0].y + 1;
                  //if(a1&1)
                  // continue;
                  //b1 = (UBYTE)b1 >> 1;
                  //b2 = (UBYTE)b2 >> 1;
                  b3 = local_event[3] << 1;
                  b4 = local_event[4] << 1;

                  if(b1 >= b3 && b1 <= b3 + 2 && b2 >= b4 && b2 <= b4 + 3) {
                     if(!instep) {
                        lo = local_event[1];
                        hi = local_event[2];
                        init_event(((UWORD)hi << 8) + lo);
                     }
                     ok = 1;
                  }
               }
               if(n2 == CASE_HSTEP) {
                  b1 = chx;
                  b2 = ch[0].y;
                  if(b2&1)
                     continue;
                  b2 = (b2+1) >> 1;
                  b3 = local_event[3] << 1;
                  b4 = local_event[4];
                  u  = local_event[5] << 1;

                  if(b3 + u - 2 < b3)
                     b3 += 128, b1 += 128;

                  if(b2 == b4) {
                     if(b1 >= b3 && b1 <= b3 + u - 2) {
                        if(!instep) {
                           lo = local_event[1];
                           hi = local_event[2];
                           init_event(((UWORD)hi << 8) + lo);
                        }
                        ok = 1;
                     }
                  }
               }

               if(n2 == CASE_VSTEP) {
                  b1 = chx;
                  b2 = ch[0].y+1;
                  if((b1&1))
                     continue;
                  b1 = b1 >> 1;

                  b3 = local_event[3];
                  b4 = local_event[4] << 1;
                  u  = local_event[5] << 1;

                  if(b4 + u - 1 < b4)
                     b4 += 128, b2 += 128;

                  if(b1 == b3) {
                     if(b2 >= b4 && b2 <= b4 + u - 1) {
                        if(!instep) {
                           lo = local_event[1];
                           hi = local_event[2];
                           init_event(((UWORD)hi << 8) + lo);
                        }
                        ok = 1;
                     }
                  }
               }

               else if(n2 == CASE_TALK || n2 == CASE_GUY) {
                  if(whotalk == local_event[3]) {
                     a1 = whotalk;

                     talk_dir = flip[ch[0].dir];

                     if(!ch[a1].framemode) {
                        prev_dir = ch[a1].mode == NORM ? 4: ch[a1].dir;
                        ch[a1].dir = talk_dir;
                     }

                     draw_dudes();

                     whotalk = -1;

                     if(n2 == CASE_TALK) {
                        lo = local_event[1];
                        hi = local_event[2];
                        init_event(((UWORD)hi << 8) + lo);
                     }
                     else {
                        lo = local_event[1];
                        hi = local_event[2];
                        w1 = (UWORD)hi << 8;
                        w1 += lo;

                        do_text(w1);
                        resync();
                     }

                     if(!ch[a1].framemode) {
                        if(prev_dir != 4)
                           ch[a1].dir = prev_dir;
                     }
                  }
               }
               else if((n2 == CASE_SEARCH || n2 == CASE_TREASURE) && insearch) {
                  a1 = local_event[3];
                  a2 = local_event[4];
                  a3 = local_event[5];
                                                if(n2 == CASE_TREASURE) {
                          treasuremode = 1;
                                                }
                  else {
                                                        treasuremode = 0;
                     if(!srdir(ch[0].dir, a3))
                        continue;
                  }

                  if(map_state)
                     a1 &= 31;

                  for(a4 = 0; a4 != insearch; ++a4) {
                     if(a1 == searchx[a4] && a2 == searchy[a4]) {
                        insearch = 0;
                        lo = local_event[1];
                        hi = local_event[2];

                        // get these again
                        a1 = local_event[3];
                        a2 = local_event[4];

                        if(!treasuremode) {
                           init_event(((UWORD)hi << 8) + lo);
                        }
                        else  {
                           b1 = local_event[5];
                           b2 = local_event[6];
                           if(!flag_get(b1)) {
                              b3 = map_gettype(a1, a2);
                              if(lo) {
                                 //debug_on = 0;
                                 //copy_page(1,3);
                                 //debug_on = 1;
                                 map_settype(a1, a2, lo);
                              }

                              if(f_items_have(b2) < 100) {
                                 play_sfx(hi);
                                 flag_set(b1);
                                 get_item(b2);
                              }
                              else {
                                 full_item(b2);
                                 map_settype(a1, a2, b3);
                              }
                           }
                        }
                        break;
                     }
                  }
                  treasuremode = 0;
               }
            }

            instep = ok;
            // handle quick events
            if(in_event) {
               cleanup_actions();
               get_actions();
               if(battleresult == BATTLE_LOSE)
                  goto start;
               if(!in_event)
                  control = 1;
            }

            if(!in_event) {
               b1 = ch[0].x;
               b2 = ch[0].y + 1;
               // check for ice
               for(n = 0;; ++n) {
                  if(!owe_blocked(3, b1 >> 1, b2 >> 1) || !owe_blocked(3, (b1+1) >> 1, b2 >> 1) || owe_clip(b1, b2, ch[0].dir) != ch[0].dir+1)
                     break;
                  b1 += mrel[ch[0].dir << 1];
                  b2 += mrel[(ch[0].dir << 1) + 1];
               }
               if(n) {
                  regs[0] = ch[0].dir;
                  regs[1] = n;
                  regs[2] = realdirs[ch[0].dir] << 1;
                  init_event(0x0021);
               }

                                        // check for the edge of the world
                                        if(map_cur == 0) {
                                                if(owe_blocked(0, ch[0].x >> 1, (ch[0].y+1) >> 1) && owe_blocked(0, (ch[0].x+1) >> 1, (ch[0].y+1)>>1))
                                                        init_event(0x0024);
                                        }
            }
         }

                        if(encounter_check && !encounter_protect && !in_event) {
                                if(encounter_steps == 0) {
                                        encounter_steps = ((UBYTE)randfunc()%80) + 20;

                                        // overworld?
                                        f_memcpy((void *)&w, (void *)0x4000, EFORM_PAGE, 2);
                                        w += 0x4000;

                                        if(map_cur == 0)
                                                n2 = map_getzone((UBYTE)ch[0].x>>1, (UBYTE)(ch[0].y+1)>>1);
                                        else
                                                n2 = encounter_zone;

                                        if(n2) {
                                                --n2;
                                                w += ((UWORD)n2 * 25);
                                                f_memcpy(zoneinfo, (void *)w, EFORM_PAGE, 25);

                                                n2 = f_randbattle(zoneinfo);
                                                if(n2) {
                                                        evebattle = 0;
                                                        n2 = f_battlestart(n2-1);
                                                        if(n2 != BATTLE_WIN) {
                                                                if(n2 == BATTLE_RESCUE)
                                                                        init_event(0x002a);
                                                                else
                                                                        goto start;
                                                        }
                                                }
                                        }
                                }
                                else
                                        --encounter_steps;
                        }


                        input();

                        /*if(demo_key) {
                                if(key[CANCEL]) {
                                        while(key[CANCEL])
                                                input();
                                        init_event(0x0027);
                                }
                        }*/

         // get input
         if(control) {
            c = inputb();

            whotalk = -1;
            insearch = 0;

            if(c == ENTER && ch[0].mode == NORM) {
               // check to talk to someone
               for(n = 1; n != numdudes; ++n) {
                  ds = &ch[n];
                  if(!ds->active)
                     continue;
                  if(ds->control == PARTY)
                     continue;
                  if(f_touch_guy(0, n, ch[0].dir)) {
                     whotalk = n;

                     ch[0].mode = STILL;
                     break;
                  }
               }

               // check for search tile
               insearch = 1;

               rx = mrel[(ch[0].dir<<1)  ];
               ry = mrel[(ch[0].dir<<1)+1];

               chx = ch[0].x;
               if(map_state)
                  chx &= 63;

               a1 = chx;
               a2 = ch[0].y;

               if(ch[0].dir == UP)
                  a2++;
               if(ch[0].dir == LEFT)
                  a1++;

               searchx[0] = (a1>>1)+rx;
               searchy[0] = (a2>>1)+ry;

               // we may have to check a second tile
               // if player is disaligned

               n = 0;
               // facing left/right
               if(ch[0].dir & 2) {
                  if(a2 & 1) {
                     a2 += 1;
                     n = 1;
                  }
               }
               // facing up/down
               else {
                  if(a1 & 1) {
                     a1 += 1;
                     n = 1;
                  }
               }

               if(n) {
                  searchx[1] = (a1>>1)+rx;
                  searchy[1] = (a2>>1)+ry;

                  insearch = 2;
               }
            }

            /*if(c == CANCEL && ch[0].mode == NORM) {
               if(encounter_protect) {
                  encounter_protect = 0;
                  play_sfx(SFX_SWORD);
               }
               else {
                  encounter_protect = 1;
                  play_sfx(SFX_ESCAPE);
               }*/

               /*if(map_cur == 0) {
                  if(omni_count == 0)
                     omni_save = 0;

                  omni_count = 15;
                  ++omni_save;
                  if(omni_save == 3) {
                     init_event(30);
                     omni_save = 0;
                  }
               }*/
            //}
            if((c == MENU && ch[0].mode == NORM && in_fade == 0) || c == SELECT) {
                                        do_delay(2);
                                        owe_off();
                                        if(map_cur == 0) {
                                                fadeout(32);
                                                for(n = 0; n != 32; ++n) {
                                                        w4vbl_done();
                                                        fade_step();
                                                }
                                                owe_off2();
                                        }

                                        if(c == SELECT) {
                                                optionmode = 1;
                                                f_do_shopmenu();
                                        }
                                        else {
                                                menu();
                                        }


                                        if(map_cur == 0) {
                                                fade_backstep();
                                                fade_step();
                                        }

                                        // in case the party order was altered
                                        sync_party();

                                        owe_spr_reset();
                                        draw_dudes();
                                        owe_on();

                                        if(c == MENU) {
                                                do_delay(2);
                                                // check for warp (using encounter var)
                                                if(encounter_protect == 3) {
                                                        encounter_protect = 0;
                                                        game_mode = 0;
                                                        init_event(0x03);
                                                        continue;
                                                }

                                                // check for encounter protect
                                                if(encounter_protect == 2) {
                                                        encounter_protect = 1;
                                                        do_text(1);
                                                }
                                        }

                                        if(map_cur == 0) {
                                                fadein(32);
                                                for(n = 0; n != 32; ++n) {
                                                        w4vbl_done();
                                                        fade_step();
                                                }
                                        }
                                        resync();
                                }
         }

                   for(n = 0; n < numdudes; ++n) {
            if(ch[n].active) {
                            ch[n].req = NORM;
            }
         }


         // if in an event, do it or whatever
         if(in_event) {
            cleanup_actions();
            get_actions();
            if(battleresult == BATTLE_LOSE)
               goto start;
            if(game_mode != 1)
               break;

            for(u = 0; u < numactions; ++u) {
               local_action = action_table[u];
               act_type = local_action[0];

               if(act_type == -1)
                  continue;
               if(act_type == ACTION_WALK) {
                  i = local_action[1];
                  if(local_action[3] == 0 && ch[i].mode == NORM) {
                     local_action[0] = -1;
                  }
                  else {
                     if(ch[i].mode == NORM) {
                        ch[i].req = MOVE;
                        ch[i].dir = local_action[2];
                        --local_action[3];
                     }
                  }
               }
               if(act_type == ACTION_POSXY) {
                  dx = local_action[1];
                  dy = local_action[2];
                  if(ch[0].mode == NORM) {
                     if(ch[0].x == dx && ch[0].y == dy) {
                        local_action[0] = -1;
                     }
                     else {
                        ch[0].req = MOVE;

                        // clear horizontal
                        n2 = 0;

                        // always down last
                        n = DOWN;

                        // if we are below our point, go up
                        if(ch[0].y > dy)
                           n = UP;

                        // horizontal takes place first, so we detect it last
                        if(ch[0].x < dx)
                           n2 = RIGHT;
                        if(ch[0].x > dx)
                           n2 = LEFT;

                        // but if horizontal is not blocked then do horizontal first
                        if(n2)
                           if(owe_clip(ch[0].x, ch[0].y + 1, n2) - 1 == n2 && !hitd(0, n2))
                              n = n2;

                        // set our direction now
                        ch[0].dir = n;
                     }
                  }
               }
               if(act_type == ACTION_GROUP) {
                  if(local_action[1] == 0 && ch[3].mode == NORM) {
                     local_action[0] = -1;
                  }
                  else if(ch[3].mode == NORM && ch[2].mode == NORM && ch[1].mode == NORM) {
                     for(n = 0; n < 3; ++n) {
                        ds = &ch[n+1];
                        if(!ds->active)
                           continue;
                        if(ds->control != PARTY)
                           break;
                        if(lastmoves[(n<<1)+1] != -1) {
                           ds->req = MOVE;
                           ds->dir = lastmoves[(n<<1)+1];
                        }
                     }
                     for(n = 7; n != 0; --n) {
                        lastmoves[n] = lastmoves[n-1];
                     }
                     lastmoves[0] = -1;
                     --local_action[1];
                  }
               }
            }
         }
         else
            control = 1;


         if(ch[0].mode == NORM) {
            input();
            if(control) {
               if(key[UP]) {
                  ch[0].dir = UP;
                  ch[0].req = MOVE;
               }
               if(key[DOWN]) {
                  ch[0].dir = DOWN;
                  ch[0].req = MOVE;
               }
               if(key[LEFT]) {
                  ch[0].dir = LEFT;
                  ch[0].req = MOVE;
               }
               if(key[RIGHT]) {
                  ch[0].dir = RIGHT;
                  ch[0].req = MOVE;
               }

               if(ch[0].req != NORM) {
                  b1 = ch[0].x;
                  b2 = ch[0].y+1;
                  if(hit_state) {
                     b1 &= 63;
                     b1 += (hit_state-1) << 6;
                  }

                  if(!encounter_protect) {
                  n = owe_clip(b1, b2, ch[0].dir);
                  if(n) {
                     ch[0].dir = n - 1;
                  }
                  else {
                     ch[0].req = NORM;

                     // check for boat
                     if(!map_cur) {
                        if(!on_boat) {
                           if(hitd_boat(ch[0].x, ch[0].y, ch[0].dir)) {
                              init_event(6 + ch[0].dir * 3);
                              continue;
                           }
                        }
                        else {
                           n = owe_clip(ch[0].x+(mrel[ch[0].dir<<1]<<1), ch[0].y+mrel[(ch[0].dir<<1)+1]+1, ch[0].dir);
                           if(n - 1 == ch[0].dir) {
                              init_event(18 + ch[0].dir * 3);
                              continue;
                           }
                        }
                     }
                  }
                  }

                  if(hitd(0, ch[0].dir))
                     ch[0].req = NORM;
               }
            }

            if(ch[0].req == MOVE && pullparty) {
               for(n = 0; n != 3; ++n) {
                  if(!ch[n].active)
                     continue;
                  if(ch[n+1].control != PARTY)
                     break;
                  if(lastmoves[(n<<1)+1] != -1) {
                     ch[n+1].req = MOVE;
                        ch[n+1].dir = lastmoves[(n<<1)+1];
                  }
               }
               for(n = 5; n != 0; --n) {
                  lastmoves[n] = lastmoves[n-1];
               }
               lastmoves[0] = ch[0].dir;
            }
         }

         if(control && ch[0].req == NORM) {
            for(n = 4; n != numdudes; ++n) {
               ds = &ch[n];
               if(!ds->active)
                  continue;
               if(ds->mode != NORM)
                  continue;
               if(ds->control == WALK) {
                  n2 = getrand(25);
                  if(n2 != 24)
                     continue;
                  n2 = getrand(4);
                  if(!hitd_npc(n, n2)) {
                     ds->dir = n2;
                     ds->req = MOVE;
                     break;
                  }
               }
            }
         }

                   for(n = 0; n != numdudes; ++n) {
            ds = &ch[n];
            if(!ds->active)
               continue;
                           if(ds->req != NORM) {
                                 if(ds->req == MOVE) {
                                          rx = mrel[ds->dir<<1];
                                          ry = mrel[(ds->dir<<1)+1];

                                          ds->mode = MOVE;

                  if(n < 4 || ds->type == TYPE_BOAT) {
                                             ds->x += rx;
                                          ds->y += ry;
                                             ds->count = NUM_MOVES;
                  }
                  else {
                                             ds->x += rx * 2;
                                          ds->y += ry * 2;
                                             ds->count = NUM_MOVES_NPC;
                  }
                                 }
                           }
                  }

                        encounter_check = 0;
                        for(n = 0; n != numdudes; ++n) {
                                ds = &ch[n];
                                if(!ds->active)
                                        continue;
                                if(ds->mode == MOVE) {
                                        rx = mrel2[ds->dir<<1];
                                        ry = mrel2[(ds->dir<<1)+1];
                                        ds->gx += rx;
                                        ds->gy += ry;

                                        --ds->count;
                                        if(ds->count == 0) {
                                                if(n == 0 && !in_event)
                                                        encounter_check = 1; // flag for encounter check
                                                ds->mode = NORM;
                                        }
                                }
                        }

                        set_camera();
                        ++game_loops;

                        if(omni_count)
                                --omni_count;

                        // special case -- night
                        // commented out because we have a nighttime song now
                        /*if(vars[VAR_EVEPROG] == 4) {
                                ++nighttime;
                                if(nighttime >= (UBYTE)150) {
                                        play_sfx(SFX_NIGHTTIME);
                                        nighttime = 0;
                                }
                        }*/
         }
   }
}
