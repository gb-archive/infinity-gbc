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

_grad_lastclr:
   .DS   2
_grad_first:
   .DS   1
_grad_every::
   .DS   1

_grad_rgb1::
   .DS   3
_grad_rgb2::
   .DS   3
_grad_var1::
   .DS   1
_grad_var2::
   .DS   1
_grad_issprites::
   .DS   1
_grad_isgoing::
   .DS   1
_grad_temp:
   .DS   12

dialogue_count:
   .DS   1
dialogue_scroll:
   .DS   1
dialogue_SCX:
   .DS   1
dialogue_SCY:
   .DS   1

;_grad_table::
;   .DS   288

_ram_page_grd = 5

_grad_table = 0xD480          ; 288 bytes

   .area _CODE

;----------------------------------------------------------------------------
;[[ void grad_clear() ]]
;----------------------------------------------------------------------------
_grad_clear::
   LDH   A, (_rampage)
   PUSH  AF
   LD    A, #_ram_page_grd
   LDH   (_rampage), A
   LDH   (P_SVBK), A                   ; Map D000-DFFF to ram page

   LD    HL, #_grad_table

   XOR   A
   LD    DE, #0x9080
gc_loop:
   LD    (HLI), A
   LD    (HL), E
   INC   HL
   DEC   D
   JR    NZ, gc_loop

   POP   AF
   LDH   (_rampage), A
   LDH   (P_SVBK), A                   ; Map D000-DFFF to ram page

   RET

;----------------------------------------------------------------------------
;[[ void grad_init() ]]
;----------------------------------------------------------------------------
;parameters:
; grad_rgb1 -> RGBsource
; grad_rgb2 -> RGBdest
; grad_var1 = Starting Line Number
; grad_var2 = Number of Lines for Gradient
; grad_issprites = Sprite Setting for this gradient (0 = off, 1 = on)
;----------------------------------------------------------------------------
_grad_init::
   PUSH  BC

   LDH   A, (_rampage)
   PUSH  AF
   LD    A, #_ram_page_grd
   LDH   (_rampage), A
   LDH   (P_SVBK), A                   ; Map D000-DFFF to ram page

;   CALL  _grad_op_disable

   LD    BC, #_grad_rgb1
   LD    DE, #_grad_rgb2
   LD    HL, #_grad_temp

   LD    A, #3
gi_loop:
   PUSH  AF
   PUSH  BC
   LD    A, (BC)
   LD    B, A
   LD    A, (DE)
   SUB   B

   LD    B, A
   ADD   A, A
   SBC   A, A
   LD    C, A
   XOR   B
   SUB   C
   LD    (HLI), A

   LD    A, (_grad_var2)
   CPL
   INC   A
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
   JR    NZ, gi_loop

   LD    A, (_grad_var1)
   LD    H, #0
   LD    L, A
   ADD   HL, HL
   LD    DE, #_grad_table
   ADD   HL, DE
   LD    D, H
   LD    E, L

   LD    A, (_grad_var2)
   LD    B, A

gi2_loop:
   LD    HL, #_grad_temp
   PUSH  AF
   CALL  Bresenham
   LD    C, A
   CALL  Bresenham
   RRCA
   RRCA
   RRCA
   PUSH  DE
   LD    D, A
   AND   #3
   LD    E, A
   XOR   D
   OR    C
   LD    C, E
   POP   DE
   LD    (DE), A
   INC   DE
   CALL  Bresenham
   RLCA
   RLCA
   OR    C
   LD    C, A
   LD    A, (_grad_issprites)
   ADD   #0xFF
   RRA
   AND   #0x80
   OR    C
   LD    (DE), A
   INC   DE
   POP   AF
   DEC   A
   JR    NZ, gi2_loop

   INC   A
   LD    (_grad_every), A

   POP   AF
   LDH   (_rampage), A
   LDH   (P_SVBK), A                   ; Map D000-DFFF to ram page

   POP   BC
   RET

;----------------------------------------------------------------------------
;[[ void grad_on() ]]
;----------------------------------------------------------------------------
_grad_on::
   XOR   A
   LDH   (P_LYC), A

   INC   A
   LDH   (_hbimode), A

   RET

;----------------------------------------------------------------------------
;[[ void grad_off() ]]
;----------------------------------------------------------------------------
_grad_off::
   XOR   A
   LDH   (_hbimode), A

   LD    A, #0b11100111
   LDH   (P_LCDC), A

   RET

;   LDH   A, (P_LY)                     ;3 P_LYC=(P_LY==143)?0:P_LY+1;
;   LD    D, A                          ;1
;   SUB   #143                          ;2
;   LD    E, A                          ;1
;   SBC   A, A                          ;1
;   AND   D                             ;1
;   INC   A                             ;1
;   LD    D, A                          ;1
;   LDH   (P_LYC), A                    ;3
;
;   LD    A, E                          ;1 A=(P_LY==143)?0:P_LY;
;   ADD   #255                          ;2
;   SBC   A, A                          ;1
;   AND   D                             ;1
;
;   LD    E, A                          ;1
;   LD    D, #0                         ;2

everyother:
   LDH   A, (P_LY)
   INC   A
   LD    B, A
   SUB   #144
   SBC   A, A
   ADD   A, A
   AND   B
   INC   A
   LDH   (P_LYC), A

   LD    D, #0                         ;2
   LD    E, A                          ;1

   LD    A, #20
everyother_delay:
   DEC   A
   JR    NZ, everyother_delay

   LD    HL, #_grad_table              ;3
   ADD   HL, DE                        ;2
   ADD   HL, DE                        ;2

   LD    A, (HLI)                      ;1
   LDH   (P_BCPD), A                   ;3
   LD    A, (HL)                       ;1
   LDH   (P_BCPD), A                   ;3

   JP    grad_done

;----------------------------------------------------------------------------
;[ grad_int ] Gradient Interrupt Function with Sprite On/Off support
;----------------------------------------------------------------------------
_grad_int::
   LD    A, #0xB8                      ;2
   LDH   (P_BCPS), A                   ;3

   LD    A, #_ram_page_grd             ;2
   LDH   (P_SVBK), A                   ;3 Map D000-DFFF to ram page

   PUSH  HL                            ;4
   PUSH  DE                            ;4
   PUSH  BC                            ;4

   LD    A, (_grad_every)              ;4
   OR    A                             ;1
   JR    Z, everyother                 ;3/2

   LDH   A, (P_LY)                     ;3
   CP    #143                          ;2
   JR    NZ, _gly_notfinal             ;3/2
   XOR   A                             ;1
   LD    D, A                          ;1
   LD    E, A                          ;1
   LDH   (P_LYC), A                    ;3
   NOP                                 ;1

   LD    A, #2                         ;2
   LD    (_grad_first), A              ;4

   JR    _gly_final                    ;3
_gly_notfinal:
   INC   A                             ;1
   LD    D, #0                         ;2
   LD    E, A                          ;1
   LDH   (P_LYC), A                    ;3

   LD    A, (_scl_on)                  ;4
   LD    (_grad_first), A              ;4

_gly_final:
   LD    HL, #_grad_table+1            ;3
   ADD   HL, DE                        ;2
   ADD   HL, DE                        ;2

   LD    A, (HLD)                      ;2
   LD    C, (HL)                       ;2
   RLCA                                ;1
   LD    B, A                          ;2

;----------------------------------------------------------------------------

   LD    HL, #_grad_lastclr+1          ;3

   LD    A, (_grad_first)              ;4
   DEC   A                             ;1
   JR    Z, noskip                     ;3/2

   LD    A, (HLD)                      ;2 HL -> _grad_lastclr
   CP    B                             ;1
   JR    NZ, diffclr                   ;3/2
   LD    A, (HLI)                      ;2 HL -> _grad_lastclr+1
   CP    C                             ;1
   JR    NZ, diffclr2                  ;3/2

   POP   BC                            ;3
   POP   DE                            ;3
   POP   HL                            ;3

   LDH   A, (_rampage)                 ;3
   LDH   (P_SVBK), A                   ;3 Map D000-DFFF to ram page

   JP    _hbl_end                      ;4

noskip:
   DEC   HL                            ;2
   NOP                                 ;1
   NOP                                 ;1
   NOP                                 ;1

diffclr:
   NOP                                 ;1
   NOP                                 ;1
   LD    A, C                          ;1
   LD    (HLI), A                      ;2
diffclr2:
   LD    A, B                          ;1
   LD    (HL), A                       ;2

   LD    A, (_scl_on)                  ;4
   OR    A                             ;1
   LDH   A, (_rscy)                    ;3
   JR    NZ, yes_scl                   ;3/2

   INC   HL                            ;2
   INC   HL                            ;2
   INC   HL                            ;2

   JR    do_scl                        ;3

yes_scl:
   LD    HL, #_scl_table               ;3
   ADD   HL, DE                        ;2

   SUB   E                             ;1
   ADD   A, (HL)                       ;2

do_scl:
   PUSH  HL                            ;4
   POP   HL                            ;3

   SRL   B                             ;2 delays/do necessary stuff :P
   RL    D                             ;2
   SLA   D                             ;2
   LD    HL, #_grad_first              ;3

   LDH   (P_SCY), A                    ;3

   LDH   A, (P_LCDC)                   ;3
   AND   #0xFD                         ;2
   OR    D                             ;1
   LDH   (P_LCDC), A                   ;3

   LD    A, (HL)                       ;2
   SUB   #1                            ;2
   JR    C, _grad_first_skip1          ;3/2
_grad_first_skip1:
   JR    C, _grad_first_skip2          ;3/2
   LD    (HL), A                       ;2
_grad_first_skip2:

   LD    A, C                          ;1
   LDH   (P_BCPD), A                   ;3
   LD    A, B                          ;1
   LDH   (P_BCPD), A                   ;3

grad_done:
   POP   BC                            ;3
   POP   DE                            ;3
   POP   HL                            ;3

   LDH   A, (_rampage)                 ;3
   LDH   (P_SVBK), A                   ;3 Map D000-DFFF to ram page

   JP    _hbl_end                      ;4

_dialogue_scroll::
   LDHL  SP, #2
   LD    A, (HL)
   LD    (dialogue_scroll), A
   RET

_dialogue_on::
   LD    A, #0b11000111
   LDH   (P_LCDC), A
   LD    A, #7
   LDH   (P_WX), A
   LD    A, #88
   LDH   (P_WY), A
   CALL  _w4vbl_done
   LD    A, #0b11100111
   LDH   (P_LCDC), A
   LD    A, #3
   LDH   (_hbimode), A
   LDH   (_vbimode), A
   LD    A, #87
   LDH   (P_LYC), A
   RET

_dialogue_off::
   LD    A, #0b11100111
   LDH   (P_LCDC), A
   XOR   A
   LDH   (_hbimode), A
   INC   A
   LDH   (_vbimode), A
   RET

;----------------------------------------------------------------------------
;[ dialogue_vbl ] Gradient Interrupt Function with Sprite On/Off support
;----------------------------------------------------------------------------
_dialogue_vbl::
   XOR   A
   LD    (dialogue_count), A
   LD    A, (_owe_scrollx)
   LDH   (P_SCX), A
   LD    A, (_owe_scrolly)
   LDH   (P_SCY), A
   LD    A, #87
   LDH   (P_LYC), A

   LD    A, #0b11100111
   LDH   (P_LCDC), A

   CALL  _animation

   LD    A, (_owe_panmode)
   OR    A
   CALL  NZ, scroll_update
   LD    A, (_owe_panmode)
   OR    A
   CALL  NZ, scroll_update_time

   JP    _vbl_end

;----------------------------------------------------------------------------
;[ dialogue_hbl ] Gradient Interrupt Function with Sprite On/Off support
;----------------------------------------------------------------------------
_dialogue_hbl::
   LD    A, #_ram_page_grd
   LDH   (P_SVBK), A                   ; Map D000-DFFF to ram page

   PUSH  HL

   LD    HL, #dialogue_count
   LD    A, (HL)
   INC   (HL)
   CP    #6
   JR    NZ, not_first_dialogue

   XOR   A
   LDH   (P_SCX), A

   LD    A, (dialogue_scroll)
   ADD   A, #64-96
   LDH   (P_SCY), A

   LD    A, #0b11001101
   LDH   (P_LCDC), A

   JR    done_dialogue

not_first_dialogue:
   CP    #49
   JR    NZ, done_dialogue

   LD    A, #48-136
   LDH   (P_SCY), A

done_dialogue:
   LD    A, #0xB8
   LDH   (P_BCPS), A

   LDH   A, (P_LY)                     ;

   SUB   #143                          ;
   JR    Z, dial_line0                 ;
   ADD   A, #144                       ;

dial_line0:
   LDH   (P_LYC), A                    ;

   PUSH  DE

   LD    D, #0
   LD    E, A
   LD    HL, #_grad_table
   ADD   HL, DE
   ADD   HL, DE

   POP   DE

   LD    A, #24
_dialog_delay:
   DEC   A
   JR    NZ, _dialog_delay

   LD    A, (HLI)
   LDH   (P_BCPD), A
   LD    A, (HL)
   LDH   (P_BCPD), A

   POP   HL

   LDH   A, (P_LCDC)
   AND   #0xFD
   LDH   (P_LCDC), A

   LDH   A, (_rampage)
   LDH   (P_SVBK), A                   ; Map D000-DFFF to ram page

   JP    _hbl_end
