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

   .AREA _CODE

;----------------------------------------------------------------------------
;[[ void bre_setup(word start, word end, byte steps, struct bre *struc) ]]
;----------------------------------------------------------------------------
_bre_setup::
   LDHL  SP, #2

   PUSH  BC

   LD    A, (HLI)                      ; D=start
   LD    D, A                          ;
   LD    A, (HLI)                      ; B=end-start (delta)
   SUB   D                             ;
   LD    B, A                          ;

   ADD   A, A                          ; E=(delta<0)?0xFF:0x00
   SBC   A, A                          ;
   LD    E, A                          ;

   LD    A, (HLI)                      ; C=steps
   LD    C, A                          ;

   LD    A, (HLI)                      ; HL->bres
   LD    H, (HL)                       ;
   LD    L, A                          ;

   PUSH  HL                            ;

   LD    A, B                          ; A=abs(delta)
   XOR   E                             ;
   SUB   E                             ;
   LD    H, A                          ;

   CALL  DIV_H_C_S                     ; H=delta/steps

   LD    L, A                          ; L=remainder

   LD    A, H                          ; B=sgn(delta)*quotient
   XOR   E                             ;
   SUB   E                             ;
   LD    B, A                          ;

   LD    A, L                          ; A=remainder
   POP   HL                            ; HL->bres

   LD    (HLI), A                      ; save part=remainder

   LD    A, C                          ;
   SRL   A                             ;
   CPL                                 ;
   INC   A                             ;
   LD    (HLI), A                      ; save error

   LD    A, E                          ;
   OR    #1                            ;
   LD    (HLI), A                      ; save sign

   LD    A, B                          ;
   LD    (HLI), A                      ; save whole

   LD    (HL), D                       ; save current

   LD    A, C                          ;
   LD    (steps), A                    ;
   CPL                                 ;
   INC   A                             ;
   LD    (negsteps), A                 ;

   POP   BC

   RET                                 ;

;  current=start
;  error=0-steps/2
;
;  delta=end-start
;  sign=(delta<0)?-1:1
;  whole=delta/steps       (signed)
;  part=abs(delta)%steps

; steps, negsteps                                t[1],n[1]
; part, error, sign, whole, current              p[1],r[1],x[1],w[1],y[1]
   
;----------------------------------------------------------------------------
;[[ BYTE bre_step(struct brw *struc) ]] 38 bytes 60 clocks
;----------------------------------------------------------------------------
_bre_step::
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
   LD    E, A                          ; D=(error<0)?sign:0
   LD    A, (HLI)                      ; HL->whole
   ADD   A, E                          ;
   LD    E, A                          ;
   LD    A, (HL)                       ; HL->current
   ADD   A, E                          ;
   LD    E, A                          ;
   LD    (HLI), A                      ; current+=whole+(error<0)?sign:0

   POP   BC

   RET

;----------------------------------------------------------------------------
;[ DIV_H_C_S ] by Hideaki Omuro
;----------------------------------------------------------------------------
;parameters:
; H = 8-bit dividend, C = 8-bit divisor
;returns:
; A = H % C, B = 0, H = H / C
;----------------------------------------------------------------------------
DIV_H_C_S:
   XOR   A
   LD    B, #0x08
_dHCs_Loop1:
   SLA   H                             ; 16-bit shift  A <- H <- L
   RLA
   CP    C
   JR    C, _dHCs_Loop2
   SUB   C
   INC   H
_dHCs_Loop2:
   DEC   B
   JR    NZ, _dHCs_Loop1
   RET
