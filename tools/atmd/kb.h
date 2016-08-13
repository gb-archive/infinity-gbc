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

//---------------------------------------------------------------------------
// Keyboard Handler for Watcom C/C++                            by CrASH_Man
//---------------------------------------------------------------------------
// For use with KB.ASM
//---------------------------------------------------------------------------

#ifndef _KEYBOARD_H
#define _KEYBOARD_H

#ifdef __cplusplus
extern "C" {
#endif

#define K_LAST   0x00
#define K_ESC    0x01
#define K_1      0x02
#define K_2      0x03
#define K_3      0x04
#define K_4      0x05
#define K_5      0x06
#define K_6      0x07
#define K_7      0x08
#define K_8      0x09
#define K_9      0x0A
#define K_0      0x0B
#define K_DASH   0x0C
#define K_EQUAL  0x0D
#define K_BACK   0x0E
#define K_TAB    0x0F
#define K_Q      0x10
#define K_W      0x11
#define K_E      0x12
#define K_R      0x13
#define K_T      0x14
#define K_Y      0x15
#define K_U      0x16
#define K_I      0x17
#define K_O      0x18
#define K_P      0x19
#define K_LBRACK 0x1A
#define K_RBRACK 0x1B
#define K_ENTER  0x1C
#define K_CTRL   0x1D
#define K_A      0x1E
#define K_S      0x1F
#define K_D      0x20
#define K_F      0x21
#define K_G      0x22
#define K_H      0x23
#define K_J      0x24
#define K_K      0x25
#define K_L      0x26
#define K_SCOLON 0x27
#define K_RQUOTE 0x28
#define K_LQUOTE 0x29
#define K_LSHIFT 0x2A
#define K_BKSLSH 0x2B
#define K_Z      0x2C
#define K_X      0x2D
#define K_C      0x2E
#define K_V      0x2F
#define K_B      0x30
#define K_N      0x31
#define K_M      0x32
#define K_COMMA  0x33
#define K_PERIOD 0x34
#define K_SLASH  0x35
#define K_RSHIFT 0x36
#define K_PRTSCR 0x37
#define K_ALT    0x38
#define K_SPACE  0x39
#define K_CAPS   0x3A
#define K_F1     0x3B
#define K_F2     0x3C
#define K_F3     0x3D
#define K_F4     0x3E
#define K_F5     0x3F
#define K_F6     0x40
#define K_F7     0x41
#define K_F8     0x42
#define K_F9     0x43
#define K_F10    0x44
#define K_NUML   0x45
#define K_SCRL   0x46
#define K_HOME   0x47
#define K_UP     0x48
#define K_PGUP   0x49
#define K_MINUS  0x4A
#define K_LEFT   0x4B
#define K_CENTER 0x4C
#define K_RIGHT  0x4D
#define K_PLUS   0x4E
#define K_END    0x4F
#define K_DOWN   0x50
#define K_PGDN   0x51
#define K_INS    0x52
#define K_DEL    0x53
#define K_PRESS  0xFF

extern int init_kh(void);
extern int deinit_kh(void);
extern byte key_table[256];

int inline key_stat(int num)
{
   if (key_table[K_LAST] == num)
   {
      key_table[K_LAST] = 0;
      return 1;
   }

   return 0;
}

#ifdef __cplusplus
};
#endif

#endif
