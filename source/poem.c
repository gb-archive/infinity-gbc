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

/* "Time moves forward, taking us like leaves in the wind.
    We meet, we come together, and then fall to the earth.
    Longing for the spring...
    Longing for life...
    But all things end."

   "The seasons pass eternally,
    And we vanish like the stars,
    Yet spring will always come again,
    Bringing with its promise a new hope."
*/

static UWORD poem_pal[] = {
   0x0000,0x1084,0x0000,0x7FFF,
   0x0000,0x1DC9,0x0D02,0x7FFF,
};

char poemstr[2][7][35] = {
   { "Time moves forward,", "Taking us like leaves in the wind.", "We meet, we come together,", "And then fall to the earth.", "Longing for the spring...", "Longing for life...", "But all things end." },
   { "", "The seasons pass eternally,", "And vanish like the stars,", "", "Yet spring always comes again,", "Bringing rebirth and new hope.", ""},
};

char poemwait[2][7] = {
   { 10, 10, 10, 80, 10, 10, 80 },
   {  0, 10, 80,  0, 10, 80,  0 },
};

void poem_str(UBYTE tile, char *s)
{
   UBYTE n;

   n = 0;
   while(*s) {
      n = dfs_pschar(tile, n, *s);
      ++s;
   }
}

void poem(UBYTE opt)
{
   UBYTE i,j,at;

	owe_off();

   pfs_targetspr=0;

   VBK_REG=1;
   clear_vram();
   VBK_REG=0;
   clear_vram();

   pfs_targetbgr=0xFF;
   blackout();

   LCDC_REG=0xFD;
   SCX_REG=4;
   SCY_REG=0;

   VBK_REG=1;

   for(j=0;j<4;j++)
      for(i=0;i<20;i++)
      {
         put_tile(i+1,j*2+2,j+1);
         put_tile(i+1,j*2+3,j+1);
      }
   for(j=4;j<7;j++)
      for(i=0;i<20;i++)
      {
         put_tile(i+1,j*2+3,(j+1)|0x08);
         put_tile(i+1,j*2+4,(j+1)|0x08);
      }

   VBK_REG=0;

   for(j=0;j<4;j++)
      for(i=0;i<20;i++)
      {
         put_tile(i+1,j*2+2,j*40+(i<<1));
         put_tile(i+1,j*2+3,j*40+(i<<1)+1);
      }
   for(j=4;j<7;j++)
      for(i=0;i<20;i++)
      {
         put_tile(i+1,j*2+3,(j&3)*40+(i<<1));
         put_tile(i+1,j*2+4,(j&3)*40+(i<<1)+1);
      }

   for(at=0;at<(opt?1:2);at++)
   {
      for(j=0;j<8;j++)
         memcpy(pal+j*4,poem_pal+((_gba)?4:0),8);

      VBK_REG=1;
      dfs_clear(0,160);
      VBK_REG=0;
      dfs_clear(0,160);

      for(j=0;j<4;j++)
         poem_str(j*40,poemstr[at][j]);

      VBK_REG=1;

      for(j=4;j<7;j++)
         poem_str((j&3)*40,poemstr[at][j]);

      VBK_REG=0;

      for(j=0;j<7;j++)
         if(poemwait[at][j])
         {
            pfs_targetbgr=64>>j;

            fadein(128);
            for(i=0;i<128;i++)
            {
               fade_step();
               w4vbl_done3();
            }

            for(i=0;i<poemwait[at][j];i++)
               do_delay(4);
         }

      pfs_targetbgr=0xFF;

      fadeout(32);
      for(i=0;i<32;i++)
      {
         fade_step();
         w4vbl_done3();
      }
	}

   LCDC_REG = 0xe7;

   pfs_targetbgr=0xFE;
   pfs_targetspr=0xFC;

   clear_vram();
   set_w_attr();
}
