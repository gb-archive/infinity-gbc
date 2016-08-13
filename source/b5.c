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

#include"defs.h"
#include"vars.h"

#define guy nextguy

extern UBYTE tu, tu2;
extern WORD tpx, tpy;
extern WORD tqueue[16][2];
extern BYTE tz, tv, tw;

// table stuff
BYTE *slurptab = (BYTE *)0xDC00;
BYTE (*panel_rel)[32][2] = (BYTE (*)[32][2])0xDC00;
UBYTE (*jolttab)[3] = (UBYTE (*)[3])0xDC00;

BYTE whirltab[4][3][3] = {
        {
                { -14,  0,  0 },
                { -14, 16,  1 },
                {   2, 16,  2 },
        },
        {
                {  -2, 16,  6 },
                {  14,  0,  7 },
                {  14, 16,  8 },
        },
        {
                {  -2,-16,  3 },
                {  14,-16,  4 },
                {  14,  0,  5 },
        },
        {
                { -14,-16,  9 },
                { -14,  0, 10 },
                {   2,-16, 11 },
        },
};

UBYTE whirltab2[4] = { 18, 26, 30, 22 };

extern UBYTE nextguy;
extern UBYTE curaction;

void baction_nextmore()
{
	UBYTE u, u2;
	UBYTE a1, a2;
	UBYTE d;
	BYTE s;
	WORD px2;

	switch(curaction) {

		case BACT_ARROW:

                        f_bpro_next(0);
                        px = bpro[0].rx - 3;
                        py = bpro[0].ry - 3;
                        switch(tu) {
                                case 3: n = 0; break;
                                case 0: n = 2; break;
                                case 1: n = 3; break;
                                case 2: n = 1; break;
                        }
                        bspr_8set(22, px,     py, 32 + (n << 2), 0);
                        bspr_8set(23, px + 8, py, 33 + (n << 2), 0);
                        bfocus = guy;

                        u = bch[guy].count;
                        if(u == 1) {
                                play_sfx(SFX_ARROWHIT);

                                px = bch[guy].rx;
                                py = bch[guy].ry;
                                f_gemcheck(guy);
                                f_text_init(px, py, bch[guy].damage, BDIR_UP);
                                f_bresolve(guy);
                        }
		break;

		case BACT_JOLT:

                        if(bch[guy].count == 0) {
                                bch[guy].count = 1;
                        }
                        if(bch[guy].progress != 13) {
                                u = 12 - bch[guy].progress;
                                if(u == 0)
                                        play_sfx(SFX_LIGHTNING);
                                for(u2 = 0; u2 != 3; ++u2) {
                                        a1 = u2 << 1;
                                        if(!jolttab[u][u2]) {
                                                bspr_8set(a1 + 22, 0, 0, -1, 0);
                                                bspr_8set(a1 + 23, 0, 0, -1, 0);
                                                continue;
                                        }
                                        a2 = jolttab[u][u2]-1;
                                        a2 <<= 1;

                                        px = bch[guy].rx - 3;
                                        py = bch[guy].ry - 3;
                                        py -= 32 - (u2 << 4);
                                        bspr_8set(a1 + 22, px,     py, a2 + 32, 3);
                                        bspr_8set(a1 + 23, px + 8, py, a2 + 33, 3);
                                }
                                if(bch[guy].progress == 1 && bch[guy].count == 1) {
                                        px = bch[guy].rx;
                                        py = bch[guy].ry;
                                        f_gemcheck(guy);
                                        f_text_init(px, py, bch[guy].damage, BDIR_UP);
                                        f_bresolve(guy);
                                }
                        }
                        bfocus = guy;
		break;

		case BACT_TIDAL:

                        u = 24 - bch[guy].count;
                        if(u == 0)
                                play_sfx(SFX_TIDAL);

                        u2 = (u / 3) << 2;
                        px = bch[guy].rx + 32 - (u << 1);
                        py = bch[guy].ry - 11;
                        bspr_8set(22, px,     py,      u2 + 32, 1);
                        bspr_8set(23, px + 8, py,      u2 + 33, 1);
                        bspr_8set(24, px,     py + 16, u2 + 34, 1);
                        bspr_8set(25, px + 8, py + 16, u2 + 35, 1);
                        bfocus = guy;

                        if(bch[guy].count == 1) {
                                px = bch[guy].rx;
                                py = bch[guy].ry;
                                f_gemcheck(guy);
                                f_text_init(px, py, bch[guy].damage, BDIR_UP);
                                f_bresolve(guy);
                        }
		break;

		case BACT_WAKE:

                        u = 64 - bch[guy].count;
                        if(u == 0)
                                play_sfx(SFX_REVIVE);
                        if(u < 22)
                                u2 = u & 1;
                        else {
                                u2 = ((u-22) % 14) + 2;
                        }
                        u2 <<= 1;

                        px = bch[guy].rx - 3;
                        py = bch[guy].ry - 3;

                        bspr_8set(22, px,     py, u2 + 32, 5);
                        bspr_8set(23, px + 8, py, u2 + 33, 5);
                        bfocus = guy;

		break;

		case BACT_PANEL:

                        u = 32 - bch[guy].count;
                        if(u == 0) {
                                if(tu2) {
                                        if(bch[guy].control == BCPU)
                                                play_sfx(SFX_ENEMYCAST);
                                        else
                                                play_sfx(SFX_SACRIFICE);
                                }
                        }

                        for(u2 = 0; u2 != 6; ++u2) {
                                px = bch[guy].rx + 1 + panel_rel[u2][u][0];
                                py = bch[guy].ry - 7 + panel_rel[u2][u][1];
                                if(u2 == 0)
                                        a1 = u&3;
                                if(u2 == 1)
                                        a1 = (u&3) + 4;
                                if(u2 == 2)
                                        a1 = (u&3) + 8;
                                if(u2 == 3)
                                        a1 = 3-(u&3);
                                if(u2 == 4)
                                        a1 = 7-(u&3);
                                if(u2 == 5)
                                        a1 = 11-(u&3);

                                if(u2 == 3)
                                        py -= 6;
                                bspr_8set(u2+22, px, py, 32+a1, 5);
                        }
                        bch[guy].frame = 0;
                        bfocus = guy;
		break;

		case BACT_SLURP:

                        /*if(bch[guy].count == 0) {
                                u = bch[guy].progress;
                                if(u == 1) {
                                        bch[guy].count = 40;
                                        f_text_init(px, py, bch[guy].damage, BDIR_UP);
                                        f_bresolve(guy);
                                }
                        }*/

                        if(!(tu & 128)) {
                        //if(bch[guy].progress == 2) {
                                u = 52 - bch[guy].count;
                                if(u == 0)
                                        play_sfx(SFX_SOULSLURP);

                                if(u < 8) {
                                        u2 = u & 254;
                                }
                                else {
                                        u2 = ((u&3) + 4) << 1;
                                }
                                if(u < 28)
                                        a1 = 0;
                                else
                                        a1 = u - 28;

                                if(u == 51) {
                                        //bch[guy].count = 40;
                                        f_text_init(px, py, bch[guy].damage, BDIR_UP);
                                        f_bresolve(guy);
                                }

                                px2 = px = bch[guy].rx - 3;
                                py = bch[guy].ry - 3;
                                bfocus = guy;
                        }
                        else {
                                u = 40 - bch[guy].count;
                                if(u < 8) {
                                        u2 = (u + 16) & 254;
                                        bfocus = guy;
                                }
                                else {
                                        u2 = (u&2) + 24;
                                        bfocus = tu & 15;
                                }

                                a1 = u + 24;

                                f_bpro_next(0);
                                px2 = px = bpro[0].rx - 3;
                                py = bpro[0].ry - 3;
                        }
                        px -= slurptab[a1];

                        bspr_8set(22, px,   py, u2+32, 4);
                        bspr_8set(23, px+8, py, u2+33, 4);

                        px2 += slurptab[a1];

                        bspr_8set(24, px2,   py, u2+32, 4);
                        bspr_8set(25, px2+8, py, u2+33, 4);

		break;

		case BACT_THROW:

                        f_bpro_next(0);
                        px = bpro[0].rx - 3;
                        py = bpro[0].ry - 3;
                        if(bch[guy].count&1)
                                tz += tv++;
                        bspr_8set(22, px,     py + tz, 32, tu);
                        bspr_8set(23, px + 8, py + tz, 33, tu);
                        if(bch[guy].count > 13)
                                bfocus = turn;
                        else
                                bfocus = guy;

		break;

		case BACT_SPAWN:

                        if(bch[guy].count == 0)
                                bch[guy].count = 4;

                        u = 5 - bch[guy].progress;

                        if(u == 0)
                                play_sfx(SFX_SPAWN);

                        px = bch[guy].rx - 3;
                        py = bch[guy].ry - 3;

                        u2 = u << 1;
                        bspr_8set(22, px,   py, 32 + u2, tu);
                        bspr_8set(23, px+8, py, 33 + u2, tu);

                        if(u == 4) {
                                // revive him
                                bch[guy].st.hp = bch[guy].st.hpmax;
                                bch[guy].active = 1;
                                bch[guy].poof = 0;
                                bch[guy].frame = 0;
                                bch[guy].recover = getcount(bch[guy].st.spd, 3) / 2;
                                bch[guy].points = 0;
                                bch[guy].damage = 0;
                        }

                        bfocus = guy;
		break;

		case BACT_CHARM:

                        u = 67 - bch[guy].count;

                        if(u == 0)
                                play_sfx(SFX_CHARM);
                        if(u < 48)
                                f_bpro_next(0);
                        if(u == 66) {
                                bch[guy].ail = BAIL_CHARM;
                                bch[guy].ailtime = f_bgetrecover(&bch[guy]) * 4;
                        }

                        px = bpro[0].rx - 3;
                        py = bpro[0].ry - 3 + tw;
                        tw += tz;
                        tz += tv;
                        if(tz == 3 || tz == -3)
                                tv = -tv;

                        for(u2 = 0; u2 != 18; ++u2) {
                                tqueue[u2][0] = tqueue[u2+1][0];
                                tqueue[u2][1] = tqueue[u2+1][1];
                        }
                        tqueue[18][0] = px;
                        tqueue[18][1] = py;

                        bspr_8set(22, px,     py, 44, 0);
                        bspr_8set(23, px + 8, py, 45, 0);

                        px = tqueue[9][0];
                        py = tqueue[9][1];
                        bspr_8set(24, px,     py, 44, 0);
                        bspr_8set(25, px + 8, py, 45, 0);

                        px = tqueue[0][0];
                        py = tqueue[0][1];
                        bspr_8set(26, px,     py, 44, 0);
                        bspr_8set(27, px + 8, py, 45, 0);

                        bfocus = guy;
		break;


                case BACT_EFFECT:

                        bch[guy].ail = tu;
                        bch[guy].ailtime = f_bgetrecover(&bch[guy]) * 4;

                        if(tu == BAIL_WOLF)
                                bch[guy].type = VICTOR_WOLF;
                break;


                case BACT_SLASH:

                        u = 56 - bch[guy].count;
                        u2 = u & 1;
                        u2 <<= 2;

                        f_bpro_next(0);

                        px = bpro[0].rx - 3 - 8;
                        py = bpro[0].ry - 3;

                        bspr_8set(22, px,     py, u2 + 32, 4);
                        bspr_8set(23, px + 8, py, u2 + 33, 4);
                        bspr_8set(24, px +16, py, u2 + 34, 4);
                        bspr_8set(25, px +24, py, u2 + 35, 4);

                        bfocus = guy;
                break;


                case BACT_WHIRL:

                        u = 32 - bch[guy].count;
                        u2 = (u >> 1) & 0x03;

                        px = bch[guy].rx - 3;
                        py = bch[guy].ry - 4;
                        if(u2 == 2)
                                py += 2;
                        bch[guy].frame = whirltab2[u2];

                        for(a1 = 0; a1 < 3; ++a1) {
                                tpx = px + whirltab[u2][a1][0];
                                tpy = py + whirltab[u2][a1][1];
                                a2 = (whirltab[u2][a1][2] << 1) + 32;
                                bspr_8set(22 + (a1 << 1), tpx,     tpy, a2,     4);
                                bspr_8set(23 + (a1 << 1), tpx + 8, tpy, a2 + 1, 4);
                        }

                        bfocus = guy;

                break;


                case BACT_DAMAGE:

                        u = 32 - bch[guy].count;

                        // deal damage on first frame
                        if(u == 0) {
                                px = bch[guy].rx;
                                py = bch[guy].ry;
                                f_gemcheck(guy);
                                f_text_init(px, py, bch[guy].damage, BDIR_UP);
                                f_bresolve(guy);
                        }

                        bfocus = guy;

                break;
	}
}
