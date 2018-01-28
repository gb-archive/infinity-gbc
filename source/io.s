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
; mem I/O
;----------------------------------------------------------------------------

_ram_page_svm = 5

_save_mirror  = 0xD800      ; 1k or 2k

;   enable  *(0x0000) = 0x0A
;   disable *(0x0000) = 0x00
;   switch  *(0x4000) = num

   .area _BSS

   .area _CODE

_getslotptr:
   LDHL  SP, #4
   LD    A, (HL)
   ADD   A, A
   ADD   A, A
   ADD   A, A
   ADD   A, #0xA0
   LD    H, A
   XOR   A
   LD    L, A
   RET

_fopen::
   CALL  _getslotptr
   LD    DE, #_save_mirror

_fjoin:
   PUSH  BC
   LDH   A, (_rampage)
   PUSH  AF
   LD    A, #_ram_page_svm
   LDH   (_rampage), A
   LDH   (P_SVBK), A

   XOR   A
   LD    (0x4000), A
   LD    A, #0x0A
   LD    (0x0000), A

   LD    BC, #1024
   CALL  _LDIR

   XOR   A
   LD    (BC), A

   POP   AF
   LDH   (_rampage), A
   LDH   (P_SVBK), A
   POP   BC
   RET

_fclose::
   CALL  _getslotptr
   LD    D, H
   LD    E, L
   LD    HL, #_save_mirror
   JR    _fjoin
