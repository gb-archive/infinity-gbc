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

// battle!

#include "defs.h"
#include "vars.h"


UBYTE rotpal;
UBYTE bblink[4];

UBYTE bdata_map, bdata_enum;
UWORD bdata_gold, bdata_exp;
UBYTE bname[5];

UBYTE bchain_finished, brun, bexit, bwon, bturndelay;
UBYTE brandframe;

void bgetreal(struct BDUDE *bch)
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

UBYTE bhitd(UBYTE guy, UBYTE dir)
{
   UBYTE n;

   f_get_dxdy(bch[guy].x, bch[guy].y, dir);

   for(n = 0; n != bnumdudes; ++n) {
      if(!bch[n].active)
         continue;

      if(dx == bch[n].x && dy == bch[n].y)
         return n + 1;
   }

   return 0;
}

void auto_popup()
{
   if(bin_popup) {
      bin_popup -= 4;
      setwindow(0, 112+bin_popup);
   }

   if(!bin_fade && !bin_popup)
      auto_proc(0);
}

void bautoproc()
{
   auto_proc(auto_popup);
}

UBYTE btextptr[4];

// redraw stats for a party member
void refresh_stats(UBYTE x)
{
   if(bch[x].control != BPLAYER)
      return;

   point_str(btextptr[x], 2);
   ipstr(0, 2, deci_16(bch[x].st.hp, 3));
}


BYTE turn_table[16];
UBYTE turn_reqs[16];

void turn_add(UBYTE guy, UBYTE req);
UBYTE turn_next_req;
BYTE turn_next();
UBYTE turn_at;

void turn_del(UBYTE guy)
{
   UBYTE n, n2;

   // remove guy
   n2 = 0;
   for(n = 0; n != turn_at; ++n) {
      if(turn_table[n] == guy) {
         ++n2;
         break;
      }
   }
   if(n2 == 0)
      return;

   memcpy(turn_table + n, turn_table + n + 1, (UWORD)turn_at - n - 1);
   memcpy(turn_reqs + n, turn_reqs + n + 1, (UWORD)turn_at - n - 1);
   --turn_at;
}

void turn_add(UBYTE guy, UBYTE req)
{
   turn_table[turn_at] = guy;
   turn_reqs[turn_at] = req;
   ++turn_at;
}

BYTE turn_next()
{
   BYTE c;
   UBYTE n;

   if(turn_at < 1)
      return -1;

   c = turn_table[0];
   turn_next_req = turn_reqs[0];

   if(bch[c].action != BACT_NORM)
      return -1;

   for(n = 0; n < bnumdudes - 1; ++n) {
      turn_table[n] = turn_table[n+1];
      turn_reqs[n] = turn_reqs[n+1];
   }
   --turn_at;

   if(!bch[c].active)
      return -1;

   return c;
}


void turn_init()
{
   inturn = 0;
   next_turn = 0;
   moved = 0;
   player_req = -1;
   player_turn = 0;
   player_tmode = 1;

   turn_at = 0;
   turn = 0;
}

UBYTE binqueue(UBYTE guy)
{
   UBYTE n;

   for(n = 0; n < turn_at; ++n) {
      if(turn_table[n] == guy)
         return 1;
   }
   return 0;
}

void bdel(UBYTE slot)
{
   if(!bch[slot].active)
      return;

   bch[slot].active = 0;
   bspr_16set(slot, 0, 0, 0, -1, -1);
   //--bennum;
}

void wipe_gem()
{
   UBYTE n;

   for(n = 16; n != 20; ++n)
      bspr_8set(n, 0, 0, -1, 0);
}

void wipe_menu()
{
   //UBYTE n;

   //for(n = 8; n != 16; ++n)
   // bspr_8set(n, 0, 0, -1, 0);
   bicon_reset();
}

void wipe_text()
{
   winpal(13, 1, 6, 1, 7);
   point_str(btextdesc, 10);
}

UBYTE get_gem(UBYTE guy)
{
   UBYTE n;
   struct DUDESTAT *st;

   if(bch[guy].gem) {
      st = get_party(bch[guy].rep);
      n = st->e[3] - 1;
      return gems_list[n];
   }
   return 0;
}

void player_control(UBYTE guy)
{
   UBYTE n;
   struct DUDESTAT *st;

   player_req = -1;
   player_turn = guy + 1;
   player_tmode = 1;

   binputmode = 0;
   bmenu = 0;
   bmenusquish = 0;
   bmenucount = 0;
   bgemslide = 32;

   st = get_party(bch[guy].rep);
   n = st->e[3];
   if(n) {
      bbox[4] = gems_list[n-1] + 39;
      bicon_refresh(4);
   }
   //bloadboxes();

   bcur_slot = 0;
   bcur_destx = bch[guy].rx;
   bcur_desty = bch[guy].ry;
   bcur_dist = 127;
   f_bcur_on();
}

void player_cancel()
{
   player_turn = 0;

   binputmode = 0;
   bmenu = 0;
   bmenusquish = 0;
   bmenucount = 0;

   wipe_menu();
   wipe_text();
   wipe_gem();
}

void bresolve(UBYTE guy)
{
   UBYTE n;
   struct BDUDE *b;

   b = &bch[guy];

   if(b->damage) {
      if(b->spflag)
         b->st.sp -= b->damage;
      else
         b->st.hp -= b->damage;
      b->spflag = 0;

      if(bch[guy].st.hp <= 0) {
         if(bflag_sp_guy) {
            if(guy == bflag_sp_guy-(4-bchnum)-1) {
               bch[guy].damage = 0;
               bch[guy].st.hp = 1;
               bexit = 1;
               return;
            }
         }

         bch[guy].st.hp = 0;
         bch[guy].ail = 0;

         if(bch[guy].control == BPLAYER) {
            turn_del(guy);

            if(player_turn - 1 == guy) {
               player_cancel();
            }

            n = get_gem(guy);
            if(n) {
               --n;
               if(n == GEM_CRYSTAL) {
                  f_gem_animate(guy);
                  bch[guy].damage = -12;
                  f_bchain_insert(guy, BACT_CURE, 0, 0, 0, 0);
                  bch[guy].points = 0;
                  bch[guy].recover = getcount(bch[guy].st.spd, 0) / 2;
                  return;
               }
            }
            /*else {
               // cancel turn if necessary
               if(player_turn - 1 == guy) {
                  player_cancel();
               }
            }*/
         }
         else {
            if(!bch[guy].poof) {
               bch[guy].poof = 1;
               f_bchain_insert(guy, BACT_POOF, 0, 0, 0, 0);

               // tree boss
               if(bch[guy].spawnlord) {
                  for(n = bchnum; n < bchnum+3; ++n) {
                     if(bch[n].active) {
                        bch[n].poof = 1;
                        f_bchain_insert(n, BACT_POOF, 0, 0, 0, 0);
                     }
                  }
               }
            }
         }
      }
      else if(b->st.hp > b->st.hpmax) {
         b->st.hp = b->st.hpmax;
      }

      if(b->st.sp < 0) {
         b->st.sp = 0;
      }
      else if(b->st.sp > b->st.spmax) {
         b->st.sp = b->st.spmax;
      }

      if(bch[guy].control == BPLAYER)
         refresh_stats(guy);

      bch[guy].damage = 0;
   }
}


UBYTE bhi, blo;
WORD bw, bw2, bw3;
UBYTE skill, evade;
UBYTE bdead;
BYTE bwait;
UBYTE bfirstloop;

extern text_active; // grabbing this so we can initialize it

//static struct DUDESTAT u, u2;

UWORD bgetrecover(struct BDUDE *b)
{
      // 2) Implement speed eq: Pick a "shortest possible turn" time
                //    (I think it should be pretty fast, but pick something that works).
      //    Set that to be 50 countdown clicks (or you can always scale it up
      //    or down if you need to). Then use the eq: 50+FastestCharSpeed-MySpeed
      //    (char including enemies here) to determine the # of clicks each
      //    character has to wait...hence the fastest char will be waiting 50
      //    clicks, each other char will be waiting more than 50 clicks.

      // new: Delay = 50*(10+FastestGuy'sSpeed/10+MySpeed)

      //b->recover = getcount(b->st.spd, 0) / 2;
      //b->recover = 50 + bfastestspd - b->st.spd;
      bw = 10 + bfastestspd;
      bw2 = 10 + b->st.spd;
      return bw * 50 / bw2;
}

UBYTE real_battle()
{
   // define variables
   WORD x, y;
   UBYTE c, d;
   UBYTE reqnum, bprev;
   BYTE even;
   UBYTE dir;
   struct BDUDE *b;
   UBYTE rescue = 0;

   // we are not dead yet
   bdead = 0;

   setwinpal();

   // get the battle formation info
   load_eform(bformation, bdata);
   bdata_map = bdata[0];
   bdata_enum = bdata[1];
   memcpy(&bdata_exp, bdata + 2, (UWORD)2);
   memcpy(&bdata_gold, bdata + 4, (UWORD)2);
   bdata_gold += randword() % (bdata_gold / 10 + 1);

   // load the map
   bloadmap(bdata_map);
   f_setaluthapal(bdata_map);

   // special case -- night
   if(vars[VAR_EVEPROG] == 4) {
      f_colorshift(COLOR_BLUEDARK);
   }
   f_bmapsetup();

   // turn on all the services
   flicker_off();
   start_gradient_battle();
   bspr_init();
   n = (UBYTE)randfunc() & 3;
   switch(n) {
      case 0: px = -32, py = -32; break;
      case 1: px = 272, py = -32; break;
      case 2: px = -32, py = 272; break;
      case 3: px = 272, py = 272; break;
   }
   bspr_scrollto(px, py);
   bspr_scrollset(px, py);

   // reset font
   reset_str(0);

   // load battle services
   load_wingding();
   f_bcur_init();
   openbattlewindow();

   // get items
   for(n = 0; n < 12; ++n)
      bitem_tab[n] = f_items_have(n+1);

   // load party members
   bnumdudes = 16;
   for(n = 0; n != bnumdudes; ++n)
      bch[n].active = 0;

   for(n = 0, k = 0; n < 4; ++n) {
      st = get_party(n);
      if(!st)
         continue;
      // draw bottom menu
      memcpy(bname, st->name, (UWORD)4);
      bname[4] = 0;
      pstr(1 + (k * 3),1, 4, (char *)bname);

      // add
      n2 = (k << 1) + 6;
      f_bpadd(n, bdata[n2], bdata[n2 + 1]);
      //bch[k].points = 1;
      ++k;
   }
   bchnum = k;

   for(n = 0; n < bchnum; ++n) {
      btextptr[n] = read_str();
      pstr(1 + (n * 3), 2, 2, deci_16(bch[n].st.hp, 3));
   }
   btextdesc = read_str();

   // load enemies
   bennum = 0;
   for(n = 0; n < bdata_enum; ++n) {
      n2 = (n * 3) + 14;
      f_beadd(bdata[n2], bdata[n2 + 1], bdata[n2 + 2]);
   }

   // initializations
   bwon_item = 0;
   bwon_gp = 1;
   bfastestguy = 0;
   bfastestspd = 0;
   for(n = 0; n != bnumdudes; ++n) {
      b = &bch[n];

      if(!b->active)
         continue;

      if(!bwon_item && b->control == BCPU) {
         c = b->st.item;
         if(c) {
            d = c < 20 ? 8: 32;
            if(((UBYTE)randfunc() % d) == 1) {
               bwon_item = c;
            }
         }
      }
      if(b->control == BCPU) {
         bw = (WORD)b->st.level + 2 - (WORD)stats[CONNOR].level;
         if(bw > 0) {
           bw += bwon_gp; // word total
           if(bw > 99)
              bw = 99;
           bwon_gp = (UBYTE)bw; // save back to byte
         }
      }

      bgetreal(b);
      if(b->st.spd > bfastestspd) {
         bfastestspd = b->st.spd;
         bfastestguy = n;
      }
   }

   // set recovery
   for(n = 0; n != bnumdudes; ++n) {
      if(!bch[n].active)
         continue;

      bch[n].recover = bgetrecover(&bch[n]);
   }

   bfirstloop = 1;
   text_active = 0;
   bcoloractive = 0;
   brun = 0;
   bexit = 0;
   bwon = 0;
   bmenu = 0;
   reqcount = 0;
   lastreq = -1;

   bearly = 0;
   bcritical = 0;
   brun_counter = 0;

   icon_wiggle = 0;
   icon_count = 0;
   flop = 0;
   for(n = 0; n != 8; ++n)
      keylock[n] = 0;

   // turn init
   turn_init();

   // menu init
   bicon_reset();

   // start the fade in
        //winpal(1, 1, 18, 1, 7);
   bin_fade = 32;
   bin_popup = 0;
   pfs_targetspr |= 1;
   fadein(bin_fade);

   // scroll to first player
   bdsx = bch[0].rx;
   bdsy = bch[0].ry;

   f_bchain_stop();

   bloops = 0;
   vblank_timer = 0;
   rotpal = 0;
   bwait = 0;
   badvance = 0;

   auto_proc(auto_popup);

   while(!bwon && !bdead) {
      rotpal &= 3;

      if(bin_fade) {
         fade_step();
         if(!--bin_fade)
            bin_popup = 32;
      }

      // keep random numbers as random as possible
      randfunc();

      // handle chain
      odone = done;
      done = !f_bchain_do();
      bchain_finished = done != odone;

      if(bchain_finished) {
         // charmed?
         for(n = 0; n < 4; ++n) {
            if(bch[n].active && (bch[n].ail == BAIL_CHARM || bch[n].ail == BAIL_WOLF)) {
               if(player_turn - 1 == n)
                  player_cancel();
               turn_del(n);
            }
         }

         // remove dead enemies
         for(n = 0; n != bnumdudes; ++n) {
            if(!bch[n].active)
               continue;

            //if(bch[n].action == BACT_CLOSE) {
            if(bch[n].poof) {
               turn_del(n); // turn coming up?  i think not
               bdel(n);
            }
         }

         // check for loss
         bdead = 1;
         for(n = 0; n != bchnum; ++n) {
            if(bch[n].st.hp || bch[n].count)
               bdead = 0;
//            if(bch[n].count)
//               bdead = 0;
         }

         // check for victory
         for(n = 0; n != bnumdudes; ++n) {
            if(bch[n].active && bch[n].control == BCPU)
               break;
         }
         if(n == bnumdudes)
            bwon = 1;

         b_attrib = BATR_NONE;

         // ran away?
         if(brun || bexit) {
            break;
         }
      }

      //*** [graphics] ***
      // frame skip
      draw_gfx = 1;
      do {
         n = vblank_timer / 2;
      } while(bloops > n);
      //if(n > bloops)
      // draw_gfx = 0;
      if(n >= bloops && bloops == 32) {
         vblank_timer = 0;
         bloops = 0;
      }
      n = draw_gfx ? 0: bnumdudes;

      for(; n != bnumdudes; ++n) {
         b = &bch[n];

         if(!b->active)
            continue;

         k = 0;
         c = b->action;
         if(!bfirstloop) {
            if(c == BACT_NORM && (icon_count>>1) != n && !bchain_finished)
               continue;
         }

         d = f_get_4waydir(b->dir);

         // standing still?  wiggle around (or lay or sit)
         if(b->control == BPLAYER) {
            if(b->st.hp == 0) {
               k = 1; // shift 1 pixel up, see bspr_16set() call below
               b->frame = 47;

               // next two lines added 07/06/2004
               b->ail = 0;
               f_bshowail(n, 0);
            }
            else if(c == BACT_NORM) {
               if(b->st.hp <= (b->st.hpmax >> 2))
                  b->frame = 40 + flop;
               else
                  b->frame = (d << 1) + flop;
            }
         }
         else {
            if(!b->boss) {
               if(c == BACT_NORM) {
                  b->frame = (d << 1) + flop;
               }
            }
            else {
               // boss mode
               if(c == BACT_NORM) {
                  b->frame = flop;
                  if(!(b->flags & BOSS_COMPACT))
                     b->frame += (d << 1);
               }

               //if(b->frame > 3)
               // while(1);

               if(c == BACT_POOF || b->poof == 2) {// || b->poof) { //c == BACT_CLOSE) {
                  for(n2 = 0; n2 != 4; ++n2)
                     bspr_16set(n+n2, 0, 0, 0, -1, -1);
               }
               else {
                  // skip 7 heroes
                  curframe = 448;
                  n2 = b->type - SPRBASE_ENEMY - BOSS; // get BOSS number
                  curframe += (UWORD)BOSS * 9;             // skip past regular enemies
                  curframe += (UWORD)n2 * 36;              // index to the boss
                  curframe += b->frame << 2;           // find the frame.  whew!
                  bspr_16set(n,   bch[n].rx - 8, bch[n].ry, bch[n].rz + 16, curframe,   -1);
                  bspr_16set(n+1, bch[n].rx - 8, bch[n].ry, bch[n].rz,      curframe+1, -1);
                  bspr_16set(n+2, bch[n].rx + 8, bch[n].ry, bch[n].rz + 16, curframe+2, -1);
                  bspr_16set(n+3, bch[n].rx + 8, bch[n].ry, bch[n].rz,      curframe+3, -1);
               }
               continue;
            }
         }

         if(b->st.hp != 0) {
            n2 = b->ail;
            if(n2--) {
               f_bshowail(n, flop);
               if(c == BACT_NORM) {
                  if(n2 == 0)
                     b->frame = brandframe;
               }
            }
         }

         // add sprite base
         curframe = b->frame;
         if(b->control == BPLAYER) {
            curframe += (UWORD)b->type << 6;
         }
         else {
            // skip 7 heroes
            curframe += 448;
            curframe += ((UWORD)b->type-SPRBASE_ENEMY) * 9;
         }

         // display dude
         if(c == BACT_POOF || b->poof == 2) // || bch[n].poof) //c == BACT_CLOSE)
            bspr_16set(n, 0, 0, 0, -1, -1);
         else
            bspr_16set(n, b->rx, b->ry - k, b->rz, curframe, -1);
      }

      // bail now, after everyone has been rendered
      if(bdead)
         break;

      // update cursor
      if(binputmode == 2) {
         bcur_slot = 0;
         bcur_destx = bch[bchoosetarget].rx;
         bcur_desty = bch[bchoosetarget].ry;
         f_bcur_lock();
      }
      else {
         n = turn;
         if(player_turn) {
            bcur_slot = 0;
            bcur_destx = bch[player_turn-1].rx;
            bcur_desty = bch[player_turn-1].ry;
            f_bcur_dest();
         }
         else if(inturn) {
            bcur_slot = 0;
            bcur_destx = bch[n].rx;
            bcur_desty = bch[n].ry;
            f_bcur_dest();
         }
         else {
            bcur_slot = 0;
            f_bcur_off();
         }
      }
      f_bcur_update();


      // point camera
      if(binputmode == 2) {
         bdsx = bch[bchoosetarget].rx;
         bdsy = bch[bchoosetarget].ry;
      }
      else {
         if(inturn || player_turn) {
            n = turn;
            if(player_turn) {
               if(player_tmode)
                  n = player_turn - 1;
            }
            bdsx = bch[n].rx;
            bdsy = bch[n].ry;
         }
      }
      if(bfocus != -1) {
         bdsx = bch[bfocus].rx;
         bdsy = bch[bfocus].ry;
      }

      bspr_scrollto(bdsx, bdsy);

      // sync with LCD
      //w4vbl_done();


      //*** [turns/input] ***


      // active battle
      bblink[0] = 0;
      bblink[1] = 0;
      bblink[2] = 0;
      bblink[3] = 0;

      if(bin_fade)
         goto skip_meter;

      if(!options[0]) {
         if(player_turn)
            goto skip_meter;
      }
      else {
         if(bwait > 0) {
            --bwait;
            goto skip_meter;
         }
         else {
            bwait = (3 - options[0]);

            // quickfix to make life easy, extends negative wait by 1
            if(bwait < 0)
                   --bwait;
         }
      }

      bfreeze = 0;
      do {
         for(n = 0; n != bnumdudes; ++n) {
            if(!bch[n].active)
               continue;

            if(bch[n].st.hp == 0 || (n == turn && inturn) || bch[n].action == BACT_CLOSE)
               continue;

            // Someone using a Carnelian gem?
            if(bfreeze && bch[n].control == BCPU)
               continue;

            // advance clock, etc, as long as no one is currently moving
            if(!(moved && inturn)) {
               if(bch[n].gem) {
                  st = get_party(bch[n].rep);
                  n2 = gems_list[st->e[3]-1]-1;
                  if(n2 == GEM_CARNELIAN)
                     bfreeze = 1;

                  n2 = (UBYTE)item_info2(n2 + GEMS_INDEX)[5];
                  n2 &= 63;
                  if(bch[n].gemcount < n2)
                     ++bch[n].gemcount;
                  else {
                     bch[n].gemcount = 0;
                     if(gems_charge[st->e[3]-1] != 0)
                        --gems_charge[st->e[3]-1];
                     else {
                        bch[n].gem = 0;
                        if(n == turn)
                           wipe_gem();
                     }
                  }
               }

               if(bch[n].ail) {
                  if(!bch[n].ailtime) {
                     // restore victor back to human form
                     if(bch[n].ail == BAIL_WOLF)
                        bch[n].type = VICTOR;
                     bch[n].ail = 0;
                     f_bshowail(n, 0);
                  }
                  else
                     --bch[n].ailtime;
               }

               if(bch[n].points < 1) {
                  if(bch[n].recover) {
                     --bch[n].recover;
                  }
                  else {
                     ++bch[n].points;
                     n2 = get_gem(n);
                     if(n2) {
                        --n2;
                        if(n2 == GEM_TURQUIOSE)
                           ++bch[n].points;
                     }
                     if(bch[n].control == BCPU)
                        turn_add(n, 0);
                  }
               }
            }

            if(bch[n].points >= 1) {
               // if player, give control but don't add to turn queue
               if(bch[n].control == BPLAYER) {
                  bblink[n] = 1;

                  // make sure you're not in the queue
                  if(!binqueue(n)) {
                     if(bch[n].ail == BAIL_CHARM) {
                        turn_add(n, f_b_ai_charm(n));
                     }
                     else if(bch[n].ail == BAIL_WOLF) {
                        turn_add(n, f_b_ai(n, 0));
                     }
                     else {
                        if(!player_turn && !bturndelay)
                           player_control(n);
                     }
                  }
                  else
                     bblink[n] = 0;
               }
            }
         }

         // negative means to go faster
         if(bwait < 0) {
            ++bwait;

            if(bwait == 0) {
               vblank_timer = 0;
               bloops = 0;
               break;
            }
         }
      } while(!player_turn && !inturn && !turn_at);
skip_meter:

      // set the blink
      for(n = 0; n != 4; ++n) {
         winpal(1+(n*3),1,3,1, bblink[n]?rotpal:7);
      }

      if(!inturn) {
         i = turn_next();
         if(i != -1) {
            turn = i;

            --bch[turn].points;

            inturn = 1;
            moved = 0;

            if(!player_turn) {
               bcur_slot = 0;
               bcur_destx = bch[turn].rx;
               bcur_desty = bch[turn].ry;
               f_bcur_lock();
            }
         }
      }

      // log user input
      input();
      for(n = 0; n != 8; ++n) {
         if(!key[n])
            keylock[n] = 0;
         if(keylock[n])
            key[n] = 0;
      }
                /*if(demo_key) {
                        if(key[SELECT]) {
                                bwon = 99;
                                break;
                        }
                }*/
      ////debug: SELECT button to end battle in victory
      //if(key[SELECT]) {
      //   bwon = 1;
      //   for(n = 0; n != bnumdudes; ++n)
      //      if(bch[n].control == BCPU)
      //         bdel(n);
      //   break;
      //}


      if(player_turn) {
         if(player_req == -1) {
            player_req = f_get_player_input(player_turn-1);
            if(player_req != -1) {
               turn_add(player_turn-1, player_req);
               player_turn = 0;
               bturndelay = 4;
            }
         }
      }
      else {
         // delete icon
         bspr_8set(4, 0, 0, -1, 0);
         bspr_8set(5, 0, 0, -1, 0);
      }

      // get move
      if(inturn && !moved) {
         // get request
         req = -1;
         if(bch[turn].control == BCPU) {
            while(req == -1) {
               // b_ai may return -1 if we should try a different move
               req = f_b_ai(turn, bch[turn].st.ai[(UBYTE)randfunc()%16]);
            }
            /*for(k = 0; k != 6; ++k) {
               get_dxdy(bch[turn].x, bch[turn].y, k);
               for(k2 = 0; k2 != bnumdudes; ++k2) {
                  if(!bch[k2].active)
                     continue;
                  if(turn == k2)
                     continue;
                  if(bch[k2].x == dx && bch[k2].y == dy && bch[k2].control == BPLAYER) {
                     if(bch[k2].st.hp > 0) {
                        bch[turn].target = k2;
                        break;
                     }
                  }
               }
               if(k2 < bnumdudes)
                  break;
            }
            if(k < 6) {
               req = k + 6;
            }
            else {
               req = ((UBYTE)randfunc()&127) % 6;
               get_dxdy(bch[turn].x, bch[turn].y, req);
            }*/
         }
         if(bch[turn].control == BPLAYER) {
            req = turn_next_req; //player_req;
         }


         // process request
         if(req != -1) {
            // positional type? (attack/move)  then set some stuff
            if(req < 12) {
               f_get_dxdy(bch[turn].x, bch[turn].y, req % 6);
               even = ~dx & 1;
               px = (WORD)dx * 24;
               py = (WORD)dy * 24;
               py += (WORD)even * 12;
               px += 8;
               py += 8;
            }

            // movement
            if(req < 6) {
               // verify that the move is allowed
               if(!bmap[dy][dx] && dy >= 0 && dy < 9 && dx >= 0 && dx < 11) {
                  k = 0;
                  n = bhitd(turn, req);
                  if(!n || (n && bch[n-1].st.hp == 0))
                     k = 1;

                  if(k) {
                     bch[turn].dir = req;
                     f_bchain_reset();

                     if(n) {
                        --n;

                        f_bchain_add(turn, BACT_JUMP, bch[turn].dir, px, py, 1);

                        // guy on ground swap with the guy moving
                        bch[n].x = bch[turn].x;
                        bch[n].y = bch[turn].y;
                        f_bchain_add(n, BACT_WALK, bch[n].dir, bch[turn].rx, bch[turn].ry, 0);
                     }
                     else
                        f_bchain_add(turn, BACT_WALK, bch[turn].dir, px, py, 0);

                     bch[turn].x = dx;
                     bch[turn].y = dy;

                     done = !f_bchain_do();

                     moved = 1;
                  }
               }
               if(!moved) {
                  if(bch[turn].control == BPLAYER) {
                     bch[turn].points = 1;
                     inturn = 0;
                  }
               }
            }
            // attack
            else if(req < 12) {
               dir = req - 6;
               f_get_dxdy(bch[turn].x, bch[turn].y, dir);
               b_target = bch[turn].target;

               // verify that the move is allowed
               if(bch[b_target].x == dx && bch[b_target].y == dy && bch[b_target].active && bch[b_target].st.hp > 0) {
                  d = f_get_4waydir(dir);

                  if(d == RLEFT)
                     px += 12;
                  if(d == RRIGHT)
                     px -= 12;
                  if(d == RDOWN)
                     py -= 12;
                  if(d == RUP)
                     py += 12;

                  bch[turn].dir = dir;
                  bch[turn].olddir = bch[turn].dir;

                  // damage
                  b_damage = f_bwepdamage(turn, b_target);
                                                if(bch[turn].ail == BAIL_WOLF)
                                                        b_damage *= 2;

                                                n = randfunc() % 101;
                                                if(n < 5) {
                                                        bcritical = 1;
                                                        b_damage *= 2;
                                                        f_bdamagecap();
                                                }

                  if(bch[turn].control == BCPU)
                     n = 5;
                  else
                     n = 10;

                  bw = (UWORD)n * (UWORD)bch[b_target].st.agl / (UWORD)bch[turn].st.agl;
                  bw2 = randfunc() % 101;
                  if(bw2 < bw) {
                     b_damage = 0;
                     bcritical = 0;  // disable critical hit if you miss
                  }

                  b_attrib = BATR_HAND;
                  f_bchain_reset();

                  if(bch[turn].control == BPLAYER) {
                     st = get_party(bch[turn].rep);
                     if(st->e[0] >= 40 && st->e[0] <= 43) {
                        f_bchain_add(turn, BACT_PULLBOW, dir, 0, 0, 0);
                        f_bchain_add(b_target, BACT_ARROW, dir, bch[turn].rx, bch[turn].ry, 0);
                        //f_bchain_add(turn, BACT_ARROW, dir, bch[b_target].rx, bch[b_target].ry, 0);
                     }
                     else {
                        f_bchain_add(turn, BACT_JUMP, dir, px, py, 1);
                        if(st->e[0])
                           f_bchain_add(turn, BACT_ATTACK, dir, bch[b_target].rx, bch[b_target].ry, 0);
                        else
                           f_bchain_add(b_target, BACT_SCRATCH, dir, 0, 0, 0);

                        f_bchain_add(turn, BACT_JUMP, dir, bch[turn].rx, bch[turn].ry, 1);
                     }
                  }
                  else {
                     n = bch[turn].slider ? BACT_WALK: BACT_JUMP;
                     f_bchain_add(turn, n, dir, px, py, 1);
                     f_bchain_add(b_target, BACT_SCRATCH, dir, 0, 0, 0);
                     f_bchain_add(turn, n, dir, bch[turn].rx, bch[turn].ry, 1);
                  }

                  bch[b_target].damage += b_damage;

                  done = !f_bchain_do();
                  moved = 1;
               }
               else {
                  // invalid move?  let the player try again
                  if(bch[turn].control == BPLAYER) {
                     bch[turn].points = 1;
                     inturn = 0;
                  }
               }
            }
            // skill (30 skills, 16 gems, 12 items (66-77)
            else if(req >= 16 && req <= 77) {
               // make sure it's an ok target
               b_target = bch[turn].target;
               if(bch[b_target].active && !(bch[b_target].st.hp <= 0 && bch[b_target].control != bch[turn].control)) {
                  n = req - 16;
                  //n = bch[turn].skl[n] - 1;
                  f_load_skill(n);  // load skill

                  b_attrib = BATR_NONE;
                  bskillselect = n;
                  f_start_skill();

                  moved = 1;
               }
               else {
                  // invalid move?  let the player try again
                  if(bch[turn].control == BPLAYER) {
                     bch[turn].points = 1;
                     inturn = 0;
                  }
               }
            }
            // run (80 == maybe, 81 == for sure)
            else if(req == 80 || req == 81) {
               n = 0;

               if(req == 81)
                  n = 1;
               else if(f_brun_check())
                  n = 1;

               if(!bflag_run)
                  n = 0;

               // success!
               if(n) {
                  dir = BDIR_DOWNRIGHT;
                  px = bch[turn].rx + 160;
                  py = bch[turn].ry;
                  bch[turn].dir = dir;
                  brun = 1;

                  f_bchain_reset();
                  f_bchain_add(turn, BACT_JUMP, dir, px, py, 1);

                  done = !f_bchain_do();
                  moved = 1;
               }
               // fail!
               else {
                  dir = bch[turn].dir;
                  px = bch[turn].rx;
                  py = bch[turn].ry;

                  f_bchain_reset();
                  f_bchain_add(turn, BACT_JUMP, dir, px, py, 1);

                  //pstr(14, 1, 5, "Can't run!");

                  done = !f_bchain_do();
                  moved = 1;
               }
            }
            // skip turn
            else if(req == 99) {
               inturn = 0;
            }

            if(moved) {
               if(req < 6)
                  n = 0;
               else if(req < 12)
                  n = 2;
               else
                  n = 3;

               bch[turn].recover = bgetrecover(&bch[turn]);
            }

            /* enhance effect based on gems */
            if(moved && b_attrib != BATR_NONE) {
               for(n = 0; n != bnumdudes && moved; ++n) {
                  bch[n].gemshow = 0;

                  if(!bch[n].active)
                     continue;
                  if(bch[n].damage == 0)
                     continue;

                  // attacker
                  c = get_gem(turn);
                  if(c) {
                     n2 = 1;
                     --c;
                     if((c == GEM_RUBY && b_attrib == BATR_FIRE) ||
                        (c == GEM_AQUAMARINE && b_attrib == BATR_WATER) ||
                        (c == GEM_AMETHYST && b_attrib == BATR_WIND)) {
                        bch[n].damage *= 2;
                     }
                     else if(c == GEM_ONYX && b_attrib == BATR_HAND) {
                        bch[n].damage *= 2;
                     }
                     else if(c == GEM_PEARL && b_attrib == BATR_HEAL) {
                        bch[n].damage *= 2;
                     }
                     else
                        n2 = 0;

                     if(n2)
                        bch[turn].gemshow = 1;
                  }

                  // defender
                  c = get_gem(n);
                  if(c) {
                     n2 = 1;
                     --c;
                     if(c == GEM_SAPPHIRE && b_attrib != BATR_NONE && b_attrib != BATR_HAND) {
                        bch[n].damage = bch[n].damage > 0 ? 1: -1;
                     }
                     else if(c == GEM_JADE && bch[n].damage > 0) {
                        bch[n].damage /= 4;
                        if(bch[n].damage == 0)
                           bch[n].damage = 1;
                     }
                     else
                        n2 = 0;

                     if(n2)
                        bch[n].gemshow = 1;
                  }

                  // cap the damage
                  if(bch[n].damage > 999)
                     bch[n].damage = 999;
               }
            }
         }
      }

      // move character
      if(inturn && done && moved) {
         inturn = 0;
      }

      ++icon_count;
      if(icon_count == 16) {
         icon_wiggle ^= 1;
         flop ^= 1;
         brandframe = randfunc() % 40;
         icon_count = 0;
      }
      if(bturndelay)
         --bturndelay;

      f_text_update();
      ++bloops;
      bfirstloop = 0;
      ++rotpal;
   }
   //grad_op_disable();

   // play victory song when battle is won (but not when running or dead party)
   if( bwon )
      play_song(SNG_VICTORY);
      
   bcur_slot = 0;
   f_bcur_off();
   f_bcur_update();

   for(n = 0; n != 32; ++n)
      bspr_8set(n, 0, 0, -1, 0);

   do_delay(2);

   wipe_gem();
   wipe_text();
   wipe_menu();

   winpal(1, 1, 18, 1, 7);

   // move window offscreen
   for(n = 4; n <= 32; n += 4) {
      w4vbl_done();
      setwindow(0, 112+n);
   }

   // copy battle hp/sp back to main stats
   for(n = 0; n != bchnum; ++n) {
      st = get_party(bch[n].rep);
      st->hp    = bch[n].st.hp;
      st->hpmax = bch[n].st.hpmax;
      st->sp    = bch[n].st.sp;
      st->spmax = bch[n].st.spmax;
   }

   // copy items back
   for(n = 0; n < 12; ++n)
      f_items_set(n+1, bitem_tab[n]);

   /*if(bwon == BATTLE_DEMOSKIP) {
      auto_proc(0);
      closebattlewindow();
      bspr_off();
      do_delay(2);
      return BATTLE_DEMOSKIP;
   }*/

   if(bdead || brun || bexit)
      goto bend;

   w4vbl_done();
   openbattlewindow();
   reset_str(0);

   f_bvictory();

bend:
   if(!bflag_lose && bdead) {
      bdead = 0;
      rescue = 1;
      for(n = 0; n != 4; ++n) {
         st = get_party(bch[n].rep);
         if(!st)
            continue;
         st->hp = 1;
      }
   }

   auto_proc(0);
   closebattlewindow();

   do_delay(2);

   if(bdead)
      n2 = 180;
   else
      n2 = 32;

   fadeout(n2);
   for(n = 0; n != n2; ++n) {
      w4vbl_done();
      fade_step();
   }

   bspr_off();
   do_delay(2);

   if(bdead) {
      return BATTLE_LOSE;
   }
   else { 
      if(rescue)
         return BATTLE_RESCUE;
      else
         return BATTLE_WIN;
   }
}
