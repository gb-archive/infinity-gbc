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

/*
   MOUSE.C

   Defines routines for reading the state of the mouse and
   sensing if a mouse driver is installed.
*/

//#include <dos.h> //interrupts...

byte ptrgfx[] = {
    8, 0,16, 0,

   20,20, 0, 0, 0, 0, 0, 0,
   20,31,20, 0, 0, 0, 0, 0,
   20,31,31,20, 0, 0, 0, 0,
   20,29,30,31,20, 0, 0, 0,
   20,28,29,30,31,20, 0, 0,
   20,27,28,29,30,31,20, 0,
   20,27,28,29,20,20,20,20,
   20,27,20,28,20, 0, 0, 0,
   20,20, 0,20,28,20, 0, 0,
   20, 0, 0,20,28,20, 0, 0,
    0, 0, 0, 0,20,28,20, 0,
    0, 0, 0, 0,20,20,20, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
};

struct MOUSE_STATE {
	int x;
	int y;
	int buttons;
};

MOUSE_STATE gMouseState;
int initted;
int mouse_x, mouse_y;

/*
void mouse_move (int mx, int my)
{
   union REGS regs;

   if (!initted)
      return;

   regs.w.ax = 4;
   regs.w.cx = mx * 2;
   regs.w.dx = my;
   int386 (0x33, &regs, &regs);
}

void mouse_region (int x1, int y1, int x2, int y2)
{
   union REGS regs;

   if (!initted)
      return;

   regs.w.ax = 7;
   regs.w.cx = x1 * 2;
   regs.w.dx = x2 * 2;
   int386 (0x33, &regs, &regs);

   regs.w.ax = 8;
   regs.w.cx = y1;
   regs.w.dx = y2;
   int386 (0x33, &regs, &regs);
}
*/

int mouse_init ()
{
	gMouseState.x = SCREEN_WIDTH / 2;
	gMouseState.y = SCREEN_HEIGHT / 2;

	return 1;

//   union REGS regs;
//
//   regs.w.ax = 0;
//   int386 (0x33, &regs, &regs);
//
//   if (initted = (regs.h.al) ? 1 : 0)
//   {
//      regs.w.ax = 11;
//      int386 (0x33, &regs, &regs);

//      mouse_x=320,mouse_y=200;
////      mouse_region(0, 0, 639, 479);
////      mouse_move (160, 100);
//   }
//
//   return initted;
}

int mouse_status (int *mx, int *my)
{
	*mx = gMouseState.x;
	*my = gMouseState.y;
	return gMouseState.buttons;

//   short x, y;
//
//   union REGS regs;
//
//   if (!initted)
//      return -1;
//
//   regs.w.ax = 11;
//   int386 (0x33, &regs, &regs);
//
//   x = regs.w.cx;
//   y = regs.w.dx;
//
//   mouse_x+=x,mouse_y+=y;
//   if (mouse_x < 0) mouse_x = 0;
//   if (mouse_y < 0) mouse_y = 0;
//   if (mouse_x > 639) mouse_x = 639;
//   if (mouse_y > 479) mouse_y = 479;
//
//   *mx=mouse_x,*my=mouse_y;
//
//   regs.w.ax = 3;
//   int386 (0x33, &regs, &regs);
//
//   return regs.w.bx;
//	return 0;
}

/*void
set_sense(int x, int y, int double_speed)
{
        __dpmi_regs regs;

        regs.x.ax = 0x001a;
        regs.x.bx = x;
        regs.x.cx = y;
        regs.x.dx = double_speed;
        __dpmi_int(0x33, &regs);
}

*/
