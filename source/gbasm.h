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
; GB ASM Header
;
; for Affinix by Hideaki Omuro
;----------------------------------------------------------------------------

C_PA_VRAM_PT        = 0x8000
C_PP_VRAM_PT        = 0x80
C_PA_VRAM_NT        = 0x9800
C_PP_VRAM_NT        = 0x98
C_PA_VRAM_WT        = 0x9C00
C_PP_VRAM_WT        = 0x9C

C_PA_BigSpriteTable = 0xC000
C_PP_BigSpriteTable = 0xC0
C_PA_SpriteTable1   = 0xC100
C_PP_SpriteTable1   = 0xC1
C_PA_SpriteTable2   = 0xC200
C_PP_SpriteTable2   = 0xC2
C_PA_FontLenData    = 0x4700
C_PP_FontLenData    = 0x47
C_PA_FontData       = 0x4800
C_PP_FontData       = 0x48

;P_Joypad          = 0x00
;P_SerialData      = 0x01
;P_SerialCtrl      = 0x02
;P_DIV             = 0x03
;P_TimerCount      = 0x04
;P_TimerReload     = 0x05
;P_TimerCtrl       = 0x06
;P_LCDCtrl         = 0x40
;P_LCDStatus       = 0x41
;P_LCDSCY          = 0x42
;P_LCDSCX          = 0x43
;P_LCDLine         = 0x44
;P_LCDLineCmp      = 0x45
;P_OAMDMA          = 0x46
;P_PalBKG          = 0x47
;P_PalSPR0         = 0x48
;P_PalSPR1         = 0x49
;P_WINScrollY      = 0x4A
;P_WINScrollX      = 0x4B
;P_CPUSpeed        = 0x4D
;P_VRAMBank        = 0x4F
;P_BKGPALIndex     = 0x68
;P_BKGPALData      = 0x69
;P_OAMPALIndex     = 0x6A
;P_OAMPALData      = 0x6B

;P_LCDScrollX      = 0x42 ;biff
;P_LCDScrollY      = 0x43

P_P1              = 0x00
P_SB              = 0x01
P_SC              = 0x02
P_DIV             = 0x04
P_TIMA            = 0x05
P_TMA             = 0x06
P_TAC             = 0x07
P_IF              = 0x0F
P_IE              = 0xFF

P_LCDC            = 0x40
P_STAT            = 0x41
P_SCY             = 0x42
P_SCX             = 0x43
P_LY              = 0x44
P_LYC             = 0x45
P_DMA             = 0x46
P_BGP             = 0x47
P_OBP0            = 0x48
P_OBP1            = 0x49
P_WY              = 0x4A
P_WX              = 0x4B
P_KEY1            = 0x4D
P_VBK             = 0x4F
P_HDMA1           = 0x51
P_HDMA2           = 0x52
P_HDMA3           = 0x53
P_HDMA4           = 0x54
P_HDMA5           = 0x55
P_RP              = 0x56
P_BCPS            = 0x68
P_BCPD            = 0x69
P_OCPS            = 0x6A
P_OCPD            = 0x6B
P_SVBK            = 0x70
