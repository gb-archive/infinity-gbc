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

   .AREA _BSS

steps:
   .DS   1
negsteps:
   .DS   1

   .AREA _CODE

not_signed:
   CALL  DIV_HL_C_S                    ; A = remainder
   LD    D, H                          ; DE = quotient
   LD    E, L                          ;

   JR    bresen_join                   ; B = sign

;----------------------------------------------------------------------------
;[[ void brw_setup(struct brw *struc, word start, word end, byte steps) ]]
;----------------------------------------------------------------------------
_brw_setup::
   LDHL  SP, #2

   PUSH  BC

   LD    A, (HLI)                      ; BC->brws
   LD    C, A                          ;
   LD    A, (HLI)                      ;
   LD    B, A                          ;

   LD    A, (HLI)                      ; DE=start
   LD    E, A                          ;
   LD    A, (HLI)                      ;
   LD    D, A                          ;

   PUSH  DE                            ; save start
   PUSH  BC                            ; save pointer to brws

   LD    A, (HLI)                      ; BA=end
   LD    B, (HL)                       ;
   INC   HL                            ;
   LD    C, (HL)                       ; C=steps

   SUB   E                             ; HL=end-start (delta)
   LD    L, A                          ;
   LD    A, B                          ;
   SBC   A, D                          ;
   LD    H, A                          ;

   ADD   A, A                          ;
   JR    NC, not_signed                ;

   LD    A, H                          ;
   CPL                                 ;
   LD    H, A                          ;
   LD    A, L                          ;
   CPL                                 ;
   LD    L, A                          ;
   INC   HL                            ;

   CALL  DIV_HL_C_S                    ; HL=delta/steps

   LD    B, A                          ; B=remainder
   LD    A, H                          ; DE=-quotient
   CPL                                 ;
   LD    D, A                          ;
   LD    A, L                          ;
   CPL                                 ;
   LD    E, A                          ;
   INC   DE                            ;
   LD    A, B                          ; A=remainder

   LD    B, #0xFF                      ; B=sign

bresen_join:
   POP   HL                            ; HL->brws

   LD    (HLI), A                      ; save part=remainder

   LD    A, C                          ;
   SRL   A                             ;
   CPL                                 ;
   INC   A                             ;
   LD    (HLI), A                      ; save part

   LD    A, B                          ;
   OR    #1                            ;
   LD    (HLI), A                      ;
   LD    A, B                          ;
   LD    (HLI), A                      ; save sign

   LD    A, E                          ;
   LD    (HLI), A                      ;
   LD    A, D                          ;
   LD    (HLI), A                      ; save whole

   POP   DE                            ; DE=start

   LD    A, E                          ;
   LD    (HLI), A                      ;
   LD    (HL), D                       ; save current

   LD    A, C                          ;
   LD    (steps), A                    ;
   CPL                                 ;
   INC   A                             ;
   LD    (negsteps), A                 ;

   POP   BC

   RET                                 ;

; steps, negsteps                                t[1],n[1]
; part, error, sign, whole, current              p[1],r[1],x[2],w[2],y[2]
   
;  current=start
;  error=0-steps/2
;
;  delta=end-start
;  sign=(delta<0)?-1:1
;  whole=delta/steps       (signed)
;  part=abs(delta)%steps

;----------------------------------------------------------------------------
;[[ WORD brw_step(struct brw *struc) ]] 38 bytes 60 clocks
;----------------------------------------------------------------------------
_brw_step::
   LDHL  SP, #2
   LD    A, (HLI)
   LD    H, (HL)
   LD    L, A

   PUSH  BC

   LD    A, (negsteps)                 ;
   LD    C, A                          ;

   LD    A, (HLI)                      ; A=part
   ADD   A, (HL)                       ; A+=error
   LD    B, A                          ; B=error
   SBC   A, A                          ; A=(CY)?0xFF:0x00
   LD    D, A                          ; D=(error>255)?0xFF:0x00
   AND   C                             ; C=(error>255)?negsteps:0
   ADD   A, B                          ;
   LD    (HLI), A                      ; error+=part+(error>256)?negsteps:0
   LD    A, (HLI)                      ; HL->sign
   AND   D                             ;
   LD    E, A                          ;
   LD    A, (HLI)                      ;
   AND   D                             ;
   LD    D, A                          ; DE=(error<0)?sign:0
   LD    A, (HLI)                      ; HL->whole
   ADD   A, E                          ;
   LD    E, A                          ;
   LD    A, (HLI)                      ;
   ADC   A, D                          ;
   LD    D, A                          ;
   LD    A, (HL)                       ; HL->current
   ADD   A, E                          ;
   LD    E, A                          ;
   LD    (HLI), A                      ;
   LD    A, (HL)                       ;
   ADC   A, D                          ;
   LD    D, A                          ;
   LD    (HLI), A                      ; current+=whole+(error<0)?sign:0

   POP   BC

   RET

;----------------------------------------------------------------------------
;[ DIV_HL_C_S ] by Hideaki Omuro
;----------------------------------------------------------------------------
;parameters:
; HL = 16-bit dividend, C = 8-bit divisor
;returns:
; A = remainder, B = 0, HL = HL / C
;----------------------------------------------------------------------------
DIV_HL_C_S:
   XOR   A
   LD    B, #0x10
_dHLCs_Loop1:
   ADD   HL, HL                        ; 24-bit shift  A <- H <- L
   RLA
   CP    C
   JR    C, _dHLCs_Loop2
   SUB   C
   INC   L
_dHLCs_Loop2:
   DEC   B
   JR    NZ, _dHLCs_Loop1
   RET
