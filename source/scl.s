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

_scl_table::
   .ds   144
_scl_on::
   .ds   1

   .area _CODE

_winbkg_on::
   XOR   A
   LDH   (P_SCX), A
   LDH   (P_SCY), A
   LDH   (_rscx), A
   LDH   (_rscy), A
   LD    A, #0b11001111
   LDH   (P_LCDC), A
   RET

_winbkg_off::
   LD    A, (_owe_scrollx)             ;
   LDH   (P_SCX), A                 ;
   LDH   (_rscx), A
   LD    A, (_owe_scrolly)             ;
   LDH   (P_SCY), A                 ;
   LDH   (_rscy), A
   LD    A, #0b11100111
   LDH   (P_LCDC), A
   RET

_scl_off::
   XOR   A
   LD    (_scl_on), A
   RET

_scl_pixelate::
   LDHL  SP, #2
   PUSH  BC

   LD    D, #144

   LD    C, (HL)

   LD    HL, #_grad_every
   LD    A, C
   DEC   A
   JR    Z, pix_go_on

   LD    A, #1
   SET   7, (HL)

   JR    pix_go_on2
pix_go_on:
   RES   7, (HL)
   LDH   (P_SCY), A                 ;

pix_go_on2:
   LD    (_scl_on), A

   LD    A, C
   SRL   A
   LD    B, C

   LD    HL, #_scl_table
pixelate_loop:
   LD    (HLI), A
   DEC   D
   JR    Z, finished_pixelate
   DEC   B
   JR    NZ, pixelate_loop
   LD    B, C
   ADD   A, C
   JR    pixelate_loop

finished_pixelate:
finished_stretch:
   POP   BC

   RET

_scl_stretch::
   LDHL  SP, #2
   PUSH  BC

   LD    A, (_owe_mode)
   OR    A
   JR    Z, set_owe_mode_skip
   LD    A, #2
   LD    (_owe_mode), A
set_owe_mode_skip:

   LD    D, #72

   LD    A, (HL)
   LD    C, A
   DEC   A
   ADD   A, #255
   LD    A, #0
   ADC   A, A
   LD    (_scl_on), A

   LD    A, D
   LD    B, C

   LD    HL, #_scl_table+72
stretch_loop:
   LD    (HLI), A
   DEC   D
   JR    Z, finished_stretch1
   DEC   B
   JR    NZ, stretch_loop
   LD    B, C
   INC   A
   JR    stretch_loop

finished_stretch1:
   LD    A, #71
   LD    D, #72

   LD    B, C

   LD    HL, #_scl_table+71
stretch_loop2:
   LD    (HLD), A
   DEC   D
   JR    Z, finished_stretch
   DEC   B
   JR    NZ, stretch_loop2
   LD    B, C
   DEC   A
   JR    stretch_loop2

_scl_init::
   LDH   A, (_rompage)                 ;3
   PUSH  AF                            ;4

   LD    A, #_rom_page_title           ;2
   LDH   (_rompage), A                 ;3
   LD    (0x2000), A                   ;4

   CALL  _scl_init_real                ;6

   POP   AF                            ;3
   LDH   (_rompage), A                 ;3
   LD    (0x2000), A                   ;4
   RET                                 ;4

;----------------------------------------------------------------------------
;[ void scl_roll(ubyte frame) ] frame=0-108
;----------------------------------------------------------------------------
_scl_roll::
   LDH   A, (_rompage)                 ;3
   PUSH  AF                            ;4

   LD    A, #_rom_page_title           ;2
   LDH   (_rompage), A                 ;3
   LD    (0x2000), A                   ;4

   CALL  _scl_roll_real                ;6

   POP   AF                            ;3
   LDH   (_rompage), A                 ;3
   LD    (0x2000), A                   ;4
   RET                                 ;4

_scl_hbl::
   LDH   A, (_rompage)                 ;3
   PUSH  AF                            ;4

   LD    A, #_rom_page_title           ;2
   LDH   (_rompage), A                 ;3
   LD    (0x2000), A                   ;4

   CALL  _scl_hbl_real                 ;6

   POP   AF                            ;3
   LDH   (_rompage), A                 ;3
   LD    (0x2000), A                   ;4

   JP    _hbl_end                      ;4

_scl_prophecy::
   LDHL  SP, #2
   LD    A, (HL)
   OR    A
   LD    (_scl_on), A
   RET   Z

   PUSH  BC
   LD    B, #0x50

   LD    D, A
   SUB   B
   ADD   A, #144
   LD    C, A

   LD    A, D

   LD    HL, #_scl_table

   SUB   B
   JR    NC, _do_real_lines

   LD    E, A
   LD    A, #144
_killtop:
   LD    (HLI), A
   DEC   B
   INC   E
   JR    NZ, _killtop

   LD    C, A
_do_real_lines:
   LD    A, #176
   SUB   D
   JR    Z, _killbot
   LD    E, A

   LD    A, C
_domiddle:
   LD    (HLI), A
   INC   A
   DEC   B
   JR    Z, done_prophecy
   DEC   E
   JR    NZ, _domiddle

   LD    A, #144
_killbot:
   LD    (HLI), A
   DEC   B
   JR    NZ, _killbot

done_prophecy:
   POP   BC

   RET
