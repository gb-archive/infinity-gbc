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

#include <SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <string.h>
#include <malloc.h>
#include <direct.h>
#include "glib.h"
#include "mouse.c"
#include "kb.h"
//#include "vgovbe20.h"

#define pal_r     2
#define pal_g     1
#define pal_b     0

#define dir_u     0
#define dir_d     1
#define dir_l     2
#define dir_r     3

#define palette   0x07
#define misc      0x08
#define block     0x10
#define hflip     0x20
#define vflip     0x40
#define priority  0x80

#define scrollable 0x04

#define mouse_in(x,y,x1,y1,x2,y2) ((x)>=(x1)&&(x)<(x2)&&(y)>=(y1)&&(y)<(y2))

#define swap(a,b) (a)^=(b)^=(a)^=(b)

extern byte hflipgfx[],vflipgfx[],overlay[],overlayset[],disk[];
extern byte darrow[],uarrow[],larrow[],rarrow[],palettegfx[];
extern byte small_uarrow[], small_darrow[], small_larrow[], small_rarrow[];
extern byte smallb_uarrow[],smallb_darrow[],smallb_larrow[],smallb_rarrow[];
extern byte blockedgfx[],blockedset[],scrollgfx[];

graphlib glib;

byte id[3];
byte idchk_tile[3]={'J','2','T'};
byte idchk_map[3]={'J','2','M'};

byte palettes[64];
byte gbapalettes[64];
byte palettetable[256];
byte tilebuf[4096];
byte clip[17];
byte clip2[8];

byte *currentpal;
byte gbamode;

byte tile16[1024];
byte attr[1024];
byte tmparray[1024];

byte enemymap[99];

byte map[16384];
byte en[16384];
byte tile_expand[1048576];

byte tmpbuf[16384];
byte tmpen[16384];

char tmp[128],*tmp2,*tmp3;

byte an_tiles[8],an_index[8],an_counter[8];
byte updatecounter;

int atmdmode;

//VBESURFACE* m640x480;

void conv_tile(int tile)
{
   int i,j,k,l,m,p,q,b,clr;
   int data_a,data_b;
   int attr;

   byte *expandptr;

   expandptr=tile_expand+tile*4096;

   for(attr=0;attr<8;attr++)
   {
      for(m=0;m<2;m++)
      {
         for(l=0;l<2;l++)
         {
            for(k=0;k<2;k++)
            {
               for(j=0;j<8;j++)
               {
                  q=l?7-j:j;
            
                  data_a=tilebuf[tile*16+j * 2];
                  data_b=tilebuf[tile*16+j*2+1];
            
                  for (i=0,b=0x80;i<8;i++,b>>=1)
                  {
                     p=k?7-i:i;
            
                     clr=((b&data_a)?1:0)
                        +((b&data_b)?2:0)
                        +(attr&7)*4+128;
   
                     if (m)
                     {
                        if (clr&3)
                           expandptr[q*8+p]=clr;
//tile_expand[tile*4096+attr*512+m*256+l*128+k*64+q*8+p]=clr;
                        else
                           expandptr[q*8+p]=0;
                     }
                     else
                        expandptr[q*8+p]=clr;
                  }
               }
               expandptr+=64;
            }
         }
      }
   }
}

void draw_tile2(int x,int y,int tile,int attr)
{
   glib.putsprchr(x,y,&tile_expand[tile*4096+(((attr&0xE0)>>5)|((attr&0x07)<<3))*64]);
}

void draw_tile(int x,int y,int tile,int attr)
{
   if(tile<8)
      if(an_tiles[tile])
         tile=an_index[tile]+an_counter[tile];

   glib.putsprchr(x,y,&tile_expand[tile*4096+(((attr&0xE0)>>5)|((attr&0x07)<<3))*64]);

/*
   int i,j,p,q,b,clr;
   int data_a,data_b;

   for (j=0;j<8;j++)
   {
      q=(attr&vflip)?7-j:j;

      data_a=tilebuf[tile*16+j * 2];
      data_b=tilebuf[tile*16+j*2+1];

      for (i=0,b=0x80;i<8;i++,b>>=1)
      {
         p=(attr&hflip)?7-i:i;

         clr=((b&data_a)?1:0)
            +((b&data_b)?2:0)
            +(attr&7)*4+128;
//            +palettetable[tile]*4+128;

         if (attr&priority)
         {
            if (clr&3)
               glib.putpixel(x+p,y+q,clr);
         }
         else
            glib.putpixel(x+p,y+q,clr);
      }
   }
*/
}

void inline draw_stile(int x,int y,int tile,int attr)
{
   if(tile<8)
      if(an_tiles[tile])
         tile=an_index[tile]+an_counter[tile];

   glib.putpixel(x,y,tile_expand[tile*4096+(((attr&0xE0)>>5)|((attr&0x07)<<3))*64+8*4+4]);
}

void draw_stile16(int x,int y,int tile)
{
   draw_stile(x++,y  ,tile16[tile * 4],attr[tile * 4]);
   draw_stile(x  ,y++,tile16[tile*4+1],attr[tile*4+1]);
   draw_stile(x--,y  ,tile16[tile*4+3],attr[tile*4+3]);
   draw_stile(x  ,y  ,tile16[tile*4+2],attr[tile*4+2]);
}

void draw_bits(byte *ptr,int x,int y,int clr)
{
   int i,j,b;

   for(j=0;j<8;j++)
      for(i=0,b=0x80;i<8;i++,b>>=1)
         if(b&ptr[j])
            glib.putpixel(x+i,y+j,clr);
}

void draw_big_tile(int x,int y,int tile,int attr)
{
   int i, j, p, q, b, clr;
   int data_a, data_b;

   if(tile<8)
      if(an_tiles[tile])
         tile=an_index[tile]+an_counter[tile];

   for (j = 0; j < 8; j++)
   {
      data_a = tilebuf[tile * 16 + j * 2    ];
      data_b = tilebuf[tile * 16 + j * 2 + 1];

      q = (attr & vflip) ? 7 - j : j;

      for (b = 128, i = 0; i < 8; i++, b >>= 1)
      {
         clr=((b&data_a)?1:0)
            +((b&data_b)?2:0)
            +(attr&7)*4+128;
            //+ palettetable[tile] * 4 + 128;

         p=(attr&hflip)?7-i:i;

         if(attr&priority)
         {
            if(clr&3)
               glib.drawbar(x+p*4,y+q*4,x+p*4+3,y+q*4+3,clr);
         }
         else
            glib.drawbar(x+p*4,y+q*4,x+p*4+3,y+q*4+3,clr);
      }
   }
}

byte g_font[] = {
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,	// 32 ' '
  0x20,0x20,0x20,0x20,0x20,0x00,0x20,0x00,
  0x50,0x50,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x50,0xF8,0x50,0xF8,0x50,0x00,0x00,
  0x20,0x20,0x80,0x70,0x08,0x20,0x20,0x00,
  0x00,0xC8,0x10,0x20,0x40,0x98,0x00,0x00,
  0x60,0x90,0xA0,0x48,0xA8,0x90,0x68,0x00,
  0x20,0x20,0x00,0x00,0x00,0x00,0x00,0x00,
  0x10,0x20,0x40,0x40,0x40,0x20,0x10,0x00,
  0x40,0x20,0x10,0x10,0x10,0x20,0x40,0x00,
  0x00,0x20,0xF8,0x70,0xF8,0x20,0x00,0x00,
  0x00,0x00,0x20,0xF8,0x20,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x20,0x40,0x00,
  0x00,0x00,0x00,0xF8,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x20,0x00,
  0x00,0x08,0x10,0x20,0x40,0x80,0x00,0x00,
  0x70,0x88,0x98,0xA8,0xC8,0x88,0x70,0x00,	// 48 '0'
  0x20,0x60,0x20,0x20,0x20,0x20,0x70,0x00,
  0x70,0x88,0x08,0x10,0x20,0x40,0xF8,0x00,
  0x70,0x88,0x08,0x30,0x08,0x88,0x70,0x00,
  0x10,0x30,0x50,0x90,0x90,0xF8,0x10,0x00,
  0xF8,0x80,0xF0,0x08,0x08,0x88,0x70,0x00,
  0x30,0x40,0x80,0xF0,0x88,0x88,0x70,0x00,
  0xF8,0x88,0x08,0x10,0x20,0x20,0x20,0x00,
  0x70,0x88,0x88,0x70,0x88,0x88,0x70,0x00,
  0x70,0x88,0x88,0x78,0x08,0x10,0x60,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x20,0x40,0x00,
  0x08,0x10,0x40,0x80,0x40,0x10,0x08,0x00,
  0x00,0x00,0xF8,0x00,0xF8,0x00,0x00,0x00,
  0x80,0x40,0x10,0x08,0x10,0x40,0x80,0x00,
  0x70,0x88,0x10,0x20,0x20,0x00,0x20,0x00,
  0x70,0x88,0xA8,0xA8,0xA8,0x80,0x70,0x00,	// 64 '@'
  0x20,0x50,0x88,0x88,0xF8,0x88,0x88,0x00,
  0xF0,0x88,0x88,0xF0,0x88,0x88,0xF0,0x00,
  0x70,0x88,0x80,0x80,0x80,0x88,0x70,0x00,
  0xF0,0x88,0x88,0x88,0x88,0x88,0xF0,0x00,
  0xF8,0x80,0x80,0xF0,0x80,0x80,0xF8,0x00,
  0xF8,0x80,0x80,0xF0,0x80,0x80,0x80,0x00,
  0x70,0x88,0x80,0xB8,0x88,0x98,0x68,0x00,
  0x88,0x88,0x88,0xF8,0x88,0x88,0x88,0x00,
  0x70,0x20,0x20,0x20,0x20,0x20,0x70,0x00,
  0x38,0x10,0x10,0x10,0x10,0x90,0x60,0x00,
  0x88,0x98,0xA0,0xC0,0xA0,0x98,0x88,0x00,
  0x80,0x80,0x80,0x80,0x80,0x80,0xF8,0x00,
  0x88,0x88,0xD8,0xD8,0xA8,0xA8,0x88,0x00,
  0x88,0xC8,0xA8,0xA8,0x98,0x88,0x88,0x00,
  0x70,0x88,0x88,0x88,0x88,0x88,0x70,0x00,
  0xF0,0x88,0x88,0xF0,0x80,0x80,0x80,0x00,	// 80 'P'
  0x70,0x88,0x88,0x88,0xA8,0x90,0x68,0x00,
  0xF0,0x88,0x88,0xF0,0x90,0x88,0x88,0x00,
  0x70,0x88,0x80,0x70,0x08,0x88,0x70,0x00,
  0xF8,0x20,0x20,0x20,0x20,0x20,0x20,0x00,
  0x88,0x88,0x88,0x88,0x88,0x88,0x70,0x00,
  0x88,0x88,0x50,0x50,0x50,0x20,0x20,0x00,
  0x88,0x88,0xA8,0xA8,0xA8,0x50,0x50,0x00,
  0x88,0x88,0x50,0x20,0x50,0x88,0x88,0x00,
  0x88,0x88,0x50,0x20,0x20,0x20,0x20,0x00,
  0xF8,0x08,0x10,0x20,0x40,0x80,0xF8,0x00,
  0x70,0x40,0x40,0x40,0x40,0x40,0x70,0x00,
  0x00,0x80,0x40,0x20,0x10,0x08,0x00,0x00,
  0x70,0x10,0x10,0x10,0x10,0x10,0x70,0x00,
  0x20,0x50,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0xF8,0x00,
  0x20,0x20,0x00,0x00,0x00,0x00,0x00,0x00,	// 96 '`'
  0x00,0x00,0x70,0x08,0x78,0x88,0x78,0x00,
  0x80,0x80,0xF0,0x88,0x88,0x88,0xF0,0x00,
  0x00,0x00,0x70,0x88,0x80,0x88,0x70,0x00,
  0x08,0x08,0x78,0x88,0x88,0x88,0x78,0x00,
  0x00,0x00,0x70,0x88,0xF8,0x80,0x70,0x00,
  0x30,0x40,0x70,0x40,0x40,0x40,0x40,0x00,
  0x00,0x00,0x78,0x88,0x78,0x08,0xF0,0x00,
  0x80,0x80,0xF0,0x88,0x88,0x88,0x88,0x00,
  0x20,0x00,0x20,0x20,0x20,0x20,0x20,0x00,
  0x10,0x10,0x10,0x10,0x10,0x10,0x60,0x00,
  0x80,0x80,0x88,0x90,0xA0,0x90,0x88,0x00,
  0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x00,
  0x00,0x00,0xD0,0xA8,0xA8,0xA8,0x88,0x00,
  0x00,0x00,0xF0,0x88,0x88,0x88,0x88,0x00,
  0x00,0x00,0x70,0x88,0x88,0x88,0x70,0x00,
  0x00,0x00,0xF0,0x88,0xF0,0x80,0x80,0x00,	// 112 'p'
  0x00,0x00,0x78,0x88,0x78,0x08,0x08,0x00,
  0x00,0x00,0xB0,0xC8,0x80,0x80,0x80,0x00,
  0x00,0x00,0x78,0x80,0x70,0x08,0xF0,0x00,
  0x00,0x40,0x70,0x40,0x40,0x40,0x30,0x00,
  0x00,0x00,0x88,0x88,0x88,0x88,0x78,0x00,
  0x00,0x00,0x88,0x88,0x50,0x50,0x20,0x00,
  0x00,0x00,0x88,0x88,0xA8,0x50,0x50,0x00,
  0x00,0x00,0x88,0x50,0x20,0x50,0x88,0x00,
  0x00,0x00,0x88,0x88,0x78,0x08,0xF0,0x00,
  0x00,0x00,0xF8,0x10,0x20,0x40,0xF8,0x00,
  0x10,0x20,0x20,0x40,0x20,0x20,0x10,0x00,
  0x20,0x20,0x20,0x00,0x20,0x20,0x20,0x00,
  0x40,0x20,0x20,0x10,0x20,0x20,0x40,0x00,
  0x68,0xB0,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x20,0x50,0x50,0x88,0xF8,0x00,0x00,
};

void draw_string(int x,int y,char *string,int clr)
{
   for (;*string;string++,x+=8)
//------------------------------------------------------
      draw_bits((byte *)(g_font+(*(byte*)string-' ')*8),x,y,clr);
//------------------------------------------------------
//======================================================
//    draw_bits((byte *)(0xFFA6E+*string*8),x,y,clr);
//======================================================
}

void draw_tile16(int x,int y,int tile,int attrmask)
{
   if (priority&attr[tile * 4]&attrmask||!(priority&attrmask))
      draw_tile(x  ,y  ,tile16[tile * 4],attr[tile * 4]&attrmask);
   if (priority&attr[tile*4+1]&attrmask||!(priority&attrmask))
      draw_tile(x+8,y  ,tile16[tile*4+1],attr[tile*4+1]&attrmask);
   if (priority&attr[tile*4+2]&attrmask||!(priority&attrmask))
      draw_tile(x  ,y+8,tile16[tile*4+2],attr[tile*4+2]&attrmask);
   if (priority&attr[tile*4+3]&attrmask||!(priority&attrmask))
      draw_tile(x+8,y+8,tile16[tile*4+3],attr[tile*4+3]&attrmask);
}

void draw_appr16(int x,int y,int tile)
{
   glib.drawbar(x,y,x+15,y+15,tile);

   sprintf(tmp,"%02X",tile);
   draw_string(x,y,tmp,tile^0x55);
}

void draw_sappr16(int x,int y,int tile)
{
   glib.drawbar(x,y,x+1,y+1,tile);
}

void draw_big_tile16(int x,int y,int tile,int attrmask)
{
   if (priority & attr[tile * 4] & attrmask || !(priority & attrmask))
      draw_big_tile(x + 1, y + 1, tile16[tile * 4], attr[tile * 4] & attrmask);
   if (priority & attr[tile*4+1] & attrmask || !(priority & attrmask))
      draw_big_tile(x + 34, y + 1, tile16[tile * 4 + 1], attr[tile * 4 + 1] & attrmask);
   if (priority & attr[tile*4+2] & attrmask || !(priority & attrmask))
      draw_big_tile(x + 1, y + 34, tile16[tile * 4 + 2], attr[tile * 4 + 2] & attrmask);
   if (priority & attr[tile*4+3] & attrmask || !(priority & attrmask))
      draw_big_tile(x + 34, y + 34, tile16[tile * 4 + 3], attr[tile * 4 + 3] & attrmask);
}

int get_comp(byte *pal,int clr,int comp)
{
   if (comp==pal_r)
      return   pal[clr * 2]&0x1F;
   if (comp==pal_g)
      return ((pal[clr*2+1]&0x03)<<3)
           | ((pal[clr * 2]&0xE0)>>5);
   if (comp==pal_b)
      return  (pal[clr*2+1]&0x7C)>>2;

   return 0;
}

void set_comp(byte *pal,int clr,int comp,int val)
{
   if(comp==pal_r)
      pal[clr * 2]=pal[clr * 2]&0xE0|val;
   if(comp==pal_g)
   {
      pal[clr*2+1]=pal[clr*2+1]&0xFC|(val>>3);
      pal[clr * 2]=pal[clr * 2]&0x1F|(val<<5);
   }
   if(comp==pal_b)
      pal[clr*2+1]=pal[clr*2+1]&0x83|(val<<2);
}


int GetValue(int min,int max,int v)
{
//   return min+(float)(max-min)*(2.0*(v/31.0)-(v/31.0)*(v/31.0));

   return min+((max-min)*(v*(64-v))/256+1)/4;
}

/*int GetValue2(int min,int max,int v)
{
   return min+((max-min)*v*33+512)/1024;
}*/

int GetValue2(int min,int max,int v)
{
   return min+(max-min)*v*v/1024;
}


//void GenFilter()
//{
//    for (int r=0;r<32;r++)
//    {
//        for (int g=0;g<32;g++)
//        {
//            for (int b=0;b<32;b++)
//            {
//                int nr=GetValue(GetValue(4,14,g),GetValue(24,29,g),r)-4;
//                int ng=GetValue(GetValue(4+GetValue(0,5,r),14+GetValue(0,3,r),b),
//                    GetValue(24+GetValue(0,3,r),29+GetValue(0,1,r),b),g)-4;
//                int nb=GetValue(GetValue(4+GetValue(0,5,r),14+GetValue(0,3,r),g),
//                    GetValue(24+GetValue(0,3,r),29+GetValue(0,1,r),g),b)-4;
//                Filter[(b<<10)|(g<<5)|r]=(nr<<10)|(ng<<5)|nb;
//            }
//        }
//    }
//}

void set_pal(int setclr, int clr)
{
   int r,g,b;
   int x,y,z;

   if(gbamode)
   {
      x=get_comp(gbapalettes,clr,pal_r)+1;
      y=get_comp(gbapalettes,clr,pal_g)+1;
      z=get_comp(gbapalettes,clr,pal_b)+1;

      r=GetValue(GetValue2(0,9,y),GetValue2(17,22,y),x);
      g=GetValue(GetValue2(GetValue2(0,4,x),GetValue2(9,12,x),z),
        GetValue2(GetValue2(18,21,x),GetValue2(22,23,x),z),y);
      b=GetValue(GetValue2(GetValue2(0,5,x),GetValue2(10,13,x),y),
        GetValue2(GetValue2(20,23,x),GetValue2(25,26,x),y),z);

/*    r=GetValue(GetValue(0, 8,(y+z)/2),GetValue(22,27,(y+z)/2),x)*18/32;
      g=GetValue(GetValue(0, 6,(x+z)/2),GetValue(18,27,(x+z)/2),y)*18/32;
      b=GetValue(GetValue(0, 8,(x+y)/2),GetValue(20,27,(x+y)/2),z)*18/32; */

/*    r=GetValue(GetValue(0, 8,(y+z)/2),GetValue(22,27,(y+z)/2),x)*18/32;
      g=GetValue(GetValue(0, 8,(x+z)/2),GetValue(19,27,(x+z)/2),y)*18/32;
      b=GetValue(GetValue(0, 9,(x+y)/2),GetValue(20,27,(x+y)/2),z)*18/32; */

/*    r=GetValue2(GetValue2(0,f1,(y+z)/2),GetValue2(f4,27,(y+z)/2),x)*24/32;
      g=GetValue2(GetValue2(0,f2,(x+z)/2),GetValue2(f5,27,(x+z)/2),y)*24/32;
      b=GetValue2(GetValue2(0,f3,(x+y)/2),GetValue2(f6,27,(x+y)/2),z)*24/32; */

/*    r=GetValue(GetValue(0,10,z),GetValue(22,27,y),x)*24/32;
      g=GetValue(GetValue(0, 8,x),GetValue(18,27,z),y)*24/32;
      b=GetValue(GetValue(0, 9,y),GetValue(20,27,x),z)*24/32; */

/*    r=GetValue(GetValue(0, 4,z),GetValue(12,24,y),x);
      g=GetValue(GetValue(0, 2,x),GetValue(10,24,z),y);
      b=GetValue(GetValue(0, 3,y),GetValue(12,24,x),z); */

/*    r=GetValue(GetValue(4,14,z),GetValue(24,29,y),x)-4;
      g=GetValue(GetValue(4,14,x),GetValue(24,29,z),y)-4;
      b=GetValue(GetValue(4,14,y),GetValue(24,29,x),z)-4; */

/*    r=GetValue(GetValue(4,14,y),GetValue(24,29,z),x)-4;
      g=GetValue(GetValue(4,14,x),GetValue(24,29,z),y)-4;
      b=GetValue(GetValue(4,14,x),GetValue(24,29,y),z)-4; */

/*    r=GetValue(GetValue(4,14,y),GetValue(24,29,y),x)-4;
      g=GetValue(GetValue(4,14,x),GetValue(24,29,x),y)-4;
      b=GetValue(GetValue(4,14,x),GetValue(24,29,x),z)-4; */

/*    r=GetValue(GetValue(4,14,y),GetValue(24,29,y),x)-4;

      g=GetValue(GetValue(4,14,x),GetValue(24,29,x),y)-4;

      b=GetValue(GetValue(GetValue(4,9,x),GetValue(14,17,x),y),
        GetValue(GetValue(24,27,x),GetValue(29,30,x),y),z)-4;*/

/*    r=GetValue(GetValue(GetValue(4,9,y),GetValue(14,17,y),z),
        GetValue(GetValue(24,27,y),GetValue(29,30,y),z),x)-4;
      g=GetValue(GetValue(4,14,x),GetValue(24,29,x),y)-4;

      b=GetValue(GetValue(GetValue(4,9,x),GetValue(14,17,x),y),
        GetValue(GetValue(24,27,x),GetValue(29,30,x),y),z)-4; */
   }
   else
   {
      x=get_comp(palettes,clr,pal_r)+1;
      y=get_comp(palettes,clr,pal_g)+1;
      z=get_comp(palettes,clr,pal_b)+1;

      r=GetValue(GetValue(4,14,y),GetValue(24,29,y),x)-4;

      g=GetValue(GetValue(GetValue(4,9,x),GetValue(14,17,x),z),
        GetValue(GetValue(24,27,x),GetValue(29,30,x),z),y)-4;
      b=GetValue(GetValue(GetValue(4,9,x),GetValue(14,17,x),y),
        GetValue(GetValue(24,27,x),GetValue(29,30,x),y),z)-4;

/*
      set_comp(gbapalettes,clr,pal_r,r);
      set_comp(gbapalettes,clr,pal_g,g);
      set_comp(gbapalettes,clr,pal_b,b);
*/
   }

   glib.setrgb(setclr,r*0x020000+g*0x000200+b*0x000002);
}

/*
void pixelate(byte *s, int w, int h, int pass, int size)
{
   int n, n2, down, across, maxw, maxh;
   byte get, *bak;
        
   if (size == 0)
      return;

   maxw = w / size + 1;
   maxh = h / size + 1;
   bak = s;

   for(down = 0; down < maxh; ++down)
   {
      for(across = 0; across < maxw; ++across)
      {
         get = *s;
         for(n2 = 0; n2 < size; ++n2)
         {
            if (down * size + n2 > h)
               break;
            for (n = 0; n < size; ++n)
            {
               if (across * size + n > w)
                  break;
               else
                  *(s + n2 * (w + pass) + n) = get;
            }
         }
         s += size;
      }
      bak += (w + pass) * size;
      s = bak;
   }
}
*/

#include "files.h"

int import_tiles(char *file)
{
   int a,j,x,size;

   if(!(fp=fopen(file,"rb")))
      return 1;

   fseek(fp,0,SEEK_END);
   size=ftell(fp);
   a=size/17;
   rewind(fp);

   fread(tmparray,1,a,fp);

   num_tiles=1;
   for (j=0;j<256;j++)
   {
      for (x=0;x<16;x++)
      if (tilebuf[j*16+x])
         num_tiles=j+1;
   }

   if(a+num_tiles>=256)
      a=256-num_tiles;

   for(j=0;j<a;j++)
      palettetable[num_tiles+j]=tmparray[j];

   fread(tmparray,16,a,fp);

   fclose(fp);

   memcpy(tilebuf+num_tiles*16,tmparray,a*16);

   num_tiles=1;
   for (j=0;j<256;j++)
   {
      for (x=0;x<16;x++)
      if (tilebuf[j*16+x])
         num_tiles=j+1;
   }

   return 0;
}

struct COPY_BYTES_PARAM {
	int dstindex;
	int srcindex;
};

COPY_BYTES_PARAM keystate2keytable[] = {
	K_L,		SDLK_l,
	K_E,		SDLK_e,
	K_I,		SDLK_i,
	K_T,		SDLK_t,
	K_G,		SDLK_g,
	K_H,		SDLK_h,
	K_Y,		SDLK_y,
	K_J,		SDLK_j,
	K_F2,		SDLK_F2,
	K_F3,		SDLK_F3,
	K_F5,		SDLK_F5,
	K_F6,		SDLK_F6,
	K_F7,		SDLK_F7,
	K_F8,		SDLK_F8,
	K_F9,		SDLK_F9,
	K_F10,		SDLK_F10,
	K_P,		SDLK_p,
	K_ESC,		SDLK_ESCAPE,
	K_Q,		SDLK_q,
	K_A,		SDLK_a,
	K_UP,		SDLK_UP,
	K_DOWN,		SDLK_DOWN,
	K_LEFT,		SDLK_LEFT,
	K_RIGHT,	SDLK_RIGHT,
	K_O,		SDLK_o,
	K_B,		SDLK_b,
	K_Z,		SDLK_z,
	K_U,		SDLK_u,
	K_D,		SDLK_d,
	K_R,		SDLK_r,
	K_X,		SDLK_x,
	K_C,		SDLK_c,
	K_V,		SDLK_v,
	K_F,		SDLK_f,
	K_LSHIFT,	SDLK_LSHIFT,
	K_N,		SDLK_n,
	K_M,		SDLK_m,
	K_5,		SDLK_5,
	K_6,		SDLK_6,
	K_S,		SDLK_s,
	K_W,		SDLK_w,
	K_F1,		SDLK_F1,
	K_1,		SDLK_1,
	-1,			-1,
};

void copy_bytes( byte *pDst, byte *pSrc, COPY_BYTES_PARAM *pCopyBytesParam )
{
	while( pCopyBytesParam->dstindex >= 0 ) {
		pDst[ pCopyBytesParam->dstindex ] = pSrc[ pCopyBytesParam->srcindex ];
		pCopyBytesParam++;
	}
}

bool do_sdl_update( byte *keytable, MOUSE_STATE *mousestate )
{
	// run state -> not finished
	bool state = false;

	SDL_Event event;
	while( SDL_PollEvent(&event) ) {
		if(event.type == SDL_QUIT)		// if we receive a quit message, then set finished flag
			state = true;
	}

	byte *keystate = (byte *) SDL_GetKeyState( 0 );
	copy_bytes( keytable, keystate, keystate2keytable );

	int dx, dy;
	int buttonstatus = SDL_GetRelativeMouseState(&dx, &dy);

	int rbutton = buttonstatus & 4; // right button
	int lbutton = buttonstatus & 1; // left button
	mousestate->buttons = (rbutton ? 2 : 0) | (lbutton ? 1 : 0); // convert to int 33 format

	int mousex = mousestate->x + dx;
	int mousey = mousestate->y + dy;
	if(mousex < 0)
		mousex = 0;
	if(mousey < 0)
		mousey = 0;
	if(mousex > SCREEN_WIDTH - 1)
		mousex = SCREEN_WIDTH - 1;
	if(mousey > SCREEN_HEIGHT - 1)
		mousey = SCREEN_HEIGHT - 1;

	mousestate->x = mousex;
	mousestate->y = mousey;

	return state;
}

void do_sdl_update_screen( SDL_Surface *screen, byte *src )
{
	//if we can grab the screen
	if(!SDL_LockSurface(screen)) {
		int i, j;

		//make palette
		word pixpalette[256];
		for(i = 0; i < 256; i++) {
			int r, g, b;
			r = (g_palette[i] >> 16) & 0xFF;
			g = (g_palette[i] >> 8) & 0xFF;
			b = (g_palette[i] >> 0) & 0xFF;
			pixpalette[i] = (word)SDL_MapRGB(screen->format, r, g, b);
		}

		//copy screen info
		for(j = 0; j < SCREEN_HEIGHT; j++) {
			word *dstpixarray = (word*)&((byte *)screen->pixels)[j * screen->pitch];
			byte *srcpixarray = &vscreen[j * SCREEN_WIDTH];
			for(i = 0; i < SCREEN_WIDTH; i++)
				dstpixarray[i] = pixpalette[srcpixarray[i]];
		}

		SDL_UnlockSurface(screen);
		SDL_Flip(screen);
	}
}

int main(int argc,char *argv[])
{
   int i,j,k,l,size;
   int x,y,num;
   int buttons,oldbuttons,changebuttons;
   int scroll,curtile,curchip,curmeta,curclr,curpal,pal;
   int mx,my,clx,cly,crx,cry,showoverlay,showclipping,showbattle,showen;
   int tx,ty,notsaved_tiles,notsaved_map,quit,savedpcx;
   int blocked_button,blocked_button2,overlay_button,overlay_button2;
   int pixels;
   int mapx,mapy;
   int mapclipx,mapclipy,selecting,cx1,cx2,cy1,cy2,cxs,cys;
   char cur_tileset_name[128], cur_map_name[128];
   int a,b;

   currentpal=palettes;
   gbamode=0;

   pixels=1;
   scroll=curtile=curchip=curmeta=curclr=curpal=notsaved_tiles=notsaved_map=0;
   mapx=mapy=mapclipx=mapclipy=selecting=cxs=cys=0;

   for(i=0;i<8;i++)
      an_tiles[i]=an_index[i]=an_counter[i]=clip2[i]=0;

   for(i=0;i<16384;i++)
      en[i]=0;

//-------------------------------------------------------------
	if( SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO) < 0 ) {
		printf("Unable to init SDL: %s\n", SDL_GetError());
		return 1;
	}
//-------------------------------------------------------------

//=============================================================
//   // Detect the VBE bios
//   if(NULL == (vbeInfo = vbeDetect()))
//   {
//      printf("Cannot find VESA BIOS extentions.");
//      return 0;
//   }
//
//   // Check for VBE 2.0
//   if( vbeInfo->VbeVersion < 0x200 )
//   {
//      printf("VESA BIOS extentions 2.0 or better needed.");
//      return 0;
//   }
//=============================================================

   printf("\n");
   printf("Affinix Tile/Map Designer by Hideaki Omuro\n");
   printf("(C) 2000 Affinix Software\n\n");
   printf(" ATMD ats_file amp_file [/T:tile_bin_file] [/M:map_bin_file,x,y] [/P:pal_file]\n\n");

//-------------------------------------------------------------
//	SDL_ShowCursor(SDL_DISABLE);
//-------------------------------------------------------------

   if(!mouse_init())
   {
      printf(" No mouse detected.");
      return 1;
   }

   notsaved_map=notsaved_tiles=1;

   if (argc<2)
   {
      printf(" Please specify a name for the tileset.\n");
      return 1;
   }
   else
      switch (open_tileset(argv[1]))
      {
         case -1:
            printf(" Invalid ATS format.\n");
            return 1;
         case 1:
            new_tileset();
         case 0:
            strcpy(cur_tileset_name,argv[1]);
            break;
      }

   if (argc<3)
   {
      printf(" Please specify a name for the map.\n");
      return 1;
   }
   else
      switch (open_map(argv[2]))
      {
         case -1:
            printf(" Invalid AMP format.\n");
            return 1;
         case 1:
            new_map();
         case 0:
            strcpy(cur_map_name,argv[2]);
            break;
      }

   if (argc>=4)
   {
      for(i=3;i<argc;i++)
      {
         if(!strnicmp("/P:",argv[i],3))
         {
            strcpy(tmp,argv[i]+3);

            if (!(fp=fopen(tmp,"rb")))
            {
               printf(" Cannot find PAL file.");
               return 1;
            }
      
            fread(palettes,1,64,fp);
      
            fclose(fp);

            notsaved_tiles=0;
         }
         if(!strnicmp("/T:",argv[i],3))
         {
            strcpy(tmp,argv[i]+3);

            if(import_tiles(tmp))
            {
               printf(" Cannot find BIN file.");
               return 1;
            }

            notsaved_tiles=0;
         }
         if(!strnicmp("/M:",argv[i],3))
         {
            strcpy(tmp,argv[i]+3);
            tmp[strlen(argv[i])]=0;
            if(!(tmp2=strchr(tmp,',')))
            {
               printf(" No width/height specified for import.");
               return 1;
            }
            *tmp2++=0;

            if(!(tmp3=strchr(tmp2,',')))
            {
               printf(" No height specified for import.");
               return 1;
            }
            *tmp3++=0;

            if(strchr(tmp3,','))
               l=0;
            else
               l=1;

            j=atoi(tmp2);
            k=atoi(tmp3);

            if (!(fp=fopen(tmp,"rb")))
            {
               printf(" Cannot find MAP file.");
               return 1;
            }

            fread(tmpbuf, j, k, fp);
            fclose(fp);

            for(y=0;y<k;y++)
               for(x=0;x<j;x++)
                  map[(height*l+y)*128+x+width-width*l]=tmpbuf[y*j+x];

            width +=j-j*l;
            height+=k*l;
         }
      }
   }

   notsaved_tiles=notsaved_map=0;

//-------------------------------------------------------------
	SDL_Surface *screen;
	screen = SDL_SetVideoMode(640, 480, 16, SDL_HWSURFACE | SDL_DOUBLEBUF | SDL_FULLSCREEN);
//-------------------------------------------------------------

//=============================================================
////   if (NULL == (m640x480 = vbeOpen(512, 384, 8)))
//   if (NULL == (m640x480 = vbeOpen(640, 480, 8)))
//   {
//      printf("Could not set 640x480x256 mode.");
//   }
//   vga=(unsigned byte *)m640x480->lfb;
//   vbeSetScanWidth(640);
//=============================================================

//	vga = (unsigned byte *)screen->pixels;

   memset(clip,0,17);

//=============================================================
//   init_kh();
//=============================================================

   for(i=0;i<32;i++)
      set_pal(i+128,i);

   for(i=0;i<256;i++)
      conv_tile(i);

   glib.setdest(vscreen);

   atmdmode=savedpcx=blocked_button2=overlay_button2=0;

//-------------------------------------------------------------
	SDL_ShowCursor(SDL_DISABLE);
	bool done = false;
	int dummy;
	SDL_GetRelativeMouseState(&dummy, &dummy);
	buttons = 0;
//-------------------------------------------------------------

   for(;;)
   {
//-------------------------------------------------------------
		done = do_sdl_update( key_table, &gMouseState );
//-------------------------------------------------------------
      quit=showen=showoverlay=blocked_button=overlay_button=showclipping=showbattle=0;

      oldbuttons=buttons;
      buttons=mouse_status(&mx,&my);
      changebuttons=oldbuttons^buttons;

      pal=curpal*4+curclr;

      if(key_table[K_L])
      {
         fname=get_file_list(&i, ".ATS");

         if(i)
         {
            for(j=0;j<i;j++)
            {
               l=j;
               for(k=j+1;k<i;k++)
                  if(strcmp(fname[k],fname[l])<0)
                     l=k;
               tmp2=fname[j];
               fname[j]=fname[l];
               fname[l]=tmp2;
            }
            k=0;

            for (;;)
            {
//-------------------------------------------------------------
				done = do_sdl_update( key_table, &gMouseState );
//-------------------------------------------------------------
               oldbuttons=buttons;
               buttons=mouse_status(&mx,&my);
               changebuttons=oldbuttons^buttons;

               glib.fillpage(15);

               x=(mx-8)/104,y=(my-8)/8;
               l=x*58+y;
               if(mx>=8&&my>=8&&my<472&&(mx-8)%104<96&&l<i)
                  k=l;

               x=k/58,y=k%58;
               glib.drawbar(8+x*104,8+y*8,8+95+x*104,8+7+y*8,12);

               for(j=0;j<i;j++)
               {
                  x=j/58,y=j%58;
                  draw_string(8+x*104,8+y*8,fname[j],0);
               }

               if(changebuttons&buttons&1)
               {
                  strcpy(cur_tileset_name,fname[k]);
                  open_tileset(cur_tileset_name);

                  for (x=0;x<32;x++)
                     set_pal(x+128,x);

                  notsaved_tiles=0;
               }

               if(changebuttons&buttons)
               {
//================================================================
//                  while(mouse_status(&mx,&my));
//================================================================
//----------------------------------------------------------------
					while(mouse_status(&mx,&my))
						done = do_sdl_update( key_table, &gMouseState );
//----------------------------------------------------------------
				   oldbuttons=changebuttons=buttons=0;
                  break;
               }

               glib.putspr(mx,my,ptrgfx);

//================================================================
//               glib.vrwait();
//               glib.copypage(vga);
//================================================================
//----------------------------------------------------------------
			  do_sdl_update_screen( screen, vscreen );
//----------------------------------------------------------------
            }
         }
         clear_file_list(i, fname);
      }

      if (key_table[K_E])
      {
         if(!savedpcx)
         {
            savedpcx=1;
            save_mappcx();
         }
      }
      else
         savedpcx=0;

      if(key_table[K_I])
      {
         for(j=0;j<9;j++)
            for(i=0;i<11;i++)
            {
               x=i*24,y=j*24+12-(i&1)*12;

               k=j==8&&i&1;

               if(attr[map[((y   )&~15)*8+(x   )/16]*4+1]&0x08
                ||attr[map[((y   )&~15)*8+(x+15)/16]*4+1]&0x08
                ||attr[map[((y+15)&~15)*8+(x   )/16]*4+1]&0x08
                ||attr[map[((y+15)&~15)*8+(x+15)/16]*4+1]&0x08)
                  k=1;

               enemymap[j*11+i]=k;
            }

         k=0;

         strcpy(tmp,cur_map_name);
         for(i=0;i<12;i++)
            if(tmp[i]=='.')
            {
               tmp[i+1]='H';
               tmp[i+2]='I';
               tmp[i+3]='T';
               tmp[i+4]=0;
               k=1;
               break;
            }

         if(k)
         {
            fp=fopen(tmp,"wb");
            fwrite(enemymap,11,9,fp);
            fclose(fp);
         }

//================================================================
//         while(key_table[K_I]);
//================================================================
//----------------------------------------------------------------
			while(key_table[K_I])
				done = do_sdl_update( key_table, &gMouseState );
//----------------------------------------------------------------
      }

      if(key_table[K_T])
      {
//================================================================
//         while(key_table[K_T]);
//================================================================
//----------------------------------------------------------------
			while(key_table[K_T])
				done = do_sdl_update( key_table, &gMouseState );
//----------------------------------------------------------------

         fix_mapsize();

         k=0;

         strcpy(tmp,cur_map_name);
         for(i=0;i<12;i++)
            if(tmp[i]=='.')
            {
               tmp[i+1]='X';
               tmp[i+2]='M';
               tmp[i+3]='P';
               tmp[i+4]=0;
               k=1;
               break;
            }

         if(k)
         {
            fp=fopen(tmp,"wb");

            for(y=0;y<height;y++)
               fwrite(&map[128*y],width,1,fp);

            fclose(fp);
         }
      }

      if (key_table[K_G]||key_table[K_H])
      {
         if(key_table[K_G]&&(fp=fopen("export1.bin","rb")))
         {
            fseek(fp,0,SEEK_END);
            size=ftell(fp);
            i=size/17;
            j=(i>127)?128:i+1;
            rewind(fp);

            fread(tmparray    , 1,i,fp);
            fread(tmparray+256,16,i,fp);

            fclose(fp);
         }
         else
            i=0,j=1;

         tmparray[i]=palettetable[curtile];
         memcpy(&tmparray[16*i]+256,&tilebuf[16*curtile],16);

         if(fp=fopen("export1.bin","wb"))
         {
            fwrite(tmparray    , 1,j,fp);
            fwrite(tmparray+256,16,j,fp);

            fclose(fp);
         }

//================================================================
//         while(key_table[K_G]||key_table[K_H]);
//================================================================
//----------------------------------------------------------------
			while(key_table[K_G]||key_table[K_H])
				done = do_sdl_update( key_table, &gMouseState );
//----------------------------------------------------------------
      }

      if (key_table[K_Y])
      {
         notsaved_tiles=!import_tiles("export1.bin");

         for(i=0;i<256;i++)
            conv_tile(i);

//================================================================
//         while(key_table[K_Y]);
//================================================================
//----------------------------------------------------------------
			while(key_table[K_Y])
				done = do_sdl_update( key_table, &gMouseState );
//----------------------------------------------------------------
      }

      if (key_table[K_J])
      {
         gbamode^=1;
         if(gbamode)
            currentpal=gbapalettes;
         else
            currentpal=palettes;

         for(i=0;i<32;i++)
            set_pal(i+128,i);

//================================================================
//         while(key_table[K_J]);
//================================================================
//----------------------------------------------------------------
			while(key_table[K_J])
				done = do_sdl_update( key_table, &gMouseState );
//----------------------------------------------------------------
      }

      if (key_table[K_F2])
      {
         for(j=127;j>=0;j--)
            for(i=0;i<128;i++)
               if(map[j*128+i])
                  a=j;
         for(j=0;j<128;j++)
            for(i=0;i<128;i++)
               if(map[j*128+i])
                  b=j-a;

         for(j=0;j<=b;j++)
            for(i=0;i<128;i++)
               map[j*128+i]=map[(j+a)*128+i],
               en[j*128+i]=en[(j+a)*128+i];
         for(;j<128;j++)
            for(i=0;i<128;i++)
               map[j*128+i]=0,
               en[j*128+i]=0;

         for(i=127;i>=0;i--)
            for(j=0;j<128;j++)
               if(map[j*128+i])
                  a=i;
         for(i=0;i<128;i++)
            for(j=0;j<128;j++)
               if(map[j*128+i])
                  b=i-a;

         for(i=0;i<=b;i++)
            for(j=0;j<128;j++)
               map[j*128+i]=map[j*128+(i+a)],
               en[j*128+i]=en[j*128+(i+a)];
         for(;i<128;i++)
            for(j=0;j<128;j++)
               map[j*128+i]=0,
               en[j*128+i]=0;

         notsaved_map=1;
//================================================================
//         while(key_table[K_F2]);
//================================================================
//----------------------------------------------------------------
			while(key_table[K_F2])
				done = do_sdl_update( key_table, &gMouseState );
//----------------------------------------------------------------
      }

      if (key_table[K_F3])
      {
         for(j=127;j>=0;j--)
            for(i=127;i>=0;i--)
               map[j*128+i]=((i&&j)?map[(j-1)*128+(i-1)]:0),
               en[j*128+i]=((i&&j)?en[(j-1)*128+(i-1)]:0);

         notsaved_map=1;
//================================================================
//         while(key_table[K_F3]);
//================================================================
//----------------------------------------------------------------
			while(key_table[K_F3])
				done = do_sdl_update( key_table, &gMouseState );
//----------------------------------------------------------------
      }

      if (key_table[K_F5])
      {
         for(i=0;i<16;i++)
            tilebuf[curtile*16+i]=(tilebuf[curtile*16+i]<<1)|((tilebuf[curtile*16+i]&128)>>7);
         conv_tile(curtile);
         notsaved_tiles=1;
//================================================================
//         while(key_table[K_F5]);
//================================================================
//----------------------------------------------------------------
			while(key_table[K_F5])
				done = do_sdl_update( key_table, &gMouseState );
//----------------------------------------------------------------
      }

      if (key_table[K_F6])
      {
         for(i=0;i<16;i++)
            tilebuf[curtile*16+i]=(tilebuf[curtile*16+i]>>1)|((tilebuf[curtile*16+i]&1)<<7);
         conv_tile(curtile);
         notsaved_tiles=1;
//================================================================
//         while(key_table[K_F6]);
//================================================================
//----------------------------------------------------------------
			while(key_table[K_F6])
				done = do_sdl_update( key_table, &gMouseState );
//----------------------------------------------------------------
      }

      if (key_table[K_F7])
      {
         k=tilebuf[curtile*16];
         l=tilebuf[curtile*16+1];

         for(i=0;i<7;i++)
            for(j=0;j<2;j++)
               tilebuf[curtile*16+i*2+j]=tilebuf[curtile*16+(i+1)*2+j];

         tilebuf[curtile*16+14]=k;
         tilebuf[curtile*16+15]=l;

         conv_tile(curtile);
         notsaved_tiles=1;
//================================================================
//         while(key_table[K_F7]);
//================================================================
//----------------------------------------------------------------
			while(key_table[K_F7])
				done = do_sdl_update( key_table, &gMouseState );
//----------------------------------------------------------------
      }

      if (key_table[K_F8])
      {
         k=tilebuf[curtile*16+14];
         l=tilebuf[curtile*16+15];

         for(i=0;i<7;i++)
            for(j=0;j<2;j++)
               tilebuf[curtile*16+(7-i)*2+j]=tilebuf[curtile*16+(6-i)*2+j];

         tilebuf[curtile*16]=k;
         tilebuf[curtile*16+1]=l;

         conv_tile(curtile);
         notsaved_tiles=1;
//================================================================
//         while(key_table[K_F8]);
//================================================================
//----------------------------------------------------------------
			while(key_table[K_F8])
				done = do_sdl_update( key_table, &gMouseState );
//----------------------------------------------------------------
      }

      if (key_table[K_F9])
      {
         for(i=0;i<16;i++)
         {
            j=tilebuf[curtile*16+i];
            for(k=0,l=0;k<8;k++)
            {
               l<<=1;
               l|=j&1;
               j>>=1;
            }
            tilebuf[curtile*16+i]=l;
         }
         conv_tile(curtile);
         notsaved_tiles=1;
//================================================================
//         while(key_table[K_F9]);
//================================================================
//----------------------------------------------------------------
			while(key_table[K_F9])
				done = do_sdl_update( key_table, &gMouseState );
//----------------------------------------------------------------
      }

      if (key_table[K_F10])
      {
         for(i=0;i<4;i++)
            for(j=0;j<2;j++)
            {
               k=tilebuf[curtile*16+i*2+j];
               tilebuf[curtile*16+i*2+j]=tilebuf[curtile*16+(7-i)*2+j];
               tilebuf[curtile*16+(7-i)*2+j]=k;
            }

         conv_tile(curtile);
         notsaved_tiles=1;
//================================================================
//         while(key_table[K_F10]);
//================================================================
//----------------------------------------------------------------
			while(key_table[K_F10])
				done = do_sdl_update( key_table, &gMouseState );
//----------------------------------------------------------------
      }

      if (key_table[K_P])
      {
         fname=get_file_list(&i, ".AMP");

         if(i)
         {
            for(j=0;j<i;j++)
            {
               l=j;
               for(k=j+1;k<i;k++)
                  if(strcmp(fname[k],fname[l])<0)
                     l=k;
               tmp2=fname[j];
               fname[j]=fname[l];
               fname[l]=tmp2;
            }
            k=0;

            for (;;)
            {
//----------------------------------------------------------------
				done = do_sdl_update( key_table, &gMouseState );
//----------------------------------------------------------------
               oldbuttons=buttons;
               buttons=mouse_status(&mx,&my);
               changebuttons=oldbuttons^buttons;

               glib.fillpage(15);

               x=(mx-8)/104,y=(my-8)/8;
               l=x*58+y;
               if(mx>=8&&my>=8&&my<472&&(mx-8)%104<96&&l<i)
                  k=l;

               x=k/58,y=k%58;
               glib.drawbar(8+x*104,8+y*8,8+95+x*104,8+7+y*8,12);

               for(j=0;j<i;j++)
               {
                  x=j/58,y=j%58;
                  draw_string(8+x*104,8+y*8,fname[j],0);
               }

               if(changebuttons&buttons&1)
               {
                  strcpy(cur_map_name,fname[k]);
                  open_map(cur_map_name);
                  notsaved_map=0;
               }

               if(changebuttons&buttons)
               {
//================================================================
//                  while(mouse_status(&mx,&my));
//================================================================
//----------------------------------------------------------------
					while(mouse_status(&mx,&my))
						done = do_sdl_update( key_table, &gMouseState );
//----------------------------------------------------------------
                  oldbuttons=changebuttons=buttons=0;
                  break;
               }

               glib.putspr(mx,my,ptrgfx);

//================================================================
//               glib.vrwait();
//               glib.copypage(vga);
//================================================================
//----------------------------------------------------------------
			  do_sdl_update_screen( screen, vscreen );
//----------------------------------------------------------------
            }
         }

         clear_file_list(i, fname);
      }

      if (key_table[K_ESC])
         quit=((key_table[K_1])?2:0)+1;
      if (key_table[K_Q])
         scroll = (scroll - 1) & 31;
      if (key_table[K_A])
         scroll = (scroll + 1) & 31;

	  if (key_table[K_UP])
         if(mapy)
            mapy--;
      if (key_table[K_DOWN])
         if(mapy+16<128)
            mapy++;
      if (key_table[K_LEFT])
         if(mapx)
            mapx--;
      if (key_table[K_RIGHT])
         if(mapx+20<128)
            mapx++;
      if (key_table[K_O])
         showoverlay=1;
      if (key_table[K_B])
         showclipping=1;
      if (key_table[K_Z])
         showbattle=1;
      if (key_table[K_U])
         showen=1;

      if (key_table[K_LEFT])
         if (pixels > 1)
            pixels--;
      if (key_table[K_RIGHT])
         pixels++;

      if (key_table[K_D])
      {
//================================================================
//         while(key_table[K_D]);
//================================================================
//----------------------------------------------------------------
			while(key_table[K_D])
				done = do_sdl_update( key_table, &gMouseState );
//----------------------------------------------------------------

         if(curmeta<256)
         {
            for(i=0;i<4;i++)
            {
               clip2[i]=tile16[curmeta*4+i];
               clip2[i+4]=attr[curmeta*4+i];
               tile16[curmeta*4+i]=0;
               attr[curmeta*4+i]=0;
            }
            notsaved_tiles=1;
         }
      }
      if (key_table[K_R])
      {
//================================================================
//         while(key_table[K_R]);
//================================================================
//----------------------------------------------------------------
			while(key_table[K_R])
				done = do_sdl_update( key_table, &gMouseState );
//----------------------------------------------------------------

         if(curmeta<256)
         {
            for(i=0;i<4;i++)
            {
               tile16[curmeta*4+i]=clip2[i];
               attr[curmeta*4+i]=clip2[i+4];
            }
            notsaved_tiles=1;
         }
      }

      if (key_table[K_X])
      {
//================================================================
//         while(key_table[K_X]);
//================================================================
//----------------------------------------------------------------
			while(key_table[K_X])
				done = do_sdl_update( key_table, &gMouseState );
//----------------------------------------------------------------
         memcpy(clip,&tilebuf[curtile*16],16);
         memset(&tilebuf[curtile*16],0,16);
         clip[16]=palettetable[curtile];
         palettetable[curtile]=0;
         conv_tile(curtile);
         notsaved_tiles=1;
      }
      if (key_table[K_C])
      {
         memcpy(clip,&tilebuf[curtile*16],16);
         clip[16]=palettetable[curtile];
      }
      if (key_table[K_V])
      {
         memcpy(&tilebuf[curtile*16],clip,16);
         palettetable[curtile]=clip[16];
         conv_tile(curtile);
         notsaved_tiles = 1;
      }
      if (key_table[K_F])
      {
         for(i=0;i<8;i++)
            for(j=128;j;j>>=1)
            {
               k=clip[i*2]&j,l=clip[i*2+1]&j;
   
               if(k||l)
                  tilebuf[curtile*16+i*2  ]=tilebuf[curtile*16+i*2  ]&~j|k,
                  tilebuf[curtile*16+i*2+1]=tilebuf[curtile*16+i*2+1]&~j|l;
            }
         conv_tile(curtile);
         notsaved_tiles = 1;
      }

      if (changebuttons&buttons&1)
      {
         clx=mx,cly=my;

         if(key_table[K_LSHIFT])
         {
            if mouse_in(clx,cly,175,148,207,180)
            {
               x=(clx-175)/4,y=(cly-148)/4;
               clx=-100;

               k =(tilebuf[curtile*16+y * 2]&(128>>x))?1:0;
               k+=(tilebuf[curtile*16+y*2+1]&(128>>x))?2:0;

               j=curclr^k;
               i=(j&1)?128:0;
               j=(j&2)?128:0;

               for(y=0;y<8;y++)
                  for(x=0;x<8;x++)
                  {
                     l =(tilebuf[curtile*16+y * 2]&(128>>x))?1:0;
                     l+=(tilebuf[curtile*16+y*2+1]&(128>>x))?2:0;

                     if (l==k||l==curclr)
                        tilebuf[curtile*16+y * 2]^=i>>x,
                        tilebuf[curtile*16+y*2+1]^=j>>x;
                  }

               conv_tile(curtile);
               notsaved_tiles=1;
            }

            if(mapclipx&&mapclipy)
               if mouse_in(clx,cly,0,200,320,456)
               {
                  tx=(mx)/16+mapx,ty=(my-200)/16+mapy;
                  clx=-100;

                  for(y=0;y<mapclipy;y++)
                     for(x=0;x<mapclipx;x++)
                        if((tx+x)<128&&(ty+y)<128)
                        {
                           map[(ty+y)*128+(tx+x)]=tmpbuf[y*mapclipx+x];
                           en[(ty+y)*128+(tx+x)]=tmpen[y*mapclipx+x];
                        }

                  notsaved_map=1;
               }
         }

         if mouse_in(clx,cly,1,69,137,188)
         {
            x=(clx-1)/17,y=(cly-69)/17;
            curmeta=((scroll*8+y*8+x)&255)+atmdmode*256;
         }
         if mouse_in(clx,cly,174,1,318,145)
         {
            x=(clx-174)/9,y=(cly-1)/9;
            curtile=y*16+x;
         }
         if mouse_in(clx,cly,1,1,67,67)
         {
            x=(clx-1)/33,y=(cly-1)/33;
            curchip=y*2+x;
         }
         if mouse_in(clx,cly,209,147,257,156)
         {
            x=(clx-209)/12;
            curclr=x;
         }
         if mouse_in(clx,cly,209,158,257,190)
         {
            num=(cly-158)/4;

            num_metas=1;   
            for (y=0;y<256;y++)
            {
               for (x=0;x<4;x++)
                  if (tile16[y*4+x])
                     num_metas=y+1;
            }

            for (y=0;y<num_metas;y++)
               for (x=0;x<4;x++)
                  if (tile16[y*4+x]==curtile)
                     if ((attr[y*4+x]&7)==(palettetable[curtile]&7))
                        attr[y*4+x]=(attr[y*4+x]&0xF8)|num;

            palettetable[curtile]=num;

            notsaved_tiles=1;
         }
         if(curmeta<256)
         {
            if mouse_in(clx,cly,69,1,84,16)
               attr[curmeta*4+curchip]^=hflip,notsaved_tiles=1;
            if mouse_in(clx,cly,69,17,84,32)
               attr[curmeta*4+curchip]^=vflip,notsaved_tiles=1;
            if mouse_in(clx,cly,69,49,84,64)
            {
               i=attr[curmeta*4+curchip];
               j=(i+1)&7,i=(i&0xF8)|j;
               attr[curmeta*4+curchip]=i;
               notsaved_tiles=1;
            }
            if mouse_in(clx,cly,117,1,125,9)
            {
               attr[curmeta*4+0]^=misc;
               notsaved_tiles=1;
            }
            if mouse_in(clx,cly,125,1,133,9)
            {
               attr[curmeta*4+1]^=misc;
               notsaved_tiles=1;
            }
            if mouse_in(clx,cly,133,1,141,9)
            {
               attr[curmeta*4+2]^=misc;
               notsaved_tiles=1;
            }
            if mouse_in(clx,cly,141,1,149,9)
            {
               attr[curmeta*4+3]^=misc;
               notsaved_tiles=1;
            }
         }

         if mouse_in(clx,cly,69,33,84,48)
            overlay_button=1;
         if mouse_in(clx,cly,85,33,100,48)
            blocked_button=1;
         if mouse_in(clx,cly,139,69,154,83)
            scroll=(scroll-1)&31;
         if mouse_in(clx,cly,139,173,154,186)
            scroll=(scroll+1)&31;
         if mouse_in(clx,cly,264,147,271,151)
            if ((i=get_comp(currentpal,pal,pal_r))<31)
            {
               set_comp(currentpal,pal,pal_r,++i);
               set_pal(pal+128,pal);
               notsaved_tiles=1;
            }
         if mouse_in(clx,cly,264,185,271,188)
            if (i=get_comp(currentpal,pal,pal_r))
            {
               set_comp(currentpal,pal,pal_r,--i);
               set_pal(pal+128,pal);
               notsaved_tiles=1;
            }
         if mouse_in(clx,cly,284,147,291,151)
            if ((i=get_comp(currentpal,pal,pal_g))<31)
            {
               set_comp(currentpal,pal,pal_g,++i);
               set_pal(pal+128,pal);
               notsaved_tiles=1;
            }
         if mouse_in(clx,cly,284,185,291,188)
            if (i=get_comp(currentpal,pal,pal_g))
            {
               set_comp(currentpal,pal,pal_g,--i);
               set_pal(pal+128,pal);
               notsaved_tiles=1;
            }
         if mouse_in(clx,cly,304,147,311,151)
            if ((i=get_comp(currentpal,pal,pal_b))<31)
            {
               set_comp(currentpal,pal,pal_b,++i);
               set_pal(pal+128,pal);
               notsaved_tiles=1;
            }
         if mouse_in(clx,cly,304,185,311,188)
            if (i=get_comp(currentpal,pal,pal_b))
            {
               set_comp(currentpal,pal,pal_b,--i);
               set_pal(pal+128,pal);
               notsaved_tiles=1;
            }

         if mouse_in(clx,cly,321,200,336,215)
         {
            if(mapy)
               mapy--;
         }
         if mouse_in(clx,cly,321,441,336,456)
         {
            if(mapy+16<128)
               mapy++;
         }
         if mouse_in(clx,cly,0,457,15,472)
         {
            if(mapx)
               mapx--;
         }
         if mouse_in(clx,cly,305,457,320,472)
         {
            if(mapx+20<128)
               mapx++;
         }
         if mouse_in(clx,cly,321,232,336,247)
         {
            map_type^=scrollable;
            notsaved_map=1;
         }

         x=0;
         for(i=0;i<8;i++)
         {
            if mouse_in(clx,cly,640-256+11,302+i*12,640-256+16,309+i*12)
               if(an_tiles[i])
                  an_tiles[i]--,x=1;
            if mouse_in(clx,cly,640-256+20+8*16,302+i*12,640-256+25+8*16,309+i*12)
               if(an_tiles[i]<16)
                  an_tiles[i]++,x=1;
         }

         if(x)
            for(i=0,notsaved_tiles=1;i<8;i++)
               an_counter[i]=0;
      }

      if(key_table[K_N])
         blocked_button=1;

      if(key_table[K_M])
         overlay_button=1;

      if(key_table[K_5])
         atmdmode=0;

      if(key_table[K_6])
         atmdmode=1;

      if(blocked_button==1)
      {
         if(!blocked_button2)
         {
            blocked_button2=1;
     
            if(curmeta<256)
            {
               attr[curmeta*4+curchip]^=block;
               attr[curmeta*4+0]&=~misc;
               attr[curmeta*4+1]&=~misc;
               for(i=0;i<4;i++)
               {
                  if(attr[curmeta*4+i]&block)
                  {
                     attr[curmeta*4+0]|=misc;
                     attr[curmeta*4+1]|=misc;
                  }
               }
               notsaved_tiles=1;
            }
         }
      }
      else
         blocked_button2=0;

      if(overlay_button==1)
      {
         if(!overlay_button2)
         {
            overlay_button2=1;

            if(curmeta<256)
            {
               attr[curmeta*4+curchip]^=priority;
               notsaved_tiles=1;
            }
         }
      }
      else
         overlay_button2=0;

      if (key_table[K_S] || ((changebuttons&buttons&1) && (mouse_in(clx,cly,85,17,100,32))))
      {
         if (notsaved_tiles)
         {
            notsaved_tiles=0;
            save_tileset(cur_tileset_name);
         }
      }

      if (key_table[K_W] || ((changebuttons&buttons&1) && (mouse_in(clx,cly,321,216,336,231))))
      {
         if (notsaved_map)
         {
            notsaved_map=0;
            save_map(cur_map_name);
         }
      }

      if (buttons&1)
      {
         if mouse_in(clx,cly,175,148,207,180)
            if mouse_in(mx,my,175,148,207,180)
            {
               x=(mx-175)/4,y=(my-148)/4;   
               i=(curclr&2)>>1,j=(curclr&1);   
               tilebuf[curtile*16+y * 2]&=~(1<<(7-x));
               tilebuf[curtile*16+y*2+1]&=~(1<<(7-x));
               tilebuf[curtile*16+y * 2]|= (j<<(7-x));
               tilebuf[curtile*16+y*2+1]|= (i<<(7-x));
               conv_tile(curtile);
               notsaved_tiles=1;
            }
         if mouse_in(clx,cly,85,1,100,16)
            showoverlay=1;
         if mouse_in(clx,cly,101,1,116,16)
            showclipping=1;
         if mouse_in(clx,cly,262,152,273,184)
         {
            i=183-my;
            i=(i<0)?0:(i>31)?31:i;
            set_comp(currentpal,pal,pal_r,i);
            set_pal(pal+128,pal);
            notsaved_tiles=1;
         }
         if mouse_in(clx,cly,282,152,293,184)
         {
            i=183-my;
            i=(i<0)?0:(i>31)?31:i;
            set_comp(currentpal,pal,pal_g,i);
            set_pal(pal+128,pal);
            notsaved_tiles=1;
         }
         if mouse_in(clx,cly,302,152,313,184)
         {
            i=183-my;
            i=(i<0)?0:(i>31)?31:i;
            set_comp(currentpal,pal,pal_b,i);
            set_pal(pal+128,pal);
            notsaved_tiles=1;
         }

         if mouse_in(clx,cly,0,200,320,456)
            if mouse_in(mx,my,0,200,320,456)
            {
               tx=(mx)/16+mapx,ty=(my-200)/16+mapy;
               if(curmeta<256)
                  map[ty*128+tx]=curmeta;
               else
                  en[ty*128+tx]=curmeta-256;
               notsaved_map=1;
            }
      }

      if (changebuttons&buttons&2)
      {
         crx=mx,cry=my;
         if mouse_in(crx,cry,69,1,84,17)
         {
            if(curmeta<256)
            {
               for (i=0;i<4;i++)
                  datastruct[i]=attr[curmeta*4+i]&(misc);
               swap(tile16[curmeta * 4],tile16[curmeta*4+1]);
               swap(tile16[curmeta*4+2],tile16[curmeta*4+3]);
               swap(attr  [curmeta * 4],attr  [curmeta*4+1]);
               swap(attr  [curmeta*4+2],attr  [curmeta*4+3]);
               for (i=0;i<4;i++)
                  attr[curmeta*4+i]=((attr[curmeta*4+i]^hflip)&~(misc))|datastruct[i];
               notsaved_tiles=1;
            }
         }
         if mouse_in(crx,cry,69,17,84,33)
         {
            if(curmeta<256)
            {
               for (i=0;i<4;i++)
                  datastruct[i]=attr[curmeta*4+i]&(misc);
               swap(tile16[curmeta * 4],tile16[curmeta*4+2]);
               swap(tile16[curmeta*4+1],tile16[curmeta*4+3]);
               swap(attr  [curmeta * 4],attr  [curmeta*4+2]);
               swap(attr  [curmeta*4+1],attr  [curmeta*4+3]);
               for (i=0;i<4;i++)
                  attr[curmeta*4+i]=((attr[curmeta*4+i]^vflip)&~(misc))|datastruct[i];
               notsaved_tiles=1;
            }
         }
         if mouse_in(crx,cry,175,148,207,180)
         {
            x=(crx-175)/4,y=(cry-148)/4;
            curclr=((tilebuf[curtile*16+y * 2]&(1<<(7-x)))?1:0)
                  +((tilebuf[curtile*16+y*2+1]&(1<<(7-x)))?2:0);
         }
         if mouse_in(crx,cry,1,1,67,67)
         {
            if(curmeta<256)
            {
               x=(crx-1)/33,y=(cry-1)/33;
               curchip=y*2+x;
               curtile=tile16[curmeta*4+curchip];
            }
         }
         if mouse_in(crx,cry,0,200,320,456)
         {
            tx=(crx)/16+mapx,ty=(cry-200)/16+mapy;
            if(!atmdmode)
               curmeta=map[ty*128+tx];
            else
               curmeta=en[ty*128+tx]+256;
         }
         if mouse_in(crx,cry,69,49,84,64)
         {
            i=attr[curmeta*4+curchip];
            j=(i-1)&7,i=(i&0xF8)|j;
            attr[curmeta*4+curchip]=i;
            notsaved_tiles=1;
         }
      }

      if(buttons&2)
      {
         if mouse_in(crx,cry,0,200,320,456)
            if mouse_in(mx,my,0,200,320,456)
            {
               tx=(mx)/16+mapx,ty=(my-200)/16+mapy;

               if(selecting)
               {
                  cx2=tx,cy2=ty;

                  if(cx2>cx1)
                     mapclipx=cx2-cx1+1,cxs=cx1;
                  else
                     mapclipx=cx1-cx2+1,cxs=cx2;

                  if(cy2>cy1)
                     mapclipy=cy2-cy1+1,cys=cy1;
                  else
                     mapclipy=cy1-cy2+1,cys=cy2;
               }
               else
               {
                  if(!selecting)
                  {
                     cx1=cx2=cxs=tx,cy1=cy2=cys=ty;
                     mapclipx=1,mapclipy=1;
                     selecting=1;
                  }
               }
            }
      }
      else
      {
         if(selecting)
         {
            if(mapclipx==1&&mapclipy==1)
               selecting=0;

            if(selecting)
               for(y=0;y<mapclipy;y++)
                  for(x=0;x<mapclipx;x++)
                  {
                     tmpbuf[y*mapclipx+x]=map[(cys+y)*128+(cxs+x)];
                     tmpen[y*mapclipx+x]=en[(cys+y)*128+(cxs+x)];
                  }

            selecting=0;
         }
      }

      if ((changebuttons&1)&&!(buttons&1))
      {
         if mouse_in(clx,cly,174,1,318,145)
            if mouse_in(mx,my,1,1,67,67)
            {
               if(curmeta<256)
               {
                  x =(mx -  1)/33,y =(my -1)/33;
                  tx=(clx-174)/ 9,ty=(cly-1)/ 9;
                  tile16[curmeta*4+y*2+x]=ty*16+tx;
                  attr  [curmeta*4+y*2+x]&=block|misc; //t_entry|t_exit;
                  attr  [curmeta*4+y*2+x]|=palettetable[ty*16+tx];
                  curchip=y*2+x;
                  notsaved_tiles=1;
               }
            }
            else if mouse_in(mx,my,174,1,318,145)
            {
               x =(mx -174)/ 9,y =(my -1)/ 9;
               tx=(clx-174)/ 9,ty=(cly-1)/ 9;
               i=y*16+x,j=ty*16+tx;
               if (i!=j)
               {
                  notsaved_tiles=1;
                  swap(palettetable[i],palettetable[j]);
                  for (x=0;x<16;x++)
                     swap(tilebuf[i*16+x],tilebuf[j*16+x]);
                  conv_tile(i);
                  conv_tile(j);

                  num_metas=1;   
                  for (y=0;y<256;y++)
                  {
                     for (x=0;x<4;x++)
                        if (tile16[y*4+x])
                           num_metas=y+1;
                  }

                  num=i^j;

                  for (y=0;y<num_metas;y++)
                     for (x=0;x<4;x++)
                        if (tile16[y*4+x]==i||tile16[y*4+x]==j)
                           tile16[y*4+x]^=num;
               }
            }
            else
            {
               x=0;
               tx=(clx-174)/ 9,ty=(cly-1)/ 9;
               for(i=0;i<8;i++)
               {
                  if mouse_in(mx,my,640-256+1,301+i*12,640-256+9,309+i*12)
                     an_index[i]=ty*16+tx,x=1;
               }
               if(x)
                  for(i=0,notsaved_tiles=1;i<8;i++)
                     an_counter[i]=0;
            }

         if mouse_in(clx,cly,1,69,137,188)
         {
            if mouse_in(mx,my,1,69,137,188)
            {
               if(!atmdmode)
               {
                  x =(mx -1)/17,y =(my -69)/17;
   //               tx=(clx-1)/17,ty=(cly-69)/17;
                  i=(scroll*8+y*8+x)&255,j=curmeta; //(scroll*8+ty*8+tx)&255;
                  if (i!=j)
                  {
                     notsaved_tiles=1;
                     notsaved_map=1;
                     for (x=0;x<4;x++)
                     {
                        swap(tile16[i*4+x],tile16[j*4+x]);
                        swap(attr  [i*4+x],attr  [j*4+x]);
                     }
   
                     num=i^j;
   
                     for(y=0;y<height;y++)
                        for(x=0;x<width;x++)
                           if(map[y*128+x]==i||map[y*128+x]==j)
                              map[y*128+x]^=num;
                  }
               }
            }
            if mouse_in(mx,my,0,200,320,456)
            {
               tx=(mx)/16+mapx,ty=(my-200)/16+mapy;
               x =(clx-1)/17  ,y =(cly-69)/17;
               map[ty*128+tx]=(scroll*8+y*8+x)&255;
               notsaved_map=1;
            }
         }
      }

      curpal = palettetable[curtile];

      glib.fillpage(20);

      for (j=0,y=69,num=scroll*8;j<7;j++,y+=17)
         for (i=0,x=1;i<8;i++,x+=17,num=(num+1)&255)
         {
            glib.drawbox(x,y,x+17,y+17,0);
            if (!showoverlay)
            {
               if(atmdmode)
                  draw_appr16(x+1,y+1,num);
               else
                  draw_tile16(x+1,y+1,num,0x7F);
            }
         }

      for (j=0,y=1,num=0;j<16;j++,y+=9)
         for (i=0,x=174;i<16;i++,x+=9,num++)
         {
            glib.drawbox(x,y,x+9,y+9,0);
            draw_tile(x+1,y+1,num,palettetable[num]);
         }

      glib.drawbox( 1, 1,34,34,0);
      glib.drawbox(34, 1,67,34,0);
      glib.drawbox( 1,34,34,67,0);
      glib.drawbox(34,34,67,67,0);
      if (!showoverlay)
         if(curmeta<256)
            draw_big_tile16(1,1,curmeta,0x7F);

      glib.drawbox(174,147,207,180,0);
      draw_big_tile(175,148,curtile,palettetable[curtile]);

      for (i=0;i<4;i++)
      {
         glib.drawbox(209+i*12,147,221+i*12,156,0);
         glib.drawbar(210+i*12,148,220+i*12,155,128+4*curpal+i);
      }

      for (i=0;i<8;i++)
      {
         for (j=0;j<4;j++)
            glib.drawbar(210+j*12,159+i*4,221+j*12,161+i*4,128+i*4+j);
         glib.drawbox(209,158+i*4,257,162+i*4,0);
      }

      glib.putsprblk(69, 1,hflipgfx);
      if(curmeta<256)
         if (attr[curmeta*4+curchip]&hflip)
            glib.drawbox(69, 1,83,15,15);

      glib.putsprblk(69,17,vflipgfx);
      if(curmeta<256)
         if (attr[curmeta*4+curchip]&vflip)
            glib.drawbox(69,17,83,31,15);

      glib.putsprblk(69,33,overlay);
      if(curmeta<256)
         if (attr[curmeta*4+curchip]&priority)
            glib.drawbox(69,33,83,47,15);

      glib.putsprblk(69,49,palettegfx);
      if(curmeta<256)
         if ((attr[curmeta*4+curchip]&7)!=(palettetable[tile16[curmeta*4+curchip]]&7))
            glib.drawbox(69,49,83,63,15);

      glib.putsprblk(85,33,blockedgfx);
      if(curmeta<256)
         if (attr[curmeta*4+curchip]&block)
            glib.drawbox(85,33,99,47,15);

      glib.putsprblk(85,1,overlayset);
      if (showoverlay)
         glib.drawbox(85,1,99,15,15);

      glib.putsprblk(101,1,blockedset);
      if (showclipping)
        glib.drawbox(101,1,115,15,15);

      glib.putsprblk(85,17,disk);
      if (notsaved_tiles)
         glib.drawbox(85,17,99,31,15);

      glib.putsprblk(321,216,disk);
      if (notsaved_map)
         glib.drawbox(321,216,335,230,15);

      glib.putsprblk(321,232,scrollgfx);
      if (map_type&scrollable)
         glib.drawbox(321,232,335,246,15);

      glib.drawbox(127,19,144,36,0);
      if(curmeta<256)
         draw_tile16(128,20,curmeta,0x7F);
      else
         draw_appr16(128,20,curmeta-256);

      /*
      glib.putspr(122,24,(attr[curmeta*4+dir_l]&t_entry)?smallb_rarrow:small_rarrow);
      glib.putspr(132,14,(attr[curmeta*4+dir_u]&t_entry)?smallb_darrow:small_darrow);
      glib.putspr(132,38,(attr[curmeta*4+dir_d]&t_entry)?smallb_uarrow:small_uarrow);
      glib.putspr(146,24,(attr[curmeta*4+dir_r]&t_entry)?smallb_larrow:small_larrow);
      glib.putspr(117,24,(attr[curmeta*4+dir_l]&t_exit )?smallb_larrow:small_larrow);
      glib.putspr(132, 9,(attr[curmeta*4+dir_u]&t_exit )?smallb_uarrow:small_uarrow);
      glib.putspr(132,43,(attr[curmeta*4+dir_d]&t_exit )?smallb_darrow:small_darrow);
      glib.putspr(151,24,(attr[curmeta*4+dir_r]&t_exit )?smallb_rarrow:small_rarrow);
      */

      glib.putsprblk(139, 69,uarrow);
      glib.putsprblk(139,173,darrow);

      glib.putsprblk(321,200,uarrow);
      glib.putsprblk(321,441,darrow);

      glib.putsprblk(0,457,larrow);
      glib.putsprblk(305,457,rarrow);

      for (i=0;i<3;i++)
      {
         j=get_comp(currentpal,curpal*4+curclr,2-i);
         glib.putspr(264+i*20,147,small_uarrow);
         glib.drawline(267+i*20,152,267+i*20,183,8);
         glib.drawbar(262+i*20,182-j,272+i*20,184-j,12);
         glib.putspr(264+i*20,185,small_darrow);
         sprintf(tmp,"%02X",j);
         draw_string(260+i*20,190,tmp,8+(1<<(2-i)));
      }

      sprintf(tmp,"%02X",curtile);
      draw_string(174,182,tmp,15);

      sprintf(tmp,"%02X",curmeta&255);
      draw_string(2,190,tmp,15);

      x=curchip&1,y=curchip/2;
      glib.drawbox(x*33+1,y*33+1,x*33+34,y*33+34,12);

      x=curtile&15,y=curtile/16;
      glib.drawbox(x*9+174,y*9+1,x*9+183,y*9+10,12);

      x=curmeta&7,y=((curmeta&255)/8-scroll)&31;
      if (y<7)
         glib.drawbox(x*17+1,y*17+69,x*17+18,y*17+86,12);

      glib.drawbox(209+curclr*12,147,221+curclr*12,156,12);
      glib.drawbox(209,158+curpal*4,257,162+curpal*4,12);      

      i=showoverlay?0xFF:0x7F;

      for (y=0,ty=mapy;y<16;y++,ty++)
         for (x=0,tx=mapx;x<20;x++,tx++)
         {
            draw_tile16(x*16,y*16+200,map[ty*128+tx],i);
            if(showclipping)
            {
               if(attr[map[ty*128+tx] * 4]&0x10)
               {
                  glib.drawbar(x*16,y*16+200,x*16+7,y*16+207,11);
                  glib.drawbox(x*16,y*16+200,x*16+7,y*16+207,14);
               }
               if(attr[map[ty*128+tx]*4+1]&0x10)
               {
                  glib.drawbar(x*16+8,y*16+200,x*16+15,y*16+207,11);
                  glib.drawbox(x*16+8,y*16+200,x*16+15,y*16+207,14);
               }
               if(attr[map[ty*128+tx]*4+2]&0x10)
               {
                  glib.drawbar(x*16,y*16+208,x*16+7,y*16+215,11);
                  glib.drawbox(x*16,y*16+208,x*16+7,y*16+215,14);
               }
               if(attr[map[ty*128+tx]*4+3]&0x10)
               {
                  glib.drawbar(x*16+8,y*16+208,x*16+15,y*16+215,11);
                  glib.drawbox(x*16+8,y*16+208,x*16+15,y*16+215,14);
               }
               if(attr[map[ty*128+tx] * 4]&0x08)
               {
                  glib.drawbox(x*16,y*16+200,x*16+15,y*16+215,15);
                  glib.drawbox(x*16+2,y*16+202,x*16+13,y*16+213,9);
               }
            }
            if(showen||atmdmode)
            {
               glib.drawbox(x*16,y*16+200,x*16+14,y*16+214,en[ty*128+tx]);
               glib.drawbox(x*16+1,y*16+201,x*16+13,y*16+213,en[ty*128+tx]);
            }
            if(selecting==1)
            {
               if(mapclipx!=1||mapclipy!=1)
               {
                  if(tx>=cxs&&tx<cxs+mapclipx&&ty>=cys&&ty<cys+mapclipy)
                  {
                     glib.drawbox(x*16,y*16+200,x*16+14,y*16+214,4);
                  }
               }
            }
            /*
            if(showbattle)
            {
               if(attr[map[ty*128+tx]*4+1]&0x08)
               {
                  glib.drawbox(x*16+1,y*16+201,x*16+14,y*16+214,13);
                  glib.drawbox(x*16+3,y*16+203,x*16+12,y*16+212,10);
               }
            }
            */
         }

      if(!showen&&!atmdmode)
      {
         for (y=0;y<128;y++)
            for (x=0;x<128;x++)
               draw_stile16(640-256+x*2,y*2,map[y*128+x]);
      }
      else
      {
         for (y=0;y<128;y++)
            for (x=0;x<128;x++)
               draw_sappr16(640-256+x*2,y*2,en[y*128+x]);
      }

      glib.drawbox(640-256+mapx*2,       mapy*2,
                   640-256+mapx*2+20*2-1,mapy*2+16*2-1,12);

      for(i=0;i<8;i++)
      {
         glib.drawbox(640-256,  300+i*12,640-256+9,309+i*12, 0);
         draw_tile2(  640-256+1,301+i*12,an_index[i],palettetable[i]);
         glib.putspr(640-256+11,302+i*12,small_larrow);
         glib.putspr(640-256+20+8*16,302+i*12,small_rarrow);
         glib.drawbox(640-256+17,300+i*12,640-256+18+8*16,309+i*12, 0);
         for(j=0;j<an_tiles[i];j++)
            draw_tile2(640-256+18+j*8,301+i*12,an_index[i]+j,palettetable[i]);
      }

      if mouse_in(mx,my,0,200,320,456)
      {
         x =(mx)/16,y =(my-200)/16;
         tx=x+mapx ,ty=y+mapy;
         sprintf(tmp, "(%3d,%3d):%2X,%2X",tx,ty,map[ty*128+tx],en[ty*128+tx]);
         draw_string(336,200,tmp,15);
         glib.drawbox(x*16,y*16+200,x*16+15,y*16+215,12);
      }

      sprintf(tmp,"Map X size:%d",width);
      draw_string(336,260,tmp,15);

      sprintf(tmp,"Map Y size:%d",height);
      draw_string(336,268,tmp,15);

      sprintf(tmp,"Colors:%s",gbamode?"GBA":"GBC");
      draw_string(336,276,tmp,15);

      if (buttons&1)
      {
         if mouse_in(clx,cly,174,1,318,145)
         {
            x=(clx-174)/9,y=(cly-1)/9;

            for(i=0;i<8;i++)
            {
               if mouse_in(mx,my,640-256+1,301+i*12,640-256+9,309+i*12)
                  glib.drawbox(640-256,  300+i*12,640-256+9,309+i*12,14);
            }

            if mouse_in(mx,my,1,1,67,67)
            {
               tx=(mx-1)/33,ty=(my-1)/33;
               draw_big_tile(mx-16,my-16,y*16+x,palettetable[y*16+x]);
               glib.drawbox(tx*33+1,ty*33+1,tx*33+34,ty*33+34,14);
            }
            else
            {
               draw_tile(mx-4,my-4,y*16+x,palettetable[y*16+x]);
               if mouse_in(mx,my,174,1,318,145)
               {
                  tx=(mx-174)/9,ty=(my-1)/9;
                  glib.drawbox(tx*9+174,ty*9+1,tx*9+183,ty*9+10,14);
               }
            }
         }
         if mouse_in(clx,cly,1,69,137,188)
         {
//            x=(clx-1)/17,y=(cly-69)/17;
//            i=(scroll*8+y*8+x)&255;
            if(curmeta<256)
               draw_tile16(mx-8,my-8,curmeta,0x7F);
            else
               draw_appr16(mx-8,my-8,curmeta-256);

            if mouse_in(mx,my,1,69,137,188)
            {
               tx=(mx-1)/17,ty=(my-69)/17;
               glib.drawbox(tx*17+1,ty*17+69,tx*17+18,ty*17+86,14);
            }
            if mouse_in(mx,my,0,200,320,456)
            {
               tx=(mx)/16,ty=(my-200)/16;
               glib.drawbox(tx*16,ty*16+200,tx*16+15,ty*16+215,14);
            }
         }
      }

      if (showbattle)
         for(i=0;i<11;i++)
            for(j=0;j<8+(i&1);j++)
            {
               x=i*24,y=j*24+12-(i&1)*12;

               if(attr[map[((y   )/16)*128+(x   )/16]*4+1]&0x08
                ||attr[map[((y   )/16)*128+(x+15)/16]*4+1]&0x08
                ||attr[map[((y+15)/16)*128+(x   )/16]*4+1]&0x08
                ||attr[map[((y+15)/16)*128+(x+15)/16]*4+1]&0x08)
                  glib.drawbar(i*24,j*24+212-(i&1)*12,i*24+15,j*24+227-(i&1)*12,15);
               else
                  glib.drawbox(i*24,j*24+212-(i&1)*12,i*24+15,j*24+227-(i&1)*12,14);
            }

      if(curmeta<256)
      {
         draw_string(117,1,"0",(misc&attr[curmeta*4+0])?15:8);
         draw_string(125,1,"1",(misc&attr[curmeta*4+1])?15:8);
         draw_string(133,1,"2",(misc&attr[curmeta*4+2])?15:8);
         draw_string(141,1,"3",(misc&attr[curmeta*4+3])?15:8);
      }
     
      glib.putspr(mx, my, ptrgfx);

      for (j=0,y=69,num=scroll*8;j<7;j++,y+=17)
         for (i=0,x=1;i<8;i++,x+=17,num=(num+1)&255)
         {
            draw_tile16(x+1,y+1,num,0xFF);
            if(showclipping)
            {
               if(attr[num * 4]&0x10)
               {
                  glib.drawbar(x+1,y+1,x+8,y+8,11);
                  glib.drawbox(x+1,y+1,x+8,y+8,14);
               }
               if(attr[num*4+1]&0x10)
               {
                  glib.drawbar(x+9,y+1,x+16,y+8,11);
                  glib.drawbox(x+9,y+1,x+16,y+8,14);
               }
               if(attr[num*4+2]&0x10)
               {
                  glib.drawbar(x+1,y+9,x+8,y+16,11);
                  glib.drawbox(x+1,y+9,x+8,y+16,14);
               }
               if(attr[num*4+3]&0x10)
               {
                  glib.drawbar(x+9,y+9,x+16,y+16,11);
                  glib.drawbox(x+9,y+9,x+16,y+16,14);
               }
               if(attr[num * 4]&0x08)
               {
                  glib.drawbox(x+1,y+1,x+16,y+16,15);
                  glib.drawbox(x+3,y+3,x+14,y+14,13);
               }
            }
            if(showbattle)
            {
               if(attr[num*4+1]&0x08)
               {
                  glib.drawbox(x+2,y+2,x+15,y+15,9);
                  glib.drawbox(x+4,y+4,x+13,y+13,10);
               }
            }
         }

      if(curmeta<256)
         draw_big_tile16(1,1,curmeta,0xFF);

      if (key_table[K_F1])
      {
         glib.drawbar(8,8,287,327,15);
         glib.drawbox(8,8,287,327,0);
         draw_string(16, 16,"Key Help-",0);
         draw_string(16, 32," ESC    Quit",0);
         draw_string(16, 40," L      Load Tileset",0);         
         draw_string(16, 48," P      Load Map",0);         
         draw_string(16, 56," S      Save Tileset",0);
         draw_string(16, 64," W      Save Map",0);
         draw_string(16, 72," O      View overlay",0);
         draw_string(16, 80," B      View clipping",0);
         draw_string(16, 88," Z      View battle map",0);
         draw_string(16, 96," U      View battle formation map",0);
         draw_string(16,104," X      Cut",0);
         draw_string(16,112," C      Copy",0);
         draw_string(16,120," V      Paste",0);
         draw_string(16,128," F      Foreground Paste",0);
         draw_string(16,136," Q/A    Scroll Metatiles up/down",0);
         draw_string(16,144," Arrows Scroll Map",0);
         draw_string(16,152," E      Export PCX of map",0);
         draw_string(16,160," I      Export enemy map",0);
         draw_string(16,168," T      Export map",0);
         draw_string(16,176," G      Append tile to EXPORT1",0);
         draw_string(16,184," H      Export tile to EXPORT1",0);
         draw_string(16,192," Y      Import tiles from EXPORT1",0);
         draw_string(16,200," N      Toggle clipping",0);
         draw_string(16,208," M      Toggle overlay",0);
         draw_string(16,216," D      Cut metatile",0);
         draw_string(16,224," R      Paste metatile",0);
         draw_string(16,232," J      Toggle GBA color mode",0);
         draw_string(16,240," 5      Enter Map mode",0);
         draw_string(16,248," 6      Enter Battle Map mode",0);
         draw_string(16,256," F2     Move map",0);
         draw_string(16,264," F3     Snap map to topleft",0);
         draw_string(16,272," F5     Left",0);
         draw_string(16,280," F6     Right",0);
         draw_string(16,288," F7     Up",0);
         draw_string(16,296," F8     Down",0);
         draw_string(16,304," F9     HMirror",0);
         draw_string(16,312," F10    VMirror",0);
      }

      if(quit)
      {
         if (notsaved_tiles||notsaved_map)
         {
            glib.drawbar(32,84,247,115,15);
            glib.drawbox(32,84,247,115,0);
            draw_string(40, 92,"To exit without saving",0);
            draw_string(40,100,"changes, press 1 and ESC.",0);
            if (quit&2)
               break;
         }
         else
            break;
      }

//=============================================================
//      glib.vrwait();
//      glib.copypage(vga);
//=============================================================

      i=updatecounter++&7;
      if(an_tiles[i])
         an_counter[i]=(an_counter[i]+1)%an_tiles[i];

//-------------------------------------------------------------
	  do_sdl_update_screen( screen, vscreen );
//-------------------------------------------------------------
   }

//---------------------------------------------------------------------
   SDL_Quit();
//---------------------------------------------------------------------

//=====================================================================
//   deinit_kh();
//   vbeClose(m640x480);
//
//   glib.setmode(0x03);
//=====================================================================

   return 0;
}
