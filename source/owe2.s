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

_ram_page_owe           = 6            ; reserve from D8 - DF

_owe2_sort_table        = 0xD8F4

.area _CODE

_tilt_hbl::
   PUSH  BC

;   LDH   A, (0x44)                     ;
;   SUB   #143                          ;
;   JR    Z, tilt_line0                 ;
;   ADD   A, #144                       ;
;tilt_line0:
;   LDH   (0x45), A                     ;

   LDH   A, (P_LY)
   INC   A
   LD    B, A
   SUB   #144
   SBC   A, A
   AND   B
   INC   A
   LDH   (P_LYC), A

   DEC   A
   JR    NZ, not_done_owe2

;      CPU_W((x+(i+(y&3))/4)&255,0xFF43); // tilt

   PUSH  AF
   POP   AF
   PUSH  AF
   POP   AF
   PUSH  AF
   POP   AF

not_done_owe2:
   SUB   #77
   LD    B, A

   LD    A, #5
_owe2_tiltdelay:
   DEC   A
   JR    NZ, _owe2_tiltdelay

   LDH   A, (P_SCY)
   INC   A
   AND   #0x03
   ADD   A, B
   SRA   A
   SRA   A
   LD    B, A
   LDH   A, (_rscx)
   ADD   A, B
   LDH   (P_SCX), A

   POP   BC
   JP    _hbl_end

;----------------------------------------------------------------------------
_int_owe2::                            ; Overworld Engine Interrupt
;----------------------------------------------------------------------------

;. Initialize the overworld engine interrupt ._._._._._._._._._._._._._._._.

;   XOR   A
;   LDH   (P_LYC), A
;   INC   A
;   LDH   (_hbimode), A

   LD    A, #0xC0                      ;
   CALL  oam_dma                       ;

   LD    HL, #0xFF40                   ; HL -> LCD controller
   SET   1, (HL)                       ; Sprites ON

   EI

   LD    A, (_owe_status)
   OR    A
   JP    Z, _vbl_end

   CALL  _animation

   LDH   A, (_rampage)                 ;
   PUSH  AF                            ;
   LD    A, #_ram_page_owe             ; Map D000-DFFF to owe's ram page
   LDH   (_rampage), A                 ;
   LDH   (P_SVBK), A                   ;

;. Do x scrolling ._._._._._._._._._._._._._._._._._._._._._._._._._._._._.

   LD    HL, #_owe_destx               ; HL -> destx
   LD    A, (HLI)                      ; BC = (destx)
   LD    C, A                          ;
   LD    A, (HLI)                      ;
   LD    B, A                          ; HL -> scrollx

   LD    A, (_bspr_quake)              ;
   OR    A                             ;
   JR    Z, no_quakex_owe2             ;
   CALL  randfuncasm                   ;
   AND   #0x06                         ;
   SUB   #0x04                         ;
no_quakex_owe2:

   LD    D, A
   LD    A, (HLI)                      ; (HL)E = (scrollx)
   LD    E, A                          ;
   ADD   A, D                          ;
   LDH   (_rscx), A                    ;

   LD    D, A
   LDH   A, (P_SCY)
   INC   A
   AND   #0x03
   SUB   #77
   SRA   A
   SRA   A
   ADD   A, D
   LDH   (P_SCX), A
   LD    A, D

   LD    D, (HL)                       ; (LCDSCX) = scrollx

   SUB   C                             ; is scrollx == destx?
   LD    L, A                          ;
   LD    A, D                          ;
   SBC   A, B                          ;
   LD    H, A                          ;
   OR    L                             ;
   JR    Z, owe2_no_scroll_x           ; BC = scrollx - destx

   ;CALL  owe_cp_bc_de                  ; BA = 1 (NC) or -1 (CY)

   CALL  map_signed_cp
   SBC   A, A                          ;
   LD    B, A                          ;
   OR    #0x01                         ;
   ADD   A, A                          ;

   ADD   A, E                          ; AE += BA
   LD    E, A                          ;
   LD    A, D                          ;
   ADC   A, B                          ;

   LD    D, A                          ;

   LD    HL, #_owe_scrollx + 1         ; HL -> destx
   LD    (HLD), A                      ; (scrollx) = AE
   LD    (HL), E                       ; HL -> scrollx(h)

owe2_no_scroll_x:
   LD    A, E                          ;

   LD    HL, #_owe_neg_scrollx         ; (neg_scrollx) = AE
   SUB   #16                           ;
   CPL                                 ; DE = -(scrollx) + 15
   LD    (HLI), A                      ;

   LD    A, D                          ;
   SBC   A, #0x00                      ;
   CPL                                 ;
   LD    (HLD), A                      ; 

;. Do y scrolling ._._._._._._._._._._._._._._._._._._._._._._._._._._._._.

   LD    HL, #_owe_desty               ; HL -> desty
   LD    A, (HLI)                      ; BC = (desty)
   LD    C, A                          ;
   LD    A, (HLI)                      ;
   LD    B, A                          ; HL -> scrolly

   LD    A, (_bspr_quake)              ;
   OR    A                             ;
   JR    Z, no_quakey_owe2             ;
   CALL  randfuncasm                   ;
   AND   #0x06                         ;
   SUB   #0x04                         ;
no_quakey_owe2:

   LD    D, A
   LD    A, (HLI)                      ; (HL)E = (scrolly)
   LD    E, A                          ;
   ADD   A, D                          ;
   LDH   (P_SCY), A                    ;
   LDH   (_rscy), A                    ;
   LD    D, (HL)                       ; (LCDSCY) = scrolly

   SUB   C                             ; is scrollx == destx?
   LD    L, A                          ;
   LD    A, D                          ;
   SBC   A, B                          ;
   LD    H, A                          ;
   OR    L                             ;
   JR    Z, owe2_no_scroll_y           ;

   CALL  map_signed_cp

;   CALL  owe_cp_bc_de                  ;
   SBC   A, A                          ; BA = 1 (NC) or -1 (CY)
   LD    B, A                          ;
   OR    #0x01                         ;
   ADD   A, A                          ;

   ADD   A, E                          ; DE += BA
   LD    E, A                          ;
   LD    A, D                          ;
   ADC   A, B                          ;

   LD    D, A                          ;

   LD    HL, #_owe_scrolly + 1         ; HL -> desty
   LD    (HLD), A                      ; (scrolly) = DE
   LD    (HL), E                       ;

owe2_no_scroll_y:
   LD    A, E                          ;

   LD    HL, #_owe_neg_scrolly         ; (neg_scrolly) = AE
   SUB   #16                           ;
   CPL                                 ; DE = -(scrolly) + 15
   LD    (HLI), A                      ;

   LD    A, D                          ;
   SBC   A, #0                         ;
   CPL                                 ;
   LD    (HLD), A                      ; 

;. Draw edges of map ._._._._._._._._._._._._._._._._._._._._._._._._._._.

   CALL  _init_sxy

   LD    A, #1                         ; Map D000-DFFF to map's ram page
   LDH   (_rampage), A                 ;
   LDH   (P_SVBK), A                   ;

   LDH   A, (P_VBK)
   PUSH  AF

   LD    A, #2                         ;
_scrollupdate2:
   PUSH  AF                            ;
   
   LD    HL, #_owe_counter             ;
   INC   (HL)                          ;

   LD    A, (_owe_sx)                  ;
   INC   D
   INC   D
   INC   D
   INC   D
   LD    D, A                          ;
   LD    E, (HL)                       ;
   PUSH  DE                            ;
   CALL  Update_VRAM                   ;
   POP   DE                            ;
   DEC   D                             ;
   CALL  Update_VRAM                   ;
      
   LD    A, (_owe_counter)             ;
   LD    D, A                          ;
   LD    A, (_owe_sy)                  ;
   INC   E
   INC   E
   INC   E
   INC   E
   LD    E, A                          ;
   PUSH  DE                            ;
   CALL  Update_VRAM                   ;
   POP   DE                            ;
   DEC   E                             ;
   CALL  Update_VRAM                   ;

   POP   AF
   DEC   A
   JR    NZ, _scrollupdate2

   POP   AF
   LDH   (P_VBK), A

;. Draw sprites ._._._._._._._._._._._._._._._._._._._._._._._._._._._._.

   LD    A, #_ram_page_owe             ; Map D000-DFFF to owe's ram page
   LDH   (_rampage), A                 ;
   LDH   (P_SVBK), A                   ;

   LD    DE, #_owe2_sort_table         ;
   LD    HL, #_owe_sort_table_ptr      ;
   LD    A, E                          ;
   LD    (HLI), A                      ;
   LD    (HL), D                       ;

   LD    H, D                          ;
   LD    L, E                          ;
   XOR   A                             ;
   LD    (HLI), A                      ;
   LD    (HLI), A                      ;
   LD    (HLI), A                      ;
   LD    (HLI), A                      ;
   LD    (HLI), A                      ;
   LD    (HLI), A                      ;
   LD    (HLI), A                      ;
   LD    (HLI), A                      ;
   LD    (HLI), A                      ;
   LD    (HLI), A                      ;
   LD    (HLI), A                      ;
   LD    (HLI), A                      ;

   LD    HL, #_owe_spritearray         ; HL = bspr_16array
   LD    C, A                          ;
   LD    A, #5                         ;

owe2int_sprset_loop:
   PUSH  AF

   LD    A, (HLI)
   ADD   A, A   
   JR    Z, owe2int_spr_inactive_nopush

   PUSH  HL

; active     - 1 byte
; x          - 2 bytes
; z          - 1 byte
; y          - 2 bytes
; frames_bak - 2 bytes
; frames     - 2 bytes * 16 entries

   LD    A, (_owe_neg_scrollx)         ; E = X coordinate of sprite
   ADD   A, (HL)                       ;
   LD    E, A                          ;
   INC   HL                            ;

   LD    A, (_owe_neg_scrollx + 1)     ; If zero then sprite is active
   ADC   A, (HL)                       ;
   JR    NZ, owe2int_spr_inactive      ;
   INC   HL                            ;

   INC   HL                            ; B = sorting Y coordinate of sprite
   LD    A, (_owe_neg_scrolly)         ;
   ADD   A, (HL)                       ;
   LD    B, A                          ;
   DEC   HL                            ;

   PUSH  AF                            ; D = displayed Y coordinate of sprite
   SUB   (HL)                          ;
   INC   HL                            ;
   INC   HL                            ;
   LD    D, A                          ;
   POP   AF                            ;

   LD    A, (_owe_neg_scrolly + 1)     ; If zero then sprite is active
   ADC   A, (HL)                       ;
   JR    NZ, owe2int_spr_inactive      ;
   INC   HL

   LD    A, E                          ; check if inbounds
   CP    #175                          ;
   JR    NC, owe2int_spr_inactive      ;

   LD    A, D                          ; check if inbounds
   CP    #159                          ;
   JR    NC, owe2int_spr_inactive      ;

   PUSH  HL                            ; HL -> OAM
   LD    H, C                          ;
   PUSH  HL                            ;
   LD    H, #_owe_spr_presort_page     ;
   LD    L, C                          ;

   INC   D
   LD    A, D
   LD    (HLI), A

   LD    A, B

;   LD    A, E
;   SUB   #89  ;105    ;81

   SUB   #80      ;#81
   CPL
   SRA   A
   SRA   A
   ADD   A, E
   LD    E, A

   SUB   #7
   LD    (HLI), A
   INC   L
   INC   L

   LD    A, D
   LD    (HLI), A
   LD    A, E
   INC   A
   LD    (HLI), A

   LD    HL, #_owe_sort_table_ptr
   LD    A, (HLI)
   LD    D, (HL)
   LD    E, A

   LD    A, B
   LD    (DE), A
   INC   E
   POP   AF
   LD    (DE), A
   INC   E

   LD    A, D
   LD    (HLD), A
   LD    (HL), E

   POP   HL
   POP   DE

   INC   HL
   INC   HL
   INC   HL
   INC   HL

   LD    A, #8
   ADD   A, C
   LD    C, A                          ; point C to next entry

   JR    owe2int_spr_active

owe2int_spr_inactive:
   POP   HL
owe2int_spr_inactive_nopush:
   LD    A, #9
   ADD   A, L
   LD    L, A
   ADC   A, H
   SUB   L
   LD    H, A                          ; point HL to next entry

   LD    A, #8
   ADD   A, C
   LD    C, A                          ; point C to next entry

owe2int_spr_active:
   POP   AF
   DEC   A

   JP    NZ, owe2int_sprset_loop

   CALL  _sort_owe2

   CALL  _project_owe2

;. Finish Interrupt ._._._._._._._._._._._._._._._._._._._._._._._._._._.

;   LD    A, (cur_page_ram)             ; Map D000-DFFF to bspr's ram page
;   LDH   A, (_rampage)
;   LDH   (P_SVBK), A                   ;

_quit_vbl_int2:

   POP   AF
   LDH   (_rampage), A                 ;
   LDH   (P_SVBK), A                   ;

_vbl_owe2_end:
   JP    _vbl_end

_sort_owe2:
   LD    DE, #_owe2_sort_table
   LD    H, D
   LD    L, E

;   LD    A, #_ram_page_owe             ; Map D000-DFFF to owe's ram page
;   LDH   (_rampage), A                 ;
;   LDH   (P_SVBK), A                   ;

; DE -> table of y-values + number xxE0-xxFF  byte to swap
; HL -> table                                 byte to swap
; C = pointer
_sort::
   LD    C, E                          ; C->E
   LD    L, C                          ; L->C
_sort_loop:
   LD    A, (DE)                       ; search for biggest y value from E
   CP    (HL)                          ; compare current
   JR    C, _sort_smaller              ; if cur.y is smaller (behind)
   JR    NZ, _sort_ok                  ; if cur.y is bigger (in front)
   INC   E                             ; if cur.y is same (compare index)
   INC   L                             ; E->cur.idx L->big.idx
   LD    A, (DE)                       ; which is bigger?
   CP    (HL)                          ; compare current
   DEC   HL                            ; don't modify flags, L->big.y
   JR    NC, _sort_smaller2            ; if cur.idx is bigger, go to next
   DEC   E                             ; E->cur.y
_sort_ok:
   LD    L, E                          ; L->newest big one
_sort_smaller:
   INC   E                             ; E->cur.idx
_sort_smaller2:
   INC   E                             ; E->next.y
   JR    NZ, _sort_loop                ;
   LD    E, C                          ; E->start.y
   INC   C                             ; C->next.y
   INC   C                             ;
   LD    A, (DE)                       ; A=start.y
   LD    B, (HL)                       ; B=big.y
   LD    (HLI), A                      ; swap them
   LD    A, B                          ;
   LD    (DE), A                       ;
   INC   E                             ;
   LD    A, (DE)                       ;
   LD    B, (HL)                       ;
   LD    (HLI), A                      ;
   LD    A, B                          ;
   LD    (DE), A                       ;
   INC   E                             ;
   JR    NZ, _sort                     ;
   RET

_project_owe2:
   LD    B, #_owe_spr_presort_page
   LD    HL, #0xC000
   LD    DE, #_owe2_sort_table

_project_loop3:
   LD    A, (DE)
   OR    A
   JR    Z, _blankrestoftable2
   INC   E
   LD    A, (DE)
   LD    C, A

   LD    A, (BC)
   INC   C
   LD    (HLI), A
   LD    A, (BC)
   INC   C
   LD    (HLI), A
   LD    A, (BC)
   INC   C
   LD    (HLI), A
   LD    A, (BC)
   INC   C
   LD    (HLI), A
   LD    A, (BC)
   INC   C
   LD    (HLI), A
   LD    A, (BC)
   INC   C
   LD    (HLI), A
   LD    A, (BC)
   INC   C
   LD    (HLI), A
   LD    A, (BC)
   INC   C
   LD    (HLI), A
   INC   E
   JR    NZ, _project_loop3
   RET

_blankrestoftable2:
_blankrestoftable2_loop:
   XOR   A
   LD    (HLI), A
   LD    (HLI), A
   LD    (HLI), A
   LD    (HLI), A
   LD    A, L
   CP    #0xA0
   JR    NZ, _blankrestoftable2_loop
   RET
