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

string:
   .ds   9

   .area _CODE

;----------------------------------------------------------------------------
;[[ void set_24(UBYTE *num_dest, UWORD num) ]]
;----------------------------------------------------------------------------
_set_24::
   LDHL  SP, #5
   LD    A, (HLD)
   LD    D, A
   LD    A, (HLD)
   LD    E, A             ; DE = num_src

   LD    A, (HLD)
   LD    L, (HL)
   LD    H, A             ; HL = num_dest

   LD    A, E
   LD    (HLI), A
   LD    A, D
   LD    (HLI), A
   LD    (HL), #0x00

   RET

;----------------------------------------------------------------------------
;[[ void add_24(UBYTE *num_dest, UBYTE *num_src) ]]
;----------------------------------------------------------------------------
_add_24::
   LDHL  SP, #5
   LD    A, (HLD)
   LD    D, A
   LD    A, (HLD)
   LD    E, A             ; DE = num_src

   LD    A, (HLD)
   LD    L, (HL)
   LD    H, A             ; HL = num_dest

   LD    A, (DE)
   ADD   A, (HL)
   LD    (HLI), A
   INC   DE

   LD    A, (DE)
   ADC   A, (HL)
   LD    (HLI), A
   INC   DE

   LD    A, (DE)
   ADC   A, (HL)
   LD    (HL), A

   RET

;----------------------------------------------------------------------------
;[[ void sub_24(UBYTE *num_dest, UBYTE *num_src) ]]
;----------------------------------------------------------------------------
_sub_24::
   LDHL  SP, #5
   LD    A, (HLD)
   LD    D, A
   LD    A, (HLD)
   LD    E, A             ; DE = num_src

   PUSH  DE
   LD    A, (HLD)
   LD    E, (HL)
   LD    D, A             ; HL = num_dest
   POP   HL

   LD    A, (DE)
   SUB   (HL)
   LD    (DE), A
   INC   DE
   INC   HL

   LD    A, (DE)
   SBC   A, (HL)
   LD    (DE), A
   INC   DE
   INC   HL

   LD    A, (DE)
   SBC   A, (HL)
   LD    (DE), A

   RET

;----------------------------------------------------------------------------
;[[ UBYTE cmp_24(UBYTE *num_1, UBYTE *num_2) ]]
;----------------------------------------------------------------------------
_cmp_24::
   LDHL  SP, #5
   LD    A, (HLD)
   LD    D, A
   LD    A, (HLD)
   LD    E, A

   PUSH  DE
   LD    A, (HLD)
   LD    E, (HL)
   LD    D, A              ; DE = num_1
   POP   HL                ; HL = num_2

   INC   DE
   INC   DE
   INC   HL
   INC   HL

   LD    A, (DE)
   CP    (HL)
   JR    NZ, cmp_different

   DEC   DE
   DEC   HL

   LD    A, (DE)
   CP    (HL)
   JR    NZ, cmp_different

   DEC   DE
   DEC   HL

   LD    A, (DE)
   CP    (HL)
   JR    NZ, cmp_different

   LD    DE, #0
   RET

cmp_different:
   SBC   A, A
   LD    D, A
   OR    #1
   LD    E, A
   RET

;----------------------------------------------------------------------------
;[[ char *deci_24(UBYTE *num, UBYTE digits) ]]
;----------------------------------------------------------------------------
_deci_24::
   PUSH  BC

   LDHL  SP, #6
   LD    A, (HLD)
   LD    D, A
   LD    A, (HLD)
   LD    L, (HL)
   LD    H, A             ; HL = num

   LD    A, (HLI)
   LD    C, A
   LD    A, (HLI)
   LD    B, A
   LD    E, (HL)

   LD    H, B
   LD    L, C

   XOR   A
   LD    BC, #string+8
   LD    (BC), A

   PUSH  DE
deci_loop:
   CALL  div_EHL_10
   ADD   #0x30
   DEC   BC
   LD    (BC), A
   DEC   D
   JR    NZ, deci_loop
   POP   DE
   DEC   D

   LD    H, B
   LD    L, C

   PUSH  HL
deci_loop2:
   LD    A, (HL)
   CP    #0x30
   JR    NZ, deci_done
   LD    A, #0x10                    ; Space
   LD    (HLI), A
   DEC   D
   JR    NZ, deci_loop2
deci_done:
   POP   DE
   POP   BC

   RET

;----------------------------------------------------------------------------
;[[ char *deci_16(UWORD num, UBYTE digits) ]]
;----------------------------------------------------------------------------
_deci_16::
   PUSH  BC

   LDHL  SP, #6
   LD    A, (HLD)
   LD    D, A
   LD    A, (HLD)
   LD    L, (HL)
   LD    H, A             ; HL = num

   XOR   A
   LD    BC, #string+8
   LD    (BC), A

   PUSH  DE
d16_loop:
   CALL  div_HL_10
   ADD   #0x30
   DEC   BC
   LD    (BC), A
   DEC   D
   JR    NZ, d16_loop
   POP   DE
   DEC   D

   LD    H, B
   LD    L, C

   PUSH  HL
d16_loop2:
   LD    A, (HL)
   CP    #0x30
   JR    NZ, d16_done
   LD    A, #0x10                    ; Space
   LD    (HLI), A
   DEC   D
   JR    NZ, d16_loop2
d16_done:
   POP   DE
   POP   BC

   RET

;----------------------------------------------------------------------------
;[ div_EHL_10 ]
;parameters:
; EHL = 24-bit number
;returns:
; A = EHL % 10
; EHL = EHL / 10
;----------------------------------------------------------------------------
div_EHL_10:
   PUSH  BC
   XOR   A
   LD    BC, #0x180A
_dHL_Loop1:
   ADD   HL, HL                     ; 24-bit shift  E <- H <- L
   RL    E
   RLA
   CP    C
   JR    C, _dHL_Loop2
   SUB   C
   INC   L
_dHL_Loop2:
   DEC   B
   JR    NZ, _dHL_Loop1
   POP   BC
   RET

;----------------------------------------------------------------------------
;[ div_HL_10 ]
;parameters:
; HL = 16-bit number
;returns:
; A = HL % 10
; HL = HL / 10
;----------------------------------------------------------------------------
div_HL_10:
   PUSH  BC
   XOR   A
   LD    BC, #0x100A
_dHLb_Loop1:
   ADD   HL, HL                     ; 24-bit shift  E <- H <- L
   RLA
   CP    C
   JR    C, _dHLb_Loop2
   SUB   C
   INC   L
_dHLb_Loop2:
   DEC   B
   JR    NZ, _dHLb_Loop1
   POP   BC
   RET
