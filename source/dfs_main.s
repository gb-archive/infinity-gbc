; Copyright (C) 1999-2001 Affinix Software, LLC
;
; This file is part of Infinity.
;
; This file may be used under the terms of the Creative Commons Attribution-
; NonCommercial-ShareAlike 4.0 International License as published by Creative
; Commons.
;
; Alternatively, this file may be used under the terms of the GNU General
; Public License as published by the Free Software Foundation, either version
; 3 of the License, or (at your option) any later version.
;
; In addition, as a special exception, Affinix Software gives you certain
; additional rights. These rights are described in the LICENSE file in this
; package.

   .include "gbasm.h"

   .area _CODE_13

   .include "font.h"

;----------------------------------------------------------------------------
;[ DrawChar ]
;parameters:
; A = Character to Draw
; B = X offset (0-7)
; HL -> Starting Tile
;modifies:
; ABCDEFHL
;----------------------------------------------------------------------------
DrawChar::
   PUSH  HL
   SWAP  A
   LD    H, A
   AND   #0xF0
   LD    L, A
   XOR   H
   LD    H, A

   PUSH  HL
   CALL  getsmallfont
   POP   HL

   ADD   HL, DE
   LD    D, H
   LD    E, L
   POP   HL

   LD    A, B
   ADD   A, A
   LD    A, H
   ADC   A, #0
   LD    H, A

   LD    A, B
   ADD   A, A
   AND   #0xF0

   ADD   A, L
   LD    L, A
   ADC   A, H
   SUB   L
   LD    H, A

   LD    A, B
   AND   #0x07

   CALL  JumpTable
.DW DC_align,  DC_shift1, DC_shift2, DC_shift3
.DW DC_shift4, DC_shift5, DC_shift6, DC_shift7

DC_align:
   LD    B, #16
   JP    dfs_align_loop

DC_shift1:
   LD    BC, #0x087F
   PUSH  DE
   CALL  dfs_shift1_loop
   POP   DE
   LD    BC, #0x0880
   JP    dfs_shift1_loop

DC_shift2:
   LD    BC, #0x083F
   PUSH  DE
   CALL  dfs_shift2_loop
   POP   DE
   LD    BC, #0x08C0
   JP    dfs_shift2_loop

DC_shift3:
   LD    BC, #0x081F
   PUSH  DE
   CALL  dfs_shift3_loop
   POP   DE
   LD    BC, #0x08E0
   JP    dfs_shift3_loop

DC_shift4:
   LD    BC, #0x080F
   PUSH  DE
   CALL  dfs_shift4_loop
   POP   DE
   LD    BC, #0x08F0
   JP    dfs_shift4_loop

DC_shift5:
   LD    BC, #0x0807
   PUSH  DE
   CALL  dfs_shift5_loop
   POP   DE
   LD    BC, #0x08F8
   JP    dfs_shift5_loop

DC_shift6:
   LD    BC, #0x0803
   PUSH  DE
   CALL  dfs_shift6_loop
   POP   DE
   LD    BC, #0x08FC
   JP    dfs_shift6_loop

DC_shift7:
   LD    BC, #0x0801
   PUSH  DE
   CALL  dfs_shift7_loop
   POP   DE
   LD    BC, #0x08FE
   JP    dfs_shift7_loop

;----------------------------------------------------------------------------
;[ BC_getsize ]
;parms:
; A = Character
; B = coordinate of char (len)
; DE -> Font Table
;returns:
; NC if non-extended
; CY if extended
;modifies:
; ABDE
;----------------------------------------------------------------------------
BC_getsize::
   ADD   A, E                          ; DE->size of font
   LD    E, A                          ;
   ADC   A, D                          ;
   SUB   E                             ;
   LD    D, A                          ;

   LD    A, (DE)                       ; get size
   RLCA                                ; shift a bit
   SRL   A                             ; shift out a bit
   RET   NC                            ; NC if not extended

   PUSH  AF                            ; extended character adds 8 to width
   LD    A, B                          ;
   ADD   A, #0x08                      ;
   LD    B, A                          ;
   POP   AF                            ;

   RET

;----------------------------------------------------------------------------
;[ DrawBigCharSprite ]
;parameters:
; A = Character to Draw
; B = X offset
; DE -> Font Table
; HL -> Starting Tile
;modifies:
; ABCDEFHL
;----------------------------------------------------------------------------
DrawBigCharSprite::
   PUSH  HL          ; Save Starting Tile

   LD    L, A
   ADD   A, A        ; A = A * 2
   ADD   A, L
   LD    L, A
   LD    A, #0
   ADC   A, A
   LD    H, A        ; HL = A * 3

   ADD   HL, HL
   ADD   HL, HL
   ADD   HL, HL      ; HL = A * 24
   ADD   HL, DE      ; HL = A * 24 + Tile Data
   PUSH  HL          ; Save Font Address

   LD    A, B        ; HL=(B&0xF8)*4
   AND   #0xF8
   RLCA
   RLCA
   LD    C, A
   AND   #0xFC
   LD    L, A
   XOR   C
   LD    H, A

   LD    A, B
   AND   #0x07       ; Setup for jumptable

   POP   DE          ; DE = A * 24 + DE
   POP   BC          ; BC = Starting Tile
   ADD   HL, BC      ; HL = (B&0xF8)*4 + HL

;   LD    HL, #0x8800

   CALL  JumpTable

.DW DBCS_align,  DBCS_shift1, DBCS_shift2, DBCS_shift3
.DW DBCS_shift4, DBCS_shift5, DBCS_shift6, DBCS_shift7

DBCS_align:
   LD    B, #24
   JP    dfs_align_loop

DBCS_shift1:
   LD    BC, #0x0C7F
   PUSH  DE
   CALL  dfs_shift1_loop
   POP   DE
   LD    C, #0x08
   ADD   HL, BC
   LD    BC, #0x0C80
   JP    dfs_shift1_loop

DBCS_shift2:
   LD    BC, #0x0C3F
   PUSH  DE
   CALL  dfs_shift2_loop
   POP   DE
   LD    C, #0x08
   ADD   HL, BC
   LD    BC, #0x0CC0
   JP    dfs_shift2_loop

DBCS_shift3:
   LD    BC, #0x0C1F
   PUSH  DE
   CALL  dfs_shift3_loop
   POP   DE
   LD    C, #0x08
   ADD   HL, BC
   LD    BC, #0x0CE0
   JP    dfs_shift3_loop

DBCS_shift4:
   LD    BC, #0x0C0F
   PUSH  DE
   CALL  dfs_shift4_loop
   POP   DE
   LD    C, #0x08
   ADD   HL, BC
   LD    BC, #0x0CF0
   JP    dfs_shift4_loop

DBCS_shift5:
   LD    BC, #0x0C07
   PUSH  DE
   CALL  dfs_shift5_loop
   POP   DE
   LD    C, #0x08
   ADD   HL, BC
   LD    BC, #0x0CF8
   JP    dfs_shift5_loop

DBCS_shift6:
   LD    BC, #0x0C03
   PUSH  DE
   CALL  dfs_shift6_loop
   POP   DE
   LD    C, #0x08
   ADD   HL, BC
   LD    BC, #0x0CFC
   JP    dfs_shift6_loop

DBCS_shift7:
   LD    BC, #0x0C01
   PUSH  DE
   CALL  dfs_shift7_loop
   POP   DE
   LD    C, #0x08
   ADD   HL, BC
   LD    BC, #0x0CFE
   JP    dfs_shift7_loop

;----------------------------------------------------------------------------
;[ DrawBigChar ]
;parameters:
; A = Character to Draw
; (B,C) = X,Y offset
; DE -> Font Table
; HL -> Starting Tile
;modifies:
; ABCDEFHL
;----------------------------------------------------------------------------
DrawBigChar::
   PUSH  HL          ; Save Starting Tile

   LD    L, A
   ADD   A, A        ; A = A * 2
   ADD   A, L
   LD    L, A
   LD    A, #0
   ADC   A, A
   LD    H, A        ; HL = A * 3

   ADD   HL, HL
   ADD   HL, HL
   ADD   HL, HL      ; HL = A * 24
   ADD   HL, DE      ; HL = A * 24 + Tile Data
   PUSH  HL          ; Save Font Address

   LD    H, #0
   LD    L, C
   ADD   HL, HL      ; HL = C * 2

   LD    A, B
   AND   #0xF8
   RRCA
   LD    C, A
   RRCA
   RRCA
   ADD   A, C
   SWAP  A
   LD    C, A
   AND   #0xF0
   LD    E, A
   XOR   C
   LD    D, A        ; DE = (B & 0xF8) * 10
   ADD   HL, DE      ; HL = ((B & 0xF8) * 5 + C) * 2

   LD    A, B
   AND   #0x07       ; Setup for jumptable

   POP   DE          ; DE = A * 24 + DE
   POP   BC          ; BC = Starting Tile
   ADD   HL, BC      ; HL = ((B & 0xF8) * 5 + C) * 2 + HL

   CALL  JumpTable

.DW DBC_align,  DBC_shift1, DBC_shift2, DBC_shift3
.DW DBC_shift4, DBC_shift5, DBC_shift6, DBC_shift7

DBC_align:
   LD    B, #24

dfs_align_loop:
   LD    A, (DE)

   INC   DE
   CALL  or_a_vram

   DEC   B
   JR    NZ, dfs_align_loop
   RET

DBC_shift1:
   LD    BC, #0x0C7F
   PUSH  DE
   CALL  dfs_shift1_loop
   POP   DE
   LD    C, #56                         ;5*16-2*12
   ADD   HL, BC
   LD    BC, #0x0C80

dfs_shift1_loop:
   LD    A, (DE)
   INC   DE
   RRCA
   AND   C

   CALL  or_a_vram

   LD    A, (DE)
   RRCA
   AND   C

   CALL  or_a_vram
   INC   DE
   DEC   B
   JR    NZ, dfs_shift1_loop
   RET

DBC_shift2:
   LD    BC, #0x0C3F
   PUSH  DE
   CALL  dfs_shift2_loop
   POP   DE
   LD    C, #56
   ADD   HL, BC
   LD    BC, #0x0CC0

dfs_shift2_loop:
   LD    A, (DE)
   INC   DE
   RRCA
   RRCA
   AND   C

   CALL  or_a_vram

   LD    A, (DE)
   RRCA
   RRCA
   AND   C

   CALL  or_a_vram
   INC   DE
   DEC   B
   JR    NZ, dfs_shift2_loop
   RET

DBC_shift3:
   LD    BC, #0x0C1F
   PUSH  DE
   CALL  dfs_shift3_loop
   POP   DE
   LD    C, #56
   ADD   HL, BC
   LD    BC, #0x0CE0

dfs_shift3_loop:
   LD    A, (DE)
   INC   DE
   RRCA
   RRCA
   RRCA
   AND   C

   CALL  or_a_vram

   LD    A, (DE)
   RRCA
   RRCA
   RRCA
   AND   C

   CALL  or_a_vram
   INC   DE
   DEC   B
   JR    NZ, dfs_shift3_loop
   RET

DBC_shift4:
   LD    BC, #0x0C0F
   PUSH  DE
   CALL  dfs_shift4_loop
   POP   DE
   LD    C, #56
   ADD   HL, BC
   LD    BC, #0x0CF0

dfs_shift4_loop:
   LD    A, (DE)
   INC   DE
   SWAP  A
   AND   C

   CALL  or_a_vram

   LD    A, (DE)
   SWAP  A
   AND   C

   CALL  or_a_vram
   INC   DE
   DEC   B
   JR    NZ, dfs_shift4_loop
   RET

DBC_shift5:
   LD    BC, #0x0C07
   PUSH  DE
   CALL  dfs_shift5_loop
   POP   DE
   LD    C, #56
   ADD   HL, BC
   LD    BC, #0x0CF8

dfs_shift5_loop:
   LD    A, (DE)
   INC   DE
   RLCA
   RLCA
   RLCA
   AND   C

   CALL  or_a_vram

   LD    A, (DE)
   RLCA
   RLCA
   RLCA
   AND   C

   CALL  or_a_vram
   INC   DE
   DEC   B
   JR    NZ, dfs_shift5_loop
   RET

DBC_shift6:
   LD    BC, #0x0C03
   PUSH  DE
   CALL  dfs_shift6_loop
   POP   DE
   LD    C, #56
   ADD   HL, BC
   LD    BC, #0x0CFC

dfs_shift6_loop:
   LD    A, (DE)
   INC   DE
   RLCA
   RLCA
   AND   C

   CALL  or_a_vram

   LD    A, (DE)
   RLCA
   RLCA
   AND   C

   CALL  or_a_vram
   INC   DE
   DEC   B
   JR    NZ, dfs_shift6_loop
   RET

DBC_shift7:
   LD    BC, #0x0C01
   PUSH  DE
   CALL  dfs_shift7_loop
   POP   DE
   LD    C, #56
   ADD   HL, BC
   LD    BC, #0x0CFE

dfs_shift7_loop:
   LD    A, (DE)
   INC   DE
   RLCA
   AND   C

   CALL  or_a_vram

   LD    A, (DE)
   RLCA
   AND   C

   CALL  or_a_vram
   INC   DE
   DEC   B
   JR    NZ, dfs_shift7_loop
   RET
