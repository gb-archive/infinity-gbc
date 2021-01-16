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

tile_base = 0

   .area _BSS

   .area _CODE

_draw_menu::
   PUSH  BC

   LDHL  SP, #4
   LD    A, (HL)
   PUSH  AF
   XOR   #3
   LD    H, #0
   LD    L, A
   ADD   HL, HL
   LD    BC, #tab_table
   ADD   HL, BC
   LD    B, H
   LD    C, L

   XOR   #3
   RRCA
   RRCA
   RRCA
   LD    E, A
   AND   #0x1F
   LD    D, A
   XOR   E
   LD    E, A                 ; DE -> row
   LD    HL, #tab_bottom_table
   ADD   HL, DE
   LD    D, H
   LD    E, L

   LD    HL, #_window_ptr
   LD    A, (HLI)
   LD    H, (HL)
   LD    L, A                 ; HL -> window

   PUSH  DE
   LD    DE, #4
   PUSH  HL
   ADD   HL, DE
   CALL  puttab
   POP   HL
   LD    E, #10
   PUSH  HL
   ADD   HL, DE
   CALL  puttab
   POP   HL
   LD    E, #14
   PUSH  HL
   ADD   HL, DE
   CALL  puttab
   POP   HL

   LD    E, #64
   ADD   HL, DE
   POP   DE

   PUSH  HL

   LD    B, #20
dm_loop:
   LD    A, (DE)
   INC   DE
   RST   ldi_v_a
   DEC   B
   JR    NZ, dm_loop

   POP   HL

   LD    BC, #33
   ADD   HL, BC

   POP   AF
   CALL  JumpTable

   .dw   menu_ability, menu_backpack
   .dw   menu_equip,   menu_gems

menu_ability:
   LD    A, #0x04
   LD    E, #0x0E
dm_loop3:
   LD    BC, #0x120E
dm_loop2:
   RST   ldi_v_a
   DEC   B
   JR    NZ, dm_loop2
   ADD   HL, BC

   DEC   E
   JR    NZ, dm_loop3

   JR    menu_finish

menu_backpack:
   LD    A, #0x09
   LD    DE, #0x070A
   CALL  dm_routine
   LD    A, #0x0B
   RST   ldi_v_a
   ADD   HL, BC

   LD    BC, #0x050C
   LD    DE, #0x0403
dm_loop5:
   PUSH  BC
   LD    A, B
   CALL  dm_routine
   LD    A, #15
   RST   ldi_v_a
   ADD   HL, BC
   POP   BC
   DEC   C
   JR    NZ, dm_loop5

   LD    A, #0x0C
   LD    DE, #0x010D
   CALL  dm_routine
   LD    A, #0x0E
   RST   ldi_v_a

menu_gems:
menu_equip:
menu_finish:
   POP   BC
   RET

;----------------------------------------------------------------------------
;[[ menu_clearbox(ubyte x, ubyte y, ubyte w, ubyte h) ]]
;----------------------------------------------------------------------------
_menu_clearbox::
   LDHL  SP, #0x05
   PUSH  BC
   LD    A, (HLD)
   LD    C, A
   LD    A, (HLD)
   LD    B, A
   LD    A, (HLD)
   RRCA
   RRCA
   RRCA
   LD    E, A
   AND   #0x1F
   LD    D, A
   XOR   E
   LD    E, (HL)
   ADD   A, E
   LD    L, A

   LD    A, D
   ADD   A, #0x9C
   LD    H, A

   LD    A, #32
   SUB   B
   LD    E, A
   LD    D, #0x00

   LD    A, #0x04

_mcb_loop:
   PUSH  BC
   CALL  _STIR_B_VRAM
   ADD   HL, DE
   POP   BC
   DEC   C
   JR    NZ, _mcb_loop

   POP   BC
   RET

; x, y, length, height
_menu_drawbox::
   LDHL  SP, #5
   PUSH  BC

   LD    A, (HLD)   ; height
   DEC   A
   DEC   A
   LD    C, A
   LD    A, (HLD)   ; length
   DEC   A
   DEC   A
   LD    B, A
   LD    A, (HLD)   ; y
   RRCA
   RRCA
   RRCA
   LD    E, A
   AND   #0x1F
   LD    D, A
   XOR   E
   ADD   A, (HL)

   LD    L, A
   LD    A, D
   ADD   A, #0x9C
   LD    H, A

   XOR   A
   LD    DE, #0x0102
   PUSH  BC
   CALL  dm_custom
   POP   BC
   LD    A, #30
   SUB   B
   LD    D, #0x00
   LD    E, A
   ADD   HL, DE

dm_loop_1:
   LD    A, #0x03
   LD    DE, #0x0405
   PUSH  BC
   CALL  dm_custom
   POP   BC
   LD    A, #30
   SUB   B
   LD    D, #0x00
   LD    E, A
   ADD   HL, DE

   DEC   C
   JR    NZ, dm_loop_1

   LD    A, #0x06
   LD    DE, #0x0708
   CALL  dm_custom

   POP   BC

   RET

; draws tiles:
; ADDDDDDDE     <- D comes up B times
dm_routine:
   LD    BC, #0x0F0E
dm_custom:
   RST   ldi_v_a
   LD    A, D
dm_loop4:
   RST   ldi_v_a
   DEC   B
   JR    NZ, dm_loop4
   LD    A, E
   RST   ldi_v_a
   RET

puttab:
   LD    A, (BC)
   INC   BC
   RST   ldi_v_a
   LD    E, #31
   ADD   HL, DE
   LD    A, (BC)
   INC   BC
   RST   ldi_v_a
   RET

tab_bottom_table:
   .DB   3, 4, 4, 4,12, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2
tab_table:
   .DB   0, 3, 0, 3, 0, 3, 2, 5, 2, 5, 2, 5
   .DB   0, 1, 1, 1,13, 4, 4, 4, 4, 4,12, 1, 1, 1, 1, 1, 1, 1, 1, 2
   .DB  72,73,68,69,65,75,73, 0, 0, 0, 0, 0
   .DB   0, 1, 1, 1, 1, 1, 1, 1, 1, 1,13, 4, 4, 4,12, 1, 1, 1, 1, 2
   .DB  74,85,83,84,73,78, 0, 0, 0, 0, 0, 0
   .DB   0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,13, 4, 4, 4, 4, 5
