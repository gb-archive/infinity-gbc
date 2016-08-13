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

.include "gbasm.h"

bspr_rampage            = 6 ; reserve D0 - D7

.area _BSS

_bicon_expanding::
   .DS   1
_bicon_mode::
   .DS   1
_bicon_addx::
   .DS   1
_bicon_count::
   .DS   1
_bicon_x::
   .DS   1
_bicon_exp_dx::
   .DS   1
_bicon_exp::
   .DS   1
_bicon_exp_ct::
   .DS   1

bicon_set_frame::
   .DS   1
_bicon_icon::
   .DS   1
_bicon_icon2::
   .DS   1
_bicon_iconxy::
   .DS   2

_spr64_cmpline::
 .ds 1
_spr64_table::
 .ds 1
_bspr_state::
 .ds 1
_bspr_quake::
 .ds 1
_spr64_timer::
 .ds 1

bspr_spr_on::
 .ds 1

_bspr_window::
 .ds 1

_bspr_spr64table        = 0xD000
_bspr_spr64table_page   = 0xD0
_bspr_16array           = 0xD100
; active     - 1 byte
; x          - 2 bytes
; z          - 1 byte
; y          - 2 bytes
; frames_bak - 2 bytes
; frames     - 2 bytes * 16 entries

_bspr_destx             = 0xD1A0
_bspr_scrollx           = 0xD1A2
_bspr_desty             = 0xD1A4
_bspr_scrolly           = 0xD1A6
_bspr_negscrollx16      = 0xD1A8
_bspr_negscrolly16      = 0xD1AA
_bspr_negscrollx8       = 0xD1AC
_bspr_negscrolly8       = 0xD1AE

_sort_table_ptr         = 0xD1B0
_sort_table             = 0xD1E0

; D1B2 - D1DF = free

_bspr_8array            = 0xD200
; active     - 1 byte
; x          - 2 bytes
; y          - 2 bytes * 32 entries

; D2A0 - D2FF = free

_bspr_spr64split1       = 0xC000
_bspr_spr64split1_page  = 0xC0
_bspr_spr64split2       = 0xD300
_bspr_spr64split2_page  = 0xD3
_bspr_spr64split2b      = 0xD400
_bspr_spr64split2b_page = 0xD4

_presort_table_page   = 0xD5

.globl _bspr_int_real

.area _CODE

;[[ void bspr_quake_on() ]]
_bspr_quake_on::
   LD    A, #1
   LD    (_bspr_quake), A
   RET

;[[ void bspr_quake_off() ]]
_bspr_quake_off::
   XOR   A
   LD    (_bspr_quake), A
   RET

;----------------------------------------------------------------------------
;[[ void bspr_scrollto(uword x, uword y) ]]
;----------------------------------------------------------------------------
_bspr_scrollto::
   LDH   A, (_rampage)
   PUSH  AF
   LD    A, #bspr_rampage
   LDH   (_rampage), A
   LDH   (P_SVBK), A                   ; Map D000-DFFF to ram page

   LDHL  SP, #4
   LD    A, (HLI)                      ; a = x(l)
   LD    E, (HL)                       ; a = x(h)

   LD    HL, #_bspr_destx
   LD    (HLI), A
   LD    (HL), E

   LDHL  SP, #6
   LD    A, (HLI)                      ; a = y(l)
   LD    E, (HL)                       ; a = y(h)

   LD    HL, #_bspr_desty
   LD    (HLI), A
   LD    (HL), E

;   DI
;self:
;   JR    self

   POP   AF
   LDH   (_rampage), A
   LDH   (P_SVBK), A                   ; Map D000-DFFF to ram page

   RET

;----------------------------------------------------------------------------
;[[ void bspr_scrollset(uword x, uword y) ]]
;----------------------------------------------------------------------------
_bspr_scrollset::
   LDH   A, (_rampage)
   PUSH  AF
   LD    A, #bspr_rampage
   LDH   (_rampage), A
   LDH   (P_SVBK), A                   ; Map D000-DFFF to ram page

   LDHL  SP, #4
   LD    A, (HLI)                      ; a = x(l)
   LD    E, (HL)                       ; a = x(h)

   LD    HL, #_bspr_scrollx

   LD    (HLI), A
   LD    (HL), E

   LDHL  SP, #6
   LD    A, (HLI)                      ; a = y(l)
   LD    E, (HL)                       ; a = y(h)

   LD    HL, #_bspr_scrolly
   LD    (HLI), A
   LD    (HL), E

   POP   AF
   LDH   (_rampage), A
   LDH   (P_SVBK), A                   ; Map D000-DFFF to ram page

   RET

;----------------------------------------------------------------------------
;[[ void bspr_off() ]]
;----------------------------------------------------------------------------
_bspr_off::
   XOR   A
   LDH   (_hbimode), A
   LDH   (_vbimode), A
;   LD    (_bspr_state), A

   LD    HL, #0xFF40
   SET   0, (HL)
   RET

;----------------------------------------------------------------------------
;[[ void bspr_init() ]]
;----------------------------------------------------------------------------
_bspr_init::
   LD    A, #_bspr_spr64split2_page
   LD    (_spr64_table), A

   LD    A, #200
   LD    (_bspr_window), A

   LDH   A, (_rampage)
   PUSH  AF
   LD    A, #bspr_rampage
   LDH   (_rampage), A
   LDH   (P_SVBK), A                   ; Map D000-DFFF to ram page

   XOR   A
   LD    (_bspr_quake), A
   LD    (_bicon_mode), A
   LD    (bicon_set_frame), A
   LD    (_bicon_icon), A
   LD    (_bicon_icon2), A

   LD    D, #0xFF

   LD    HL, #_bspr_16array
   LD    E, #16
bspr_16fill:
   LD    (HLI), A                      ; active = no, HL -> x(l)
   INC   HL                            ; HL -> x(h)
   INC   HL                            ; HL -> z
   INC   HL                            ; HL -> y(l)
   INC   HL                            ; HL -> y(h)
   INC   HL                            ; HL -> frames_bak(l)
   INC   HL                            ; HL -> frames_bak(h)
   INC   HL                            ; HL -> frames
   LD    (HL), D                       ; frames = -1
   INC   HL
   LD    (HL), D
   INC   HL
   DEC   E
   JR    NZ, bspr_16fill

   LD    HL, #_bspr_8array
   LD    E, #32
bspr_8fill:
   LD    (HLI), A
   INC   HL
   INC   HL
   INC   HL
   INC   HL
   DEC   E
   JR    NZ, bspr_8fill

   LD    HL, #_bspr_spr64table
   LD    E, #64
bspr_kill_loop:
   LD    (HLI), A
   INC   HL
   INC   HL
   INC   HL
   DEC   E
   JR    NZ, bspr_kill_loop

;   LD    HL, #_bspr_spr64split1
;   XOR   A
;   LD    E, A
;bspr1_kill_loop:
;   LD    (HLI), A
;   DEC   E
;   JR    NZ, bspr1_kill_loop

   LD    HL, #_bspr_spr64split2
   XOR   A
   LD    E, A
bspr2_kill_loop:
   LD    (HLI), A
   DEC   E
   JR    NZ, bspr2_kill_loop

   LD    HL, #_bspr_spr64split2b
   XOR   A
   LD    E, A
bspr3_kill_loop:
   LD    (HLI), A
   DEC   E
   JR    NZ, bspr3_kill_loop

;   CALL  _w4vbl
;   CALL  _w4vbl

;   LD    A, #1
;   LD    (_bspr_state), A
   LD    A, #2
   LDH   (_vbimode), A

   POP   AF
   LDH   (_rampage), A
   LDH   (P_SVBK), A                   ; Map D000-DFFF to ram page

   RET

;_bspr_fill:
;   LD    (HLI), A
;   DEC   E
;   JR    NZ, _bspr_fill
;   RET

;----------------------------------------------------------------------------
;[[ void bspr_16set(ubyte slot, uword x, uword y, ubyte z, uword frame, ubyte pal) ]]
;----------------------------------------------------------------------------
_bspr_16set::
   LDH   A, (_rampage)
   PUSH  AF
   LD    A, #bspr_rampage
   LDH   (_rampage), A
   LDH   (P_SVBK), A                   ; Map D000-DFFF to ram page

   LDHL  SP, #4                        ; HL -> slot

   LD    A, (HL)                       ; A = slot * 10
   LD    D, A                          ;
   ADD   A, A                          ;
   ADD   A, A                          ;
   ADD   A, D                          ;
   ADD   A, A                          ;

   LDHL  SP, #11                       ; DE -> frame
   LD    D, H                          ;
   LD    E, L                          ;

   PUSH  BC                            ; Save BC for GBDK

   LD    HL, #_bspr_16array            ; HL -> _bspr_16array[slot * 9]
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
   JR    NZ, sprite_active             ;
   INC   A                             ;
   JP    Z, sprite_inactive            ;

sprite_active:
   LD    A, #1                         ; active = yes
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
   JR    NZ, sprite_update
   LD    A, (HL)
   CP    B
   JR    Z, sprite_noupdate
sprite_update:
   LD    A, D
   LD    (HLD), A
   LD    A, E
   LD    (HLD), A
   LD    A, B
   LD    (HLD), A
   LD    (HL), C

   LDH   A, (_rompage)
   PUSH  AF

   LDHL  SP, #13
   LD    A, (HL)

   INC   A
   JR    NZ, loaddefpal
   DEC   A
   JR    noloaddef

loaddefpal:
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

;   CP    #64
;   JR    NZ, notok
;   LD    A, H
;loop:
;   OR    A
;   JR    Z, loop
;notok:

   SET   6, H
   ADD   HL, BC
   INC   HL
   INC   HL
   INC   HL
   INC   HL

   LD    A, (HL)

noloaddef:
   PUSH  AF

   LDHL  SP, #10
   LD    A, (HL)                       ; a = slot

;   CP    #13
;loop:
;   JR    Z, loop

   ADD   A, A                          ; 32 * 4 + 2
   ADD   A, A
   LD    E, A
   ADD   A, A
   ADD   A, #2

   LD    L, A
   LD    H, #_presort_table_page

   POP   AF

;   LD    HL, #0xC082
;   LD    E, #0x00
;   LD    A, #0x00

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
   LD    C, A
   XOR   B
   OR    #0x40
   LD    B, A                          ; BC -> sprite

   LDHL  SP, #8
   LD    A, (HL)                       ; a = slot

   RRCA
   RRCA
   LD    H, A
   AND   #0xC0
   LD    L, A
   XOR   H
   ADD   A, #0x80
   LD    H, A                          ; HL -> destination of sprite

;   LD    HL, #0x8000
;   LD    BC, #0x4080

   ; tile into sprite data

;   LD    D, #64
;sprite_write_loop:
;   LD    A, (BC)
;   INC   BC
;   RST   ldi_v_a
;   DEC   D
;   JR    NZ, sprite_write_loop

   LD    D, #64
sprite_write_loop:
   LDH   A, (P_STAT)
   AND   #0x02
   JR    NZ, sprite_write_loop
   LD    A, (BC)
   INC   BC
   RST   ldi_v_a
   DEC   D
   JR    NZ, sprite_write_loop

   POP   AF
   LDH   (_rompage), A
   LD    (0x2000), A                           ; Map 4000-7FFF to cur_page

sprite_noupdate:
   POP   BC

   POP   AF
   LDH   (_rampage), A
   LDH   (P_SVBK), A                   ; Map D000-DFFF to ram page

   RET

sprite_inactive:
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

   LD    A, C                          ; frames_bak = -1
   LD    (HLI), A                      ;
   LD    (HL), B                       ;

   POP   BC
   POP   AF
   LDH   (_rampage), A
   LDH   (P_SVBK), A                   ; Map D000-DFFF to ram page

   RET

;----------------------------------------------------------------------------
;[[ void bspr_8set(ubyte slot, uword x, uword y, ubyte frame, ubyte mode) ]]
;----------------------------------------------------------------------------
_bspr_8set::
   LDH   A, (_rampage)
   PUSH  AF
   LD    A, #bspr_rampage
   LDH   (_rampage), A
   LDH   (P_SVBK), A                   ; Map D000-DFFF to ram page

   LDHL  SP, #4

   LD    A, (HL)                       ; a = slot
   LD    D, A
   ADD   A, A
   ADD   A, A
   ADD   A, D

   LDHL  SP, #9
   LD    D, H
   LD    E, L                          ; DE -> frame

   LD    HL, #_bspr_8array

   ADD   A, L
   LD    L, A
   ADC   A, H
   SUB   L
   LD    H, A                          ; HL -> _bspr_8array[slot * 5]

   LD    A, (DE)                       ; DE -> frame
   INC   A

   JR    Z, sprite_8inactive

   INC   DE
   LD    A, (DE)                       ; DE -> mode
   RRCA                                ; top bit -> 6th bit
   AND   #0x40                         ; mask 6th bit
   OR    #0x80                         ; add top bit
   LD    (HLI), A                      ; save to "active"
   LD    A, (DE)                       ; DE -> mode
   AND   #0x07
   DEC   DE                            ; DE -> frame
   PUSH  AF
   PUSH  DE
   DEC   DE
   DEC   DE
   DEC   DE
   DEC   DE
   PUSH  DE
   LD    A, (DE)
   LD    (HLI), A
   INC   DE
   LD    A, (DE)
   LD    (HLI), A
   INC   DE
   LD    A, (DE)
   LD    (HLI), A
   INC   DE
   LD    A, (DE)
   LD    (HL), A
   POP   DE
   DEC   DE                            ; DE -> slot
   LD    A, (DE)

   LD    HL, #_bspr_spr64table + 2
   ADD   A, A
   ADD   A, A
   ADD   A, L
   LD    L, A
   ADC   A, H
   SUB   L
   LD    H, A

   POP   DE                            ; DE -> frame

   LD    A, (DE)                                 ; A = wingding num
   ADD   A, #32                                  ; 0-31 on left, 32-95 on right
   LD    D, A                                    ; offset to 32-63
   AND   #64                                     ; on right side?
   LD    E, A                                    ; E=64 if right side
   LD    A, D                                    ; A=offset number
   AND   #63

   ADD   A, A

   LD    (HLI), A
   LD    A, E
   RRCA
   RRCA
   RRCA
   POP   DE
   OR    D

   LD    (HL), A

   POP   AF
   LDH   (_rampage), A
   LDH   (P_SVBK), A                   ; Map D000-DFFF to ram page

   RET

sprite_8inactive:
   LD    (HL), A

   POP   AF
   LDH   (_rampage), A
   LDH   (P_SVBK), A                   ; Map D000-DFFF to ram page

   RET

_bspr_int::
   LDH   A, (_rompage)
   PUSH  AF

   LD    A, #_rom_page_title
   LDH   (_rompage), A
   LD    (0x2000), A

   CALL  _bspr_int_real

   POP   AF
   LDH   (_rompage), A
   LD    (0x2000), A

   JP    _vbl_end

;;----------------------------------------------------------------------------
;_bspr_int::                            ; Battle Interrupt Routine
;;----------------------------------------------------------------------------
;
;;. Initialize the battle sprites interrupt ._._._._._._._._._._._._._._._._.
;
;   LD    A, #2                         ; Set 64 sprites mode
;   LDH   (_hbimode), A                 ;
;
;   LD    A, #111                       ; Set next gradient line to line 112
;   LDH   (P_LYC), A                    ;
;
;   LD    HL, #0xFF40                   ; HL -> LCD controller
;   SET   1, (HL)                       ; Sprites ON
;   RES   0, (HL)                       ; Sprites ON
;
;   LD    A, #_bspr_spr64split1_page    ; DMA previous frame's sprite data
;   CALL  oam_dma                       ;
;
;   LD    A, (_owe_panmode)
;   OR    A
;   CALL  NZ, scroll_update
;
;   LD    A, #bspr_rampage              ; Map D000-DFFF to bspr's ram page
;   LDH   (P_SVBK), A                   ;
;
;;. Find weighted average of scrollx ._._._._._._._._._._._._._._._._._._._._.
;
;   LD    HL, #_bspr_destx              ; HL -> bspr_destx
;   LD    A, (HLI)                      ; BC = (bspr_destx)
;   LD    C, A                          ;
;   LD    A, (HLI)                      ;
;   LD    B, A                          ; HL -> bspr_scrollx
;
;   LD    A, (HLI)                      ; DE = (bspr_scrollx)
;   LD    D, (HL)                       ;
;   LD    E, A                          ;
;
;   LD    H, D                          ; HL = bspr_scrollx
;   LD    L, E                          ;
;   ADD   HL, HL                        ; HL = bspr_scrollx * 2
;   ADD   HL, DE                        ; HL = bspr_scrollx * 3
;   ADD   HL, HL                        ; HL = bspr_scrollx * 6
;   ADD   HL, DE                        ; HL = bspr_scrollx * 7
;
;   LD    A, L                          ; HL = bspr_scrollx * 7 + bspr_destx
;   ADD   A, C                          ;
;   LD    L, A                          ;
;   LD    A, H                          ;
;   ADC   A, B                          ;
;   LD    H, A                          ;
;
;   LD    A, E                          ; A = (bspr_scrollx < bspr_dest) * 7
;   SUB   C                             ; (signed comparison)
;   RL    B                             ;
;   CCF                                 ;
;   RR    B                             ;
;   LD    A, D                          ;
;   RLA                                 ;
;   CCF                                 ;
;   RRA                                 ;
;   SBC   A, B                          ;
;   SBC   A, A                          ;
;   AND   #7                            ;
;
;   ADD   A, L                          ; DE = bspr_scrollx * 7 + bspr_destx
;   LD    E, A                          ;    + (bspr_scrollx < bspr_dest) * 7
;   ADC   A, H                          ;
;   SUB   E                             ;
;   LD    D, A                          ;
;
;   LD    HL, #_bspr_scrollx            ; HL = bspr_scrollx
;   LD    A, (HL)                       ;
;   SUB   #80                           ;
;   LDH   (P_SCX), A                    ; (LCDSCX) = scrollx - 80
;   LDH   (_rscx), A
;
;   LD    A, E                          ; DA = (bspr_scrollx * 7 + bspr_destx
;   SRA   D                             ;    + (bspr_scrollx < bspr_dest) * 7)
;   RRA                                 ;    >> 3
;   SRA   D                             ;
;   RRA                                 ;
;   SRA   D                             ;
;   RRA                                 ;
;
;   LD    (HLI), A                      ; (bspr_scrollx) = DA
;   LD    (HL), D                       ;
;
;   SUB   #88                           ; DE = -(bspr_scrollx) + 87
;   CPL                                 ;
;   LD    E, A                          ;
;   LD    A, D                          ;
;   SBC   A, #0                         ;
;   CPL                                 ;
;   LD    D, A                          ;
;
;   LD    HL, #_bspr_negscrollx16 + 1   ; (bspr_negscrollx) = AE
;   LD    (HLD), A                      ; 
;   LD    (HL), E                       ;
;
;   LD    A, E                          ; AE = -(bspr_scrollx) + 83
;   SUB   #4                            ;
;   LD    E, A                          ;
;   LD    A, D                          ;
;   SBC   A, #0                         ;
;
;   LD    HL, #_bspr_negscrollx8 + 1    ; (bspr_negscrollx) = AE
;   LD    (HLD), A                      ;
;   LD    (HL), E                       ;
;
;;. Find weighted average of scrolly ._._._._._._._._._._._._._._._._._._._._.
;
;   LD    HL, #_bspr_desty              ; HL -> bspr_desty
;   LD    A, (HLI)                      ; BC = (bspr_desty)
;   LD    C, A                          ;
;   LD    A, (HLI)                      ;
;   LD    B, A                          ; HL -> bspr_scrolly
;
;   LD    A, (HLI)                      ; DE = (bspr_scrolly)
;   LD    D, (HL)                       ;
;   LD    E, A                          ;
;
;   LD    H, D                          ; HL = bspr_scrolly
;   LD    L, E                          ;
;   ADD   HL, HL                        ; HL = bspr_scrolly * 2
;   ADD   HL, DE                        ; HL = bspr_scrolly * 3
;   ADD   HL, HL                        ; HL = bspr_scrolly * 6
;   ADD   HL, DE                        ; HL = bspr_scrolly * 7
;
;   LD    A, L                          ; HL = bspr_scrolly * 7 + bspr_desty
;   ADD   A, C                          ;
;   LD    L, A                          ;
;   LD    A, H                          ;
;   ADC   A, B                          ;
;   LD    H, A                          ;
;
;   LD    A, E                          ; A = (bspr_scrolly < bspr_dest) * 7
;   SUB   C                             ;
;   RL    B                             ;
;   CCF                                 ;
;   RR    B                             ;
;   LD    A, D                          ;
;   RLA                                 ;
;   CCF                                 ;
;   RRA                                 ;
;   SBC   A, B                          ;
;   SBC   A, A                          ;
;   AND   #7                            ;
;
;   ADD   A, L                          ; DE = bspr_scrolly * 7 + bspr_desty
;   LD    E, A                          ;    + (bspr_scrolly < bspr_dest) * 7
;   ADC   A, H                          ;
;   SUB   E                             ;
;   LD    D, A                          ;
;
;   LD    HL, #_bspr_scrolly            ; HL = bspr_scrolly
;   LD    A, (HL)                       ;
;;   SUB   #72                           ;
;   SUB   #56
;   LDH   (P_SCY), A                    ;
;   LDH   (_rscy), A
;
;   LD    A, E                          ; DA = (bspr_scrolly * 7 + bspr_desty
;   SRA   D                             ;    + (bspr_scrolly < bspr_dest) * 7)
;   RRA                                 ;    >> 3
;   SRA   D                             ;
;   RRA                                 ;
;   SRA   D                             ;
;   RRA                                 ;
;
;   LD    (HLI), A                      ; (bspr_scrolly) = DE
;   LD    (HL), D                       ;
;
;;   SUB   #80                           ;
;   SUB   #64
;   CPL                                 ;
;   LD    E, A                          ;
;   LD    A, D                          ;
;   SBC   A, #0                         ;
;   CPL                                 ;
;   LD    D, A                          ;
;
;   LD    HL, #_bspr_negscrolly16 + 1   ; (bspr_negscrolly) = DE
;   LD    (HLD), A                      ;
;   LD    (HL), E                       ;
;
;   LD    A, #3                         ; AE = -(bspr_scrolly) + 90
;   ADD   A, E                          ;
;   LD    E, A                          ;
;   ADC   A, D                          ;
;   SUB   E                             ;
;
;   LD    HL, #_bspr_negscrolly8 + 1    ; (bspr_negscrolly) = DE
;   LD    (HLD), A                      ;
;   LD    (HL), E                       ;
;
;;. Create OAM data for wingdings ._._._._._._._._._._._._._._._._._._._._._.
;
;   LD    C, #0x00                      ; C -> 64 sprites table
;
;; format for wingdings:
;;  active - 1 byte  A000000B             A = active, B = fixed
;;  x      - 2 bytes XXXXXXXX XXXXXXXX    X = absolute X coordinate
;;  y      - 2 bytes YYYYYYYY YYYYYYYY    Y = absolute Y coordinate
;
;   LD    HL, #_bspr_8array             ; HL -> array of wingdings
;   LD    E, #32                        ; 32 wingdings
;
;int_bspr_8loop:
;   LD    A, (HLI)                      ; A = wingdings.active
;   ADD   A, A                          ;
;   JR    NC, int_bspr_8inactive_nopush ;
;   JR    Z, int_bspr_8relative         ; Branch where necessary
;
;   LD    B, #_bspr_spr64table_page     ; BC -> 64 sprites table
;
;   LD    A, (HLI)                      ; D = wingdings.x(l)
;   LD    D, A                          ;
;   INC   L                             ;
;
;   LD    A, (HLI)                      ; spr.y = wingdings.y(l)
;   LD    (BC), A                       ;
;   INC   L                             ;
;   INC   C                             ;
;
;   LD    A, D                          ; spr.x = D
;   LD    (BC), A                       ;
;   INC   C                             ;
;   INC   C                             ;
;   INC   C                             ;
;
;   JR    int_bspr_8active              ; Finished processing this loop
;
;int_bspr_8relative:
;   PUSH  HL                            ; Save HL
;
;   LD    A, (_bspr_negscrollx8)        ; D = X coordinate of sprite
;   ADD   A, (HL)                       ;
;   LD    D, A                          ;
;   INC   L                             ;
;
;   LD    A, (_bspr_negscrollx8 + 1)    ; Check if sprite X is onscreen
;   ADC   A, (HL)                       ;
;   JR    NZ, int_bspr_8inactive        ;
;   INC   L                             ;
;
;   LD    A, (_bspr_negscrolly8)        ; B = Y coordinate of sprite
;   ADD   A, (HL)                       ;
;   LD    B, A                          ;
;   INC   L                             ;
;
;   LD    A, (_bspr_negscrolly8 + 1)    ; Check if sprite Y is onscreen
;   ADC   A, (HL)                       ;
;   JR    NZ, int_bspr_8inactive        ;
;   INC   L                             ;
;
;   LD    A, D                          ; Check if sprite X is inbounds
;   CP    #175                          ;
;   JR    NC, int_bspr_8inactive        ;
;
;   LD    A, B                          ; Check if sprite Y is inbounds
;   CP    #159                          ;
;   JR    NC, int_bspr_8inactive        ; A = Y coordinate of sprite
;
;   LD    B, #_bspr_spr64table_page     ; BC -> 64 sprites table
;
;   INC   A                             ; spr.y = A
;   LD    (BC), A                       ;
;   INC   C                             ;
;
;   LD    A, D                          ; spr.x = D
;   LD    (BC), A                       ;
;   INC   C                             ;
;   INC   C                             ;
;   INC   C                             ;
;   POP   AF                            ; Trash saved value (unneeded now)
;
;   JR    int_bspr_8active              ; Finished processing this loop
;
;int_bspr_8inactive:
;   POP   HL                            ; Retrieve saved value (for ptr)
;
;int_bspr_8inactive_nopush:
;   LD    B, #_bspr_spr64table_page     ; BC -> 64 sprites table
;
;   INC   L                             ; HL -> next wingding entry
;   INC   L                             ;
;   INC   L                             ;
;   INC   L                             ;
;
;   XOR   A                             ; spr.y = 0
;   LD    (BC), A                       ;
;   INC   C                             ;
;   INC   C                             ;
;   INC   C                             ;
;   INC   C                             ; BC -> next sprite entry
;
;int_bspr_8active:
;   DEC   E                             ; loop next
;   JP    NZ, int_bspr_8loop            ;
;
;;. Create OAM data for sprites ._._._._._._._._._._._._._._._._._._._._._.
;
;   LD    DE, #_sort_table
;   LD    HL, #_sort_table_ptr
;   LD    A, E
;   LD    (HLI), A
;   LD    (HL), D
;   LD    H, D
;   LD    L, E
;   XOR   A
;   LD    B, #0x10
;_sprarryclr:
;   LD    (HLI), A
;   LD    (HLI), A
;   DEC   B
;   JR    NZ, _sprarryclr
;
;   LD    HL, #_bspr_16array            ; HL = bspr_16array
;   LD    A, #16                        ;
;   LD    C, #0x00
;
;int_bspr_loop:
;   PUSH  AF
;
;   LD    A, (HLI)
;   OR    A
;   JR    Z, int_sprite_inactive_nopush
;
;   PUSH  HL
;
;; active     - 1 byte
;; x          - 2 bytes
;; z          - 1 byte
;; y          - 2 bytes
;; frames_bak - 2 bytes
;; frames     - 2 bytes * 16 entries
;
;   LD    A, (_bspr_negscrollx16)       ; E = X coordinate of sprite
;   ADD   A, (HL)                       ;
;   LD    E, A                          ;
;   INC   HL                            ;
;
;   LD    A, (_bspr_negscrollx16 + 1)   ; If zero then sprite is active
;   ADC   A, (HL)                       ;
;   JR    NZ, int_sprite_inactive       ;
;   INC   HL                            ;
;
;   INC   HL                            ; B = sorting Y coordinate of sprite
;   LD    A, (_bspr_negscrolly16)       ;
;   ADD   A, (HL)                       ;
;   LD    B, A                          ;
;   DEC   HL                            ;
;
;   PUSH  AF                            ; D = displayed Y coordinate of sprite
;   SUB   (HL)                          ;
;   INC   HL                            ;
;   INC   HL                            ;
;   LD    D, A                          ;
;   POP   AF                            ;
;
;   LD    A, (_bspr_negscrolly16 + 1)   ; If zero then sprite is active
;   ADC   A, (HL)                       ;
;   JR    NZ, int_sprite_inactive       ;
;   INC   HL
;
;   LD    A, E                          ; check if inbounds
;   CP    #175                          ;
;   JR    NC, int_sprite_inactive       ;
;
;   LD    A, D                          ; check if inbounds
;   CP    #159                          ;
;   JR    NC, int_sprite_inactive       ;
;
;   PUSH  HL                            ; HL -> OAM
;   LD    H, C                          ;
;   PUSH  HL                            ;
;   LD    H, #_presort_table_page       ;
;   LD    L, C                          ;
;
;   INC   D
;   LD    A, D
;   LD    (HLI), A
;   LD    A, E
;   SUB   #7
;   LD    (HLI), A
;   INC   HL
;   INC   HL
;
;   LD    A, D
;   LD    (HLI), A
;   LD    A, E
;   INC   A
;   LD    (HLI), A
;
;   LD    HL, #_sort_table_ptr
;   LD    A, (HLI)
;   LD    D, (HL)
;   LD    E, A
;
;   LD    A, B
;   LD    (DE), A
;   INC   DE
;   POP   AF
;   LD    (DE), A
;   INC   DE
;
;   LD    A, D
;   LD    (HLD), A
;   LD    (HL), E
;
;   POP   HL
;   POP   DE
;
;   INC   HL
;   INC   HL
;   INC   HL
;   INC   HL
;
;   LD    A, #8
;   ADD   A, C
;   LD    C, A                          ; point C to next entry
;
;   JR    int_sprite_active
;
;int_sprite_inactive:
;   POP   HL
;int_sprite_inactive_nopush:
;   LD    A, #9
;   ADD   A, L
;   LD    L, A
;   ADC   A, H
;   SUB   L
;   LD    H, A                          ; point HL to next entry
;
;   LD    A, #8
;   ADD   A, C
;   LD    C, A                          ; point C to next entry
;
;;   LD    B, #_presort_table_page
;;
;;   XOR   A
;;   LD    (BC), A
;;   INC   C
;;   INC   C
;;   INC   C
;;   INC   C
;;   LD    (BC), A
;;   INC   C
;;   INC   C
;;   INC   C
;;   INC   C
;int_sprite_active:
;   POP   AF
;   DEC   A
;   JP    NZ, int_bspr_loop
;
;   CALL  _sort_bspr
;
;   CALL  _project_bspr
;
;   CALL  spr64_buildtable
;
;_bspr_intdone:
;   LDH   A, (_rampage)
;   LDH   (P_SVBK), A                   ; Map D000-DFFF to ram page
;
;   LD    A, (_owe_panmode)
;   OR    A
;   CALL  NZ, scroll_update_time
;
;_quit_bspr_int:
;   JP    _vbl_end
;
_spr64hblank_int::
   LD    A, #bspr_rampage
   LDH   (P_SVBK), A                   ; Map D000-DFFF to ram page

   LD    A, #24                                 ; 2; A=15
sprloop:
   DEC   A                                      ; 1;
   JR    NZ, sprloop                            ; 2;
   LD    A, (HL)                                ; 2;

   LD    A, (_spr64_table)
   XOR   #7
   LD    (_spr64_table), A

   CALL  oam_dma

   LDH   A, (_rampage)
   LDH   (P_SVBK), A                   ; Map D000-DFFF to ram page

   LD    A, #1
   LDH   (_hbimode), A

   LD    A, (_bspr_window)             ; Set next gradient line to line 112
   DEC   A                             ;
   SUB   A, #10
   LDH   (P_LYC), A                    ;

   JP    _hbl_end

;;###################################################
;;# Set up (sort) the two sprite tables
;;# Input:
;;# _largetab_buf_pointer =  pointer to 256 byte 
;;#          64 sprite table to be
;;#          split into two 40 sprite
;;#          table (algined to $XX00)
;;# _sprite_table0 = 160 byte table
;;# _sprite_table1 = 160 byte table
;;# _spr64_nextcmpline = storage for mean
;;###################################################
;
;;----------------------------------------------------------------------------
;;[ spr64_buildtable ]
;;----------------------------------------------------------------------------
;spr64_buildtable:
;;   LD    BC, #_bspr_spr64table
;;
;;   LD    HL, #0x0000
;;   LD    D, H
;;_get_mean:
;;   LD    A, (BC)
;;   LD    E, A
;;   SUB   A, #0x01
;;   SBC   A, A
;;   AND   #88
;;   ADD   HL, DE
;;   LD    E, A
;;   ADD   HL, DE
;;   INC   C
;;   INC   C
;;   INC   C
;;   INC   C
;;   JR    NZ, _get_mean
;;   LD    A, L
;;   AND   #0xC0
;;   OR    H                             ; divide by 64
;;   RLCA
;;   RLCA
;;   SUB   #16
;
;   LD    A, (_spr64_cmpline)
;   LDH   (P_LYC), A
;
;   LD    B, #72
;
;checkforspriteonlineagain:
;   LD    HL, #_bspr_spr64table         ; HL -> spr64table
;checkforspriteonline:
;   LD    A, (HLI)                      ; Get Y coordinate
;   SUB   B                             ; Is it < current line?
;   JR    C, nospriteonline
;   SUB   #16                           ; Is it > current line?
;   JR    NC, nospriteonline
;
;   ADD   A, B
;   LD    B, A                          ; Is compare line above 56?
;   CP    #56
;   JR    NC, checkforspriteonlineagain
;
;   JR    checkdownwards
;
;nospriteonline:
;   INC   L
;   INC   L
;   INC   L
;   JR    NZ, checkforspriteonline
;
;   LD    A, B
;   DEC   A
;   DEC   A
;
;   JR    donecheckingsprite
;checkdownwards:
;
;   LD    B, #73
;checkforspriteonlineagainb:
;   LD    HL, #_bspr_spr64table         ; HL -> spr64table
;checkforspriteonlineb:
;   LD    A, (HLI)                      ; Get Y coordinate
;   SUB   B                             ; Is it < current line?
;   JR    C, nospriteonlineb
;   CP    #16                           ; Is it > current line?
;   JR    NC, nospriteonlineb
;
;   ADC   A, B
;   LD    B, A                           ; Is compare line below 88?
;   CP    #88
;   JR    C, checkforspriteonlineagainb
;
;   LD    A, (_spr64_table)             ; multisync code
;   RRCA
;   SBC   A, A
;   AND   #31
;
;   ADD   A, #53
;
;   JR    donecheckingsprite
;
;nospriteonlineb:
;   INC   L
;   INC   L
;   INC   L
;   JR    NZ, checkforspriteonlineb
;
;   LD    A, B
;   DEC   A
;   DEC   A
;donecheckingsprite:
;   LD    (_spr64_cmpline), A
;
;   LD    HL, #_bspr_spr64table
;   LD    B, A                          ; B=cmp-16
;   LD    C, #0x00                      ; C->table0
;   ADD   A, #16
;   LD    D, A                          ; D=cmp
;   LD    E, #0x00                      ; E->table1
;
;_sort_main_loop:
;   LD    A, (HL)
;   OR    A
;   JR    Z, _no_load_table0            ; if 0, no copy
;
;   CP    D
;   JR    NC, _no_load_table0           ; if carry, it is <= mean, so copy
;
;   LD    A, C
;   CP    #160
;   JR    Z, _no_load_table0
;
;   PUSH  BC
;   LD    B, #_bspr_spr64split1_page
;
;   LD    A, (HLI)
;   LD    (BC), A
;   INC   C
;   LD    A, (HLI)
;   LD    (BC), A
;   INC   C
;   LD    A, (HLI)
;   LD    (BC), A
;   INC   C
;   LD    A, (HLD)
;   LD    (BC), A
;   INC   C
;   POP   AF
;   LD    B, A
;
;   DEC   L
;   DEC   L
;
;_no_load_table0:                       ; sprite to table1
;   LD    A, (HL)
;   CP    B
;   JR    C, _no_load_table1
;
;   LD    A, E
;   CP    #160
;   JR    Z, _no_load_table1
;
;   PUSH  DE
;   LD    A, (_spr64_table)
;   LD    D, A
;
;   LD    A, (HLI)
;   LD    (DE), A
;   INC   E
;   LD    A, (HLI)
;   LD    (DE), A
;   INC   E
;   LD    A, (HLI)
;   LD    (DE), A
;   INC   E
;   LD    A, (HL)
;   LD    (DE), A
;   INC   E
;   POP   AF
;   LD    D, A
;
;   JR    _skiptable
;
;_no_load_table1:
;   INC   L
;   INC   L
;   INC   L
;_skiptable:
;   INC   L
;   JR    NZ, _sort_main_loop
;
;; clear the rest of the tables now
;   LD    A, #160
;   SUB   C
;   JR    Z, _skip_clear0               ; if C >= 160, can't clear any entries
;   LD    H, #_bspr_spr64split1_page
;   LD    L, C                          ; HL = BC
;   RRCA
;   RRCA                                ; A = (160 - C)/4
;   LD    C, A
;   XOR   A
;_clear_table0:
;   LD    (HLI), A
;   INC   L
;   INC   L
;   INC   L
;   DEC   C
;   JR    NZ, _clear_table0
;
;_skip_clear0:
;   LD    A, #160
;   SUB   E
;   RET   Z                             ; if E = 160, can't clear any entries
;
;   RRCA                                ; A = (160 - E)/4
;   RRCA
;   LD    C, A
;
;   LD    A, (_spr64_table)
;   LD    H, A
;   LD    L, E                          ; HL = DE
;
;   XOR   A
;_clear_table1:
;   LD    (HLI), A
;   INC   L
;   INC   L
;   INC   L
;   DEC   C
;   JR    NZ, _clear_table1
;   RET
;
;_sort_bspr:
;   LD    DE, #_sort_table
;   LD    HL, #_sort_table
;
;   JP    _sort
;
;_project_bspr:
;   LD    B, #_presort_table_page
;   LD    HL, #_bspr_spr64table + 0x80
;   LD    DE, #_sort_table
;
;_project_loop:
;   LD    A, (DE)
;   OR    A
;   JR    Z, blankrestoftable
;   INC   DE
;   LD    A, (DE)
;   INC   DE
;   LD    C, A
;
;   LD    A, (BC)
;   INC   BC
;   LD    (HLI), A
;   LD    A, (BC)
;   INC   BC
;   LD    (HLI), A
;   LD    A, (BC)
;   INC   BC
;   LD    (HLI), A
;   LD    A, (BC)
;   INC   BC
;   LD    (HLI), A
;   LD    A, (BC)
;   INC   BC
;   LD    (HLI), A
;   LD    A, (BC)
;   INC   BC
;   LD    (HLI), A
;   LD    A, (BC)
;   INC   BC
;   LD    (HLI), A
;   LD    A, (BC)
;   INC   BC
;   LD    (HLI), A
;   JR    _project_loop
;
;blankrestoftable:
;   XOR   A
;blankrestoftable_loop:
;   LD    (HLI), A
;   LD    (HLI), A
;   LD    (HLI), A
;   LD    (HL), A
;   INC   L
;   JR    NZ, blankrestoftable_loop
;   RET


;_bbox = icon
;_bboxpal = palette

;----------------------------------------------------------------------------
;[[ void bicon_reset() ]]
;----------------------------------------------------------------------------

;  8,  9, 10, 11, 12, 13, 14, 15 <- index of wingding
; 64, 65, 66, 67, 68, 69, 70, 71 <- index of tiles (/ 2)

; coords: bbox[0] top    (32,24)&(40,24)
;         bbox[1] left   (16,40)&(24,40)
;         bbox[2] right  (48,40)&(56,40)
;         bbox[3] bottom (32,56)&(40,56)

_bicon_reset::
   PUSH  BC

   LDH   A, (_rampage)
   PUSH  AF
   LD    A, #bspr_rampage              ; Map D000-DFFF to bspr's ram page
   LDH   (_rampage), A                 ;
   LDH   (P_SVBK), A                   ;

   XOR   A
   LD    (_bicon_mode), A
   LD    (_bicon_icon), A
   LD    (_bicon_icon2), A
   LD    (_bicon_expanding), A

   CALL  _bicon_clearloop

   POP   AF
   LDH   (_rampage), A                 ;
   LDH   (P_SVBK), A                   ;

   POP   BC
   RET

_bicon_clearloop::
   LD    B, #0x08
   LD    DE, #0x0005

   LD    HL, #_bspr_8array + 40        ; start at index 8 (8*5=40)
_bicrst_loop:
   LD    (HL), D
   ADD   HL, DE
   DEC   B
   JR    NZ, _bicrst_loop
   RET

_bicon_slidein::
   LD    HL, #_bicon_mode
   XOR   A                             ; mode=0
   LD    (HLI), A
   LD    A, #4                         ; addx=4
   LD    (HLI), A
   LD    A, #16                        ; count=16
   LD    (HLI), A
   LD    A, #-64                       ; x=-64
   LD    (HLI), A
   XOR   A                             ; exp=0
   LD    (HLI), A
   LD    (HLI), A
   LD    (HLI), A
   INC   A                             ; mode=1
   LD    (_bicon_mode), A

   LD    A, #0
   CALL  refresh2
   LD    A, #1
   CALL  refresh2
   LD    A, #2
   CALL  refresh2
   LD    A, #3
   JP    refresh2

_bicon_slideout::
   LD    HL, #_bicon_mode
   XOR   A                             ; mode=0
   LD    (HLI), A
   LD    A, #-4                        ; addx=-4
   LD    (HLI), A
   LD    A, #16                        ; count=16 - current count
   SUB   (HL)
   LD    (HLI), A
   INC   HL                            ; x coordinate doesn't change
   XOR   A                             ; exp=0
   LD    (HLI), A
   LD    (HLI), A
   LD    (HLI), A
   INC   A                             ; mode=1
   LD    (_bicon_mode), A
   RET

;----------------------------------------------------------------------------
;[[ void bicon_expand(ubyte x) ]]
;----------------------------------------------------------------------------
_bicon_expand::
   LD    A, (bicon_set_frame)
   OR    A
   JR    NZ, ignorethis

   LD    HL, #_bicon_expanding
   LD    A, (HL)
   ADD   A, #0xFF
   SBC   A, A
   AND   #4
   LD    (bicon_set_frame), A
   LD    (HL), #0x01

ignorethis:
   LDHL  SP, #2
   LD    A, (HL)                       ; load icon
   LD    (_bicon_icon), A

   LD    D, A
   OR    A

   LD    A, #7
   LD    (_bicon_exp_dx), A
   LD    A, #0
   LD    (_bicon_exp), A

   LD    A, #7
   LD    (_bicon_exp_ct), A
   RET   Z

   LDH   A, (_rampage)
   PUSH  AF
   LD    A, #bspr_rampage              ; Map D000-DFFF to bspr's ram page
   LDH   (_rampage), A                 ;
   LDH   (P_SVBK), A                   ;

   LD    A, D
   ADD   A, A
   ADD   A, A
   ADD   A, D
   ADD   A, A

   LD    D, #0
   LD    E, A

   LD    HL, #_bspr_8array + 41 - 10    ; start at index 8 (8*5=40)
   ADD   HL, DE

;   LD    B, B

   LD    A, (HLI)
   LD    (_bicon_iconxy), A
   INC   HL
   LD    A, (HL)
   LD    (_bicon_iconxy+1), A

   POP   AF
   LDH   (_rampage), A                 ;
   LDH   (P_SVBK), A                   ;

   RET

;----------------------------------------------------------------------------
;[[ void bicon_shrink() ]]
;----------------------------------------------------------------------------
_bicon_shrink::
   LD    A, #6
   LD    (_bicon_exp_ct), A

   XOR   A
   LD    (_bicon_icon), A
   LD    (_bicon_icon2), A
   RET

;----------------------------------------------------------------------------
;[[ void bicon_refresh(ubyte x) ]]
;----------------------------------------------------------------------------
_bicon_refresh::
   LDHL  SP, #2
   LD    A, (HL)                       ; load icon

refresh2:
   PUSH  BC
   CALL  manualloadgfx
   POP   BC

   RET

loadgraphicsthisframe::
   LD    HL, #bicon_set_frame
   DEC   (HL)
   LD    A, (HL)

manualloadgfx:
   LD    B, #0
   LD    C, A

   RRCA
   RRCA
   LD    E, A
   AND   #0b00111111
   LD    D, A
   XOR   E
   LD    E, A

   LDH   A, (P_VBK)
   PUSH  AF
   LD    A, #1
   LDH   (P_VBK), A

   LDH   A, (_rompage)
   PUSH  AF
   LD    A, #_rom_page_data
   LDH   (_rompage), A
   LD    (0x2000), A

   LD    HL, #_bbox
   ADD   HL, BC

   LD    A, (HL)

   LD    HL, #_bboxpal
   ADD   HL, BC

   LD    B, H
   LD    C, L

   LD    HL, #0x8400
   ADD   HL, DE

   CALL  bicon_thing

   POP   AF
   LDH   (_rompage), A
   LD    (0x2000), A

   POP   AF
   LDH   (P_VBK), A
   RET

; A = bbox icon number BC -> bboxpal HL -> vram
bicon_thing:
   PUSH  BC
   PUSH  HL

   LD    D, #0
   LD    E, A

   RRCA
   RRCA
   LD    C, A
   AND   #0b00111111
   LD    B, A
   XOR   C
   LD    C, A
   LD    A, E
   CP    #6
   JR    NC, bicon_notwingding

   LD    A, #6

   LD    HL, #_wingding+1024           ; data page
   ADD   HL, BC
   JR    loadbicon
bicon_notwingding:
   CP    #20
   JR    NC, bicon_notitem

   LD    HL, #_itemgfx-6               ; data page
   ADD   HL, DE
   LD    A, (HL)

   LD    HL, #_itemgfx-6*64+24         ; data page
   ADD   HL, BC
   JR    loadbicon
bicon_notitem:
   CP    #40
   JR    NC, bicon_notskill

   LD    HL, #_skillgfx-20             ; data page
   ADD   HL, DE
   LD    A, (HL)

   LD    HL, #_skillgfx-20*64+20       ; data page
   ADD   HL, BC
   JR    loadbicon
bicon_notskill:
   LD    HL, #_gemgfx-40               ; data page
   ADD   HL, DE
   LD    A, (HL)

   LD    HL, #_gemgfx-40*64+16         ; data page
   ADD   HL, BC

loadbicon:
   LD    D, H
   LD    E, L
   POP   HL

   PUSH  AF
   CALL  copy64bytes_vram3
   POP   AF

   POP   BC
   LD    (BC), A

   RET

; icon  0- 5   wingding
; icon  6-19   item
; icon 20-39   skill
; icon 40-     gem

;----------------------------------------------------------------------------
;[[ ubyte getcount(ubyte speed,ubyte action) ]]
;----------------------------------------------------------------------------
_getcount::
   LDHL  SP, #2
   LD    A, (HLI)
   LD    D, (HL)
   LD    E, A

   LDH   A, (_rompage)
   PUSH  AF

   LD    A, #_rom_page_title
   LDH   (_rompage), A
   LD    (0x2000), A

   LD    HL, #turntable
   ADD   HL, DE
   LD    E, (HL)

;   LD    B, B

   POP   AF
   LDH   (_rompage), A
   LD    (0x2000), A

   RET
