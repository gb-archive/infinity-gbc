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
#include"vars.h"

extern UBYTE demo_reset;

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

#define ACTION_DEQUIP   0x6c

#define ACTION_LASTMOVS 0x6e

#define ACTION_RESET     0x2a
#define ACTION_ENDINIT   0x2b
#define ACTION_ENDNEXT   0x2c
#define ACTION_FADEFINAL 0x2d
#define ACTION_LOCK      0x2e
#define ACTION_POEM      0x2f
#define ACTION_BOSSPAL   0x38


// vars
extern BYTE *local_event;
extern BYTE *local_action;
extern UBYTE pass_c;
extern BYTE numactions;
extern BYTE numcases;
extern UWORD actionp, actionptmp[];
extern BYTE lastmoves[];
extern struct DUDESTRUCT ch[];

void poem(UBYTE opt);

static UBYTE getreg(UBYTE op1)
{
   if(op1 == 255)
      return 255;

   if(op1 >= REGBASE)
      op1 = regs[op1-REGBASE];

   return op1;
}

static UBYTE enext()
{
   return (getreg(get_event_byte(actionp++)));
}

static UBYTE enextraw()
{
   return (get_event_byte(actionp++));
}

BYTE get_action2()
{
   UBYTE c;
   UBYTE n, n2;
   UBYTE op1, op2;
   struct DUDESTAT *st;

   c = pass_c;

   switch(c) {
      case ACTION_CSTEP:
      case ACTION_CQSTEP:
         local_event[0] = c == ACTION_CSTEP ? CASE_STEP : CASE_QSTEP;   // step
         local_event[1] = enextraw();     // event-lo
         local_event[2] = enextraw();     // event-hi
         local_event[3] = enext();     // x
         local_event[4] = enext();     // y
         ++numcases;
         break;

      case ACTION_CTALK:
         local_event[0] = CASE_TALK;      // talk
         local_event[1] = enextraw();     // event-lo
         local_event[2] = enextraw();     // event-hi
         local_event[3] = enext();     // slot
         ++numcases;
         break;

      case ACTION_CSEARCH:
         local_event[0] = CASE_SEARCH; // search
         local_event[1] = enextraw();     // event-lo
         local_event[2] = enextraw();     // event-hi
         local_event[3] = enext();     // x
         local_event[4] = enext();     // y
         local_event[5] = enext();     // dir flags
         ++numcases;
         break;

      case ACTION_CHSTEP:
      case ACTION_CVSTEP:
         local_event[0] = c == ACTION_CHSTEP ? CASE_HSTEP: CASE_VSTEP; // step
         local_event[1] = enextraw();     // event-lo
         local_event[2] = enextraw();     // event-hi
         local_event[3] = enext();     // x
         local_event[4] = enext();     // y
         local_event[5] = enext();     // len
         ++numcases;
         break;

      // item related
      case ACTION_GIVE2:
         f_items_add(enext());
         break;

      case ACTION_TAKE:
         n = enext();
         f_items_del(n);
         break;

      case ACTION_HAVE:
         op1 = enextraw();
         n = enext();

         if(op1 < REGBASE)
            while(1);

         op1 = op1 - REGBASE;
         n2 = f_items_have(n);

         regs[op1] = n2;
         break;

      case ACTION_EQUIP:
         n = enext();   // guy
         n2 = enext();  // item to equip

         st = get_realparty(n);
         op1 = item_info2(n2)[0];
         if(op1 >= 2 && op1 <= 4) {
            op1 -= 2;
            op2 = st->e[op1];
            f_items_del(n2);
            f_items_add(op2);
            st->e[op1] = n2;
            f_stats_recalc(st);
         }
         break;

      case ACTION_DEQUIP:
         n = enext();   // guy
         n2 = enext();  // item to equip

         st = get_realparty(n);
         if(st) {
            op1 = st->e[n2];
            st->e[n2] = 0;
            if(op1)
               f_items_add(op1);
         }
         break;

      case ACTION_LASTMOVS:
         op1 = enext();
         op2 = enext();
         n = enext();

         lastmoves[0] = op1;
         lastmoves[1] = op1;
         lastmoves[2] = op2;
         lastmoves[3] = op2;
         lastmoves[4] = n;
         lastmoves[5] = n;
         lastmoves[6] = -1;
         lastmoves[7] = -1;
         break;

      case ACTION_GETXY:
         op1 = enextraw();
         op1 = op1 - REGBASE;
         op2 = enextraw();
         op2 = op2 - REGBASE;
         regs[op1] = (UBYTE)ch[0].x >> 1;
         regs[op2] = ((UBYTE)ch[0].y+1) >> 1;
         break;

      case ACTION_RESET:
         fadeto(COLOR_WHITE, 60);
         for(n = 0; n < 60; ++n) {
            w4vbl_done();
            fade_step();
         }
         fadeto(COLOR_BLACK, 60);
         for(n = 0; n < 60; ++n) {
            w4vbl_done();
            fade_step();
         }
         demo_reset = 1;
         break;

      case ACTION_ENDINIT:
         end_init();
         break;

      case ACTION_ENDNEXT:
         end_next();
         f_resync();
         break;

      case ACTION_FADEFINAL:
         do_delay(60);
         owe_off();

         op1 = pfs_targetbgr;
         pfs_targetbgr = 255;
         n2 = 32;
         fadeout(n2);
         for(n = 0; n != n2; ++n) {
            w4vbl_done();
            fade_step();
         }
         pfs_targetbgr = op1;
         blackout();
         clear_vram();
         f_resync();
         end_deinit();
         return 0;
         break;

      case ACTION_POEM:
         op1 = enext();
         poem(op1);
         if(op1 == 1) {
            load_gemgfx();
            load_menugfx();
            setwinpal();
         }
         break;

      case ACTION_BOSSPAL:
         f_setbosspal(enext());
         break;

      case ACTION_LOCK:
      default:
         // unsupported/unknown event!  CRASH!
         while(1);
   }

   return 1;
}

extern UBYTE pal_bak[];
extern UBYTE pal_cur[];
extern UWORD pal[];

#include"poem.c"

UWORD aluthapal[] = {
   0x520F,0x5AD6,0x2108,0x0000,
   0x319F,0x2536,0x14AF,0x0844,
   0x3DEF,0x0550,0x04CD,0x0000,
   0x001C,0x03FE,0x09B3,0x0066,
   0x2108,0x0281,0x0562,0x0000,
   0x1CE7,0x2536,0x14AF,0x0844,
   0x001C,0x2D6B,0x0000,0x5AD6,
   0x17BC,0x22E7,0x19C4,0x14E0,
};

UWORD aluthapalgba[] = {
   0x737A,0x7BBD,0x42B0,0x0D02,
   0xE71F,0x56DD,0xA61A,0x9589,
   0x6F7A,0x12FA,0x1258,0x0D02,
   0x0D1F,0x0FFF,0x973C,0x0DAC,
   0x42B0,0x0FA5,0x1307,0x0D02,
   0x366F,0x56DD,0x261A,0x1589,
   0x0D1F,0x6315,0x0D02,0x7BBD,
   0x27FF,0x43CF,0x2B49,0x2662,
};

UBYTE aluthamaps[] = {
   225, 226, // battle alutha maps
   107, // #define MAP_ALU_BASE   107
   108, // #define MAP_ALU_THRON  108
   109, // #define MAP_ALU_HALL   109
   110, // #define MAP_ALU_TOW1   110
   111, // #define MAP_ALU_TOW2   111
   112, // #define MAP_ALU_TOW3   112
   113, // #define MAP_ALU_TOW4   113
   127, // #define MAP_ALU_CRYPT   127
   165, // #define MAP_DUR_P5     165
   65,  // #define MAP_DUR_P1     65
   66,  // #define MAP_DUR_P2     66
   67,  // #define MAP_DUR_P3     67
   0,
};

void setaluthapal(UBYTE map)
{
   UBYTE n;

   for(n = 0; aluthamaps[n]; ++n) {
      if(map == aluthamaps[n])
         break;
   }
   if(aluthamaps[n])
      memcpy(pal, _gba ? aluthapalgba : aluthapal, (UWORD)64);
}



static char *copystr = "(C)1999-2016 Affinix Software";

static UWORD copy_pal2[] = {
   0x0000,0x1084,0x0000,0x7FFF,
   0x0000,0x1DC9,0x0D02,0x7FFF,
};

void copyright()
{
   UBYTE i,j,at;
   extern UWORD pal[];
   extern UWORD window_ptr;

   LCDC_REG=0xFD;
   SCX_REG=3;
   SCY_REG=1;

   window_ptr = 0x9C00;
   point_str( 1, 17 );
   for( i = 0; i < 17; i++ )
   {
      put_tile( i, 17, i );
   }
   ipstr( 0, 17, copystr );

   for(j=0;j<8;j++)
      memcpy(pal+j*4,copy_pal2+((_gba)?4:0),8);

   pfs_targetbgr=0xFF;

#define blackpal ((UWORD *)0)
#define whitepal ((UWORD *)1)

   pfs_setup(whitepal, pal, 32);
   for(n = 0; n < 32; ++n) {
      w4vbl_done3();
      pfs_step();
   }
   do_delay(60);
   pfs_setup(pal, blackpal, 32);
   for(n = 0; n < 32; ++n) {
      w4vbl_done3();
      pfs_step();
   }

   LCDC_REG = 0xe7;
   pfs_targetbgr=0xFE;
   pfs_targetspr=0xFC;

   do_delay(30);
}
