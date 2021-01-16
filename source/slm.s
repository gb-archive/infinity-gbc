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

   .area _BSS

scrx                 = 0xDABD       ;  66 bytes

_the_40::
   .ds   1
_the_sx::
   .ds   1
_the_sy::
   .ds   1
_the_wx::
   .ds   1

_slm_count::
   .ds   1
_slm_table::
   .ds   4
_slm_bgx::
   .ds   1
_slm2::
   .ds   1

   .area _CODE

_slm_init::
   XOR   A
   LD    (_slm2), A
   LD    A, #0b11100110                ;2
   LD    (_the_40), A
   LDH   (P_LCDC), A
   LD    A, #24
   LDH   (P_WY), A
   LD    A, #167
   LD    (_the_wx), A
   LDH   (P_WX), A
   XOR   A
   LD    (_slm_count), A
   LD    (_the_sx), A
   LD    A, #196-116                   ;2
   LD    (_the_sy), A                  ;4
   LD    A, #160
   LD    HL, #_slm_table
   LD    (HLI), A
   LD    (HLI), A
   LD    (HLI), A
   LD    (HLI), A
   LD    A, #0x04
   LDH   (_vbimode), A
   LDH   (_hbimode), A
   RET

_slm2_init::
   CALL  _slm_init
   LD    A, #0x07
   LDH   (_hbimode), A
   XOR   A
   LDH   (P_WY), A
   INC   A
   LD    (_slm2), A
   LDH   A, (_rscx)
   LD    (_the_sx), A
   LDH   A, (_rscy)
   LD    (_the_sy), A
   RET

_slm_off::
   DI
   XOR   A
   LDH   (_vbimode), A
   LDH   (_hbimode), A
   LD    A, #0b11100111
   LDH   (P_LCDC), A
   LDH   (P_WY), A
   LD    A, #167
   LDH   (P_WX), A
   EI
   RET

;----------------------------------------------------------------------------
;[[ void slm_setx(ubyte window, byte x) ]]
;----------------------------------------------------------------------------
_slm_setx::
   LDHL  SP, #3
   LD    A, (HLD)
   LD    E, (HL)
   LD    D, #0x00
   LD    HL, #_slm_table
   ADD   HL, DE
   LD    (HL), A
   RET

;----------------------------------------------------------------------------
;[ slm_vbl ] Gradient Interrupt Function with Sprite On/Off support
;----------------------------------------------------------------------------
_slm_vbl::
   LD    A, (_slm2)
   OR    A
   JR    NZ, not_slm

   LDH   A, (_rampage)
   PUSH  AF
   LD    A, #1
   LDH   (_rampage), A
   LDH   (P_SVBK), A                   ; Map D000-DFFF to ram page

   LD    A, (_slm_count)
   PUSH  AF

   LD    A, (_slm_table)
   CPL
   INC   A
   LD    (_the_sx), A
   LDH   (P_SCX), A

   XOR   A
   LD    (_the_sy), A
   LDH   (P_SCY), A
   INC   A
   LD    (_slm_count), A
   LDH   (P_LYC), A

   LD    A, (_slm_table+1)
   ADD   A, #7
   LD    (_the_wx), A

   POP   AF

   OR    A
   JR    Z, first_slm

   LD    HL, #scrx
   LD    C, #0
   LD    DE, #0x2021    ; adder / count

_oceananim2:
   LD    A, (HL)
   ADD   A, D
   LD    (HLI), A
   LD    A, (HL)
   ADC   A, C
   LD    (HLI), A
   LD    A, D
   ADD   A, #16
   LD    D, A
   ADC   A, C
   SUB   D
   LD    C, A
   DEC   E
   JR    NZ, _oceananim2

first_slm:
   POP   AF
   LDH   (_rampage), A
   LDH   (P_SVBK), A                   ; Map D000-DFFF to ram page

finish_slm:
   LD    A, #0xC0
   CALL  oam_dma

   JP    _vbl_end

not_slm:
   XOR   A
   LDH   (P_WY), A
   LD    A, #7
   LDH   (P_WX), A
   LD    (_the_wx), A
   LD    A, #1
   LD    (_slm_count), A
   LDH   (P_LYC), A
   JR    finish_slm

;----------------------------------------------------------------------------
;[ slm_hbl ] Gradient Interrupt Function with Sprite On/Off support
;----------------------------------------------------------------------------
;_slm_hbl::
;   LDH   A, (_rompage)
;   PUSH  AF
;   LD    A, #_rom_page_title
;   LDH   (_rompage), A
;   LD    (0x2000), A
;
;   LD    A, (_slm2)
;   OR    A
;   CALL  Z, _slm_int_real
;   LD    A, (_slm2)
;   OR    A
;   CALL  NZ, _slm2_int_real
;
;   POP   AF
;   LDH   (_rompage), A
;   LD    (0x2000), A
;
;   JP    _hbl_end

_slm_hbl::
   LD    A, #0xB8                      ;2
   LDH   (P_BCPS), A                   ;3

   LD    A, #1                         ;2
   LDH   (P_SVBK), A                   ;3

;   XOR   A
;   LD    (scrx+15), A

   PUSH  HL                            ;4
   PUSH  DE                            ;4
   LD    HL, #_slm_count               ;3
   INC   (HL)                          ;3
   LD    A, (HL)                       ;2

   CP    #88                           ;2
   JR    C, before_ocean               ;3/2
   CP    #116                          ;2
   JR    NC, after_ocean               ;3/2

do_ocean_anim:
   CP    #104                          ;2
   JR    NZ, continue_ocean            ;3/2

   LD    A, (_slm_table+3)             ;4
   ADD   A, #7                         ;2
   LD    (_the_wx), A                  ;4
   LD    A, (scrx+3+32)                ;4
   LD    (_the_sx), A                  ;4
   JR    _slm_done                     ;3

continue_ocean:
   LD    D, #0                         ;2
   LD    E, A                          ;1
   LD    HL, #scrx+3-176               ;3
   ADD   HL, DE                        ;2
   ADD   HL, DE                        ;2
   LD    A, (HL)                       ;2
   LD    (_the_sx), A                  ;4
   JR    _slm_delay1                   ;3

after_ocean:
   LD    A, #0b11100111                ;2
   LD    (_the_40), A                  ;4
   JR    _slm_delay15                  ;3

before_ocean:
   CP    #28                           ;2
   JR    NZ, not_second_slm            ;3/2

   LD    A, #0b11101111                ;2
   LD    (_the_40), A                  ;4
   LD    A, #136-32                    ;2
   LD    (_the_sy), A                  ;4
   LD    A, (scrx+1)                   ;4
   LD    (_the_sx), A                  ;4
   JR    _slm_delay1                   ;3

_slm_notdone:
   LD    A, (_slm_table+2)             ;4
   ADD   A, #7                         ;2
   LD    (_the_wx), A                  ;4
   INC   HL                            ;2
   DEC   HL                            ;2
   JR    _slm_delay1                   ;3

not_second_slm:
   CP    #64                           ;2
   JR    Z, _slm_notdone               ;3/2

   INC   HL                            ;2
   DEC   HL                            ;2
_slm_delay15:
   PUSH  HL                            ;4
   POP   HL                            ;3
_slm_delay8:
   PUSH  HL                            ;4
   POP   HL                            ;3
_slm_delay1:
   NOP                                 ;1

_slm_done:
_slm_join:
   LDH   A, (P_LY)
   LD    D, A
   SUB   #143
   LD    E, A
   SBC   A, A
   AND   D
   INC   A
   LD    D, A
   LDH   (P_LYC), A

   LD    A, E
   ADD   #255
   SBC   A, A
   AND   D

   LD    D, #0
   LD    E, A

   LD    A, (_the_40)
   LDH   (P_LCDC), A

   LD    A, (_the_sy)
   LDH   (P_SCY), A

   LD    A, (_the_sx)
   LDH   (P_SCX), A

   LD    A, (_the_wx)
   LDH   (P_WX), A

   LD    HL, #_grad_table
   ADD   HL, DE
   ADD   HL, DE

   INC   HL
   DEC   HL
   NOP
   POP   DE

   LD    A, #_ram_page_grd
   LDH   (P_SVBK), A                   ; Map D000-DFFF to ram page

   LD    A, (HLI)
   LDH   (P_BCPD), A
   LD    A, (HL)
   LDH   (P_BCPD), A

   LDH   A, (_rampage)
   LDH   (P_SVBK), A                   ; Map D000-DFFF to ram page

   POP   HL

   JP    _hbl_end

_slm2_hbl::
   LD    A, #0xB8                      ;2
   LDH   (P_BCPS), A                   ;3

   PUSH  HL                            ;4
   PUSH  DE                            ;4

   LD    HL, #_slm_count               ;3
   INC   (HL)                          ;3
   LD    A, (HL)                       ;2  24 beyond this point to slm_done2

   CP    #24                           ;2
   JR    NZ, not_first_slm2            ;3/2

   LD    A, (_slm_table+1)             ;4
   ADD   A, #7                         ;2
   LD    (_the_wx), A                  ;4
   NOP                                 ;1
   NOP                                 ;1
   JR    _slm_delay5_2                 ;3
not_first_slm2:
   CP    #64                           ;2
   JR    NZ, not_second_slm2           ;3/2

   LD    A, (_slm_table+2)             ;4
   ADD   A, #7                         ;2
   LD    (_the_wx), A                  ;4
_slm_delay5_2:
   NOP                                 ;1
   NOP                                 ;1
   JR    _slm_done2                    ;3
_slm_delay9_2:
   NOP                                 ;1
   JR    _slm_delay5_2                 ;3
not_second_slm2:
   CP    #104                          ;2
   JR    NZ, _slm_delay9_2             ;3/2

   LD    A, (_slm_table+3)             ;4
   ADD   A, #7                         ;2
   LD    (_the_wx), A                  ;4

_slm_done2:
   ; 2 extra clocks in all/need 21

   PUSH  HL
   POP   HL
   PUSH  HL
   POP   HL
   INC   HL
   DEC   HL
   NOP
   JP    _slm_join
