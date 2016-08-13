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

;----------------------------------------------------------------------------
; MGS
;----------------------------------------------------------------------------

   .area _BSS

   .area _CODE

;----------------------------------------------------------------------------
;[[ ubyte mgs_getpal(ubyte slot) ]]
;----------------------------------------------------------------------------
_mgs_getpal::
   LDHL  SP, #2

   LD    A, (HL)                       ; A = slot

   ADD   A, A                          ; HL = C003 + slot*8 -> attributes
   ADD   A, A                          ;
   ADD   A, A                          ;
   ADD   A, #3                         ;
   LD    H, #0xC0                      ;
   LD    L, A                          ;

   LD    A, (HL)                       ;
   AND   #0x07                         ;
   LD    E, A                          ;

   RET                                 ;

;----------------------------------------------------------------------------
;[[ void mgs_shift(ubyte zone, ubyte dist, ubyte y) ]]
;----------------------------------------------------------------------------
_mgs_shift::
   LDHL  SP, #4

   PUSH  BC

   LD    A, (HLD)                      ; E = y
   ADD   A, #0x10                      ;
   LD    E, A                          ;

   LD    A, (HLD)                      ; D = dist
   LD    D, A                          ;

   LD    L, (HL)                       ; A = slot

   ADD   HL, HL                        ; HL = C001 + zone*8*4 -> x coordinate
   ADD   HL, HL
   ADD   HL, HL
   LD    H, #0x30
   ADD   HL, HL
   ADD   HL, HL
   INC   L

   LD    BC, #0x08F0
mgs_shift_loop:
   LD    A, (HL)
   SUB   D
   LD    (HLD), A
   AND   C
   CP    #0b10110000
   JR    NZ, mgs_keeprolling
   LD    (HL), E
mgs_keeprolling:
   INC   L
   INC   L
   INC   L
   INC   L
   INC   L
   DEC   B
   JR    NZ, mgs_shift_loop

   POP   BC

   RET
