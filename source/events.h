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

#include "d1.h"

#define REGBASE      0xF8

#define MOREDELAY 30

#define CASE_STEP 0
#define CASE_TALK 1
#define CASE_SEARCH  2
#define CASE_GUY  3
#define CASE_HSTEP   4
#define CASE_VSTEP   5
#define CASE_QSTEP   6
#define CASE_TREASURE   7

#define ACTION_WALK  3
#define ACTION_CATCHUP  19
#define ACTION_SAY   1
#define ACTION_POSXY 0
#define ACTION_SLEEP 20
#define ACTION_GROUP 2
#define ACTION_WARP  0x06
#define ACTION_FRAME 0x45
#define ACTION_FACE  7

#define ACTION_LOCKON   0x34
#define ACTION_PAN   0x35
#define ACTION_LOCKOFF  0x36
#define ACTION_CAMERA   0x37

#define ACTION_LMAP  10
#define ACTION_LDUDE 9
#define ACTION_DEL   0x0B
#define ACTION_LGUY  0x0C

#define ACTION_CSTEP 0x1F
#define ACTION_CTALK 0x20
#define ACTION_CSEARCH  0x21
#define ACTION_CHSTEP   0x22
#define ACTION_CVSTEP   0x23

#define ACTION_SONG  0x24
#define ACTION_SFX   0x48
#define ACTION_CONTROL  0x25

#define ACTION_GOTO  0x27
#define ACTION_FADEOUT  0x28
#define ACTION_TILE  0x29

#define ACTION_IF 0x15
#define ACTION_SET   0x16

#define ACTION_ADD   0x17
#define ACTION_SUB   0x18
#define ACTION_MUL   0x19
#define ACTION_DIV   0x1a

#define ACTION_INC   0x1b
#define ACTION_DEC   0x1c

#define ACTION_GET   0x1d
#define ACTION_PUT   0x1e

#define ACTION_GIVE  0x30
#define ACTION_TAKE  0x31
#define ACTION_HAVE  0x32

#define ACTION_PARTYSET 0x40
#define ACTION_PARTYNOR 0x41
#define ACTION_PARTYARR 0x42

#define ACTION_PANSPEED 0x43
#define ACTION_FADEIN   0x44

#define ACTION_REFORM   0x46
#define ACTION_DETACH   0x47

#define ACTION_CALL  0x49

#define ACTION_CGOLD 0x4a
#define ACTION_TGOLD 0x4b
#define ACTION_SHOPRES  0x4c
#define ACTION_SHOPADD  0x4d
#define ACTION_SHOP  0x4e
#define ACTION_YESNO 0x4f
#define ACTION_SAVE  0x50

#define ACTION_ZONE  0x51
#define ACTION_GETXY 0x52

#define ACTION_TREASURE 0x53
#define ACTION_HEALALL  0x54
#define ACTION_OPMENU   0x55
#define ACTION_COLSHIFT 0x56
#define ACTION_CIF   0x57
#define ACTION_PROPHECY 0x58
#define ACTION_TBASE 0x59

#define ACTION_BATTLE   0x5a
#define ACTION_SONGOFF  0x5b
#define ACTION_SETSKL   0x5c

#define ACTION_SONGPAUSE  0x5d
#define ACTION_SONGRESUME 0x5e

#define ACTION_CQSTEP   0x5f

#define ACTION_GIVEGEM  0x60
#define ACTION_REVIVE   0x61
#define ACTION_SETFORM  0x62
#define ACTION_EQUIP    0x63

#define ACTION_WALK2 0x64
#define ACTION_BOATON   0x65
#define ACTION_BOATOFF  0x66

#define ACTION_MAPSTATE 0x67
#define ACTION_HITSTATE 0x68

#define ACTION_QUAKE 0x69
#define ACTION_SETZ  0x70
#define ACTION_SETWOLF  0x71
#define ACTION_RAND  0x72
#define ACTION_FADEIN2  0x73
#define ACTION_PANREL   0x74

#define ACTION_PALSET   0x75
#define ACTION_PALNORM  0x76
#define ACTION_LEVELUP  0x77
#define ACTION_SETBOAT  0x78
#define ACTION_SHOPADD2 0x79
#define ACTION_SETWOLFP 0x7a

#define ACTION_GIVE2    0x7b
#define ACTION_SETSKL2  0x7c

#define ACTION_ANIM     0x7d
#define ACTION_WARP2    0x7e
#define ACTION_LDUDEHI  0x7f
#define ACTION_DARKEN   0x6a

#define ACTION_COLSHIFT2 0x6b

#define ACTION_DEQUIP   0x6c
#define ACTION_SAYAUTO  0x6d

#define ACTION_LASTMOVS 0x6e
#define ACTION_DRAWDUDES 0x6f

#define ACTION_UNFRAME 0x80

#define ACTION_BSONG 0x81

#define ACTIONSIZE   12
#define MAXACTIONS   8
#define MAXCASES        36

BYTE numactions;
BYTE action_table[MAXACTIONS][ACTIONSIZE];
UWORD actionp, actionptmp[12];
UBYTE actionpstack;
UBYTE in_event;
UBYTE action_catchup;
WORD action_sleep;
UBYTE isyesno;
BYTE numcases;
BYTE event_cases[MAXCASES][8];
UWORD tbase;
UBYTE tbaselen, tbasemul;
UBYTE battleresult;

extern UBYTE pfs_targetspr, pfs_targetbgr;
extern UWORD pal_cur[];
extern UWORD great_dark_fade_pal[];

void reset_events()
{
   numcases = 0;
   in_event = 0;
}

void init_event(UWORD eventnum)
{
   numactions = 0;
   in_event = 1;
   control = 0;

   action_catchup = 0;
   actionp = eventnum;
   actionpstack = 0;
   isyesno = 0;
   battleresult = BATTLE_WIN;
   bsong = 0;
}

UBYTE getreg(UBYTE op1)
{
   if(op1 == 255)
      return 255;

   if(op1 >= REGBASE)
      op1 = regs[op1-REGBASE];

   return op1;
}

UBYTE enext()
{
   return (getreg(get_event_byte(actionp++)));
}

UBYTE enextraw()
{
   return (get_event_byte(actionp++));
}

void dialogue_scroll(UBYTE n);

void do_text(UWORD num)
{
   BYTE c;
   BYTE n;
   UBYTE tbyte;
   UWORD offset;
   UBYTE count, state;
   UBYTE newlines;
   char *p;
   BYTE reqline, index;

   if(tbase && num >= tbase && num < tbase + tbaselen) {
      num += (UWORD)tbaselen * tbasemul;
   }

   sync_party();
   draw_dudes();

// dfs_pschar(128, 0, 'A');
// dfs_pschar(128,10, 'A');
// while(1);

   owe_off();
   openwindow();

   if(!treasuremode)
      dialog_setup(num);
   else
      p = treasurestr;

        index = 0;
top:
   dialogue_scroll(0);
   newlines = 0;
   reqline = 0;

   while(1) {
      // room for this word?
      if(!treasuremode) {
         if(checkwindowlen())
            reqline = 1;
      }

      if(reqline) {
                        // if index == -1, then don't reset index since we'll lose indentation
                        if(index == -1)
                                index = 1;
                        else
                 index = 0;

         reqline = 0;

         ++newlines;
         if(newlines == 3)
            break;
         win_newline();
         continue;
      }

      // get a byte
      do {
         tbyte = treasuremode ? *p++ : get_dialog_byte();
      } while(tbyte == ' ' && index == 0);

      if(!tbyte)
         break;
      if(tbyte == '|') {
         reqline = 1;
                        index = -1; // don't destroy indentation
         continue;
      }

      win_pchar(tbyte);
      //pflush();
      ++index;

                if(!text_auto) {
                        input();
                        if(!key[ENTER]) {
                                do_delay(2);
                        }
                }
                else {
                        do_delay(4);
                }
   }

   if(!tbyte) {
                if(!text_auto) {
                        w4vbl_done();
                        do { input(); } while(key[ENTER]);
                        w4vbl_done();
                        do { input(); } while(!key[ENTER]);
                        w4vbl_done();
                        do { input(); } while(key[ENTER]);
                }
                else {
                        do_delay(128);
                }

                goto bottom;
        }

        if(!text_auto) {
                show_moreicon();
                w4vbl_done();

                do { input(); } while(key[ENTER]);

                count = 0;
                state = 0;
                while(1) {
                        w4vbl_done();
                        c = inputb();
                        if(c == ENTER) {
                                break;
                        }

                        ++count;
                        if(count == MOREDELAY) {
                                count = 0;
                                state ^= 1;
                                if(state)
                                        hide_moreicon();
                                else
                                        show_moreicon();
                        }
                }
                hide_moreicon();
        }
        else {
                do_delay(128);
        }


   for(n = 0; n <= 48; n += 2) {
      w4vbl_done();
      dialogue_scroll((UBYTE)n);
   }

   for(n = 0; n < 10; ++n)
      w4vbl_done();

   if(tbyte) {
      resetupwindow();
      //pflush();
      goto top;
   }

   w4vbl_done();
   do { input(); } while(key[ENTER]);

bottom:
   w4vbl_done();
   grad_on();
   do_delay(4);

   if(isyesno) {
      n = f_do_yesnomenu();
      regs[isyesno-1] = n;
      isyesno = 0;
   }

   closewindow();
   owe_on();
}

UBYTE pass_c;

BYTE get_action()
{
   UBYTE c;
   BYTE i, i2;
   WORD x, y;
   UBYTE num;
   UBYTE n, n2;
   UBYTE type;
   UBYTE dir, con;
   UBYTE tileset;
   UBYTE op1, op2, reg, imm;
   UWORD var, eve;
   UBYTE lo, hi;
   UBYTE ask[3];
   struct DUDESTRUCT *ds;

   if(numactions >= MAXACTIONS)
      while(1);

   if(numactions == 0 && action_sleep == -1)
      action_catchup = 0;

   if(action_catchup) {
      if(action_sleep == -1) {
         return -1;
      }

      --action_sleep;
      if(action_sleep == 0) {
         action_catchup = 0;
      }
      else
         return -1;
   }

   c = enextraw();
   if(c == 255) {
      if(actionpstack) {
         actionp = actionptmp[--actionpstack];
         return 1;
      }
      else {
         in_event = 0;
         return 0;
      }
   }

   local_event = event_cases[numcases];

   switch(c) {
      case ACTION_WALK:
      case ACTION_WALK2:
         local_action = action_table[numactions];
         local_action[0] = ACTION_WALK;
         n = enext();
         local_action[1] = n;
         local_action[2] = enext();
         n2 = enext();

         // if it's not a townsperson, then double the movement
         if(c != ACTION_WALK2) {
            if(n < 4 || ch[n].type == TYPE_BOAT)
               n2 <<= 1;
         }
         local_action[3] = n2;

         if(ch[n].active)
            ++numactions;
         break;

      case ACTION_WARP:
                case ACTION_WARP2:
         ds = &ch[enext()];
         x = enext();
         y = enext();
                        if(c == ACTION_WARP) {
                                x <<= 1;
                                y <<= 1;
                        }
         dir = enext();

         ds->x = x;
         ds->y = y;
         ds->gx = x * 8;
         ds->gy = y * 8;
         ds->dir = dir;
         break;

      case ACTION_FRAME:
         ds = &ch[enext()];
         op1 = enext();

         frame = getbaseframe(ds->type);
         ds->framemode = 1;
         ds->frame = frame + op1;
         break;

      case ACTION_ANIM:
         ch[enext()].animmode = 1;
         break;

      case ACTION_UNFRAME:
         ds = &ch[enext()];
         ds->dir = talk_dir; // face direction of most recent talk action
         ds->framemode = 0;
         ds->animmode = 0;
         break;

      case ACTION_CATCHUP:
         action_catchup = 1;
         action_sleep = -1;
         break;

      case ACTION_SAYAUTO:
         text_auto = enext();
         break;

      case ACTION_SAY:
         lo = enextraw();
         hi = enextraw();
         var = (UWORD)hi << 8;
         var += lo;

         do_text(var);
         resync();
         break;

      case ACTION_POSXY:
         local_action = action_table[numactions];
         local_action[0] = c;
         local_action[1] = enext() << 1;
         local_action[2] = enext() << 1;
         ++numactions;
         break;

      case ACTION_SLEEP:
         action_catchup = 1;
         action_sleep = enext();
         break;

      case ACTION_GROUP:
         if(on_boat) {
            for(n = 1; n < 4; ++n) {
               ds = &ch[n];
               ds->x = ch[0].x;
               ds->y = ch[0].y;
               ds->gx = ch[0].gx;
               ds->gy = ch[0].gy;
               ds->dir = ch[0].dir;
            }
            reset_lastmoves();
         }
         else {
            local_action = action_table[numactions];
            local_action[0] = c;
            local_action[1] = 8;
            ++numactions;
         }
         break;

      case ACTION_FACE:
         ds = &ch[enext()];
         ds->dir = enext();
         ds->framemode = 0;
         ds->animmode = 0;
         break;

      case ACTION_LOCKON:
         scroll_guy = enext();
         scroll_on = 1;
         break;

      case ACTION_PAN:
                        if(map_state)
                 scroll_x = (enext()&31) + (map_state << 5);
                        else
                                scroll_x = enext();
         scroll_y = enext();
         scroll_on = 2;
         break;

      case ACTION_PANREL:
         sx += (BYTE)enextraw() << 3;
         sy += (BYTE)enextraw() << 3;
         scroll_on = 0;
         break;

      case ACTION_LOCKOFF:
         scroll_on = 0;
         break;

      case ACTION_CAMERA:
         x = (WORD)enext() << 4;
         y = (WORD)enext() << 4;
                  sx = x - 80 + 8;
                   sy = y - 72 + 8;
         if(!allow_scroll)
            fix_camera();
         scroll_on = 0;
         owe_redraw(sx, sy);
         break;

      case ACTION_LMAP:
         num = enext();
         x = enext();
         y = enext();

         deldudes();

         loadmap(num, x, y);

                        // set alutha special-case palette, if an alutha map is detected
                        f_setaluthapal(num);

         //copy_page(1, 3);
         //debug_on = 1;

         // load in the party
         load_party(x, y);

         numcases = 0;

         ch[0].req = NORM;

              anim = 0;
         pullparty = 1;
         whotalk = -1;
         instep = 1;
         insearch = 0;
         encounter_steps = ((UBYTE)randfunc()%64) + 2;
         encounter_protect = 0;
         encounter_zone = 0;
         map_state = hit_state = 0;
         in_fade = 0;
         show_realparty = 0;

         game_loops = 0;
         vblank_timer = 0;
         atmenu = 0;

         sync_party();
         draw_dudes();
         break;

      case ACTION_LDUDE:
         num = enext();
         type = enext();
         n = enext();
         n2 = enext();
         dir = enext();
         con = enext();

         f_loaddude(num, n, n2, type, dir, con);
         break;

                case ACTION_LDUDEHI:
                        num = enext();
                        frame = getbaseframe(enext());
                        n = enext();
                        n2 = enext();
                        op1 = enext();

                        // hi-sprites aren't normal dudes
                        owe_spr(num, (WORD)n << 4, (WORD)n2 << 4, 0, frame + op1);
                        break;
                case ACTION_DRAWDUDES:
                        draw_dudes();
                        break;

      case ACTION_DEL:
         n = enext();
         deldude(n);
         break;

      case ACTION_LGUY:
         type = enext();
         n = enext();
         n2 = enext();
         lo = enextraw();
         hi = enextraw();

         // find a free spot
         for(i = 4; i != numdudes; ++i) {
            if(!ch[i].active)
               break;
         }

         // load the guy
         f_loaddude(i, n, n2, type, DOWN, WALK);

         // associate the talking event
         local_event[0] = CASE_GUY;
         local_event[1] = lo;
         local_event[2] = hi;
         local_event[3] = i;
         ++numcases;
         break;

      case ACTION_SONG:
         /*if(!demo_key)
            play_song(enext());
         else
            enext();
         break;*/
#ifndef NO_MUSIC
         play_song(enext());
#else
         enext();
#endif
         break;

      case ACTION_BSONG:
         bsong = (enext() + 1);
         break;

      case ACTION_SFX:
         play_sfx(enext());
         break;

      case ACTION_CONTROL:
         control = enext();
         ch[0].req = NORM;
         break;

      case ACTION_GOTO:
      case ACTION_CALL:
         lo = enextraw();
         hi = enextraw();
         if(c == ACTION_CALL)
            actionptmp[actionpstack++] = actionp;
         actionp = (UWORD)hi << 8;
         actionp += lo;
         break;

      case ACTION_FADEIN:
      case ACTION_FADEIN2:
         op1 = enext();

         owe_on();
         sync_party();
         draw_dudes();
         do_delay(4);

         if(op1 == 0 || op1 == 2 || op1 == 3) {
                                n2 = op1 == 2 ? 180: 32;
//            fadeout(n2);

            n = op1 == 3 ? COLOR_WHITE : COLOR_BLACK;

            fadefrom(n, n2);
            if(c == ACTION_FADEIN2) {
               in_fade = n2;
            }
            else {
               for(n = 0; n != n2; ++n) {
                  w4vbl_done();
                  fade_step();
               }
            }
         }
         else if(op1 == 1) {
            if(map_cur)
               owe_off();
            else
               owe_deinit();

            // start the transition
            scl_init();
            scl_roll(108);
            w4vbl_done();
            setcurpal();
            for(n = 0; n < 108; n+=2) {
               do_delay(1);
               scl_roll(108 - n);
            }
            w4vbl_done();
            grad_off();
            scl_off();
            owe_on();
         }

         resync();
         game_mode = 1;
         return 0;
         break;

      case ACTION_FADEOUT:
         do_delay(4);
         op1 = enext();
         if(op1) {
            if(map_cur)
               owe_off();
            else
               owe_deinit();
         }

         //owe_off();

         if(op1 == 0 || op1 == 2 || op1 == 3) {
                                n2 = op1 == 2 ? 180: 32;
//            fadeout(n2);

            n = op1 == 3 ? COLOR_WHITE : COLOR_BLACK;

            fadeto(n, n2);
            for(n = 0; n != n2; ++n) {
               w4vbl_done();
               fade_step();
            }
         }
         else if(op1 == 1) {
            // start the transition
            scl_init();
            for(n = 0; n < 108; n+=2) {
               do_delay(1);
               scl_roll(n);
            }
            scl_roll(108);
            w4vbl_done();
            grad_off();
            scl_off();
            blackout();
         }

         owe_deinit();

         resync();
         game_mode = 0;
         return 0;
         break;

      case ACTION_TILE:
         n = enext();
         n2 = enext();
         num = enext();

         //debug_on = 0;

         map_settype(n, n2, num);
         //copy_page(1, 3);
         //debug_on = 1;
         break;

      // logic
      case ACTION_IF:
      case ACTION_CIF:
         op1 = enext();
         n = enext();
         op2 = enext();
         lo = enextraw();
         hi = enextraw();

         eve = (UWORD)hi << 8;
         eve += lo;

         if((n == 0 && op1 > op2) ||
            (n == 1 && op1 >= op2)||
            (n == 2 && op1 == op2)||
            (n == 3 && op1 <= op2)||
            (n == 4 && op1 < op2) ||
            (n == 5 && op1 != op2)) {
            if(c == ACTION_CIF)
               actionptmp[actionpstack++] = actionp;
            actionp = eve;
         }
         break;

      case ACTION_SET:
      case ACTION_ADD:
      case ACTION_SUB:
         op1 = enextraw();
         op2 = enext();

         if(op1 < REGBASE)
            while(1);
         n = op1 - REGBASE;
         op1 = getreg(op1);
         if(c == ACTION_ADD)
            n2 = op1 + op2;
         else if(c == ACTION_SUB)
            n2 = op1 - op2;
         else
            n2 = op2;

         regs[n] = n2;
         break;

      /*case ACTION_ADD:
         op1 = enextraw();
         op2 = enext();

         if(op1 < REGBASE)
            while(1);
         n = op1 - REGBASE;
         op1 = getreg(op1);
         regs[n] = op1 + op2;
         break;

      case ACTION_SUB:
         op1 = enextraw();
         op2 = enext();

         if(op1 < REGBASE)
            while(1);
         n = op1 - REGBASE;
         op1 = getreg(op1);
         regs[n] = op1 - op2;
         break;*/

      case ACTION_INC:
      case ACTION_DEC:
         op1 = enextraw();
         if(op1 < REGBASE)
            while(1);
         n = op1 - REGBASE;
         op1 = getreg(op1);
         regs[n] = op1 + (c == ACTION_INC ? 1: -1);
         break;

      case ACTION_GET:
         op1 = enextraw();
         lo = enextraw();
         hi = enextraw();
         var = (UWORD)hi << 8;
         var += lo;

         n2 = vars[var];
         if(op1 < REGBASE)
            while(1);
         n = op1 - REGBASE;
         regs[n] = n2;
         break;

      case ACTION_PUT:
         op1 = enext();
         lo = enextraw();
         hi = enextraw();
         var = (UWORD)hi << 8;
         var += lo;

         vars[var] = op1;
         break;

      // party related
      case ACTION_PARTYSET:
         op1 = enext();
         op2 = enext();
         lo = realparty[op1];
         realparty[op1] = op2;

         for(n = 0; n != 4; ++n) {
            if(lo) {
               if(party[n] == lo)
                  break;
            }
            else {
               if(!party[n])
                  break;
            }
         }
         party[n] = op2;

         break;

      case ACTION_LEVELUP:
         op1 = enext();
         st = get_realparty(op1);
         if(st)
            f_catchup_level(st);
         break;

      case ACTION_PARTYNOR:
         show_realparty = 0;
         sync_party();
         draw_dudes();
         break;

      case ACTION_PARTYARR:
         show_realparty = 1;
         sync_party();
         draw_dudes();
         break;

      case ACTION_PANSPEED:
         owe_panspeed(enext());
         break;

      case ACTION_REFORM:
         show_realparty = 0;
         //sync_party();
         //draw_dudes();
         pullparty = 1;
         load_party(ch[0].x >> 1, ch[0].y >> 1);
                        sync_party();
         draw_dudes();
         break;

      case ACTION_DETACH:
         pullparty = 0;
         break;

      case ACTION_CGOLD:
         op1 = enextraw();
         op1 = op1 - REGBASE;

         lo = enextraw();
         hi = enextraw();
         var = (UWORD)hi << 8;
         var += lo;

         set_24(ask, var);
         if(cmp_24(gold, ask) >= 0)
            regs[op1] = 1;
         else
            regs[op1] = 0;
         break;

      case ACTION_TGOLD:
         lo = enextraw();
         hi = enextraw();
         var = (UWORD)hi << 8;
         var += lo;

         if(var) {
            set_24(ask, var);
            if(cmp_24(gold, ask) >= 0)
               sub_24(gold, ask);
            else
               set_24(gold, 0);
         }
         else
            set_24(gold, 0);

         break;

      case ACTION_GIVE:
         n = enext();
         get_item(n);
         break;

      case ACTION_SHOPRES:
         f_shop_reset();
         break;

      case ACTION_SHOPADD:
      case ACTION_SHOPADD2:
         n = enext();

         if(c == ACTION_SHOPADD2) {
            lo = enextraw();
            hi = enextraw();
            var = (UWORD)hi << 8;
            var += lo;
         }
         else {
            var = item_buycost(n);
         }
         //var = 0;
         f_shop_add(n, var);
         break;

      case ACTION_SHOP:
      case ACTION_OPMENU:
         owe_off();

         optionmode = c == ACTION_OPMENU ? 1: 0;
         f_do_shopmenu();

         // in case the party order was altered
         sync_party();

         owe_spr_reset();
         draw_dudes();

         owe_on();
         resync();
         break;

      case ACTION_SAVE:
         owe_off();

         start_gradient_slm();

         // predraw some stuff
         menu_drawbox(0, 0, 20, 3);
         reset_str(0);
         sl_borderloc = read_str();
         ppstr(1, 1, 52, 18, "Save Game");

         if(map_cur == 0)
            setdudepal();

         slm2_init();
         f_do_slmenu(1);
         slm_off();

         // in case the party order was altered
         sync_party();

         owe_spr_reset();
         draw_dudes();

         owe_on();
         do_delay(2);

         resync();
         break;

      case ACTION_YESNO:
         op1 = enextraw();
         op1 -= REGBASE;
         isyesno = op1 + 1;
         break;

      case ACTION_ZONE:
         encounter_zone = enext();
         break;

      case ACTION_TREASURE:
         // flag, x, y, gfx, sfx, contents
         n = enext();
         lo = enext();
         hi = enext();
         op1 = enext();
         op2 = enext();
         n2 = enext();
         // already set?  show it as opened
         if(flag_get(n)) {
            if(op1) {
               map_settype(lo, hi, op1);
               //map[(UWORD)hi * maplenx + lo] = op1;
               //copy_page(1,3);
               //debug_on = 1;
               //owe_update(lo, hi);
            }
         }
         // otherwise add to cases
         else {
            local_event = event_cases[numcases];
            local_event[0] = CASE_TREASURE;     // search
            local_event[1] = op1;
            local_event[2] = op2;
            local_event[3] = lo;    // x
            local_event[4] = hi;    // y
            //local_event[5] = 16;
            local_event[5] = n;
            local_event[6] = n2;
            ++numcases;
         }
         break;

      case ACTION_HEALALL:
      case ACTION_REVIVE:
         for(n = 0; n != 4; ++n) {
            st = get_party(n);
            if(!st)
               continue;

            if(c == ACTION_REVIVE) {
               if(st->hp == 0)
                  st->hp = 1;
            }
            else {
               st->hp = st->hpmax;
               st->sp = st->spmax;
            }
         }
         break;

      case ACTION_COLSHIFT:
         f_colorshift(enext());
         break;

      case ACTION_COLSHIFT2:
         op1 = pfs_targetspr;
         pfs_targetspr = 0;
         pfs_setup(pal_cur, (UWORD *)enext(), 32);
         pfs_targetspr = op1;
         in_fade = 16;
         break;

      case ACTION_DARKEN:
         op1 = pfs_targetspr;
         pfs_targetspr = 0;
         pfs_setup(pal_cur, (UWORD *)COLOR_GRDKISLE, 64);
         pfs_targetspr = op1;
         in_fade = 48;
         break;

      case ACTION_PROPHECY:
         f_prophecy();
         break;

      case ACTION_TBASE:
         lo = enextraw();
         hi = enextraw();
         tbase = (UWORD)hi << 8;
         tbase += lo;
         tbaselen = enext();
         tbasemul = enext();
         break;

      case ACTION_BATTLE:
         n = enext();
         bflag_run = enext();
         bflag_lose = enext();
         bflag_sp_guy = enext();
         bflag_sp_hp = enext();

         evebattle = 1;
         battleresult = f_battlestart(n);
         evebattle = 0;
         if(battleresult == BATTLE_DEMOSKIP) {
            init_event(0x0027);
            break;
         }

         if(battleresult == BATTLE_LOSE)
            return 0;
         break;

      case ACTION_SONGOFF:
              stop_song();
         break;

      case ACTION_SONGPAUSE:
              pause_song();
         break;

      case ACTION_SONGRESUME:
              resume_song();
         break;

      case ACTION_SETSKL:
      case ACTION_SETSKL2:
         n = enext();
         op1 = enext();
         op2 = enext();
         st = get_realparty(n);
         if(st) {
            st->skl[op1] = op2;
            if(c == ACTION_SETSKL) {
               f_load_skill(op2 - 1);
               get_skill(st);
            }
         }
         break;

      case ACTION_GIVEGEM:
         play_sfx(SFX_SEARCH);
         n = enext();
         get_item(gems_preorder[n] + GEMS_INDEX);
         break;

      case ACTION_SETFORM:
         for(n = 0; n != 4; ++n) {
            party[n] = realparty[n];
         }
         break;

      case ACTION_BOATON:
         boatmode();
         break;

      case ACTION_MAPSTATE:
         map_state = enext();
         f_map_restate();
         draw_dudes();
         set_camera();
         owe_redraw(sx, sy);
         break;

      case ACTION_HITSTATE:
         hit_state = enext();
         break;

      case ACTION_QUAKE:
         bspr_quake ^= 1;
         break;

      case ACTION_SETZ:
         n = enext();
         ch[n].gz = enext();
         break;

      case ACTION_SETWOLF:
         n = enext();
         stats[3].type = n ? VICTOR_WOLF: VICTOR;
                        if(n == 0)
                           strcpy(stats[3].class, "Guide");
         break;
      case ACTION_SETWOLFP:
         n = enext();
         stats[3].wolfpow = n;
         f_stats_recalc(&stats[3]);
         break;

      case ACTION_RAND:
         op1 = enextraw();
         regs[op1 - REGBASE] = (UBYTE)randfunc() % enext();
         break;

      case ACTION_PALSET:
         enext();
                        w4vbl_done();
         whiteout();
         break;
      case ACTION_PALNORM:
                        w4vbl_done();
         setcurpal();
         break;
      case ACTION_SETBOAT:
         boat_x = enext();
         boat_y = enext();
         break;

      default:
                        pass_c = c;
                        return f_get_action2();
   }

   return 1;
}

void get_actions()
{
   BYTE c;

   while(1) {
      c = get_action();
      if(c == 0 || c == -1) {
         break;
      }
   }
}

void eat_actions(BYTE num)
{
   BYTE n, n2;

   for(n = num; n < numactions; ++n) {
      for(n2 = 0; n2 < ACTIONSIZE; ++n2) {
         action_table[n][n2] = action_table[n+1][n2];
      }
   }
   --numactions;
}

void cleanup_actions()
{
   BYTE n;

   for(n = 0; n < numactions; ++n) {
      if(action_table[n][0] == -1) {
         eat_actions(n);
      }
   }
}
