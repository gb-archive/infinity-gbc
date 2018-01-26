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
;[ void loadtileset(ubyte x) ]
;----------------------------------------------------------------------------

; A = tileset

_loadtileset::
;   LDHL  SP, #2                        ; HL -> arg

;   LDH   A, (_rompage)                 ; A =prev. rompage
;   PUSH  AF                            ;
;   PUSH  BC                            ;

;   LD    A, (HL)                       ; E =x*4

   ADD   A, A                          ;
   ADD   A, A                          ;
   LD    E, A                          ;

   XOR   A
   LD    (_animate_yes), A             ; turn off animation for now
   LD    (_owe_timer), A               ;

   LD    HL, #_owe_tileset_count
   LD    (HLI), A                      ; get current
   LD    (HLI), A                      ; get current
   LD    (HLI), A                      ; get current
   LD    (HLI), A                      ; get current
   LD    (HLI), A                      ; get current
   LD    (HLI), A                      ; get current
   LD    (HLI), A                      ; get current
   LD    (HLI), A                      ; get current

   LD    HL, #0x2002                   ;

   LD    A, #table_page                ; switch to table page
   LDH   (_rompage), A                 ;
   LD    (HL), A                       ;
   ADD   HL, HL                        ; HL = 4004

   LD    A, (HLI)                      ; (4004)=rom page of tileset start
   LD    B, A                          ; (4005/4006)=offset to tileset table
   LD    A, (HLI)                      ;
   LD    H, (HL)                       ;
   LD    L, A                          ; B =rompage
   SET   6, H                          ; HL=0x4000+offset

   LD    D, #0                         ; DE=x*4
   ADD   HL, DE                        ; HL->tileset entry
   ADD   HL, DE                        ;

   LD    A, (HLI)                      ; A =rompage with tileset
   ADD   A, B                          ;
   LD    E, (HL)                       ; DE=offset within this rompage
   INC   HL                            ;
   LD    D, (HL)                       ;
   INC   HL                            ;
   LD    C, (HL)                       ; C =number of metatiles
   INC   HL                            ;
   LD    B, (HL)                       ; B =number of tiles

   LDH   (_rompage), A                 ; switch to this page
   LD    (0x2000), A                   ;

   SET   6, D                          ; DE->tileset

   LD    A, #1                         ; bank 1
   LDH   (P_VBK), A                    ;

   PUSH  BC                            ;

   PUSH  DE                            ; select gbc/gba palette
   LD    A, (__gba)                    ;
   ADD   A, #0xFF                      ;
   SBC   A, A                          ;
   AND   #0x40                         ;
   LD    H, #0x00                      ;
   LD    L, A                          ;
   ADD   HL, DE                        ;
   LD    D, H                          ;
   LD    E, L                          ;

   LD    HL, #_pal                     ; 64 bytes to pal
   LD    B, #64                        ;
strange_LDIR:
   LD    A, (DE)                       ;
   LD    (HLI), A                      ;
   INC   DE                            ;
   DEC   B                             ;
   JR    NZ, strange_LDIR              ;
   POP   DE                            ;

   LD    HL, #144                      ; skip past gbc pal/gba pal/animation
   ADD   HL, DE                        ;
   LD    D, H                          ;
   LD    E, L                          ;

   LD    H, #0                         ; DE->tileset+64+metatiles*8
   LD    L, C                          ;
   ADD   HL, HL                        ;
   ADD   HL, HL                        ;
   ADD   HL, HL                        ;
   ADD   HL, DE                        ;
   LD    D, H                          ;
   LD    E, L                          ;
   POP   BC                            ; B=num of tiles

   LD    L, #0                         ;

strange_loadloop:                      ;
   PUSH  HL                            ;
   CALL  strange_getadr                ;
   LD    C, #16                        ;
strange_loop:
   LD    A, (DE)                       ;
   CALL  ld_vram_a_acc                 ;
   INC   DE                            ;
   DEC   C                             ;
   JR    NZ, strange_loop              ;
   POP   HL                            ;
   INC   L                             ;
   DEC   B                             ;
   JR    NZ, strange_loadloop          ;

   LD    A, L
   OR    A
   JR    Z, strange_noblacktiles

strange_fillloop:                      ;
   PUSH  HL                            ;
   CALL  strange_getadr                ;
   XOR   A                             ;
   LD    C, #16                        ;
strange_loop2:                         ;
   RST   ldi_v_a                       ;
   DEC   C                             ;
   JR    NZ, strange_loop2             ;
   POP   HL                            ;
   INC   L                             ;
   JR    NZ, strange_fillloop          ;

strange_noblacktiles:
   XOR   A                             ;
   LDH   (P_VBK), A                    ; bank 0

;   POP   BC                            ;
;   POP   AF                            ;
;   LDH   (_rompage), A                 ;
;   LD    (0x2000), A                   ;

   RET

; HL=(signed)L*16+0x9000

; L=tilenum
strange_getadr:
   LD    A, L
   ADD   A, A
   SBC   A, A
   ADD   A, #0x09
   LD    H, A
   ADD   HL, HL
   ADD   HL, HL
   ADD   HL, HL
   ADD   HL, HL                        ; HL -> destination of tile
   RET

_animation::
   LD    A, (_animate_yes)
   OR    A
   RET   Z

   LDH   A, (_rompage)
   PUSH  AF
   LD    A, (_owe_tileset_rompage)
   LDH   (_rompage), A
   LD    (0x2000), A

   LD    HL, #_owe_timer
   INC   (HL)
   RES   3, (HL)

   LD    D, #0
   LD    E, (HL)

   LD    HL, #_owe_tileset_animation
   LD    A, (HLI)
   LD    H, (HL)
   LD    L, A

   ADD   HL, DE
   LD    A, (HL)                       ; count
   OR    A
   POP   BC
   LD    A, B
   LDH   (_rompage), A
   LD    (0x2000), A
   RET   Z

   PUSH  BC
   LD    BC, #8

   LD    A, (_owe_tileset_rompage)
   LDH   (_rompage), A
   LD    (0x2000), A

   LD    A, (HL)                       ; count
   ADD   HL, BC
   LD    B, (HL)                       ; B = index
   LD    C, A                          ; C = count

   LD    HL, #_owe_tileset_count
   ADD   HL, DE
   LD    A, (HL)                       ; get current
   INC   A

   CP    C
   LD    C, A
   SBC   A, A
   AND   C
   LD    (HL), A

   ADD   A, B
   SWAP  A
   LD    B, A
   AND   #0xF0
   LD    C, A
   XOR   B
   LD    B, A

   PUSH  DE

   LD    HL, #_owe_tileset_tiles
   LD    A, (HLI)
   LD    H, (HL)
   LD    L, A
   ADD   HL, BC
   LD    D, H
   LD    E, L

   POP   HL
   CALL  strange_getadr

   LDH   A, (P_VBK)
   PUSH  AF
   LD    A, #1
   LDH   (P_VBK), A                    ; bank 1

   LD    C, #16                        ;
strange_loop3:
   LD    A, (DE)                       ;
   CALL  ld_vram_a_acc                 ;
   INC   DE                            ;
   DEC   C                             ;
   JR    NZ, strange_loop3             ;

   POP   AF
   LDH   (P_VBK), A                    ; bank x

   POP   AF
   LDH   (_rompage), A
   LD    (0x2000), A

   RET
