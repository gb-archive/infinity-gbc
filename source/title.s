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
; Title Screen routine
;----------------------------------------------------------------------------
   .include "gbasm.h"

; RAM mapping for title screen
;
; C100      = fade_count
; C101      = dec_yes
; C102      = title_y
; C103      = title_timer
; C104      = key_cur
; C105      = key_old
; C106      = _pfs_fadelen
; D000-     = program
; DAB7-DFFF = data

_blackpal   = 0x0000
_whitepal   = 0x0001
_redpal     = 0x0002
_bluepal    = 0x0003

_ram_page_pfs        = 0x01
_ram_page_title      = 0x01
_rom_page_title      = 0x0E

fade_count           = 0xDAB7
dec_yes              = 0xDAB8
title_y              = 0xDAB9
title_timer          = 0xDABA
key_cur              = 0xDABB
key_old              = 0xDABC

scrx                 = 0xDABD       ;  66 bytes
_pfs_fadelen         = 0xDAFF
_pal_cur2            = 0xDB00       ; 128 bytes
_pfs_rgb_src         = 0xDB80       ; 192 bytes (original palette)
_pfs_rgb_dst         = 0xDC40       ; 192 bytes (destination palette)
_pfs_bresen          = 0xDD00       ; 768 bytes (bresenham structure)

_ram_routine_dest    = 0xD000
_ram_routine_call    = 0xD000 - 0x4800

_rompage             = 0xF0
_rampage             = 0xF1
_rscx                = 0xFA

ldi_v_a     = 8

order::
   .ds 1
row::
   .ds 1

;   .globl playing, currentbeat, song_page, song_ptr
;   .globl songnum, tempo, song_backup, song_struct, volume, instrument

   .globl _pal_cur

   .globl _LDIR, _LDIR_B, _LDIR_B_VRAM, _LDIR_VRAM_ACC
   .globl _STIR_VRAM, _STIR_B_VRAM
   .globl ld_vram_a_acc

   .area _CODE_14

.db 0x00,0x80,0x01,0x82,0x04,0x86,0x09,0x8C,0x10,0x94,0x19,0x9E,0x24,0xAA,0x31,0xB8
.db 0x40,0xC8,0x51,0xDA,0x64,0xEE,0x79,0x04,0x90,0x1C,0xA9,0x36,0xC4,0x52,0xE1,0x70
.db 0x00,0x90,0x21,0xB2,0x44,0xD6,0x69,0xFC,0x90,0x24,0xB9,0x4E,0xE4,0x7A,0x11,0xA8
.db 0x40,0xD8,0x71,0x0A,0xA4,0x3E,0xD9,0x74,0x10,0xAC,0x49,0xE6,0x84,0x22,0xC1,0x60
.db 0x00,0xA0,0x41,0xE2,0x84,0x26,0xC9,0x6C,0x10,0xB4,0x59,0xFE,0xA4,0x4A,0xF1,0x98
.db 0x40,0xE8,0x91,0x3A,0xE4,0x8E,0x39,0xE4,0x90,0x3C,0xE9,0x96,0x44,0xF2,0xA1,0x50
.db 0x00,0xB0,0x61,0x12,0xC4,0x76,0x29,0xDC,0x90,0x44,0xF9,0xAE,0x64,0x1A,0xD1,0x88
.db 0x40,0xF8,0xB1,0x6A,0x24,0xDE,0x99,0x54,0x10,0xCC,0x89,0x46,0x04,0xC2,0x81,0x40
.db 0x00,0xC0,0x81,0x42,0x04,0xC6,0x89,0x4C,0x10,0xD4,0x99,0x5E,0x24,0xEA,0xB1,0x78
.db 0x40,0x08,0xD1,0x9A,0x64,0x2E,0xF9,0xC4,0x90,0x5C,0x29,0xF6,0xC4,0x92,0x61,0x30
.db 0x00,0xD0,0xA1,0x72,0x44,0x16,0xE9,0xBC,0x90,0x64,0x39,0x0E,0xE4,0xBA,0x91,0x68
.db 0x40,0x18,0xF1,0xCA,0xA4,0x7E,0x59,0x34,0x10,0xEC,0xC9,0xA6,0x84,0x62,0x41,0x20
.db 0x00,0xE0,0xC1,0xA2,0x84,0x66,0x49,0x2C,0x10,0xF4,0xD9,0xBE,0xA4,0x8A,0x71,0x58
.db 0x40,0x28,0x11,0xFA,0xE4,0xCE,0xB9,0xA4,0x90,0x7C,0x69,0x56,0x44,0x32,0x21,0x10
.db 0x00,0xF0,0xE1,0xD2,0xC4,0xB6,0xA9,0x9C,0x90,0x84,0x79,0x6E,0x64,0x5A,0x51,0x48
.db 0x40,0x38,0x31,0x2A,0x24,0x1E,0x19,0x14,0x10,0x0C,0x09,0x06,0x04,0x02,0x01,0x00

.db 0x40,0x3F,0x3F,0x3E,0x3E,0x3D,0x3D,0x3C,0x3C,0x3B,0x3B,0x3A,0x3A,0x39,0x39,0x38
.db 0x38,0x37,0x37,0x36,0x36,0x35,0x35,0x35,0x34,0x34,0x33,0x33,0x32,0x32,0x31,0x31
.db 0x31,0x30,0x30,0x2F,0x2F,0x2E,0x2E,0x2D,0x2D,0x2D,0x2C,0x2C,0x2B,0x2B,0x2B,0x2A
.db 0x2A,0x29,0x29,0x29,0x28,0x28,0x27,0x27,0x27,0x26,0x26,0x25,0x25,0x25,0x24,0x24
.db 0x24,0x23,0x23,0x22,0x22,0x22,0x21,0x21,0x21,0x20,0x20,0x1F,0x1F,0x1F,0x1E,0x1E
.db 0x1E,0x1D,0x1D,0x1D,0x1C,0x1C,0x1C,0x1B,0x1B,0x1B,0x1A,0x1A,0x1A,0x19,0x19,0x19
.db 0x19,0x18,0x18,0x18,0x17,0x17,0x17,0x16,0x16,0x16,0x15,0x15,0x15,0x15,0x14,0x14
.db 0x14,0x13,0x13,0x13,0x13,0x12,0x12,0x12,0x12,0x11,0x11,0x11,0x11,0x10,0x10,0x10
.db 0x10,0x0F,0x0F,0x0F,0x0F,0x0E,0x0E,0x0E,0x0E,0x0D,0x0D,0x0D,0x0D,0x0C,0x0C,0x0C
.db 0x0C,0x0C,0x0B,0x0B,0x0B,0x0B,0x0A,0x0A,0x0A,0x0A,0x0A,0x09,0x09,0x09,0x09,0x09
.db 0x09,0x08,0x08,0x08,0x08,0x08,0x07,0x07,0x07,0x07,0x07,0x07,0x06,0x06,0x06,0x06
.db 0x06,0x06,0x05,0x05,0x05,0x05,0x05,0x05,0x05,0x04,0x04,0x04,0x04,0x04,0x04,0x04
.db 0x04,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x02,0x02,0x02,0x02,0x02,0x02,0x02
.db 0x02,0x02,0x02,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01
.db 0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
.db 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00


.db 0x00,0x00,0x01,0x02,0x04,0x06,0x09,0x0C,0x10,0x14,0x19,0x1E,0x24,0x2A,0x31,0x38
.db 0x40,0x48,0x51,0x5A,0x64,0x6E,0x79,0x84,0x90,0x9C,0xA9,0xB6,0xC4,0xD2,0xE1,0xF0
.db 0x00,0x10,0x21,0x32,0x44,0x56,0x69,0x7C,0x90,0xA4,0xB9,0xCE,0xE4,0xFA,0x11,0x28
.db 0x40,0x58,0x71,0x8A,0xA4,0xBE,0xD9,0xF4,0x10,0x2C,0x49,0x66,0x84,0xA2,0xC1,0xE0
.db 0x00,0x20,0x41,0x62,0x84,0xA6,0xC9,0xEC,0x10,0x34,0x59,0x7E,0xA4,0xCA,0xF1,0x18
.db 0x40,0x68,0x91,0xBA,0xE4,0x0E,0x39,0x64,0x90,0xBC,0xE9,0x16,0x44,0x72,0xA1,0xD0
.db 0x00,0x30,0x61,0x92,0xC4,0xF6,0x29,0x5C,0x90,0xC4,0xF9,0x2E,0x64,0x9A,0xD1,0x08
.db 0x40,0x78,0xB1,0xEA,0x24,0x5E,0x99,0xD4,0x10,0x4C,0x89,0xC6,0x04,0x42,0x81,0xC0
.db 0x00,0x40,0x81,0xC2,0x04,0x46,0x89,0xCC,0x10,0x54,0x99,0xDE,0x24,0x6A,0xB1,0xF8
.db 0x40,0x88,0xD1,0x1A,0x64,0xAE,0xF9,0x44,0x90,0xDC,0x29,0x76,0xC4,0x12,0x61,0xB0
.db 0x00,0x50,0xA1,0xF2,0x44,0x96,0xE9,0x3C,0x90,0xE4,0x39,0x8E,0xE4,0x3A,0x91,0xE8
.db 0x40,0x98,0xF1,0x4A,0xA4,0xFE,0x59,0xB4,0x10,0x6C,0xC9,0x26,0x84,0xE2,0x41,0xA0
.db 0x00,0x60,0xC1,0x22,0x84,0xE6,0x49,0xAC,0x10,0x74,0xD9,0x3E,0xA4,0x0A,0x71,0xD8
.db 0x40,0xA8,0x11,0x7A,0xE4,0x4E,0xB9,0x24,0x90,0xFC,0x69,0xD6,0x44,0xB2,0x21,0x90
.db 0x00,0x70,0xE1,0x52,0xC4,0x36,0xA9,0x1C,0x90,0x04,0x79,0xEE,0x64,0xDA,0x51,0xC8
.db 0x40,0xB8,0x31,0xAA,0x24,0x9E,0x19,0x94,0x10,0x8C,0x09,0x86,0x04,0x82,0x01,0x80

.db 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
.db 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
.db 0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x02,0x02
.db 0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03
.db 0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x05,0x05,0x05,0x05,0x05,0x05,0x05,0x06
.db 0x06,0x06,0x06,0x06,0x06,0x07,0x07,0x07,0x07,0x07,0x07,0x08,0x08,0x08,0x08,0x08
.db 0x09,0x09,0x09,0x09,0x09,0x09,0x0A,0x0A,0x0A,0x0A,0x0A,0x0B,0x0B,0x0B,0x0B,0x0C
.db 0x0C,0x0C,0x0C,0x0C,0x0D,0x0D,0x0D,0x0D,0x0E,0x0E,0x0E,0x0E,0x0F,0x0F,0x0F,0x0F
.db 0x10,0x10,0x10,0x10,0x11,0x11,0x11,0x11,0x12,0x12,0x12,0x12,0x13,0x13,0x13,0x13
.db 0x14,0x14,0x14,0x15,0x15,0x15,0x15,0x16,0x16,0x16,0x17,0x17,0x17,0x18,0x18,0x18
.db 0x19,0x19,0x19,0x19,0x1A,0x1A,0x1A,0x1B,0x1B,0x1B,0x1C,0x1C,0x1C,0x1D,0x1D,0x1D
.db 0x1E,0x1E,0x1E,0x1F,0x1F,0x1F,0x20,0x20,0x21,0x21,0x21,0x22,0x22,0x22,0x23,0x23
.db 0x24,0x24,0x24,0x25,0x25,0x25,0x26,0x26,0x27,0x27,0x27,0x28,0x28,0x29,0x29,0x29
.db 0x2A,0x2A,0x2B,0x2B,0x2B,0x2C,0x2C,0x2D,0x2D,0x2D,0x2E,0x2E,0x2F,0x2F,0x30,0x30
.db 0x31,0x31,0x31,0x32,0x32,0x33,0x33,0x34,0x34,0x35,0x35,0x35,0x36,0x36,0x37,0x37
.db 0x38,0x38,0x39,0x39,0x3A,0x3A,0x3B,0x3B,0x3C,0x3C,0x3D,0x3D,0x3E,0x3E,0x3F,0x3F


.db 0x00,0x80,0x01,0x82,0x04,0x86,0x09,0x8C,0x10,0x94,0x19,0x9E,0x24,0xAA,0x31,0xB8
.db 0x40,0xC8,0x51,0xDA,0x64,0xEE,0x79,0x04,0x90,0x1C,0xA9,0x36,0xC4,0x52,0xE1,0x70
.db 0x00,0x90,0x21,0xB2,0x44,0xD6,0x69,0xFC,0x90,0x24,0xB9,0x4E,0xE4,0x7A,0x11,0xA8
.db 0x40,0xD8,0x71,0x0A,0xA4,0x3E,0xD9,0x74,0x10,0xAC,0x49,0xE6,0x84,0x22,0xC1,0x60
.db 0x00,0xA0,0x41,0xE2,0x84,0x26,0xC9,0x6C,0x10,0xB4,0x59,0xFE,0xA4,0x4A,0xF1,0x98
.db 0x40,0xE8,0x91,0x3A,0xE4,0x8E,0x39,0xE4,0x90,0x3C,0xE9,0x96,0x44,0xF2,0xA1,0x50
.db 0x00,0xB0,0x61,0x12,0xC4,0x76,0x29,0xDC,0x90,0x44,0xF9,0xAE,0x64,0x1A,0xD1,0x88
.db 0x40,0xF8,0xB1,0x6A,0x24,0xDE,0x99,0x54,0x10,0xCC,0x89,0x46,0x04,0xC2,0x81,0x40
.db 0x00,0xC0,0x81,0x42,0x04,0xC6,0x89,0x4C,0x10,0xD4,0x99,0x5E,0x24,0xEA,0xB1,0x78
.db 0x40,0x08,0xD1,0x9A,0x64,0x2E,0xF9,0xC4,0x90,0x5C,0x29,0xF6,0xC4,0x92,0x61,0x30
.db 0x00,0xD0,0xA1,0x72,0x44,0x16,0xE9,0xBC,0x90,0x64,0x39,0x0E,0xE4,0xBA,0x91,0x68
.db 0x40,0x18,0xF1,0xCA,0xA4,0x7E,0x59,0x34,0x10,0xEC,0xC9,0xA6,0x84,0x62,0x41,0x20
.db 0x00,0xE0,0xC1,0xA2,0x84,0x66,0x49,0x2C,0x10,0xF4,0xD9,0xBE,0xA4,0x8A,0x71,0x58
.db 0x40,0x28,0x11,0xFA,0xE4,0xCE,0xB9,0xA4,0x90,0x7C,0x69,0x56,0x44,0x32,0x21,0x10
.db 0x00,0xF0,0xE1,0xD2,0xC4,0xB6,0xA9,0x9C,0x90,0x84,0x79,0x6E,0x64,0x5A,0x51,0x48
.db 0x40,0x38,0x31,0x2A,0x24,0x1E,0x19,0x14,0x10,0x0C,0x09,0x06,0x04,0x02,0x01,0x00

.db 0x40,0x40,0x41,0x41,0x42,0x42,0x43,0x43,0x44,0x44,0x45,0x45,0x46,0x46,0x47,0x47
.db 0x48,0x48,0x49,0x49,0x4A,0x4A,0x4B,0x4C,0x4C,0x4D,0x4D,0x4E,0x4E,0x4F,0x4F,0x50
.db 0x51,0x51,0x52,0x52,0x53,0x53,0x54,0x54,0x55,0x56,0x56,0x57,0x57,0x58,0x59,0x59
.db 0x5A,0x5A,0x5B,0x5C,0x5C,0x5D,0x5D,0x5E,0x5F,0x5F,0x60,0x60,0x61,0x62,0x62,0x63
.db 0x64,0x64,0x65,0x65,0x66,0x67,0x67,0x68,0x69,0x69,0x6A,0x6A,0x6B,0x6C,0x6C,0x6D
.db 0x6E,0x6E,0x6F,0x70,0x70,0x71,0x72,0x72,0x73,0x74,0x74,0x75,0x76,0x76,0x77,0x78
.db 0x79,0x79,0x7A,0x7B,0x7B,0x7C,0x7D,0x7D,0x7E,0x7F,0x7F,0x80,0x81,0x82,0x82,0x83
.db 0x84,0x84,0x85,0x86,0x87,0x87,0x88,0x89,0x8A,0x8A,0x8B,0x8C,0x8D,0x8D,0x8E,0x8F
.db 0x90,0x90,0x91,0x92,0x93,0x93,0x94,0x95,0x96,0x96,0x97,0x98,0x99,0x99,0x9A,0x9B
.db 0x9C,0x9D,0x9D,0x9E,0x9F,0xA0,0xA0,0xA1,0xA2,0xA3,0xA4,0xA4,0xA5,0xA6,0xA7,0xA8
.db 0xA9,0xA9,0xAA,0xAB,0xAC,0xAD,0xAD,0xAE,0xAF,0xB0,0xB1,0xB2,0xB2,0xB3,0xB4,0xB5
.db 0xB6,0xB7,0xB7,0xB8,0xB9,0xBA,0xBB,0xBC,0xBD,0xBD,0xBE,0xBF,0xC0,0xC1,0xC2,0xC3
.db 0xC4,0xC4,0xC5,0xC6,0xC7,0xC8,0xC9,0xCA,0xCB,0xCB,0xCC,0xCD,0xCE,0xCF,0xD0,0xD1
.db 0xD2,0xD3,0xD4,0xD4,0xD5,0xD6,0xD7,0xD8,0xD9,0xDA,0xDB,0xDC,0xDD,0xDE,0xDF,0xE0
.db 0xE1,0xE1,0xE2,0xE3,0xE4,0xE5,0xE6,0xE7,0xE8,0xE9,0xEA,0xEB,0xEC,0xED,0xEE,0xEF
.db 0xF0,0xF1,0xF2,0xF3,0xF4,0xF5,0xF6,0xF7,0xF8,0xF9,0xFA,0xFB,0xFC,0xFD,0xFE,0xFF


.db 0x00,0x00,0x01,0x02,0x04,0x06,0x09,0x0C,0x10,0x14,0x19,0x1E,0x24,0x2A,0x31,0x38
.db 0x40,0x48,0x51,0x5A,0x64,0x6E,0x79,0x84,0x90,0x9C,0xA9,0xB6,0xC4,0xD2,0xE1,0xF0
.db 0x00,0x10,0x21,0x32,0x44,0x56,0x69,0x7C,0x90,0xA4,0xB9,0xCE,0xE4,0xFA,0x11,0x28
.db 0x40,0x58,0x71,0x8A,0xA4,0xBE,0xD9,0xF4,0x10,0x2C,0x49,0x66,0x84,0xA2,0xC1,0xE0
.db 0x00,0x20,0x41,0x62,0x84,0xA6,0xC9,0xEC,0x10,0x34,0x59,0x7E,0xA4,0xCA,0xF1,0x18
.db 0x40,0x68,0x91,0xBA,0xE4,0x0E,0x39,0x64,0x90,0xBC,0xE9,0x16,0x44,0x72,0xA1,0xD0
.db 0x00,0x30,0x61,0x92,0xC4,0xF6,0x29,0x5C,0x90,0xC4,0xF9,0x2E,0x64,0x9A,0xD1,0x08
.db 0x40,0x78,0xB1,0xEA,0x24,0x5E,0x99,0xD4,0x10,0x4C,0x89,0xC6,0x04,0x42,0x81,0xC0
.db 0x00,0x40,0x81,0xC2,0x04,0x46,0x89,0xCC,0x10,0x54,0x99,0xDE,0x24,0x6A,0xB1,0xF8
.db 0x40,0x88,0xD1,0x1A,0x64,0xAE,0xF9,0x44,0x90,0xDC,0x29,0x76,0xC4,0x12,0x61,0xB0
.db 0x00,0x50,0xA1,0xF2,0x44,0x96,0xE9,0x3C,0x90,0xE4,0x39,0x8E,0xE4,0x3A,0x91,0xE8
.db 0x40,0x98,0xF1,0x4A,0xA4,0xFE,0x59,0xB4,0x10,0x6C,0xC9,0x26,0x84,0xE2,0x41,0xA0
.db 0x00,0x60,0xC1,0x22,0x84,0xE6,0x49,0xAC,0x10,0x74,0xD9,0x3E,0xA4,0x0A,0x71,0xD8
.db 0x40,0xA8,0x11,0x7A,0xE4,0x4E,0xB9,0x24,0x90,0xFC,0x69,0xD6,0x44,0xB2,0x21,0x90
.db 0x00,0x70,0xE1,0x52,0xC4,0x36,0xA9,0x1C,0x90,0x04,0x79,0xEE,0x64,0xDA,0x51,0xC8
.db 0x40,0xB8,0x31,0xAA,0x24,0x9E,0x19,0x94,0x10,0x8C,0x09,0x86,0x04,0x82,0x01,0x80

.db 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
.db 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
.db 0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x02,0x02
.db 0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03
.db 0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x05,0x05,0x05,0x05,0x05,0x05,0x05,0x06
.db 0x06,0x06,0x06,0x06,0x06,0x07,0x07,0x07,0x07,0x07,0x07,0x08,0x08,0x08,0x08,0x08
.db 0x09,0x09,0x09,0x09,0x09,0x09,0x0A,0x0A,0x0A,0x0A,0x0A,0x0B,0x0B,0x0B,0x0B,0x0C
.db 0x0C,0x0C,0x0C,0x0C,0x0D,0x0D,0x0D,0x0D,0x0E,0x0E,0x0E,0x0E,0x0F,0x0F,0x0F,0x0F
.db 0x10,0x10,0x10,0x10,0x11,0x11,0x11,0x11,0x12,0x12,0x12,0x12,0x13,0x13,0x13,0x13
.db 0x14,0x14,0x14,0x15,0x15,0x15,0x15,0x16,0x16,0x16,0x17,0x17,0x17,0x18,0x18,0x18
.db 0x19,0x19,0x19,0x19,0x1A,0x1A,0x1A,0x1B,0x1B,0x1B,0x1C,0x1C,0x1C,0x1D,0x1D,0x1D
.db 0x1E,0x1E,0x1E,0x1F,0x1F,0x1F,0x20,0x20,0x21,0x21,0x21,0x22,0x22,0x22,0x23,0x23
.db 0x24,0x24,0x24,0x25,0x25,0x25,0x26,0x26,0x27,0x27,0x27,0x28,0x28,0x29,0x29,0x29
.db 0x2A,0x2A,0x2B,0x2B,0x2B,0x2C,0x2C,0x2D,0x2D,0x2D,0x2E,0x2E,0x2F,0x2F,0x30,0x30
.db 0x31,0x31,0x31,0x32,0x32,0x33,0x33,0x34,0x34,0x35,0x35,0x35,0x36,0x36,0x37,0x37
.db 0x38,0x38,0x39,0x39,0x3A,0x3A,0x3B,0x3B,0x3C,0x3C,0x3D,0x3D,0x3E,0x3E,0x3F,0x3F

_ram_routine_begin:

.DB 0x00,0x06,0x0C,0x12,0x19,0x1F,0x25,0x2B,0x31,0x38,0x3E,0x44,0x4A,0x50,0x56,0x5C
.DB 0x61,0x67,0x6D,0x73,0x78,0x7E,0x83,0x88,0x8E,0x93,0x98,0x9D,0xA2,0xA7,0xAB,0xB0
.DB 0xB5,0xB9,0xBD,0xC1,0xC5,0xC9,0xCD,0xD1,0xD4,0xD8,0xDB,0xDE,0xE1,0xE4,0xE7,0xEA
.DB 0xEC,0xEE,0xF1,0xF3,0xF4,0xF6,0xF8,0xF9,0xFB,0xFC,0xFD,0xFE,0xFE,0xFF,0xFF,0xFF

.DB 0x00,0xFF,0xFF,0xFF,0xFE,0xFE,0xFD,0xFC,0xFB,0xF9,0xF8,0xF6,0xF4,0xF3,0xF1,0xEE
.DB 0xEC,0xEA,0xE7,0xE4,0xE1,0xDE,0xDB,0xD8,0xD4,0xD1,0xCD,0xC9,0xC5,0xC1,0xBD,0xB9
.DB 0xB5,0xB0,0xAB,0xA7,0xA2,0x9D,0x98,0x93,0x8E,0x88,0x83,0x7E,0x78,0x73,0x6D,0x67
.DB 0x61,0x5C,0x56,0x50,0x4A,0x44,0x3E,0x38,0x31,0x2B,0x25,0x1F,0x19,0x12,0x0C,0x06

.DB 0x00,0xFA,0xF4,0xEE,0xE7,0xE1,0xDB,0xD5,0xCF,0xC8,0xC2,0xBC,0xB6,0xB0,0xAA,0xA4
.DB 0x9F,0x99,0x93,0x8D,0x88,0x82,0x7D,0x78,0x72,0x6D,0x68,0x63,0x5E,0x59,0x55,0x50
.DB 0x4B,0x47,0x43,0x3F,0x3B,0x37,0x33,0x2F,0x2C,0x28,0x25,0x22,0x1F,0x1C,0x19,0x16
.DB 0x14,0x12,0x0F,0x0D,0x0C,0x0A,0x08,0x07,0x05,0x04,0x03,0x02,0x02,0x01,0x01,0x01

.DB 0x00,0x01,0x01,0x01,0x02,0x02,0x03,0x04,0x05,0x07,0x08,0x0A,0x0C,0x0D,0x0F,0x12
.DB 0x14,0x16,0x19,0x1C,0x1F,0x22,0x25,0x28,0x2C,0x2F,0x33,0x37,0x3B,0x3F,0x43,0x47
.DB 0x4B,0x50,0x55,0x59,0x5E,0x63,0x68,0x6D,0x72,0x78,0x7D,0x82,0x88,0x8D,0x93,0x99
.DB 0x9F,0xA4,0xAA,0xB0,0xB6,0xBC,0xC2,0xC8,0xCF,0xD5,0xDB,0xE1,0xE7,0xEE,0xF4,0xFA

;   .DB 0x00,0x09,0x17,0x20,0x01,0x21,0x09,0x22,0x10,0x23,0x11,0x50,0x21
;   .DB 0x52,0x15,0x53,0x00,0xF0,0xD0,0x24,0x51,0x21,0xD3,0x00,0xD0,0x28,0xD1,0x24,0x50
;   .DB 0x2F,0xD3,0x00,0xD1,0x28,0x23,0x13,0x50,0x2F,0x52,0x11,0xD3,0x00,0xD1,0x2F,0xD0
;   .DB 0x30,0x23,0x11,0x51,0x2F,0xD3,0x00,0xD0,0x2D,0xD1,0x30,0xD3,0x00,0xD1,0x2D,0x23
;   .DB 0x13,0x51,0x29,0x53,0x00,0xF1,0x23,0x11,0xD3,0x00,0x51,0x28,0xF0,0x53,0x00,0xF0
;   .DB 0x50,0x2F,0x51,0x26,0x52,0x13,0x53,0x00,0xF0,0xD0,0x30,0xD1,0x2F,0x23,0x13,0x50
;   .DB 0x2F,0x52,0x1A,0xD3,0x00,0xD1,0x30,0xD0,0x2B,0xD1,0x2F,0x23,0x11,0x50,0x21,0x52
;   .DB 0x15,0xD3,0x00,0xD1,0x2B,0xD0,0x24,0x51,0x21,0xD3,0x00,0xD0,0x28,0xD1,0x24,0x50
;   .DB 0x2F,0xD3,0x00,0xD1,0x28,0x23,0x13,0x50,0x2F,0x52,0x11,0xD3,0x00,0xD1,0x2F,0xD0
;   .DB 0x30,0x23,0x11,0x51,0x2F,0xD3,0x00,0xD0,0x32,0xD1,0x30,0xD3,0x00,0xD1,0x32,0x23
;   .DB 0x13,0x50,0x34,0x52,0x18,0x53,0x00,0xF1,0x23,0x11,0x51,0x34,0x53,0x00,0xF1,0x50
;   .DB 0x35,0xD3,0x00,0xD0,0x34,0x50,0x32,0x52,0x13,0xD3,0x00,0xD1,0x35,0xD1,0x34,0xD1
;   .DB 0x32,0x23,0x13,0x53,0x00,0xF0,0xD0,0x30,0xD0,0x32,0x23,0x11,0x50,0x21,0x52,0x15
;   .DB 0xD3,0x00,0xD1,0x30,0x50,0x24,0xD1,0x32,0x51,0x21,0xD3,0x00,0xD0,0x28,0xD1,0x24
;   .DB 0x50,0x2F,0xD3,0x00,0xD1,0x28,0x23,0x13,0x50,0x2F,0x52,0x11,0xD3,0x00,0xD1,0x2F
;   .DB 0xD0,0x30,0x23,0x11,0x51,0x2F,0xD3,0x00,0xD0,0x2D,0xD1,0x30,0xD3,0x00,0xD1,0x2D
;   .DB 0x23,0x13,0x51,0x24,0x53,0x00,0xF0,0xD1,0x26,0x23,0x11,0x51,0x28,0xD3,0x00,0x51
;   .DB 0x29,0xF0,0x53,0x00,0xF0,0x50,0x2F,0x51,0x2B,0x52,0x13,0x53,0x00,0xF0,0xD0,0x30
;   .DB 0xD1,0x2F,0x23,0x13,0x50,0x2F,0x52,0x1A,0xD3,0x00,0xD1,0x30,0xD0,0x2B,0xD1,0x2F
;   .DB 0x23,0x11,0x50,0x21,0x52,0x15,0xD3,0x00,0xD1,0x2B,0xD0,0x24,0x51,0x21,0xD3,0x00
;   .DB 0xD0,0x28,0xD1,0x24,0x50,0x2F,0xD3,0x00,0xD1,0x28,0x23,0x13,0x50,0x2F,0x52,0x11
;   .DB 0xD3,0x00,0xD1,0x2F,0xD0,0x30,0x23,0x11,0x51,0x2F,0xD3,0x00,0xD0,0x32,0xD1,0x30
;   .DB 0xD3,0x00,0xD1,0x32,0x23,0x13,0x50,0x34,0x52,0x18,0x53,0x00,0xF1,0x23,0x11,0x51
;   .DB 0x34,0x53,0x00,0xF1,0x50,0x35,0xD3,0x00,0xD0,0x34,0x50,0x32,0x52,0x13,0xD3,0x00
;   .DB 0xD1,0x35,0xD1,0x34,0xD1,0x32,0x23,0x13,0x53,0x00,0xF0,0xD0,0x30,0xD0,0x32,0x23
;   .DB 0x11,0x50,0x34,0x52,0x15,0xD3,0x00,0xD1,0x30,0xD1,0x32,0x51,0x34,0xD3,0x00,0x50
;   .DB 0x35,0x52,0x1C,0xF0,0x50,0x2D,0x52,0x15,0xD3,0x00,0xD1,0x35,0x23,0x13,0xD3,0x00
;   .DB 0xD1,0x2D,0xD0,0x28,0x23,0x11,0xD3,0x00,0xD2,0x13,0xD1,0x28,0x50,0x24,0x52,0x15
;   .DB 0x53,0x00,0xF0,0x23,0x13,0x50,0x2B,0x52,0x11,0xD3,0x00,0x51,0x24,0xF0,0x23,0x11
;   .DB 0x51,0x2B,0x53,0x00,0xF1,0x50,0x29,0x53,0x00,0xF0,0x52,0x18,0xD3,0x00,0x51,0x29
;   .DB 0xF1,0x23,0x13,0x50,0x28,0x53,0x00,0xF0,0xD0,0x29,0xD1,0x28,0x23,0x11,0x50,0x28
;   .DB 0x52,0x13,0xD3,0x00,0x51,0x29,0xF0,0x51,0x28,0x53,0x00,0xF1,0x50,0x2D,0xD3,0x00
;   .DB 0xD0,0x30,0x23,0x13,0x50,0x34,0x52,0x14,0xD3,0x00,0xD1,0x2D,0xD1,0x30,0x23,0x11
;   .DB 0x51,0x34,0xD3,0x00,0x52,0x17,0xF0,0xD3,0x00,0xB0,0x09,0x00

;----------------------------------------------------------------------------
;[[ get_keypad ]]
;----------------------------------------------------------------------------
_get_keypad:
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

   LD    HL, #key_cur
   XOR   (HL)
   LD    (HL), E
   INC   HL
   LD    (HLD), A
   
   LD    A, #0x30                      ; reset port
   LDH   (P_P1), A

   RET

deinit_title:
   LD    HL, #0xC000
   XOR   A
   LD    B, #40
fill_oam_blank:
   LD    (HLI), A
   INC   L
   INC   L
   INC   L
   DEC   B
   JR    NZ, fill_oam_blank

   LD    A, #0x01
   LDH   (P_VBK), A

   LD    HL, #0x99AD                   ; annoying palette 3 thing
   RST   ldi_v_a

   LD    HL, #0x9800                   ; VRAM attrib = 0
   LD    B, #9
   LD    A, #7
   CALL  _STIR_B_VRAM
   LD    B, #23
   LD    A, #6
   CALL  _STIR_B_VRAM
   LD    B, #9
   LD    A, #7
   CALL  _STIR_B_VRAM
   LD    B, #23
   LD    A, #6
   CALL  _STIR_B_VRAM
   LD    B, #9
   LD    A, #7
   CALL  _STIR_B_VRAM
   LD    B, #23
   LD    A, #6
   JP    _STIR_B_VRAM

deinit_title2:
   LD    A, (__gba)                    ; select gbc/gba palette
   ADD   A, #0xFF                      ;
   SBC   A, A                          ;
   AND   #0x80                         ;
   LD    B, #0x00                      ;
   LD    C, A                          ; BC = 0 or 128
   PUSH  BC

   LD    HL, #newpal+96
   ADD   HL, BC
   LD    B, H
   LD    C, L

   LD    DE, #0x0104
   LD    HL, #0xFF6A
   CALL  set_palette

   POP   BC

   LD    HL, #newpal
   ADD   HL, BC
   LD    B, H
   LD    C, L

   LD    DE, #0x0600
   LD    HL, #0xFF68
   JP    set_palette

deinit_title3:
   XOR   A
   LDH   (P_VBK), A

   LD    HL, #0x9800                   ; VRAM attrib = 0
   XOR   A
   RST   ldi_v_a
   INC   A
   LD    B, #7
   CALL  _STIR_B_VRAM
   INC   A
   RST   ldi_v_a
   LD    A, #4
   LD    B, #23
   CALL  _STIR_B_VRAM

   LD    A, #3
   RST   ldi_v_a

   LD    A, #0x80
   RST   ldi_v_a
   INC   A
   RST   ldi_v_a
   INC   A
   RST   ldi_v_a
   INC   A
   RST   ldi_v_a
   INC   A
   RST   ldi_v_a
   INC   A
   RST   ldi_v_a
   INC   A
   RST   ldi_v_a

   LD    A, #5
   RST   ldi_v_a
   LD    A, #4
   LD    B, #23
   CALL  _STIR_B_VRAM

   LD    A, #6
   RST   ldi_v_a
   INC   A
   LD    B, #7
   CALL  _STIR_B_VRAM
   INC   A
   RST   ldi_v_a
   LD    A, #4
   LD    B, #23
   JP    _STIR_B_VRAM

dec_title_y:
   LD    HL, #dec_yes
   DEC   (HL)

   LD    A, (HL)
   CP    #5
   JR    NZ, keepgoing
   LD    A, #0x00
   LD    (_ram_routine_call + timermodify + 1), A

keepgoing:
   LD    BC, #0xFC1C

   LD    HL, #title_y
   LD    A, (HL)
   ADD   A, B
   LD    (HL), A
   LD    HL, #0xC030
dec_loop:
   LD    A, (HL)
   ADD   A, B
   LD    (HLI), A
   INC   L
   INC   L
   INC   L
   DEC   C
   JR    NZ, dec_loop
   RET

scroll_clouds:
   LD    HL, #0xC000
   LD    C, #12

cl_dec_loop:
   INC   L
   DEC   (HL)
   INC   L
   INC   L
   INC   L
   DEC   C
   JR    NZ, cl_dec_loop
   RET

wait4line:
   LDH   A, (P_LY)
   CP    B
   JR    NZ, wait4line
   RET

wait4line_pal:
   LDH   A, (P_LY)
   CP    B
   JR    NZ, wait4line_pal

   LD    A, #0x80
   LDH   (P_BCPS), A
   LD    (HL), E
   LD    (HL), D
   ADD   A, C
   LDH   (P_BCPS), A
   LD    (HL), E
   LD    (HL), D
   ADD   A, C
   LDH   (P_BCPS), A
   LD    (HL), E
   LD    (HL), D

   RET

main_title_loop:
   PUSH  HL
   PUSH  DE
   PUSH  BC

   CALL  _ram_routine_call + _tpfs_step

;   CALL  _SOUNDLIB_Update

   POP   BC
   POP   DE
   POP   HL

_start_title_loop:
   LDH   A, (P_LY)
   CP    #144
   JR    NZ, _start_title_loop

   LD    A, #0b10000111 ;#0b10101111
   LDH   (P_LCDC), A
   LD    A, #140
   LDH   (P_SCY), A
   LD    A, #0xFD
   LDH   (P_SCX), A

titleloop:
   LD    A, #0xC0
   CALL  0xFF80

   LD    HL, #title_timer
   INC   (HL)

   LD    HL, #0xFF69
   LD    BC, #0x0008
   LD    D, B
   LD    E, B
   CALL  _ram_routine_call + wait4line_pal

   LD    A, (_pal_cur2)
   LD    E, A
   LD    A, (_pal_cur2+1)
   LD    D, A
   LD    B, #0x1E

   LD    A, (title_y)
   SUB   B
   CPL
   INC   A
   LDH   (P_SCY), A

   LD    A, (dec_yes)
   DEC   A
   CP    #3
   PUSH  AF
   CALL  Z, deinit_title
   POP   AF
   CP    #2
   PUSH  AF
   CALL  Z, deinit_title2
   POP   AF
   CP    #1
   PUSH  AF
   CALL  Z, deinit_title3
   POP   AF

   LD    BC, #0x1C08

   CP    #5
   JR    C, specialcase

   CALL  _ram_routine_call + wait4line_pal

   LD    B, #0x4C
specialcase:
   LD    C, #0x00

   CALL  _ram_routine_call + wait4line
   LD    DE, #0x201B    ; adder / count

   LD    HL, #scrx

   LD    A, (HL)
   ADD   A, D
   LD    (HLI), A
   PUSH  AF
   LD    A, #104
   LDH   (P_SCY), A
   LD    A, #0b10001111 ;#0b10101111
   LDH   (P_LCDC), A
   LD    A, (HL)
   LDH   (P_SCX), A
   ADC   A, C
   LD    (HLI), A
   POP   AF

   PUSH  HL
   CALL  C, _ram_routine_call + scroll_clouds
   POP   HL

   LD    A, D
   ADD   A, #16
   LD    D, A
   ADC   A, C
   SUB   D
   LD    C, A

   PUSH  BC
   PUSH  DE
   PUSH  HL

   CALL  _SOUNDLIB_Update

   POP   HL
   POP   DE
   POP   BC

   LD    B, #88

;   XOR   A
;   LD    (scrx+15), A

_oceananim:
   CALL  _ram_routine_call + wait4line

   LD    A, (HL)
   ADD   A, D
   LD    (HLI), A
   LD    A, (HL)
   LDH   (P_SCX), A
   ADC   A, C
   LD    (HLI), A
   LD    A, D
   ADD   A, #16
   LD    D, A
   ADC   A, C
   SUB   D
   LD    C, A
   INC   B
   DEC   E
   JR    NZ, _oceananim

   CALL  _ram_routine_call + wait4line

   LD    A, (dec_yes)
   OR    A
   JR    Z, nofix
   CP    #5
   JR    C, fix_x
nofix:
   LD    A, (title_timer)
timermodify:
   AND   #0x20
   SUB   #0x01
   SBC   A, A
   AND   #120
   PUSH  AF
   INC   B
   CALL  _ram_routine_call + wait4line
   POP   AF

   LDH   (P_SCX), A
   LD    A, #140
   LDH   (P_SCY), A
   LD    A, #0b10000111 ;#0b10100111
   LDH   (P_LCDC), A

   LD    HL, #fade_count
   LD    A, (HL)
   CP    #240
   JR    NC, join
   INC   (HL)
   PUSH  HL
   PUSH  DE
   PUSH  BC
   PUSH  AF
   CALL  _ram_routine_call + _tpfs_step
   POP   AF
   POP   BC
   POP   DE
   POP   HL
   JR    join
fix_x:
   PUSH  AF
   LD    A, (scrx+53)
   LDH   (P_SCX), A
   POP   AF
   PUSH  AF
   POP   AF
   PUSH  AF
   POP   AF
   PUSH  AF
   POP   AF
   PUSH  AF
   POP   AF
   PUSH  AF
   POP   AF
   PUSH  AF
   POP   AF

   PUSH  AF
   LD    A, #72
   LDH   (P_SCX), A
   LD    A, #0b10000111 ;#0b10100111
   LDH   (P_LCDC), A
   POP   AF

   CP    #1
   RET   Z

join:
   LD    B, #144
   CALL  _ram_routine_call + wait4line
   LD    A, #0xFD
   LDH   (P_SCX), A

;   LD    A, (0x0100)
;   OR    A
;   CALL  NZ, _ram_routine_call + rom_is_out

   CALL  _ram_routine_call + _get_keypad

   LD    HL, #dec_yes
   LD    A, (HL)
   OR    A
   JR    NZ, raisetitle

   LD    A, (key_cur)
   AND   #0x80
   JR    Z, dont_raise_title

   PUSH  BC
   PUSH  DE
   PUSH  HL
   LD    BC, #0x0004                   ; SFX_BUY
   PUSH  BC
   CALL  _SOUNDLIB_sfx
   POP   BC                            ; FIXME: consider modifying SP directly
   POP   HL
   POP   DE
   POP   BC

   LD    A, #0x04
   LD    (_ram_routine_call + timermodify + 1), A

   LD    (HL), #41
raisetitle:
   CALL  _ram_routine_call + dec_title_y

dont_raise_title:
   JP    _ram_routine_call + titleloop

rom_is_out:
   LD    A, #0xD0
   CALL  0xFF80

   LD    A, #0b10001111
   LDH   (P_LCDC), A

rom_out_loop:
   LD    A, (0x0100)
   OR    A
   JR    NZ, rom_out_loop

   LD    A, #0b10000111
   LDH   (P_LCDC), A

   RET

Bresenham:
   LD    A, (HLI)
   ADD   A, (HL)
   JR    NC, bh_noadd
   SUB   B
   LD    (HLI), A
   LD    A, (HLI)
   ADD   A, (HL)
   LD    (HLI), A
   RET
bh_noadd:
   LD    (HLI), A
   INC   HL
   LD    A, (HLI)
   RET



;;----------------------------------------------------------------------------
;;[[ SOUNDLIB_Update ]]
;;----------------------------------------------------------------------------
;_SOUNDLIB_Update:
;   LD    A, (playing)                  ; Playing?
;   OR    A
;   RET   Z
;
;   LD    HL, #currentbeat              ; Check if updating this time
;   DEC   (HL)
;   RET   NZ
;
;   LD    HL, #waitloop
;   LD    A, (HL)
;   OR    A
;   JR    Z, go_update
;   DEC   (HL)
;
;   LD    A, (tempo)
;   LD    (currentbeat), A
;
;   RET
;
;go_update:
;;   LDH   A, (_rompage)
;;   PUSH  AF
;   PUSH  BC
;
;;   LD    A, (song_page)
;;   LDH   (_rompage), A
;;   LD    (0x2000), A
;
;   LD    HL, #position
;   LD    A, (HLI)
;   LD    H, (HL)
;   LD    L, A
;
;   LD    BC, #_ram_routine_call
;   ADD   HL, BC
;
;command_loop:
;
;   LD    A, (HLI)
;   LD    B, A
;
;   PUSH  HL
;
;   AND   #0x70
;   SWAP  A
;   ADD   A, A
;   LD    HL, #_ram_routine_call + command_vector
;   ADD   A, L
;   LD    L, A
;   ADC   A, H
;   SUB   L
;   LD    H, A
;   LD    A, (HLI)
;   LD    H, (HL)
;   LD    L, A
;   JP    (HL)
;
;call_de:
;   PUSH  DE
;   RET
;
;sound_tempo:
;   POP   HL
;   LD    A, (HLI)
;   OR    A
;   JR    Z, sound_stop
;   LD    (tempo), A
;   JR    command_finish
;
;sound_stop:
;   LD    (playing), A
;   JR    command_finish
;
;sound_volume:
;   LD    A, B
;   AND   #0x07
;   LD    B, A
;   SWAP  A
;   OR    B
;   LD    (volume), A
;   LDH   (0x24), A
;   JR    command_finish2
;
;sound_play:
;   LD    A, B
;   AND   #0x03
;   LD    D, #0x00
;   LD    E, A
;   LD    HL, #instrument
;   ADD   HL, DE
;   LD    C, (HL)
;   LD    HL, #_ram_routine_call + sound_play_table
;   ADD   HL, DE
;   ADD   HL, DE
;   LD    E, (HL)
;   INC   HL
;   LD    D, (HL)
;   POP   HL
;   LD    A, (HLI)
;   PUSH  HL
;   PUSH  BC
;   ADD   A, A
;   LD    HL, #_ram_routine_call + notes_table
;   ADD   A, L
;   LD    L, A
;   ADC   A, H
;   SUB   L
;   LD    H, A                          ; HL -> frequency data
;   LD    A, C
;
;   CALL  _ram_routine_call + call_de
;   POP   BC
;command_finish2:
;   POP   HL
;command_finish:
;
;   BIT   7, B
;   JR    Z, command_loop
;
;   LD    BC, #-_ram_routine_call
;   ADD   HL, BC
;
;   LD    D, H
;   LD    A, L
;   LD    HL, #position
;   LD    (HLI), A
;   LD    (HL), D
;
;   POP   BC
;
;   LD    A, (tempo)
;   LD    (currentbeat), A
;
;;   POP   AF
;;   LDH   (_rompage), A
;;   LD    (0x2000), A
;
;   RET
;
;sound_nop:
;   POP   HL
;   LD    A, B
;   AND   #0x0F
;   INC   A
;   LD    (waitloop), A
;   JR    command_finish
;
;sound_instrument:
;   LD    A, B
;   AND   #0x03
;   LD    DE, #instrument
;   ADD   A, E
;   LD    E, A
;   ADC   A, D
;   SUB   E
;   LD    D, A
;   POP   HL
;   LD    A, (HLI)
;   LD    (DE), A
;   JR    command_finish
;
;sound_jump:
;   POP   HL
;   LD    A, (HLI)
;   LD    D, (HL)
;   LD    E, A
;   LD    HL, #song_ptr
;   LD    A, (HLI)
;   LD    H, (HL)
;   LD    L, A
;   ADD   HL, DE
;   LD    DE, #_ram_routine_call
;   ADD   HL, DE
;   JR    command_finish
;
;sound_off:
;   LD    A, B
;   AND   #0x03
;   LD    D, #0x00
;   LD    E, A
;   LD    HL, #_ram_routine_call + sound_off_table
;   ADD   HL, DE
;   LD    C, (HL)
;   XOR   A
;   LDH   (C), A
;   JR    command_finish2
;
;sound_on:
;   LD    A, B
;   AND   #0x03
;   LD    D, #0x00
;   LD    E, A
;   LD    HL, #_ram_routine_call + sound_on_table
;   ADD   HL, DE
;   LD    L, (HL)
;   LD    H, #0xFF
;   SET   7, (HL)
;   JR    command_finish2
;
;command_vector:
;   .DW   _ram_routine_call + sound_tempo, _ram_routine_call + sound_volume
;   .DW   _ram_routine_call + sound_instrument, _ram_routine_call + sound_jump
;   .DW   _ram_routine_call + sound_off, _ram_routine_call + sound_play
;   .DW   _ram_routine_call + sound_on, _ram_routine_call + sound_nop
;
;sound_off_table:
;   .DB   0x12,0x17,0x1A,0x21
;
;sound_play_table:
;   .DW   _ram_routine_call + playch1, _ram_routine_call + playch2
;   .DW   _ram_routine_call + playch3, _ram_routine_call + playch4
;
;sound_on_table:
;   .DB   0x14,0x19,0x1E,0x23
;
;;----------------------------------------------------------------------------
;
;getinstrumentdata_E:
;   LD    A, E
;
;; A = instrument number
;; HL -> instrument data
;
;getinstrumentdata:
;   RLCA
;   RLCA
;   LD    E, A
;   AND   #0x03
;   LD    D, A
;   XOR   E
;   LD    E, A
;   LD    HL, #_ram_routine_call + sound_data
;   ADD   HL, DE                        ; HL -> instrument data
;
;   RET
;
;; mode1:
;;  AACCCCCC, DDDDEFFF, BGGGHIII, xxxxxxxx
;;   A = wave pattern duty, B = use/ignore length, C = length
;;   D = initial volume, E = volume slide up/down, F = volume slide number
;;   G = sweep time, H = sweep down/up, I = sweep number
;; A = instrument number
;; HL -> frequency data
;playch1:
;   LD    E, A
;
;   LD    A, (HLI)                      ; get low 8 bits of frequency
;   LDH   (0x13), A
;   LD    C, (HL)                       ; get high 3 bits of frequency
;
;   CALL  _ram_routine_call + getinstrumentdata_E
;
;   LD    A, (HLI)                      ; get wave pattern duty/length data
;   LDH   (0x11), A
;   LD    A, (HLI)                      ; get volume/volume slide data
;   LDH   (0x12), A
;   LD    A, (HLI)                      ; get sweep data
;   LD    E, A
;   AND   #0x7F
;   LDH   (0x10), A
;   XOR   E                             ; get length use/ignore
;   RRCA
;
;   OR    C
;   LDH   (0x14), A                     ; start note
;
;   RET
;
;; mode2:
;;  AACCCCCC, DDDDEFFF, xBxxxxxx, xxxxxxxx
;;   A = wave pattern duty, B = use/ignore length, C = length
;;   D = initial volume, E = volume slide up/down, F = volume slide number
;; A = instrument number
;; HL -> frequency data
;playch2:
;   LD    E, A
;
;   LD    A, (HLI)                      ; get low 8 bits of frequency
;   LDH   (0x18), A
;   LD    C, (HL)                       ; get high 3 bits of frequency
;
;   CALL  _ram_routine_call + getinstrumentdata_E
;
;   LD    A, (HLI)                      ; get wave pattern duty/length data
;   LDH   (0x16), A
;   LD    A, (HLI)                      ; get volume/volume slide data
;   LDH   (0x17), A
;
;   LD    A, (HLI)                      ; get length use/ignore
;   AND   #0x80
;   RRCA
;   OR    C
;   LDH   (0x19), A
;
;   RET
;
;; mode3:
;;  CCCCCCCC, xDDxxxxx, BAAAAAAA, xxxxxxxx
;;   A = wave pattern number, B = use/ignore length, C = length
;;   D = volume
;; A = instrument number
;; HL -> frequency data
;playch3:
;   LD    E, A
;
;   LD    A, (HLI)                      ; get low 8 bits of frequency
;   LDH   (0x1D), A
;   LD    C, (HL)                       ; get high 3 bits of frequency
;
;   CALL  _ram_routine_call + getinstrumentdata_E
;
;   LD    A, (HLI)                      ; get wave length data
;   LDH   (0x1B), A   
;   LD    A, (HLI)                      ; get volume data
;   AND   #0x60
;   LDH   (0x1C), A
;
;   LD    A, (HL)
;
;   PUSH  HL
;   SWAP  A
;   LD    E, A
;   AND   #0x0F
;   LD    D, A
;   XOR   E
;   LD    E, A
;   LD    HL, #_ram_routine_call + wavedatas
;   ADD   HL, DE
;
;   LD    A, (HLI)
;   LDH   (0x30), A
;   LD    A, (HLI)
;   LDH   (0x31), A
;   LD    A, (HLI)
;   LDH   (0x32), A
;   LD    A, (HLI)
;   LDH   (0x33), A
;   LD    A, (HLI)
;   LDH   (0x34), A
;   LD    A, (HLI)
;   LDH   (0x35), A
;   LD    A, (HLI)
;   LDH   (0x36), A
;   LD    A, (HLI)
;   LDH   (0x37), A
;   LD    A, (HLI)
;   LDH   (0x38), A
;   LD    A, (HLI)
;   LDH   (0x39), A
;   LD    A, (HLI)
;   LDH   (0x3A), A
;   LD    A, (HLI)
;   LDH   (0x3B), A
;   LD    A, (HLI)
;   LDH   (0x3C), A
;   LD    A, (HLI)
;   LDH   (0x3D), A
;   LD    A, (HLI)
;   LDH   (0x3E), A
;   LD    A, (HLI)
;   LDH   (0x3F), A
;   POP   HL
;
;   LD    A, #0x80                      ; sound on
;   LDH   (0x1A), A
;
;   AND   (HL)
;   RRCA
;   OR    C
;   LDH   (0x1E), A
;
;   RET
;
;; mode4:
;;  xBCCCCCC, DDDDEFFF, AAAAHGGG, xxxxxxxx
;;   A = shift clock frequency, B = use/ignore length, C = length
;;   D = initial volume, E = volume slide up/down, F = volume slide number
;;   G = dividing ratio of frequency, H = polynomial counter step = 7/15
;; A = instrument number
;playch4:
;   CALL  _ram_routine_call + getinstrumentdata
;
;   LD    A, (HLI)                      ; get wave length data
;   LD    D, A
;   AND   #0x3F
;   LDH   (0x20), A
;   XOR   D
;   LD    C, A
;   LD    A, (HLI)                      ; get volume/volume slide data
;   LDH   (0x21), A
;
;   LD    A, (HLI)                      ; get shift clock, polynomial counter
;   LDH   (0x22), A
;
;   LD    A, #0x80
;   OR    C
;   LDH   (0x23), A
;
;   RET
;
;; frequencies for each note
;notes_table:
;   .DW     44+0x8000   ; C-3 =   65.41 Hz
;   .DW    157+0x8000   ; C#3 =   69.30 Hz
;   .DW    263+0x8000   ; D-3 =   73.42 Hz
;   .DW    363+0x8000   ; D#3 =   77.78 Hz
;   .DW    457+0x8000   ; E-3 =   82.41 Hz
;   .DW    547+0x8000   ; F-3 =   87.31 Hz
;   .DW    631+0x8000   ; F#3 =   92.50 Hz
;   .DW    711+0x8000   ; G-3 =   98.00 Hz
;   .DW    786+0x8000   ; G#3 =  103.83 Hz
;   .DW    856+0x8000   ; A-3 =  110.00 Hz
;   .DW    923+0x8000   ; A#3 =  116.54 Hz
;   .DW    986+0x8000   ; B-3 =  123.47 Hz
;
;   .DW   1046+0x8000   ; C-4 =  130.81 Hz
;   .DW   1102+0x8000   ; C#4 =  138.59 Hz
;   .DW   1155+0x8000   ; D-4 =  146.83 Hz
;   .DW   1205+0x8000   ; D#4 =  155.56 Hz
;   .DW   1253+0x8000   ; E-4 =  164.81 Hz
;   .DW   1297+0x8000   ; F-4 =  174.61 Hz
;   .DW   1339+0x8000   ; F#4 =  185.00 Hz
;   .DW   1379+0x8000   ; G-4 =  196.00 Hz
;   .DW   1417+0x8000   ; G#4 =  207.65 Hz
;   .DW   1452+0x8000   ; A-4 =  220.00 Hz
;   .DW   1486+0x8000   ; A#4 =  233.08 Hz
;   .DW   1517+0x8000   ; B-4 =  246.94 Hz
;
;   .DW   1547+0x8000   ; C-5 =  261.63 Hz
;   .DW   1575+0x8000   ; C#5 =  277.18 Hz
;   .DW   1602+0x8000   ; D-5 =  293.66 Hz
;   .DW   1627+0x8000   ; D#5 =  311.13 Hz
;   .DW   1650+0x8000   ; E-5 =  329.63 Hz
;   .DW   1673+0x8000   ; F-5 =  349.23 Hz
;   .DW   1694+0x8000   ; F#5 =  369.99 Hz
;   .DW   1714+0x8000   ; G-5 =  392.00 Hz
;   .DW   1732+0x8000   ; G#5 =  415.30 Hz
;   .DW   1750+0x8000   ; A-5 =  440.00 Hz
;   .DW   1767+0x8000   ; A#5 =  466.16 Hz
;   .DW   1783+0x8000   ; B-5 =  493.88 Hz
;
;   .DW   1798+0x8000   ; C-6 =  523.25 Hz
;   .DW   1812+0x8000   ; C#6 =  554.37 Hz
;   .DW   1825+0x8000   ; D-6 =  587.33 Hz
;   .DW   1837+0x8000   ; D#6 =  622.25 Hz
;   .DW   1849+0x8000   ; E-6 =  659.26 Hz
;   .DW   1860+0x8000   ; F-6 =  698.46 Hz
;   .DW   1871+0x8000   ; F#6 =  739.99 Hz
;   .DW   1881+0x8000   ; G-6 =  783.99 Hz
;   .DW   1890+0x8000   ; G#6 =  830.61 Hz
;   .DW   1899+0x8000   ; A-6 =  880.00 Hz
;   .DW   1907+0x8000   ; A#6 =  932.33 Hz
;   .DW   1915+0x8000   ; B-6 =  987.77 Hz
;
;   .DW   1923+0x8000   ; C-7 = 1046.50 Hz
;   .DW   1930+0x8000   ; C#7 = 1108.73 Hz
;   .DW   1936+0x8000   ; D-7 = 1174.66 Hz
;   .DW   1943+0x8000   ; D#7 = 1244.51 Hz
;   .DW   1949+0x8000   ; E-7 = 1318.51 Hz
;   .DW   1954+0x8000   ; F-7 = 1396.91 Hz
;   .DW   1959+0x8000   ; F#7 = 1479.98 Hz
;   .DW   1964+0x8000   ; G-7 = 1567.98 Hz
;   .DW   1969+0x8000   ; G#7 = 1661.22 Hz
;   .DW   1974+0x8000   ; A-7 = 1760.00 Hz
;   .DW   1978+0x8000   ; A#7 = 1864.66 Hz
;   .DW   1982+0x8000   ; B-7 = 1975.53 Hz
;
;   .DW   1985+0x8000   ; C-8 = 2093.00 Hz
;   .DW   1989+0x8000   ; C#8 = 2217.46 Hz
;   .DW   1992+0x8000   ; D-8 = 2349.32 Hz
;   .DW   1995+0x8000   ; D#8 = 2489.02 Hz
;   .DW   1998+0x8000   ; E-8 = 2637.02 Hz
;   .DW   2001+0x8000   ; F-8 = 2793.83 Hz
;   .DW   2004+0x8000   ; F#8 = 2959.96 Hz
;   .DW   2006+0x8000   ; G-8 = 3135.96 Hz
;   .DW   2009+0x8000   ; G#8 = 3322.44 Hz
;   .DW   2011+0x8000   ; A-8 = 3520.00 Hz
;   .DW   2013+0x8000   ; A#8 = 3729.31 Hz
;   .DW   2015+0x8000   ; B-8 = 3951.07 Hz
;
;; mode1:
;;  AACCCCCC, DDDDEFFF, BGGGHIII, xxxxxxxx
;;   A = wave pattern duty, B = use/ignore length, C = length
;;   D = initial volume, E = volume slide up/down, F = volume slide number
;;   G = sweep time, H = sweep down/up, I = sweep number
;; mode2:
;;  AACCCCCC, DDDDEFFF, Bxxxxxxx, xxxxxxxx
;;   A = wave pattern duty, B = use/ignore length, C = length
;;   D = initial volume, E = volume slide up/down, F = volume slide number
;; mode3:
;;  CCCCCCCC, xDDxxxxx, BAAAAAAA, xxxxxxxx
;;   A = wave pattern number, B = use/ignore length, C = length
;;   D = volume
;; mode4:
;;  xBCCCCCC, DDDDEFFF, AAAAHGGG, xxxxxxxx
;;   A = shift clock frequency, B = use/ignore length, C = length
;;   D = initial volume, E = volume slide up/down, F = volume slide number
;;   G = dividing ratio of frequency, H = polynomial counter step = 7/15
;
;sound_data:
;.DB 0x00,0xF7,0x00,0x00 ; 00 [1/2] Loud/Long  12.5%
;.DB 0x40,0xF7,0x00,0x00 ; 01 [1/2] Loud/Long  25.0%
;.DB 0x80,0xF7,0x00,0x00 ; 02 [1/2] Loud/Long  50.0%
;.DB 0xC0,0xF7,0x00,0x00 ; 03 [1/2] Loud/Long  75.0%
;.DB 0x00,0xF1,0x00,0x00 ; 04 [1/2] Loud/Short 12.5%
;.DB 0x40,0xF1,0x00,0x00 ; 05 [1/2] Loud/Short 25.0%
;.DB 0x80,0xF1,0x00,0x00 ; 06 [1/2] Loud/Short 50.0%
;.DB 0xC0,0xF1,0x00,0x00 ; 07 [1/2] Loud/Short 75.0%
;.DB 0x00,0x77,0x00,0x00 ; 08 [1/2] Soft/Long  12.5%
;.DB 0x40,0x77,0x00,0x00 ; 09 [1/2] Soft/Long  25.0%
;.DB 0x80,0x77,0x00,0x00 ; 10 [1/2] Soft/Long  50.0%
;.DB 0xC0,0x77,0x00,0x00 ; 11 [1/2] Soft/Long  75.0%
;.DB 0x00,0x71,0x00,0x00 ; 12 [1/2] Soft/Short 12.5%
;.DB 0x40,0x71,0x00,0x00 ; 13 [1/2] Soft/Short 25.0%
;.DB 0x80,0x71,0x00,0x00 ; 14 [1/2] Soft/Short 50.0%
;.DB 0xC0,0x71,0x00,0x00 ; 15 [1/2] Soft/Short 75.0%
;
;.DB 0x00,0x20,0x00,0x00 ; 16 [ 3 ] Soft  Square Wave
;
;.DB 0x00,0xB2,0x22,0x00 ; 17 [ 4 ] Long  Hihat
;.DB 0x00,0xF7,0x80,0x00 ; 18 [ 4 ] Long  Explosion
;.DB 0x00,0xB1,0x24,0x00 ; 19 [ 4 ] Short Drum1
;.DB 0x00,0xF2,0x31,0x00 ; 20 [ 4 ] Long  Splash
;.DB 0x00,0xF1,0x31,0x00 ; 21 [ 4 ] Ride Cymbal 2
;.DB 0x00,0xF1,0x89,0x00 ; 22 [ 4 ] Exploding Drum
;.DB 0x00,0xF1,0x09,0x00 ; 23 [ 4 ] Short Triangle
;.DB 0x00,0xF5,0x08,0x00 ; 24 [ 4 ] Long Triangle
;.DB 0x00,0xF1,0x72,0x00 ; 25 [ 4 ] Drum2
;.DB 0x00,0xF1,0x6F,0x00 ; 26 [ 4 ] Drum3
;.DB 0x00,0xF1,0x07,0x00 ; 27 [ 4 ] Clap
;.DB 0x00,0xF1,0x14,0x00 ; 28 [ 4 ] Snare
;.DB 0x00,0xF3,0x25,0x00 ; 29 [ 4 ] Open Hihat
;.DB 0x00,0xF5,0x27,0x00 ; 30 [ 4 ] Crash Cymbal
;.DB 0x00,0xF4,0x17,0x00 ; 31 [ 4 ] Cymbal 2
;.DB 0x00,0xF4,0x14,0x00 ; 32 [ 4 ] Cymbal 3
;.DB 0x00,0xF7,0x07,0x00 ; 33 [ 4 ] Ocean Splash
;.DB 0x00,0xF7,0x16,0x00 ; 34 [ 4 ] Ocean Splash 2
;.DB 0x00,0xF7,0x13,0x00 ; 35 [ 4 ] Ocean Splash 3
;
;.DB 0x00,0xF7,0x24,0x00 ; 36 [ 1 ] cursor sound F#5
;.DB 0x00,0xF7,0x74,0x00 ; 37 [ 1 ] bulip
;.DB 0x00,0xF7,0x75,0x00 ; 38 [ 1 ] treasure
;.DB 0x00,0xF7,0x76,0x00 ; 39 [ 1 ] heal
;.DB 0x00,0xF7,0x77,0x00 ; 40 [ 1 ] super heal
;.DB 0x80,0xF7,0x1D,0x00 ; 41 [ 1 ] boom
;
;wavedatas:
;.DB 0x06,0x9C,0xFE,0xB8,0x56,0x79,0xAB,0xA9,0x79,0xAB,0xCA,0x87,0x69,0xBD,0xEB,0x86
;.DB 0xBD,0xDB,0x98,0x9C,0xEF,0xEB,0x76,0x68,0xAA,0x94,0x21,0x49,0xFD,0xB9,0x35,0x8B
;
;; 100 beats per minute
;; 100 beats per 60 seconds
;
;; 36 vblanks per beat
;; 1 beat every 36/60 seconds
;

;----------------------------------------------------------------------------
;[[ tpfs_step() ]]
;----------------------------------------------------------------------------
; Run through 1 iteration of fade.
;----------------------------------------------------------------------------
_tpfs_step::
   LD    A, (_pfs_fadelen)
   LD    B, A

   LD    HL, #_pfs_bresen
   LD    DE, #_pal_cur2

   CALL  _ram_routine_call + _make_pal_step_b

   LD    HL, #_pfs_bresen+144
   LD    DE, #_pal_cur2+24

   LD    A, #12
make_pal_step_c:
   PUSH  AF
   CALL  _ram_routine_call + _make_pal_step_b
   POP   AF
   DEC   A
   JR    NZ, make_pal_step_c

   LD    HL, #_pfs_bresen+528
   LD    DE, #_pal_cur2+88

   LD    A, #4
make_pal_step_d:
   PUSH  AF
   CALL  _ram_routine_call + _make_pal_step_b
   POP   AF
   DEC   A
   JR    NZ, make_pal_step_d

   LD    DE, #0x0303

;   NOP
;   NOP
;   NOP
;   NOP
;   LD    B, B

   LD    HL, #0xFF68
   LD    BC, #_pal_cur2+24
   CALL  _ram_routine_call + set_palette_b

   LD    DE, #0x0103
   LD    HL, #0xFF6A
   LD    BC, #_pal_cur2+88
   CALL  _ram_routine_call + set_palette_b

   RET

_make_pal_step_b:
   PUSH  DE
   CALL  _ram_routine_call + Bresenham
   LD    E, A
   CALL  _ram_routine_call + Bresenham
   RRCA
   RRCA
   RRCA
   LD    D, A
   AND   #3
   LD    C, A
   XOR   D
   OR    E
   POP   DE
   LD    (DE), A
   INC   DE
   CALL  _ram_routine_call + Bresenham
   RLCA
   RLCA
   OR    C
   LD    (DE), A
   INC   DE
   RET

;----------------------------------------------------------------------------
;[[ tpfs_palette() ]]
;----------------------------------------------------------------------------
;parms:
; D = number of palettes
; E = first palette
; HL -> Palette pointer (0xFF68 or 0xFF6A)
; BC -> rgb_data
;value:
; none
;----------------------------------------------------------------------------
set_palette_b:
   LD    A, D                          ; A = nb_palettes
   ADD   A, A                          ; A *= 8
   ADD   A, A
   ADD   A, A
   LD    D, A

   LD    A, E                          ; E = first_palette
   ADD   A, A                          ; A *= 8
   ADD   A, A
   ADD   A, A
   LD    E, A                          ; E = first pointer data

pal_loop_p:
   LDH   A, (P_STAT)
	AND	#0x02
   JR    NZ, pal_loop_p

   LD    A, E
   LD    (HLI), A
   LD    A, (BC)
   LD    (HLD), A
   INC   BC                            ; next rgb_data
   INC   E                             ; next pointer
   DEC   D                             ; count down palettes
   JR    NZ, pal_loop_p

   RET

_ram_routine_end:

; init
_title_init_real::
;   LD    A, #_rom_page_title
;   LD    (song_page), A

   LD    A, #1
   LDH   (P_VBK), A

   XOR   A
   LD    HL, #0x9800
   LD    BC, #0x0800
   CALL  _STIR_VRAM

   LDH   (P_VBK), A

   LD    HL, #0x9800
   LD    BC, #0x0800
   CALL  _STIR_VRAM

   LD    HL, #0xC000
   LD    B, #0xA0
   CALL  _STIR_B

   LD    A, #72
   LDH   (_rscx), A

   LD    A, #0xC0
   CALL  0xFF80

   LD    A, #_ram_page_title
   LDH   (_rampage), A
   LDH   (P_SVBK), A

   LD    A, #0x1E
   LDH   (P_SCY), A
   LD    A, #0xFD
   LDH   (P_SCX), A

   LD    DE, #wave_data
   LD    HL, #0x9400
   LD    BC, #576
   CALL  _LDIR_VRAM_ACC

;   LD    HL, #0x9400
;   LD    DE, #0x100
;randompix:
;   LD    A, (DE)
;   ADD   A, C
;   LD    C, A
;   CALL  ld_vram_a_acc
;   INC   DE
;   LD    A, H
;   CP    #0x98
;   JR    NZ, randompix

   LD    DE, #Title_tiles
   LD    HL, #0x8800
   LD    BC, #2048
   CALL  _LDIR_VRAM_ACC

   LD    DE, #Background_tiles
   LD    HL, #0x8100
   LD    BC, #576
   CALL  _LDIR_VRAM_ACC

;   LD    DE, #Window_tiles
;   LD    HL, #0x9000
;   LD    BC, #144
;   CALL  _LDIR_VRAM_ACC

   LD    DE, #Background_tiles
   LD    HL, #0x9100
   LD    BC, #576
   CALL  _LDIR_VRAM_ACC

   LD    DE, #Footer_tiles
   LD    BC, #176
   CALL  _LDIR_VRAM_ACC

   LD    HL, #0x9C00                   ; VRAM attrib = 0

   LD    DE, #0x0303
blah:
   PUSH  DE

   XOR   A
   RST   ldi_v_a
   INC   A
   LD    B, #17
   CALL  _STIR_B_VRAM
   INC   A
   RST   ldi_v_a
   LD    A, #4
   LD    B, #13
   CALL  _STIR_B_VRAM

blah2:
   LD    A, #3
   RST   ldi_v_a
   INC   A
   LD    B, #17
   CALL  _STIR_B_VRAM
   INC   A
   RST   ldi_v_a
   LD    A, #4
   LD    B, #13
   CALL  _STIR_B_VRAM
   DEC   D
   JR    NZ, blah2

   LD    A, #6
   RST   ldi_v_a
   INC   A
   LD    B, #17
   CALL  _STIR_B_VRAM
   INC   A
   RST   ldi_v_a
   LD    A, #4
   LD    B, #13
   CALL  _STIR_B_VRAM

   POP   DE
   DEC   E
   JR    NZ, blah

;   LD    BC, #fadeoutpal
;   LD    DE, #0x0800
;   LD    HL, #0xFF68
;   CALL  set_palette
;   LD    DE, #0x0800
;   LD    HL, #0xFF6A
;   CALL  set_palette

   XOR   A
   LDH   (P_VBK), A

   LD    HL, #0x9800                   ; VRAM tiles = 0
   LD    BC, #0x0400
   LD    A, #4
   CALL  _STIR_VRAM

   LD    HL, #0x9E20                   ; VRAM tiles = 0
   LD    BC, #0x0160
   LD    A, #4
   CALL  _STIR_VRAM

   LD    HL, #0x9F00                   ; Water Tiles
   LD    BC, #0x0440
   XOR   A
fillloopx3:
   LD    D, #0x20
fillloop2:
   OR    C
   RST   ldi_v_a
   INC   A
   AND   #0x03
   DEC   D
   JR    NZ, fillloop2
   INC   C
   INC   C
   INC   C
   INC   C
   DEC   B
   JR    NZ, fillloopx3

   LD    B, #0x80
   LD    A, #0x04
   CALL  _STIR_B_VRAM

   LD    A, #0x01
   LDH   (P_VBK), A

   LD    HL, #0x9800                   ; VRAM attrib = 0
   LD    BC, #0x0400
   XOR   A
   CALL  _STIR_VRAM

   LD    HL, #0x9E20                   ; VRAM attrib = 0
   LD    BC, #0x0160
   XOR   A
   CALL  _STIR_VRAM

   LD    HL, #0x9800                   ; License Attrib (9800-987F)
   LD    A, #0x86
   LD    B, #0x80
   CALL  _STIR_B_VRAM

   LD    HL, #0x9E00                   ; Cloud Attrib (9E00-9F5F)
   LD    A, #0x03
   LD    B, #0xE0
   CALL  _STIR_B_VRAM

   LD    DE, #mountain_data2           ; Mountain Attrib (9F60-9F7F)
   LD    B, #0x20
   CALL  _LDIR_B_VRAM

   LD    A, #0x05                      ; Ocean Attrib (9F80-9FFF)
   LD    B, #0x80
   CALL  _STIR_B_VRAM

   LD    B, #0x80
   LD    A, #0x06
   CALL  _STIR_B_VRAM

   XOR   A
   LDH   (P_VBK), A

   LD    DE, #cloud_data
   LD    HL, #0x9E00                   ; Mountain Tiles
   LD    B, #0x00
   CALL  _LDIR_B_VRAM

   LD    DE, #footer_data
   LD    HL, #0x9812 ;->footer locale
   LD    BC, #0x0E03
fillloopl:
   PUSH  BC

_fill_z:
   LD    A, (DE)
   INC   DE
   RST   ldi_v_a
   DEC   B
   JR    NZ, _fill_z

   LD    A, #18
   ADD   A, L
   LD    L, A
   ADC   A, H
   SUB   L
   LD    H, A
   POP   BC
   DEC   C
   JR    NZ, fillloopl

   LD    DE, #title_data
   LD    HL, #0x98A3 ;->title locale
   LD    BC, #0x0E0E
fillloop9:
   PUSH  BC
   CALL  _LDIR_B_VRAM

   LD    A, #18
   ADD   A, L
   LD    L, A
   ADC   A, H
   SUB   L
   LD    H, A
   POP   BC
   DEC   C
   JR    NZ, fillloop9

   LD    A, #1
   LDH   (P_VBK), A

   LD    HL, #0x98A3 ;->title locale
   LD    BC, #0x0E0E
fillloopA:
   PUSH  BC
   CALL  _LDIR_B_VRAM

   LD    A, #18
   ADD   A, L
   LD    L, A
   ADC   A, H
   SUB   L
   LD    H, A
   POP   BC
   DEC   C
   JR    NZ, fillloopA

   LD    HL, #0x9C00                   ; VRAM attrib = 0
   LD    A, #7
   LD    BC, #0x0200
   CALL  _STIR_VRAM

   LD    HL, #oam_data
   LD    DE, #0xC000
   LD    B, #152
   CALL  _LDIR_B

   LD    HL, #_hiram_routine_begin
   LD    DE, #0xFF80
   LD    B, #_hiram_routine_end - _hiram_routine_begin
   CALL  _LDIR_B

   LD    A, #0b10000111
   LDH   (P_LCDC), A

   LD    A, (__gba)                    ; select gbc/gba palette
   ADD   A, #0xFF                      ;
   SBC   A, A                          ;
   AND   #0x80                         ;
   LD    C, A                          ; BC = 0 or 128 (B is already 0)

   LD    HL, #fadeoutpal               ;
   ADD   HL, BC                        ;
   LD    D, H                          ;
   LD    E, L                          ;

   LD    HL, #fadeinpal                ;
   ADD   HL, BC                        ;
   LD    B, H                          ;
   LD    C, L                          ;

   LD    A, #240
   CALL  _tpfs_setup_asm

;   CALL  _SOUNDLIB_Init
;   LD    A, #1
;   LD    (playing), A
;   LD    (currentbeat), A

;   XOR   A
;   PUSH  AF
;   INC   SP
;   CALL  _SOUNDLIB_Play
;   DEC   SP
;   POP   AF

   LD    HL, #_ram_routine_begin
   LD    DE, #_ram_routine_dest
   LD    BC, #_ram_routine_end-_ram_routine_begin
   CALL  _LDIR

   XOR   A
   LD    (title_y), A
   LD    (dec_yes), A

   RET

; interrupts must be disabled
_title_screen_real::
;   LD    A, #1
;   LD    (playing), A
;   LD    (currentbeat), A

   JP    _ram_routine_call + main_title_loop

;----------------------------------------------------------------------------
;[[ pfs_setup_asm ]]
;----------------------------------------------------------------------------
; DE -> source
; BC -> dest
; A = fadelength
;----------------------------------------------------------------------------
_tpfs_setup_asm::
   PUSH  BC
   JR    _pfs_join

;----------------------------------------------------------------------------
;[[ pfs_setup(uword *source, uword *dest, ubyte fadelength) ]]
;----------------------------------------------------------------------------
; Split 2- 64 entry 16-bit palette into 32 entry 3- 5-bit RGB entries
; Set up bresenham's structures for the fade
;
;Input:
; source = pointer to source 16-bit palette
; dest   = pointer to destination 16-bit palette
;----------------------------------------------------------------------------
_tpfs_setup::
   PUSH  BC

   LDHL  SP, #4                        ; HL->first parameter

   LD    A, (HLI)                      ;
   LD    E, A                          ;
   LD    A, (HLI)                      ;
   LD    D, A                          ; DE=source

   LD    A, (HLI)                      ;
   LD    C, A                          ;
   LD    A, (HLI)                      ;
   LD    B, A                          ; BC=dest

   LD    A, (HL)                       ; A=fadelength

_pfs_join:
   LD    (_pfs_fadelen), A             ; save fade length

   LDH   A, (_rampage)
   PUSH  AF
   LD    A, #_ram_page_pfs
   LDH   (_rampage), A
   LDH   (P_SVBK), A

   PUSH  BC                            ; save dest

   LD    B, #64                        ; 64 entries/palette
   LD    HL, #_pfs_rgb_src             ; HL->split table
   CALL  _pfs_split_rgb

   POP   DE                            ; DE=dest

   LD    B, #64                        ; 64 entries/palette
   LD    HL, #_pfs_rgb_dst             ; HL->split table
   CALL  _pfs_split_rgb

   LD    BC, #_pfs_rgb_src
   LD    DE, #_pfs_rgb_dst
   LD    HL, #_pfs_bresen

   LD    A, #192
   CALL  _pfs_make_bresen

   POP   AF
   LDH   (_rampage), A
   LDH   (P_SVBK), A

   POP   BC
   RET

;----------------------------------------------------------------------------
;[ pfs_split_rgb ]
;----------------------------------------------------------------------------
;parm:
; DE -> 16-bit RGB
; HL -> 5-bit RGB * 3
; B = number of 16-bit palettes to split
;value:
; none
;----------------------------------------------------------------------------
_pfs_split_rgb:
   LD    A, (DE)                       ; A = GGGRRRRR
   LD    C, A                          ; save a in c

   AND   #0b00011111                   ; get the R mask
   LD    (HLI), A                      ; save it

   XOR   C                             ; get lower 3 bits of G
   LD    C, A                          ; save it

   INC   DE                            ; point to next source

   LD    A, (DE)                       ; A = ?BBBBBGG
   AND   #0b00000011                   ; get the G mask
   OR    C                             ;
   RLCA                                ;
   RLCA                                ;
   RLCA                                ; shift G into place
   LD    (HLI), A                      ; save it

   LD    A, (DE)                       ; A = ?BBBBBGG
   AND   #0b01111100                   ; get the B mask
   RRCA                                ;
   RRCA                                ; shift B into place
   LD    (HLI), A                      ; save it

   INC   DE

   DEC   B
   JR    NZ, _pfs_split_rgb
   RET

;----------------------------------------------------------------------------
;[ pfs_make_bresen ]
;----------------------------------------------------------------------------
;parm:
; BC -> 5-bit RGB * 3 of source palette
; DE -> 5-bit RGB * 3 of destination palette
; HL -> bresenham structure
; A = number of bytes to setup
;value:
; none
;----------------------------------------------------------------------------
_pfs_make_bresen:                      ; setup bresenham
   PUSH  AF

   PUSH  BC                            ; bc -> starting r g b
   LD    A, (BC)                       ; HL -> struct bresenham
   LD    B, A
   LD    A, (DE)                       ; de -> destination r g b
   SUB   B

   LD    B, A
   ADD   A, A
   SBC   A, A
   LD    C, A
   XOR   B
   SUB   C
   LD    (HLI), A

   LD    A, (_pfs_fadelen)
   SRL   A
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
   JR    NZ, _pfs_make_bresen
   RET

;----------------------------------------------------------------------------
;[[ pfs_step() ]]
;----------------------------------------------------------------------------
; Run through 1 iteration of fade.
;----------------------------------------------------------------------------
_pfs_step:
   PUSH  BC

   LDH   A, (_rampage)
   PUSH  AF
   LD    A, #_ram_page_pfs
   LDH   (_rampage), A
   LDH   (P_SVBK), A

   LD    A, (_pfs_fadelen)
   LD    B, A

   LD    HL, #_pfs_bresen
   LD    DE, #_pal_cur2

   LD    A, #64
_make_pal_step:
   PUSH  AF
   PUSH  DE
   CALL  Bresenham
   LD    E, A
   CALL  Bresenham
   RRCA
   RRCA
   RRCA
   LD    D, A
   AND   #3
   LD    C, A
   XOR   D
   OR    E
   POP   DE
   LD    (DE), A
   INC   DE
   CALL  Bresenham
   RLCA
   RLCA
   OR    C
   LD    (DE), A
   INC   DE
   POP   AF
   DEC   A
   JR    NZ, _make_pal_step

set_pals:
   LD    DE, #0x0700
   LD    HL, #0xFF68
   LD    BC, #_pal_cur2
   CALL  set_palette

   LD    DE, #0x0600
   LD    HL, #0xFF6A
   LD    BC, #_pal_cur2+64
   CALL  set_palette

   POP   AF
   LDH   (_rampage), A
   LDH   (P_SVBK), A
   POP   BC
   RET

;----------------------------------------------------------------------------
;[[ set_palette() ]]
;----------------------------------------------------------------------------
;parms:
; D = number of palettes
; E = first palette
; HL -> Palette pointer (0xFF68 or 0xFF6A)
; BC -> rgb_data
;value:
; none
;----------------------------------------------------------------------------
set_palette:
   LD    A, D                          ; A = nb_palettes
   ADD   A, A                          ; A *= 8
   ADD   A, A
   ADD   A, A
   LD    D, A

   LD    A, E                          ; E = first_palette
   ADD   A, A                          ; A *= 8
   ADD   A, A
   ADD   A, A
   LD    E, A                          ; E = first pointer data

pal_loop:
   LDH   A, (P_STAT)
	AND	#0x02
   JR    NZ, pal_loop

   LD    A, E
   LD    (HLI), A
   LD    A, (BC)
   LD    (HLD), A
   INC   BC                            ; next rgb_data
   INC   E                             ; next pointer
   DEC   D                             ; count down palettes
   JR    NZ, pal_loop

   RET

_hiram_routine_begin:
_oam_dma_rom_b:
   LDH   (P_DMA), A
   LD    A, #0x28
_oam_dma_delay:                        ; wait 160us for DMA to complete
   DEC   A
   JR    NZ, _oam_dma_delay
   RET
_hiram_routine_end:

_title_pal::

fadeoutpal:
.DW 0b000000000000000,0b000000010000100,0b000001010110100,0b110001111111111
.DW 0b000000000000000,0b000010010000110,0b000000010010010,0b000001100011000
.DW 0b000000000000000,0b001000010000100,0b011000110001100,0b111111111111111
.DW 0b000000000000000,0b000000000000000,0b000000000000000,0b000000000000000
.DW 0b000000000000000,0b000000000000000,0b000000000000000,0b000000000000000
.DW 0b000000000000000,0b000000000000000,0b000000000000000,0b000000000000000
.DW 0b000000000000000,0b001000010000100,0b000100001011111,0b111111111111111
.DW 0b000000000000000,0b000000000000000,0b011110111101111,0b111111111111111

.DW 0b000000000000000,0b000000010000100,0b000001010110100,0b110001111111111
.DW 0b000000000000000,0b000010010000110,0b000000010010010,0b000001100011000
.DW 0b000000000000000,0b001000010000100,0b011000110001100,0b111111111111111
.DW 0b000000000000000,0b000000000000000,0b000000000000000,0b000000000000000
.DW 0b000000000000000,0b000000000000000,0b000000000000000,0b000000000000000
.DW 0b000000000000000,0b000000000000000,0b000000000000000,0b000000000000000
.DW 0b000000000000000,0b000000000000000,0b000000000000000,0b000000000000000
.DW 0b000000000000000,0b000000000000000,0b000000000000000,0b000000000000000

_title_pal_gba::

.DW 0b000000000000000,0b000110111001001,0b000111110111100,0b111101111111111
.DW 0b000000000000000,0b001000111001100,0b000110111011011,0b000111111111110
.DW 0b000000000000000,0b001110111001001,0b110011100010111,0b111111111111111
.DW 0b000000000000000,0b000000000000000,0b000000000000000,0b000000000000000
.DW 0b000000000000000,0b000000000000000,0b000000000000000,0b000000000000000
.DW 0b000000000000000,0b000000000000000,0b000000000000000,0b000000000000000
.DW 0b000000000000000,0b001110111001001,0b001110110011111,0b111111111111111
.DW 0b000000000000000,0b000000000000000,0b110111101111010,0b111111111111111

.DW 0b000000000000000,0b000110111001001,0b000111110111100,0b111101111111111
.DW 0b000000000000000,0b001000111001010,0b000110111011011,0b000111111111110
.DW 0b000000000000000,0b001110111001001,0b110011100010111,0b111111111111111
.DW 0b000000000000000,0b000000000000000,0b000000000000000,0b000000000000000
.DW 0b000000000000000,0b000000000000000,0b000000000000000,0b000000000000000
.DW 0b000000000000000,0b000000000000000,0b000000000000000,0b000000000000000
.DW 0b000000000000000,0b000000000000000,0b000000000000000,0b000000000000000
.DW 0b000000000000000,0b000000000000000,0b000000000000000,0b000000000000000

fadeinpal:
.DW 0b111110011100000,0b000000010000100,0b000001010110100,0b110001111111111
.DW 0b111110011100000,0b000010010000110,0b000000010010010,0b000001100011000
.DW 0b111110011100000,0b001000010000100,0b011000110001100,0b111111111111111
.DW 0b111110011100000,0b101001001001000,0b111110101100100,0b111111111111111
.DW 0b111110011100000,0b011000010000001,0b111110101100100,0b001010111100101
.DW 0b111110011100000,0b101000000000000,0b010100001100000,0b101001001001000
.DW 0b000000000000000,0b001000010000100,0b000100001011111,0b111111111111111
.DW 0b000000000000000,0b000000000000000,0b011110111101111,0b111111111111111

.DW 0b000000000000000,0b000000010000100,0b000001010110100,0b110001111111111
.DW 0b000000000000000,0b000010010000110,0b000000010010010,0b000001100011000
.DW 0b000000000000000,0b001000010000100,0b011000110001100,0b111111111111111
.DW 0b000000000000000,0b101001001001000,0b111110101100100,0b111111111111111
.DW 0b000000000000000,0b010000001000001,0b101001001001000,0b111111111111111
.DW 0b000000000000000,0b000000000000000,0b000000000000000,0b000000000000000
.DW 0b000000000000000,0b000000000000000,0b000000000000000,0b000000000000000
.DW 0b000000000000000,0b000000000000000,0b000000000000000,0b000000000000000


.DW 0b111111001100010,0b000110111001001,0b000111110111100,0b111101111111111
.DW 0b111111001100010,0b001000111001010,0b000110111011011,0b000111111111110
.DW 0b111111001100010,0b001110111001001,0b110011100010111,0b111111111111111
.DW 0b111111001100010,0b111001110110000,0b111111100001001,0b111111111111111
.DW 0b111111001100010,0b110010111000101,0b111111100001001,0b100101101001110
.DW 0b111111001100010,0b111000100000010,0b101100100000010,0b111001110110000
.DW 0b000000000000000,0b001110111001001,0b001110110011111,0b111111111111111
.DW 0b000000000000000,0b000000000000000,0b110111101111010,0b111111111111111

.DW 0b000000000000000,0b000110111001001,0b000111110111100,0b111101111111111
.DW 0b000000000000000,0b001000111001010,0b000110111011011,0b000111111111110
.DW 0b000000000000000,0b001110111001001,0b110011100010111,0b111111111111111
.DW 0b000000000000000,0b111001110110000,0b111111100001001,0b111111111111111
.DW 0b000000000000000,0b100100111100101,0b110111110010000,0b111111111111111
.DW 0b000000000000000,0b000000000000000,0b000000000000000,0b000000000000000
.DW 0b000000000000000,0b000000000000000,0b000000000000000,0b000000000000000
.DW 0b000000000000000,0b000000000000000,0b000000000000000,0b000000000000000

_title_pal_full::

newpal:
.DW 0b000000000000000,0b000000000000000,0b000000000000000,0b000000000000000
.DW 0b000000000000000,0b000000000000000,0b000000000000000,0b000000000000000
.DW 0b000000000000000,0b000000000000000,0b000000000000000,0b000000000000000
.DW 0b111110011100000,0b101001001001000,0b111110101100100,0b111111111111111
.DW 0b111110011100000,0b011000010000001,0b111110101100100,0b001010111100101
.DW 0b111110011100000,0b101000000000000,0b010100001100000,0b101001001001000
.DW 0b000000000000000,0b001000010000100,0b000100001011111,0b111111111111111
.DW 0b000000000000000,0b000000000000000,0b011110111101111,0b111111111111111

.DW 0b000000000000000,0b000000000000000,0b000000000000000,0b000000000000000
.DW 0b000000000000000,0b000000000000000,0b000000000000000,0b000000000000000
.DW 0b000000000000000,0b000000000000000,0b000000000000000,0b000000000000000
.DW 0b000000000000000,0b000000000000000,0b000000000000000,0b000000000000000
.DW 0b000000000000000,0b000000000000000,0b000000000000000,0b000000000000000
.DW 0b000000000000000,0b000000000000000,0b000000000000000,0b000000000000000
.DW 0b000000000000000,0b000000000000000,0b000000000000000,0b000000000000000
.DW 0b000000000000000,0b000000000000000,0b000000000000000,0b000000000000000

_title_pal_full_gba::

.DW 0b000000000000000,0b000000000000000,0b000000000000000,0b000000000000000
.DW 0b000000000000000,0b000000000000000,0b000000000000000,0b000000000000000
.DW 0b000000000000000,0b000000000000000,0b000000000000000,0b000000000000000
.DW 0b111111001100010,0b111001110110000,0b111111100001001,0b111111111111111
.DW 0b111111001100010,0b110010111000101,0b111111100001001,0b100101101001110
.DW 0b111111001100010,0b111000100000010,0b101100100000010,0b111001110110000
.DW 0b000000000000000,0b001110111001001,0b001110110011111,0b111111111111111
.DW 0b000000000000000,0b000000000000000,0b110111101111010,0b111111111111111

.DW 0b000000000000000,0b000000000000000,0b000000000000000,0b000000000000000
.DW 0b000000000000000,0b000000000000000,0b000000000000000,0b000000000000000
.DW 0b000000000000000,0b000000000000000,0b000000000000000,0b000000000000000
.DW 0b000000000000000,0b000000000000000,0b000000000000000,0b000000000000000
.DW 0b000000000000000,0b000000000000000,0b000000000000000,0b000000000000000
.DW 0b000000000000000,0b000000000000000,0b000000000000000,0b000000000000000
.DW 0b000000000000000,0b000000000000000,0b000000000000000,0b000000000000000
.DW 0b000000000000000,0b000000000000000,0b000000000000000,0b000000000000000

title_data:
.DB   0xD0,0xD0,0xAE,0xD0,0x94,0x9C,0xA4,0xD0,0xD0,0xD0,0xB3,0xD0,0xD0,0xD0
.DB   0x81,0x84,0x8B,0xD0,0x95,0x9D,0xD0,0xD0,0xD0,0xD0,0xB4,0xD0,0xD0,0xD0
.DB   0x82,0x85,0xBB,0xD0,0x96,0x9E,0xD0,0xD0,0xD0,0xD0,0xB5,0xD0,0xD0,0xD0
.DB   0xD0,0x86,0xD0,0xD0,0x97,0x9F,0xD0,0xFE,0xD0,0xAF,0xB6,0xBC,0xC0,0xC5
.DB   0xD0,0x87,0x8C,0x90,0x98,0xA0,0xA5,0xA8,0xAB,0xB0,0xB7,0xBD,0xC1,0xC6
.DB   0xD0,0x88,0x8D,0x91,0x99,0xA1,0xA6,0xA9,0xAC,0xB1,0xB8,0xBE,0xC2,0xC7
.DB   0xD0,0x89,0x8E,0x92,0x9A,0xA2,0xA7,0xAA,0xAD,0xB2,0xB9,0xBF,0xC3,0xC8
.DB   0x83,0x8A,0x8F,0x93,0x9B,0xA3,0xD0,0xD0,0xD0,0x80,0xBA,0xD0,0xC4,0xC9
.DB   0xD0,0xCC,0xD1,0xD5,0xD9,0xDC,0xD0,0xD0,0xE4,0xE9,0xED,0xF5,0xD0,0xFA
.DB   0xCA,0xCD,0xD0,0xD0,0xD0,0xDD,0xE0,0xE2,0xE5,0xD0,0xEE,0xD0,0xD0,0xFB
.DB   0xCB,0xCE,0xD0,0xD0,0xD0,0xDE,0xE1,0xE3,0xE6,0xD0,0xEF,0xD0,0xF8,0xFC
.DB   0xD0,0xCF,0xD2,0xD6,0xDA,0xDF,0xD0,0xD0,0xE7,0xEA,0xF0,0xF6,0xF9,0xD0
.DB   0xD0,0xD0,0xD0,0xD0,0xD0,0xD0,0xD0,0xD0,0xD0,0xD0,0xF1,0xD0,0xD0,0xD0
.DB   0xD0,0xD0,0xD0,0xD0,0xD0,0xD0,0xD0,0xD0,0xD0,0xD0,0xF2,0xD0,0xD0,0xD0

.DB   0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x81,0x80,0x80,0x80
.DB   0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x81,0x80,0x80,0x80
.DB   0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x81,0x80,0x80,0x80
.DB   0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x81,0x81,0x81,0x80,0x80
.DB   0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x82,0x80,0x80,0x80
.DB   0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x82,0x80,0x80,0x80
.DB   0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x82,0x80,0x80,0x80
.DB   0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x82,0x82,0x80,0x80,0x80
.DB   0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80
.DB   0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x82,0x80,0x80,0x80
.DB   0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x82,0x80,0x80,0x80
.DB   0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x82,0x80,0x80,0x80
.DB   0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x82,0x80,0x80,0x80
.DB   0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x82,0x80,0x80,0x80

footer_data:
.DB   0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04
.DB   0x04,0x04,0x34,0x35,0x36,0x37,0x38,0x39,0x3A,0x3B,0x3C,0x3D,0x04,0x04
.DB   0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04

cloud_data:
.DB 0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x10,0x12,0x14,0x16,0x18,0x04,0x04,0x04,0x04
.DB 0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x10,0x12,0x14,0x16,0x18,0x04,0x04,0x04,0x04

.DB 0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x11,0x13,0x15,0x17,0x19,0x04,0x04,0x04,0x04
.DB 0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x11,0x13,0x15,0x17,0x19,0x04,0x04,0x04,0x04

.DB 0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04
.DB 0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04

.DB 0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04
.DB 0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04

.DB 0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04
.DB 0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04

.DB 0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04
.DB 0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04

.DB 0x57,0x58,0x50,0x50,0x50,0x50,0x5C,0x50,0x50,0x51,0x53,0x55,0x50,0x50,0x50,0x50
.DB 0x50,0x57,0x58,0x50,0x5C,0x50,0x50,0x50,0x50,0x57,0x58,0x50,0x5C,0x5E,0x60,0x50

mountain_data:
.DB 0x1C,0x1D,0x1E,0x1F,0x20,0x21,0x22,0x5A,0x59,0x52,0x54,0x56,0x23,0x24,0x25,0x26
.DB 0x27,0x28,0x29,0x2A,0x2B,0x2C,0x2D,0x2E,0x2F,0x30,0x31,0x32,0x5D,0x5F,0x61,0x63

mountain_data2:
.DB 0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x03,0x03,0x03,0x03,0x03,0x03,0x04,0x04,0x04
.DB 0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x03,0x03,0x03,0x03

wave_data:
.DB 0x00,0x00,0x5D,0x08,0x00,0x00,0xBA,0x30
.DB 0x00,0x00,0xD5,0x80,0xAA,0x00,0xDD,0x0C
.DB 0x00,0x00,0x55,0x11,0x00,0x00,0xAF,0x87
.DB 0x00,0x00,0x7D,0x38,0xAA,0x00,0xDD,0x40
.DB 0x02,0x02,0x55,0x00,0x38,0x38,0xAA,0x00
.DB 0x00,0x00,0x55,0x00,0xAE,0x0E,0xDD,0x00
.DB 0x18,0x18,0x55,0x00,0x03,0x03,0xAA,0x00
.DB 0x40,0x40,0x55,0x00,0xBA,0x30,0xDD,0x00
.DB 0xAA,0x00,0x77,0x00,0xFF,0x00,0xFF,0x0F
.DB 0xFF,0x00,0xFF,0x00,0xFF,0x00,0xFF,0x00
.DB 0xAA,0x00,0x7F,0x0F,0xFF,0x00,0xFF,0x80
.DB 0xFF,0x00,0xCE,0x0E,0xFF,0x00,0xFF,0x00
.DB 0xAA,0x00,0x77,0x00,0xFF,0x3C,0xFF,0x00
.DB 0xFF,0x00,0xFF,0x00,0xFF,0x00,0xFF,0x00
.DB 0xAA,0x00,0x77,0x00,0xFF,0x38,0xFF,0x00
.DB 0xFF,0x00,0xCE,0x0E,0xFF,0x00,0xFF,0x00
.DB 0xFF,0x00,0xEE,0x0E,0xAA,0x55,0x55,0xAA
.DB 0xE0,0xEF,0x00,0xFF,0x00,0xFF,0x00,0xFF
.DB 0xFF,0x00,0xFF,0x00,0xAA,0x55,0x55,0xAA
.DB 0x03,0xF3,0x00,0xFF,0x00,0xFF,0x00,0xFF
.DB 0x9D,0x1C,0xFF,0x00,0xAA,0x55,0x55,0xAA
.DB 0x00,0xFF,0x00,0xF8,0x00,0xFF,0x00,0xFF
.DB 0xFF,0x00,0xFF,0x00,0xAA,0x55,0x55,0xAA
.DB 0x00,0xFF,0xF8,0xF9,0x00,0xFF,0x00,0xFF
.DB 0x03,0xF3,0x00,0xFF,0x00,0xFF,0x00,0xFF
.DB 0x00,0xFF,0x00,0xFF,0x00,0xFF,0x00,0xFF
.DB 0xC0,0xCF,0x00,0xFF,0x00,0xFF,0x00,0xFF
.DB 0x00,0xFF,0x00,0xFF,0x00,0xFF,0x00,0xFF
.DB 0x00,0xFF,0x00,0xFF,0x0F,0x0F,0x00,0xFF
.DB 0x00,0xFF,0x00,0xFF,0x00,0xFF,0x00,0xFF
.DB 0x00,0xFF,0x00,0xFF,0xB0,0xB0,0x00,0xFF
.DB 0x00,0xFF,0x00,0xFF,0x00,0xFF,0x00,0xFF

.DB 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
.DB 0x00,0x55,0x00,0x00,0x00,0xAA,0x00,0xDD
.DB 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
.DB 0x00,0x55,0x00,0x00,0x87,0x29,0xFF,0x03
.DB 0x1F,0xA3,0xBF,0x5D,0x7F,0xBE,0x7F,0xBF
.DB 0xFF,0x7F,0xFF,0xDE,0xFF,0x00,0x00,0x00
.DB 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
.DB 0x06,0x51,0xF0,0xC0,0xF8,0xE2,0xF8,0xE5
.DB 0xFF,0xC0,0xFF,0xDD,0xFF,0xFF,0xFF,0xFF
.DB 0xFF,0xF3,0xFF,0x0F,0xFF,0x00,0x00,0x00
.DB 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
.DB 0x00,0x55,0x00,0x00,0xE4,0x6A,0x71,0xAC
.DB 0xF0,0xCA,0xF8,0x87,0xFC,0x7B,0xFF,0xD9
.DB 0xFF,0xC0,0xFF,0xE3,0xFF,0x00,0x00,0x00
.DB 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
.DB 0x00,0x55,0x0F,0x0E,0x7F,0x7F,0xFE,0xE1
.DB 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
.DB 0x00,0x55,0xE3,0xE0,0xFF,0x5E,0x80,0x5D
.DB 0x00,0xAA,0x00,0x77,0x00,0xFF,0x00,0xFF
.DB 0x03,0xFC,0x7F,0x8F,0xFF,0x00,0x00,0x00
.DB 0x00,0xAA,0x06,0x71,0x00,0xFF,0x00,0xFF
.DB 0x60,0xFF,0xF8,0x77,0xFF,0x00,0x00,0x00
.DB 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
.DB 0x00,0x55,0x00,0x00,0xF0,0x6A,0xFC,0xB5
.DB 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
.DB 0x00,0x55,0x7E,0x2C,0xEF,0x45,0x00,0xDD
.DB 0x00,0xAA,0x00,0x77,0x30,0xCF,0x00,0xFF
.DB 0x03,0xFD,0x1F,0xE7,0x7F,0x80,0x00,0x00
.DB 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
.DB 0x00,0x55,0x00,0x00,0xC0,0x2A,0x60,0x9D
.DB 0x03,0xAA,0x7F,0x33,0xFF,0x77,0xFF,0xFF
.DB 0xFF,0xFF,0xFF,0x72,0xFF,0x00,0x00,0x00
.DB 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
.DB 0x00,0x55,0x00,0x00,0x74,0xEA,0xFE,0xF5
.DB 0xF8,0x7A,0xFF,0xBB,0xFF,0xC7,0xFF,0xF7
.DB 0xFF,0x9F,0xFF,0x73,0xFF,0x00,0x00,0x00
.DB 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
.DB 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
.DB 0x00,0xAA,0x80,0x77,0x80,0x7F,0xC0,0xBF
.DB 0xF8,0xC7,0xFC,0xFB,0xFF,0x00,0x00,0x00

oam_data:
   .DB   0x28,0x40,0x10,0x03
   .DB   0x28,0x48,0x12,0x03
   .DB   0x28,0x50,0x14,0x03
   .DB   0x28,0x58,0x16,0x03
   .DB   0x28,0x60,0x18,0x03
   .DB   0x28,0xC0,0x10,0x03
   .DB   0x28,0xC8,0x12,0x03
   .DB   0x28,0xD0,0x14,0x03
   .DB   0x28,0xD8,0x16,0x03
   .DB   0x28,0xE0,0x18,0x03
   .DB   0x00,0x00,0x00,0x00
   .DB   0x00,0x00,0x00,0x00

;   .DB   0x40,0x10,0x28,0x04 ; old clouds
;   .DB   0x40,0x18,0x2A,0x04
;   .DB   0x40,0x20,0x2C,0x04
;   .DB   0x50,0x40,0x2E,0x04
;   .DB   0x50,0x48,0x30,0x04
;   .DB   0x50,0x50,0x32,0x04
;   .DB   0x30,0x78,0x28,0x04
;   .DB   0x30,0x80,0x2A,0x04
;   .DB   0x30,0x88,0x2C,0x04
;   .DB   0x48,0xA8,0x2E,0x04
;   .DB   0x48,0xB0,0x30,0x04
;   .DB   0x48,0xB8,0x32,0x04
;;  .DB   0x34,0x40,0x18,0x04
;;  .DB   0x34,0x48,0x1A,0x04

   .DB   0x62,0x23,0xCA,0x00 ; 26 sprites for title
   .DB   0x5A,0x2B,0xCC,0x00
   .DB   0x6A,0x2B,0xCE,0x00
   .DB   0x52,0x33,0xD0,0x00
   .DB   0x72,0x33,0xD2,0x00
   .DB   0x52,0x3B,0xD4,0x00
   .DB   0x72,0x3B,0xD6,0x00
   .DB   0x52,0x43,0xD8,0x00
   .DB   0x72,0x43,0xDA,0x00
   .DB   0x5A,0x4B,0xDC,0x00
   .DB   0x6A,0x4B,0xDE,0x00
   .DB   0x62,0x53,0xE0,0x00
   .DB   0x62,0x5B,0xE2,0x00
   .DB   0x5A,0x63,0xE4,0x00
   .DB   0x6A,0x63,0xE6,0x00
   .DB   0x52,0x6B,0xE8,0x00
   .DB   0x72,0x6B,0xEA,0x00
   .DB   0x52,0x73,0xEC,0x00
   .DB   0x62,0x73,0xEE,0x02
   .DB   0x72,0x73,0xF0,0x02
   .DB   0x82,0x73,0xF2,0x02
   .DB   0x52,0x7B,0xF4,0x00
   .DB   0x72,0x7B,0xF6,0x00
   .DB   0x6A,0x83,0xF8,0x00
   .DB   0x5A,0x8B,0xFA,0x00
   .DB   0x6A,0x8B,0xFC,0x00

Title_tiles:
.DB 0x00,0x04,0x02,0x03,0x01,0x03,0x00,0x01
.DB 0x07,0x07,0x00,0x01,0x00,0x00,0x00,0x00

;.DB 0x00,0xAA,0x00,0x77,0x00,0xFF,0x00,0xFF
;.DB 0x00,0xFF,0x00,0xFF,0x00,0xFF,0x00,0x00
.DB 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
.DB 0x00,0x00,0x00,0x00,0x00,0x47,0x47,0xFF
.DB 0xBC,0xFF,0x01,0xFE,0x3F,0x40,0x3F,0x00
.DB 0x1E,0x00,0x00,0x00,0x00,0x00,0x00,0x00
.DB 0x00,0x00,0x00,0x47,0x47,0xFF,0xB8,0xFF
.DB 0x03,0xFC,0x3F,0x40,0x3F,0x00,0x1E,0x00
.DB 0x00,0x00,0x00,0x00,0x03,0x00,0x00,0x00
.DB 0x00,0x07,0x07,0x7F,0x78,0xFF,0x81,0xFE
.DB 0x07,0xF8,0x87,0x78,0x47,0xF8,0x67,0xF8
.DB 0x57,0xF8,0x4F,0xF8,0x47,0xF8,0x47,0xF8
.DB 0x47,0xF8,0x47,0xF8,0x47,0xF8,0x47,0xF8
.DB 0x47,0xF8,0x47,0xF8,0x47,0xF8,0x47,0xF8
.DB 0x47,0xF8,0x47,0xF8,0x47,0xF8,0x67,0xF8
.DB 0x77,0xF8,0x7F,0xF8,0x7F,0xF8,0x5F,0xF8
.DB 0x4F,0xF8,0x47,0xF8,0x47,0xF8,0x47,0xF8
.DB 0x47,0xF8,0x47,0xF8,0x47,0xF8,0x47,0xF8
.DB 0x47,0xF8,0x47,0xF8,0x47,0xF8,0x47,0xF8
.DB 0x47,0xF8,0x47,0xF8,0x67,0xF8,0x57,0xF8
.DB 0x4B,0xFF,0x44,0xFF,0x83,0xFC,0x3F,0xC0
.DB 0xFF,0x00,0xFE,0x00,0xE0,0x00,0x00,0x00
.DB 0x00,0x10,0x38,0x10,0x3C,0xF8,0x39,0x7E
.DB 0x70,0xFC,0x82,0xFC,0x37,0xC8,0xFF,0x00
.DB 0x00,0x1F,0x1F,0x3F,0x18,0x3F,0x1C,0x3F
.DB 0x16,0x3F,0x13,0x3F,0x11,0x3E,0x11,0x3E
.DB 0x11,0x3E,0x11,0x3E,0x11,0x3E,0x11,0x3E
.DB 0x11,0x3E,0x11,0x3E,0x11,0x3E,0x19,0x3E
.DB 0x1D,0x3E,0x1B,0x3C,0x17,0x38,0x0F,0x30
.DB 0x1F,0x00,0x0E,0x00,0x04,0x78,0x74,0xF8
.DB 0x82,0xFC,0x37,0xC8,0xFF,0x00,0xFF,0x00
.DB 0xE2,0x00,0x00,0x00,0x00,0x00,0x00,0x00
.DB 0x00,0xFF,0xFE,0xFF,0x00,0xFF,0x06,0xF9
.DB 0x1F,0xE1,0x7F,0x81,0xFF,0x01,0xFB,0x01
.DB 0xE3,0x01,0xC3,0x01,0xC3,0x01,0xC3,0x01
.DB 0xC3,0x01,0xC3,0x01,0xC3,0x01,0xC3,0x01
.DB 0xC2,0x01,0xC1,0x00,0xC0,0x00,0x80,0x00
.DB 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
.DB 0x00,0x03,0x03,0x0F,0x8C,0x1F,0x08,0x07
.DB 0x06,0x01,0x03,0x00,0x00,0x00,0x00,0x00
.DB 0x00,0x00,0x00,0x00,0x00,0x03,0x03,0x07
.DB 0x04,0x0F,0x04,0x0F,0x04,0x0F,0x04,0x0F
.DB 0x06,0x0F,0x07,0x0F,0x05,0x0F,0x04,0x0F
.DB 0x04,0x0F,0x04,0x0F,0x04,0x0F,0x04,0x0F
.DB 0x04,0x0F,0x04,0x0F,0x04,0x0F,0x04,0x0F
.DB 0x04,0x0F,0x04,0x0F,0x04,0x0F,0x06,0x0F
.DB 0x05,0x0F,0x04,0x0F,0x04,0x0F,0x04,0x0F
.DB 0x04,0x0F,0x04,0x1F,0x18,0x7F,0x60,0xFF
.DB 0x78,0x07,0xB4,0x0F,0x56,0x8F,0x67,0x8F
.DB 0x77,0x8F,0x77,0x8F,0x75,0x8F,0x74,0x8F
.DB 0xF4,0x8F,0x74,0x8F,0x74,0x8F,0x74,0x8F
.DB 0x74,0x8F,0x74,0x8F,0x74,0x8F,0x74,0x8F
.DB 0x74,0x8F,0xF4,0x0F,0xF4,0x0F,0x64,0x0F
.DB 0x04,0x0F,0x04,0x0F,0x06,0x0F,0x07,0xCF
.DB 0xE5,0xCF,0x34,0xCF,0x28,0xDF,0x30,0xDF
.DB 0x03,0xFC,0x8F,0x70,0xFF,0x00,0x3F,0x00
.DB 0x00,0x30,0x34,0xF8,0xC5,0xFE,0x11,0xEF
.DB 0x38,0xCF,0x78,0x8F,0x73,0x8C,0x7F,0x80
.DB 0x77,0x80,0x70,0x80,0xF0,0x80,0xF0,0x80
.DB 0x70,0x80,0x70,0x80,0x70,0x80,0x70,0x80
.DB 0x70,0x80,0x70,0x80,0x70,0x80,0x70,0x80
.DB 0x70,0x80,0x70,0x80,0x70,0x80,0x70,0x80
.DB 0x70,0x80,0xF0,0x80,0x70,0x80,0x70,0x80
.DB 0x70,0x80,0x08,0xF0,0x34,0xF8,0x1E,0xE0
.DB 0x7C,0x83,0x7B,0x87,0x72,0x87,0x72,0x87
.DB 0xF3,0x87,0xF2,0x87,0xF2,0x87,0xF2,0x87
.DB 0x72,0x87,0x72,0x87,0x72,0x87,0x72,0x87
.DB 0x72,0x87,0x72,0x87,0x72,0x87,0x72,0x87
.DB 0x72,0x87,0x73,0x87,0x73,0x87,0x72,0x87
.DB 0x71,0x86,0x73,0x80,0x71,0x80,0x70,0x80
.DB 0xF0,0x80,0xF0,0x80,0x70,0x80,0xF0,0x00
.DB 0xF0,0x00,0xE0,0x00,0x80,0x00,0x00,0x00
.DB 0x00,0x00,0x00,0x00,0x00,0x00,0x40,0x80
.DB 0x20,0xC0,0xF0,0x00,0xF8,0x00,0xE0,0x00
.DB 0x00,0xC3,0xA3,0xC7,0x33,0xC7,0x3B,0xC7
.DB 0x3A,0xC7,0xBA,0xC7,0x7A,0xC7,0x3A,0xC7
.DB 0x3A,0xC7,0x3A,0xC7,0x3A,0xC7,0x3A,0xC7
.DB 0x3A,0xC7,0x3A,0xC7,0x3A,0xC7,0x3B,0xC7
.DB 0x3B,0xC7,0x7B,0x87,0xFA,0x07,0xF1,0x06
.DB 0xE3,0x00,0xC1,0x00,0x80,0x00,0x00,0x00
.DB 0x1E,0xFF,0xFF,0xFF,0x00,0xFF,0x80,0xFF
.DB 0xC3,0xFC,0x6F,0xF0,0x3F,0xC0,0x3F,0xC0
.DB 0x3C,0xC0,0x38,0xC0,0x38,0xC0,0x38,0xC0
.DB 0x38,0xC0,0x38,0xC0,0x38,0xC0,0x38,0xC0
.DB 0xB8,0xC0,0x78,0x80,0xF8,0x00,0xF0,0x00
.DB 0xE0,0x00,0xC0,0x00,0x80,0x00,0x00,0x00
.DB 0x00,0xE0,0xD0,0xE1,0x08,0xF1,0xCC,0x31
.DB 0xEE,0x31,0xEE,0x31,0xEE,0x31,0x6E,0x31
.DB 0x7E,0x31,0x6E,0x31,0x6E,0x31,0x6E,0x31
.DB 0x6E,0x31,0x6E,0x31,0x6E,0x31,0x6E,0x31
.DB 0x4E,0x31,0x3E,0x01,0x1E,0x01,0x0C,0x01
.DB 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
.DB 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
.DB 0x00,0x00,0x10,0x00,0x10,0x00,0x00,0x10
.DB 0x1F,0x00,0x6F,0x1F,0xBE,0x7F,0xFC,0x7F
.DB 0x9C,0x63,0xE6,0x01,0x01,0x00,0x00,0x00
.DB 0x00,0xF0,0xE8,0xF0,0x8C,0xF0,0x8E,0xF0
.DB 0xCE,0xF0,0xAE,0xF0,0x9E,0xF0,0x8E,0xF0
.DB 0x8E,0xF0,0x8E,0xF0,0x8E,0xF0,0x8E,0xF0
.DB 0x8E,0xF0,0x8E,0xF0,0x8E,0xF0,0x8E,0xF0
.DB 0xCE,0xF0,0xDE,0xE0,0xBE,0xC0,0x7C,0x80
.DB 0xF8,0x00,0x70,0x00,0x20,0x00,0x00,0x00
.DB 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
.DB 0x00,0x00,0x00,0x00,0x18,0x00,0x34,0x18
.DB 0x3C,0x00,0x7E,0x18,0x52,0x3C,0x56,0x38
.DB 0x7E,0x00,0x7E,0x18,0x52,0x3C,0x56,0x38
.DB 0x7E,0x00,0x7E,0x18,0x52,0x3C,0x56,0x38
.DB 0x7E,0x00,0x7E,0x18,0xE3,0x3C,0xBD,0x42
.DB 0xFF,0x18,0xFF,0x24,0xFF,0x52,0xFF,0x42
.DB 0xF7,0x4A,0xE7,0x5A,0xFF,0x24,0xFF,0x18
.DB 0x5E,0x20,0x9F,0x60,0x1F,0xE0,0x57,0xE8
.DB 0x73,0xEC,0x79,0xE6,0x7D,0xE2,0x77,0xE8
.DB 0x73,0xEC,0x71,0xEE,0x71,0xEE,0x31,0xEE
.DB 0x11,0xEE,0x51,0xEE,0x79,0xEE,0x7D,0xEE
.DB 0x37,0xEE,0x13,0xEE,0x11,0xEE,0x19,0xEE
.DB 0x1D,0xEE,0x5F,0xEE,0x7F,0xEE,0x7F,0xEE
.DB 0x77,0xEE,0x73,0xEE,0xF1,0xEE,0xE1,0xFE
.DB 0xF9,0xFE,0xE1,0xFE,0xF9,0xEE,0x7D,0xEE
.DB 0xFF,0x00,0xE2,0x00,0x80,0x00,0x00,0x00
.DB 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
.DB 0xF8,0x00,0x76,0xF8,0x05,0xFE,0x01,0xFE
.DB 0x39,0xC6,0x67,0x80,0x80,0x00,0x00,0x00
.DB 0x00,0x18,0x14,0x38,0x16,0x38,0x17,0x38
.DB 0x17,0x38,0x17,0x38,0x17,0x38,0x17,0x38
.DB 0x17,0x38,0x17,0x38,0x17,0x38,0x17,0x38
.DB 0x17,0x38,0x17,0x38,0x17,0x38,0x16,0x39
.DB 0x11,0x3F,0x07,0x3F,0x11,0x0F,0x0F,0x00
.DB 0x07,0x00,0x03,0x00,0x00,0x00,0x00,0x00
.DB 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
.DB 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01
.DB 0x01,0x03,0x02,0x07,0x02,0x07,0x02,0x07
.DB 0x02,0x07,0x02,0x07,0x02,0x07,0x03,0x07
.DB 0x03,0x07,0x02,0x07,0x02,0x07,0x02,0x07
.DB 0x02,0x07,0x02,0x1F,0x1C,0x7F,0x60,0xFF
.DB 0x80,0xFF,0x00,0xFF,0x80,0xFF,0xFC,0x03
.DB 0xFA,0x07,0xFA,0x07,0x02,0x07,0x00,0x03
.DB 0x01,0x03,0x01,0x03,0x01,0x03,0x00,0x01
.DB 0x00,0x01,0x00,0x01,0x00,0x01,0x00,0x01
.DB 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
.DB 0x00,0x00,0x00,0x40,0x60,0xC0,0xB0,0xC0
.DB 0x38,0xC0,0x38,0xC0,0x38,0xC0,0x38,0xC0
.DB 0x38,0xC0,0x38,0xC0,0x38,0xC0,0x38,0xC0
.DB 0xB8,0xC0,0xF8,0xC0,0x78,0xC0,0x38,0xC0
.DB 0x38,0xC0,0x38,0xC0,0x38,0xC0,0x38,0xC0
.DB 0x38,0xC0,0x38,0xC0,0x38,0xC0,0x38,0xC0
.DB 0x38,0xC0,0x18,0xE0,0x18,0xE0,0x1C,0xE0
.DB 0x1C,0xE0,0x9C,0xE0,0x5C,0xE0,0xAC,0xF0
.DB 0x8C,0xF0,0x8E,0xF0,0x8E,0xF0,0x8E,0xF0
.DB 0x00,0x01,0x01,0x03,0x02,0x07,0x02,0x07
.DB 0x02,0x07,0x04,0x0F,0x04,0x0F,0x04,0x0F
.DB 0x04,0x0F,0x04,0x0F,0x00,0x0F,0x0C,0x07
.DB 0x00,0x07,0x04,0x03,0x02,0x01,0x01,0x00
.DB 0x00,0x00,0x00,0x00,0x00,0x01,0x01,0x07
.DB 0x06,0x1F,0x18,0x3F,0x21,0x7E,0x67,0xF8
.DB 0xCF,0xF0,0xFC,0xE0,0x78,0xC0,0x70,0x80
.DB 0x60,0x80,0xE0,0x00,0xC0,0x00,0xC0,0x00
.DB 0xC0,0x00,0xC0,0x00,0xC0,0x00,0x40,0x80
.DB 0x60,0x80,0x20,0xC0,0x10,0xE0,0x04,0xF8
.DB 0x84,0x7F,0x41,0x3F,0x20,0x1F,0x18,0x07
.DB 0x0F,0x00,0x03,0x00,0x00,0x00,0x00,0x00
.DB 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
.DB 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
.DB 0x00,0x00,0x00,0x1F,0x1F,0xFF,0xE0,0xFF
.DB 0x00,0xFF,0x3F,0xC0,0xFF,0x00,0xE0,0x00
.DB 0x80,0x00,0x80,0xFF,0x3F,0xFF,0x1C,0xFF
.DB 0x8E,0x7F,0xFF,0x00,0xFF,0x00,0x00,0x00
.DB 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
.DB 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
.DB 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
.DB 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
.DB 0x00,0x00,0x00,0xFE,0xFE,0xFF,0x83,0xFF
.DB 0x41,0xFF,0xFC,0x03,0xFF,0x00,0x00,0x00
.DB 0x00,0x00,0x00,0xFF,0xFF,0xFF,0x00,0xFF
.DB 0x01,0xFE,0xFF,0x00,0xFF,0x00,0x00,0x00
.DB 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
.DB 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
.DB 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
.DB 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
.DB 0x0C,0x00,0x00,0x00,0x00,0xF8,0xFA,0xFC
.DB 0x82,0xFF,0xC0,0xFF,0xF0,0x0F,0x3E,0x01
.DB 0x01,0x1F,0x7E,0xFF,0x00,0xFF,0x0F,0xF0
.DB 0xFF,0x00,0xFC,0x00,0xC0,0x00,0x00,0x00
.DB 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
.DB 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
.DB 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
.DB 0x40,0x80,0xD0,0xE0,0x14,0xF8,0x01,0xFE
.DB 0x81,0x7F,0x20,0x1F,0x18,0x07,0x06,0x01
.DB 0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00
.DB 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
.DB 0x01,0x03,0x04,0x0F,0x10,0x3F,0x43,0xFC
.DB 0x8F,0xF0,0x3E,0xC0,0xF8,0x00,0xE0,0x00
.DB 0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00
.DB 0x40,0x80,0xD0,0xE0,0x60,0xF8,0x3A,0xFC
.DB 0x95,0x7E,0x69,0x1F,0x34,0x0F,0x0E,0x03
.DB 0x07,0x09,0x03,0x1C,0x13,0x3C,0x47,0xF8
.DB 0x0E,0xF0,0x3C,0xC0,0xF0,0x00,0xC0,0x00
.DB 0x00,0x03,0x03,0x0F,0x0C,0x1F,0x10,0x7F
.DB 0x63,0xFC,0x87,0x78,0x5C,0xA0,0x38,0xC0
.DB 0x10,0xE0,0x98,0xF0,0xCC,0x78,0xC3,0x3E
.DB 0x60,0x1F,0x10,0x0F,0x0C,0x03,0x03,0x00
.DB 0x00,0x00,0x00,0x00,0x02,0x12,0x12,0x1B
.DB 0x0B,0x0F,0x07,0x0F,0x0E,0x3F,0x32,0xFF
.DB 0xC3,0xFC,0x0E,0xF0,0x38,0xC0,0xE0,0x00
.DB 0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00
.DB 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
.DB 0xC0,0x80,0x50,0xE0,0x0C,0xF8,0x04,0xFF
.DB 0xC3,0x3F,0x31,0x0F,0x0E,0x01,0x03,0x00
.DB 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
.DB 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
.DB 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
.DB 0x00,0x00,0x00,0x00,0x00,0x1F,0x1F,0x7F
.DB 0x60,0xFF,0x80,0xFF,0x1F,0xE0,0xF0,0x00
.DB 0x80,0xF8,0xF8,0xFF,0x47,0xFF,0x80,0x7F
.DB 0xF8,0x07,0x1E,0x01,0x07,0x00,0x00,0x00
.DB 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
.DB 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
.DB 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
.DB 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
.DB 0xFF,0xEE,0x80,0xFF,0x7F,0xFF,0x80,0xFF
.DB 0x00,0xFF,0x3F,0xC0,0xFF,0x0E,0xFF,0xEE
.DB 0x77,0xEE,0x73,0xEE,0x71,0xEE,0x71,0xEE
.DB 0x71,0xEE,0x71,0xEE,0x71,0xEE,0x71,0xEE
.DB 0x11,0xEE,0x11,0xEE,0x59,0xEE,0x7D,0xEE
.DB 0x7F,0xEE,0x77,0xEE,0x73,0xEE,0x71,0xEE
.DB 0x71,0xEE,0x71,0xEE,0x31,0xEE,0x11,0xEE
.DB 0x51,0xEE,0x79,0xEE,0x7D,0xEE,0x77,0xEE
.DB 0x73,0xEE,0x71,0xEE,0x71,0xEE,0x49,0xF6
.DB 0x49,0xF6,0x05,0xFA,0x05,0xFA,0x82,0x7C
.DB 0x82,0x7C,0x82,0x7C,0x44,0x38,0x44,0x38
.DB 0x28,0x10,0x28,0x10,0x10,0x00,0x00,0x00
.DB 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
.DB 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
.DB 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
.DB 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
.DB 0x00,0x00,0x00,0xF0,0xF8,0xF0,0x84,0xF8
.DB 0x42,0xFC,0xE7,0x18,0xEF,0x10,0x3C,0x00
.DB 0x00,0x03,0x03,0xFF,0xFC,0xFF,0x00,0xFF
.DB 0x07,0xF8,0xFF,0x00,0xFC,0x00,0x00,0x00
.DB 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
.DB 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
.DB 0x00,0x01,0x01,0x01,0x01,0x03,0x02,0x03
.DB 0x02,0x07,0x04,0x0F,0x08,0x1F,0x1D,0x7E
.DB 0x67,0xFC,0x87,0xF8,0x1E,0xE0,0x78,0x80
.DB 0xE0,0x00,0x80,0x00,0x00,0x00,0x00,0x00
.DB 0x4E,0xF0,0x6E,0xF0,0x7E,0xF0,0x5E,0xF0
.DB 0x4E,0xF0,0x0E,0xF0,0x4E,0xF0,0x4E,0xF0
.DB 0x4E,0xF0,0x4C,0xF0,0x4C,0xF0,0x4C,0xF0
.DB 0x4C,0xF0,0x9C,0xE0,0x98,0xE0,0x98,0xE0
.DB 0x98,0xE0,0x38,0xC0,0x30,0xC0,0x70,0x80
.DB 0x60,0x80,0xE0,0x00,0xC0,0x00,0x80,0x00
.DB 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
.DB 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
.DB 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
.DB 0x00,0x20,0x00,0x10,0x08,0x19,0x04,0x0E

;Window_tiles:
;   .DB 0xFF,0xFF,0x80,0xFF,0xBF,0xC0,0xA0,0xC0
;   .DB 0xAF,0xC0,0xA8,0xC0,0xA8,0xC0,0xA8,0xC0
;   .DB 0xFF,0xFF,0x00,0xFF,0xFF,0x00,0x00,0x00
;   .DB 0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00
;   .DB 0xFF,0xFF,0x01,0xFF,0xFD,0x03,0x05,0x03
;   .DB 0xF5,0x03,0x15,0x03,0x15,0x03,0x15,0x03
;   .DB 0xA8,0xC0,0xA8,0xC0,0xA8,0xC0,0xA8,0xC0
;   .DB 0xA8,0xC0,0xA8,0xC0,0xA8,0xC0,0xA8,0xC0
;   .DB 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
;   .DB 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
;   .DB 0x15,0x03,0x15,0x03,0x15,0x03,0x15,0x03
;   .DB 0x15,0x03,0x15,0x03,0x15,0x03,0x15,0x03
;   .DB 0xA8,0xC0,0xA8,0xC0,0xA8,0xC0,0xAF,0xC0
;   .DB 0xA0,0xC0,0xBF,0xC0,0x80,0xFF,0xFF,0xFF
;   .DB 0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0x00
;   .DB 0x00,0x00,0xFF,0x00,0x00,0xFF,0xFF,0xFF
;   .DB 0x15,0x03,0x15,0x03,0x15,0x03,0xF5,0x03
;   .DB 0x05,0x03,0xFD,0x03,0x01,0xFF,0xFF,0xFF

Background_tiles:
.DB 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
.DB 0x00,0x3F,0x00,0xF8,0x00,0x00,0x00,0x00
.DB 0x00,0x00,0x00,0x00,0x00,0x01,0x01,0x00
.DB 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
.DB 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
.DB 0x03,0x00,0x03,0x03,0x07,0x0F,0x0F,0x30
.DB 0x78,0x84,0x00,0x00,0xC0,0x00,0x00,0x80
.DB 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
.DB 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
.DB 0xFF,0xFC,0xFC,0xE3,0xC0,0xBF,0x00,0x87
.DB 0x00,0x00,0x00,0x00,0x03,0x00,0x00,0x06
.DB 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
.DB 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
.DB 0xC3,0x3D,0x07,0xFB,0x1E,0xE1,0x38,0xFF
.DB 0xC0,0x38,0x00,0x00,0x00,0x00,0x00,0x00
.DB 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
.DB 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
.DB 0xFE,0xF9,0xF0,0x8C,0x60,0x90,0x00,0xC0
.DB 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
.DB 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
.DB 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
.DB 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
.DB 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
.DB 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
.DB 0x00,0xAA,0x00,0x77,0x00,0xFF,0x00,0xFF
.DB 0x00,0xFF,0x20,0xC7,0xF3,0x63,0xFF,0x00
.DB 0x00,0xAA,0x00,0x77,0x00,0xFF,0x00,0xFF
.DB 0x00,0xFF,0x03,0xFC,0xFF,0xFF,0xFF,0x00
.DB 0x00,0xAA,0x00,0x77,0x01,0xFE,0x06,0xF8
.DB 0x3F,0xC0,0xFF,0x00,0xFF,0xFF,0xFF,0x00
.DB 0x00,0xAA,0x00,0x77,0x00,0xFF,0x00,0x0F
.DB 0x01,0x00,0x80,0x00,0xE0,0x80,0xFF,0x00
.DB 0x00,0xAA,0x00,0x77,0x00,0xFF,0x08,0xF3
.DB 0xF0,0x00,0x38,0x00,0x1F,0x0F,0xFF,0x00
.DB 0x00,0xAA,0x00,0x77,0x00,0xFF,0x00,0xFF
.DB 0x00,0x3F,0x00,0x00,0xFF,0xFF,0xFF,0x00
.DB 0x00,0xAA,0x00,0x77,0x00,0xFF,0x00,0xFF
.DB 0x00,0xFF,0x00,0xFF,0xFC,0xFF,0xFC,0x00
.DB 0x00,0xAA,0x00,0x77,0x00,0xFF,0xF0,0xCF
.DB 0xFC,0xFB,0xFF,0xC0,0xE0,0x00,0x00,0x00
.DB 0x00,0xAA,0x00,0x77,0x00,0xFF,0x00,0xFF
.DB 0x00,0xFF,0x0F,0xF0,0x3F,0x3F,0xFF,0x00
.DB 0x00,0xAA,0x00,0x77,0x00,0xFF,0x00,0xFF
.DB 0x0F,0xF0,0xFF,0x00,0xFF,0xE0,0xFF,0x00
.DB 0x00,0xAA,0x01,0x76,0x07,0xF8,0x3E,0xC0
.DB 0xFC,0x00,0xF8,0x00,0xFF,0x07,0xFF,0x00
.DB 0x30,0x82,0xE0,0x01,0xE0,0x00,0xF0,0x00
.DB 0x78,0x00,0x3E,0x00,0xFF,0xF8,0xFF,0x00
.DB 0x00,0xAA,0x00,0x77,0x00,0xFF,0x00,0x1F
.DB 0x00,0x00,0x00,0x00,0xF0,0xF0,0xFF,0x00
.DB 0x00,0xAA,0x00,0x77,0x00,0xFF,0x00,0xFF
.DB 0x02,0xFC,0x0E,0x00,0x04,0x00,0xFF,0x00
.DB 0x00,0xAA,0x00,0x77,0x00,0xFF,0x00,0xFF
.DB 0x00,0xFF,0x00,0x7F,0x00,0x00,0xFF,0x00
.DB 0x00,0xAA,0x00,0x77,0x00,0xFF,0x00,0xFF
.DB 0x00,0xFF,0x08,0xF3,0x7F,0x87,0xFF,0x00
.DB 0x00,0xAA,0x00,0x77,0x00,0xFF,0x00,0xFF
.DB 0x01,0xFE,0x1F,0xE0,0xFF,0xF0,0xFF,0x00
.DB 0x00,0xAA,0x00,0x77,0x08,0xF3,0x70,0x81
.DB 0xF0,0x00,0xF8,0x00,0xFF,0x3F,0xFF,0x00
.DB 0x00,0xAA,0x00,0x77,0x00,0xFF,0x10,0xE3
.DB 0xF8,0x01,0x13,0x00,0xFF,0xE0,0xFF,0x00
.DB 0x00,0xAA,0x00,0x77,0x02,0xFC,0x0F,0xF0
.DB 0x3F,0xC0,0xFF,0x00,0xFF,0x03,0xFF,0x00
.DB 0x00,0xAA,0x00,0x77,0x80,0x3F,0xC0,0x03
.DB 0xE0,0x00,0xFC,0x00,0xFF,0xFF,0xFF,0x00
.DB 0x00,0xAA,0x00,0x77,0x00,0xFF,0x00,0xFF
.DB 0x00,0x1F,0x00,0x00,0xFF,0xFF,0xFF,0x00
.DB 0x00,0xAA,0x00,0x77,0x00,0xFF,0x00,0xFF
.DB 0x00,0xFF,0x00,0xFF,0xFC,0xFF,0xFC,0x00

.DB 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
.DB 0x00,0x55,0x00,0x00,0x00,0xAA,0x00,0xDD
.DB 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
.DB 0x00,0x55,0x00,0x00,0x87,0x29,0xFF,0x03
.DB 0x1F,0xA3,0xBF,0x5D,0x7F,0xBE,0x7F,0xBF
.DB 0xFF,0x7F,0xFF,0xDE,0xFF,0x00,0x00,0x00
.DB 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
.DB 0x06,0x51,0xF0,0xC0,0xF8,0xE2,0xF8,0xE5
.DB 0xFF,0xC0,0xFF,0xDD,0xFF,0xFF,0xFF,0xFF
.DB 0xFF,0xF3,0xFF,0x0F,0xFF,0x00,0x00,0x00
.DB 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
.DB 0x00,0x55,0x00,0x00,0xE4,0x6A,0x71,0xAC
.DB 0xF0,0xCA,0xF8,0x87,0xFC,0x7B,0xFF,0xD9
.DB 0xFF,0xC0,0xFF,0xE3,0xFF,0x00,0x00,0x00
.DB 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
.DB 0x00,0x55,0x0F,0x0E,0x7F,0x7F,0xFE,0xE1
.DB 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
.DB 0x00,0x55,0xE3,0xE0,0xFF,0x5E,0x80,0x5D
.DB 0x00,0xAA,0x00,0x77,0x00,0xFF,0x00,0xFF
.DB 0x03,0xFC,0x7F,0x8F,0xFF,0x00,0x00,0x00
.DB 0x00,0xAA,0x06,0x71,0x00,0xFF,0x00,0xFF
.DB 0x60,0xFF,0xF8,0x77,0xFF,0x00,0x00,0x00
.DB 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
.DB 0x00,0x55,0x00,0x00,0xF0,0x6A,0xFC,0xB5
.DB 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
.DB 0x00,0x55,0x7E,0x2C,0xEF,0x45,0x00,0xDD
.DB 0x00,0xAA,0x00,0x77,0x30,0xCF,0x00,0xFF
.DB 0x03,0xFD,0x1F,0xE7,0x7F,0x80,0x00,0x00
.DB 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
.DB 0x00,0x55,0x00,0x00,0xC0,0x2A,0x60,0x9D
.DB 0x03,0xAA,0x7F,0x33,0xFF,0x77,0xFF,0xFF
.DB 0xFF,0xFF,0xFF,0x72,0xFF,0x00,0x00,0x00
.DB 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
.DB 0x00,0x55,0x00,0x00,0x74,0xEA,0xFE,0xF5
.DB 0xF8,0x7A,0xFF,0xBB,0xFF,0xC7,0xFF,0xF7
.DB 0xFF,0x9F,0xFF,0x73,0xFF,0x00,0x00,0x00
.DB 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
.DB 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
.DB 0x00,0xAA,0x80,0x77,0x80,0x7F,0xC0,0xBF
.DB 0xF8,0xC7,0xFC,0xFB,0xFF,0x00,0x00,0x00

Footer_tiles:
   .DB 0xFF,0x00,0xFF,0x7C,0xFF,0x66,0xFF,0x66
   .DB 0xFF,0x7C,0xFF,0x60,0xF1,0x60,0xF1,0x00
   .DB 0xFF,0x00,0xFF,0xF9,0xFF,0xCD,0xFF,0xCD
   .DB 0xFF,0xF9,0xFF,0xCD,0xFF,0xCD,0xFF,0x00
   .DB 0xFF,0x00,0xFF,0xF9,0xFF,0x83,0xF7,0xE3
   .DB 0xF7,0x81,0xFF,0x80,0xFF,0xFB,0xFF,0x00
   .DB 0xFF,0x00,0xFF,0xF3,0xFF,0x06,0xFF,0xE7
   .DB 0xFF,0xF3,0xFF,0x30,0xFF,0xE7,0xFF,0x00
   .DB 0xF0,0x00,0xF0,0xE0,0xF0,0x00,0xF0,0xC0
   .DB 0xF0,0xE0,0xF0,0x60,0xF0,0xC0,0xE0,0x00
   .DB 0x07,0x00,0x0F,0x03,0x0F,0x06,0x0F,0x07
   .DB 0x0F,0x03,0x0F,0x00,0x0F,0x07,0x0F,0x00
   .DB 0xFF,0x00,0xFF,0xEF,0xFF,0x03,0xF7,0xC3
   .DB 0xF7,0xE3,0xF7,0x63,0xF7,0xC3,0xE7,0x00
   .DB 0xFF,0x00,0xFF,0xCF,0xFF,0x19,0xBF,0x19
   .DB 0xBF,0x1F,0xBF,0x19,0xBF,0x19,0xBF,0x00
   .DB 0xFF,0x00,0xFF,0x3E,0xFF,0xB3,0xFF,0xB3
   .DB 0xFF,0xBE,0xFF,0xB3,0xFF,0xB3,0xFF,0x00
   .DB 0xFF,0x00,0xFF,0x7E,0xFF,0x18,0xBC,0x18
   .DB 0xBC,0x18,0xBC,0x18,0xBC,0x18,0xBC,0x00

.globl _spr64_cmpline,_spr64_table,_bspr_state,_spr64_timer
.globl _vbl_end,_hbl_end,scroll_update_time,_oam_dma_rom,_sort,_owe_panmode
.globl scroll_update,_vblank_timer

_rompage    = #0xF0
_rampage    = #0xF1
_vbimode    = #0xF2
_hbimode    = #0xF3

_rscx       = #0xFA
_rscy       = #0xFB

oam_dma     = _oam_dma_rom  ;0xFF80

bspr_rampage            = 6 ; reserve D0 - D7

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


;----------------------------------------------------------------------------
_bspr_int_real::                       ; Battle Interrupt Routine
;----------------------------------------------------------------------------

;. Initialize the battle sprites interrupt ._._._._._._._._._._._._._._._._.

   LD    A, #2                         ; Set 64 sprites mode
   LDH   (_hbimode), A                 ;

   LD    A, (_bspr_window)             ; Set next gradient line to line 112
   LDH   (P_WY), A                     ; Set window
   DEC   A                             ;
   LDH   (P_LYC), A                    ;

   LD    HL, #0xFF40                   ; HL -> LCD controller
   SET   1, (HL)                       ; Sprites ON
   RES   0, (HL)                       ; Sprites ON

   LD    A, #_bspr_spr64split1_page    ; DMA previous frame's sprite data
   CALL  _oam_dma_rom_b                ;

;   LD    A, (_owe_panmode)
;   OR    A
;   CALL  NZ, scroll_update

   CALL  _animation

   LD    A, #bspr_rampage              ; Map D000-DFFF to bspr's ram page
   LDH   (P_SVBK), A                   ;

;. Find weighted average of scrollx ._._._._._._._._._._._._._._._._._._._._.

   LD    HL, #_bspr_destx              ; HL -> bspr_destx
   LD    A, (HLI)                      ; BC = (bspr_destx)
   LD    C, A                          ;
   LD    A, (HLI)                      ;
   LD    B, A                          ; HL -> bspr_scrollx

   LD    A, (HLI)                      ; DE = (bspr_scrollx)
   LD    D, (HL)                       ;
   LD    E, A                          ;

   LD    H, D                          ; HL = bspr_scrollx
   LD    L, E                          ;
   ADD   HL, HL                        ; HL = bspr_scrollx * 2
   ADD   HL, DE                        ; HL = bspr_scrollx * 3
   ADD   HL, HL                        ; HL = bspr_scrollx * 6
   ADD   HL, DE                        ; HL = bspr_scrollx * 7

   LD    A, L                          ; HL = bspr_scrollx * 7 + bspr_destx
   ADD   A, C                          ;
   LD    L, A                          ;
   LD    A, H                          ;
   ADC   A, B                          ;
   LD    H, A                          ;

   LD    A, E                          ; A = (bspr_scrollx < bspr_dest) * 7
   SUB   C                             ; (signed comparison)
   RL    B                             ;
   CCF                                 ;
   RR    B                             ;
   LD    A, D                          ;
   RLA                                 ;
   CCF                                 ;
   RRA                                 ;
   SBC   A, B                          ;
   SBC   A, A                          ;
   AND   #7                            ;

   ADD   A, L                          ; DE = bspr_scrollx * 7 + bspr_destx
   LD    E, A                          ;    + (bspr_scrollx < bspr_dest) * 7
   ADC   A, H                          ;
   SUB   E                             ;
   LD    D, A                          ;

   LD    A, (_bspr_quake)              ;
   OR    A                             ;
   JR    Z, no_quakex                  ;
   CALL  randfuncasm                   ;
   AND   #0x0F                         ;
   SUB   #0x08                         ;
no_quakex:

   LD    C, A

   LD    HL, #_bspr_scrollx            ; HL = bspr_scrollx
   ADD   A, (HL)                       ;
   SUB   #80                           ;
   LDH   (P_SCX), A                    ; (LCDSCX) = scrollx - 80
   LDH   (_rscx), A

   LD    A, E                          ; DA = (bspr_scrollx * 7 + bspr_destx
   SRA   D                             ;    + (bspr_scrollx < bspr_dest) * 7)
   RRA                                 ;    >> 3
   SRA   D                             ;
   RRA                                 ;
   SRA   D                             ;
   RRA                                 ;

   LD    (HLI), A                      ; (bspr_scrollx) = DA
   LD    (HL), D                       ;

   LD    H, D
   LD    L, A
   LD    A, C
   ADD   A, A
   SBC   A, A
   LD    B, A
   ADD   HL, BC
   LD    D, H
   LD    A, L

   SUB   #88                           ; DE = -(bspr_scrollx) + 87
   CPL                                 ;
   LD    E, A                          ;
   LD    A, D                          ;
   SBC   A, #0                         ;
   CPL                                 ;
   LD    D, A                          ;

   LD    HL, #_bspr_negscrollx16 + 1   ; (bspr_negscrollx) = AE
   LD    (HLD), A                      ; 
   LD    (HL), E                       ;

   LD    A, E                          ; AE = -(bspr_scrollx) + 83
   SUB   #4                            ;
   LD    E, A                          ;
   LD    A, D                          ;
   SBC   A, #0                         ;

   LD    HL, #_bspr_negscrollx8 + 1    ; (bspr_negscrollx) = AE
   LD    (HLD), A                      ;
   LD    (HL), E                       ;

;. Find weighted average of scrolly ._._._._._._._._._._._._._._._._._._._._.

   LD    HL, #_bspr_desty              ; HL -> bspr_desty
   LD    A, (HLI)                      ; BC = (bspr_desty)
   LD    C, A                          ;
   LD    A, (HLI)                      ;
   LD    B, A                          ; HL -> bspr_scrolly

   LD    A, (HLI)                      ; DE = (bspr_scrolly)
   LD    D, (HL)                       ;
   LD    E, A                          ;

   LD    H, D                          ; HL = bspr_scrolly
   LD    L, E                          ;
   ADD   HL, HL                        ; HL = bspr_scrolly * 2
   ADD   HL, DE                        ; HL = bspr_scrolly * 3
   ADD   HL, HL                        ; HL = bspr_scrolly * 6
   ADD   HL, DE                        ; HL = bspr_scrolly * 7

   LD    A, L                          ; HL = bspr_scrolly * 7 + bspr_desty
   ADD   A, C                          ;
   LD    L, A                          ;
   LD    A, H                          ;
   ADC   A, B                          ;
   LD    H, A                          ;

   LD    A, E                          ; A = (bspr_scrolly < bspr_dest) * 7
   SUB   C                             ;
   RL    B                             ;
   CCF                                 ;
   RR    B                             ;
   LD    A, D                          ;
   RLA                                 ;
   CCF                                 ;
   RRA                                 ;
   SBC   A, B                          ;
   SBC   A, A                          ;
   AND   #7                            ;

   ADD   A, L                          ; DE = bspr_scrolly * 7 + bspr_desty
   LD    E, A                          ;    + (bspr_scrolly < bspr_dest) * 7
   ADC   A, H                          ;
   SUB   E                             ;
   LD    D, A                          ;

   LD    A, (_bspr_quake)              ;
   OR    A                             ;
   JR    Z, no_quakey                  ;
   CALL  randfuncasm                   ;
   AND   #0x0F                         ;
   SUB   #0x08                         ;
no_quakey:

   LD    C, A

   LD    HL, #_bspr_scrolly            ; HL = bspr_scrolly
   ADD   A, (HL)                       ;
;   SUB   #72                           ;
;   SUB   #56
   SUB   #60
   LDH   (P_SCY), A                 ;
   LDH   (_rscy), A

   LD    A, E                          ; DA = (bspr_scrolly * 7 + bspr_desty
   SRA   D                             ;    + (bspr_scrolly < bspr_dest) * 7)
   RRA                                 ;    >> 3
   SRA   D                             ;
   RRA                                 ;
   SRA   D                             ;
   RRA                                 ;

   LD    (HLI), A                      ; (bspr_scrolly) = DE
   LD    (HL), D                       ;

   LD    H, D
   LD    L, A
   LD    A, C
   ADD   A, A
   SBC   A, A
   LD    B, A
   ADD   HL, BC
   LD    D, H
   LD    A, L

;   SUB   #80                           ;
;   SUB   #64
   SUB   #68
   CPL                                 ;
   LD    E, A                          ;
   LD    A, D                          ;
   SBC   A, #0                         ;
   CPL                                 ;
   LD    D, A                          ;

   LD    HL, #_bspr_negscrolly16 + 1   ; (bspr_negscrolly) = DE
   LD    (HLD), A                      ;
   LD    (HL), E                       ;

   LD    A, #3                         ; AE = -(bspr_scrolly) + 90
   ADD   A, E                          ;
   LD    E, A                          ;
   ADC   A, D                          ;
   SUB   E                             ;

   LD    HL, #_bspr_negscrolly8 + 1    ; (bspr_negscrolly) = DE
   LD    (HLD), A                      ;
   LD    (HL), E                       ;

;. Create OAM data for wingdings ._._._._._._._._._._._._._._._._._._._._._.

   LD    C, #0x00                      ; C -> 64 sprites table

; format for wingdings:
;  active - 1 byte  A000000B             A = active, B = fixed
;  x      - 2 bytes XXXXXXXX XXXXXXXX    X = absolute X coordinate
;  y      - 2 bytes YYYYYYYY YYYYYYYY    Y = absolute Y coordinate

   LD    HL, #_bspr_8array             ; HL -> array of wingdings
   LD    E, #32                        ; 32 wingdings

int_bspr_8loop:
   LD    A, E
   CP    #27
   JR    C, int_bspr_8noflicker

   INC   L
   LD    A, (_vblank_timer)
   AND   #0x01
   JR    Z, int_bspr_8inactive_nopush
   DEC   L

int_bspr_8noflicker:
   LD    A, (HLI)                      ; A = wingdings.active
   ADD   A, A                          ;
   JR    NC, int_bspr_8inactive_nopush ;
   JR    Z, int_bspr_8relative         ; Branch where necessary

   LD    B, #_bspr_spr64table_page     ; BC -> 64 sprites table

   LD    A, (HLI)                      ; D = wingdings.x(l)
   LD    D, A                          ;
   INC   L                             ;

   LD    A, (HLI)                      ; spr.y = wingdings.y(l)
   LD    (BC), A                       ;
   INC   L                             ;
   INC   C                             ;

   LD    A, D                          ; spr.x = D
   LD    (BC), A                       ;
   INC   C                             ;
   INC   C                             ;
   INC   C                             ;

   JR    int_bspr_8active              ; Finished processing this loop

int_bspr_8relative:
   PUSH  HL                            ; Save HL

   LD    A, (_bspr_negscrollx8)        ; D = X coordinate of sprite
   ADD   A, (HL)                       ;
   LD    D, A                          ;
   INC   L                             ;

   LD    A, (_bspr_negscrollx8 + 1)    ; Check if sprite X is onscreen
   ADC   A, (HL)                       ;
   JR    NZ, int_bspr_8inactive        ;
   INC   L                             ;

   LD    A, (_bspr_negscrolly8)        ; B = Y coordinate of sprite
   ADD   A, (HL)                       ;
   LD    B, A                          ;
   INC   L                             ;

   LD    A, (_bspr_negscrolly8 + 1)    ; Check if sprite Y is onscreen
   ADC   A, (HL)                       ;
   JR    NZ, int_bspr_8inactive        ;
   INC   L                             ;

   LD    A, D                          ; Check if sprite X is inbounds
   CP    #160+15                       ;
   JR    NC, int_bspr_8inactive        ;

   LD    A, B                          ; Check if sprite Y is inbounds
   CP    #144+15                       ;
   JR    NC, int_bspr_8inactive        ; A = Y coordinate of sprite

   LD    B, #_bspr_spr64table_page     ; BC -> 64 sprites table

   INC   A                             ; spr.y = A
   LD    (BC), A                       ;
   INC   C                             ;

   LD    A, D                          ; spr.x = D
   LD    (BC), A                       ;
   INC   C                             ;
   INC   C                             ;
   INC   C                             ;
   POP   AF                            ; Trash saved value (unneeded now)

   JR    int_bspr_8active              ; Finished processing this loop

int_bspr_8inactive:
   POP   HL                            ; Retrieve saved value (for ptr)

int_bspr_8inactive_nopush:
   LD    B, #_bspr_spr64table_page     ; BC -> 64 sprites table

   INC   L                             ; HL -> next wingding entry
   INC   L                             ;
   INC   L                             ;
   INC   L                             ;

   XOR   A                             ; spr.y = 0
   LD    (BC), A                       ;
   INC   C                             ;
   INC   C                             ;
   INC   C                             ;
   INC   C                             ; BC -> next sprite entry

int_bspr_8active:
   DEC   E                             ; loop next
   JP    NZ, int_bspr_8loop            ;

;. Create OAM data for sprites ._._._._._._._._._._._._._._._._._._._._._.

   LD    DE, #_sort_table
   LD    HL, #_sort_table_ptr
   LD    A, E
   LD    (HLI), A
   LD    (HL), D
   LD    H, D
   LD    L, E
   XOR   A
   LD    B, #0x10
_sprarryclr:
   LD    (HLI), A
   LD    (HLI), A
   DEC   B
   JR    NZ, _sprarryclr

   LD    HL, #_bspr_16array            ; HL = bspr_16array
   LD    A, #16                        ;
   LD    C, #0x00

int_bspr_loop:
   PUSH  AF

   LD    A, (HLI)
   OR    A
   JR    Z, int_sprite_inactive_nopush

   PUSH  HL

; active     - 1 byte
; x          - 2 bytes
; z          - 1 byte
; y          - 2 bytes
; frames_bak - 2 bytes
; frames     - 2 bytes * 16 entries

   LD    A, (_bspr_negscrollx16)       ; E = X coordinate of sprite
   ADD   A, (HL)                       ;
   LD    E, A                          ;
   INC   HL                            ;

   LD    A, (_bspr_negscrollx16 + 1)   ; If zero then sprite is active
   ADC   A, (HL)                       ;
   JR    NZ, int_sprite_inactive       ;
   INC   HL                            ;

   LD    D, (HL)                       ; D = Z coordinate of sprite
   INC   HL                            ;

   LD    A, (_bspr_negscrolly16)       ; B = sorting Y coordinate of sprite
   ADD   A, (HL)                       ;
   LD    B, A                          ;
   INC   HL                            ;

   PUSH  AF                            ; D = displayed Y coordinate of sprite
   SUB   D                             ;
   LD    D, A                          ;
   POP   AF                            ;

   LD    A, (_bspr_negscrolly16 + 1)   ; If zero then sprite is active
   ADC   A, (HL)                       ;
   JR    NZ, int_sprite_inactive       ;
   INC   HL

   LD    A, E                          ; check if inbounds
   CP    #160+15                       ;
   JR    NC, int_sprite_inactive       ;

   LD    A, D                          ; check if inbounds
   CP    #144+15                       ;
   JR    NC, int_sprite_inactive       ;

   PUSH  HL                            ; HL -> OAM
   LD    H, #_presort_table_page       ;
   LD    L, C                          ;

   INC   D
   LD    A, D                          ; y coordinate
   LD    (HLI), A                      ;
   LD    A, E                          ;
   SUB   #7                            ;
   LD    (HLI), A                      ; x coordinate
   INC   L                             ;
   INC   L                             ;

   LD    A, D                          ; y coordinate
   LD    (HLI), A                      ;
   INC   E                             ;
   LD    (HL), E                       ; x coordinate

   LD    HL, #_sort_table_ptr
   LD    A, (HLI)
   LD    D, (HL)
   LD    E, A

   LD    A, B
   LD    (DE), A
   INC   E
   LD    A, C
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

   JR    int_sprite_active

int_sprite_inactive:
   POP   HL
int_sprite_inactive_nopush:
   LD    A, #9
   ADD   A, L
   LD    L, A
   ADC   A, H
   SUB   L
   LD    H, A                          ; point HL to next entry

   LD    A, #8
   ADD   A, C
   LD    C, A                          ; point C to next entry

;   LD    B, #_presort_table_page
;
;   XOR   A
;   LD    (BC), A
;   INC   C
;   INC   C
;   INC   C
;   INC   C
;   LD    (BC), A
;   INC   C
;   INC   C
;   INC   C
;   INC   C
int_sprite_active:
   POP   AF
   DEC   A
   JP    NZ, int_bspr_loop

   CALL  _sort_bspr

   CALL  _project_bspr

   CALL  _bicon_setstuff               ; battle icons

   CALL  spr64_buildtable

_bspr_intdone:
   LDH   A, (_rampage)
   LDH   (P_SVBK), A                   ; Map D000-DFFF to ram page

   LD    A, (_owe_panmode)
   OR    A
   CALL  NZ, scroll_update_time

_quit_bspr_int:
   RET

;_spr64hblank_int::
;   LD    A, #bspr_rampage
;   LDH   (P_SVBK), A                              ; Map D000-DFFF to ram page
;
;   LD    A, #24                                 ; 2; A=15
;sprloop:
;   DEC   A                                      ; 1;
;   JR    NZ, sprloop                            ; 2;
;   LD    A, (HL)                                ; 2;
;
;   LD    A, (_spr64_table)
;   XOR   #7
;   LD    (_spr64_table), A
;
;   CALL  oam_dma
;
;   LDH   A, (_rampage)
;   LDH   (P_SVBK), A                              ; Map D000-DFFF to ram page
;
;   LD    A, #1
;   LDH   (_hbimode), A
;
;   LD    A, #111                       ; Set next gradient line to line 112
;   LDH   (P_LYC), A             ;
;
;   JP    _hbl_end


;###################################################
;# Set up (sort) the two sprite tables
;# Input:
;#	_largetab_buf_pointer =	 pointer to 256 byte 
;#				64 sprite table to be
;#				split into two 40 sprite
;#				table (algined to $XX00)
;#	_sprite_table0 = 160 byte table
;#	_sprite_table1 = 160 byte table
;# _spr64_nextcmpline = storage for mean
;###################################################

;----------------------------------------------------------------------------
;[ spr64_buildtable ]
;----------------------------------------------------------------------------
spr64_buildtable:
;   LD    BC, #_bspr_spr64table
;
;   LD    HL, #0x0000
;   LD    D, H
;_get_mean:
;   LD    A, (BC)
;   LD    E, A
;   SUB   A, #0x01
;   SBC   A, A
;   AND   #88
;   ADD   HL, DE
;   LD    E, A
;   ADD   HL, DE
;   INC   C
;   INC   C
;   INC   C
;   INC   C
;   JR    NZ, _get_mean
;   LD    A, L
;   AND   #0xC0
;   OR    H                             ; divide by 64
;   RLCA
;   RLCA
;   SUB   #16

   LD    A, (_spr64_cmpline)
   LDH   (P_LYC), A

   LD    B, #72  ;60

checkforspriteonlineagain:
   LD    HL, #_bspr_spr64table         ; HL -> spr64table
checkforspriteonline:
   LD    A, (HLI)                      ; Get Y coordinate
   SUB   B                             ; Is it < current line?
   JR    C, nospriteonline
   SUB   #16                           ; Is it > current line?
   JR    NC, nospriteonline

   ADD   A, B
   LD    B, A                          ; Is compare line above 56?
   CP    #56 ;36
   JR    NC, checkforspriteonlineagain

   JR    checkdownwards

nospriteonline:
   INC   L
   INC   L
   INC   L
   JR    NZ, checkforspriteonline

   LD    A, B
   DEC   A
   DEC   A

   JR    donecheckingsprite
checkdownwards:

   LD    B, #73 ;61
checkforspriteonlineagainb:
   LD    HL, #_bspr_spr64table         ; HL -> spr64table
checkforspriteonlineb:
   LD    A, (HLI)                      ; Get Y coordinate
   SUB   B                             ; Is it < current line?
   JR    C, nospriteonlineb
   CP    #16                           ; Is it > current line?
   JR    NC, nospriteonlineb

   ADC   A, B
   LD    B, A                           ; Is compare line below 88?
   CP    #88 ;84
   JR    C, checkforspriteonlineagainb

   LD    A, (_spr64_table)             ; multisync code
   RRCA
   SBC   A, A
   AND   #31

   ADD   A, #53

   JR    donecheckingsprite

nospriteonlineb:
   INC   L
   INC   L
   INC   L
   JR    NZ, checkforspriteonlineb

   LD    A, B
   DEC   A
   DEC   A
donecheckingsprite:
   LD    (_spr64_cmpline), A

   LD    HL, #_bspr_spr64table
   LD    B, A                          ; B=cmp-16
   LD    C, #0x00                      ; C->table0
   ADD   A, #16
   LD    D, A                          ; D=cmp
   LD    E, #0x00                      ; E->table1

_sort_main_loop:
   LD    A, (HL)
   OR    A
   JR    Z, _no_load_table0            ; if 0, no copy

   CP    D
   JR    NC, _no_load_table0           ; if carry, it is <= mean, so copy

   LD    A, C
   CP    #160
   JR    Z, _no_load_table0

   PUSH  BC
   LD    B, #_bspr_spr64split1_page

   LD    A, (HLI)
   LD    (BC), A
   INC   C
   LD    A, (HLI)
   LD    (BC), A
   INC   C
   LD    A, (HLI)
   LD    (BC), A
   INC   C
   LD    A, (HLD)
   LD    (BC), A
   INC   C
   POP   AF
   LD    B, A

   DEC   L
   DEC   L

_no_load_table0:                       ; sprite to table1
   LD    A, (HL)
   CP    B
   JR    C, _no_load_table1

   LD    A, E
   CP    #160
   JR    Z, _no_load_table1

   PUSH  DE
   LD    A, (_spr64_table)
   LD    D, A

   LD    A, (HLI)
   LD    (DE), A
   INC   E
   LD    A, (HLI)
   LD    (DE), A
   INC   E
   LD    A, (HLI)
   LD    (DE), A
   INC   E
   LD    A, (HL)
   LD    (DE), A
   INC   E
   POP   AF
   LD    D, A

   JR    _skiptable

_no_load_table1:
   INC   L
   INC   L
   INC   L
_skiptable:
   INC   L
   JR    NZ, _sort_main_loop

; clear the rest of the tables now
   LD    A, #160
   SUB   C
   JR    Z, _skip_clear0               ; if C >= 160, can't clear any entries
   LD    H, #_bspr_spr64split1_page
   LD    L, C                          ; HL = BC
   RRCA
   RRCA                                ; A = (160 - C)/4
   LD    C, A
   XOR   A
_clear_table0:
   LD    (HLI), A
   INC   L
   INC   L
   INC   L
   DEC   C
   JR    NZ, _clear_table0

_skip_clear0:
   LD    A, #160
   SUB   E
   RET   Z                             ; if E = 160, can't clear any entries

   RRCA                                ; A = (160 - E)/4
   RRCA
   LD    C, A

   LD    A, (_spr64_table)
   LD    H, A
   LD    L, E                          ; HL = DE

   XOR   A
_clear_table1:
   LD    (HLI), A
   INC   L
   INC   L
   INC   L
   DEC   C
   JR    NZ, _clear_table1
   RET

_sort_bspr:
   LD    DE, #_sort_table
   LD    H, D
   LD    L, E

   JP    _sort

_project_bspr:
   LD    B, #_presort_table_page
   LD    HL, #_bspr_spr64table + 0x80
   LD    DE, #_sort_table

_project_loop:
   LD    A, (DE)
   OR    A
   JR    Z, blankrestoftable
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
   JR    NZ, _project_loop
   RET

blankrestoftable:
   XOR   A
blankrestoftable_loop:
   LD    (HLI), A
   LD    (HLI), A
   LD    (HLI), A
   LD    (HL), A
   INC   L
   JR    NZ, blankrestoftable_loop
   RET

.globl   _timer_mask,_owe_tileset_rompage,_owe_tileset_pointer
.globl   _owe_counter,_owe_sx,_owe_sy,_owe_mask,_owe_status
.globl   map_signed_cp,map_at
.globl   _owe_destx,_owe_scrollx,_owe_desty,_owe_scrolly
.globl   Update_VRAM,_init_sxy

_ram_page_owe           = 6            ; reserve from D8 - DF

_scr_pos    = 0xEF


_owe_spritearray        = 0xD800
_owe_neg_scrollx        = 0xD8D0
_owe_neg_scrolly        = 0xD8D2
_owe_sort_table_ptr     = 0xD8D4

_owe_sort_table         = 0xD8D8

; D884 - D8FF = free

_owe_spr_presort_page   = 0xD9

_ram_page_grd = 5

_grad_table = 0xD480          ; 288 bytes

;----------------------------------------------------------------------------
_int_owe_real::                        ; Overworld Engine Interrupt
;----------------------------------------------------------------------------

;. Initialize the overworld engine interrupt ._._._._._._._._._._._._._._._.

; for TILT engine
;
;   EI
;
;   LD    A, #6
;   LDH   (_hbimode), A
;   LD    A, #1
;   LDH   (P_LYC), A
;
;;;;;;;;;;;;;

   LD    A, #0xC0                      ;
   CALL  _oam_dma_rom_b                ;

   CALL  _animation

;   LD    A, (_owe_panmode)
;   OR    A
;   CALL  NZ, scroll_update

;;;;;;;;;;;;; ending
;   LDH   A, (P_LCDC)
;   LD    A, #0b11000111
;   LDH   (P_LCDC), A
;;;;;;;;;;;;;

   LD    HL, #0xFF40                   ; HL -> LCD controller
   SET   1, (HL)                       ; Sprites ON

   LD    A, (_owe_status)
   OR    A
   JP    Z, _vbl_owe_end

   LD    A, #_ram_page_owe             ; Map D000-DFFF to bspr's ram page
   LDH   (P_SVBK), A                   ;

;. Do x scrolling ._._._._._._._._._._._._._._._._._._._._._._._._._._._._.

   LD    HL, #_owe_destx               ; HL -> destx
   LD    A, (HLI)                      ; BC = (destx)
   LD    C, A                          ;
   LD    A, (HLI)                      ;
   LD    B, A                          ; HL -> scrollx

   LD    A, (_bspr_quake)              ;
   OR    A                             ;
   JR    Z, no_quakex_owe              ;
   CALL  randfuncasm                   ;
   AND   #0x06                         ;
   SUB   #0x04                         ;
no_quakex_owe:

   LD    D, A
   LD    A, (HLI)                      ; (HL)E = (scrollx)
   LD    E, A
   ADD   A, D                          ;
   LDH   (P_SCX), A                    ;
   LDH   (_rscx), A                    ;
   LD    D, (HL)                       ; (LCDSCX) = scrollx

   SUB   C                             ; is scrollx == destx?
   LD    L, A                          ;
   LD    A, D                          ;
   SBC   A, B                          ;
   LD    H, A                          ;
   OR    L                             ;
   JR    Z, owe_no_scroll_x            ; BC = scrollx - destx

;   CALL  owe_cp_bc_de                  ;

   CALL  map_signed_cp

   SBC   A, A                          ; BA = 1 (NC) or -1 (CY)
;   PUSH  DE
;   PUSH  BC
;   PUSH  AF
;   CALL  C, _rotate_l
;   POP   AF
;   PUSH  AF
;   CALL  NC, _rotate_r
;   POP   AF
;   POP   BC
;   POP   DE

   LD    B, A                          ;
   ADD   A, #_scr_pos
   LD    C, A
   LDH   A, (C)

   LD    C, A
   LD    A, (_vblank_timer)
   LD    H, A
   LD    A, (_timer_mask)
   AND   H
   SUB   #1
   SBC   A, A
   LD    L, A
   AND   B
   LD    B, A
   LD    A, L
   AND   C

;   OR    #0x01                         ;
;   ADD   A, A                          ;

   ADD   A, E                          ; AE += BA
   LD    E, A                          ;
;   LD    A, (_owe_mask)                ;
;   LD    C, A                          ;
   LD    A, D                          ;
   ADC   A, B                          ;

;   AND   C                             ;
   LD    D, A                          ;

   LD    HL, #_owe_scrollx + 1         ; HL -> destx
   LD    (HLD), A                      ; (scrollx) = AE
   LD    (HL), E                       ; HL -> scrollx(h)

owe_no_scroll_x:
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
   JR    Z, no_quakey_owe              ;
   CALL  randfuncasm                   ;
   AND   #0x06                         ;
   SUB   #0x04                         ;
no_quakey_owe:

   LD    D, A                          ;
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
   JR    Z, owe_no_scroll_y            ;

   CALL  map_signed_cp

;   CALL  owe_cp_bc_de                  ;

   SBC   A, A                          ; BA = 1 (NC) or -1 (CY)
;   PUSH  DE
;   PUSH  BC
;   PUSH  AF
;   CALL  C, _rotate_u
;   POP   AF
;   PUSH  AF
;   CALL  NC, _rotate_d
;   POP   AF
;   POP   BC
;   POP   DE

   LD    B, A                          ;
   ADD   A, #_scr_pos
   LD    C, A
   LDH   A, (C)

   LD    C, A
   LD    A, (_vblank_timer)
   LD    H, A
   LD    A, (_timer_mask)
   AND   H
   SUB   #1
   SBC   A, A
   LD    L, A
   AND   B
   LD    B, A
   LD    A, L
   AND   C

;   OR    #0x01                         ;
;   ADD   A, A                          ;

   ADD   A, E                          ; DE += BA
   LD    E, A                          ;
;   LD    A, (_owe_mask)                ;
;   LD    C, A                          ;
   LD    A, D                          ;
   ADC   A, B                          ;

;   AND   C                             ;
   LD    D, A                          ;

   LD    HL, #_owe_scrolly + 1         ; HL -> desty
   LD    (HLD), A                      ; (scrolly) = DE
   LD    (HL), E                       ;

owe_no_scroll_y:

;----------------------------------
   LD    A, (_endingmode)
   OR    A
   JP    Z, not_ending

   LDH   A, (_hbimode)
   CP    #8
   JR    Z, yes_ending
   OR    A
   JP    NZ, not_ending

;   JR    init_credits

; init credits

;yes_ending:
;   LD    A, (ending_done)
;   CP    #7
;   JR    NZ, ending_notfinished

;   EI

;   LD    A, #78
;   LD    (_endingcount), A
;
;   XOR   A
;   LD    (ending_done), A
;
;   LD    A, #1
;   LD    (_endingmode), A
;
;   JR    reinit_credits
;
;init_credits:
   LD    A, #8
   LDH   (_hbimode), A

yes_ending:
;
;   LDH   A, (_rscy)
;   LD    (_newoffset), A
;
;   LD    A, #1
;   LD    (_endingmode), A
;
;   LD    HL, #_topline
;   LD    A, #28         ; top = 28
;   LD    (HLI), A       ; bottom = 115
;   LD    (HL), #115
;
;   LD    HL, #_endingcount
;   XOR   A
;   LD    (HLI), A
;   LD    (HLI), A
;   LD    (HLI), A
;
;   LD    HL, #_pfs_targetbgr
;   LD    A, (HLI)
;   LD    E, A
;   LD    D, (HL)
;   PUSH  DE
;   XOR   A
;   LD    (HLD), A
;   LD    (HL), #0x01
;   LD    DE, #endingblackpal-56
;   CALL  _asm_setpal
;   POP   DE
;   LD    HL, #_pfs_targetbgr
;   LD    A, E
;   LD    (HLI), A
;   LD    (HL), D
;
;   LD    HL, #0x9C00
;   LD    BC, #1024
;   LD    A, #0x04
;   CALL  _STIR_VRAM
;
;reinit_credits:
;   CALL  _drawglob_real

ending_notfinished:
   XOR   A
   LD    (_ending_local), A

   LD    A, #-144
   LDH   (P_SCY), A
   LD    A, #0b11101101
   LDH   (P_LCDC), A
   LD    A, #167
   LDH   (P_WX), A

   LD    A, (_newoffset)
   LD    (_offset), A

   LD    HL, #_topline
   LD    A, (HLI)
   DEC   A
   LDH   (P_LYC), A
   ADD   (HL)
   CPL
   LD    H, #0xFF
   LD    L, A
   LD    BC, #143
   ADD   HL, BC
   SRA   H
   RR    L
   ADD   HL, DE
   LD    D, H
   LD    E, L
   LD    A, L

   LD    HL, #_newoffset
   LD    (HLI), A

   LD    A, (_endingmode)
   SUB   #2
   JR    Z, ending_stuff2

   LD    HL, #_endingcount
   LD    A, (HL)
   CP    #138
   JR    Z, ending_done4
   INC   (HL)
   CP    #90
   JP    C, ending_done3

   INC   HL
   INC   (HL)
   LD    A, (HL)
   CP    #3
   SBC   A, A
   AND   (HL)
   LD    (HL), A
   JR    NZ, ending_skip_move_top
   LD    HL, #_topline
   DEC   (HL)
ending_skip_move_top:
   LD    HL, #_bottomline
   DEC   (HL)
   JP    ending_done3

ending_done4:
;   LD    A, #2
;   LD    (_endingmode), A
   LD    A, #80
   LDH   (P_WY), A
   LD    A, #159
   LDH   (P_WX), A

   JR    ending_done3

endingblackpal:
.DW 0b000000000000000,0b000000000011111,0b111110101000000,0b111111111111111
; gba
.DW 0b000000000000000,0b000110100011111,0b111111011100010,0b111111111111111

ending_stuff2:
   LD    HL, #_endingobj_mem
   LD    BC, #0x0700

ending_loop2:
   LD    A, (HLI)
   DEC   A
   JR    Z, _ending_delay
   DEC   A
   JR    NZ, ending_stuff3
   LD    A, (HL)
   INC   A
   CP    #144
   JR    Z, _ending_fellout2
   LD    (HLI), A
   JR    ending_loop_join2
_ending_delay:
   DEC   (HL)
   JR    NZ, ending_loop_join3
_ending_fellout:
   XOR   A
   JR    _ending_felljoin
_ending_fellout2:
   LD    A, #128
_ending_felljoin:
   LD    (HLD), A
   INC   (HL)
   INC   HL
ending_loop_join3:
   INC   HL
   JR    ending_loop_join2
ending_loop_finished:
;   LD    A, (ending_done)
;   INC   A
;   LD    (ending_done), A
;   JR    ending_loop_join3
ending_stuff3:
   DEC   A
   JR    Z, _ending_delay
   DEC   A
   JR    NZ, ending_loop_join3 ;finished
   LD    A, (HL)
   INC   A
   CP    #144
   JR    Z, _ending_fellout
   LD    (HLI), A
   SUB   #123
   JR    C, ending_loop_join2
   CP    #20
   JR    NC, ending_loop_join2
   PUSH  HL
   LD    L, A
   LD    A, C
   RRCA
   RRCA
   RRCA
   OR    L
   LD    L, A
   LD    H, #0x9C
   LD    A, #0x04
   RST   ldi_v_a
   POP   HL
ending_loop_join2:
   INC   C
   DEC   B
   JR    NZ, ending_loop2

ending_done3:
   EI

not_ending:
;----------------------------------
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
   LDH   (P_SVBK), A                   ;

   LDH   A, (P_VBK)
   PUSH  AF

   LD    A, #2 ;(_owe_scrollspeed)
_scrollupdate:
   PUSH  AF
   
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
   JR    NZ, _scrollupdate

   POP   AF
   LDH   (P_VBK), A

;. Draw sprites ._._._._._._._._._._._._._._._._._._._._._._._._._._._._.

   LD    A, #_ram_page_owe             ; Map D000-DFFF to bspr's ram page
   LDH   (P_SVBK), A                   ;

   LD    DE, #_owe_sort_table          ;
   LD    HL, #_owe_sort_table_ptr      ;
   LD    A, E                          ;
   LD    (HLI), A                      ;
   LD    (HL), D                       ;

   XOR   A                             ;
   LD    H, D                          ;
   LD    L, E                          ;
   LD    B, #10                        ;
owe_sprarryclr:
   LD    (HLI), A                      ;
   LD    (HLI), A                      ;
   LD    (HLI), A                      ;
   LD    (HLI), A                      ;
   DEC   B                             ;
   JR    NZ, owe_sprarryclr            ;

   LD    HL, #_owe_spritearray         ; HL = bspr_16array
   LD    A, #20                        ;
   LD    C, #0x00                      ;

;self:
;   JR    self

oweint_sprset_loop:
   PUSH  AF

   LD    A, (HLI)
   OR    A
   JR    Z, oweint_spr_inactive_nopush

;   RLCA
;   JR    Z, oweint_spr_inactive_nopush
;   JR    NC, not_ghost
;
;   LD    A, (_vblank_timer)
;   AND   #0x01
;   JR    Z, oweint_spr_inactive_nopush
not_ghost:
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
   JR    NZ, oweint_spr_inactive       ;
   INC   HL                            ;

;   INC   HL                            ; B = sorting Y coordinate of sprite
;   LD    A, (_owe_neg_scrolly)         ;
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

   LD    D, (HL)                       ; D = Z coordinate of sprite
   INC   HL                            ;

   LD    A, (_owe_neg_scrolly)         ; B = sorting Y coordinate of sprite
   ADD   A, (HL)                       ;
   LD    B, A                          ;
   INC   HL                            ;

   PUSH  AF
   SUB   D                             ; D = displayed Y coordinate of sprite
   LD    D, A                          ;
   POP   AF                            ; save flags

   LD    A, (_owe_neg_scrolly + 1)     ; If zero then sprite is active
   ADC   A, (HL)                       ;
   JR    NZ, oweint_spr_inactive       ;
   INC   HL

   LD    A, E                          ; check if inbounds
   CP    #160+15                       ;
   JR    NC, oweint_spr_inactive       ;

   LD    A, D                          ; check if inbounds
   CP    #144+15                       ;
   JR    NC, oweint_spr_inactive       ;

   PUSH  HL                            ; HL -> OAM
   LD    H, #_owe_spr_presort_page     ;
   LD    L, C                          ;

   INC   D                             ;
   LD    A, D                          ;
   LD    (HLI), A                      ; y coordinate
   LD    A, E                          ;
   SUB   #7                            ;
   LD    (HLI), A                      ; x coordinate
   INC   L                             ;
   INC   L                             ;

   LD    A, D                          ;
   LD    (HLI), A                      ; y coordinate
   INC   E                             ;
   LD    (HL), E                       ; x coordinate

   LD    HL, #_owe_sort_table_ptr
   LD    A, (HLI)
   LD    D, (HL)
   LD    E, A

   LD    A, B
   LD    (DE), A
   INC   E
   LD    A, C
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

   JR    oweint_spr_active

oweint_spr_inactive:
   POP   HL
oweint_spr_inactive_nopush:
   LD    A, #9
   ADD   A, L
   LD    L, A
   ADC   A, H
   SUB   L
   LD    H, A                          ; point HL to next entry

   LD    A, #8
   ADD   A, C
   LD    C, A                          ; point C to next entry

oweint_spr_active:
   POP   AF
   DEC   A

   JP    NZ, oweint_sprset_loop

   CALL  _sort_owe

   CALL  _project_owe

;. Finish Interrupt ._._._._._._._._._._._._._._._._._._._._._._._._._._.

;   LD    A, (cur_page_ram)             ; Map D000-DFFF to bspr's ram page
   LDH   A, (_rampage)
   LDH   (P_SVBK), A                   ;

_vbl_owe_end:
;   LD    A, (_owe_panmode)
;   OR    A
;   CALL  NZ, scroll_update_time

_quit_vbl_int:
   RET

_sort_owe:
   LD    DE, #_owe_sort_table
   LD    H, D
   LD    L, E
   JP    _sort

_project_owe:
   LD    B, #_owe_spr_presort_page
   LD    HL, #0xC000
   LD    DE, #_owe_sort_table

_project_loop2:
   LD    A, (DE)
   OR    A
   JR    Z, _blankrestoftable
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
   JR    NZ, _project_loop2
   RET

_blankrestoftable:
_blankrestoftable_loop:
   XOR   A
   LD    (HLI), A
   LD    (HLI), A
   LD    (HLI), A
   LD    (HLI), A
   LD    A, L
   CP    #0xA0
   JR    NZ, _blankrestoftable_loop
   RET

;.db 0xde,0xad,0xbe,0xef
;.dw 0x0040
;.db 11
;   JP    _vblank_int
;   .db   0,0,0,0,0
;   JP    _hblank_int
;
;.db 0xde,0xad,0xbe,0xef
;.dw 0x0000
;.db 6
;vram_copy:
;   LD    (HL), A
;   CP    (HL)
;   JR    NZ, vram_copy
;   INC   HL
;   RET

;---------------------------------------------------------------------------

rolluptop:

;roll table top (upward)

.db  65, 62, 60, 59, 57, 56, 55, 54, 53, 52, 51, 50
.db  49, 48, 47, 46, 45, 43, 42, 41, 40, 38, 36, 34

rollupbottom:

;roll table bottom (upward)

.db   0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11
.db  12, 13, 14, 15, 16, 17, 18, 20, 21, 22, 24, 27

rolldowntop:

;roll table top (downward)

.db -11,-13,-15,-17,-18,-19,-20,-22,-23,-24,-25,-26
.db -27,-28,-29,-30,-31,-32,-33,-34,-36,-37,-39,-42

rolldownbottom:

;roll table bottom (downward)

.db  -4, -1,  1,  2,  3,  5,  6,  7,  8,  9, 10, 11
.db  12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23

_scl_roll_real::
   PUSH  BC
   LDHL  SP, #8

   LD    D, #0                         ; always 0

   LD    B, D                          ; start at entry 0

   LD    A, #60                        ; C=72-radius-frame
   SUB   (HL)                          ;
   LD    C, A                          ;

   LD    HL, #_scl_table               ; save results in scl_table

   LD    A, B                          ;
_sr_toploop:
   SUB   C                             ; E=line-start
   LD    E, A                          ;

   ADD   A, A                          ; Is the current line<the starting?
   LD    A, B                          ; If so, send current line
   JR    C, _sr_donetop                ;

   LD    A, E                          ; Is the current line>=the ending?
   ADD   A, #-24                       ; If so, out of bounds
   SBC   A, A                          ;
   JR    C, _sr_donetop                ;

   PUSH  HL                            ;

   LD    HL, #rolluptop                ; Find value from upper table
   ADD   HL, DE                        ;
   LD    A, C                          ;
   ADD   A, (HL)                       ;
   CP    #72                           ; Keep if in range
   JR    C, _sr_donetoppop             ;

   LD    HL, #rollupbottom             ; Find value from lower table
   ADD   HL, DE                        ;
   LD    A, C                          ;
   ADD   A, (HL)                       ;
   CP    #72                           ; Keep if in range
   JR    C, _sr_donetoppop             ;

   LD    A, #0xFF                      ; Out of our range
_sr_donetoppop:
   POP   HL                            ;

_sr_donetop:
   LD    (HLI), A                      ; Save table entry

   INC   B                             ; Next entry
   LD    A, B                          ;
   CP    #72                           ;
   JR    C, _sr_toploop                ;

   LD    A, #120                       ; C=72-radius+frame
   SUB   C                             ;
   LD    C, A                          ;

   LD    A, B                          ;
_sr_bottomloop:
   SUB   C                             ; E=line-start
   LD    E, A                          ;

   ADD   A, A                          ; Is the current line<the starting?
   SBC   A, A                          ; If so, out of bounds
   JR    C, _sr_donebottom             ;

   LD    A, E                          ; Is the current line>=the ending?
   CP    #24                           ; If so, send current line
   LD    A, B                          ;
   JR    NC, _sr_donebottom            ;

   PUSH  HL                            ;

   LD    HL, #rolldowntop              ; Find value from upper table
   ADD   HL, DE                        ;
   LD    A, C                          ;
   ADD   A, (HL)                       ;
   CP    #72                           ; Keep if in range
   JR    NC, _sr_donebotpop            ;

   LD    HL, #rolldownbottom           ; Find value from lower table
   ADD   HL, DE                        ;
   LD    A, C                          ;
   ADD   A, (HL)                       ;
   CP    #72                           ; Keep if in range
   JR    NC, _sr_donebotpop            ;

   LD    A, #0xFF                      ; Out of our range
_sr_donebotpop:
   POP   HL                            ;

_sr_donebottom:
   LD    (HLI), A                      ; Save table entry

   INC   B                             ; Next entry
   LD    A, B                          ;
   CP    #144                          ;
   JR    C, _sr_bottomloop             ;

   LD    A, #0x01
   LD    (_scl_on), A

   POP   BC
   RET

_scl_init_real::
   PUSH  BC

   LD    B, #144
   XOR   A

   LD    HL, #_scl_table
_blahblah:
   LD    (HLI), A
   INC   A
   DEC   B
   JR    NZ, _blahblah

   LD    A, #1
   LDH   (P_VBK), A

   LD    A, #0x87
   LD    HL, #0x9FE0
   LD    B, #0x20

   CALL  _STIR_B_VRAM

   XOR   A
   LDH   (P_VBK), A

   LD    A, #0x13
   LD    HL, #0x9FE0
   LD    B, #0x20

   CALL  _STIR_B_VRAM

   CALL  _w4vbl_done

   LD    A, #0b11100111
   LDH   (P_LCDC), A

   LD    A, #0x05
   LDH   (_hbimode), A

   XOR   A
   LDH   (P_LYC), A

   POP   BC

   RET

_scl_hbl_real::
   PUSH  DE                            ;4
   PUSH  HL                            ;4

   LDH   A, (P_LY)                     ;3
   CP    #143                          ;2
   JR    NZ, _sly_notfinal             ;3/2
   XOR   A                             ;1
   LD    D, A                          ;1
   LD    E, A                          ;1
   LDH   (P_LYC), A                    ;3
   JR    _sly_final                    ;3
_sly_notfinal:
   LD    D, #0                         ;2
   LD    E, A                          ;1
   INC   A                             ;1
   LDH   (P_LYC), A                    ;3
   NOP                                 ;1
_sly_final:

   LD    HL, #_scl_table               ;3
   ADD   HL, DE                        ;2

   LD    A, (_scl_on)                  ;4
   OR    A                             ;1
   JR    Z, _sly_no_scl                ;3/2

   LD    A, (HL)                       ;2
   INC   A                             ;1
   JR    NZ, _sly_unblack              ;3/2

   LD    A, #252                       ;2
   SUB   E                             ;1
   PUSH  AF                            ;4

   LD    D, #0b11101111                ;2
   NOP                                 ;1

   JR    _sly_do_scl                   ;3

_sly_no_scl:
   LDH   A, (_rscy)                    ;3
   PUSH  AF                            ;4

   LD    D, #0b11100111                ;2
   NOP                                 ;1
   NOP                                 ;1

   JR    _sly_do_scl_d3                ;3
_sly_do_scl_d3:
   JR    _sly_do_scl                   ;3

_sly_unblack:
   LDH   A, (_rscy)                    ;3
   SUB   E                             ;1
   ADD   A, (HL)                       ;2
   PUSH  AF                            ;4

   LD    D, #0b11100111                ;2

_sly_do_scl:

   LD    A, (_owe_mode)                ;3
   OR    A                             ;1
   JR    NZ, _yes_scx                  ;3/2

   PUSH  HL                            ;4
   POP   HL                            ;3
   PUSH  HL                            ;4
   POP   HL                            ;3

;;;;;;;;;;;;;;
   PUSH  HL                            ;4
   POP   HL                            ;3
;;;;;;;;;;;;;;
   JR    _done_scl

_yes_scx:
   DEC   A                             ;1
   JR    NZ, _scx_1                    ;3/2
   LD    E, (HL)                       ;2
_scx_1:
   LD    A, D                          ;1
   INC   A                             ;1
   INC   A                             ;1
   AND   #0x03                         ;2
   ADD   A, E                          ;1
   SUB   #76                           ;2
   SRA   A                             ;2
   SRA   A                             ;2
   LD    E, A                          ;1
   LDH   A, (_rscx)                    ;3
   ADD   A, E                          ;1
   LDH   (P_SCX), A                    ;3

_done_scl:
   LD    A, D                          ;1
   LDH   (P_LCDC), A                   ;3
   POP   AF                            ;3
   LDH   (P_SCY), A                    ;3

   POP   HL                            ;3
   POP   DE                            ;3

   RET                                 ;4

;----------------------------------------------------------------------------

_bicon_setstuff::
   LD    A, (bicon_set_frame)
   OR    A
   JP    NZ, loadgraphicsthisframe

   LD    HL, #_bicon_mode
   LD    A, (HLI)
   OR    A
   RET   Z

   LD    A, (_bicon_count)
   OR    A
   LD    E, A
   JR    Z, no_bicon_move

   LD    A, (HLI)
   DEC   (HL)
   INC   HL
   JR    NZ, bicon_notdone
   LD    E, A

   ADD   A, (HL)
   JR    Z, bicon_notdone2
   XOR   A
   LD    (_bicon_mode), A

bicon_notdone2:
   LD    A, E
bicon_notdone:
   ADD   A, (HL)
   LD    (HLI), A
   LD    E, A
no_bicon_move:

   LD    D, #0
   LD    A, (_bicon_exp_ct)
   OR    A
   JR    Z, _bicon_continue

   LD    HL, #_bicon_exp_dx            ; change
   DEC   (HL)
   LD    A, (HLI)
   LD    C, A                          ; C=dx
   ADD   A, (HL)
   LD    (HLI), A
   LD    D, A                          ; D=expand
   DEC   (HL)
   CALL  Z, _biconexp
_bicon_continue:

   LD    HL, #_bspr_8array + 40        ; start at index 8 (8*5=40)

   LD    A, (_bicon_icon2)
   OR    A
   JR    NZ, finished_bicon

   LD    A, (_bicon_icon)
   LD    B, A

   DEC   B
   JR    Z, notthisicon1

   LD    A, #0x18
   SUB   D
   LD    C, A
   LD    A, #0x20
   CALL  setupicon

   DEC   B
   JR    Z, notthisicon2
   JR    thisicon1
notthisicon1:
   LD    C, #10
   ADD   HL, BC
thisicon1:
   LD    C, #0x28
   LD    A, #0x10
   SUB   D
   CALL  setupicon

   DEC   B
   JR    Z, notthisicon3
   JR    thisicon2
notthisicon2:
   LD    C, #10
   ADD   HL, BC
thisicon2:
   LD    C, #0x28
   LD    A, #0x30
   ADD   A, D
   CALL  setupicon

   DEC   B
   JR    Z, notthisicon4
   JR    thisicon3
notthisicon3:
   LD    C, #10
   ADD   HL, BC
thisicon3:
   LD    A, #0x38
   ADD   A, D
   LD    C, A
   LD    A, #0x20
   CALL  setupicon
notthisicon4:
   JR    finished_bicon2

finished_bicon:
   CALL  _bicon_clearloop

finished_bicon2:
   CALL  _finish_bicon

   LD    DE, #_bboxpal
   LD    BC, #0x0420                   ; C = frame
   LD    HL, #_bspr_spr64table + 32    ; start from slot 8
setupiconsloop2:
   CALL  setupicons_sub
   INC   DE
   DEC   B
   JR    NZ, setupiconsloop2
   RET

_finish_bicon:
   LD    A, (_bicon_icon)
   OR    A
   RET   Z

   LD    D, A
   ADD   A, A
   ADD   A, A
   ADD   A, D
   ADD   A, A

   LD    D, #0
   LD    E, A

   LD    H, D
   LD    A, (_bicon_iconxy+1)
   LD    L, A
   LD    B, D
   LD    C, A
   ADD   HL, HL
   ADD   HL, BC
   LD    C, #24
   ADD   HL, BC
   LD    A, L
   RR    H
   RRA
   RR    H
   RRA
   LD    (_bicon_iconxy+1), A
   LD    L, A

   PUSH  HL
   LD    H, D
   LD    A, (_bicon_iconxy)
   LD    L, A
   LD    C, A
   ADD   HL, HL
   ADD   HL, BC
   LD    C, #141
   ADD   HL, BC
   LD    A, L
   RR    H
   RRA
   RR    H
   RRA
   POP   BC
   LD    (_bicon_iconxy), A

;   LD    HL, #_bicon_iconxy+1          ; go to 140, 24
;   LD    A, (HL)
;   ADD   A, #24
;   RRA
;   LD    (HLD), A
;   LD    C, A
;   LD    A, (HL)
;   ADD   A, #141
;   RRA
;   LD    (HL), A

   LD    HL, #_bspr_8array + 40 - 10   ; start at index 8 (8*5=40)
   ADD   HL, DE

   LD    E, D

setupicon:
   PUSH  AF
   CALL  setupicon2
   POP   AF
   ADD   A, #8
setupicon2:
   ADD   A, E
   LD    (HL), #0xC0                   ; mode = 0xc0
   INC   L
   LD    (HLI), A                      ; low byte = x coord
   XOR   A                             ; high byte = 0
   LD    (HLI), A
   LD    A, C                          ; low byte = y coord
   LD    (HLI), A
   XOR   A                             ; high byte = 0
   LD    (HLI), A
   RET

_biconexp:
   LD    A, C
   OR    A
   RET   NZ

   LD    (_bicon_expanding), A

   LD    A, (_bicon_icon)
   OR    A
   JR    NZ, bicon_icon2set

   LD    (HL), #6
   LD    A, #4
   LD    (bicon_set_frame), A
   RET

bicon_icon2set:
   LD    (_bicon_icon2), A
   RET

setupicons_sub:
   CALL  setupicons_sub2
setupicons_sub2:
   INC   L
   INC   L
   LD    A, C
   INC   C
   ADD   A, A
   LD    (HLI), A
   LD    A, (DE)                       ; pal/right
   OR    #0x08
   LD    (HLI), A
   RET

;_bicon_mode::
;_bicon_addx:
;_bicon_count:
;_bicon_x::
;_bicon_expand::

turntable::
.db 63,62,61,60,60,59,58,58,57,57,56,55,55,54,53,53
.db 52,52,51,51,50,49,49,48,48,47,47,46,46,45,45,44
.db 44,43,43,42,42,41,41,40,40,40,39,39,38,38,37,37
.db 37,36,36,35,35,35,34,34,33,33,33,32,32,32,31,31
.db 31,30,30,29,29,29,28,28,28,28,27,27,27,26,26,26
.db 25,25,25,25,24,24,24,23,23,23,23,22,22,22,22,21
.db 21,21,21,20,20,20,20,19,19,19,19,19,18,18,18,18
.db 18,17,17,17,17,17,16,16,16,16,16,15,15,15,15,15
.db 15,14,14,14,14,14,13,13,13,13,13,13,13,12,12,12
.db 12,12,12,12,11,11,11,11,11,11,11,10,10,10,10,10
.db 10,10,10,9,9,9,9,9,9,9,9,9,8,8,8,8
.db 8,8,8,8,8,8,7,7,7,7,7,7,7,7,7,7
.db 7,6,6,6,6,6,6,6,6,6,6,6,6,5,5,5
.db 5,5,5,5,5,5,5,5,5,5,5,4,4,4,4,4
.db 4,4,4,4,4,4,4,4,4,4,4,4,3,3,3,3
.db 3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3

.db 111,109,108,107,106,105,103,102,101,100,99,98,97,96,95,94
.db 93,92,91,90,89,88,87,86,85,84,83,82,81,80,79,79
.db 78,77,76,75,74,74,73,72,71,70,70,69,68,67,67,66
.db 65,64,64,63,62,62,61,60,60,59,58,58,57,56,56,55
.db 55,54,53,53,52,52,51,50,50,49,49,48,48,47,47,46
.db 46,45,45,44,44,43,43,42,42,41,41,40,40,39,39,39
.db 38,38,37,37,36,36,36,35,35,34,34,34,33,33,33,32
.db 32,31,31,31,30,30,30,29,29,29,28,28,28,27,27,27
.db 27,26,26,26,25,25,25,24,24,24,24,23,23,23,23,22
.db 22,22,22,21,21,21,21,20,20,20,20,19,19,19,19,19
.db 18,18,18,18,17,17,17,17,17,16,16,16,16,16,16,15
.db 15,15,15,15,14,14,14,14,14,14,13,13,13,13,13,13
.db 13,12,12,12,12,12,12,11,11,11,11,11,11,11,11,10
.db 10,10,10,10,10,10,10,9,9,9,9,9,9,9,9,9
.db 8,8,8,8,8,8,8,8,8,7,7,7,7,7,7,7
.db 7,7,7,7,6,6,6,6,6,6,6,6,6,6,6,6

.db 159,157,155,153,152,150,148,147,145,144,142,141,139,137,136,135
.db 133,132,130,129,127,126,125,123,122,121,119,118,117,115,114,113
.db 112,110,109,108,107,106,105,103,102,101,100,99,98,97,96,95
.db 94,93,92,91,90,89,88,87,86,85,84,83,82,81,80,79
.db 79,78,77,76,75,74,73,73,72,71,70,70,69,68,67,67
.db 66,65,64,64,63,62,62,61,60,60,59,58,58,57,56,56
.db 55,54,54,53,53,52,52,51,50,50,49,49,48,48,47,47
.db 46,46,45,45,44,44,43,43,42,42,41,41,40,40,39,39
.db 39,38,38,37,37,36,36,36,35,35,34,34,34,33,33,33
.db 32,32,31,31,31,30,30,30,29,29,29,28,28,28,27,27
.db 27,26,26,26,26,25,25,25,24,24,24,24,23,23,23,23
.db 22,22,22,22,21,21,21,21,20,20,20,20,19,19,19,19
.db 19,18,18,18,18,17,17,17,17,17,16,16,16,16,16,16
.db 15,15,15,15,15,14,14,14,14,14,14,13,13,13,13,13
.db 13,12,12,12,12,12,12,12,11,11,11,11,11,11,11,11
.db 10,10,10,10,10,10,10,10,9,9,9,9,9,9,9,9

.db 255,252,249,246,244,241,238,236,233,231,228,226,223,221,218,216
.db 214,211,209,207,205,202,200,198,196,194,192,190,188,185,183,181
.db 180,178,176,174,172,170,168,166,164,163,161,159,157,156,154,152
.db 151,149,147,146,144,143,141,140,138,137,135,134,132,131,129,128
.db 127,125,124,122,121,120,118,117,116,115,113,112,111,110,108,107
.db 106,105,104,103,102,100,99,98,97,96,95,94,93,92,91,90
.db 89,88,87,86,85,84,83,82,81,81,80,79,78,77,76,75
.db 75,74,73,72,71,71,70,69,68,68,67,66,65,65,64,63
.db 63,62,61,60,60,59,58,58,57,57,56,55,55,54,53,53
.db 52,52,51,51,50,49,49,48,48,47,47,46,46,45,45,44
.db 44,43,43,42,42,41,41,40,40,40,39,39,38,38,37,37
.db 37,36,36,35,35,35,34,34,33,33,33,32,32,32,31,31
.db 31,30,30,29,29,29,28,28,28,28,27,27,27,26,26,26
.db 25,25,25,25,24,24,24,23,23,23,23,22,22,22,22,21
.db 21,21,21,20,20,20,20,19,19,19,19,19,18,18,18,18
.db 18,17,17,17,17,17,16,16,16,16,16,15,15,15,15,15

_real_getsin::  ; L = angle
   LD    A, L
   LD    H, #0x48
   LD    E, (HL)
   ADD   A, A
   LD    D, A
   RET   Z
   LD    H, A
   SBC   A, A
   LD    D, A
   RET   C
   SLA   H
   RET   NZ
   INC   D
   RET

;_real_getsin::
;   LD    DE, #0x0000
;   LD    H, #0x40
;   LD    L, A
;   ADD   A, A
;   JR    C, _trig_quad34
;   ADD   A, A
;   JR    Z, _trig_quadrantal1
;   JR    NC, _trig_quad1
;   LD    A, #128
;   SUB   L
;   LD    L, A
;_trig_quad1:
;   LD    E, (HL)
;   RET
;_trig_quad34:
;   RES   7, L
;   ADD   A, A
;   JR    Z, _trig_quadrantal3
;   JR    NC, _trig_quad3
;   LD    A, #128
;   SUB   L
;   LD    L, A
;_trig_quad3:
;   XOR   A
;   SUB   (HL)
;   LD    E, A
;   DEC   D
;   RET
;_trig_quadrantal1:
;   RET   C
;   INC   D
;   RET
;_trig_quadrantal3:
;   RET   C
;   DEC   D
;   RET


_real_getarctan::
   INC   BC
   LD    HL, #_tantable
   LD    DE, #0x807F
_atan_loop:
   LD    A, (HLI)
   PUSH  HL
   LD    H, (HL)
   LD    L, A
   ADD   HL, BC
   POP   HL
   JR    C, _atan_comp
   INC   HL
   DEC   E
   DEC   D
   JR    NZ, _atan_loop
   RET
_atan_comp:
   LD    A, E
   AND   #0x40
   ADD   A, A
   OR    E
   LD    E, A
   RET

_tantable:
.dw 0x0005,0x000B,0x0011,0x0018,0x001E,0x0024,0x002B,0x0031,0x0038,0x003F,0x0045,0x004C
.dw 0x0053,0x005A,0x0061,0x0069,0x0070,0x0078,0x007F,0x0087,0x0090,0x0098,0x00A1,0x00AA
.dw 0x00B3,0x00BC,0x00C6,0x00D1,0x00DB,0x00E7,0x00F2,0x00FF,0x010B,0x0119,0x0127,0x0136
.dw 0x0147,0x0158,0x016A,0x017E,0x0193,0x01AA,0x01C2,0x01DD,0x01FB,0x021C,0x0240,0x0269
.dw 0x0296,0x02CA,0x0305,0x034A,0x039C,0x03FD,0x0473,0x0505,0x05C2,0x06BC,0x081A,0x0A26
.dw 0x0D8D,0x1459,0x28BB,0x0000,0xD743,0xEBA5,0xF271,0xF5D8,0xF7E4,0xF942,0xFA3C,0xFAF9
.dw 0xFB8B,0xFC01,0xFC62,0xFCB4,0xFCF9,0xFD34,0xFD68,0xFD95,0xFDBE,0xFDE2,0xFE03,0xFE21
.dw 0xFE3C,0xFE54,0xFE6B,0xFE80,0xFE94,0xFEA6,0xFEB7,0xFEC8,0xFED7,0xFEE5,0xFEF3,0xFEFF
.dw 0xFF0C,0xFF17,0xFF23,0xFF2D,0xFF38,0xFF42,0xFF4B,0xFF54,0xFF5D,0xFF66,0xFF6E,0xFF77
.dw 0xFF7F,0xFF86,0xFF8E,0xFF95,0xFF9D,0xFFA4,0xFFAB,0xFFB2,0xFFB9,0xFFBF,0xFFC6,0xFFCD
.dw 0xFFD3,0xFFDA,0xFFE0,0xFFE6,0xFFED,0xFFF3,0xFFF9,0xFFFF

paled_tiles:
.DB 0b00000000,0b00000000
.DB 0b01000100,0b00000000
.DB 0b10101010,0b00000000
.DB 0b10101010,0b00000000
.DB 0b10101010,0b00000000
.DB 0b01000100,0b00000000
.DB 0b00000000,0b00000000
.DB 0b00000000,0b00000000

.DB 0b00000000,0b00000000
.DB 0b01000100,0b00000000
.DB 0b10101100,0b00000000
.DB 0b10100100,0b00000000
.DB 0b10100100,0b00000000
.DB 0b01001110,0b00000000
.DB 0b00000000,0b00000000
.DB 0b00000000,0b00000000

.DB 0b00000000,0b00000000
.DB 0b01001100,0b00000000
.DB 0b10100010,0b00000000
.DB 0b10100100,0b00000000
.DB 0b10101000,0b00000000
.DB 0b01001110,0b00000000
.DB 0b00000000,0b00000000
.DB 0b00000000,0b00000000

.DB 0b00000000,0b00000000
.DB 0b01001100,0b00000000
.DB 0b10100010,0b00000000
.DB 0b10100100,0b00000000
.DB 0b10100010,0b00000000
.DB 0b01001100,0b00000000
.DB 0b00000000,0b00000000
.DB 0b00000000,0b00000000

.DB 0b00000000,0b00000000
.DB 0b01000010,0b00000000
.DB 0b10100110,0b00000000
.DB 0b10101010,0b00000000
.DB 0b10101110,0b00000000
.DB 0b01000010,0b00000000
.DB 0b00000000,0b00000000
.DB 0b00000000,0b00000000

.DB 0b00000000,0b00000000
.DB 0b01001110,0b00000000
.DB 0b10101000,0b00000000
.DB 0b10101100,0b00000000
.DB 0b10100010,0b00000000
.DB 0b01001100,0b00000000
.DB 0b00000000,0b00000000
.DB 0b00000000,0b00000000

.DB 0b00000000,0b00000000
.DB 0b01000110,0b00000000
.DB 0b10101000,0b00000000
.DB 0b10101110,0b00000000
.DB 0b10101010,0b00000000
.DB 0b01001110,0b00000000
.DB 0b00000000,0b00000000
.DB 0b00000000,0b00000000

.DB 0b00000000,0b00000000
.DB 0b01001110,0b00000000
.DB 0b10100010,0b00000000
.DB 0b10100100,0b00000000
.DB 0b10100100,0b00000000
.DB 0b01000100,0b00000000
.DB 0b00000000,0b00000000
.DB 0b00000000,0b00000000

.DB 0b00000000,0b00000000
.DB 0b01001110,0b00000000
.DB 0b10101010,0b00000000
.DB 0b10101110,0b00000000
.DB 0b10101010,0b00000000
.DB 0b01001110,0b00000000
.DB 0b00000000,0b00000000
.DB 0b00000000,0b00000000

.DB 0b00000000,0b00000000
.DB 0b01001110,0b00000000
.DB 0b10101010,0b00000000
.DB 0b10101110,0b00000000
.DB 0b10100010,0b00000000
.DB 0b01001100,0b00000000
.DB 0b00000000,0b00000000
.DB 0b00000000,0b00000000

.DB 0b00000000,0b00000000
.DB 0b01000100,0b00000000
.DB 0b10101010,0b00000000
.DB 0b10101010,0b00000000
.DB 0b10101110,0b00000000
.DB 0b01001010,0b00000000
.DB 0b00000000,0b00000000
.DB 0b00000000,0b00000000

.DB 0b00000000,0b00000000
.DB 0b01001100,0b00000000
.DB 0b10101010,0b00000000
.DB 0b10101100,0b00000000
.DB 0b10101010,0b00000000
.DB 0b01001100,0b00000000
.DB 0b00000000,0b00000000
.DB 0b00000000,0b00000000

.DB 0b00000000,0b00000000
.DB 0b01000110,0b00000000
.DB 0b10101000,0b00000000
.DB 0b10101000,0b00000000
.DB 0b10101000,0b00000000
.DB 0b01000110,0b00000000
.DB 0b00000000,0b00000000
.DB 0b00000000,0b00000000

.DB 0b00000000,0b00000000
.DB 0b01001100,0b00000000
.DB 0b10101010,0b00000000
.DB 0b10101010,0b00000000
.DB 0b10101010,0b00000000
.DB 0b01001100,0b00000000
.DB 0b00000000,0b00000000
.DB 0b00000000,0b00000000

.DB 0b00000000,0b00000000
.DB 0b01001110,0b00000000
.DB 0b10101000,0b00000000
.DB 0b10101100,0b00000000
.DB 0b10101000,0b00000000
.DB 0b01001110,0b00000000
.DB 0b00000000,0b00000000
.DB 0b00000000,0b00000000

.DB 0b00000000,0b00000000
.DB 0b01001110,0b00000000
.DB 0b10101000,0b00000000
.DB 0b10101100,0b00000000
.DB 0b10101000,0b00000000
.DB 0b01001000,0b00000000
.DB 0b00000000,0b00000000
.DB 0b00000000,0b00000000

.DB 0b00000000,0b00000000
.DB 0b01000100,0b00000000
.DB 0b11001010,0b00000000
.DB 0b01001010,0b00000000
.DB 0b01001010,0b00000000
.DB 0b11100100,0b00000000
.DB 0b00000000,0b00000000
.DB 0b00000000,0b00000000

.DB 0b00000000,0b00000000
.DB 0b01000100,0b00000000
.DB 0b11001100,0b00000000
.DB 0b01000100,0b00000000
.DB 0b01000100,0b00000000
.DB 0b11101110,0b00000000
.DB 0b00000000,0b00000000
.DB 0b00000000,0b00000000

.DB 0b00000000,0b00000000
.DB 0b01001100,0b00000000
.DB 0b11000010,0b00000000
.DB 0b01000100,0b00000000
.DB 0b01001000,0b00000000
.DB 0b11101110,0b00000000
.DB 0b00000000,0b00000000
.DB 0b00000000,0b00000000

.DB 0b00000000,0b00000000
.DB 0b01001100,0b00000000
.DB 0b11000010,0b00000000
.DB 0b01000100,0b00000000
.DB 0b01000010,0b00000000
.DB 0b11101100,0b00000000
.DB 0b00000000,0b00000000
.DB 0b00000000,0b00000000

.DB 0b00000000,0b00000000
.DB 0b01000010,0b00000000
.DB 0b11000110,0b00000000
.DB 0b01001010,0b00000000
.DB 0b01001110,0b00000000
.DB 0b11100010,0b00000000
.DB 0b00000000,0b00000000
.DB 0b00000000,0b00000000

.DB 0b00000000,0b00000000
.DB 0b01001110,0b00000000
.DB 0b11001000,0b00000000
.DB 0b01001100,0b00000000
.DB 0b01000010,0b00000000
.DB 0b11101100,0b00000000
.DB 0b00000000,0b00000000
.DB 0b00000000,0b00000000

.DB 0b00000000,0b00000000
.DB 0b01000110,0b00000000
.DB 0b11001000,0b00000000
.DB 0b01001110,0b00000000
.DB 0b01001010,0b00000000
.DB 0b11101110,0b00000000
.DB 0b00000000,0b00000000
.DB 0b00000000,0b00000000

.DB 0b00000000,0b00000000
.DB 0b01001110,0b00000000
.DB 0b11000010,0b00000000
.DB 0b01000100,0b00000000
.DB 0b01000100,0b00000000
.DB 0b11100100,0b00000000
.DB 0b00000000,0b00000000
.DB 0b00000000,0b00000000

.DB 0b00000000,0b00000000
.DB 0b01001110,0b00000000
.DB 0b11001010,0b00000000
.DB 0b01001110,0b00000000
.DB 0b01001010,0b00000000
.DB 0b11101110,0b00000000
.DB 0b00000000,0b00000000
.DB 0b00000000,0b00000000

.DB 0b00000000,0b00000000
.DB 0b01001110,0b00000000
.DB 0b11001010,0b00000000
.DB 0b01001110,0b00000000
.DB 0b01000010,0b00000000
.DB 0b11101100,0b00000000
.DB 0b00000000,0b00000000
.DB 0b00000000,0b00000000

.DB 0b00000000,0b00000000
.DB 0b01000100,0b00000000
.DB 0b11001010,0b00000000
.DB 0b01001010,0b00000000
.DB 0b01001110,0b00000000
.DB 0b11101010,0b00000000
.DB 0b00000000,0b00000000
.DB 0b00000000,0b00000000

.DB 0b00000000,0b00000000
.DB 0b01001100,0b00000000
.DB 0b11001010,0b00000000
.DB 0b01001100,0b00000000
.DB 0b01001010,0b00000000
.DB 0b11101100,0b00000000
.DB 0b00000000,0b00000000
.DB 0b00000000,0b00000000

.DB 0b00000000,0b00000000
.DB 0b01000110,0b00000000
.DB 0b11001000,0b00000000
.DB 0b01001000,0b00000000
.DB 0b01001000,0b00000000
.DB 0b11100110,0b00000000
.DB 0b00000000,0b00000000
.DB 0b00000000,0b00000000

.DB 0b00000000,0b00000000
.DB 0b01001100,0b00000000
.DB 0b11001010,0b00000000
.DB 0b01001010,0b00000000
.DB 0b01001010,0b00000000
.DB 0b11101100,0b00000000
.DB 0b00000000,0b00000000
.DB 0b00000000,0b00000000

.DB 0b00000000,0b00000000
.DB 0b01001110,0b00000000
.DB 0b11001000,0b00000000
.DB 0b01001100,0b00000000
.DB 0b01001000,0b00000000
.DB 0b11101110,0b00000000
.DB 0b00000000,0b00000000
.DB 0b00000000,0b00000000

.DB 0b00000000,0b00000000
.DB 0b01001110,0b00000000
.DB 0b11001000,0b00000000
.DB 0b01001100,0b00000000
.DB 0b01001000,0b00000000
.DB 0b11101000,0b00000000
.DB 0b00000000,0b00000000
.DB 0b00000000,0b00000000

.DB 0b00000000,0b00000000
.DB 0b00000000,0b01000100
.DB 0b00000000,0b10101010
.DB 0b00000000,0b10101010
.DB 0b00000000,0b10101010
.DB 0b00000000,0b01000100
.DB 0b00000000,0b00000000
.DB 0b00000000,0b00000000

.DB 0b00000000,0b00000000
.DB 0b00000000,0b01000100
.DB 0b00000000,0b10101100
.DB 0b00000000,0b10100100
.DB 0b00000000,0b10100100
.DB 0b00000000,0b01001110
.DB 0b00000000,0b00000000
.DB 0b00000000,0b00000000

.DB 0b00000000,0b00000000
.DB 0b00000000,0b01001100
.DB 0b00000000,0b10100010
.DB 0b00000000,0b10100100
.DB 0b00000000,0b10101000
.DB 0b00000000,0b01001110
.DB 0b00000000,0b00000000
.DB 0b00000000,0b00000000

.DB 0b00000000,0b00000000
.DB 0b00000000,0b01001100
.DB 0b00000000,0b10100010
.DB 0b00000000,0b10100100
.DB 0b00000000,0b10100010
.DB 0b00000000,0b01001100
.DB 0b00000000,0b00000000
.DB 0b00000000,0b00000000

.DB 0b00000000,0b00000000
.DB 0b00000000,0b01000010
.DB 0b00000000,0b10100110
.DB 0b00000000,0b10101010
.DB 0b00000000,0b10101110
.DB 0b00000000,0b01000010
.DB 0b00000000,0b00000000
.DB 0b00000000,0b00000000

.DB 0b00000000,0b00000000
.DB 0b00000000,0b01001110
.DB 0b00000000,0b10101000
.DB 0b00000000,0b10101100
.DB 0b00000000,0b10100010
.DB 0b00000000,0b01001100
.DB 0b00000000,0b00000000
.DB 0b00000000,0b00000000

.DB 0b00000000,0b00000000
.DB 0b00000000,0b01000110
.DB 0b00000000,0b10101000
.DB 0b00000000,0b10101110
.DB 0b00000000,0b10101010
.DB 0b00000000,0b01001110
.DB 0b00000000,0b00000000
.DB 0b00000000,0b00000000

.DB 0b00000000,0b00000000
.DB 0b00000000,0b01001110
.DB 0b00000000,0b10100010
.DB 0b00000000,0b10100100
.DB 0b00000000,0b10100100
.DB 0b00000000,0b01000100
.DB 0b00000000,0b00000000
.DB 0b00000000,0b00000000

.DB 0b00000000,0b00000000
.DB 0b00000000,0b01001110
.DB 0b00000000,0b10101010
.DB 0b00000000,0b10101110
.DB 0b00000000,0b10101010
.DB 0b00000000,0b01001110
.DB 0b00000000,0b00000000
.DB 0b00000000,0b00000000

.DB 0b00000000,0b00000000
.DB 0b00000000,0b01001110
.DB 0b00000000,0b10101010
.DB 0b00000000,0b10101110
.DB 0b00000000,0b10100010
.DB 0b00000000,0b01001100
.DB 0b00000000,0b00000000
.DB 0b00000000,0b00000000

.DB 0b00000000,0b00000000
.DB 0b00000000,0b01000100
.DB 0b00000000,0b10101010
.DB 0b00000000,0b10101010
.DB 0b00000000,0b10101110
.DB 0b00000000,0b01001010
.DB 0b00000000,0b00000000
.DB 0b00000000,0b00000000

.DB 0b00000000,0b00000000
.DB 0b00000000,0b01001100
.DB 0b00000000,0b10101010
.DB 0b00000000,0b10101100
.DB 0b00000000,0b10101010
.DB 0b00000000,0b01001100
.DB 0b00000000,0b00000000
.DB 0b00000000,0b00000000

.DB 0b00000000,0b00000000
.DB 0b00000000,0b01000110
.DB 0b00000000,0b10101000
.DB 0b00000000,0b10101000
.DB 0b00000000,0b10101000
.DB 0b00000000,0b01000110
.DB 0b00000000,0b00000000
.DB 0b00000000,0b00000000

.DB 0b00000000,0b00000000
.DB 0b00000000,0b01001100
.DB 0b00000000,0b10101010
.DB 0b00000000,0b10101010
.DB 0b00000000,0b10101010
.DB 0b00000000,0b01001100
.DB 0b00000000,0b00000000
.DB 0b00000000,0b00000000

.DB 0b00000000,0b00000000
.DB 0b00000000,0b01001110
.DB 0b00000000,0b10101000
.DB 0b00000000,0b10101100
.DB 0b00000000,0b10101000
.DB 0b00000000,0b01001110
.DB 0b00000000,0b00000000
.DB 0b00000000,0b00000000

.DB 0b00000000,0b00000000
.DB 0b00000000,0b01001110
.DB 0b00000000,0b10101000
.DB 0b00000000,0b10101100
.DB 0b00000000,0b10101000
.DB 0b00000000,0b01001000
.DB 0b00000000,0b00000000
.DB 0b00000000,0b00000000

.DB 0b00000000,0b00000000
.DB 0b00000000,0b01000100
.DB 0b00000000,0b11001010
.DB 0b00000000,0b01001010
.DB 0b00000000,0b01001010
.DB 0b00000000,0b11100100
.DB 0b00000000,0b00000000
.DB 0b00000000,0b00000000

.DB 0b00000000,0b00000000
.DB 0b00000000,0b01000100
.DB 0b00000000,0b11001100
.DB 0b00000000,0b01000100
.DB 0b00000000,0b01000100
.DB 0b00000000,0b11101110
.DB 0b00000000,0b00000000
.DB 0b00000000,0b00000000

.DB 0b00000000,0b00000000
.DB 0b00000000,0b01001100
.DB 0b00000000,0b11000010
.DB 0b00000000,0b01000100
.DB 0b00000000,0b01001000
.DB 0b00000000,0b11101110
.DB 0b00000000,0b00000000
.DB 0b00000000,0b00000000

.DB 0b00000000,0b00000000
.DB 0b00000000,0b01001100
.DB 0b00000000,0b11000010
.DB 0b00000000,0b01000100
.DB 0b00000000,0b01000010
.DB 0b00000000,0b11101100
.DB 0b00000000,0b00000000
.DB 0b00000000,0b00000000

.DB 0b00000000,0b00000000
.DB 0b00000000,0b01000010
.DB 0b00000000,0b11000110
.DB 0b00000000,0b01001010
.DB 0b00000000,0b01001110
.DB 0b00000000,0b11100010
.DB 0b00000000,0b00000000
.DB 0b00000000,0b00000000

.DB 0b00000000,0b00000000
.DB 0b00000000,0b01001110
.DB 0b00000000,0b11001000
.DB 0b00000000,0b01001100
.DB 0b00000000,0b01000010
.DB 0b00000000,0b11101100
.DB 0b00000000,0b00000000
.DB 0b00000000,0b00000000

.DB 0b00000000,0b00000000
.DB 0b00000000,0b01000110
.DB 0b00000000,0b11001000
.DB 0b00000000,0b01001110
.DB 0b00000000,0b01001010
.DB 0b00000000,0b11101110
.DB 0b00000000,0b00000000
.DB 0b00000000,0b00000000

.DB 0b00000000,0b00000000
.DB 0b00000000,0b01001110
.DB 0b00000000,0b11000010
.DB 0b00000000,0b01000100
.DB 0b00000000,0b01000100
.DB 0b00000000,0b11100100
.DB 0b00000000,0b00000000
.DB 0b00000000,0b00000000

.DB 0b00000000,0b00000000
.DB 0b00000000,0b01001110
.DB 0b00000000,0b11001010
.DB 0b00000000,0b01001110
.DB 0b00000000,0b01001010
.DB 0b00000000,0b11101110
.DB 0b00000000,0b00000000
.DB 0b00000000,0b00000000

.DB 0b00000000,0b00000000
.DB 0b00000000,0b01001110
.DB 0b00000000,0b11001010
.DB 0b00000000,0b01001110
.DB 0b00000000,0b01000010
.DB 0b00000000,0b11101100
.DB 0b00000000,0b00000000
.DB 0b00000000,0b00000000

.DB 0b00000000,0b00000000
.DB 0b00000000,0b01000100
.DB 0b00000000,0b11001010
.DB 0b00000000,0b01001010
.DB 0b00000000,0b01001110
.DB 0b00000000,0b11101010
.DB 0b00000000,0b00000000
.DB 0b00000000,0b00000000

.DB 0b00000000,0b00000000
.DB 0b00000000,0b01001100
.DB 0b00000000,0b11001010
.DB 0b00000000,0b01001100
.DB 0b00000000,0b01001010
.DB 0b00000000,0b11101100
.DB 0b00000000,0b00000000
.DB 0b00000000,0b00000000

.DB 0b00000000,0b00000000
.DB 0b00000000,0b01000110
.DB 0b00000000,0b11001000
.DB 0b00000000,0b01001000
.DB 0b00000000,0b01001000
.DB 0b00000000,0b11100110
.DB 0b00000000,0b00000000
.DB 0b00000000,0b00000000

.DB 0b00000000,0b00000000
.DB 0b00000000,0b01001100
.DB 0b00000000,0b11001010
.DB 0b00000000,0b01001010
.DB 0b00000000,0b01001010
.DB 0b00000000,0b11101100
.DB 0b00000000,0b00000000
.DB 0b00000000,0b00000000

.DB 0b00000000,0b00000000
.DB 0b00000000,0b01001110
.DB 0b00000000,0b11001000
.DB 0b00000000,0b01001100
.DB 0b00000000,0b01001000
.DB 0b00000000,0b11101110
.DB 0b00000000,0b00000000
.DB 0b00000000,0b00000000

.DB 0b00000000,0b00000000
.DB 0b00000000,0b01001110
.DB 0b00000000,0b11001000
.DB 0b00000000,0b01001100
.DB 0b00000000,0b01001000
.DB 0b00000000,0b11101000
.DB 0b00000000,0b00000000
.DB 0b00000000,0b00000000

.DB 0b00000000,0b00000000
.DB 0b01000100,0b01000100
.DB 0b10101010,0b10101010
.DB 0b10101010,0b10101010
.DB 0b10101010,0b10101010
.DB 0b01000100,0b01000100
.DB 0b00000000,0b00000000
.DB 0b00000000,0b00000000

.DB 0b00000000,0b00000000
.DB 0b01000100,0b01000100
.DB 0b10101100,0b10101100
.DB 0b10100100,0b10100100
.DB 0b10100100,0b10100100
.DB 0b01001110,0b01001110
.DB 0b00000000,0b00000000
.DB 0b00000000,0b00000000

.DB 0b00000000,0b00000000
.DB 0b01001100,0b01001100
.DB 0b10100010,0b10100010
.DB 0b10100100,0b10100100
.DB 0b10101000,0b10101000
.DB 0b01001110,0b01001110
.DB 0b00000000,0b00000000
.DB 0b00000000,0b00000000

.DB 0b00000000,0b00000000
.DB 0b01001100,0b01001100
.DB 0b10100010,0b10100010
.DB 0b10100100,0b10100100
.DB 0b10100010,0b10100010
.DB 0b01001100,0b01001100
.DB 0b00000000,0b00000000
.DB 0b00000000,0b00000000

.DB 0b00000000,0b00000000
.DB 0b01000010,0b01000010
.DB 0b10100110,0b10100110
.DB 0b10101010,0b10101010
.DB 0b10101110,0b10101110
.DB 0b01000010,0b01000010
.DB 0b00000000,0b00000000
.DB 0b00000000,0b00000000

.DB 0b00000000,0b00000000
.DB 0b01001110,0b01001110
.DB 0b10101000,0b10101000
.DB 0b10101100,0b10101100
.DB 0b10100010,0b10100010
.DB 0b01001100,0b01001100
.DB 0b00000000,0b00000000
.DB 0b00000000,0b00000000

.DB 0b00000000,0b00000000
.DB 0b01000110,0b01000110
.DB 0b10101000,0b10101000
.DB 0b10101110,0b10101110
.DB 0b10101010,0b10101010
.DB 0b01001110,0b01001110
.DB 0b00000000,0b00000000
.DB 0b00000000,0b00000000

.DB 0b00000000,0b00000000
.DB 0b01001110,0b01001110
.DB 0b10100010,0b10100010
.DB 0b10100100,0b10100100
.DB 0b10100100,0b10100100
.DB 0b01000100,0b01000100
.DB 0b00000000,0b00000000
.DB 0b00000000,0b00000000

.DB 0b00000000,0b00000000
.DB 0b01001110,0b01001110
.DB 0b10101010,0b10101010
.DB 0b10101110,0b10101110
.DB 0b10101010,0b10101010
.DB 0b01001110,0b01001110
.DB 0b00000000,0b00000000
.DB 0b00000000,0b00000000

.DB 0b00000000,0b00000000
.DB 0b01001110,0b01001110
.DB 0b10101010,0b10101010
.DB 0b10101110,0b10101110
.DB 0b10100010,0b10100010
.DB 0b01001100,0b01001100
.DB 0b00000000,0b00000000
.DB 0b00000000,0b00000000

.DB 0b00000000,0b00000000
.DB 0b01000100,0b01000100
.DB 0b10101010,0b10101010
.DB 0b10101010,0b10101010
.DB 0b10101110,0b10101110
.DB 0b01001010,0b01001010
.DB 0b00000000,0b00000000
.DB 0b00000000,0b00000000

.DB 0b00000000,0b00000000
.DB 0b01001100,0b01001100
.DB 0b10101010,0b10101010
.DB 0b10101100,0b10101100
.DB 0b10101010,0b10101010
.DB 0b01001100,0b01001100
.DB 0b00000000,0b00000000
.DB 0b00000000,0b00000000

.DB 0b00000000,0b00000000
.DB 0b01000110,0b01000110
.DB 0b10101000,0b10101000
.DB 0b10101000,0b10101000
.DB 0b10101000,0b10101000
.DB 0b01000110,0b01000110
.DB 0b00000000,0b00000000
.DB 0b00000000,0b00000000

.DB 0b00000000,0b00000000
.DB 0b01001100,0b01001100
.DB 0b10101010,0b10101010
.DB 0b10101010,0b10101010
.DB 0b10101010,0b10101010
.DB 0b01001100,0b01001100
.DB 0b00000000,0b00000000
.DB 0b00000000,0b00000000

.DB 0b00000000,0b00000000
.DB 0b01001110,0b01001110
.DB 0b10101000,0b10101000
.DB 0b10101100,0b10101100
.DB 0b10101000,0b10101000
.DB 0b01001110,0b01001110
.DB 0b00000000,0b00000000
.DB 0b00000000,0b00000000

.DB 0b00000000,0b00000000
.DB 0b01001110,0b01001110
.DB 0b10101000,0b10101000
.DB 0b10101100,0b10101100
.DB 0b10101000,0b10101000
.DB 0b01001000,0b01001000
.DB 0b00000000,0b00000000
.DB 0b00000000,0b00000000

.DB 0b00000000,0b00000000
.DB 0b01000100,0b01000100
.DB 0b11001010,0b11001010
.DB 0b01001010,0b01001010
.DB 0b01001010,0b01001010
.DB 0b11100100,0b11100100
.DB 0b00000000,0b00000000
.DB 0b00000000,0b00000000

.DB 0b00000000,0b00000000
.DB 0b01000100,0b01000100
.DB 0b11001100,0b11001100
.DB 0b01000100,0b01000100
.DB 0b01000100,0b01000100
.DB 0b11101110,0b11101110
.DB 0b00000000,0b00000000
.DB 0b00000000,0b00000000

.DB 0b00000000,0b00000000
.DB 0b01001100,0b01001100
.DB 0b11000010,0b11000010
.DB 0b01000100,0b01000100
.DB 0b01001000,0b01001000
.DB 0b11101110,0b11101110
.DB 0b00000000,0b00000000
.DB 0b00000000,0b00000000

.DB 0b00000000,0b00000000
.DB 0b01001100,0b01001100
.DB 0b11000010,0b11000010
.DB 0b01000100,0b01000100
.DB 0b01000010,0b01000010
.DB 0b11101100,0b11101100
.DB 0b00000000,0b00000000
.DB 0b00000000,0b00000000

.DB 0b00000000,0b00000000
.DB 0b01000010,0b01000010
.DB 0b11000110,0b11000110
.DB 0b01001010,0b01001010
.DB 0b01001110,0b01001110
.DB 0b11100010,0b11100010
.DB 0b00000000,0b00000000
.DB 0b00000000,0b00000000

.DB 0b00000000,0b00000000
.DB 0b01001110,0b01001110
.DB 0b11001000,0b11001000
.DB 0b01001100,0b01001100
.DB 0b01000010,0b01000010
.DB 0b11101100,0b11101100
.DB 0b00000000,0b00000000
.DB 0b00000000,0b00000000

.DB 0b00000000,0b00000000
.DB 0b01000110,0b01000110
.DB 0b11001000,0b11001000
.DB 0b01001110,0b01001110
.DB 0b01001010,0b01001010
.DB 0b11101110,0b11101110
.DB 0b00000000,0b00000000
.DB 0b00000000,0b00000000

.DB 0b00000000,0b00000000
.DB 0b01001110,0b01001110
.DB 0b11000010,0b11000010
.DB 0b01000100,0b01000100
.DB 0b01000100,0b01000100
.DB 0b11100100,0b11100100
.DB 0b00000000,0b00000000
.DB 0b00000000,0b00000000

.DB 0b00000000,0b00000000
.DB 0b01001110,0b01001110
.DB 0b11001010,0b11001010
.DB 0b01001110,0b01001110
.DB 0b01001010,0b01001010
.DB 0b11101110,0b11101110
.DB 0b00000000,0b00000000
.DB 0b00000000,0b00000000

.DB 0b00000000,0b00000000
.DB 0b01001110,0b01001110
.DB 0b11001010,0b11001010
.DB 0b01001110,0b01001110
.DB 0b01000010,0b01000010
.DB 0b11101100,0b11101100
.DB 0b00000000,0b00000000
.DB 0b00000000,0b00000000

.DB 0b00000000,0b00000000
.DB 0b01000100,0b01000100
.DB 0b11001010,0b11001010
.DB 0b01001010,0b01001010
.DB 0b01001110,0b01001110
.DB 0b11101010,0b11101010
.DB 0b00000000,0b00000000
.DB 0b00000000,0b00000000

.DB 0b00000000,0b00000000
.DB 0b01001100,0b01001100
.DB 0b11001010,0b11001010
.DB 0b01001100,0b01001100
.DB 0b01001010,0b01001010
.DB 0b11101100,0b11101100
.DB 0b00000000,0b00000000
.DB 0b00000000,0b00000000

.DB 0b00000000,0b00000000
.DB 0b01000110,0b01000110
.DB 0b11001000,0b11001000
.DB 0b01001000,0b01001000
.DB 0b01001000,0b01001000
.DB 0b11100110,0b11100110
.DB 0b00000000,0b00000000
.DB 0b00000000,0b00000000

.DB 0b00000000,0b00000000
.DB 0b01001100,0b01001100
.DB 0b11001010,0b11001010
.DB 0b01001010,0b01001010
.DB 0b01001010,0b01001010
.DB 0b11101100,0b11101100
.DB 0b00000000,0b00000000
.DB 0b00000000,0b00000000

.DB 0b00000000,0b00000000
.DB 0b01001110,0b01001110
.DB 0b11001000,0b11001000
.DB 0b01001100,0b01001100
.DB 0b01001000,0b01001000
.DB 0b11101110,0b11101110
.DB 0b00000000,0b00000000
.DB 0b00000000,0b00000000

.DB 0b00000000,0b00000000
.DB 0b01001110,0b01001110
.DB 0b11001000,0b11001000
.DB 0b01001100,0b01001100
.DB 0b01001000,0b01001000
.DB 0b11101000,0b11101000
.DB 0b00000000,0b00000000
.DB 0b00000000,0b00000000

.DB 0b00000000,0b00000000
.DB 0b00000000,0b00000000
.DB 0b00000000,0b00000000
.DB 0b00000000,0b00000000
.DB 0b00000000,0b00000000
.DB 0b00000000,0b00000000
.DB 0b00000000,0b00000000
.DB 0b00000000,0b00000000

.DB 0b11111111,0b00000000
.DB 0b11111111,0b00000000
.DB 0b11111111,0b00000000
.DB 0b11111111,0b00000000
.DB 0b11111111,0b00000000
.DB 0b11111111,0b00000000
.DB 0b11111111,0b00000000
.DB 0b11111111,0b00000000

.DB 0b00000000,0b11111111
.DB 0b00000000,0b11111111
.DB 0b00000000,0b11111111
.DB 0b00000000,0b11111111
.DB 0b00000000,0b11111111
.DB 0b00000000,0b11111111
.DB 0b00000000,0b11111111
.DB 0b00000000,0b11111111

.DB 0b11111111,0b11111111
.DB 0b11111111,0b11111111
.DB 0b11111111,0b11111111
.DB 0b11111111,0b11111111
.DB 0b11111111,0b11111111
.DB 0b11111111,0b11111111
.DB 0b11111111,0b11111111
.DB 0b11111111,0b11111111

paled_pal:
   .DW   0b000000000000000
   .DW   0b000000000011111
   .DW   0b000001111100000
   .DW   0b111110000000000

colornum = 0xDFFE
rgbnum = 0xDFFF

real_crash_stuff::
   EI

cpu_slow:                              ; Try to set FAST mode
   LD    A, #0x01
   LDH   (P_KEY1), A                   ; Causes 1x <-> 2x toggle
   STOP
   LDH   A, (P_KEY1)                   ; Is CPU 2x speed?
   ADD   A, A
   JR    C, cpu_slow

   DI

   XOR   A
   LDH   (0x26), A

   LD    HL, #paled_pal
   LD    DE, #_pal_cur+8*7
   LD    B, #8
   CALL  _LDIR_B

   LD    A, #1
   LDH   (P_VBK), A

   LD    A, #7
   LD    HL, #0x9C00
   LD    BC, #64
   CALL  _STIR_VRAM

   XOR   A
   LDH   (P_VBK), A

   LD    DE, #paled_tiles
   LD    HL, #0x9000
   LD    BC, #1600
   CALL  _LDIR_VRAM_ACC

   XOR   A
   LD    HL, #0x9C00
   LD    BC, #64
   CALL  _STIR_VRAM

   LD    A, #119
   LDH   (P_WX), A
   LD    A, #135
   LDH   (P_WY), A

   LD    HL, #crash_code
   LD    DE, #0xD000
   LD    BC, #crash_code_end-crash_code
   CALL  _LDIR

   XOR   A
   LD    HL, #colornum
   LD    (HLI), A
   LD    (HLI), A

   JP    0xD000

crash_code:
   JP    stuff-crash_code+0xD000
   JP    ld_v_a-crash_code+0xD000
   JP    set_pal-crash_code+0xD000
   JP    update_paled-crash_code+0xD000
   JP    _getk-crash_code+0xD000

ld_v_a:
   LD    (HL), A
   CP    (HL)
   JR    NZ, ld_v_a
   INC   HL
   RET

set_pal:
   LD    BC, #0xFF68
   LD    HL, #_pal_cur

   LD    A, B

   LD    E, #0x80                       ; Incremental pointer
set_pal_loop2:
   ADD   A, A
   JR    NC, pal_nomodify

   PUSH  AF
   LD    D, #0x04
pal_loop2:
   LDH   A, (P_STAT)
   AND   #0x02
   JR    NZ, pal_loop2

   LD    A, E
   LD    (BC), A
   INC   C
   LD    A, (HLI)
   LD    (BC), A
   LD    A, (HLI)
   LD    (BC), A
   DEC   C                             ; next rgb_data
   INC   E                             ; next pointer
   INC   E                             ;
   DEC   D                             ; count down palettes
   JR    NZ, pal_loop2
   POP   AF

   JR    NZ, set_pal_loop2
pal_nomodify:
   RET   Z

   PUSH  AF
   LD    A, E

   LD    DE, #0x0008
   ADD   HL, DE

   ADD   A, E
   LD    E, A
   POP   AF

   JR    set_pal_loop2

stuff:

w4vbl_a:
   LDH   A, (P_LY)
   CP    #144
   JR    NC, w4vbl_a
w4vbl_b:
   LDH   A, (P_LY)
   CP    #144
   JR    C, w4vbl_b

   CALL  0xD006 ; set pal
   CALL  0xD009 ; update pal info
   CALL  0xD00C ; get keys

;.DB   0x04,0x08,0x02,0x01,0x10,0x20,0x80,0x40
; up, down, left, right, a, b, start, select

   LD    HL, #key_cur
   LD    A, (HLI)
   AND   (HL)
   LD    D, A

   LD    HL, #colornum

   BIT   2, D
   JR    Z, not_up
   INC   (HL)
not_up:

   BIT   3, D
   JR    Z, not_down
   DEC   (HL)
not_down:

   LD    HL, #rgbnum

   BIT   0, D
   JR    Z, not_right
   LD    A, (HL)
   CP    #2
   JR    Z, not_right
   INC   (HL)
not_right:

   BIT   1, D
   JR    Z, not_left
   LD    A, (HL)
   OR    A
   JR    Z, not_left
   DEC   (HL)
not_left:

   BIT   4, D
   JR    Z, not_a

   LD    A, (colornum)
   AND   #0x1F
   ADD   A, A
   LD    HL, #_pal_cur
   LD    B, #0
   LD    C, A
   ADD   HL, BC

   LD    BC, #rgbnum

   LD    A, (rgbnum)
   OR    A
   JR    NZ, no_inc_red
   LD    A, (HL)
   AND   #0x1F
   CP    #0x1F
   JR    Z, no_inc_red
   INC   (HL)
no_inc_red:

   LD    A, (rgbnum)
   CP    #1
   JR    NZ, no_inc_green
   LD    A, (HLI)
   AND   #0xE0
   LD    E, A
   LD    A, (HLD)
   AND   #0x03
   OR    E
   CP    #0xE3
   JR    Z, no_inc_green
   LD    A, (HL)
   ADD   A, #0x20
   LD    (HLI), A
   LD    A, (HL)
   ADC   A, #0x00
   LD    (HLD), A
no_inc_green:
   INC   HL

   LD    A, (rgbnum)
   CP    #2
   JR    NZ, no_inc_blue
   LD    A, (HL)
   AND   #0x7C
   CP    #0x7C
   JR    Z, no_inc_blue
   LD    A, (HL)
   ADD   A, #0x04
   LD    (HL), A
no_inc_blue:

not_a:

   BIT   5, D
   JR    Z, not_b

   LD    A, (colornum)
   AND   #0x1F
   ADD   A, A
   LD    HL, #_pal_cur
   LD    B, #0
   LD    C, A
   ADD   HL, BC

   LD    BC, #rgbnum

   LD    A, (rgbnum)
   OR    A
   JR    NZ, no_dec_red
   LD    A, (HL)
   AND   #0x1F
   JR    Z, no_dec_red
   DEC   (HL)
no_dec_red:

   LD    A, (rgbnum)
   CP    #1
   JR    NZ, no_dec_green
   LD    A, (HLI)
   AND   #0xE0
   LD    E, A
   LD    A, (HLD)
   AND   #0x03
   OR    E
   JR    Z, no_dec_green
   LD    A, (HL)
   SUB   A, #0x20
   LD    (HLI), A
   LD    A, (HL)
   SBC   A, #0x00
   LD    (HLD), A
no_dec_green:
   INC   HL

   LD    A, (rgbnum)
   CP    #2
   JR    NZ, no_dec_blue
   LD    A, (HL)
   AND   #0x7C
   JR    Z, no_dec_blue
   LD    A, (HL)
   SUB   A, #0x04
   LD    (HL), A
no_dec_blue:

not_b:
   JP    0xD000

update_paled:
   XOR   A
   LD    HL, #0x9C23
   CALL  0xD003
   CALL  0xD003
   CALL  0xD003

   LD    DE, #colornum

   LD    A, #1
   LDH   (P_VBK), A

   LD    A, (DE)
   RRCA
   RRCA
   AND   #0x07
   LD    HL, #0x9C02
   CALL  0xD003
   XOR   A
   LDH   (P_VBK), A

   LD    A, (DE)
   AND   #0x1F
   ADD   A, A

   LD    HL, #_pal_cur
   LD    B, #0x00
   LD    C, A
   ADD   HL, BC
   LD    B, H
   LD    C, L

   LD    HL, #0x9C00
   LD    A, (DE)
   RRCA
   RRCA
   AND   #0x07
   CALL  0xD003
   LD    A, (DE)
   AND   #0x03
   ADD   #32
   CALL  0xD003
   ADD   #64
   CALL  0xD003
   LD    A, (DE)
   ADD   A, A

   LD    A, (BC)
   AND   #0x1F
   CALL  0xD003
   LD    A, (BC)
   AND   #0xE0
   LD    D, A
   INC   BC
   LD    A, (BC)
   AND   #0x03
   OR    D
   RLCA
   RLCA
   RLCA
   ADD   A, #32
   CALL  0xD003
   LD    A, (BC)
   AND   #0x7C
   RRCA
   RRCA
   ADD   A, #64
   CALL  0xD003

   LD    A, (rgbnum)
   LD    D, #0x00
   LD    E, A
   LD    HL, #0x9C23
   ADD   HL, DE
   ADD   A, #97
   CALL  0xD003
   RET

_getk:
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

   LD    HL, #key_cur
   XOR   (HL)
   LD    (HL), E
   INC   HL
   LD    (HLD), A
   
   LD    A, #0x30                      ; reset port
   LDH   (P_P1), A

   RET

crash_code_end:

_great_dark_fade_pal::
.DW 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0
.DW 0,0,0,0
.DW 0b000000000000000,0b000101101011111,0b001100101011111,0b000000000000000
.DW 0,0,0,0, 0,0,0,0

.DW 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0
.DW 0,0,0,0
.DW 0b000000000000000,0b001011111111111,0b010101011111111,0b000000000000000
.DW 0,0,0,0, 0,0,0,0

s_Infiniti:
   .ASCIZ "Justin Karneges"
s_Matt:
   .ASCIZ "Matt Rossi"
s_OneAndOnly:
   .ASCIZ "Mark Yohalem"
s_CrASH_Man:
   .ASCIZ "Hideaki Omuro"
s_Paragon:
   .ASCIZ "Paragon5"
s_Melll:
   .ASCIZ "Melvin de Voor"
s_Ville:
   .ASCIZ "Ville Jarvi"
s_NutZBoy:
   .ASCIZ "Nick London"
s_EGM:
   .ASCIZ "Eric Hache"
s_TSSF:
   .ASCIZ "Mathew Valente"
s_adam1844:
   .ASCIZ "Adam Ludwin"
s_SoyBomb:
   .ASCIZ "Sohail Shajary"
s_Enrique:
   .ASCIZ "Enrique Caballero"
s_Kirk:
   .ASCIZ "Kirk Meyer"
s_Sabrewulf:
   .ASCIZ "Chuck Mason"
s_Jim:
   .ASCIZ "Jim Corbett"
s_Doug:
   .ASCIZ "Doug Weeks"
s_Affinix:
   .ASCIZ "Affinix Software"
s_Publisher:
   .ASCIZ "XXXXX XXXXXXXXXXXXX"

s_d_director:
   .ASCIZ "DIRECTOR"
s_d_producer:
   .ASCIZ "PRODUCED BY"
s_d_scenario:
   .ASCIZ "SCENARIO DESIGN AND SCRIPT"

s_p_main:
   .ASCIZ "LEAD PROGRAMMER"
s_p_hardware:
   .ASCIZ "HARDWARE PROGRAMMER"
s_p_battle:
   .ASCIZ "BATTLE SYSTEM"
s_p_sound:
   .ASCIZ "SOUND PROGRAM"
s_p_event:
   .ASCIZ "EVENT PROGRAMMING"
s_p_map:
   .ASCIZ "MAP DESIGNER"

s_c_concept:
   .ASCIZ "CONCEPT ART"
s_c_character:
   .ASCIZ "CHARACTER/ENEMY ART"
s_c_background:
   .ASCIZ "BACKGROUND ART"
s_c_misc:
   .ASCIZ "MISCELLANEOUS ART"
s_c_music:
   .ASCIZ "MUSIC COMPOSER"
s_c_sound:
   .ASCIZ "SOUND ENGINEER"

s_t_beta:
   .ASCIZ "BETA TESTING"
s_t_thanks:
   .ASCIZ "SPECIAL THANKS TO"

s_i_company:
   .ASCIZ "CREATED BY"
s_i_publisher:
   .ASCIZ "PUBLISHED BY"

strings_table:
.DW s_Infiniti, s_Matt, s_OneAndOnly, s_CrASH_Man, s_Paragon           ; 0
.DW s_Melll, s_Ville, s_NutZBoy, s_EGM, s_TSSF, s_adam1844, s_Soybomb  ; 5
.DW s_Enrique, s_Kirk, s_Sabrewulf, s_Jim, s_Doug                      ;12
.DW s_Affinix, s_Publisher                                             ;17
.DW s_d_director, s_d_producer, s_d_scenario, s_p_main, s_p_hardware   ;19
.DW s_p_battle, s_p_sound, s_p_event, s_p_map, s_c_concept             ;24
.DW s_c_character, s_c_background, s_c_misc, s_c_music, s_c_sound      ;29
.DW s_t_beta, s_t_thanks, s_i_company, s_i_publisher                   ;34

credits_table:
.DB   19,0,-1
.DB   20,1,-1
.DB   21,2,-1
.DB   22,0,-1
.DB   23,3,25,4,-1
.DB   24,0,-1
.DB   26,0,2,3,-1
.DB   27,3,-1
.DB   28,5,29,5,-1
.DB   30,5,3,-1
.DB   30,6,7,-1
.DB   31,3,0,-1
.DB   32,8,33,9,-1
.DB   34,2,12,-1
.DB   35,10,11,13,-1
.DB   35,14,15,16,-1
.DB   36,17,-1
.DB   37,18,-1

; dfs_pschar

;----------------------------------------------------------------------------
;[ sch_line ]
;parameters:
; (B,C) Tile Offset
; A = Starting Character
; D = Length
;modifies:
; none
;----------------------------------------------------------------------------
_sch_line:
   LD    E, A
   PUSH  BC
   PUSH  DE
   CALL  _sch_line2
   POP   DE
   POP   BC
   INC   C
   INC   E
_sch_line2:
   LD    A, C
   RRCA
   RRCA
   RRCA
   LD    L, A
   AND   #0x1F
   ADD   A, #C_PP_VRAM_WT
   LD    H, A
   LD    A, L
   AND   #0xE0
   ADD   A, B
   LD    L, A
   LD    A, E
sch_loop:
   RST   ldi_v_a
   INC   A
   INC   A
   DEC   D
   JR    NZ, sch_loop
   RET

done_glob:
   LD    D, H
   LD    A, L

   LD    HL, #ending_ptr
   LD    (HLI), A
   LD    (HL), D

   POP   DE
   RET

_drawglob_real::
   PUSH  DE

   LD    HL, #ending_ptr
   LD    A, (HLI)
   LD    H, (HL)
   LD    L, A

;   LD    HL, #credits_table
   LD    BC, #0x0080
   LD    DE, #_endingobj_mem

_drawglob_loop:
   LD    A, (HLI)
   CP    #0xFF
   JR    Z, done_glob
   PUSH  DE
   PUSH  HL
   LD    HL, #strings_table
   PUSH  BC
   LD    B, #0
   LD    C, A
   ADD   HL, BC
   ADD   HL, BC
   POP   BC          ; get C
   PUSH  BC
   CP    #19
   JR    NC, _credit_title

   PUSH  HL
   LD    B, #36
   PUSH  BC
   CALL  _dfs_clear
   POP   BC
   POP   HL
   POP   BC
   LD    D, #18
   LD    A, C
   PUSH  BC
   LD    C, B
   LD    B, #1
   PUSH  HL
   CALL  _sch_line
   POP   HL

   LD    A, (HLI)
   LD    C, A
   LD    A, (HLI)
   LD    B, A

   LD    A, #1
   LD    (_dfs_face), A

   POP   HL
   PUSH  HL
   LD    H, #8

pscloop:
   LD    A, (BC)
   OR    A
   JR    Z, _done_pscing
   PUSH  BC
   LD    C, A
   PUSH  BC
   PUSH  HL
   CALL  _dfs_pschar
   POP   HL
   POP   BC
   LD    H, E
   POP   BC
   INC   BC
   JR    pscloop

_done_pscing:
   LD    (_dfs_face), A

   POP   BC
   POP   HL
   LD    A, C
   ADD   #36
   LD    C, A
   INC   B
   INC   B

   POP   DE
   LD    A, #1
   LD    (DE), A
   INC   DE
   LD    A, B
   ADD   A, A
   ADD   A, A
   LD    (DE), A
   INC   DE
   LD    A, #1
   LD    (DE), A
   INC   DE
   LD    A, B
   ADD   A, A
   ADD   A, A
   LD    (DE), A
   INC   DE

   JR    _drawglob_loop

_credit_title:
   PUSH  HL
   LD    B, #18
   PUSH  BC
   CALL  _dfs_clear
   POP   BC
   POP   HL
   POP   BC
   LD    A, C
   LD    (_barg3), A
   LD    A, #18
   LD    (_barg4), A
   ADD   A, C
   LD    C, A
   LD    A, #1
   LD    (_barg1), A
   LD    A, B
   LD    (_barg2), A
   PUSH  BC
   PUSH  HL
   CALL  _dfs_line
   POP   HL

   LD    A, (HLI)
   LD    C, A
   LD    A, (HLI)
   LD    B, A

   LD    A, #1
   LD    (_dfs_face), A

   XOR   A
   LD    (_barg1), A

   LD    HL, #_warg1
   LD    A, C
   LD    (HLI), A
   LD    (HL), B
   CALL  _dfs_pstr

   XOR   A
   LD    (_dfs_face), A

   POP   BC
   POP   HL
   INC   B

   POP   DE
   LD    A, #1
   LD    (DE), A
   INC   DE
   LD    A, B
   ADD   A, A
   ADD   A, A
   LD    (DE), A
   INC   DE

   JP    _drawglob_loop

_ending_offset_table:
.db 0xA0,0x9B,0x96,0x91,0x8C,0x88,0x84,0x7F,0x7B,0x78,0x74,0x70
.db 0x6D,0x69,0x66,0x63,0x5F,0x5C,0x59,0x57,0x54,0x51,0x4F,0x4C
.db 0x4A,0x47,0x45,0x43,0x41,0x3F,0x3D,0x3B,0x39,0x37,0x35,0x34
.db 0x32,0x30,0x2F,0x2D,0x2C,0x2A,0x29,0x28,0x26,0x25,0x24,0x23
.db 0x22,0x21,0x1F,0x1E,0x1D,0x1D,0x1C,0x1B,0x1A,0x19,0x18,0x17
.db 0x17,0x16,0x15,0x14,0x14,0x13,0x12,0x12,0x11,0x11,0x10,0x0F
.db 0x0F,0x0E,0x0E,0x0D,0x0D,0x0D,0x0C,0x0C,0x0B,0x0B,0x0A,0x0A
.db 0x0A,0x09,0x09,0x09,0x08,0x08,0x08,0x07,0x07,0x07,0x07,0x06
.db 0x06,0x06,0x06,0x05,0x05,0x05,0x05,0x05,0x05,0x04,0x04,0x04
.db 0x04,0x04,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x02,0x02
.db 0x02,0x02,0x02,0x02,0x02,0x02,0x01,0x01,0x01,0x01,0x01,0x01
.db 0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x00,0x00

;----------------------------------------
; should wait 82 clocks to set stuff

_ending_int_real::               ; 19 (to get here), 3 extra later
   LD    HL, #_ending_local      ; 3
   LD    A, (HL)                 ; 2
   INC   (HL)                    ; 4
   OR    A                       ; 1
   JR    NZ, _ending_int2        ; 3/2

   LD    A, (_bottomline)        ; 4
   LDH   (P_LYC), A              ; 3

   ; 40 clock delay
   NOP                           ; 1
   NOP                           ; 1
   NOP                           ; 1
   LD    A, #9                   ; 2
_ending_delay1:
   DEC   A                       ; 1
   JR    NZ, _ending_delay1      ; 3/2

   LD    A, (_offset)            ; 4
   LDH   (P_SCY), A              ; 3
   LD    A, #0b11100111          ; 2
   LDH   (P_LCDC), A             ; 3

   RET                           ; 4

_ending_int2:
   DEC   A                       ; 1
   JR    NZ, _ending_int3        ; 3/2

   LD    A, (_endingmode)        ; 4
   CP    #2                      ; 2
   JR    NZ, _ending_nobottom    ; 3/2
   LD    A, #79                  ; 2
   LDH   (P_LYC), A              ; 3
   JR    _ending_bot_join        ; 3
_ending_nobottom:

   ; 7 clock delay
   PUSH  AF
   POP   AF

_ending_bot_join:

   ; 31 clock delay
   NOP
   NOP
   LD    A, #7                   ; 2
_ending_delay2:
   DEC   A                       ; 1
   JR    NZ, _ending_delay2      ; 3/2

   LD    A, #-144                ; 2
   LDH   (P_SCY), A              ; 3
   LD    A, #0b11101101          ; 2
   LDH   (P_LCDC), A             ; 3

   RET

_ending_int3:
   CP    #8                      ; 2
   RET   Z                       ; 2

   PUSH  DE                      ; 4

   ADD   A, A                    ; 1

   LD    D, #0                   ; 2
   LD    E, A                    ; 1

   LD    HL, #_endingobj_mem-2   ; 4
   ADD   HL, DE                  ; 2

   LD    E, A                    ; 1

   LD    A, (HLI)                ; 2
   DEC   A                       ; 1
   JR    Z, _delay1              ; 3/2
   DEC   A                       ; 1
   JR    NZ, _ending_secondmode  ; 3/2

   LD    A, E                    ; 1
   LD    E, (HL)                 ; 2

   LD    HL, #_ending_offset_table ; 4
   ADD   HL, DE                  ; 2

   LD    E, A                    ; 1

   LD    A, #7                   ; 2
   ADD   A, (HL)                 ; 2 ; 1 clock before necessary
   LDH   (P_WX), A               ; 3

   XOR   A                       ; 1
   LDH   (P_SCY), A              ; 3
   LD    A, #0b11101101          ; 2
   LDH   (P_LCDC), A             ; 3

   JR    _lyc_join

;   ADD   A, A                    ; 1
;   LD    L, A                    ; 1
;
;   ADD   A, A                    ; 1
;   ADD   A, A                    ; 1
;   ADD   A, #79                  ; 2
;
;   LDH   (P_LYC), A              ; 3
;   LD    A, L                    ; 1
;
;   LD    HL, #_endingobj_mem-2   ; 4
;   ADD   A, L                    ; 1
;   LD    L, A                    ; 1
;   ADC   A, H                    ; 1
;   SUB   L                       ; 1
;   LD    H, A                    ; 1
;
;   LD    A, (HLI)                ; 2
;   DEC   A                       ; 1
;   JR    Z, _delay1              ; 3/2
;   DEC   A                       ; 1
;   JR    NZ, _ending_secondmode  ; 3/2
;   LD    A, (HLI)                ; 2
;
;   LD    HL, #_ending_offset_table ; 4
;   ADD   A, L                    ; 1
;   LD    L, A                    ; 1
;   ADC   A, H                    ; 1
;   SUB   L                       ; 1
;   LD    H, A                    ; 1
;
;   LD    A, #7                   ; 2
;   ADD   A, (HL)                 ; 2 ; 2 extra clocks up to here... oh well
;   LDH   (P_WX), A               ; 3
;
;   XOR   A                       ; 1
;   LDH   (P_SCY), A              ; 3
;   LD    A, #0b11101101          ; 2
;   LDH   (P_LCDC), A             ; 3
   RET

_delay1:
   ; 17 clocks
   PUSH  AF
   POP   AF
   PUSH  AF
   POP   AF
   NOP
   NOP
   NOP

   LD    A, #159                 ; 2
   LDH   (P_WX), A               ; 3

_lyc_join:
   LD    A, E                    ; 1

   ADD   A, A                    ; 1
   ADD   A, A                    ; 1
   ADD   A, #79                  ; 2

   LDH   (P_LYC), A              ; 3

   POP   DE

   RET

_delay2:
   ; 8 clocks
   PUSH  AF
   POP   AF
   NOP

   LD    A, #159
   LDH   (P_WX), A
   XOR   A
   LDH   (P_SCX), A
   LD    A, #-80
   LDH   (P_SCY), A

   JR    _lyc_join

_ending_secondmode:
   DEC   A
   JR    Z, _delay2

   LD    A, #143
   SUB   (HL)

   LD    HL, #_ending_offset_table
   ADD   A, L
   LD    L, A
   ADC   A, H
   SUB   L
   LD    H, A

   LD    A, (HL)                    ; 7 clocks over... oh well
   LDH   (P_SCX), A
   LD    A, #-80
   LDH   (P_SCY), A
   LD    A, #0b11101101
   LDH   (P_LCDC), A

   JR    _lyc_join

;--------------------------------------------

credits_init_real::
   LD    HL, #_topline
   LD    A, #28         ; top = 28
   LD    (HLI), A       ; bottom = 115
   LD    (HL), #115

   LDH   A, (_rscy)
   LD    (_newoffset), A

   LD    HL, #_endingcount
   XOR   A
   LD    (HLI), A
   LD    (HLI), A
   LD    (HLI), A

   LD    HL, #_pfs_targetbgr
   LD    A, (HLI)
   LD    E, A
   LD    D, (HL)
   PUSH  DE
   XOR   A
   LD    (HLD), A
   LD    (HL), #0x01

   LD    DE, #endingblackpal-56

   LD    H, A                       ; select gba
   LD    A, (__gba)
   ADD   A, #255
   SBC   A, A
   AND   #0x08
   LD    L, A
   ADD   HL, DE
   LD    D, H
   LD    E, L

   CALL  _asm_setpal
   POP   DE
   LD    HL, #_pfs_targetbgr
   LD    A, E
   LD    (HLI), A
   LD    (HL), D

   LD    HL, #0x9C00
   LD    BC, #1024
   LD    A, #0x04
   CALL  _STIR_VRAM

   LD    BC, #credits_table
   LD    HL, #ending_ptr
   LD    A, C
   LD    (HLI), A
   LD    (HL), B

;   CALL  _drawglob_real

;   XOR   A
;   LD    (ending_done), A

   LD    A, #8
   LDH   (_hbimode), A

   LD    A, #1
   LD    (_endingmode), A
   RET

credits_next_real::
;   XOR   A
;   LD    (ending_done), A

   LD    A, #1
   LD    (_endingmode), A

   CALL  _drawglob_real

   LD    A, #2
   LD    (_endingmode), A

   RET
