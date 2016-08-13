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

// GraPhat Lib

//---------------------------------
#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480
//---------------------------------

typedef unsigned char      byte;
typedef unsigned int       uint;
typedef unsigned short int word;
typedef unsigned int       dword;

typedef char           s08;
typedef unsigned char  u08;
typedef short          s16;
typedef unsigned short u16;
typedef int            s32;
typedef unsigned int   u32;

#define nmin(x, y) (((x) < (y)) ? (x) : (y))
#define nmax(x, y) (((x) > (y)) ? (x) : (y))

#define clr8(r, g, b) ((b) * 36 + (g) * 6 + (r) + 40)

#define abs(x) (((x) < 0) ? -(x) : (x))

class graphlib
{
   public:

      graphlib();
      ~graphlib();

      void vrwait();
      void setdest(byte* dest);
      void setmode(int mode);
      void setpal();

      int  getpixel(int x, int y);
      void putpixel(int x, int y, int clr);
      void addpixel(int x, int y, int clr);
      void subpixel(int x, int y, int clr);
      void avgpixel(int x, int y, int clr);

      void fillpage(int clr);
      void copypage(byte* target);
      int  checkclip(int x, int y);
      void setrgb(int clr, int rgb);
      int  imagesize(int x1, int y1, int x2, int y2);

      void drawstring(int x, int y, byte* message);
      void drawlineh(int x1, int y, int x2, byte clr);
      void drawlinev(int x, int y1, int y2, byte clr);
      void drawbox(int x1, int y1, int x2, int y2, byte clr);
      void drawbar(int x1, int y1, int x2, int y2, byte clr);
      void getimage(int x1, int y1, int x2, int y2, byte *image);
      void drawline(int x1, int y1, int x2, int y2, byte clr);
      void set4Pixels(int x, int y, int xc, int yc, byte clr);
      void ellipse(int xc, int yc, int wide, int deep);
      void circle(int x, int y, int radius);

      void putimg(int x, int y, byte* image);
      void putimgblk(int x, int y, byte* image);
      void putimgchr(int x, int y, byte* image);
      void putspr(int x, int y, byte* spr);
      void putsprblk(int x, int y, byte* image);
      void putsprchr(int x, int y, byte* image);
      void addspr(int x, int y, byte* spr);
      void addsprblk(int x, int y, byte* image);
      void addsprchr(int x, int y, byte* image);
      void subspr(int x, int y, byte* spr);
      void subsprblk(int x, int y, byte* image);
      void subsprchr(int x, int y, byte* image);
      void avgspr(int x, int y, byte* spr);
      void avgsprblk(int x, int y, byte* image);
      void avgsprchr(int x, int y, byte* image);

   private:

      byte* defscreen;
      int defclr, defx, defy;

};

byte* vscreen;
byte* bscreen;
byte* transadd;
byte* transsub;
byte* transavg;
byte* comp_r;
byte* comp_g;
byte* comp_b;
byte* vga;

//--------------------------------
dword* g_palette;
//--------------------------------

// Inline ASM declarations
void _setmode(int mode);
void _putpixel(byte* screen, int x, int y, int clr);
void _subpixel(byte* screen, int x, int y, int clr);
void _addpixel(byte* screen, int x, int y, int clr);
void _avgpixel(byte* screen, int x, int y, int clr);
void _vrwait();
void _fillpage(byte* screen, int clr);
void _copypage(byte* screen, byte* source);
void _setrgb(int clr, int rgb);
int _getpixel(byte* screen, int x, int y);

int memlimit = 0;

/*
 * Constructor
 *
 * Allocates virtual screen, sets mode 13h.
 */
graphlib::graphlib()
{
   if (comp_r = new byte [256])
      memlimit++;
   if (comp_g = new byte [256])
      memlimit++;
   if (comp_b = new byte [256])
      memlimit++;
   if (vscreen = new byte [307200])
      memlimit++;
   if (bscreen = new byte [307200])
      memlimit++;
   if (transadd = new byte [65536])
      memlimit++;
   if (transsub = new byte [65536])
      memlimit++;
   if (transavg = new byte [65536])
      memlimit++;

//======================================
//   vga = (byte*) 0xA0000;
//======================================
//--------------------------------------
   if (vga = new byte [640*480])
      memlimit++;
   if (g_palette = new dword [256])
      memlimit++;

	int colors[16] = {
		0x000000, 0x000030, 0x003000, 0x003030,
		0x300000, 0x300030, 0x303000, 0x303030,
		0x202020, 0x30203f, 0x203f20, 0x203f3f,
		0x3f2030, 0x3f203f, 0x2f3f20, 0x3f3f3f,
	};
					
	int c;
	for (c = 0; c < 16; c++)
		setrgb(c, colors[c]);
	for (c = 16; c < 32; c++)
		setrgb(c, (c - 16) * 0x040404);

//--------------------------------------
   defscreen = vscreen;
   fillpage(0);
   defscreen = bscreen;
   fillpage(0);
   defscreen = vga;
}

/*
 * Destructor
 *
 * Deallocates virtual screen, sets text mode.
 */
graphlib::~graphlib()
{
   if (memlimit-- > 0)
      delete [] comp_r;
   if (memlimit-- > 0)
      delete [] comp_g;
   if (memlimit-- > 0)
      delete [] comp_b;
   if (memlimit-- > 0)
      delete [] vscreen;
   if (memlimit-- > 0)
      delete [] bscreen;
   if (memlimit-- > 0)
      delete [] transadd;
   if (memlimit-- > 0)
      delete [] transsub;
   if (memlimit-- > 0)
      delete [] transavg;
//--------------------------------------
   if (memlimit-- > 0)
	   delete [] vga;
   if (memlimit-- > 0)
	   delete [] g_palette;
//--------------------------------------
}

/*
 * Sets screen mode using BIOS int 10h.
 */
void graphlib::setmode(int mode)
{
//   #pragma aux _setmode = \
//      "   pushad" \
//      "   int 10h" \
//      "   popad" \
//      parm [eax]
//
//   _setmode(mode);
}

/*
 * Sets virtual color cube.
 */
void graphlib::setpal()
{
   int r, g, b, c;
   int r2, g2, b2, c2;

   setrgb(0, 0x000000);

   byte rs[] = {0, 15, 27, 39, 51, 63};
   byte gs[] = {0, 17, 29, 40, 52, 63};
   byte bs[] = {0, 23, 34, 44, 54, 63};

   for (c = 0; c < 256; c++)
   {
      comp_r[c] = comp_g[c] = comp_b[c] = 0;
      for (c2 = 0; c2 < 256; c2++)
         transadd[c * 256 + c2] = transsub[c * 256 + c2] =
            transavg[c * 256 + c2] = c2;
   }

   for (c = 40, b = 0; b < 6; b++)
      for (g = 0; g < 6; g++)
         for (r = 0; r < 6; r++, c++)
         {
            setrgb(c, rs[r] * 0x10000 + gs[g] * 0x100 + bs[b]);

            comp_r[c] = r, comp_g[c] = g, comp_b[c] = b;

            for(c2 = 40, b2 = 0; b2 < 6; b2++)
               for(g2 = 0; g2 < 6; g2++)
                  for(r2 = 0; r2 < 6; r2++, c2++)
                  {
                     transadd[c * 256 + c2] = clr8(nmin(r + r2, 5),
                                                   nmin(g + g2, 5),
                                                   nmin(b + b2, 5));
                     transsub[c * 256 + c2] = clr8(nmax(r - r2, 0),
                                                   nmax(g - g2, 0),
                                                   nmax(b - b2, 0));
                     transavg[c * 256 + c2] = clr8(nmin((r + r2 + 1) / 2, 5),
                                                   nmin((g + g2 + 1) / 2, 5),
                                                   nmin((b + b2 + 1) / 2, 5));
                  }
         }
}

/*
 * Gets the color value of a pixel.
 */
int graphlib::getpixel(int x, int y)
{
	return vscreen[y * SCREEN_WIDTH + x];

//   #pragma aux _getpixel = \
//      "   xor ecx, ecx" \
//      "   cmp ebx, 480" \
//      "   lea ebx, [ebx + ebx * 4]" \
//      "   jae done" \
//      "   shl ebx, 7" \
//      "   add ebx, edx" \
//      "   cmp edx, 640" \
//      "   jae done" \
//      "   mov cl, [eax + ebx]" \
//      "done:" \
//      parm [eax][edx][ebx] \
//      modify [ebx ecx] \
//      value [ecx]
//
//   return _getpixel(vscreen, x, y);
}

/*
 * Draws a pixel manually with clipping
 */
void graphlib::putpixel(int x, int y, int clr)
{
	defscreen[y * SCREEN_WIDTH + x] = clr;

//   #pragma aux _putpixel = \
//      "   cmp ebx, 480" \
//      "   lea ebx, [ebx + ebx * 4]" \
//      "   jae done" \
//      "   shl ebx, 7" \
//      "   add ebx, edx" \
//      "   cmp edx, 640" \
//      "   jae done" \
//      "   mov [eax + ebx], cl" \
//      "done:" \
//      parm [eax][edx][ebx][ecx] \
//      modify [ebx edx]
//
//   _putpixel(defscreen, x, y, clr);
}

/*
 * Adds a pixel.
 */
void graphlib::addpixel(int x, int y, int clr)
{
	//unused

//   #pragma aux _addpixel = \
//      "   cmp edx, 320" \
//      "   jae done" \
//      "   cmp ebx, 200" \
//      "   jae done" \
//      "   add dh, bl" \
//      "   shl ebx, 6" \
//      "   and ecx, 0xFFFF" \
//      "   add edx, ebx" \
//      "   mov ebx, transadd" \
//      "   mov ch, [eax + edx]" \
//      "   mov cl, byte ptr [ebx + ecx]" \
//      "   mov [eax + edx], cl" \
//      "done:" \
//      parm [eax][edx][ebx][ecx] \
//      modify [ebx ecx edx]
//
//   _addpixel(defscreen, x, y, clr);
}

/*
 * Subtracts a pixel.
 */
void graphlib::subpixel(int x, int y, int clr)
{
	//unused

//   #pragma aux _subpixel = \
//      "   cmp edx, 320" \
//      "   jae done" \
//      "   cmp ebx, 200" \
//      "   jae done" \
//      "   add dh, bl" \
//      "   shl ebx, 6" \
//      "   and ecx, 0xFFFF" \
//      "   add edx, ebx" \
//      "   mov ebx, transsub" \
//      "   mov ch, [eax + edx]" \
//      "   mov cl, byte ptr [ebx + ecx]" \
//      "   mov [eax + edx], cl" \
//      "done:" \
//      parm [eax][edx][ebx][ecx] \
//      modify [ebx ecx edx]
//
//   _subpixel(defscreen, x, y, clr);
}

/*
 * Averages a pixel.
 */
void graphlib::avgpixel(int x, int y, int clr)
{
	//unused

//   #pragma aux _avgpixel = \
//      "   cmp edx, 320" \
//      "   jae done" \
//      "   cmp ebx, 200" \
//      "   jae done" \
//      "   add dh, bl" \
//      "   shl ebx, 6" \
//      "   and ecx, 0xFFFF" \
//      "   add edx, ebx" \
//      "   mov ebx, transavg" \
//      "   mov ch, [eax + edx]" \
//      "   mov cl, byte ptr [ebx + ecx]" \
//      "   mov [eax + edx], cl" \
//      "done:" \
//      parm [eax][edx][ebx][ecx] \
//      modify [ebx ecx edx]
//
//   _avgpixel(defscreen, x, y, clr);
}

/*
 * Sets default destination in glib functions
 */
void graphlib::setdest(byte* dest)
{
   defscreen = dest;
}

/*
 * Waits for vertical retrace.
 */
void graphlib::vrwait()
{
//   #pragma aux _vrwait = \
//      "   mov dx, 0x03da" \
//      "vtr:" \
//      "   in  al, dx" \
//      "   test al, 0x08" \
//      "   jnz vtr" \
//      "ste:" \
//      "   in  al, dx" \
//      "   test al, 0x08" \
//      "   jz ste" \
//      modify [eax edx]
//
//  _vrwait();
}

/*
 * Fills default page with clr.
 */
void graphlib::fillpage(int clr)
{
	int i;
	byte *ptr = defscreen;
	for(i = 0; i < SCREEN_HEIGHT * SCREEN_WIDTH; i++)
		*ptr++ = clr;

//   #pragma aux _fillpage = \
//      "   mov dh, dl" \
//      "   push dx" \
//      "   shl edx, 16" \
//      "   mov ecx, 19200" \
//      "   pop dx" \
//      "fillloop:" \
//      "   mov [eax], edx" \
//      "   mov [eax + 4], edx" \
//      "   mov [eax + 8], edx" \
//      "   mov [eax + 12], edx" \
//      "   add eax, 16" \
//      "   dec ecx" \
//      "   jne fillloop" \
//      parm [eax][edx] \
//      modify [eax ecx edx]
//
//   _fillpage(defscreen, clr);
}

/*
 * Copies default page to target page.
 */
void graphlib::copypage(byte* target)
{
	int i;
	byte *ptr = defscreen;
	for(i = 0; i < SCREEN_HEIGHT * SCREEN_WIDTH; i++)
		*target++ = *ptr++;

//   #pragma aux _copypage = \
//      "   mov ecx, 19200" \
//      "copyloop:" \
//      "   mov edi, [edx]" \
//      "   mov ebx, [edx + 4]" \
//      "   mov [eax], edi" \
//      "   mov [eax + 4], ebx" \
//      "   mov edi, [edx + 8]" \
//      "   mov ebx, [edx + 12]" \
//      "   mov [eax + 8], edi" \
//      "   mov [eax + 12], ebx" \
//      "   add eax, 16" \
//      "   add edx, 16" \
//      "   dec ecx" \
//      "   jne copyloop" \
//      parm [eax][edx] \
//      modify [eax ebx ecx edx edi]
//
//   _copypage(target, defscreen);
}

/*
 * Checks if a coordinate is out of range.
 */
int graphlib::checkclip(int x, int y)
{
   if (x < 0 || x >= SCREEN_WIDTH) return 0;
   if (y < 0 || y >= SCREEN_HEIGHT) return 0;
   return 1;
}

/*
 * Sets palette values for clr, in 0xRRGGBB format.
 */
void graphlib::setrgb(int clr, int rgb)
{
	rgb = (rgb << 2) & 0xFCFCFC;

	g_palette[clr] = rgb;

//	int r, g, b;
//	r = (rgb >> 16) & 0xFF;
//	g = (rgb >> 8) & 0xFF;
//	b = (rgb) & 0xFF;

//	int r5, g5, b5;
//	r5 = r >> 3;
//	g5 = g >> 3;
//	b5 = b >> 3;

//	int rgb555;
//	rgb555 = (r5 << 10) | (g5 << 5) | b5;

//	g_palette[clr] = rgb555;

//   #pragma aux _setrgb = \
//      "   mov dx, 0x03C8" \
//      "   out dx, al" \
//      "   inc edx" \
//      "   mov eax, ebx" \
//      "   shr eax, 16" \
//      "   out dx, al" \
//      "   mov al, bh" \
//      "   out dx, al" \
//      "   mov al, bl" \
//      "   out dx, al" \
//      parm[eax][ebx] \
//      modify [eax edx]
//
//   _setrgb(clr, rgb);
}

/*
 * Returns the amount of memory that should be allocated
 * for an image. (use the same coordinates)
 */
int graphlib::imagesize(int x1, int y1, int x2, int y2)
{
   return (y2 - y1 + 1) * (x2 - x1 + 1) + 4;
}

void graphlib::drawline(int x1, int y1, int x2, int y2, byte clr)
{
   register int t, distance;
   int xerr, yerr, deltax, deltay;
   int incx, incy;

   deltax = x2 - x1, deltay = y2 - y1;  /* compute both distances */

   if (deltax > 0)       /* compute increments */
      incx = 1;
   else if (deltax == 0)
      incx = 0;
   else
      incx = -1;

   if (deltay > 0)
      incy = 1;
   else if (deltay == 0)
      incy = 0;
   else
      incy = -1;

   deltax = abs(deltax), deltay = abs(deltay); /* determine greater distance */
   if (deltax > deltay)
      distance = deltax;
   else
      distance = deltay;

   xerr = deltax, yerr = deltay;

   for (t = 0; t <= distance; t++)     /* draw the line */
   {
      putpixel(x1, y1, clr);

      xerr += deltax, yerr += deltay;
      if (xerr > distance)
         xerr -= distance, x1 += incx;
      if (yerr > distance)
         yerr -= distance, y1 += incy;
   }
}

void graphlib::ellipse(int xc, int yc, int wide, int deep)
{
  int x, y;
  long a, b;
  long Asquared, TwoAsquared;
  long Bsquared, TwoBsquared;
  long d, dx, dy;

  wide /= 2, deep /= 2;

  x = 0;				/* initialize variables */
  y = deep;
  a = wide;
  b = deep;
  Asquared = a * a;
  TwoAsquared = 2 * Asquared;
  Bsquared = b * b;
  TwoBsquared = 2 * Bsquared;

  d = Bsquared - Asquared * b + Asquared/4L;
  dx = 0;
  dy = TwoAsquared * b;

  while (dx < dy)
  {
    set4Pixels(x, y, xc, yc, defclr);

    if (d > 0L)
    {
      y--;
      dy -= TwoAsquared;
      d -= dy;
    }

    x++;
    dx += TwoBsquared;
    d += Bsquared + dx;
  }

  d += (3L*(Asquared-Bsquared)/2L - (dx+dy)) / 2L;

  while (y >= 0)
  {
    set4Pixels(x, y, xc, yc, defclr);

    if (d < 0L)
    {
      x++;
      dx += TwoBsquared;
      d += dx;
    }

    y--;
    dy -= TwoAsquared;
    d += Asquared - dy;
  }
}

void graphlib::set4Pixels(int x, int y, int xc, int yc, byte clr)
{
   register int xcoord, ycoord;

   xcoord = xc + x;
   ycoord = yc + y;
   putpixel(xcoord, ycoord, clr);

   xcoord = xc - x;
   putpixel(xcoord, ycoord, clr);

   xcoord = xc + x;
   ycoord = yc - y;
   putpixel(xcoord, ycoord, clr);

   xcoord = xc - x;
   putpixel(xcoord, ycoord, clr);
}

void graphlib::circle(int x, int y, int radius)
{
   ellipse(x, y, 2*radius, (radius*40)/24);
}

void graphlib::getimage(int x1, int y1, int x2, int y2, byte *image)
{
   int x, y, xsize, ysize;
   int temp;
   byte* p = image + 4;

   if (x2 < x1)        /* swap coordinates if necessary */
   {
      temp = x1;
      x2 = x1;
      x2 = temp;
   }
   if (y2 < y1)
   {
      temp = y1;
      y2 = y1;
      y2 = temp;
   }

   if (x1 < 0)    /* ensure coords are in bounds */
      x1 = 0;
   else if (x1 > 319)
      x1 = 319;

   if (y1 < 0)
      y1 = 0;
   else if (y1 > 199)
      y1 = 199;

   if (x2 < 0)
      x2 = 0;
   else if (x2 > 319)
      x2 = 319;

   if (y2 < 0)
      y2 = 0;
   else if (y2 > 199)
      y2 = 199;

   xsize = x2 - x1 + 1;
   ysize = y2 - y1 + 1;

   ((short*) image)[0] = xsize;
   ((short*) image)[1] = ysize;

   if (xsize && ysize)
      for (y = 0; y < ysize; y++)
         for (x = 0; x < xsize; x++)
            *p++ = getpixel(x + x1, y + y1);
}

void graphlib::drawlineh(int x1, int y, int x2, byte clr)
{
   int x, xm;

   if (y < 0 || y > 479)
      return;

   if (x1 > x2)
   {
      if (x2 > 639)
         return;
      x = x2, xm = x1;
   }
   else
   {
      if (x2 < 0)
         return;
      x = x1, xm = x2;
   }

   x = nmax(0, x), xm = nmin(639, xm);

   for (; x <= xm; x++)
      putpixel(x, y, clr);
}

void graphlib::drawlinev(int x, int y1, int y2, byte clr)
{
   int y, ym;

   if (x < 0 || x > 639)
      return;

   if (y1 > y2)
   {
      if (y2 > 479)
         return;
      y = y2, ym = y1;
   }
   else
   {
      if (y2 < 0)
         return;
      y = y1, ym = y2;
   }

   y = nmax(0, y), ym = nmin(479, ym);

   for (; y <= ym; y++)
      putpixel(x, y, clr);
}

void graphlib::drawbox(int x1, int y1, int x2, int y2, byte clr)
{
   drawlineh(x1, y1, x2, clr);
   drawlineh(x1, y2, x2, clr);
   drawlinev(x1, y1, y2, clr);
   drawlinev(x2, y1, y2, clr);
}

void graphlib::drawbar(int x1, int y1, int x2, int y2, byte clr)
{
   int x, xm, y, ym, xsize;
         
   if (x1 > x2)
   {
      if (x2 > 639)
         return;
      x = x2, xm = x1;
   }
   else
   {
      if (x2 < 0)
         return;
      x = x1, xm = x2;
   }

   if (y1 > y2)
   {
      if (y2 > 479)
         return;
      y = y2, ym = y1;
   }
   else
   {
      if (y2 < 0)
         return;
      y = y1, ym = y2;
   }

   x = nmax(0, x), xm = nmin(639, xm);
   y = nmax(0, y), ym = nmin(479, ym);

   xsize = xm - x + 1;

   for (; y <= ym; y++, x -= xsize)
      for (; x <= xm; x++)
         putpixel(x, y, clr);
}

//#include "vgovbe20.h"

extern "C" {
//int inp(unsigned short) { return 0; }
//int outp(unsigned short, int) { return 0; }
//void vbeClose(VBESURFACE *) {}
//int vbeSetScanWidth(long) { return 0; }
//VBESURFACE *vbeOpen(long,long,long) { return 0; }
//VBEINFO *vbeDetect(void) { return 0; }
//int deinit_kh(void) { return 0; }
byte key_table[256];
//int init_kh(void) { return 0; }
}

typedef void (*DrawLineFunc)( byte *, byte *, int );

void drawspriteline( byte *dst, byte *src, int num )
{
	int i, j;
	for( i = 0; i < num; i++ ) {
		if( j = *src )
			*dst = *src;
		dst++;
		src++;
	}
}

void drawimageline( byte *dst, byte *src, int num )
{
	int i;
	for( i = 0; i < num; i++ ) {
		*dst++ = *src++;
	}
}

void putimage( DrawLineFunc pfDrawLine, byte *surf, int dpitch, int dx, int dy, byte *image, int spitch, int sx, int sy, int sw, int sh )
{
	int y;

	if( dx < 0 ) {
		sx -= dx;
		sw += dx;
		dx = 0;
	}
	if( dx + sw > SCREEN_WIDTH ) {
		sw = SCREEN_WIDTH - dx;
	}
	if( dy < 0 ) {
		sy -= dy;
		sh += dy;
		dy = 0;
	}
	if( dy + sh > SCREEN_HEIGHT ) {
		sh = SCREEN_HEIGHT - dy;
	}

	for( y = 0; y < sh; y++ ) {
		byte *src = &image[ spitch * (sy + y) + sx ];
		byte *dst = &surf[ dpitch * (dy + y) + dx ];
		pfDrawLine( dst, src, sw );
	}
}

void graphlib::putimg(int x, int y, byte* image)
{
	int xsize, ysize;
	xsize = ((short*) image)[0];
	ysize = ((short*) image)[1];
	byte* p = image + 4;

	putimage( drawimageline, defscreen, SCREEN_WIDTH, x, y, p, xsize, 0, 0, xsize, ysize );
}
void graphlib::putimgblk(int x, int y, byte* image)
{
	int xsize, ysize;
	xsize = 16;
	ysize = 16;
	byte* p = image;

	putimage( drawimageline, defscreen, SCREEN_WIDTH, x, y, p, xsize, 0, 0, xsize, ysize );
}
void graphlib::putimgchr(int x, int y, byte* image)
{
	int xsize, ysize;
	xsize = 8;
	ysize = 8;
	byte* p = image;

	putimage( drawimageline, defscreen, SCREEN_WIDTH, x, y, p, xsize, 0, 0, xsize, ysize );
}
void graphlib::putspr(int x, int y, byte* image)
{
	int xsize, ysize;
	xsize = ((short*) image)[0];
	ysize = ((short*) image)[1];
	byte* p = image + 4;

	putimage( drawspriteline, defscreen, SCREEN_WIDTH, x, y, p, xsize, 0, 0, xsize, ysize );
}
void graphlib::putsprblk(int x, int y, byte* image)
{
	int xsize, ysize;
	xsize = 16;
	ysize = 16;
	byte* p = image;

	putimage( drawspriteline, defscreen, SCREEN_WIDTH, x, y, p, xsize, 0, 0, xsize, ysize );
}
void graphlib::putsprchr(int x, int y, byte* image)
{
	int xsize, ysize;
	xsize = 8;
	ysize = 8;
	byte* p = image;

	putimage( drawspriteline, defscreen, SCREEN_WIDTH, x, y, p, xsize, 0, 0, xsize, ysize );
}
void graphlib::addspr(int x, int y, byte* spr) {}
void graphlib::addsprblk(int x, int y, byte* image) {}
void graphlib::addsprchr(int x, int y, byte* image) {}
void graphlib::subspr(int x, int y, byte* spr) {}
void graphlib::subsprblk(int x, int y, byte* image) {}
void graphlib::subsprchr(int x, int y, byte* image) {}
void graphlib::avgspr(int x, int y, byte* spr) {}
void graphlib::avgsprblk(int x, int y, byte* image) {}
void graphlib::avgsprchr(int x, int y, byte* image) {}

// void graphlib::putimage(int x, int y, byte* image)
// {
//    inside of blit.asm
// }

// void graphlib::putspr(int x, int y, byte* spr)
// {
//    inside of blit.asm
// }
