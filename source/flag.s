; Copyright (C) 1999-2001 Affinix Software, LLC
;
; This file is part of Infinity.
;
; This file may be used under the terms of the Creative Commons Attribution-
; NonCommercial-ShareAlike 4.0 International License as published by Creative
; Commons.
;
; Alteratively, this file may be used under the terms of the GNU General
; Public License as published by the Free Software Foundation, either version
; 3 of the License, or (at your option) any later version.
;
; In addition, as a special exception, Affinix Software gives you certain
; additional rights. These rights are described in the LICENSE file in this
; package.

	.area _BSS

_flags::
   .ds   32

   .area _CODE

bits:
   .DB   0x80,0x40,0x20,0x10,0x08,0x04,0x02,0x01

getflagptr:
   LDHL  SP, #4
   LD    A, (HL)
   AND   #7
   LD    E, A
   LD    D, #0
   XOR   (HL)
   LD    HL, #bits
   ADD   HL, DE
   RRCA
   RRCA
   RRCA
   LD    E, A
   LD    A, (HL)
   LD    HL, #_flags
   ADD   HL, DE
   RET

;----------------------------------------------------------------------------
;[ flag_set(ubyte flagnum) ]
;----------------------------------------------------------------------------
_flag_set::
   CALL  getflagptr
   OR    (HL)
   LD    (HL), A
   RET

;----------------------------------------------------------------------------
;[ flg_get(ubyte flagnum) ]
;----------------------------------------------------------------------------
_flag_get::
   CALL  getflagptr
   AND   (HL)
   LD    E, A
   RET
