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

_window_ptr::
.ds 2

	.area _CODE

; Backup a rectangle region of the map
; Global:
;  _window_ptr = pointer to upper left of map data
; Input:
;  void backup_tiles(ubyte x, ubyte y, ubyte w, ubyte h, ubyte *dest);
;
;  x = x tile of upper left of rectangle
;  y = y tile of upper left of rectangle
;  w = width of rectangle to save
;  h = height of rectangle to save
;  dest = pointer to where to store rectangle
;
; Output:
;  (dest) = width x height rectangle of sprite numbers
_backup_tiles::
 ldhl sp, #7
 push	bc
 ld   a, (hld)
 ld   b, a
 ld   a, (hld)
 ld   c, a        ; bc = backup_dest
 push bc
 ld   a, (hld)
 ld   c, a        ; c = h
 ld   a, (hld)
 ld   b, a        ; b = w
 ld   a, (hld)    ; a = y
 rrca
 rrca
 rrca
 ld   e, a
 and  #0x1F
 ld   d, a
 xor  e           ; da = y * 32
 add  a, (hl)     ; add x
 ld   e, a
 adc  a, d
 sub  e
 ld   d, a
 ld   hl, #_window_ptr
 ld   a, (hli)
 ld   h, (hl)
 ld   l, a
 add  hl, de       ; (Y * 32) + X + MAP OFFSET
 pop  de
_backup_copy_main_loop:
 push bc
 push hl
_backup_copy:
 ld   a, (hl)
 inc  a
 jr   z,_backup_copy
 dec  a
 ld   (de), a
 inc  hl
 inc  de
 dec  b
 jr   nz, _backup_copy
 pop  hl
 ld   c, #32
 add  hl, bc
 pop  bc
 dec  c
 jr   nz, _backup_copy_main_loop
 pop  bc
 ret

; Write a rectangle region of the map
; Global:
;  _window_ptr = pointer to upper left of map data
; Input:
;  void backup_tiles(ubyte x, ubyte y, ubyte w, ubyte h, ubyte *dest);
;
;  x = x tile of upper left of rectangle
;  y = y tile of upper left of rectangle
;  w = width of rectangle to save
;  h = height of rectangle to save
;  dest = pointer to source of tiles to write
;
; Output:
;  window will be written with rectangle w*h
_restore_tiles::
 ldhl sp, #7

 push	bc
 ld   a, (hld)
 ld   b, a
 ld   a, (hld)
 ld   c, a        ; bc = backup_dest
 push bc
 ld   a, (hld)
 ld   c, a        ; c = h
 ld   a, (hld)
 ld   b, a        ; b = w
 ld   a, (hld)    ; a = y
 rrca
 rrca
 rrca
 ld   e, a
 and  #0x1F
 ld   d, a
 xor  e           ; da = y * 32
 add  a, (hl)     ; add x
 ld   e, a
 adc  a, d
 sub  e
 ld   d, a
 ld   hl, #_window_ptr
 ld   a, (hli)
 ld   h, (hl)
 ld   l, a
 add  hl, de       ; (Y * 32) + X + MAP OFFSET
 pop  de           ; de = where to get the rectangle
_restore_copy_main_loop:
 push bc
 push hl
_restore_copy:
 ld   a, (de)
   RST   ldi_v_a
 inc  de
 dec  b
 jr   nz, _restore_copy
 pop  hl
 ld   c, #32
 add  hl, bc
 pop  bc
 dec  c
 jr   nz, _restore_copy_main_loop
 pop  bc
 ret

; Draws a border with contents all tile 4
; Input:
;	_window_ptr = upper left corner of window data
; Output:
;	window drawn like following:
;	0 1x18 2
;       3 4x18 5 x16
;	6 7x18 8

;_reset_window::
; push bc
; ld   de, #12
; ld   hl, #_window_ptr
; ld   a, (hli)
; ld   h, (hl)
; ld   l, a
; xor  a
; call _window_draw_horizontal
; ld   a,#3
; ld   c,#16
;_fill_blank_loop:
; call _window_draw_horizontal
; sub  #2
; dec  c
; jr   nz,_fill_blank_loop
; ld   a,#6
; call _window_draw_horizontal
; pop  bc
; ret
;_window_draw_horizontal:
; call ld_vram_a
; inc  a
; ld   b,#18
;_window_draw_horizontal_loop:
; call ld_vram_a
; dec  b
; jr   nz,_window_draw_horizontal_loop
; inc  a
; call ld_vram_a
; add  hl, de
; ret

; Draws a tile into window_ptr at (x,y)
; Input:
;  _window_ptr = upper left corner of screen data
;  void backup_tiles(ubyte x, ubyte y, ubyte tile_num);
;
;  x = x tile of upper left of rectangle
;  y = y tile of upper left of rectangle
;  tile_nume = tile number
;
; Output:
;  tile drawn to window_ptr at (x,y)
_put_tile::
   LDHL  SP, #4
   LD    A, (HLD)
   PUSH  AF
   LD    A, (HLD)                      ; a = y
   LD    L, (HL)

   CALL  _getwindowptr

   POP   AF
   RST   ldi_v_a
   RET

; push bc
; ld   a, (hld)
; ld   b, a
; ld   a, (hld)  
; rrca
; rrca
; rrca
; ld   e, a
; and  #0x1F
; ld   d, a
; xor  e           ; da = y * 32
; add  a, (hl)     ; add x
; ld   e, a
; adc  a, d
; sub  e
; ld   d, a
; ld   hl, #_window_ptr
; ld   a, (hli)
; ld   h, (hl)
; ld   l, a
; add  hl, de       ; (Y * 32) + X + MAP OFFSET
; ld   a,b
;   RST   ldi_v_a
;; ld   (hl),a
; pop  bc
; ret

; A=Y, L=X, (_window_ptr) -> window
_getwindowptr:
   RRCA
   RRCA
   RRCA
   LD    E, A
   AND   #0x1F
   LD    D, A
   XOR   E                             ; da = y * 32

   ADD   A, L
   LD    E, A
   ADC   A, D
   SUB   E
   LD    D, A

   LD    A, H

   LD    HL, #_window_ptr
   LD    A, (HLI)
   LD    H, (HL)
   LD    L, A

   ADD   HL, DE
   RET

; Get a tile into window_ptr at (x,y)
; Input:
;  _window_ptr = upper left corner of screen data
;  ubyte get_tile(ubyte x, ubyte y);
;
;  x = x tile of upper left of rectangle
;  y = y tile of upper left of rectangle
;  tile_nume = tile number
;
_get_tile::
   LDHL  SP, #3
   LD    A, (HLD)                      ; a = y
   LD    L, (HL)

   CALL  _getwindowptr

   CALL  ld_a_vram

   LD    E, A
   RET
