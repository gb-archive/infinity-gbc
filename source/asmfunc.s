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
; Z80-A Assembly Routines
;----------------------------------------------------------------------------
   .include "gbasm.h"

oam_dma     = _oam_dma_rom  ;0xFF80

_blackpal   = 0x0000
_whitepal   = 0x0001
_redpal     = 0x0002
_bluepal    = 0x0003

_rom_page_master  = 0x00
_rom_page_event   = 0x01
_rom_page_event2  = 0x02
_rom_page_data    = 0x03
_rom_page_data2   = 0x04
_rom_page_menu1   = 0x05
_rom_page_menu2   = 0x06
_rom_page_battle1 = 0x07
_rom_page_battle2 = 0x08
_rom_page_battle3 = 0x09
_rom_page_battle4 = 0x0A
_rom_page_battle5 = 0x0B
_rom_page_battle6 = 0x0C
_rom_page_dfs     = 0x0D
_rom_page_title   = 0x0E
_rom_page_zone    = 0x0F
_rom_page_form    = 0x10
_rom_page_events  = 0x11 ; to 0x14
_rom_page_text    = 0x15 ; to 0x1A
_rom_page_table   = 0x1C
_rom_page_world   = 0x2D
_rom_page_para5   = 0x3F

table_page  = _rom_page_table

_rompage    = 0xF0
_rampage    = 0xF1
_vbimode    = 0xF2
_hbimode    = 0xF3

_scr_neg    = 0xEE
_scr_pos    = 0XEF

_rscx       = 0xFA
_rscy       = 0xFB

ldi_v_a     = 0x08
ldi_v_aa    = 0x20

   .area _BSS

_ram_page_backup = 3             ;;;;;;;;;;;;;; tmp

;_demo_key::
;   .ds   1
;_demo_key_ptr::
;   .ds   2

;_debug_on::
;   .ds   1
;_debug_ptr::
;   .ds   2

_animate_yes::
   .ds   1

_gametime::
   .ds   4

_reset_ok::
   .ds   1

_ending_local::
   .ds   1
_endingmode::
   .ds   1
_topline::
   .ds   1
_bottomline::
   .ds   1
_offset::
   .ds   1
_newoffset::
   .ds   1
_endingcount::
   .ds   2
_endingobj_mem::
   .ds   14
ending_ptr::
   .ds   2

_dialogue_page::
   .ds   1
_dialogue_ptr::
   .ds   2

_mapsrc_ptr::
   .ds   2
_mapdest_ptr::
   .ds   2
_map_lenx::
   .ds   1
_pal_ptr::
   .ds   2
_map_vram_x::
   .ds   1
_map_vram_y::
   .ds   1

using_auto_proc_ptr:                   ; autoproc stuff
   .ds   1
auto_proc_rom:
   .ds   1
auto_proc_ptr:
   .ds   2

_backup_rompage:
   .ds   1
_backup_return:
   .ds   2

rnd_var:
   .ds   1

_rnd_seed::
rnd_ofs:
   .ds   2

_vblank_timer::
   .ds   1

;_owe_pantile::
;   .ds   64
;_owe_pantile2::
;   .ds   64
_owe_panmode::
   .ds   1
_owe_pancount::
   .ds   1

key_cur:
   .ds   1
key_chg:
   .ds   1

_barg1::
   .ds   1
_barg2::
   .ds   1
_barg3::
   .ds   1
_barg4::
   .ds   1
_barg5::
   .ds   1
_warg1::
   .ds   2
_warg2::
   .ds   2
_warg3::
   .ds   2
_warg4::
   .ds   2
_warg5::
   .ds   2

;_sprite_blast::
;   .ds   79

inputb_curbit:
   .ds   1
inputb_current:
   .ds   1
inputb_timer:
   .ds   1

   .area _CODE

_dmg_init::
   LD    A, #0b10111000
   LDH   (0x47), A

   XOR   A
   LDH   (_hbimode), A
   LDH   (_vbimode), A

   LDH   (P_SCX), A
   LDH   (P_SCY), A

   LDH   (P_WY), A
   LD    A, #167
   LDH   (P_WX), A

   LD    A, #0b11100111
   LDH   (P_LCDC), A
   LD    A, #0b01000100
   LDH   (P_STAT), A

   RET

;----------------------------------------------------------------------------
;[[ asm_init ]]
; ASM initialization routines
;----------------------------------------------------------------------------
_asm_init::
   XOR   A
   LD    (_reset_ok), A
   LD    (sound_play), A
   LD    (_animate_yes), A

;   LD    (_demo_key_ptr), A
;   LD    (_demo_key_ptr+1), A

;   LD    A, #1
;   LD    (_endingmode), A

;   LD    A, (_demo_key)
;   OR    A
;   JR    Z, _nocopysaves
;
;   LD    HL, #0x4000
;   LD    BC, #0x2000
;   LD    DE, #0xA000
;
;   XOR   A
;   LD    (HL), A
;   LD    A, #0x0A
;   LD    (0x0000), A
;
;   LD    A, #59
;   LDH   (_rompage), A
;   LD    (BC), A
;
;   CALL  _LDIR
;
;   XOR   A
;   LD    (BC), A  ; BC = 0
;_nocopysaves:

ai_fm_tryagain:                        ; Try to set FAST mode
   LD    A, #0x01
   LDH   (P_KEY1), A                   ; Causes 1x <-> 2x toggle
   STOP
   LDH   A, (P_KEY1)                   ; Is CPU 2x speed?
   ADD   A, A
   JR    NC, ai_fm_tryagain

   DI

   CALL  _hblank_init
   CALL  _vblank_init

   LD    A, #0x01
   LDH   (_hbimode), A
   CALL  set_ram_page
   CALL  set_rom_page

   LD    HL, #_pfs_targetbgr
   LD    (HL), #0b11111110
   INC   HL
   LD    (HL), #0b11111100

;   LD    HL, #_debug_ptr   ; 0xD000 - 0xDFFF
;   LD    DE, #0xD000
;   LD    A, E
;   LD    (HLI), A
;   LD    (HL), D

   LD    A, #0xFF
   LD    (_backup_rompage), A

   XOR   A
;   LD    (_debug_on), A
   LDH   (_vbimode), A

   LDH   (_rscx), A
   LDH   (P_SCX), A
   LDH   (_rscy), A
   LDH   (P_SCY), A

   LD    A, #0b11100111
   LDH   (P_LCDC), A
   LD    A, #0b01000100
   LDH   (P_STAT), A

   XOR   A
   LDH   (P_WY), A
   LD    A, #167
   LDH   (P_WX), A

   XOR   A                                        ; Set rom page to 0
   LD    (0x3000), A

   LD    HL, #0xC000
   LD    B, #0xA0
   JP    _STIR_B

;----------------------------------------------------------------------------
;[[ ubyte rom_page(UBYTE cur_page) ]]
;
; Switch Rom Page, returns previous.
;----------------------------------------------------------------------------
_rom_page::
   LDHL  SP, #2                        ; HL->SP+2, 1st parameter

   LDH   A, (_rompage)
   LD    E, A

   LD    A, (HL)

;   OR    A
;   JR    NZ, nocrash
;;;   LD    B, B
;nocrash:

set_rom_page:
   LDH   (_rompage), A
   LD    (0x2000), A                   ; Map 4000-7FFF to cur_page

   RET

;----------------------------------------------------------------------------
;[[ ubyte ram_page(UBYTE cur_page_ram) ]]
;
; Switch Ram Page.
;----------------------------------------------------------------------------
_ram_page::
   LDHL  SP, #2                        ; HL->SP+2, 1st parameter

   LDH   A, (_rampage)
   LD    E, A

   LD    A, (HL)
set_ram_page:
   LDH   (_rampage), A
   LDH   (P_SVBK), A                   ; Map D000-DFFF to ram page

   RET

;----------------------------------------------------------------------------
;[[ w4vbl_done ]]
; Wait for vblank to end.
;----------------------------------------------------------------------------
_w4vbl_done::
   LD    HL, #_vblank_timer
   LD    A, (HL)
w4vbld_loop1:
   HALT
   CP    (HL)
   JR    Z, w4vbld_loop1
   RET

;----------------------------------------------------------------------------
;[[ w4vbl ]]
; Wait for vblank.
;----------------------------------------------------------------------------
_w4vbl::
   PUSH  AF
w4vbl_loop:
   LDH   A, (P_LY)
   CP    #144
   JR    C, w4vbl_loop
   POP   AF
   RET

;----------------------------------------------------------------------------
;[[ w4vbl_done2 ]]
; Wait for vblank.
;----------------------------------------------------------------------------
_w4vbl_done2::
   PUSH  AF
w4vbl_loopa:
   LDH   A, (P_LY)
   CP    #144
   JR    NC, w4vbl_loopa
w4vbl_loopb:
   LDH   A, (P_LY)
   CP    #144
   JR    C, w4vbl_loopb
   POP   AF
   RET

;----------------------------------------------------------------------------
;[[ w4vbl_done3 ]]
; Wait for vblank.
;----------------------------------------------------------------------------
_w4vbl_done3::
   PUSH  AF
w4vbl_loopaa:
   LDH   A, (P_LY)
   CP    #116
   JR    NC, w4vbl_loopaa
w4vbl_loopab:
   LDH   A, (P_LY)
   CP    #116
   JR    C, w4vbl_loopab
   POP   AF
   RET

;----------------------------------------------------------------------------
;[[ w4line ]]
; Wait for vblank.
;----------------------------------------------------------------------------
_w4line::
   LDHL  SP, #2
   LD    D, (HL)
w4l_loop:
   LDH   A, (P_LY)
   CP    D
   JR    NZ, w4l_loop
   RET

;;----------------------------------------------------------------------------
;;[ w4vram ]
;; Wait to see if vram is writable.
;;modifies:
;; none
;;----------------------------------------------------------------------------
;w4vram:
;   PUSH  AF
;w4vram_loop:
;   LDH   A, (P_STAT)
;   AND   #0x02
;   JR    NZ, w4vram_loop
;   POP   AF
;   RET

;_record_or_get::
;
;.include "record.h"

;----------------------------------------------------------------------------
;[[ get_keypad ]]
;----------------------------------------------------------------------------
_get_keypad::
   LD    A, #0x10                      ; mask out buttons
   LDH   (P_P1), A

   LDH   A, (P_P1)                     ; read key
   OR    #0xF0
   SWAP  A
   LD    E, A                          ; save backup

   LD    A, #0x20                      ; mask out dirs
   LDH   (P_P1), A

   LDH   A, (P_P1)                     ; read key
   OR    #0xF0
   XOR   E                             ; combine the keys
   LD    E, A

;   LD    A, (_demo_key)
;   OR    A
;   CALL  NZ, _record_or_get

;   LD    HL, #key_cur
;   XOR   (HL)
;   LD    (HL), E
;   INC   HL
;   LD    (HLD), A
;   LD    D, A

   LD    A, #0x30                      ; reset port
   LDH   (P_P1), A

   RET

_input::
   PUSH  BC
   CALL  _get_keypad

   LD    HL, #_key
   LD    BC, #input_table
   LD    D, #8
input_loop:
   LD    A, (BC)
   INC   BC
   AND   E
   LD    (HLI), A
   DEC   D
   JR    NZ, input_loop
   POP   BC

   RET

_end_init::
   CALL  do_far_noarg
   .DB   _rom_page_title
   .DW   credits_init_real

_end_next::
   CALL  do_far_noarg
   .DB   _rom_page_title
   .DW   credits_next_real

input_table:
.DB   0x04,0x08,0x02,0x01,0x10,0x20,0x80,0x40

; up, down, left, right, a, b, start, select

_inputb::
   PUSH  BC                            ; save BC

   CALL  _input                        ; E=current bits

   LD    HL, #key_cur                  ; D=changed bits
   LD    A, (HLI)
   XOR   E
   LD    (HLD), A
   LD    D, A
   LD    (HL), E

   LD    HL, #input_table+7
   LD    B, #0x08                      ; 8 loops
_inputb_checkloop:
   LD    A, (HLD)                      ; read the key bit
   AND   D                             ; do we have it in current key bits?
   AND   E                             ; do we have it in changed key bits?
   JR    NZ, _inputb_pressed           ; yes we do
   DEC   B                             ; next loop
   JR    NZ, _inputb_checkloop         ;

   POP   BC                            ; don't need BC anymore

   LD    HL, #inputb_curbit            ; HL->current key bit
   LD    A, (HL)                       ; Save state of current bit
   AND   E                             ;
   LD    (HLI), A                      ; HL->current key number
   SUB   A, #1                         ; Save current key number
   SBC   A, A                          ;
   OR    (HL)                          ;
   LD    (HLI), A                      ; HL->current key timer
   LD    E, A                          ; E=current key number

   INC   A                             ; If let go, return -1
   JR    Z, _inputb_nokey              ;
   DEC   (HL)                          ; Count down timer
   JR    NZ, _inputb_nokey             ; If timer not 0, return -1

   LD    (HL), #2                      ; Set timer to 2
   RET

_inputb_nokey:
   LD    E, #-1                        ; return no key (-1)
   RET

_inputb_pressed:
   LD    HL, #inputb_curbit            ; HL->current key bit
   LD    (HLI), A                      ; HL->current key number
   DEC   B                             ; Save key number
   LD    A, B                          ;
   LD    (HLI), A                      ; HL->current key timer
   LD    (HL), #12                     ;
   POP   BC                            ; don't need BC anymore
   LD    E, A                          ; return key number
   RET

;ubyte randword()

_randword::
   CALL  _randfunc
   LD    A, E
   PUSH  AF
   CALL  _randfunc
   POP   AF
   LD    D, A
   RET

; mersenne random number generator
;
; vars/constants: seed, a, c, m
;
; seed=(a*seed+c)%m
;
; a = 257, c = 5297, m = 8191

;----------------------------------------------------------------------------
;[[ ubyte randfunc() ]]
;----------------------------------------------------------------------------
_randfunc::
   LD    HL, #rnd_ofs

;   LD    A, (_demo_key)
;   OR    A
;   JR    Z, _randok
;
;   LD    HL, #_demo_key_ptr
;
;_randok:
   LD    A, (HLI)
   LD    H, (HL)
   LD    L, A                          ; HL = seed

   LD    E, H
   ADD   A, H
   LD    H, A
   LD    A, E
   ADC   A, #0                         ; AHL = HL * 257

   LD    DE, #4476  ; though it's not prime the period is > w/ #5297
   ADD   HL, DE
   ADC   A, #0                         ; AHL = HL * 257 + 5297

   LD    E, L
   LD    D, H

   LD    L, H
   LD    H, A
   XOR   A

   ADD   HL, HL
   RLA
   ADD   HL, HL
   RLA
   ADD   HL, HL
   RLA

   LD    L, A
   LD    A, H
   ADD   A, E
   LD    E, A
   LD    A, L
   ADC   A, D
   AND   #0x1F

   LD    HL, #rnd_ofs+1
   LD    (HLD), A
   LD    (HL), E

   RET

;----------------------------------------------------------------------------
;[[ randfuncasm ]]
;----------------------------------------------------------------------------
randfuncasm::
;   PUSH  HL
;   PUSH  DE
;
;   LD    HL, #rnd_var
;   LD    A, (HL)
;   LD    D, A
;   XOR   A
;   LD    E, A
;   RLC   D
;   ADC   A, E
;   RLC   D
;   RLC   D
;   ADC   A, E
;   RLC   D
;   ADC   A, E
;   RLC   D
;   ADC   A, E
;   RRCA
;   LD    A, (HL)
;   RLA
;   ADD   A, D
;   SWAP  A
;   SUB   D
;   LD    (HL), A
;
;   POP   DE
;   POP   HL
;
;   RET

;   PUSH  HL
;   LD    HL, #rnd_ofs
;   INC   (HL)
;   LD    A, (HLI)
;   DEC   (HL)
;   LD    H, (HL)
;   LD    L, A
;
;   LD    A, (rnd_var)
;   ADD   A, (HL)
;   RLCA
;   RLCA
;   SUB   H
;   ADD   A, L
;   LD    (rnd_var), A
;   POP   HL

;   PUSH  HL
;   PUSH  BC
;   LD    HL, #rnd_ofs
;   LD    A, (HLI)
;   LD    B, (HL)
;   LD    C, A
;
;   LD    A, #193
;   ADD   A, C
;   LD    C, A
;   LD    A, #69
;   ADC   A, B
;   LD    B, A
;
;   LD    A, B
;   LD    (HLD), A
;   LD    A, C
;   LD    (HLD), A
;   LD    A, (BC)
;   RRA
;   ADC   A, (HL)
;   RRCA
;   ADC   A, H
;   SWAP  A
;   ADD   A, L
;   LD    (HL), A
;   POP   BC
;   POP   HL
;   RET

   PUSH  HL
   PUSH  DE
   CALL  _randfunc
   LD    A, E
   POP   DE
   POP   HL

   RET

;parms:
; BC=frame #
;returns:
; HL->frame
;modifies:
; AEHL
get_frame_ptr:
   LD    HL, #0x2000
   LD    A, #table_page                ; Map 4000-7FFF to cur_page
   LDH   (_rompage), A
   LD    (HL), A
   ADD   HL, HL
   LD    A, (HLI)
   ADD   A, B
   LD    E, A
   LD    A, (HLI)
   LD    H, (HL)
   LD    L, A
   LD    A, E

   LDH   (_rompage), A
   LD    (0x2000), A                   ; page # with sprite
   SET   6, H
   ADD   HL, BC
   INC   HL
   INC   HL
   INC   HL
   INC   HL

   RET

;;----------------------------------------------------------------------------
;;[[ void spr_load(ubyte dest, uword frame) ]]
;;----------------------------------------------------------------------------
;_spr_load::
;   LDHL  SP, #4                        ; get parameters
;
;   LDH   A, (_rompage)
;   PUSH  AF
;   PUSH  BC
;   LD    A, (HLD)                      ;
;   LD    B, A                          ;
;   LD    A, (HLD)                      ;
;   LD    C, A                          ; BC = frame
;   LD    A, (HL)                       ;
;   LD    D, A                          ; D = slot number
;
;   CALL  get_frame_ptr
;
;   PUSH  HL                            ; pointer to sprite pal
;
;   LD    A, D
;   ADD   A, A
;   ADD   A, A
;   LD    B, A
;   ADD   A, A
;   ADD   A, #3
;   LD    L, A
;   LD    H, #0xC0
;
;   PUSH  HL                            ; pointer to OAM data
;
;   LD    A, C
;   RRCA
;   RRCA
;   LD    H, A
;   AND   #0xC0
;   LD    L, A
;   XOR   H
;   OR    #0x40
;   LD    H, A
;
;   LD    A, D
;   RRCA
;   RRCA
;   LD    D, A
;   AND   #0xC0
;   LD    E, A
;   XOR   D
;   ADD   A, #0x80
;   LD    D, A
;
;   PUSH  BC
;
;   CALL  copy64bytes_vram
;
;   POP   BC
;
;   POP   HL
;   POP   DE
;
;   LD    A, #table_page
;   LDH   (_rompage), A
;   LD    (0x2000), A                   ; Map 4000-7FFF to cur_page
;
;   LD    A, (DE)
;
;   LD    (HLD), A
;   LD    (HL), B
;   INC   L
;   INC   L
;   INC   L
;   INC   L
;   INC   B
;   INC   B
;   LD    (HL), B
;   INC   L
;   LD    (HL), A
;
;   POP   BC
;   POP   AF
;   LDH   (_rompage), A
;   LD    (0x2000), A                   ; Map 4000-7FFF to cur_page
;
;   RET

;----------------------------------------------------------------------------
;[[ void spr_load2(ubyte dest, uword frame) ]]
;----------------------------------------------------------------------------
_spr_load::
_spr_load2::
   LDHL  SP, #4                        ; get parameters

   LDH   A, (_rompage)                 ; save rom page
   PUSH  AF                            ; save BC
   PUSH  BC                            ;

   LD    A, (HLD)                      ;
   LD    B, A                          ;
   LD    A, (HLD)                      ;
   LD    C, A                          ; BC = frame
   LD    D, (HL)                       ; D = slot number

   CALL  get_frame_ptr

   PUSH  HL                            ; pointer to sprite pal

   LD    A, D
   ADD   A, A
   ADD   A, A
   LD    B, A
   ADD   A, A
   ADD   A, #3
   LD    L, A
   LD    H, #0xC0

   PUSH  HL                            ; pointer to OAM data

   LD    A, C
   RRCA
   RRCA
   LD    H, A
   AND   #0xC0
   LD    L, A
   XOR   H
   OR    #0x40
   LD    H, A

   LD    A, D
   RRCA
   RRCA
   LD    D, A
   AND   #0xC0
   LD    E, A
   XOR   D
   ADD   A, #0x80
   LD    D, A

   PUSH  BC

   PUSH  HL
   PUSH  DE
   POP   HL
   POP   DE

   CALL  copy64bytes_vram2

   POP   BC

   POP   HL
   POP   DE

   LD    A, #table_page
   LDH   (_rompage), A
   LD    (0x2000), A                   ; Map 4000-7FFF to cur_page

   LD    A, (DE)

   LD    (HLD), A
   LD    (HL), B
   INC   L
   INC   L
   INC   L
   INC   L
   INC   B
   INC   B
   LD    (HL), B
   INC   L
   LD    (HL), A

   POP   BC
   POP   AF
   LDH   (_rompage), A
   LD    (0x2000), A                   ; Map 4000-7FFF to cur_page

   RET

;;----------------------------------------------------------------------------
;;[[ void gfx_load(ubyte dest, ubyte *ptr) ]]
;;----------------------------------------------------------------------------
;_gfx_load::
;   LDHL  SP, #2
;   PUSH  BC
;
;   LD    A, (HLI)
;   LD    B, A
;   RRCA
;   RRCA
;   LD    D, A
;   AND   #0xC0
;   LD    E, A
;   XOR   D
;   ADD   A, #0x90
;   LD    D, A                          ; DE -> destination of sprite
;   LD    A, B
;   RLCA
;   RLCA
;   RLCA
;   SBC   A, A
;   AND   #0xF0
;   ADD   A, D
;   LD    D, A
;
;   PUSH  DE
;
;   LD    A, (HLI)                      ; HL -> graphic frame
;   LD    D, (HL)
;   LD    E, A
;
;   POP   HL
;
;   CALL  copy64bytes_vram2
;
;   POP   BC
;   RET
;

copy64bytes_vram2:
   LDH   A, (_hbimode)
   OR    A
   JR    NZ, copy64bytes_vram

copy64bytes_vram3:
   LD    B, #64
gfx_write_loop_a:
   LD    A, (DE)
   RST   ldi_v_aa
   INC   DE
   DEC   B
   JR    NZ, gfx_write_loop_a
   RET

copy64bytes_vram:
   LD    B, #64
gfx_write_loop_ab:
   LD    A, (DE)
   RST   ldi_v_a
   INC   DE
   DEC   B
   JR    NZ, gfx_write_loop_ab
   RET

;; HL->source
;; DE->destination
;;modifies:
;; ABHL
;copy64bytes_vram:
;   PUSH  DE
;   LD    DE, #_sprite_blast+15
;   LD    A, E
;   AND   A, #0xF0
;   LD    E, A
;   LD    B, #0x20
;_sprite_blast_loop:
;   LD    A, (HLI)
;   LD    (DE), A
;   INC   DE
;   LD    A, (HLI)
;   LD    (DE), A
;   INC   DE
;   DEC   B
;   JR    NZ, _sprite_blast_loop
;   LD    HL, #_sprite_blast+15
;   POP   DE
;   LD    A, H
;   LDH   (P_HDMA1), A                  ; 51/52 -> source of sprite
;   LD    A, L
;   LDH   (P_HDMA2), A
;   LD    A, D
;   LDH   (P_HDMA3), A                  ; 53/54 -> destination of sprite
;   LD    A, E
;   LDH   (P_HDMA4), A
;   LD    A, #0x03                      ; Blast DMA
;   LDH   (P_HDMA5), A
;   RET

;;----------------------------------------------------------------------------
;;[[ render_bg_vline_asm ]]
;;
;; Draws a vertical line of 16x16 tiles on the vram.
;;----------------------------------------------------------------------------
;_render_bg_vline_asm::
;   PUSH  BC
;
;   LD    HL, #_mapsrc_ptr
;   LD    A, (HLI)             ; HL -> _mapsrc_ptr
;   LD    E, A
;   LD    A, (HLI)
;   LD    D, A
;
;   LD    A, (HLI)             ; HL -> _mapdest_ptr
;   LD    H, (HL)
;   LD    L, A
;
;   LD    A, (_map_vram_y)
;   ADD   A, #0xF0
;   LD    C, A
;   LD    B, #0x0A
;
;   PUSH  BC
;   PUSH  DE
;   PUSH  HL
;vdraw_tile:
;   LD    A, (DE)
;   PUSH  DE
;   ADD   A, A
;   ADD   A, A
;   LD    DE, #0x0020
;   CALL  w4vram
;   LD    (HLI), A
;   ADD   A, #0x02
;   LD    (HLD), A
;   ADD   HL, DE
;   DEC   A
;   LD    (HLI), A
;   ADD   A, #0x02
;   LD    (HLD), A
;   INC   C
;   JR    NZ, vdt_no_wrap
;   LD    D, #0xFC
;vdt_no_wrap:
;   ADD   HL, DE
;   POP   DE
;   LD    A, (_map_lenx)
;   ADD   A, E
;   LD    E, A
;   ADC   A, D
;   SUB   A, E
;   LD    D, A
;   DEC   B
;   JR    NZ, vdraw_tile
;
;   POP   HL
;   POP   DE
;   POP   BC
;
;   LD    A, #1
;   LDH   (P_VBK), A
;
;vdraw_attr:
;   LD    A, (DE)
;   PUSH  DE
;   AND   #0x3F
;   LD    E, A
;   LD    A, (_pal_ptr)
;   ADD   A, E
;   LD    E, A
;   LD    A, (_pal_ptr+1)
;   ADC   A, #0
;   LD    D, A
;;self:
;;   JR    self
;
;   LD    A, (DE)
;   OR    #0x08
;   LD    DE, #0x0020
;   CALL  w4vram
;   LD    (HLI), A
;   LD    (HLD), A
;   ADD   HL, DE
;   LD    (HLI), A
;   LD    (HLD), A
;   INC   C
;   JR    NZ, vda_no_wrap
;   LD    D, #0xFC
;vda_no_wrap:
;   ADD   HL, DE
;   POP   DE
;   LD    A, (_map_lenx)
;   ADD   A, E
;   LD    E, A
;   ADC   A, D
;   SUB   A, E
;   LD    D, A
;   DEC   B
;   JR    NZ, vdraw_attr
;
;   JP    reset_pop_ret
;
;_render_bg_hline_asm::
;   PUSH  BC
;
;   LD    HL, #_mapsrc_ptr
;   LD    A, (HLI)           ; HL -> _mapsrc_ptr
;   LD    E, A
;   LD    A, (HLI)
;   LD    D, A
;
;   LD    A, (HLI)           ; HL -> _mapdest_ptr
;   LD    H, (HL)
;   LD    L, A
;
;   LD    A, (_map_vram_x)
;   ADD   A, #0xF0
;   LD    C, A
;   LD    B, #0x0B
;
;   PUSH  HL
;   PUSH  BC
;   PUSH  DE
;   PUSH  HL
;hdraw_tile_align:
;   LD    A, (DE)
;   ADD   A, A
;   ADD   A, A
;   CALL  w4vram
;   LD    (HLI), A
;   ADD   A, #2
;   LD    (HLI), A
;   INC   C
;   CALL  Z, hdt_wrap
;   INC   DE
;   DEC   B
;   JR    NZ, hdraw_tile_align
;   POP   HL
;   POP   DE
;   LD    BC, #0x0020
;   ADD   HL, BC
;   POP   BC
;
;   PUSH  BC
;   PUSH  DE
;hdraw_tile_unalign:
;   LD    A, (DE)
;   ADD   A, A
;   ADD   A, A
;   INC   A
;   CALL  w4vram
;   LD    (HLI), A
;   ADD   A, #2
;   LD    (HLI), A
;   INC   C
;   CALL  Z, hdt_wrap
;   INC   DE
;   DEC   B
;   JR    NZ, hdraw_tile_unalign
;   POP   DE
;   POP   BC
;   POP   HL
;
;   LD    A, #1
;   LDH   (P_VBK), A
;
;   PUSH  BC
;   PUSH  DE
;   PUSH  HL
;hdraw_attr_align:
;   LD    A, (DE)
;   PUSH  DE
;   AND   #0x3F
;   LD    E, A
;   LD    A, (_pal_ptr)
;   ADD   A, E
;   LD    E, A
;   LD    A, (_pal_ptr+1)
;   ADC   A, #0
;   LD    D, A
;   LD    A, (DE)
;   OR    #8
;   CALL  w4vram
;   LD    (HLI), A
;   LD    (HLI), A
;   INC   C
;   CALL  Z, hda_wrap
;   POP   DE
;   INC   DE
;   DEC   B
;   JR    NZ, hdraw_attr_align
;   POP   HL
;   POP   DE
;   LD    BC, #0x0020
;   ADD   HL, BC
;   POP   BC
;
;hdraw_attr_unalign:
;   LD    A, (DE)
;   PUSH  DE
;   AND   #0x3F
;   LD    E, A
;   LD    A, (_pal_ptr)
;   ADD   A, E
;   LD    E, A
;   LD    A, (_pal_ptr+1)
;   ADC   A, #0
;   LD    D, A
;   LD    A, (DE)
;   OR    #8
;   CALL  w4vram
;   LD    (HLI), A
;   LD    (HLI), A
;   INC   C
;   CALL  Z, hda_wrap
;   POP   DE
;   INC   DE
;   DEC   B
;   JR    NZ, hdraw_attr_unalign
;
;   JP    reset_pop_ret
;
;hdt_wrap:
;   LD    A, B
;   LD    BC, #0xFFE0
;   ADD   HL, BC
;   LD    B, A
;   RET
;
;hda_wrap:
;   LD    DE, #0xFFE0
;   ADD   HL, DE
;   RET

;----------------------------------------------------------------------------
;[[ set_w_attr ]]
; Set the window's attribute to #7.
;----------------------------------------------------------------------------
_set_w_attr::
   PUSH  BC

   LD    A, #1
   LDH   (P_VBK), A

   LD    A, #7

   LD    HL, #0x9C00
   LD    B, L
SWA_loop:
   RST   ldi_v_a
   RST   ldi_v_a
   RST   ldi_v_a
   RST   ldi_v_a
   DEC   B
   JR    NZ, SWA_loop

reset_pop_ret:
   XOR   A
   LDH   (P_VBK), A

   POP   BC

   RET

;----------------------------------------------------------------------------
;[ JumpTable ]
;parameters:
;    A = Switch Number
; (SP) = Previous return Address
;modifies:
; ABC
;----------------------------------------------------------------------------
JumpTable::
   ADD  A, A                                      ; Word Boundary
   POP  BC                                        ; Get Return Address
   PUSH HL

   ADD  A, C                                      ; Add A to Return Address
   LD   L, A
   ADC  A, B
   SUB  L
   LD   H, A

   LD   A, (HLI)                                  ; Find address to jump to
   LD   B, (HL)
   LD   C, A

   POP  HL
   PUSH BC
   RET                                            ; Jump

;ldvram_hli_a::
;   PUSH  BC
;   LD    B, A
;ldvram_hli_a_loop:
;   LDH   A, (P_STAT)
;   LD    (HL), B
;   AND   #0x02
;   JR    NZ, ldvram_hli_a_loop
;   INC   HL
;   POP   BC
;   RET

or_a_vram::
;   PUSH  BC
;   LD    C, A
;oav_loop:
;   LDH   A, (P_STAT)
;   LD    B, (HL)
;   AND   #0x02
;   JR    NZ, oav_loop
;   LD    A, C
;   OR    B
;   POP   BC

   CP    #0xFF
   JR    Z, _done_oav_loop2

   PUSH  BC
   LD    B, #0x60
   LD    C, A
oav_loop:
   LD    A, (HL)
   CP    #0xFF
   JR    NZ, done_oav_loop
   DEC   B
   JR    NZ, oav_loop
done_oav_loop:
   OR    C
   POP   BC

;   CP    #0xFF
;   JR    NZ, _done_oav_loop2
;oav_loop2:
;   LDH   A, (P_STAT)
;   AND   #0x02
;   JR    NZ, oav_loop2
;   DEC   A

_done_oav_loop2:
   RST   ldi_v_aa
   RET

;ld_vram_a::
;   LD    (HL), A
;   CP    (HL)
;   JR    NZ, ld_vram_a
;   INC   HL
;   RET

;mov_hl_de_vram::
;mav_loop:
;   LDH   A, (P_STAT)
;   AND   #0x02
;   LD    A, (DE)
;   JR    NZ, mav_loop
;   INC   DE
;   JR    ld_vram_a

ld_a_vram::
   LD    A, (HL)
   INC   A
   JR    Z, ld_a_vram
   DEC   A
   RET

_copy_page::
   LDHL  SP, #2
   PUSH  BC

   LDH   A, (_rampage)
   PUSH  AF

   LD    A, (HLI)       ; src
   LD    D, (HL)        ; dest
   LD    E, A
   LD    B, #0x20

   LD    HL, #0xD000
copy_page_loop:
   LD    A, E
   LDH   (_rampage), A
   LDH   (P_SVBK), A
   LD    B, (HL)
   LD    A, D
   LDH   (_rampage), A
   LDH   (P_SVBK), A
   LD    (HL), B
   INC   L
   JR    NZ, copy_page_loop
   INC   H
   DEC   B
   JR    NZ, copy_page_loop

   POP   AF
   LDH   (_rampage), A
   LDH   (P_SVBK), A

   POP   BC

   RET

_STIR_B::
   LD    (HLI), A
   DEC   B
   JR    NZ, _STIR_B
   RET

_STIR_B_VRAM::
   RST   ldi_v_a
   DEC   B
   JR    NZ, _STIR_B_VRAM
   RET

_STIR::
   LD    D, A
_STIR_LOOP:
   LD    A, D
   LD    (HLI), A
   DEC   BC
   LD    A, B
   OR    C
   JR    NZ, _STIR_LOOP
   RET

_STIR_VRAM::
   LD    D, A
_STIR_LOOP_V:
   LD    A, D
   RST   ldi_v_a
   DEC   BC
   LD    A, B
   OR    C
   JR    NZ, _STIR_LOOP_V
   RET

;_LDIR_B::
;   LD    A, (HLI)
;   LD    (DE), A
;   INC   DE
;   DEC   B
;   JR    NZ, _LDIR_B
;   RET

_LDIR_B_VRAM::
   LD    A, (DE)
   RST   ldi_v_a
   INC   DE
   DEC   B
   JR    NZ, _LDIR_B_VRAM
   RET

_LDIR::
   LD    A, (HLI)
   LD    (DE), A
   INC   DE
   DEC   BC
   LD    A, B
   OR    C
   JR    NZ, _LDIR
   RET

_LDIR_VRAM_ACC::
   LD    A, (DE)
   CALL  ld_vram_a_acc
   INC   DE
   DEC   BC
   LD    A, B
   OR    C
   JR    NZ, _LDIR_VRAM_ACC
   RET

;ld_vram_a::
;   LD    (HL), A
;   CP    (HL)
;   JR    NZ, ld_vram_a
;   INC   HL
;   RET

ld_vram_a_acc::
   PUSH  BC
   LD    B, A
ldvrama_loop:
   LDH   A, (P_STAT)
   LD    (HL), B
   AND   #0x02
   JR    NZ, ldvrama_loop
   INC   HL
   POP   BC
   RET

;   PUSH  BC
;lav_loop:
;   LDH   A, (P_STAT)
;   LD    B, (HL)
;   AND   #0x02
;   JR    NZ, lav_loop
;   LD    A, B
;   POP   BC
;   RET

_title_screen::
   CALL  _title_join
   EI
   HALT
   RET
_title_join:
   CALL  do_far_noarg
   .DB   _rom_page_title
   .DW   _title_screen_real

;   LDH   A, (_rompage)
;   PUSH  AF
;   LD    A, #_rom_page_title
;   LDH   (_rompage), A
;   LD    (0x2000), A
;
;   CALL  _title_screen_real
;
;   POP   AF
;   LDH   (_rompage), A
;   LD    (0x2000), A
;
;   EI
;   HALT
;
;   RET

_title_init::
   DI

;   CALL  _SOUNDLIB_Init
;   LD    A, #1
;   LD    (playing), A
;   LD    (currentbeat), A

   CALL  do_far_noarg
   .DB   _rom_page_title
   .DW   _title_init_real

;   LDH   A, (_rompage)
;   PUSH  AF
;
;   LD    A, #_rom_page_title
;   LDH   (_rompage), A
;   LD    (0x2000), A
;
;   CALL  _title_init_real
;
;   POP   AF
;   LDH   (_rompage), A
;   LD    (0x2000), A
;
;   RET

; pointer within = dest & 16383
; rom page = (dest >> 14) + event_page
;
;----------------------------------------------------------------------------
;[[ ubyte get_event_byte(ubyte *ptr) ]]
;----------------------------------------------------------------------------
_get_event_byte::
   LDHL  SP, #3

   LDH   A, (_rompage)
   LD    D, A

   LD    A, (HLD)
   LD    L, (HL)
   LD    E, A

   AND   #0x3F
   LD    H, A
   SET   6, H

   XOR   E
   RLCA
   RLCA
   ADD   #_rom_page_events

   LDH   (_rompage), A
   LD    (0x2000), A

   LD    E, (HL)

   LD    A, D
   LDH   (_rompage), A
   LD    (0x2000), A

   RET

;----------------------------------------------------------------------------
;[[ void dialog_setup(uword text) ]]
;----------------------------------------------------------------------------
_dialog_setup::
   LDHL  SP, #2
   LD    A, (HLI)
   LD    H, (HL)
   LD    L, A

;   LD    A, H
;   OR    A
;   JR    Z, noblah
;;;;   LD    B, B
;noblah:

   LD    D, H
   LD    E, L
   ADD   HL, DE
   ADD   HL, DE

   SET   6, H

;   OR    A
;   JR    Z, noblah2
;;;;   LD    B, B
;noblah2:

   LDH   A, (_rompage)
   LD    D, A

   LD    A, #_rom_page_text
   LDH   (_rompage), A
   LD    (0x2000), A

   ADD   A, (HL)
   LD    (_dialogue_page), A

   INC   HL
   LD    A, (HLI)
   LD    E, (HL)

   SET   6, E

   LD    HL, #_dialogue_ptr
   LD    (HLI), A
   LD    (HL), E

   LD    A, D
   LDH   (_rompage), A
   LD    (0x2000), A

   RET

;----------------------------------------------------------------------------
;[[ ubyte get_dialog_byte() ]]
;----------------------------------------------------------------------------
_get_dialog_byte::
   LDH   A, (_rompage)
   LD    D, A

   LD    HL, #_dialogue_page
   LD    A, (HLI)
   LDH   (_rompage), A
   LD    (0x2000), A

   LD    A, (HLI)
   LD    H, (HL)
   LD    L, A

   LD    A, (HLI)
   LD    E, A

   LD    A, L
   LD    (_dialogue_ptr), A
   LD    A, H
   LD    (_dialogue_ptr+1), A

   LD    A, D
   LDH   (_rompage), A
   LD    (0x2000), A
   RET

;----------------------------------------------------------------------------
;[[ ubyte get_dialog_strlen() ]]
;----------------------------------------------------------------------------
_get_dialog_strlen::
   LDH   A, (_rompage)
   PUSH  AF
   PUSH  BC

   LD    HL, #_dialogue_page
   LD    A, (HLI)
   LDH   (_rompage), A
   LD    (0x2000), A

   LD    C, A

   LD    A, (HLI)
   LD    H, (HL)
   LD    L, A

   LD    B, #0x00
   CALL  SL_loop

   LD    E, B

   POP   BC
   POP   AF
   LDH   (_rompage), A
   LD    (0x2000), A
   RET

_checkwindowlen::
   CALL  _get_dialog_strlen            ; E=strlen
   LD    A, (_barg1)                   ; A=current x offset
   ADD   A, E                          ; A=offset after adding strlen
   ADD   A, #-145
   SBC   A, A
   LD    E, A
   RET

;----------------------------------------------------------------------------
;[[ void get_event(byte *dest, uword event_num) ]]
;----------------------------------------------------------------------------
;_get_event::
;   LDHL  SP, #2
;
;   LDH   A, (_rompage)
;   PUSH  AF
;
;   PUSH  BC
;
;   LD    A, (HLI)
;   LD    E, A
;   LD    A, (HLI)
;   LD    D, A                          ; dest
;
;   LD    A, (HLI)
;   LD    H, (HL)
;   LD    L, A                          ; event num
;   ADD   HL, HL
;   ADD   HL, HL
;
;   PUSH  HL
;
;   LD    HL, #0x2006
;   LD    A, #table_page                ; Map 4000-7FFF to cur_page
;   LDH   (_rompage), A
;   LD    (HL), A
;   ADD   HL, HL
;   LD    A, (HLI)                      ; Event base page
;   LD    C, (HL)
;   INC   HL
;   LD    B, (HL)                       ; Pointer to event table
;   SET   6, B
;
;   POP   HL
;
;   ADD   HL, BC
;
;   ADD   A, (HL)                       ; Event page
;   LD    C, A
;   INC   HL
;   LD    A, (HLI)
;   LD    B, A
;   LD    A, (HLI)
;   LD    L, (HL)
;   LD    H, A                          ; Pointer within event page
;   SET   6, H
;   LD    A, L
;   LD    L, B
;   LD    B, A
;
;   LD    A, C
;
;   LDH   (_rompage), A
;   LD    (0x2000), A                   ; page # with sprite
;
;   CALL  _LDIR_B
;
;   POP   BC
;
;   POP   AF
;
;   LDH   (_rompage), A
;   LD    (0x2000), A                   ; page # with sprite
;   RET

;----------------------------------------------------------------------------
;[[ Bresenham ]]
;parameters:
; B = run  HL -> Struct Bresenham
;
; struct Bresenham
; delta, error, increment, current
;
;returns:
; updated Struct, HL -> next Struct Bresenham
;----------------------------------------------------------------------------
Bresenham::
   LD    A, (HLI)
   ADD   A, (HL)
   JR    NC, bh_noadd
   SUB   B
   LD    (HLI), A
   LD    A, (HLI)
   ADD   A, (HL)
   LD    (HLI), A
   RET

Bresenham_Back::
   INC   HL
   LD    A, (HLD)
   ADD   A, B
   SUB   (HL)
   INC   HL

   JR    NC, bhb_add
   LD    (HLI), A

   LD    A, (HLI)
   CPL
   ADC   A, (HL)
   LD    (HLI), A
   RET

bhb_add:
   SUB   B
bh_noadd:
   LD    (HLI), A
   INC   HL
   LD    A, (HLI)
   RET

; hblank master interrupt
; Input:
;   _hbimode = mode of int
;    0=nothing 1=gradient 2=spr64 3=dialogue 4=slm 5=bx 6=tilt 7=slm2
_hblank_int::
   PUSH  AF
   LDH   A, (_hbimode)
   OR    A
   JR    Z, _hblank_int_exit

;   PUSH  BC
;   PUSH  DE
;   PUSH  HL
;   LDH   A, (_hbimode)
   DEC   A
   JP    Z, _grad_int
   DEC   A
   JP    Z, _spr64hblank_int
   DEC   A
   JP    Z, _dialogue_hbl
   DEC   A
   JP    Z, _slm_hbl
   DEC   A
   JP    Z, _scl_hbl
   DEC   A
   JP    Z, _tilt_hbl
   DEC   A
   JP    Z, _slm2_hbl
   DEC   A
   JP    Z, _ending_int

_hbl_end::
;   POP   HL
;   POP   DE
;   POP   BC
_hblank_int_exit:
   POP   AF
   RETI

_ending_int::                          ;19 more than scl hbl
   LDH   A, (_rompage)                 ;3
   PUSH  AF                            ;4

   LD    A, #_rom_page_title           ;2
   LDH   (_rompage), A                 ;3
   LD    (0x2000), A                   ;4

   PUSH  HL                            ;4
   CALL  _ending_int_real              ;6
   POP   HL                            ;3

   POP   AF                            ;3
   LDH   (_rompage), A                 ;3
   LD    (0x2000), A                   ;4

   JP    _hbl_end                      ;4

; set hblank mode
; Input:
;   hblank_mode(UBYTE mode);
_hblank_mode::
   LDHL  SP, #2
   LD    A, (HL)
   JR    _hbi_join

_end_deinit::
   XOR   A
   LD    (_endingmode), A

   LD    A, #0b11000111          ; 2
   LDH   (P_LCDC), A             ; 3
_hblank_init::
   XOR   A
_hbi_join:
   LDH   (_hbimode), A
   RET

; vblank master interrupt
; Input:
;   _vbimode = mode of int
;    0=nothing 1=flicker 2=battle 
_vblank_int::
   PUSH  AF
   PUSH  HL
   PUSH  BC
   PUSH  DE

;   CALL  _SOUNDLIB_Update

   LD    HL, #_vblank_timer
   INC   (HL)

   LDH   A, (_vbimode)
   OR    A
   JR    Z, _vblank_int_exit

   LDH   A, (_vbimode)
   DEC   A
;   JP    Z, _flicker_int
   JP    Z, _int_owe
   DEC   A
   JP    Z, _bspr_int
   DEC   A
   JP    Z, _dialogue_vbl
   DEC   A
   JP    Z, _slm_vbl
   DEC   A
;   JP    Z, _bx_vbl
   DEC   A
   JP    Z, _int_owe2

_vbl_end::
   LD    A, (sound_play)
   OR    A
   CALL  NZ, _SOUNDLIB_Update

;   LD    A, (_scl_on)
;   OR    A
;   JR    NZ, _vblank_int_quit

   LD    HL, #using_auto_proc_ptr
   LD    A, (HLI)
   OR    A
   JR    NZ, _vblank_int_quit

   LDH   A, (_rompage)
   PUSH  AF
   LD    A, (HLI)
   LDH   (_rompage), A
   LD    (0x2000), A

   LD    A, (HLI)
   LD    H, (HL)
   LD    L, A
   OR    H
   CALL  NZ, _callhl

   POP   AF
   LDH   (_rompage), A
   LD    (0x2000), A

_vblank_int_quit:
   LD    A, (_reset_ok)
   OR    A
   JR    Z, _vblank_noreset

   LD    A, #0x10                      ; mask out buttons
   LDH   (P_P1), A

   LDH   A, (P_P1)                     ; read key
   AND   #0x0F
   JR    Z, _reset_gb

_vblank_noreset:

;  13 clocks/vblank
; +11 clocks/second
; +11 clocks/minute
; +11 clocks/hour
; + 8 clocks if 99 hrs

   LD    HL, #_gametime+3
   INC   (HL)                          ; vblanks
   LD    A, (HL)
   SUB   #60
   JR    C, _gt_done
   LD    (HLD), A
   INC   (HL)                          ; seconds
   LD    A, (HL)
   SUB   #60
   JR    C, _gt_done
   LD    (HLD), A
   INC   (HL)                          ; minutes
   LD    A, (HL)
   SUB   #60
   JR    C, _gt_done
   LD    (HLD), A
   INC   (HL)                          ; hours
   LD    A, (HL)
   SUB   #100
   JR    C, _gt_done
   DEC   (HL)
   INC   HL
   LD    (HL), #59
_gt_done:

   JR    _vbi_int_exit2

_vblank_int_exit:
   LD    A, (sound_play)
   OR    A
   CALL  NZ, _SOUNDLIB_Update

_vbi_int_exit2:
;   LD    A, (_debug_on)    ;;;;;;;;;;;;;;;;;;;;; temp
;   OR    A                 ;;;;;;;;;;;;;;;;;;;;;
;   JR    NZ, debug_stuff   ;;;;;;;;;;;;;;;;;;;;;

_vbi_int_exit3:
;   CALL  _randfunc

   POP   DE
   POP   BC
   POP   HL
   POP   AF

   RETI

;debug_stuff:               ;;;;;;;;;;;;;;;;;;;;; temp
;   LD    HL, #_debug_ptr   ; 0xD000 - 0xDFFF
;   LD    A, (HLI)          ; 0x11010000 - 0x11100000
;   LD    D, (HL)
;   LD    E, A
;
;   LD    C, #70
;_debug_loop:
;   LD    A, #3
;   LDH   (P_SVBK), A
;
;   LD    A, (DE)
;   LD    B, A
;
;   LD    A, #1
;   LDH   (P_SVBK), A
;
;   LD    A, (DE)
;   CP    B
;   JR    Z, _debug_right
;   LD    B, B
;_debug_right:
;   INC   DE
;   LD    A, D
;;   CP    #0xE0
;   CP    #0xD4
;   JR    NZ, _debug_ok_top_byte
;
;   LD    D, #0xD0
;_debug_ok_top_byte:
;   DEC   C
;   JR    NZ, _debug_loop
;
;   LD    A, D
;   LD    (HLD), A
;   LD    (HL), E
;
;   LDH   A, (_rampage)
;   LDH   (P_SVBK), A
;
;   JR    _vbi_int_exit3

; proc_ptr
_auto_proc::
   LDHL  SP, #2
   LD    A, #1
   LD    (using_auto_proc_ptr), A
   LDH   A, (_rompage)
   LD    (auto_proc_rom), A
   LD    A, (HLI)
   LD    (auto_proc_ptr), A
   LD    A, (HLI)
   LD    (auto_proc_ptr+1), A
   XOR   A
   LD    (using_auto_proc_ptr), A
   RET

_reset_gb::
   DI

;   ;;;;;;;;;;;;;;;;;;
;   JR    crash_stuff
;   ;;;;;;;;;;;;;;;;;;

   LDH   A, (_rscx)
   LDH   (P_SCX), A

   LD    A, #167
   LDH   (P_WX), A

   LD    A, #_rom_page_data
   LD    (0x2000), A

   LD    HL, #_pfs_targetbgr
   LD    A, #0xFF
   LD    (HLI), A
   LD    (HL), A

   LD    BC, #_pal_cur
   LD    DE, #_whitepal
   LD    A, #32
   CALL  _pfs_setup_asm

   LD    B, #32

_waitforvblloop:
   LDH   A, (P_LY)
   CP    #144
   JR    NZ, _waitforvblloop
   CALL  _pfs_step
   DEC   B
   JR    NZ, _waitforvblloop

   CALL  _clear_vram

wait_let_go:
   LDH   A, (P_P1)                     ; read key
   AND   #0x0F
   JR    Z, wait_let_go

   LD    HL, #__cpu
   LD    A, (HLI)
   LD    B, (HL)

   LD    SP, #0xFFFE
   JP    0x0100

_clear_vram_1:
   LD    HL, #0x8000
   LD    BC, #0x2000
   XOR   A
   JP    _STIR_VRAM

crash_stuff:
   DI

   LD    A, #_rom_page_title
   LDH   (_rompage), A
   LD    (0x2000), A

   JP    real_crash_stuff

_clear_vram::
   PUSH  BC

   LD    A, #1
   LDH   (P_VBK), A

   CALL  _clear_vram_1

   XOR   A
   LDH   (P_VBK), A

   CALL  _clear_vram_1

   POP   BC
   RET

overlay:
;   .DB   #0b00000000,#0b00000000
;   .DB   #0b00111111,#0b00111111
;   .DB   #0b01111111,#0b01111111
;   .DB   #0b01111111,#0b01111101
;   .DB   #0b01111111,#0b01111011
;   .DB   #0b01111111,#0b01110111
;   .DB   #0b01111111,#0b01101111
;   .DB   #0b01111111,#0b01111111
;
;   .DB   #0b01111111,#0b01111111
;   .DB   #0b01111111,#0b01111110
;   .DB   #0b01111111,#0b01111101
;   .DB   #0b01111111,#0b01111011
;   .DB   #0b01111111,#0b01111111
;   .DB   #0b01111111,#0b01111111
;   .DB   #0b00111111,#0b00111111
;   .DB   #0b00000000,#0b00000000
;
;   .DB   #0b00000000,#0b00000000
;   .DB   #0b11111100,#0b11111100
;   .DB   #0b11111110,#0b11111110
;   .DB   #0b11111110,#0b11111110
;   .DB   #0b11111110,#0b11110110
;   .DB   #0b11111110,#0b11101110
;   .DB   #0b11111110,#0b11011110
;   .DB   #0b11111110,#0b10111110
;
;   .DB   #0b11111110,#0b01101110
;   .DB   #0b11111110,#0b11011110
;   .DB   #0b11111110,#0b10111110
;   .DB   #0b11111110,#0b11111110
;   .DB   #0b11111110,#0b11111110
;   .DB   #0b11111110,#0b11111110
;   .DB   #0b11111100,#0b11111100
;   .DB   #0b00000000,#0b00000000

;   .DB   #0b01111110,#0b01111110
;   .DB   #0b10111101,#0b10111101
;   .DB   #0b11011011,#0b11011011
;   .DB   #0b11100111,#0b11100111
;   .DB   #0b11100111,#0b11100111
;   .DB   #0b11011011,#0b11011011
;   .DB   #0b10111101,#0b10111101
;   .DB   #0b01111110,#0b01111110
;
;   .DB   #0b01111110,#0b01111110
;   .DB   #0b10111101,#0b10111101
;   .DB   #0b11011011,#0b11011011
;   .DB   #0b11100111,#0b11100111
;   .DB   #0b11100111,#0b11100111
;   .DB   #0b11011011,#0b11011011
;   .DB   #0b10111101,#0b10111101
;   .DB   #0b01111110,#0b01111110
;
;   .DB   #0b01111110,#0b01111110
;   .DB   #0b10111101,#0b10111101
;   .DB   #0b11011011,#0b11011011
;   .DB   #0b11100111,#0b11100111
;   .DB   #0b11100111,#0b11100111
;   .DB   #0b11011011,#0b11011011
;   .DB   #0b10111101,#0b10111101
;   .DB   #0b01111110,#0b01111110
;
;   .DB   #0b01111110,#0b01111110
;   .DB   #0b10111101,#0b10111101
;   .DB   #0b11011011,#0b11011011
;   .DB   #0b11100111,#0b11100111
;   .DB   #0b11100111,#0b11100111
;   .DB   #0b11011011,#0b11011011
;   .DB   #0b10111101,#0b10111101
;   .DB   #0b01111110,#0b01111110

scroll_update::
;   LD    A, #1
;   LDH   (P_VBK), A
;
;   LD    BC, #_owe_pantile
;   LD    HL, #overlay
;   LD    DE, #_owe_pantile2
;
;   LD    A, #0x40
overlaytile:
;   PUSH  AF
;   LD    A, (BC)
;   AND   (HL)
;   LD    (DE), A
;   INC   BC
;   INC   DE
;   INC   HL
;   POP   AF
;   DEC   A
;   JR    NZ, overlaytile
;
;   LD    BC, #_owe_pantile2
;   LD    HL, #0x9000
;   CALL  gfxl_abs_asm
;
;   XOR   A
;   LDH   (P_VBK), A
;   RET

scroll_update_time::
   LD    HL, #_owe_pancount
   INC   (HL)
   LD    A, (HL)
   AND   #31
   JP    Z, _rotate_d
   INC   A
   AND   #3
   JP    Z, _rotate_l
   RET

;  -> 0x0040
;  PUSH  AF                            ;1
;  LDH   A, (_vbimode)                 ;2
;  OR    A                             ;1
;  JR    NZ, _vblank_int               ;2
;  POP   AF                            ;1
;  RETI                                ;1
;  -> 0x0048
;  PUSH  AF                            ;1
;  LDH   A, (_hbimode)                 ;2
;  OR    A                             ;1
;  JR    NZ, _hblank_int               ;2
;  POP   AF                            ;1
;  RETI                                ;1


; set vblank mode
; Input:
;   vblank_mode(UBYTE mode);
_vblank_mode::
   LDHL  SP, #0x02
   LD    A, (HL)
   LDH   (_vbimode), A
   RET

_vblank_init::
   XOR   A
   LDH   (_vbimode), A

;   PUSH  BC
;
;   LD    HL, #_hiram_routine_begin
;   LD    DE, #0xFF80
;   LD    B, #_hiram_routine_end - _hiram_routine_begin
;
;   CALL  _LDIR_B
;
;   POP   BC
   RET

_LDIR_B::
   LD    A, (HLI)
   LD    (DE), A
   INC   DE
   DEC   B
   JR    NZ, _LDIR_B
   RET

_oam_dma_rom::
   LDH   (P_DMA), A
   LD    A, #0x28
_oam_dma_delay:                        ; wait 160us for DMA to complete
   DEC   A
   JR    NZ, _oam_dma_delay
   RET

;_crash::
;   LD    HL, #_hiram_routine_begin
;   LD    DE, #0xD000
;   LD    B, #_hiram_routine_end - _hiram_routine_begin
;
;_copy_hiram_loop2:
;   LD    A, (HLI)
;   LD    (DE), A
;   INC   DE
;   DEC   B
;   JR    NZ, _copy_hiram_loop2
;
;   JP    0xD000
;
_hiram_routine_begin:
;_crash_rom::
;   DI
;   LDH   A, (_rscx)
;   LDH   (P_SCX), A
;   INC   A
;   LDH   (_rscx), A
;w4vbl_loop2:
;   LDH   A, (P_LY)
;   CP    #144
;   JR    C, w4vbl_loop2
;w4vbl_loop3:
;   LDH   A, (P_LY)
;   CP    #144
;   JR    NC, w4vbl_loop3
;   LD    A, (0x0000)
;   CP    #0xC9
;   JR    Z, _crash_rom
;
;_crash_rom2:
;   LDH   A, (_rscx)
;   LDH   (P_SCX), A
;   INC   A
;   LDH   (_rscx), A
;w4vbl_loop4:
;   LDH   A, (P_LY)
;   CP    #144
;   JR    C, w4vbl_loop4
;w4vbl_loop5:
;   LDH   A, (P_LY)
;   CP    #144
;   JR    NC, w4vbl_loop5
;   LD    A, (0x0000)
;   INC   A
;   JR    Z, _crash_rom2
;
;;   LD    BC, #0x1180
;;   PUSH  BC
;;   POP   AF
;;   LD    SP, #0xFFFE
;;   LD    BC, #0x0013
;;   LD    DE, #0x0008
;;   LD    HL, #0x014D
;;   EI
;;   JP    0x0100
;
;   LDH   A, (_rompage)
;   LD    (0x2000), A
;   XOR   A
;   LD    (0x3000), A
;   EI
;   RET
_hiram_routine_end:

; General

   .include "dfs.s"                    ; Dynamic Font System
   .include "gradient.s"               ; Gradient Routines
   .include "pfs.s"                    ; Fading Routines
   .include "math24.s"                 ; 24-bit Math
   .include "tile.s"                   ; Window backup routines
   .include "menu.s"                   ; Window drawing functions
   .include "io.s"                     ; Memory I/O
   .include "scl.s"                    ; Scanline Scaling
   .include "brw.s"                    ; Bresenham Word
   .include "bre.s"                    ; Bresenham (byte)
   .include "sound.s"                  ; Sound Library
   .include "flag.s"                   ; Save/Load Menu

; Infinity Specific

   .include "slm.s"                    ; Save/Load Menu
   .include "mgs.s"                    ; Menu graphics system
   .include "bspr.s"                   ; Battle sprite system
   .include "owe.s"                    ; Overworld engine
   .include "owe2.s"                   ; Overworld engine 2
   .include "ltileset.s"               ; Load Tileset
;   .include "itm.s"                    ; Item Interface

;   .include "trans.s"                  ; Battle Transition
;   .include "flicker.s"

_flicker_init::
_flicker_off::
_flicker_on::
;_bx_hbl::
;_bx_vbl::
   RET

;----------------------------------------------------------------------------
; trig stuff
;----------------------------------------------------------------------------
_getsin::
   LDHL  SP, #2
   LD    L, (HL)
   JR    _getsin_join

_getcos::
   LDHL  SP, #2
   LD    A, (HL)
   ADD   A, #64
   LD    L, A
_getsin_join:

   LDH   A, (_rompage)
   PUSH  AF
   LD    A, #_rom_page_title
   LDH   (_rompage), A
   LD    (0x2000), A

   CALL  _real_getsin
_done_trig_join:
   POP   AF
   LDH   (_rompage), A
   LD    (0x2000), A
   RET

_getarctan::
   LDHL  SP, #2
   LDH   A, (_rompage)
   PUSH  AF
   PUSH  BC
   LD    A, #_rom_page_title
   LDH   (_rompage), A
   LD    (0x2000), A
   LD    A, (HLI)
   LD    B, (HL)
   LD    C, A
   CALL  _real_getarctan
   POP   BC
   JR    _done_trig_join

_sqrt::
   LDHL  SP, #2
   LD    A, (HLI)
   LD    H, (HL)
   LD    L, A

   XOR   A
   LD    D, A
   LD    E, A                          ; A = 0, DE = 0

_sqrt_hl_loop:
   DEC   DE                            ; Increase difference of squares
   ADD   HL, DE                        ; Compare with difference of squares
   JR    NC, _sqrt_finished            ; Leave if we were under it
   INC   A                             ; Increase result
   DEC   E                             ; Difference of squares needs to be
   JR    _sqrt_hl_loop                 ;  increased by 2 each loop

_sqrt_finished:
   LD    E, A
   RET

;----------------------------------------------------------------------------
; ubyte loadmapbase(ubyte mapnum)
;----------------------------------------------------------------------------
_loadmapbase::
   LDHL  SP, #2                        ; E=mapnum
   LD    E, (HL)                       ;

   PUSH  BC                            ;
   LDH   A, (_rompage)                 ; save current rompage
   PUSH  AF                            ;

   LD    HL, #0xD000                   ; clear out map
   LD    BC, #0x1000                   ;
   XOR   A                             ;
   CALL  _STIR                         ;

   LD    HL, #0x2004                   ;
   LD    A, #table_page                ;
   LDH   (_rompage), A                 ;
   LD    (HL), A                       ; set table_page
   ADD   HL, HL                        ;

   LD    A, (HLI)                      ; B=map start page [4008]
   LD    B, A                          ;

   LD    A, (HLI)                      ; DE->map data     [4009/400A]
   LD    D, (HL)                       ;
   LD    H, E                          ; H=mapnum
   LD    L, E                          ; L=mapnum
   LD    E, A                          ;

   PUSH  HL                            ;

   LD    H, #0x08                      ; HL->map data+mapnum*8 [0x4000]
   ADD   HL, HL                        ;
   ADD   HL, HL                        ;
   ADD   HL, HL                        ;
   ADD   HL, DE                        ;

   LD    A, (HLI)                      ; A=relative rom page
   ADD   A, B                          ;
   LD    E, (HL)                       ; DE->map on rom page
   INC   HL                            ;
   LD    D, (HL)                       ;
   INC   HL                            ;
   LD    B, (HL)                       ; B=xsize
   INC   HL                            ;
   LD    C, (HL)                       ; C=ysize
   INC   HL                            ;
   PUSH  AF                            ; save rom page

   LD    A, B                          ; save C map xsize
   LD    (_mapw), A                    ;
   LD    A, C                          ; save C map ysize
   LD    (_maph), A                    ;

   PUSH  DE                            ; save pointer
   LD    A, (HLI)                      ; A=tileset number

   LD    (_tileset_cur), A             ; save C tileset number

   LD    E, (HL)                       ; E=info byte
   PUSH  DE                            ;
   PUSH  BC                            ;
   CALL  _loadtileset                  ; Load the tileset
   POP   BC                            ;
   POP   DE                            ;
   POP   HL                            ; HL->map on rom page
   POP   AF                            ;

   LDH   (_rompage), A                 ;
   LD    (0x2000), A                   ; set table_page

   POP   AF                            ; A=mapnum
   LD    (_map_cur), A                 ; save C current map

   PUSH  DE                            ; save info byte

   OR    A                             ;
   JR    Z, load_overworld             ;

   SET   6, H                          ; HL->map

   LD    A, E                          ;
   AND   #3                            ; get dimensions
   JR    Z, dim64_64                   ;
   DEC   A                             ;
   JR    Z, dim32_128                  ;
   LD    A, #0x80                      ; 128x32 assumed
   JR    dim_join
dim64_64:
   LD    A, #0x40
   JR    dim_join
dim32_128:
   LD    A, #0x20
dim_join:
   LD    (_maplenx), A                 ; save C maplenx

   LD    D, A
   LD    A, (_protect_rampages)
   OR    A
   JR    NZ, done_loading
   LD    A, D

   SUB   B                             ; A=add after each row
   LD    DE, #0xD000

; A=add after each row,B=xsize,C=ysize (counter),DE->buffer,HL->map

   PUSH  AF
maploadvloop:
   PUSH  BC
maploadhloop:
   LD    A, (HLI)                      ; get byte from map
   LD    (DE), A                       ; save to buffer
   INC   DE                            ; point to next byte in map and buffer
   DEC   B
   JR    NZ, maploadhloop
   POP   BC
   POP   AF
   PUSH  AF
   ADD   A, E                          ; DE->next row of buffer
   LD    E, A
   ADC   A, D
   SUB   E
   LD    D, A
   DEC   C
   JR    NZ, maploadvloop
   POP   AF

   JR    done_loading

load_overworld:
   LD    A, (_protect_rampages)
   OR    A
   JR    NZ, done_loading

   LDH   A, (_rampage)
   PUSH  AF

   LD    HL, #0x4000
   LD    B, #0x04
   LD    DE, #_map_order_table

_load_ovw_loop2:
   LD    A, (DE)
   INC   DE
   LDH   (_rampage), A
   LDH   (P_SVBK), A

   PUSH  DE

   LD    DE, #0xD000
   LD    C, #0x10
_load_ovw_loop:
   LD    A, (HLI)
   LD    (DE), A
   INC   E
   JR    NZ, _load_ovw_loop
   INC   D
   DEC   C
   JR    NZ, _load_ovw_loop

   POP   DE

   DEC   B
   JR    NZ, _load_ovw_loop2

   POP   AF
   LDH   (_rampage), A
   LDH   (P_SVBK), A

done_loading:
   POP   DE                            ; E=info byte

   POP   AF                            ; get original page
   LDH   (_rompage), A                 ;
   LD    (0x2000), A                   ; set rom page

   LD    A, E                          ;
   AND   #4                            ;
   ADD   A, #255                       ;
   SBC   A, A                          ;
   INC   A                             ;
   LD    (_allow_scroll), A            ; save C allow_scroll

   POP   BC

   RET

;----------------------------------------------------------------------------
; far calls
;----------------------------------------------------------------------------
do_far_noarg::
   POP   HL
   LDH   A, (_rompage)
   PUSH  AF
   LD    A, (HLI)
   LDH   (_rompage), A
   LD    (0x2000), A
   LD    A, (HLI)
   LD    H, (HL)
   LD    L, A
   CALL  _callhl
   POP   AF
   LDH   (_rompage), A
   LD    (0x2000), A
   RET

do_far_arg::
   LD    A, (_backup_rompage)
   INC   A
   JR    Z, _far_ok
   LD    B, B
_far_ok:
   POP   HL                            ;
   LDH   A, (_rompage)                 ; save current rom page
   LD    (_backup_rompage), A          ;

   LD    A, (HLI)                      ; set rom page
   LDH   (_rompage), A                 ;
   LD    (0x2000), A                   ;

   LD    A, (HLI)                      ; HL=farcall address
   LD    H, (HL)                       ;
   LD    L, A                          ;

   POP   DE                            ; hack return address

   PUSH  HL                            ; save farcall pointer
   LD    HL, #_backup_return           ;
   LD    A, E                          ;
   LD    (HLI), A                      ;
   LD    (HL), D                       ; save return address for later
   POP   HL                            ; restore farcall pointer

   CALL  _callhl                       ; call function

   LD    HL, #_backup_return           ; reget return address
   LD    A, (HLI)                      ;
   LD    H, (HL)                       ;
   LD    L, A                          ;

   LD    A, (_backup_rompage)          ;
   LDH   (_rompage), A                 ;
   LD    (0x2000), A                   ;

   LD    A, #0xFF
   LD    (_backup_rompage), A

_callhl:
   JP    (HL)                          ;

;;void get_dotext();
;_f_get_dotext::
;   CALL  do_far_noarg
;   .DB   _rom_page_event
;   .DW   _get_dotext

;----------------------------------------------------------------------------
;_rom_page_event
;----------------------------------------------------------------------------
;void lose();
;void sync_party();
;void draw_dudes();
;void resync();
;----------------------------------------------------------------------------
_f_lose::
   CALL  do_far_noarg
   .DB   _rom_page_event
   .DW   _lose

_f_sync_party::
   CALL  do_far_noarg
   .DB   _rom_page_event
   .DW   _sync_party

_f_draw_dudes::
   CALL  do_far_noarg
   .DB   _rom_page_event
   .DW   _draw_dudes

_f_resync::
   CALL  do_far_noarg
   .DB   _rom_page_event
   .DW   _resync

;----------------------------------------------------------------------------
;_rom_page_event2
;----------------------------------------------------------------------------
;byte get_action2();
;void poem();
;void setaluthapal(ubyte map);
;----------------------------------------------------------------------------
_f_get_action2::
   CALL  do_far_noarg
   .DB   _rom_page_event2
   .DW   _get_action2

_f_poem::
   CALL  do_far_noarg
   .DB   _rom_page_event2
   .DW   _poem

_f_setaluthapal::
   CALL  do_far_arg
   .DB   _rom_page_event2
   .DW   _setaluthapal

;----------------------------------------------------------------------------
;_rom_page_battle1
;----------------------------------------------------------------------------
;void wipe_menu();
;void wipe_text();
;void wipe_gem();
;void bautoproc();
;void player_cancel();
;
;ubyte binqueue(ubyte guy);
;----------------------------------------------------------------------------
_f_wipe_menu::
   CALL  do_far_noarg
   .DB   _rom_page_battle1
   .DW   _wipe_menu

_f_wipe_text::
   CALL  do_far_noarg
   .DB   _rom_page_battle1
   .DW   _wipe_text

_f_wipe_gem::
   CALL  do_far_noarg
   .DB   _rom_page_battle1
   .DW   _wipe_gem

_f_bautoproc::
   CALL  do_far_noarg
   .DB   _rom_page_battle1
   .DW   _bautoproc

_f_player_cancel::
   CALL  do_far_noarg
   .DB   _rom_page_battle1
   .DW   _player_cancel

;_f_binqueue::
;   CALL  do_far_arg
;   .DB   _rom_page_battle1
;   .DW   _binqueue

;----------------------------------------------------------------------------
;_rom_page_battle2
;----------------------------------------------------------------------------
;ubyte bchain_do();
;void bchain_donext();
;void bchain_reset();
;void bchain_stop();
;void text_update();
;
;void bchain_add(ubyte guy,ubyte action,ubyte dir,word arg1,word arg2,word arg3);
;ubyte get_4waydir(ubyte bdir);
;void text_init(word x,word y,word value,ubyte mode);
;void bpro_set(ubyte slot,word x1,word y1,word x2,word y2,ubyte steps);
;void bpro_next(ubyte slot);
;----------------------------------------------------------------------------
_f_bchain_do::
   CALL  do_far_noarg
   .DB   _rom_page_battle2
   .DW   _bchain_do

_f_bchain_donext::
   CALL  do_far_noarg
   .DB   _rom_page_battle2
   .DW   _bchain_donext

_f_bchain_reset::
   CALL  do_far_noarg
   .DB   _rom_page_battle2
   .DW   _bchain_reset

_f_bchain_stop::
   CALL  do_far_noarg
   .DB   _rom_page_battle2
   .DW   _bchain_stop

_f_text_update::
   CALL  do_far_noarg
   .DB   _rom_page_battle2
   .DW   _text_update

_f_bchain_add::
   CALL  do_far_arg
   .DB   _rom_page_battle2
   .DW   _bchain_add

_f_get_4waydir::
   CALL  do_far_arg
   .DB   _rom_page_battle2
   .DW   _get_4waydir

;_f_baction_poof::
;   CALL  do_far_arg
;   .DB   _rom_page_battle2
;   .DW   _baction_poof

_f_text_init::
   CALL  do_far_arg
   .DB   _rom_page_battle2
   .DW   _text_init

_f_bpro_set::
   CALL  do_far_arg
   .DB   _rom_page_battle2
   .DW   _bpro_set

_f_bpro_next::
   CALL  do_far_arg
   .DB   _rom_page_battle2
   .DW   _bpro_next

;----------------------------------------------------------------------------
;_rom_page_battle3
;----------------------------------------------------------------------------
;void bmapsetup();
;void bcoloroff();
;
;void gem_animate(ubyte guy);
;byte get_player_input(ubyte guy);
;void bcoloron(ubyte color);
;void bgetreal(struct BDUDE *bch);
;void bshowail(ubyte guy, ubyte flop);
;void setbosspal(ubyte x);
;----------------------------------------------------------------------------
_f_bmapsetup::
   CALL  do_far_noarg
   .DB   _rom_page_battle3
   .DW   _bmapsetup

_f_bcoloroff::
   CALL  do_far_noarg
   .DB   _rom_page_battle3
   .DW   _bcoloroff

_f_gem_animate::
   CALL  do_far_arg
   .DB   _rom_page_battle3
   .DW   _gem_animate

_f_get_player_input::
   CALL  do_far_arg
   .DB   _rom_page_battle3
   .DW   _get_player_input

_f_bcoloron::
   CALL  do_far_arg
   .DB   _rom_page_battle3
   .DW   _bcoloron

_f_bgetreal::
   CALL  do_far_arg
   .DB   _rom_page_battle3
   .DW   _bgetreal

_f_bshowail::
   CALL  do_far_arg
   .DB   _rom_page_battle3
   .DW   _bshowail

_f_setbosspal::
   CALL  do_far_arg
   .DB   _rom_page_battle3
   .DW   _setbosspal

;----------------------------------------------------------------------------
;_rom_page_battle4
;----------------------------------------------------------------------------
;void bcur_on();
;void bcur_off();
;void bcur_init();
;void bcur_dest();
;void bcur_lock();
;void bcur_update();
;void bvictory();
;void prophecy();
;void setbosspal();
;void map_restate();
;void bdamageflux();
;void bdamagecap();
;ubyte bitem_have();
;ubyte bitem_type();
;void bitem_del();
;
;uword bwepdamage(ubyte guy,ubyte target);
;void loaddude(ubyte slot,word xpos,word ypos,ubyte type,ubyte dir,ubyte control);
;void colorshift(ubyte color);
;byte b_ai(ubyte guy,ubyte style);
;ubyte touch_guy(ubyte x,ubyte n,byte dir);
;uword get_skill(struct DUDESTAT *st);
;uword get_item(ubyte item);
;ubyte randbattle(ubyte *info);
;byte b_ai_charm(ubyte guy);
;void getdistxy(word xlen, word ylen, ubyte dist);
;----------------------------------------------------------------------------
_f_bcur_on::
   CALL  do_far_noarg
   .DB   _rom_page_battle4
   .DW   _bcur_on

_f_bcur_off::
   CALL  do_far_noarg
   .DB   _rom_page_battle4
   .DW   _bcur_off

_f_bcur_init::
   CALL  do_far_noarg
   .DB   _rom_page_battle4
   .DW   _bcur_init

_f_bcur_dest::
   CALL  do_far_noarg
   .DB   _rom_page_battle4
   .DW   _bcur_dest

_f_bcur_lock::
   CALL  do_far_noarg
   .DB   _rom_page_battle4
   .DW   _bcur_lock

_f_bcur_update::
   CALL  do_far_noarg
   .DB   _rom_page_battle4
   .DW   _bcur_update

_f_bvictory::
   CALL  do_far_noarg
   .DB   _rom_page_battle4
   .DW   _bvictory

_f_prophecy::
   CALL  do_far_noarg
   .DB   _rom_page_battle4
   .DW   _prophecy

_f_map_restate::
   CALL  do_far_noarg
   .DB   _rom_page_battle4
   .DW   _map_restate

_f_bdamageflux::
   CALL  do_far_noarg
   .DB   _rom_page_battle4
   .DW   _bdamageflux

_f_bdamagecap::
   CALL  do_far_noarg
   .DB   _rom_page_battle4
   .DW   _bdamagecap

_f_bitem_have::
   CALL  do_far_noarg
   .DB   _rom_page_battle4
   .DW   _bitem_have

_f_bitem_type::
   CALL  do_far_noarg
   .DB   _rom_page_battle4
   .DW   _bitem_type

_f_bitem_del::
   CALL  do_far_noarg
   .DB   _rom_page_battle4
   .DW   _bitem_del

;_f_setbosspal::
;   CALL  do_far_arg
;   .DB   _rom_page_battle4
;   .DW   _setbosspal

_f_loaddude::
   CALL  do_far_arg
   .DB   _rom_page_battle4
   .DW   _loaddude

_f_colorshift::
   CALL  do_far_arg
   .DB   _rom_page_battle4
   .DW   _colorshift

_f_b_ai::
   CALL  do_far_arg
   .DB   _rom_page_battle4
   .DW   _b_ai

_f_touch_guy::
   CALL  do_far_arg
   .DB   _rom_page_battle4
   .DW   _touch_guy

_f_get_skill::
   CALL  do_far_arg
   .DB   _rom_page_battle4
   .DW   _get_skill

_f_get_item::
   CALL  do_far_arg
   .DB   _rom_page_battle4
   .DW   _get_item

_f_randbattle::
   CALL  do_far_arg
   .DB   _rom_page_battle4
   .DW   _randbattle

_f_b_ai_charm::
   CALL  do_far_arg
   .DB   _rom_page_battle4
   .DW   _b_ai_charm

_f_bwepdamage::
   CALL  do_far_arg
   .DB   _rom_page_battle4
   .DW   _bwepdamage

_f_getdistxy::
   CALL  do_far_arg
   .DB   _rom_page_battle4
   .DW   _getdistxy

;----------------------------------------------------------------------------
;_rom_page_battle5
;----------------------------------------------------------------------------
;void baction_nextmore();
;----------------------------------------------------------------------------
_f_baction_nextmore::
   CALL  do_far_noarg
   .DB   _rom_page_battle5
   .DW   _baction_nextmore

;----------------------------------------------------------------------------
;_rom_page_battle6
;----------------------------------------------------------------------------
;void start_skill();
;----------------------------------------------------------------------------
_f_start_skill::
   CALL  do_far_noarg
   .DB   _rom_page_battle6
   .DW   _start_skill

;----------------------------------------------------------------------------
;_rom_page_menu1
;----------------------------------------------------------------------------
;void items_reset();
;void curflick_off();
;void menu_error();
;void slide_out_menu();
;
;void items_add(ubyte type);
;void items_del(ubyte type);
;ubyte items_have(ubyte type);
;void items_set(ubyte type,ubyte value);
;void curflick_on(ubyte x,ubyte y);
;void show_menu(ubyte type);
;void stats_recalc(struct DUDESTAT *guy);
;void slide_in_menu(ubyte num);
;----------------------------------------------------------------------------
_f_items_reset::
   CALL  do_far_noarg
   .DB   _rom_page_menu1
   .DW   _items_reset

_f_curflick_off::
   CALL  do_far_noarg
   .DB   _rom_page_menu1
   .DW   _curflick_off

_f_menu_error::
   CALL  do_far_noarg
   .DB   _rom_page_menu1
   .DW   _menu_error

_f_slide_out_menu::
   CALL  do_far_noarg
   .DB   _rom_page_menu1
   .DW   _slide_out_menu

_f_items_add::
   CALL  do_far_arg
   .DB   _rom_page_menu1
   .DW   _items_add

_f_items_del::
   CALL  do_far_arg
   .DB   _rom_page_menu1
   .DW   _items_del

_f_items_have::
   CALL  do_far_arg
   .DB   _rom_page_menu1
   .DW   _items_have

_f_items_set::
   CALL  do_far_arg
   .DB   _rom_page_menu1
   .DW   _items_set

_f_curflick_on::
   CALL  do_far_arg
   .DB   _rom_page_menu1
   .DW   _curflick_on

_f_show_menu::
   CALL  do_far_arg
   .DB   _rom_page_menu1
   .DW   _show_menu

_f_stats_recalc::
   CALL  do_far_arg
   .DB   _rom_page_menu1
   .DW   _stats_recalc

_f_slide_in_menu::
   CALL  do_far_arg
   .DB   _rom_page_menu1
   .DW   _slide_in_menu

;----------------------------------------------------------------------------
;_rom_page_menu2
;----------------------------------------------------------------------------
;void do_shopmenu();
;void draw_shopmenu();
;void shop_reset();
;ubyte do_yesnomenu();
;
;void shop_add(ubyte type, uword price);
;void do_slmenu(ubyte type);
;void gain_level(struct DUDESTAT *guy);
;ubyte *get_expnext(ubyte lv);
;----------------------------------------------------------------------------
_f_do_shopmenu::
   CALL  do_far_noarg
   .DB   _rom_page_menu2
   .DW   _do_shopmenu

_f_draw_shopmenu::
   CALL  do_far_noarg
   .DB   _rom_page_menu2
   .DW   _draw_shopmenu

_f_shop_reset::
   CALL  do_far_noarg
   .DB   _rom_page_menu2
   .DW   _shop_reset

_f_do_yesnomenu::
   CALL  do_far_noarg
   .DB   _rom_page_menu2
   .DW   _do_yesnomenu

_f_shop_add::
   CALL  do_far_arg
   .DB   _rom_page_menu2
   .DW   _shop_add

_f_do_slmenu::
   CALL  do_far_arg
   .DB   _rom_page_menu2
   .DW   _do_slmenu

_f_gain_level::
   CALL  do_far_arg
   .DB   _rom_page_menu2
   .DW   _gain_level

_f_get_expnext::
   CALL  do_far_arg
   .DB   _rom_page_menu2
   .DW   _get_expnext

_f_copyright::
   CALL  do_far_noarg
   .DB   _rom_page_event2
   .DW   _copyright
