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

; C interface to asm audio routines

_rom_page_sound = _rom_page_para5
_ram_page_sound = 6

.area _BSS

song_page::
   .DS   1
song_page2::
   .DS   1
sound_play::
   .DS   1

.area _CODE

;------------ songs playlist --------------------
playtable:
; old playtable
;   .DB   0x3E,4         ; 00 - title
;   .DB   0x3E,2         ; 01 - prophecy
;   .DB   0x3E,1         ; 02 - overworld
;   .DB   0x3E,5         ; 03 - town1
;   .DB   0x3F,1         ; 04 - castle
;   .DB   0x3E,0         ; 05 - mountain
;   .DB   0x3F,2         ; 06 - fight1
;   .DB   0x3F,0         ; 07 - boss1
;   .DB   0x3E,3         ; 08 - rest
;   .DB   0x3F,3         ; 09 - game over

.DB   0x49,2         ; 00 - title
.DB   0x3E,2         ; 01 - prophecy
.DB   0x4B,0         ; 02 - overworld
.DB   0x4C,1         ; 03 - town1
.DB   0x42,0         ; 04 - castle
.DB   0x49,0         ; 05 - mountain
.DB   0x48,0         ; 06 - fight1
.DB   0x4D,1         ; 07 - boss1
.DB   0x4E,0         ; 08 - rest
.DB   0x4E,1         ; 09 - game over
.DB   0x46,0         ; 10 - cave
.DB   0x46,1         ; 11 - creator
.DB   0x47,0         ; 12 - creator evil
.DB   0x4C,2         ; 13 - town 2
.DB   0x48,1         ; 14 - kassim
.DB   0x4A,1         ; 15 - forest
.DB   0x4D,0         ; 16 - alutha
.DB   0x49,1         ; 17 - mystery
.DB   0x43,0         ; 18 - trouble
.DB   0x4E,2         ; 19 - victory
.DB   0x4B,1         ; 20 - sadness
.DB   0x4C,0         ; 21 - sailing
.DB   0x47,1         ; 22 - icecavern
.DB   0x4A,0         ; 23 - mystic
.DB   0x44,0         ; 24 - great dark
.DB   0x45,0         ; 25 - lastboss
.DB   0x41,0         ; 26 - ending1
.DB   0x40,0         ; 27 - ending2
.DB   0x4E,3         ; 28 - nighttime

;----------------------------------------------

;----------------------------------------------------------------------------
;[[ SOUNDLIB_Update ]]
;----------------------------------------------------------------------------
_SOUNDLIB_Update::
_Audio_FrameProcess::
   LD    A, #_ram_page_sound
   LDH   (P_SVBK), A
   LD    A, (song_page)
   LD    (0x2000), A

   CALL  0x4003

   LDH   A, (_rampage)
   LDH   (P_SVBK), A
   LDH   A, (_rompage)
   LD    (0x2000), A
   RET

_soundlib_setup:
   LDH   A, (_rampage)
   LD    D, A
   LDH   A, (_rompage)
   LD    E, A
   LD    A, #_ram_page_sound
   LDH   (_rampage), A
   LDH   (P_SVBK), A
   LD    A, (song_page)
   LDH   (_rompage), A
   LD    (0x2000), A
   RET

;----------------------------------------------------------------------------
;[[ void SOUNDLIB_Stop() ]]
;----------------------------------------------------------------------------
_SOUNDLIB_Stop::
   CALL  _soundlib_setup
   CALL  0x4009

;   XOR   A
;   LD    (sound_play), A

   JR    _soundlib_deinit

;----------------------------------------------------------------------------
;[[ void SOUNDLIB_Pause() ]]
;----------------------------------------------------------------------------
_SOUNDLIB_Pause::
_Audio_Music_Stop::
   CALL  _soundlib_setup
   PUSH  DE

   CALL  0x4009

;   XOR   A
;   LD    (sound_play), A

   LD    HL, #0xDF00
   LD    DE, #0xDF80
_sl_copyloop:
   LD    A, (DE)
   LD    (HLI), A
   INC   E
   JR    NZ, _sl_copyloop

   LD    A, (song_page)
   LD    (song_page2), A
   POP   DE

   JR    _soundlib_deinit

;----------------------------------------------------------------------------
;[[ void SOUNDLIB_Resume() ]]
;----------------------------------------------------------------------------
_SOUNDLIB_Resume::
_Audio_Music_Resume::
;   XOR   A
;   LD    (sound_play), A

   CALL  _soundlib_setup
   PUSH  DE

   LD    HL, #0xDF00
   LD    DE, #0xDF80
_sl_copyloop2:
   LD    A, (HLI)
   LD    (DE), A
   INC   E
   JR    NZ, _sl_copyloop2

   LD    A, (song_page2)
   LD    (song_page), A

   CALL  0x400C
   POP   DE

   CALL  _soundlib_deinit

;   LD    A, #1
;   LD    (sound_play), A

   RET

;--------------------------------- audio stop
_Audio_SFX_Stop::
   CALL  _soundlib_setup
   CALL  0x4012

   JR    _soundlib_deinit

;--------------------------------- lock chan3
_Audio_SFX_LockChnl3::
   CALL  _soundlib_setup
   CALL  0x4015

_soundlib_deinit:
   LD    A, D
   LDH   (_rampage), A
   LDH   (P_SVBK), A
   LD    A, E
   LDH   (_rompage), A
   LD    (0x2000), A
   RET

;--------------------------------- unlock chan3
_Audio_SFX_UnlockChnl3::
   CALL  _soundlib_setup
   CALL  0x4018
   JR    _soundlib_deinit

;----------------------------------------------------------------------------
;[[ void SOUNDLIB_Init() ]]
;----------------------------------------------------------------------------
_SOUNDLIB_Init::
   RET

_Audio_Init::
;   LDH   A, (_rampage)
;   LD    D, A
;   LDH   A, (_rompage)
;   LD    E, A
;   LD    A, #_ram_page_sound
;   LDH   (_rampage), A
;   LDH   (P_SVBK), A
;   LD    A, (song_page)
;   LDH   (_rompage), A
;   LD    (0x2000), A
;
;   PUSH  DE
;   PUSH  BC
;   CALL  0x4000
;   POP   BC
;   POP   DE
;
;   LD    A, D
;   LDH   (_rampage), A
;   LDH   (P_SVBK), A
;   LD    A, E
;   LDH   (_rompage), A
;   LD    (0x2000), A
;   RET

;----------------------------------------------------------------------------
;[[ void SOUNDLIB_Play(byte songnum) ]]
;----------------------------------------------------------------------------
_SOUNDLIB_Play::
_Audio_Music_Play::
;   XOR   A
;   LD    (sound_play), A

   LDHL  SP, #2

   PUSH  BC

   LD    A, (HL)
   ADD   A, A

   LD    D, #0
   LD    E, A

   LD    HL, #playtable
   ADD   HL, DE

   LDH   A, (_rampage)
   LD    D, A
   LDH   A, (_rompage)
   LD    E, A
   PUSH  DE

   LD    A, #_ram_page_sound
   LDH   (_rampage), A
   LDH   (P_SVBK), A
   LD    A, (HLI)
   LD    (song_page), A
   LDH   (_rompage), A
   LD    (0x2000), A

   LD    A, (HL)

   PUSH  AF
   CALL  0x4000
   POP   AF

   CALL  0x4006

   POP   DE

   CALL  _soundlib_deinit

   POP   BC

   LD    A, #1
   LD    (sound_play), A

   RET

;----------------------------------------------------------------------------
;[ void SOUNDLIB_sfx(ubyte sfx) ]
;----------------------------------------------------------------------------
_SOUNDLIB_sfx::
_Audio_SFX_Play::
   LDHL  SP, #2
   CALL  _soundlib_setup
   LD    A, (HL)
   CALL  0x400F
   JR    _soundlib_deinit

_Audio_SFX_PlayNote::
   LDHL  SP, #2
   CALL  _soundlib_setup
   LD    A, (HLI)
   LD    L, (HL)
   CALL  0x400F
   JR    _soundlib_deinit
