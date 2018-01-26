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

_ram_page_owe           = 6            ; reserve from D8 - DF

.area _BSS

_timer_mask::
   .ds   1

_owe_tileset_rompage::
   .ds   1
_owe_tileset_animation::
   .ds   2
_owe_tileset_pointer::
   .ds   2
_owe_tileset_tiles::
   .ds   2
_owe_tileset_count::
   .ds   8

_owe_timer::
   .ds   1

_owe_map_rompage::
   .ds   1
_owe_mode::
   .ds   1

_owe_counter::
   .ds   1
_owe_sx::
   .ds   1
_owe_sy::
   .ds   1
_owe_mask::
   .ds   1
_owe_status::
   .ds   1
_owe_destx::
   .ds   2
_owe_scrollx::
   .ds   2
_owe_desty::
   .ds   2
_owe_scrolly::
   .ds   2

map_at_number::
   .ds   1

_owe_spritearray        = 0xD800
; active     - 1 byte
; x          - 2 bytes
; z          - 1 byte
; y          - 2 bytes
; frames     - 2 bytes
; frames_bak - 2 bytes * 20 entries

_owe_neg_scrollx        = 0xD8D0
_owe_neg_scrolly        = 0xD8D2
_owe_sort_table_ptr     = 0xD8D4

_owe_sort_table         = 0xD8D8

; D884 - D8FF = free

;_owe_overlayarray       = 0xD900
;; active     - 1 byte
;; x          - 2 bytes
;; y          - 2 bytes
;
;_owe_spr_presort_page   = 0xDA

_owe_spr_presort_page   = 0xD9

.area _CODE

_map_order_table:
   .DB   1,3,4,7

map_set_vectors:
   .DW   map_set_64x64
   .DW   map_set_32x128
   .DW   map_set_128x32
   .DW   map_set_128x128

; return tile pointer at coordinate (B,C) in ram 64x64 map, A=rampage
; destroys A B H L
map_set_64x64:
   LD    A, B                          ; B = B & 63
   ADD   #0xC0                         ;
   RET   C                             ;
   AND   #0x3F                         ;
   LD    B, A                          ;

   LD    A, C                          ; A = C & 63
   ADD   #0xC0                         ;
   RET   C                             ;
   AND   #0x3F                         ;

map_addr_64x64:
   RRCA                                ; HL = &map[A * 64 + B]
   RRCA                                ;
   LD    L, A                          ;
   AND   #0x3F                         ;
   LD    H, A                          ;
   XOR   L                             ;
   ADD   A, B                          ;
   LD    L, A                          ;
   ADC   A, H                          ;
   SUB   L                             ;
   ADD   A, #0xD0                      ;
   LD    H, A                          ;

   LD    A, #1                         ;
   RET

; return tile pointer at coordinate (B,C) in ram 128x32 map, A=rampage
; destroys A B H L
map_set_128x32:
   LD    A, B                          ; B = B & 127
   ADD   #0x80                         ;
   RET   C                             ;
   AND   #0x7F                         ;
   LD    B, A                          ;

   LD    A, C                          ; A = C & 31
   ADD   #0xE0                         ;
   RET   C                             ;
   AND   #0x1F                         ;

map_addr_128x32:
   RRCA                                ; HL = &map[A * 128 + B]
   LD    L, A                          ;
   AND   #0x7F                         ;
   LD    H, A                          ;
   XOR   L                             ;
   ADD   A, B                          ;
   LD    L, A                          ;
   ADC   A, H                          ;
   SUB   L                             ;
   ADD   A, #0xD0                      ;
   LD    H, A                          ;

   LD    A, #1                         ;
   RET

; return tile pointer at coordinate (B,C) in ram 32x128 map, A=rampage
; destroys A B H L
map_set_32x128:
   LD    A, B                          ; B = B & 31
   ADD   #0xE0                         ;
   RET   C                             ;
   AND   #0x1F                         ;
   LD    B, A                          ;

   LD    A, C                          ; A = C & 127
   ADD   #0x80                         ;
   RET   C                             ;
   AND   #0x7F                         ;

map_addr_32x128:
   RRCA                                ; HL = &map[A * 32 + B]
   RRCA                                ;
   RRCA                                ;
   LD    L, A                          ;
   AND   #0x1F                         ;
   LD    H, A                          ;
   XOR   L                             ;
   ADD   A, B                          ;
   LD    L, A                          ;
   ADC   A, H                          ;
   SUB   L                             ;
   ADD   A, #0xD0                      ;
   LD    H, A                          ;

   LD    A, #1
   RET

; return tile pointer at coordinate (B,C) in rom 128x128 map, A=rampage
; destroys A B H L
map_set_128x128:
   LD    A, B                          ; B = B & 127
   ADD   #0x80                         ;
   RET   C                             ;
   AND   #0x7F                         ;
   LD    B, A                          ;

   LD    A, C                          ;
   ADD   #0x80                         ;
   RET   C                             ;

map_addr_128x128:
   LD    A, C                          ; A = which page??
   RRCA                                ;
   SWAP  A                             ;
   AND   #0x03                         ;

   LD    HL, #_map_order_table         ;
   ADD   A, L                          ;
   LD    L, A                          ;
   ADC   A, H                          ;
   SUB   L                             ;
   LD    H, A                          ;

   XOR   A
   LD    A, (HL)                       ;

   PUSH  AF                            ;

   LD    A, C                          ; A = C & 31
   AND   #0x1F                         ;
   RRCA                                ; HL = &map[A * 128 + B]
   LD    L, A                          ;
   AND   #0x7F                         ;
   LD    H, A                          ;
   XOR   L                             ;
   ADD   A, B                          ;
   LD    L, A                          ;
   ADC   A, H                          ;
   SUB   L                             ;
   ADD   A, #0xD0                      ;
   LD    H, A                          ;

   POP   AF

   RET

;----------------------------------------------------------------------------
;[ void map_settype(ubyte x,ubyte y,ubyte type) ]
;----------------------------------------------------------------------------
_map_settype::
   LDHL  SP, #2

   LDH   A, (_rampage)                 ;
   PUSH  AF                            ;
   PUSH  BC

   LD    A, (HLI)
   LD    B, A
   LD    A, (HLI)
   LD    C, A
   LD    D, (HL)

   PUSH  BC

;   LD    HL, #map_set_vectors
   CALL  map_set_get_addr

   LDH   (_rampage), A
   LDH   (P_SVBK), A

   LD    (HL), D

   POP   DE

   CALL  Update_VRAM

   POP   BC
   POP   AF                            ;
   LDH   (_rampage), A                 ;
   LDH   (P_SVBK), A                   ;

   RET

;----------------------------------------------------------------------------
;[ ubyte map_gettype(ubyte x,ubyte y) ]
;----------------------------------------------------------------------------
_map_gettype::
   LDHL  SP, #2

   LDH   A, (_rampage)                 ;
   PUSH  AF                            ;
   PUSH  BC

   LD    A, (HLI)
   LD    B, A
   LD    A, (HLI)
   LD    C, A

;   LD    HL, #map_set_vectors
   CALL  map_set_get_addr

;   LD    B, B

   LDH   (_rampage), A
   LDH   (P_SVBK), A

   LD    E, (HL)
   JR    NC, _map_gotone
   LD    E, #0
_map_gotone:

   POP   BC
   POP   AF                            ;
   LDH   (_rampage), A                 ;
   LDH   (P_SVBK), A                   ;

   RET

;map_wrap:
;   LD    A, H
;   AND   #0x07
;   CP    #0x04
;   RET

;----------------------------------------------------------------------------
;[ ubyte map_getzone(ubyte x,ubyte y) ]
;----------------------------------------------------------------------------
_map_getzone::
   LDH   A, (_rompage)                 ;
   PUSH  AF                            ;
   LD    A, #_rom_page_zone
;   JR    _map_get_join

;;----------------------------------------------------------------------------
;;[ ubyte map_gettype(ubyte x,ubyte y) ]
;;----------------------------------------------------------------------------
;_map_gettype::
;   LDH   A, (_rompage)                 ;
;   PUSH  AF                            ;
;   LD    A, #_rom_page_world
;_map_get_join:
   LDH   (_rompage), A
   LD    (0x2000), A

   LDHL  SP, #5
   LD    A, (HLD)
   LD    D, (HL)

   RRCA                                ; HL = &map[A * 128 + B]
   LD    L, A                          ;
   AND   #0x7F                         ;
   LD    H, A                          ;
   XOR   L                             ;
   ADD   A, D                          ;
   LD    L, A                          ;
   ADC   A, H                          ;
   SUB   L                             ;
   ADD   A, #0x40                      ;
   LD    H, A                          ;

   LD    E, (HL)                       ; B = map[A * 128 + B]

   POP   AF                            ;
   LDH   (_rompage), A                 ;
   LD    (0x2000), A                   ;

   RET


; BC = dest, DE = source
map_signed_cp::
   LD    A, H
   CP    #128
   RET

;   LD    B, B
;   LD    A, H
;   AND   #0x07
;   CP    #0x04
;   RET

;   LD    A, B                          ; if B==-1 && D==0 then go -
;   SUB   D                             ; if B==127 && D==128 then go +
;   JR    Z, still_cmp                  ;
;   SUB   #128                          ;
;   RET
;still_cmp:
;   LD    A, C                          ;
;   SUB   E                             ;
;   RET

;; DE & BC
;map_signed_cp:
;   LD    A, C                          ; A = (scrolly < desty)
;   SUB   E                             ; (signed comparison)
;   LD    A, B                          ;
;   LD    B, D                          ;
;   RL    B                             ;
;   CCF                                 ;
;   RR    B                             ;
;   RLA                                 ;
;   CCF                                 ;
;   RRA                                 ;
;   SBC   A, B                          ;
;   RET

_owe_deinit::
   XOR   A
   LDH   (_vbimode), A
   RET

_owe_off2::
   LDH   A, (_hbimode)
   CP    #8
   RET   Z
   XOR   A
   LDH   (_hbimode), A
   RET

_owe_on::
   LD    A, (_owe_mode)
   OR    A
   JR    Z, _owe_mode_normal
   LD    A, #1
   LDH   (P_LYC), A
   LD    A, #6
   LDH   (_hbimode), A
   JR    _owe_mode_join
_owe_mode_normal:
   LD    A, #1
_owe_mode_join:
   LDH   (_vbimode), A

   LD    (_owe_status), A

   RET

;----------------------------------------------------------------------------
;[[ ubyte _owe_panspeed(ubyte speed) ]]
;
;0 = every other frame, 1 pix
;1 = every frame, 1 pix
;2 = every frame, 2 pix, etc
;----------------------------------------------------------------------------
_owe_panspeed::
   LDHL  SP, #2
   LD    A, (HL)
_owe_panspeed_asm:
   LD    L, A
   XOR   A
   LD    (_timer_mask), A
   OR    L
   CALL  Z, pan_special
   LDH   (_scr_pos), A
   CPL
   INC   A
   LDH   (_scr_neg), A
   RET

pan_special:
   LD    A, #1
   LD    (_timer_mask), A
   RET

_owe_dscroll_on::
   LD    A, #1
   LD    (_owe_panmode), A
   RET

_owe_dscroll_off::
   XOR   A
   LD    (_owe_panmode), A
   RET

;map_at::
;   LD    HL, #map_at_vectors
map_set_get_addr:
   LD    HL, #map_set_vectors
   LD    A, (map_at_number)
   AND   #0x03
   ADD   A, A
   ADD   A, L
   LD    L, A
   ADC   A, H
   SUB   L
   LD    H, A
   LD    A, (HLI)
   LD    H, (HL)
   LD    L, A
   JP    (HL)

;----------------------------------------------------------------------------
;[[ ubyte _owe_clip_exit(ubyte x, ubyte y, ubyte dir) ]]
;----------------------------------------------------------------------------
;_owe_clip_exit::
;   LDH   A, (_rompage)
;   PUSH  AF
;   PUSH  BC
;
;   LDHL  SP, #6
;   LD    A, (HLI)
;   LD    B, A
;   LD    A, (HLI)
;   LD    C, A
;   LD    A, (HLI)
;   ADD   A, #4
;   LD    E, A
;
;   CALL  map_at
;
;   LD    HL, #_owe_tileset_pointer     ;
;   LD    A, (HLI)                      ;
;   ADD   A, E                          ;
;   LD    E, A                          ;
;   ADC   A, (HL)                       ;
;   SUB   E                             ;
;   LD    H, A                          ;
;   LD    L, E                          ;
;
;   LD    A, (_owe_tileset_rompage)     ;
;   LDH   (_rompage), A
;   LD    (0x2000), A                   ;
;
;   LD    A, B
;   RLCA
;   RLCA
;   RLCA
;   LD    E, A
;   AND   #0x07
;   LD    D, A
;   XOR   E
;   LD    E, A
;   ADD   HL, DE
;
;   LD    A, (HL)
;   AND   #0x10
;   LD    E, A
;
;   POP   BC
;   POP   AF
;   LDH   (_rompage), A
;   LD    (0x2000), A
;
;   RET

;----------------------------------------------------------------------------
;[[ ubyte _owe_clip_enter(ubyte x, ubyte y, ubyte dir) ]]
;----------------------------------------------------------------------------
;_owe_clip_enter::
;   LDH   A, (_rompage)
;   PUSH  AF
;   PUSH  BC
;
;   LDHL  SP, #6
;   LD    A, (HLI)
;   LD    B, A
;   LD    A, (HLI)
;   LD    C, A
;   LD    A, (HLI)
;   ADD   A, #4
;   LD    E, A
;
;   CALL  map_at
;
;   LD    HL, #_owe_tileset_pointer     ;
;   LD    A, (HLI)                      ;
;   ADD   A, E                          ;
;   LD    E, A                          ;
;   ADC   A, (HL)                       ;
;   SUB   E                             ;
;   LD    H, A                          ;
;   LD    L, E                          ;
;
;   LD    A, (_owe_tileset_rompage)     ;
;   LDH   (_rompage), A
;   LD    (0x2000), A                   ;
;
;   LD    A, B
;   RLCA
;   RLCA
;   RLCA
;   LD    E, A
;   AND   #0x07
;   LD    D, A
;   XOR   E
;   LD    E, A
;   ADD   HL, DE
;
;   LD    A, (HL)
;   AND   #0x08
;   LD    E, A
;
;   POP   BC
;   POP   AF
;   LDH   (_rompage), A
;   LD    (0x2000), A
;
;   RET

; (B, C) = 16x16 coordinates

;----------------------------------------------------------------------------
;[[ ubyte owe_blocked(ubyte type,ubyte x, ubyte y) ]
;----------------------------------------------------------------------------
_owe_blocked::
   LDH   A, (_rompage)
   PUSH  AF
   PUSH  BC

   LD    A, (_owe_tileset_rompage)     ;
   LDH   (_rompage), A
   LD    (0x2000), A                   ;

   LDHL  SP, #6
   LD    A, (HLI)
   ADD   A, #4
   LD    E, A

   LD    A, (HLI)
   LD    B, A
   LD    C, (HL)

   LD    D, #0x00
   CALL  get_tile_whole

   AND   #0x08
   LD    E, A

;   LD    BC, #0x10FF
;   LD    E, B
;
;   AND   B
;   ADD   A, C
;   RL    E
;   LD    A, (HLI)
;   AND   B
;   ADD   A, C
;   RL    E
;   LD    A, (HLI)
;   AND   B
;   ADD   A, C
;   RL    E
;   LD    A, (HLI)
;   AND   B
;   ADD   A, C
;   RL    E

   POP   BC
   POP   AF
   LDH   (_rompage), A
   LD    (0x2000), A

   RET


move_xy_table:
   .DB   0,-1, 0,1, -1,0, 1,0

move_tile_table:
   .DB   1, 4, 3, 0     ; up
   .DB   2, 4, 3, 0     ; down
   .DB   3, 0, 0, 0     ; left
   .DB   4, 0, 0, 0     ; right

; get_tile_whole -> BC = 16x16 coordinates, DE=0x0004

; BC = 8x8 coordinates, DE = 0x0001
get_tile_BC:
   LD    A, (_mapw)
   ADD   A, A
   JR    C, ok_b
   DEC   A
   LD    H, A

   LD    A, B
   CP    H
   JR    C, ok_b
   LD    B, D
   CP    #254
   JR    NC, ok_b
   LD    B, H

ok_b:
   LD    A, (_maph)
   ADD   A, A
   JR    C, ok_c
   DEC   A
   LD    H, A

   LD    A, C
   CP    H
   JR    C, ok_c
   LD    C, D
   CP    #254
   JR    NC, ok_c
   LD    C, H

ok_c:
   SRL   C                             ; B=tile x, C=tile y
   RL    E                             ; E=4+(y&1)*2+(x&1)
   SRL   B                             ;
   RL    E                             ;

get_tile_whole:
   CALL  map_at                        ; B=tile at (B,C)

   LD    HL, #_owe_tileset_pointer     ;
   LD    A, (HLI)                      ;
   LD    H, (HL)                       ;
   LD    L, A                          ;
   ADD   HL, DE                        ; HL->tiledata[x]

   LD    A, B                          ; DE=B*8
   RLCA                                ;
   RLCA                                ;
   RLCA                                ;
   LD    E, A                          ;
   AND   #0x07                         ;
   LD    D, A                          ;
   XOR   E                             ;
   LD    E, A                          ;
   ADD   HL, DE                        ;

   LD    A, (HLI)                      ; A=tile's extra stats
   RET                                 ;

; A = direction, (B,C) = 8x8 coordinates
find_newdir:
   LD    E, A                          ; E=dir
find_newdir_E:
   LD    D, #0                         ; DE=dir
   PUSH  DE                            ;
   LD    HL, #move_xy_table            ; HL->move_xy_table+dir*2
   ADD   HL, DE                        ;
   ADD   HL, DE                        ;

   LD    A, (HLI)                      ; B=new x/y
   ADD   A, B                          ;
   LD    B, A                          ;

   LD    A, (HL)                       ; C=new x/y
   ADD   A, C                          ;
   LD    C, A                          ;

   LD    E, #1                         ; DE=1

   PUSH  BC                            ; A=tile's extra stats
   CALL  get_tile_BC                   ;
   POP   BC                            ;
   AND   #0x10                         ; is it blocked?

   PUSH  AF                            ;
   LD    DE, #0x0001                   ; DE=1
   INC   B                             ; B=x+1
   CALL  get_tile_BC                   ; A=tile's extra stats
   POP   DE                            ;
   AND   #0x10                         ; is it blocked?

   RLCA                                ;
   OR    D                             ;
   SWAP  A                             ;
   LD    D, A                          ; D=0-3
   POP   HL                            ; HL=dir

   LD    A, L                          ;
   ADD   A, A                          ;
   ADD   A, A                          ;
   ADD   A, D                          ;

   LD    D, #0                         ;
   LD    E, A                          ;

   LD    HL, #move_tile_table          ;
   ADD   HL, DE                        ;
   LD    E, (HL)                       ; return direction
   RET

;----------------------------------------------------------------------------
;[[ ubyte owe_clip(ubyte x, ubyte y, ubyte dir) ]
;----------------------------------------------------------------------------
_owe_clip::
   LDHL  SP, #2                        ; set stack frame

   LDH   A, (_rompage)                 ; save rompage
   PUSH  AF                            ; save registers
   PUSH  BC                            ;

   LD    A, (_owe_tileset_rompage)     ; set tileset rompage
   LDH   (_rompage), A                 ;
   LD    (0x2000), A                   ;

   LD    A, (HLI)                      ; B=x, C=y, A=direction
   LD    B, A                          ;
   LD    A, (HLI)                      ;
   LD    C, A                          ;
   LD    A, (HLI)                      ;

   PUSH  BC                            ;
   CALL  find_newdir                   ;
   POP   BC                            ;
   DEC   E                             ;
   INC   E                             ;
   JR    Z, bad_dir                    ;
   DEC   E                             ;
   CALL  find_newdir_E                 ;

bad_dir:
   POP   BC                            ;
   POP   AF                            ;
   LDH   (_rompage), A                 ;
   LD    (0x2000), A                   ;

   RET                                 ;

;----------------------------------------------------------------------------
;[[ void owe_off() ]]
;----------------------------------------------------------------------------
_owe_off::
   XOR   A
   LD    (_owe_status), A

   RET

_owe_spr_reset::
   LDH   A, (_rampage)
   PUSH  AF
   PUSH  BC
   LD    A, #_ram_page_owe
   LDH   (_rampage), A
   LDH   (P_SVBK), A                   ; Map D000-DFFF to ram page

   LD    B, #20
   LD    A, #0xFF
   LD    HL, #_owe_spritearray + 8     ; HL -> _bspr_16array[slot * 10]
   LD    DE, #8

_owe_reset_loop:
   LD    (HLI), A
   LD    (HLI), A
   ADD   HL, DE
   DEC   B
   JR    NZ, _owe_reset_loop

   POP   BC
   POP   AF
   LDH   (_rampage), A
   LDH   (P_SVBK), A                   ; Map D000-DFFF to ram page
   RET

;----------------------------------------------------------------------------
;[[ void owe_spr(ubyte slot, uword x, uword y, ubyte z, uword frame) ]]
;----------------------------------------------------------------------------
_owe_spr::
   LDHL  SP, #9                        ; DE -> frame
   LD    D, H                          ;
   LD    E, L                          ;

   LDHL  SP, #2                        ; HL -> slot

   LDH   A, (_rampage)
   PUSH  AF
   PUSH  BC                            ; Save BC for GBDK

   LD    A, #_ram_page_owe
   LDH   (_rampage), A
   LDH   (P_SVBK), A                   ; Map D000-DFFF to ram page

   LD    B, (HL)                       ; A = (slot&15) * 10
   LD    A, B                          ; B = high slot
   LD    C, A                          ;
   ADD   A, A                          ;
   ADD   A, A                          ;
   ADD   A, C                          ;
   ADD   A, A                          ;
   PUSH  BC                            ;

   LD    HL, #_owe_spritearray         ; HL -> _bspr_16array[slot * 10]
   ADD   A, L                          ;
   LD    L, A                          ;
   ADC   A, H                          ;
   SUB   L                             ;
   LD    H, A                          ;

   LD    A, (DE)                       ; BC = frame, DE -> z
   LD    B, A                          ;
   DEC   DE                            ;
   LD    A, (DE)                       ;
   LD    C, A                          ;
   DEC   DE                            ;

   CP    B                             ; If BC == -1 sprite is inactive.
   JR    NZ, owe_spr_active            ;
   INC   A                             ;
   JP    Z, owe_spr_inactive           ;

owe_spr_active:
   POP   AF                            ; active = yes
   OR    #0x01                         ;
   LD    (HLI), A                      ;

   LD    A, (DE)                       ; A = z
   PUSH  AF                            ;
   DEC   DE                            ; DE -> y(h)
   DEC   DE                            ; DE -> y(l)
   DEC   DE                            ; DE -> x(h)
   DEC   DE                            ; DE -> x(l)
   LD    A, (DE)                       ; A = x(l)
   LD    (HLI), A                      ; write x(l)
   INC   DE                            ; DE -> x(h)
   LD    A, (DE)                       ; A = x(h)
   LD    (HLI), A                      ; write x(h)
   POP   AF                            ; A = z
   LD    (HLI), A                      ; write z
   INC   DE                            ; DE -> y(l)
   LD    A, (DE)                       ; A = y(l)
   LD    (HLI), A                      ; write y(l)
   INC   DE                            ; DE -> y(h)
   LD    A, (DE)                       ; A = y(h)
   LD    (HLI), A                      ; write y(h)

   LD    A, (HLI)
   LD    E, A
   LD    A, (HLI)
   LD    D, A
   LD    A, (HLI)
   CP    C
   JR    NZ, owe_spr_update
   LD    A, (HL)
   CP    B
   JR    Z, owe_spr_noupdate
owe_spr_update:
   LD    A, D
   LD    (HLD), A
   LD    A, E
   LD    (HLD), A
   LD    A, B
   LD    (HLD), A
   LD    (HL), C

   LDH   A, (_rompage)
   PUSH  AF

   LD    HL, #0x2000

   LD    A, #table_page
   LDH   (_rompage), A
   LD    (HL), A                           ; Map 4000-7FFF to cur_page
   LD    H, #0x40
   LD    A, (HLI)
   LD    D, A
   LD    A, (HLI)
   LD    H, (HL)                       ; D = sprite page
   LD    L, A                          ; HL -> palettes

   SET   6, H
   ADD   HL, BC
   INC   HL
   INC   HL
   INC   HL
   INC   HL
   LD    A, (HL)

   PUSH  AF

   LDHL  SP, #10
   LD    A, (HL)                       ; a = slot

   ADD   A, A                          ; 32 * 4 + 2
   ADD   A, A
   LD    E, A
   ADD   A, A
   ADD   A, #2

   LD    L, A
   LD    H, #_owe_spr_presort_page

   POP   AF

   LD    (HL), E                       ; tile into OAM tile
   INC   L
   LD    (HLI), A                      ; palette inside OAM attrib
   INC   L
   INC   L
   INC   E
   INC   E
   LD    (HL), E
   INC   L
   LD    (HL), A

   LD    A, B
   ADD   A, D
   LDH   (_rompage), A
   LD    (0x2000), A                   ; page # with sprite

   LD    A, C
   RRCA
   RRCA
   LD    B, A
   AND   #0xC0
;   LDH   (0x52), A
   LD    C, A
   XOR   B
   OR    #0x40
;   LDH   (0x51), A
   LD    B, A                          ; BC -> sprite

   LDHL  SP, #8
   LD    A, (HL)                       ; a = slot

   RRCA
   RRCA
   LD    H, A
   AND   #0xC0
;   LDH   (0x54), A
   LD    L, A
   XOR   H
   ADD   A, #0x80
;   LDH   (0x53), A
   LD    H, A                          ; HL -> destination of sprite

   ; tile into sprite data

;   DI
;   LD    A, #0x03                      ; Blast DMA
;   LDH   (0x55), A
;
;   LD    A, #0x00
;owe_write_loop:
;   DEC   A
;   JR    NZ, owe_write_loop
;   EI

   LD    D, #64
owe_spr_write_loop:
   LD    A, (BC)
   INC   BC
   RST   ldi_v_aa
   DEC   D
   JR    NZ, owe_spr_write_loop
   POP   AF

   LDH   (_rompage), A
   LD    (0x2000), A                           ; Map 4000-7FFF to cur_page

owe_spr_noupdate:
   POP   BC
   POP   AF

;   DI
;   LD    HL, #_owe_spritearray         ; HL -> _bspr_16array[slot * 9]
;self4:
;   JR    self4

   LDH   (_rampage), A
   LDH   (P_SVBK), A                   ; Map D000-DFFF to ram page

   RET

owe_spr_inactive:
   XOR   A                             ; active = no
   LD    (HLI), A                      ; HL -> x(l)

   INC   HL                            ; HL -> frames_bak(l)
   INC   HL                            ;
   INC   HL                            ;
   INC   HL                            ;
   INC   HL                            ;

   INC   HL
   INC   HL
   PUSH  HL                            ;
   INC   HL
   LD    A, (HLD)
   DEC   HL
   LD    (HLI), A
   LD    A, (HLD)
   DEC   HL
   LD    (HL), A
   POP   HL

;   PUSH  HL                            ;
;   LD    A, (HLI)                      ; A = bak(l), HL -> bak(h)
;   INC   HL                            ; HL -> frame(l)
;   LD    (HLD), A                      ; write frame(l), HL -> bak(h)
;   LD    A, (HLI)                      ; A = bak(h), HL -> frame(l)
;   INC   HL                            ; HL -> frame(h)
;   LD    (HL), A                       ; write frame(h)
;   POP   HL                            ; HL -> frames_bak(l)

   LD    A, C                           ; frames_bak = -1
   LD    (HLI), A                       ;
   LD    (HL), B                        ;

;   DI
;   LD    HL, #_owe_spritearray         ; HL -> _bspr_16array[slot * 9]
;self3:
;   JR    self3

   POP   BC
   POP   BC
   POP   AF

   LDH   (_rampage), A
   LDH   (P_SVBK), A                   ; Map D000-DFFF to ram page

   RET


;----------------------------------------------------------------------------
;[[ void owe_update(ubyte x, ubyte y) ]]
;----------------------------------------------------------------------------
_owe_update::
   LDHL  SP, #2
   LD    A, (HLI)                      ; D = x
   LD    D, A                          ;
   LD    E, (HL)                       ; E = y

   PUSH  BC
   CALL  Update_VRAM
   POP   BC

   RET

map_at::
   CALL  map_set_get_addr              ; B = map(B,C)
   JR    C, map_ob

   LD    B, A
   LDH   A, (_rampage)
   LD    C, A

   LD    A, B
   LDH   (_rampage), A
   LDH   (P_SVBK), A

   LD    B, (HL)

   LD    A, C
   LDH   (_rampage), A
   LDH   (P_SVBK), A
   RET

map_ob:
   LD    B, #0
   RET   

;; return tile number at coordinate (B,C) in ram 64x64 map B (0 if OB)
;; destroys A B H L
;map_at_64x64:
;   LD    A, B                          ; B = B & 63, if OB return B = 0
;   AND   #0xC0                         ;
;   JR    NZ, map_ob                    ;
;   XOR   B                             ;
;   LD    B, A                          ;
;
;   LD    A, C                          ; A = C & 63, if OB return B = 0
;   AND   #0xC0                         ;
;   JR    NZ, map_ob                    ;
;   XOR   C                             ;
;
;   RRCA                                ; HL = &map[A * 64 + B]
;   RRCA                                ;
;   LD    L, A                          ;
;   AND   #0x3F                         ;
;   LD    H, A                          ;
;   XOR   L                             ;
;   ADD   A, B                          ;
;   LD    L, A                          ;
;   ADC   A, H                          ;
;   SUB   L                             ;
;   ADD   A, #0xD0                      ;
;   LD    H, A                          ;
;
;   LD    B, (HL)                       ; B = map[A * 64 + B]
;   RET

;; return tile number at coordinate (B,C) in ram 128x32 map B (0 if OB)
;; destroys A B H L
;map_at_128x32:
;   LD    A, B                          ; B = B & 127, if OB return B = 0
;   AND   #0x80                         ;
;   JR    NZ, map_ob                    ;
;   XOR   B                             ;
;   LD    B, A                          ;
;
;   LD    A, C                          ; A = C & 31, if OB return B = 0
;   AND   #0xE0                         ;
;   JR    NZ, map_ob                    ;
;   XOR   C                             ;
;
;   RRCA                                ; HL = &map[A * 128 + B]
;   LD    L, A                          ;
;   AND   #0x7F                         ;
;   LD    H, A                          ;
;   XOR   L                             ;
;   ADD   A, B                          ;
;   LD    L, A                          ;
;   ADC   A, H                          ;
;   SUB   L                             ;
;   ADD   A, #0xD0                      ;
;   LD    H, A                          ;
;
;   LD    B, (HL)                       ; B = map[A * 128 + B]
;   RET
;
;; return tile number at coordinate (B,C) in ram 32x128 map B (0 if OB)
;; destroys A B H L
;map_at_32x128:
;   LD    A, B                          ; B = B & 31, if OB return B = 0
;   AND   #0xE0                         ;
;   JR    NZ, map_ob                    ;
;   XOR   B                             ;
;   LD    B, A                          ;
;
;   LD    A, C                          ; A = C & 127, if OB return B = 0
;   AND   #0x80                         ;
;   JR    NZ, map_ob                    ;
;   XOR   C                             ;
;
;   RRCA                                ; HL = &map[A * 32 + B]
;   RRCA                                ;
;   RRCA                                ;
;   LD    L, A                          ;
;   AND   #0x1F                         ;
;   LD    H, A                          ;
;   XOR   L                             ;
;   ADD   A, B                          ;
;   LD    L, A                          ;
;   ADC   A, H                          ;
;   SUB   L                             ;
;   ADD   A, #0xD0                      ;
;   LD    H, A                          ;
;
;   LD    B, (HL)                       ; B = map[A * 32 + B]
;   RET
;
;
;; return tile number at coordinate (B,C) in rom 128x128 map B (wrap if OB)
;; destroys A B H L
;map_at_128x128:
;   LD    A, B                          ; B = B & 127
;   ADD   A, A                          ;
;   JR    C, map_ob                     ;
;
;   LD    A, C                          ; A = C & 127
;   ADD   A, A                          ;
;   JR    C, map_ob                     ;
;   RRCA                                ;
;
;   CALL  map_addr_128x128
;
;   LD    B, A
;   LDH   A, (_rampage)
;   PUSH  AF
;
;   LD    A, B
;   LDH   (_rampage), A
;   LDH   (P_SVBK), A
;
;   LD    B, (HL)
;
;   POP   AF
;   LDH   (_rampage), A
;   LDH   (P_SVBK), A
;
;   RET


;   RRCA                                ; HL = &map[A * 128 + B]
;   LD    L, A                          ;
;   AND   #0x7F                         ;
;   LD    H, A                          ;
;   XOR   L                             ;
;   ADD   A, B                          ;
;   LD    L, A                          ;
;   ADC   A, H                          ;
;   SUB   L                             ;
;   ADD   A, #0x40                      ;
;   LD    H, A                          ;
;
;   LDH   A, (_rompage)                 ;
;   PUSH  AF                            ;
;   LD    A, (_owe_map_rompage)         ;
;   LDH   (_rompage), A                 ;
;   LD    (0x2000), A                   ;
;
;   LD    B, (HL)                       ; B = map[A * 128 + B]
;
;   POP   AF                            ;
;   LDH   (_rompage), A                 ;
;   LD    (0x2000), A                   ;
;
;   RET

; update vram coordinate (D,E)
Update_VRAM::
   LD    HL, #_owe_sx                  ; Map x-coordinate start

   LD    A, D                          ; B = x-coordinate to get from map
   SUB   (HL)                          ;
   AND   #0x0F                         ;
   ADD   A, (HL)                       ;
   LD    B, A                          ;

   INC   HL                            ; Map y-coordinate start

   LD    A, E                          ; A = y-coordinate to get from map
   SUB   (HL)                          ;
   AND   #0x0F                         ;
   ADD   A, (HL)                       ;
   LD    C, A                          ;

   CALL  map_at                        ;

   LD    A, E                          ; HL = &vram[(E&15)*64+(D&15)*2]
   AND   #0x0F                         ;
   RRCA                                ;
   RRCA                                ;
   LD    L, A                          ;
   AND   #0x3F                         ;
   LD    H, A                          ;
   XOR   L                             ;
   LD    L, A                          ;
   LD    A, D                          ;
   AND   #0x0F                         ;
   ADD   A, A                          ;
   ADD   A, L                          ;
   LD    L, A                          ;
   ADC   A, H                          ;
   SUB   L                             ;
   ADD   A, #0x98                      ;
   LD    H, A                          ;

   PUSH  HL

   LD    HL, #_owe_tileset_pointer     ;
   LD    A, (HLI)                      ;
   LD    H, (HL)                       ;
   LD    L, A                          ;

   LD    A, B
   RLCA
   RLCA
   RLCA
   LD    C, A
   AND   #0x07
   LD    B, A
   XOR   C
   ADD   A, L
   LD    C, A
   LD    A, B
   ADC   A, H
   LD    B, A

   POP   HL

   LDH   A, (_rompage)
   PUSH  AF

   LD    A, (_owe_tileset_rompage)     ;
   LDH   (_rompage), A                 ;
   LD    (0x2000), A                   ;

   LD    DE, #0x001E                   ;

   XOR   A                             ;
   LDH   (P_VBK), A                    ;

   PUSH  HL                            ;
   LD    A, (BC)
   RST   ldi_v_a
   INC   BC
   LD    A, (BC)
   RST   ldi_v_a
   ADD   HL, DE                        ;
   INC   BC
   LD    A, (BC)
   RST   ldi_v_a
   INC   BC
   LD    A, (BC)
   RST   ldi_v_a
   INC   BC

   LD    A, #1                         ;
   LDH   (P_VBK), A                    ;

   POP   HL                            ;

   LD    A, (BC)
   OR    #8
   RST   ldi_v_a
   INC   BC
   LD    A, (BC)
   OR    #8
   RST   ldi_v_a
   ADD   HL, DE
   INC   BC
   LD    A, (BC)
   OR    #8
   RST   ldi_v_a
   INC   BC
   LD    A, (BC)
   OR    #8
   RST   ldi_v_a

   XOR   A                             ;
   LDH   (P_VBK), A                    ;

   POP   AF
   LDH   (_rompage), A
   LD    (0x2000), A                   ;

   RET

_init_sxy::
   LD    HL, #_owe_scrollx + 1         ; A = (scrollx) / 16 - 2
   LD    A, (HLD)                      ;
   AND   #0x0F                         ;
   LD    D, A                          ;
   LD    A, (HL)                       ;
   AND   #0xF0                         ;
   OR    D                             ;
   SWAP  A                             ;
   SUB   #2                            ;
   LD    (_owe_sx), A                  ; (sx) = A

   LD    HL, #_owe_scrolly + 1         ; A = (scrolly) / 16 - 3
   LD    A, (HLD)                      ;
   AND   #0x0F                         ;
   LD    D, A                          ;
   LD    A, (HL)                       ;
   AND   #0xF0                         ;
   OR    D                             ;
   SWAP  A                             ;
   SUB   #3                            ;
   LD    (_owe_sy), A                  ; (sy) = A

   RET

;----------------------------------------------------------------------------
;[[ void owe_scrollto(uword x, uword y) ]]
;----------------------------------------------------------------------------
_owe_scrollto::
   LDHL  SP, #2

   LDH   A, (_rampage)
   PUSH  AF

   LD    A, #_ram_page_owe
   LDH   (_rampage), A
   LDH   (P_SVBK), A                   ; Map D000-DFFF to ram page

   LD    A, (HLI)                      ; a = x(l)
   LD    E, (HL)                       ; a = x(h)

   LD    HL, #_owe_destx
   LD    (HLI), A
   LD    (HL), E
;   LD    A, (_owe_mask)
;   AND   E
;   LD    (HL), A

   LDHL  SP, #6
   LD    A, (HLI)                      ; a = y(l)
   LD    E, (HL)                       ; a = y(h)

   LD    HL, #_owe_desty
   LD    (HLI), A
   LD    (HL), E
;   LD    A, (_owe_mask)
;   AND   E
;   LD    (HL), A

   POP   AF
   LDH   (_rampage), A
   LDH   (P_SVBK), A                   ; Map D000-DFFF to ram page

   RET

;map_at_vectors:
;   .DW   map_at_64x64
;   .DW   map_at_32x128
;   .DW   map_at_128x32
;   .DW   map_at_128x128

;map_cp_vectors:
;   .DW   map_signed_cp
;   .DW   map_signed_cp
;   .DW   map_signed_cp
;   .DW   map_signed_cp ;map_wrap

map_masks:
   .DB   #0xFF
   .DB   #0xFF
   .DB   #0xFF
   .DB   #0xFF ; 0x07

;----------------------------------------------------------------------------
;[[ void owe_redraw(uword x, uword y) ]]
;----------------------------------------------------------------------------
_owe_redraw::
   LDHL  SP, #2

   LDH   A, (_rampage)
   PUSH  AF

   LD    A, #_ram_page_owe
   LDH   (_rampage), A
   LDH   (P_SVBK), A                   ; Map D000-DFFF to ram page

   LD    A, (HLI)                      ; a = x(l)
   LD    E, (HL)                       ; a = x(h)

   PUSH  HL

   LD    HL, #_owe_destx
   LD    (HLI), A
   LD    (HL), E
   INC   HL
   LD    (HLI), A
   LD    (HL), E

   POP   HL
   INC   HL

   LD    A, (HLI)                      ; a = y(l)
   LD    E, (HL)                       ; a = y(h)

   LD    HL, #_owe_desty
   LD    (HLI), A
   LD    (HL), E
   INC   HL
   LD    (HLI), A
   LD    (HL), E

   POP   AF

   LDH   (_rampage), A                 ; Map D000-DFFF to map's ram page
   LDH   (P_SVBK), A                   ;

camera_warp_main:
   CALL  _init_sxy

   LDH   A, (P_VBK)
   PUSH  AF

   LD    D, #0x10
owe_redraw_loop1:
   LD    E, #0x10
owe_redraw_loop2:
   PUSH  DE
   CALL  Update_VRAM
   POP   DE
   DEC   E
   JR    NZ, owe_redraw_loop2
   DEC   D
   JR    NZ, owe_redraw_loop1

   POP   AF
   LDH   (P_VBK), A
   RET

;----------------------------------------------------------------------------
;[[ void owe_init(ubyte set, ubyte type, uword x, uword y) ]]
;----------------------------------------------------------------------------
_owe_init::
   LDHL  SP, #2

   LDH   A, (_rampage)
   PUSH  AF
   LD    A, #_ram_page_owe
   LDH   (_rampage), A
   LDH   (P_SVBK), A                   ; Map D000-DFFF to ram page

;   LD    A, #0xC3
;   LD    (map_at), A

   LD    A, (HLI)    ; A=set
   PUSH  AF

   LD    A, (HLI)    ; A=type
   AND   #3

   LD    (map_at_number), A
   SUB   #3
   JR    Z, _use_owe2
   XOR   A
   LD    (_owe_mode), A
   CALL  _owe_off2
   ;LDH   (_hbimode), A
   INC   A
   JR    _switch_owe
_use_owe2:
   INC   A
   LDH   (P_LYC), A
   LD    (_owe_mode), A
   LD    A, #6
   LDH   (_hbimode), A
_switch_owe:
   LDH   (_vbimode), A

;   PUSH  AF
;   LD    DE, #map_masks
;   ADD   A, E
;   LD    E, A
;   ADC   A, D
;   SUB   E
;   LD    D, A
;   LD    A, (DE)
;   LD    (_owe_mask), A
;   POP   AF

   LD    A, (HLI)                      ; a = x(l)
   LD    E, (HL)                       ; a = x(h)

   PUSH  HL

   LD    HL, #_owe_destx
   LD    (HLI), A
   LD    (HL), E
   INC   HL
   LD    (HLI), A
   LD    (HL), E

   POP   HL
   INC   HL

   LD    A, (HLI)                      ; a = y(l)
   LD    E, (HL)                       ; a = y(h)

   LD    HL, #_owe_desty
   LD    (HLI), A
   LD    (HL), E
   INC   HL
   LD    (HLI), A
   LD    (HL), E

   XOR   A
   LD    D, #0xFF

   LD    HL, #_owe_spritearray
   LD    E, #20
owe_16fill:
   LD    (HLI), A                      ; active = no, HL -> x(l)
   INC   HL                            ; HL -> x(h)
   INC   HL                            ; HL -> z
   INC   HL                            ; HL -> y(l)
   INC   HL                            ; HL -> y(h)
   INC   HL                            ; HL -> frames_bak
   INC   HL                            ;
   INC   HL                            ; HL -> frames
   LD    (HL), D                       ; frames = -1
   INC   HL                            ;
   LD    (HL), D                       ;
   INC   HL                            ;
   DEC   E
   JR    NZ, owe_16fill

   POP   AF

   RLCA
   RLCA
   RLCA
   LD    E, A
   AND   #0x07
   LD    D, A
   XOR   E
   LD    E, A

   POP   AF
   LDH   (_rampage), A                 ; Map D000-DFFF to map's ram page
   LDH   (P_SVBK), A                   ;

   LDH   A, (_rompage)
   PUSH  AF
   PUSH  BC

   LD    HL, #0x2004

   LD    A, #table_page                ;
   LDH   (_rompage), A                 ;
   LD    (HL), A                       ;
   ADD   HL, HL                        ;
   LD    A, (HLD)                      ; start of maps
   LD    (_owe_map_rompage), A         ;
   DEC   HL                            ;
   DEC   HL                            ;
   DEC   HL                            ;
   LD    A, (HLI)                      ;
   LD    C, A                          ; HL -> start of tile sets
   LD    A, (HLI)                      ;
   LD    H, (HL)                       ;
   LD    L, A                          ;
   SET   6, H                          ; HL -> tileset table

   ADD   HL, DE                        ; offset to right entry

   LD    A, (HLI)                      ; relative page number of tile set
   ADD   A, C                          ;
   LD    (_owe_tileset_rompage), A     ;
   LD    A, (HLI)                      ;
   INC   HL                            ;
   LD    C, (HL)                       ; C = number of metatiles
   DEC   HL                            ;
   LD    H, (HL)                       ;
   LD    L, A                          ; HL -> tileset
   SET   6, H

   LD    A, #128
   ADD   A, L
   LD    E, A
   ADC   A, H
   SUB   E
   LD    D, A

   LD    HL, #_owe_tileset_animation
   LD    A, E
   LD    (HLI), A
   LD    A, D
   LD    (HLI), A

   LD    A, #1
   LD    (_animate_yes), A             ; turn off animation for now

   LD    A, #16                        ; owe_tileset_pointer
   ADD   A, E
   LD    E, A
   ADC   A, D
   SUB   E
   LD    D, A

;   LD    B, B

   LD    A, E
   LD    (HLI), A
   LD    A, D
   LD    (HLI), A

   LD    A, C
   RLCA
   RLCA
   RLCA
   LD    C, A
   AND   #0x07
   LD    B, A
   XOR   C
   ADD   A, E
   LD    C, A
   LD    A, B
   ADC   A, D
   LD    B, A

   LD    A, C
   LD    (HLI), A
   LD    (HL), B

   CALL  camera_warp_main

   POP   BC
   POP   AF
   LDH   (_rompage), A
   LD    (0x2000), A

   LD    A, #1
   LD    (_owe_status), A

   RET

_rotate_r::
;   PUSH  BC
;   LD    HL, #_owe_pantile       ;0x9000
;   LD    DE, #_owe_pantile+0x20  ;0x9020
;   LD    C, #0x20
;
;_rr_loop:
;   LD    A, (HL)
;   RRA
;   LD    A, (DE)
;   RRA
;   LD    (DE), A
;   RR    (HL)
;   INC   HL
;   INC   DE
;   DEC   C
;   JR    NZ, _rr_loop
;
;   POP   BC
   RET

_rotate_l::
;   PUSH  BC
;   LD    HL, #_owe_pantile       ;0x9000
;   LD    DE, #_owe_pantile+0x20  ;0x9020
;   LD    C, #0x20
;
;_rl_loop:
;   LD    A, (HL)
;   RLA
;   LD    A, (DE)
;   RLA
;   LD    (DE), A
;   RL    (HL)
;   INC   HL
;   INC   DE
;   DEC   C
;   JR    NZ, _rl_loop
;
;   POP   BC
   RET

_rotate_u::
;   PUSH  BC
;   LD    HL, #_owe_pantile       ;0x9000
;   LD    DE, #_owe_pantile+0x02  ;0x9002
;   LD    B, #2
;_ru_loop2:
;   LD    C, #0x1E
;
;   INC   HL
;   LD    A, (HLD)    ; byte 1
;   PUSH  AF
;   LD    A, (HL)     ; byte 0
;   PUSH  AF
;_ru_loop:
;   LD    A, (DE)
;   LD    (HLI), A
;   INC   DE
;   DEC   C
;   JR    NZ, _ru_loop
;   POP   AF
;   LD    (HLI), A
;   POP   AF
;   LD    (HLI), A
;   INC   DE
;   INC   DE
;
;   DEC   B
;   JR    NZ, _ru_loop2
;
;   POP   BC
   RET

_rotate_d::
;   PUSH  BC
;   LD    HL, #_owe_pantile+0x3F  ;0x903F
;   LD    DE, #_owe_pantile+0x3D  ;0x903D
;   LD    B, #2
;_rd_loop2:
;   LD    C, #0x1E
;
;   DEC   HL
;   LD    A, (HLI)    ; byte 1
;   PUSH  AF
;   LD    A, (HL)     ; byte 0
;   PUSH  AF
;_rd_loop:
;   LD    A, (DE)
;   LD    (HLD), A
;   DEC   DE
;   DEC   C
;   JR    NZ, _rd_loop
;   POP   AF
;   LD    (HLD), A
;   POP   AF
;   LD    (HLD), A
;   DEC   DE
;   DEC   DE
;
;   DEC   B
;   JR    NZ, _rd_loop2
;
;   POP   BC
   RET

_int_owe::
   LDH   A, (_rompage)
   PUSH  AF

   LD    A, #_rom_page_title
   LDH   (_rompage), A
   LD    (0x2000), A

   CALL  _int_owe_real

   POP   AF
   LDH   (_rompage), A
   LD    (0x2000), A

   JP    _vbl_end
