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
; PFS - Palette Fading System
;----------------------------------------------------------------------------
   .area _BSS

_ram_page_pfs           = 5            ; reserve from D0 - D8

_pfs_rgb_src            = 0xD000       ; 192 bytes (original palette)
_pfs_rgb_dst            = 0xD0C0       ; 192 bytes (destination palette)
_pfs_bresen             = 0xD180       ; 768 bytes (bresenham structure)

_pal_cur::
   .ds   128
_pal_bak::
   .ds   128
_pfs_fadelen::
   .ds   1
_pfs_targetbgr::
   .ds   1
_pfs_targetspr::
   .ds   1

   .area _CODE

;----------------------------------------------------------------------------
;[ pfs_copy_pal ]
;----------------------------------------------------------------------------
;parm:
; DE -> 16-bit RGB source
; HL -> 16-bit RGB dest
; BC = bits of palettes to copy B=bgr, C=spr
;value:
; none
;----------------------------------------------------------------------------
_pfs_copy_pal:
   LDH   A, (_rompage)
   PUSH  AF

   CALL  _getptrdat
   JR    Z, _pfs_copy_de

_pfs_copy_loop:
   SLA   C
   RL    B
   JR    NC, _no_copy

   LD    A, #0x08
_pfs_copy_loop2:
   PUSH  AF
   LD    A, (DE)                       ; (HL)=(DE)
   LD    (HLI), A                      ;
   INC   DE                            ;
   POP   AF
   DEC   A
   JR    NZ, _pfs_copy_loop2

   LD    A, B
   OR    C

   JR    NZ, _pfs_copy_loop
_pfs_done_copy:
   POP   AF
   LDH   (_rompage), A
   LD    (0x2000), A

   RET

_no_copy:
   LD    A, B
   OR    C

   JR    Z, _pfs_done_copy

   LD    A, #0x08                      ; DE+=8
   ADD   A, E                          ;
   LD    E, A                          ;
   ADC   A, D                          ;
   SUB   E                             ;
   LD    D, A                          ;

   LD    A, #0x08                      ; HL+=8
   ADD   A, L                          ;
   LD    L, A                          ;
   ADC   A, H                          ;
   SUB   L                             ;
   LD    H, A                          ;

   JR    _pfs_copy_loop

_pfs_copy_de:
   SLA   C
   RL    B
   JR    NC, _no_copy_de

   LD    A, #0x04
_pfs_copy_de2:
   LD    (HL), E                       ; (HL)=E
   INC   HL
   LD    (HL), D                       ; (HL)=D
   INC   HL
   DEC   A
   JR    NZ, _pfs_copy_de2

   LD    A, B
   OR    C

   JR    NZ, _pfs_copy_de

   JR    _pfs_done_copy

_no_copy_de:
   LD    A, B
   OR    C

   JR    Z, _pfs_done_copy

   LD    A, #0x08                      ; HL+=8
   ADD   A, L                          ;
   LD    L, A                          ;
   ADC   A, H                          ;
   SUB   L                             ;
   LD    H, A                          ;

   JR    _pfs_copy_de

;----------------------------------------------------------------------------
_palettes:
   .DW   0b000000000000000             ; black
   .DW   0b111111111111111             ; white
   .DW   0b000000000001111             ; red
   .DW   0b011110000000000             ; blue
   .DW   0b000001011011001             ; yellow
   .DW   0b001110000000000             ; blue_dark
   .DW   0b000000011000101             ; brown_dark
                                       ; greatdark = ff

; NZ = ptr
; Z = dat
_getptrdat:
   LD    A, D
   OR    A
   RET   NZ
   LD    A, E
   CP    #0xFF
   JR    Z, _get_grdk_ptr
   PUSH  HL
   LD    HL, #_palettes
   ADD   HL, DE
   ADD   HL, DE
   LD    A, (HLI)
   LD    D, (HL)
   LD    E, A
   POP   HL
   XOR   A
   RET   

_get_grdk_ptr:
   LD    A, #_rom_page_title
   LDH   (_rompage), A
   LD    (0x2000), A
   LD    DE, #_great_dark_fade_pal
   INC   A
   RET

_asm_setpal::
   PUSH  BC
   JR    setpaljoin

;----------------------------------------------------------------------------
;[ pfs_setpal(uword *pal) ]
;----------------------------------------------------------------------------
_pfs_setpal::
   PUSH  BC

   LDHL  SP, #4                        ; HL->first parameter

   LD    A, (HLI)                      ; DE=pal
   LD    D, (HL)                       ;
   LD    E, A                          ;

setpaljoin:
   LD    HL, #_pfs_targetbgr           ;
   LD    A, (HLI)                      ;
   LD    C, (HL)                       ;
   LD    B, A                          ; BC=target palettes

   LD    HL, #_pal_cur                 ; HL=current pal
   CALL  _pfs_copy_pal                 ;

   CALL  set_pals

   POP   BC
   RET

set_pals:
   LD    A, (_pfs_targetbgr)

   LD    BC, #0xFF68
   LD    HL, #_pal_cur
   CALL  set_palette

set_spr:
   LD    A, (_pfs_targetspr)

   LD    BC, #0xFF6A
   LD    HL, #_pal_cur+64

;----------------------------------------------------------------------------
;[[ set_palette() ]]
;----------------------------------------------------------------------------
;parms:
; A = bits of palettes to modify
; BC -> Palette pointer (0xFF68 or 0xFF6A)
; HL -> rgb_data
;value:
; none
;----------------------------------------------------------------------------
set_palette:
   LD    E, #0x80                       ; Incremental pointer

set_pal_loop:
   ADD   A, A
   JR    NC, pal_nomodify

   PUSH  AF
   LD    D, #0x04
pal_loop:
   LDH   A, (P_STAT)
   AND   #0x02
   JR    NZ, pal_loop

   LD    A, E
   LD    (BC), A
   INC   C
   LD    A, (HLI)
   LD    (BC), A
   LD    A, (HLI)
   LD    (BC), A
   DEC   C                             ; next rgb_data
   INC   E                             ; next pointer
   INC   E                             ;
   DEC   D                             ; count down palettes
   JR    NZ, pal_loop
   POP   AF

   JR    NZ, set_pal_loop
pal_nomodify:
   RET   Z

   PUSH  AF
   LD    A, E

   LD    DE, #0x0008
   ADD   HL, DE

   ADD   A, E
   LD    E, A
   POP   AF

   JR    set_pal_loop

;............................................................................

;----------------------------------------------------------------------------
;[[ pfs_setup_asm ]]
;----------------------------------------------------------------------------
; BC -> source
; DE -> dest
; A = fadelength
;----------------------------------------------------------------------------
_pfs_setup_asm:
   PUSH  BC
   JR    _pfs_join

;----------------------------------------------------------------------------
;[[ pfs_setup(uword *source, uword *dest, ubyte fadelength) ]]
;----------------------------------------------------------------------------
; Split 2- 64 entry 16-bit palettes into 32 entry 3- 5-bit RGB entries
; Set up bresenham's structures for the fade
;
;Input:
; source = pointer to source 16-bit palette
; dest   = pointer to destination 16-bit palette
;----------------------------------------------------------------------------
_pfs_setup::
   PUSH  BC

   LDHL  SP, #4                        ; HL->first parameter

   LD    A, (HLI)                      ;
   LD    C, A                          ;
   LD    A, (HLI)                      ;
   LD    B, A                          ; BC=source

   LD    A, (HLI)                      ;
   LD    E, A                          ;
   LD    A, (HLI)                      ;
   LD    D, A                          ; DE=dest

   LD    A, (HL)                       ; A=fadelength

_pfs_join:
   LD    (_pfs_fadelen), A             ; save fade length

   LDH   A, (_rampage)
   PUSH  AF
   LD    A, #_ram_page_pfs
   LDH   (_rampage), A
   LDH   (P_SVBK), A                   ; Map D000-DFFF to ram page

   PUSH  BC                            ; save dest

   LD    HL, #_pfs_rgb_dst             ; HL->split table
   CALL  _pfs_split_rgb

   POP   DE                            ; DE=dest

   LD    HL, #_pfs_rgb_src             ; HL->split table
   CALL  _pfs_split_rgb

   LD    BC, #_pfs_rgb_src
   LD    DE, #_pfs_rgb_dst
   LD    HL, #_pfs_bresen

   LD    A, #192
   CALL  _pfs_make_bresen

   POP   AF
   LDH   (_rampage), A
   LDH   (P_SVBK), A                   ; Map D000-DFFF to ram page

   POP   BC
   RET

;----------------------------------------------------------------------------
;[ pfs_split_rgb ]
;----------------------------------------------------------------------------
;parm:
; DE -> 16-bit RGB (64 entries)
; HL -> 5-bit RGB * 3
;value:
; none
;----------------------------------------------------------------------------
_pfs_split_rgb:
   PUSH  HL
   PUSH  DE

   LD    HL, #_pal_cur
   LD    DE, #_pal_bak
   LD    B, #128
   CALL  _LDIR_B

   POP   DE

   LD    HL, #_pfs_targetbgr           ; BC=target palettes
   LD    A, (HLI)                      ;
   LD    C, (HL)                       ;
   LD    B, A                          ;

   LD    HL, #_pal_bak                 ; HL=pal_bak, DE=source
   CALL  _pfs_copy_pal

   LD    DE, #_pal_bak
   POP   HL

   LD    B, #64                        ; B = # of 16-bit palettes to split

;   CALL  _getptrdat
;   JR    Z, _pfs_split_de

_pfs_split_loop:
   LD    A, (DE)                       ; A = GGGRRRRR
   LD    C, A                          ; save a in c

   AND   #0b00011111                   ; get the R mask
   LD    (HLI), A                      ; save it

   XOR   C                             ; get lower 3 bits of G
   LD    C, A                          ; save it

   INC   DE                            ; point to next source

   LD    A, (DE)                       ; A = ?BBBBBGG
   AND   #0b00000011                   ; get the G mask
   OR    C                             ;
   RLCA                                ;
   RLCA                                ;
   RLCA                                ; shift G into place
   LD    (HLI), A                      ; save it

   LD    A, (DE)                       ; A = ?BBBBBGG
   AND   #0b01111100                   ; get the B mask
   RRCA                                ;
   RRCA                                ; shift B into place
   LD    (HLI), A                      ; save it

   INC   DE

   DEC   B
   JR    NZ, _pfs_split_loop
   RET

;_pfs_split_de:
;   LD    A, E                          ; A = GGGRRRRR
;   LD    C, A                          ; save a in c
;
;   AND   #0b00011111                   ; get the R mask
;   LD    (HLI), A                      ; save it
;
;   XOR   C                             ; get lower 3 bits of G
;   LD    C, A                          ; save it
;
;   LD    A, D                          ; A = ?BBBBBGG
;   AND   #0b00000011                   ; get the G mask
;   OR    C                             ;
;   RLCA                                ;
;   RLCA                                ;
;   RLCA                                ; shift G into place
;   LD    (HLI), A                      ; save it
;
;   LD    A, D                          ; A = ?BBBBBGG
;   AND   #0b01111100                   ; get the B mask
;   RRCA                                ;
;   RRCA                                ; shift B into place
;   LD    (HLI), A                      ; save it
;
;   DEC   B
;   JR    NZ, _pfs_split_de
;   RET

;----------------------------------------------------------------------------
;[ pfs_make_bresen ]
;----------------------------------------------------------------------------
;parm:
; BC -> 5-bit RGB * 3 of source palette
; DE -> 5-bit RGB * 3 of destination palette
; HL -> bresenham structure
; A = number of bytes to setup
;value:
; none
;----------------------------------------------------------------------------
_pfs_make_bresen:                      ; setup bresenham
   PUSH  AF

   PUSH  BC                            ; bc -> starting r g b
   LD    A, (BC)                       ; HL -> struct bresenham
   LD    B, A
   LD    A, (DE)                       ; de -> destination r g b
   SUB   B

   LD    B, A
   ADD   A, A
   SBC   A, A
   LD    C, A
   XOR   B
   SUB   C
   LD    (HLI), A

   LD    A, #-1
   LD    (HLI), A

   SLA   B
   SBC   A, A
   OR    #0x01
   LD    (HLI), A
   POP   BC

   LD    A, (BC)
   LD    (HLI), A

   INC   BC
   INC   DE

   POP   AF
   DEC   A
   JR    NZ, _pfs_make_bresen
   RET

;----------------------------------------------------------------------------
;[[ pfs_step() ]]
;----------------------------------------------------------------------------
; Run through 1 iteration of fade.
;----------------------------------------------------------------------------
_pfs_step::
   PUSH  BC

   LDH   A, (_rampage)
   PUSH  AF
   LD    A, #_ram_page_pfs
   LDH   (_rampage), A
   LDH   (P_SVBK), A                   ; Map D000-DFFF to ram page

   LD    A, (_pfs_fadelen)
   LD    B, A

   LD    HL, #_pfs_bresen
   LD    DE, #_pal_cur

   LD    A, #64
_make_pal_step:
   PUSH  AF
   PUSH  DE
   CALL  Bresenham
   LD    E, A
   CALL  Bresenham
   RRCA
   RRCA
   RRCA
   LD    D, A
   AND   #3
   LD    C, A
   XOR   D
   OR    E
   POP   DE
   LD    (DE), A
   INC   DE
   CALL  Bresenham
   RLCA
   RLCA
   OR    C
   LD    (DE), A
   INC   DE
   POP   AF
   DEC   A
   JR    NZ, _make_pal_step

set_join:
   CALL  set_pals

   POP   AF
   LDH   (_rampage), A
   LDH   (P_SVBK), A                   ; Map D000-DFFF to ram page
   POP   BC
   RET

;----------------------------------------------------------------------------
;[[ pfs_backstep() ]]
;----------------------------------------------------------------------------
; Run through 1 reverse iteration of fade.
;----------------------------------------------------------------------------
_pfs_backstep::
   PUSH  BC

   LDH   A, (_rampage)
   PUSH  AF
   LD    A, #_ram_page_pfs
   LDH   (_rampage), A
   LDH   (P_SVBK), A                   ; Map D000-DFFF to ram page

   LD    A, (_pfs_fadelen)             ; run
   LD    B, A

   LD    HL, #_pfs_bresen
   LD    DE, #_pal_cur

   LD    A, #64
_make_pal_step_back:
   PUSH  AF
   PUSH  DE
   CALL  Bresenham_Back
   LD    E, A
   CALL  Bresenham_Back
   RRCA
   RRCA
   RRCA
   LD    D, A
   AND   #3
   LD    C, A
   XOR   D
   OR    E
   POP   DE
   LD    (DE), A
   INC   DE
   CALL  Bresenham_Back
   RLCA
   RLCA
   OR    C
   LD    (DE), A
   INC   DE
   POP   AF
   DEC   A
   JR    NZ, _make_pal_step_back

   JR    set_join

wingdingpal:                 ; spr6,7
   .DW   0b001011111011100
   .DW   0b000000000000000
   .DW   0b110100000000000
   .DW   0b111111111111111

windowpal:                   ; bgr7
   .DW   0b011110000000000
   .DW   0b000000000000000
   .DW   0b011110111101111
   .DW   0b111111111111111


   .DW   0b000000000000000
   .DW   0b000110100000010
   .DW   0b111110101001001
   .DW   0b111111111111111

   .DW   0b000000000000000
   .DW   0b000110100000010
   .DW   0b110111101111000
   .DW   0b111111111111111

graypal:                     ; graypal
   .DW   0b000000000000000
   .DW   0b010010100101001
   .DW   0b011000110001100
   .DW   0b001000010000100

   .DW   0b000000000000000
   .DW   0b101101010110001
   .DW   0b110101100110111
   .DW   0b100001000101010

_load_graypal::
   LD    HL, #_pfs_targetbgr
   PUSH  HL
   LD    A, (HLI)
   LD    D, (HL)
   LD    E, A
   PUSH  DE
   LD    (HL), #0x02
   DEC   HL
   LD    (HL), #0x00

   LD    A, (__gba)                    ; select gbc/gba palette
   ADD   A, #0xFF                      ;
   SBC   A, A                          ;
   AND   #0x08                         ;
   LD    D, #0x00                      ;
   LD    E, A                          ; DE = 0 or 8

   LD    HL, #graypal
   ADD   HL, DE

   LD    DE, #_pal_cur+64+8*6
   LD    B, #8
   CALL  _LDIR_B

   CALL  set_pals
   POP   DE
   POP   HL
   LD    A, E
   LD    (HLI), A
   LD    (HL), D

   RET

_setwinpal::
   LD    HL, #_pfs_targetbgr
   PUSH  HL
   LD    A, (HLI)
   LD    D, (HL)
   LD    E, A
   PUSH  DE
;   LD    A, (HL)
;   CPL
;   LD    (HLD), A
   LD    (HL), #0x03
   DEC   HL
   LD    (HL), #0x01

   LD    A, (__gba)                    ; select gbc/gba palette
   ADD   A, #0xFF                      ;
   SBC   A, A                          ;
   AND   #0x10                         ;
   LD    D, #0x00                      ;
   LD    E, A                          ; DE = 0 or 16

   PUSH  DE

   LD    HL, #windowpal
   ADD   HL, DE

   LD    DE, #_pal_cur+8*7
   LD    B, #8
   CALL  _LDIR_B

   POP   DE

   LD    HL, #wingdingpal
   ADD   HL, DE

   LD    DE, #_pal_cur+64+8*6
   LD    B, #16
   CALL  _LDIR_B

   CALL  set_pals
   POP   DE
   POP   HL
   LD    A, E
   LD    (HLI), A
   LD    (HL), D

   RET

_setdudepal::
   PUSH  BC
   LDH   A, (_rompage)
   PUSH  AF
   LD    A, #_rom_page_data
   LDH   (_rompage), A
   LD    (0x2000), A

   LD    A, (__gba)                    ; select gbc/gba palette
   ADD   A, #0xFF                      ;
   SBC   A, A                          ;
   AND   #0x40                         ;
   LD    B, #0x00                      ;
   LD    C, A                          ; BC = 0 or 128

   LD    HL, #_dudepal
   ADD   HL, BC

   LD    DE, #_pal_cur+64
   LD    B, #48
   CALL  _LDIR_B

   CALL  set_spr

   POP   AF
   LDH   (_rompage), A
   LD    (0x2000), A
   POP   BC
   RET

;gbaadjust:
;   LD    C, g
;   LD    DE, #0x040E
;
;   r=GetValue(GetValue(4,14,y),GetValue(24,29,y),x)-4;
;   g=GetValue(GetValue(GetValue(4,9,x),GetValue(14,17,x),z),
;     GetValue(GetValue(24,27,x),GetValue(29,30,x),z),y)-4;
;   b=GetValue(GetValue(GetValue(4,9,x),GetValue(14,17,x),y),
;     GetValue(GetValue(24,27,x),GetValue(29,30,x),y),z)-4;
;
;   glib.setrgb(setclr, r * 0x020000 + g * 0x000200 + b * 0x000002);
;
;; DE = min and max
;; C = v
;filter:              ;return min+((max-min)*v*(64-v)/256+1)/4;
;   PUSH  DE
;   LD    A, E
;   SUB   D
;   LD    E, A
;   CALL  .mul8
;   LD    A, 64
;   SUB   C
;   LD    C, A
;   CALL  .mul16
;   LD    A, D
;   INC   A
;   RRCA
;   RRCA
;   AND   #0x1F
;   POP   DE
;   ADD   A, D
;   RET
