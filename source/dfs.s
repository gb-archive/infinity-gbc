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

;----------------------------------------------------------------------------
; DFS - Dynamic Font System
;----------------------------------------------------------------------------

_rom_page_font          = _rom_page_dfs

   .area _BSS

;_dfs_dma::
;   .ds   31

_dfs_face::
   .ds   1

   .area _CODE

big_font_types:
   .DW   bigfont, creditsfont
small_font_types:
   .DW   smallfont, titlefont

getbigfont::
   PUSH  HL
   LD    HL, #big_font_types
   CALL  getfont
   POP   HL
   RET

getsmallfont::
   PUSH  HL
   LD    HL, #small_font_types
   CALL  getfont
   POP   HL
   RET

getfont:
   PUSH  AF
   LD    A, (_dfs_face)
   ADD   A, A
   LD    D, #0
   LD    E, A
   ADD   HL, DE
   LD    A, (HLI)
   LD    D, (HL)
   LD    E, A
   POP   AF
   RET

;----------------------------------------------------------------------------
;[ dfs_line ]
;parameters:
; (barg1,barg2) Tile Offset
; barg3 = Starting Character
; barg4 = Length
;modifies:
; none
;----------------------------------------------------------------------------
_dfs_line::
   PUSH  BC
   LD    HL, #_barg1    ; HL -> barg1
   LD    A, (HLI)
   LD    B, A           ; B = barg1
   LD    A, (HLI)
   LD    C, A           ; C = barg2
   LD    A, (HLI)       ; A = barg3
   LD    D, (HL)        ; D = barg4

   LD    E, A
   LD    A, C
   RRCA
   RRCA
   RRCA
   LD    L, A
   AND   #0x1F
   ADD   A, #C_PP_VRAM_WT
   LD    H, A
   LD    A, L
   AND   #0xE0
   ADD   A, B
   LD    L, A
   LD    A, E
DFSL_loop:
   RST   ldi_v_a
   INC   A
   DEC   D
   JR    NZ, DFSL_loop
   POP   BC
   RET

;----------------------------------------------------------------------------
;[ dfs_window ]
;----------------------------------------------------------------------------
_dfs_window::
   PUSH  BC
   LD    A, #165
   CALL  Calc_PA_in_PT_at_A

   LD    D, #90
   XOR   A
DFSC2_loop:
   LD    E, #16
DFSC2_loop2:
   RST   ldi_v_a
   DEC   E
   JR    NZ, DFSC2_loop2
   DEC   D
   JR    NZ, DFSC2_loop

   LD    HL, #0x9C21
   CALL  make_dfs_bigthing

   LD    HL, #0x9D01
   CALL  make_dfs_bigthing

   POP   BC
   RET

make_dfs_bigthing:
   LD    A, #165
   LD    DE, #0x5905
   LD    BC, #0x1205
DFSW_loop2:
   PUSH  BC
DFSW_loop:
   RST   ldi_v_a
   ADD   A, E
   DEC   B
   JR    NZ, DFSW_loop
   SUB   D
   LD    C, #14
   ADD   HL, BC
   POP   BC
   DEC   C
   JR    NZ, DFSW_loop2
   RET

;;----------------------------------------------------------------------------
;;[[ void dfs_winscroll() ]]
;;----------------------------------------------------------------------------
;_dfs_winscroll::
;   PUSH  BC
;
;   LD    HL, #0x9C21
;   LD    DE, #0x9C41
;
;   LD    C, #0x04
;ws_loop2:
;   LD    B, #0x12
;ws_loop:
;   CALL  mov_hl_de_vram
;   DEC   B
;   JR    NZ, ws_loop
;   LD    A, #0x0E
;   ADD   A, L
;   LD    L, A
;   ADC   A, H
;   SUB   L
;   LD    H, A
;
;   LD    A, #0x0E
;   ADD   A, E
;   LD    E, A
;   ADC   A, D
;   SUB   E
;   LD    D, A
;
;   ;PUSH  BC
;   ;LD    C, #0x0E
;   ;ADD   HL, BC
;   ;POP   BC
;   DEC   C
;   JR    NZ, ws_loop2
;
;   LD    A, #0x04
;   LD    B, #0x12
;ws_loop3:
;   CALL  ld_vram_a
;   DEC   B
;   JR    NZ, ws_loop3
;
;   POP   BC
;   RET

;----------------------------------------------------------------------------
;[[ void dfs_clear(ubyte start, ubyte length) ]]
;----------------------------------------------------------------------------
_dfs_clear::
   LDHL  SP, #2

   LD    A, (HLI)                  ; a = start
   LD    D, (HL)                   ; d = length

   CALL  Calc_PA_in_PT_at_A

   XOR   A
DFSC_loop:
   LD    E, #16
DFSC_loop2:
   RST   ldi_v_a
   DEC   E
   JR    NZ, DFSC_loop2
   DEC   D
   JR    NZ, DFSC_loop

   RET

;----------------------------------------------------------------------------
;[ Calc_PA_in_PT_at_A ]
;parameters:
; A = Tile Number
;returns:
; HL -> Address of the Tile in VRAM
;modifies:
; AHL
;----------------------------------------------------------------------------
Calc_PA_in_PT_at_A:
   SWAP  A
   LD    H, A
   AND   #0xF0
   LD    L, A
   XOR   H
   ADD   A, #C_PP_VRAM_PT
   LD    H, A
   RET

;----------------------------------------------------------------------------
;[ dfs_pstr ]
;parameters:
; barg1 = Starting X offset (0-7)
; barg3 = Starting Tile
; warg1 -> String
;modifies:
; none
;----------------------------------------------------------------------------
_dfs_pstr::
   PUSH  BC

   LD    HL, #_warg1
   LD    A, (HLI)
   LD    H, (HL)
   LD    L, A

   LD    A, (_barg1)
   LD    B, A
   LD    A, (_barg3)

   PUSH  HL
   CALL  Calc_PA_in_PT_at_A
   POP   DE

   CALL  DS_loop
   POP   BC

   RET

DS_loop:
   LD    A, (DE)
   OR    A
   RET   Z

   PUSH  DE

   PUSH  AF
   LDH   A, (_rompage)
   LD    D, A
   LD    A, #_rom_page_font
   LDH   (_rompage), A
   LD    (0x2000), A
   POP   AF

   PUSH  DE
   PUSH  HL
   PUSH  BC
   PUSH  AF
   CALL  DrawChar
   POP   AF

;   LD    HL, #smallfont
;   ADD   A, L
;   LD    L, A
;   ADC   A, H
;   SUB   L
;   LD    H, A

   CALL  getsmallfont
   ADD   A, E
   LD    L, A
   ADC   A, D
   SUB   L
   LD    H, A

   POP   AF
   ADD   A, (HL)
   LD    B, A
   POP   HL
   POP   AF

   LDH   (_rompage), A
   LD    (0x2000), A

   POP   DE
   INC   DE

   JR    DS_loop

;DC_flush:
;   LD    BC, #_dfs_dma+15
;   LD    A, B
;   LDH   (0x51), A                     ; 51/52 -> source of sprite
;   LD    A, C
;   LDH   (0x52), A
;   LD    A, H
;   LDH   (0x53), A                     ; 53/54 -> destination of sprite
;   LD    A, L
;   LDH   (0x54), A
;   XOR   A                             ; Blast DMA
;   LDH   (0x55), A
;   RET

;----------------------------------------------------------------------------
;[ dfs_pchar ]
;parameters:
; (barg1,barg2) = Starting X,Y offset
; barg4 = Character
;modifies:
; barg1 = Updated X offset
;----------------------------------------------------------------------------
_dfs_pchar::
   PUSH  BC

   LDH   A, (_rompage)
   PUSH  AF

   LD    A, #_rom_page_font
   LDH   (_rompage), A
   LD    (0x2000), A

   LD    HL, #_barg1
   LD    A, (HLI)
   LD    C, (HL)
   LD    B, A

;   LD    DE, #bigfont
   CALL  getbigfont

   LD    A, #165
   CALL  Calc_PA_in_PT_at_A

   LD    A, (_barg4)
PC_extended:
   PUSH  AF
   PUSH  BC
   PUSH  DE
   PUSH  HL
   CALL  DrawBigChar
   POP   HL
   POP   DE
   POP   BC
   POP   AF

   PUSH  DE
   CALL  BC_getsize
   POP   DE

   JR    C, PC_extended

   ADD   A, B
   LD    (_barg1), A

   POP   AF

   LDH   (_rompage), A
   LD    (0x2000), A

   POP   BC
   RET

;----------------------------------------------------------------------------
;[[ ubyte dfs_pschar(ubyte start_tile, ubyte x, ubyte chr) ]]
;----------------------------------------------------------------------------
_dfs_pschar::
   LDHL  SP, #4                         ; HL->chr

   PUSH  BC

   LDH   A, (_rompage)
   PUSH  AF

   LD    A, #_rom_page_font
   LDH   (_rompage), A
   LD    (0x2000), A

;   LD    DE, #bigfont
   CALL  getbigfont

   LD    A, (HLD)
   LD    C, A                           ; chr
   LD    A, (HLD)
   LD    B, A                           ; x

   LD    A, (HL)                        ; start_tile
   CALL  Calc_PA_in_PT_at_A

   LD    A, C

PC_extended2:
   PUSH  AF
   PUSH  BC
   PUSH  DE
   PUSH  HL
   CALL  DrawBigCharSprite
   POP   HL
   POP   DE
   POP   BC
   POP   AF

   PUSH  DE
   CALL  BC_getsize
   POP   DE

   JR    C, PC_extended2

   ADD   A, B
   LD    E, A

   POP   AF

   LDH   (_rompage), A
   LD    (0x2000), A

   POP   BC
   RET

;;----------------------------------------------------------------------------
;[[ dfs_strlen() ]]
;;parameters:
;; warg1 -> String
;;modifies:
;; barg4 = length
;;----------------------------------------------------------------------------
;_dfs_strlen::
;   PUSH  BC
;
;   LDH   A, (_rompage)
;   LD    C, A
;
;   LD    HL, #_warg1
;   LD    A, (HLI)
;   LD    H, (HL)
;   LD    L, A
;
;   LD    B, #0
;   CALL  SL_loop
;
;   LD    A, B
;   LD    (_barg4), A
;
;   POP   BC
;   RET

; C=rompage
SL_loop:
   LD    A, (HLI)
   CP    #0x20    ; ' '
   RET   Z
   CP    #0x7C    ; '|'
   RET   Z
   OR    A        ; nul
   RET   Z

   LD    D, A
   LD    A, #_rom_page_font
   LDH   (_rompage), A
   LD    (0x2000), A
   LD    A, D

SL_extended:
;   LD    DE, #bigfont
   CALL  getbigfont
   CALL  BC_getsize
   JR    C, SL_extended

   ADD   A, B
   LD    B, A

   LD    A, C
   LDH   (_rompage), A
   LD    (0x2000), A

   JR    SL_loop
