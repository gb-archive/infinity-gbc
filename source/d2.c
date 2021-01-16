/*
 * Copyright (C) 1999-2001 Affinix Software, LLC
 *
 * This file is part of Infinity.
 *
 * This file may be used under the terms of the Creative Commons Attribution-
 * NonCommercial-ShareAlike 4.0 International License as published by Creative
 * Commons.
 *
 * Alternatively, this file may be used under the terms of the GNU General
 * Public License as published by the Free Software Foundation, either version
 * 3 of the License, or (at your option) any later version.
 *
 * In addition, as a special exception, Affinix Software gives you certain
 * additional rights. These rights are described in the LICENSE file in this
 * package.
 */

#include"defs.h"

/***** GEMS *****

5 catagories
------------
  - Melee (Conan M)
    Onyx - attack x3
    Turquoise - two turns
    Jade - recv'ed damage /4

  - Life (Goddess F)
    Diamond - revive
    Emerald - heal all (partial)
    Pearl - heal x3

  - Paranormal (Sorceress F)
    Sapphire - special damage reduced to 1
    Carnelian - time freeze
    Crystal - revive when you die (lose a turn)

  - Chaos (Fern ?)
    Ruby - fire x3
    Aquamarine - water x3
    Amethyst - wind x3

  - Trickery (Wizard M)
    Coral - no enemy encounter
    Garnet - escape from battle
    Moonstone - town warp

 ***** END *****/


/* ITEMS ***

    0 - blank

    1 - HPLO
    2 - HPHI
    3 - SPLO
    4 - HPSP
    5 - STRUP
    6 - ENDUP
    7 - AGLUP
    8 - WISUP
    9 - Bomb
   10 - LifeDoll
   11 - Rope
   12 - SlowScroll

   13 - Water
   14 - Contract

LongSwords -- Conner/Roland
   20 - Conscript
   21 - Iron
   22 - Scimitar
   23 - Crimson
   24 - Rapier
   25 - Saber
   26 - Cutlass
   27 - Gladius
   28 - Katana
   29 - Falchion
   30 - Frozen
   31 - Gem
   32 - Ancestral

BroadSwords -- Conner/Roland
   33 - Bastard
   34 - Flamberge
   35 - Claymore
   36 - Zweihander
   37 - Ancient
   38 - Frey

Arrows -- Ryan
   40 - Basic
   41 - Sharp
   42 - Pirate
   43 - Blessed

Staves -- Elya
   44 - Oak
   45 - Thorn
   46 - Traveler
   47 - Master
   48 - Cudgel
   49 - War
   50 - Archmage
   51 - Sea
   52 - Gilus

Claws  -- Victor
   53 - Wolf
   54 - Iron
   55 - Bear
   56 - Omega
   57 - Dino

Armor
   60 - Tunic     - CR-VO
   61 - Leather      - CR-VO
   62 - Studded      - CR-VO
   63 - Apprentice (R)  - --E--
   64 - Hunter    - CR-V-
   65 - Hunter (R)   - --E--
   66 - Wolf      - ---V-
   67 - Chain
   68 - Fancy
   69 - Scale
   70 - Master
   71 - Pathfinder
   72 - Hide
   73 - Plate
   74 - Maiden
   75 - Noble
   76 - Knight
   77 - Cleopatra
   78 - Annubis
   79 - Valkyrie
   80 - Thor
   81 - Atlantian
   82 - Archmage
   83 - Sacred
   84 - Alutha

Accessories
   85 - Cloak (C) - CRE-OA
   86 - Round (S) - C---OA
   87 - Traveler (B) - CREVOA
   88 - Desert (R)   - CREVOA
   89 - Wizard (R)   - CREVOA
   90 - Camoflauge (C)  - CRE-OA
   91 - Golden (N)   - CREVOA
   92 - Iron (S)
   93 - Slippers (B)
   94 - Pirate (R)

Gems
  100 - Ruby
  101 - Coral
  102 - Emerald
  103 - Diamond
  104 - Sapphire
  105 - Opal      (NOT USED!)
  106 - Amethyst
  107 - Garnet
  108 - Crystal
  109 - Jade
  110 - Onyx
  111 - Pearl
  112 - Aquamarine
  113 - Turquoise
  114 - Carnelian
  115 - Moonstone

Money
  116 - 50 Aurum
  117 - 100 Aurum
  118 - 200 Aurum
  119 - 500 Aurum
  120 - 1000 Aurum
  121 - 2000 Aurum
  122 - 5000 Aurum

Icon:
   A - None
   B - Gem
   C - Sword
   D - Arrows
   E - Staff
   F - Claws
   G - Shield
        H - Armor
        I - Cape
        J - Shoes
        K - Ring
        L - Necklace
        M - Helm
        N - Bottle
   O - Broadsword
   P - Robe
        Q - Herb
        R - Mushroom
        S - Lifedoll
        T - Flame
        U - Water
        V - Air
        W - Earth

***/

char label1[] = "InfinityItemsNames";

/*

Vervain, Germander, Currae for healing items

Mycelia, Saproph, Hyphae

Ungent, Tincture, Elixir for the party healers

LifeDoll, Marionette, Mannequin  (for the reviving item)

*/

UBYTE item_name[][20] = {
   // 0 -- Blank
   "A",

   // 1 - 19 -- Items
   "QVervain",
   "RMycelia",
   "NPotion",
   "SLifeOrb",
   "QGermander",
   "RSaproph",
   "NPlus Brew",
   "SForceOrb",
   "QCurrae",
   "RHyphae",
   "NBooster",
   "SReviveOrb",
   "NWater",
   "AContract",
   "",
   "",
   "",
   "",
   "",

   // 20 - 32 -- LongSwords
   "CConscript",
   "CIron",
   "CScimitar",
   "CCrimson",
   "CRapier",
   "CSaber",
   "CCutlas",
   "CGladius",
   "CKatana",
   "CFalchion",
   "CFrozen",
   "CGem",
   "CAncestral",

   // 33 - 38 -- BroadSwords
   "OBastard",
   "OFlamberge",
   "OClaymore",
   "OZweihander",
   "OAncient",
   "OFrey",

   // blank -- 39
   "",

   // 40 - 43 -- Arrows
   "DBasic",
   "DSharp",
   "DPirate",
   "DBlessed",

   // 44 - 52 -- Staves
   "EOak",
   "EThorn",
   "ETraveler",
   "EMaster",
   "ECudgel",
   "EWar",
   "EArchmage",
   "ESea",
   "EGilus",

   // 53 - 57 -- Claws
   "FWolf",
   "FIron",
   "FBear",
   "FOmega",
   "FDino",

   // blank -- 58,59
   "",
   "",

   // 60 - 84 -- Armor
   "HTunic",
   "HLeather",
   "HStudded",
   "PApprentice",
   "HHunter",
   "PHunter",
   "HWolf",
   "",
   "",
   "",
   "",
   "",
   "",
   "",
   "",
   "",
   "",
   "",
   "",
   "",
   "",
   "",
   "",
   "",
   "",

   // 85 - 94 -- Accessories
   "ICloak",
   "GRound",
   "JTraveler",
   "KDesert",
   "KWizard",
   "ICamoflauge",
   "LGolden",
   "",
   "",
   "",

   // blank -- 95 - 99
   "",
   "",
   "",
   "",
   "",

   // 100 - 115 -- Gems
   "BRuby",
   "BCoral",
   "BEmerald",
   "BDiamond",

   "BSapphire",
   "BOpal",
   "BAmethyst",
   "BGarnet",

   "BCrystal",
   "BJade",
   "BOnyx",
   "BPearl",

   "BAquamarine",
   "BTurquoise",
   "BCarnelian",
   "BMoonstone",

   // 116+ money

   "A50 Aurum",
   "A100 Aurum",
   "A200 Aurum",
   "A500 Aurum",
   "A1000 Aurum",
   "A2000 Aurum",
   "A5000 Aurum",
};

/** item info

   0 - unusabled
   1 - item
   2 - weapon
   3 - shield
   4 - accessory
   5 - gem

   format:
    type, str, end, agl, wis, value, equipmask

   value:
    gems = timeout or cost
    anything else = attribute

   equipmask:
     bit 0: Conner   1
         1: Ryan  2
         2: Elya  4
         3: Victor   8
         4: Roland   16
         5: Alutha      32

   attribute:
     bit 0: Fire  1
         1: Water 2
         2: Wind  4

**/

char label2[] = "InfinityItemsData";

BYTE item_data[][7] = {
   // blank
   { 0, 0, 0, 0, 0, 0, 0 },

   // items
   { 1, 0, 0, 0, 0, 0, 0 },   // HP
   { 1, 0, 0, 0, 0, 0, 0 },   // SP
   { 1, 1, 0, 0, 0, 0, 0 },   // HAll
   { 1, 0, 1, 0, 0, 0, 0 },   // Life
   { 1, 0, 0, 0, 0, 0, 0 },
   { 1, 0, 0, 0, 0, 0, 0 },
   { 1, 1, 0, 0, 0, 0, 0 },
   { 1, 0, 1, 0, 0, 0, 0 },
   { 1, 0, 0, 0, 0, 0, 0 },
   { 1, 0, 0, 0, 0, 0, 0 },
   { 1, 1, 0, 0, 0, 0, 0 },
   { 1, 0, 1, 0, 0, 0, 0 },
   { 0, 0, 0, 0, 0, 0, 0 },
   { 0, 0, 0, 0, 0, 0, 0 },
   { 0, 0, 0, 0, 0, 0, 0 },
   { 0, 0, 0, 0, 0, 0, 0 },
   { 0, 0, 0, 0, 0, 0, 0 },
   { 0, 0, 0, 0, 0, 0, 0 },
   { 0, 0, 0, 0, 0, 0, 0 },

   // LongSwords -- 20 - 32
   { 2, 7, 0, 0, 0, 0, 1 },   // Conscript
   { 2, 9, 0, 0, 0, 0, 1 },   // Iron
   { 2,12, 0, 1, 0, 0, 1 },   // Scimitar
   { 2,11, 0, 0, 0, 1, 0 },   // Crimson
   { 2,14, 0,-2, 0, 0, 0 },   // Rapier
   { 0, 0, 0, 0, 0, 0, 0 },
   { 0, 0, 0, 0, 0, 0, 0 },
   { 0, 0, 0, 0, 0, 0, 0 },
   { 0, 0, 0, 0, 0, 0, 0 },
   { 0, 0, 0, 0, 0, 0, 0 },
   { 0, 0, 0, 0, 0, 0, 0 },
   { 0, 0, 0, 0, 0, 0, 0 },
   { 0, 0, 0, 0, 0, 0, 0 },

   // BroadSwords -- 33 - 38
   { 0, 0, 0, 0, 0, 0, 0 },
   { 0, 0, 0, 0, 0, 0, 0 },
   { 0, 0, 0, 0, 0, 0, 0 },
   { 0, 0, 0, 0, 0, 0, 0 },
   { 0, 0, 0, 0, 0, 0, 0 },
   { 0, 0, 0, 0, 0, 0, 0 },

   // blank -- 39
   { 0, 0, 0, 0, 0, 0, 0 },

   // Arrows -- 40 - 43
   { 2, 6, 0, 4, 0, 0, 2 },   // Basic
   { 2, 9, 0, 4, 0, 0, 2 },   // Sharp
   { 0, 0, 0, 0, 0, 0, 0 },
   { 0, 0, 0, 0, 0, 0, 0 },

   // Staves -- 44 - 52
   { 2,11, 0, 0, 0, 0, 4 },   // Oak
   { 2,14, 2, 0, 0, 0, 4 },   // Thorn
   { 2,18, 0, 5, 0, 0, 4 },   // Traveler
   { 0, 0, 0, 0, 0, 0, 0 },
   { 0, 0, 0, 0, 0, 0, 0 },
   { 0, 0, 0, 0, 0, 0, 0 },
   { 0, 0, 0, 0, 0, 0, 0 },
   { 0, 0, 0, 0, 0, 0, 0 },
   { 0, 0, 0, 0, 0, 0, 0 },

   // Claws -- 53 - 57
   { 2,15, 0, 0, 0, 0, 8 },   // Wolf
   { 0, 0, 0, 0, 0, 0, 0 },
   { 0, 0, 0, 0, 0, 0, 0 },
   { 0, 0, 0, 0, 0, 0, 0 },
   { 0, 0, 0, 0, 0, 0, 0 },

   // blank -- 58 - 59
   { 0, 0, 0, 0, 0, 0, 0 },
   { 0, 0, 0, 0, 0, 0, 0 },

/*
     bit 0: Conner   1
         1: Ryan  2
         2: Elya  4
         3: Victor   8
         4: Roland   16
         5: Alutha      32
*/
   // Armor -- 60 - 84
   { 3, 0, 5, 0, 0, 0,27 },   // Tunic CR-VO
   { 3, 0, 7, 0, 0, 0,27 },   // Leather  CR-VO
   { 3, 0,10, 0, 0, 0,27 },   // Studded  CR-VO
   { 3, 0, 8, 0, 2, 0, 4 },   // Apprentice(R)--E--
   { 3, 0,10, 4, 2, 0,11 },   // Hunter   CR-V-
   { 3, 0, 9, 4, 0, 0, 4 },   // Hunder (R)  --E--
   { 3, 0,10, 3,-2, 0, 8 },   // Wolf     ---V-

   { 0, 0, 0, 0, 0, 0, 0 },
   { 0, 0, 0, 0, 0, 0, 0 },
   { 0, 0, 0, 0, 0, 0, 0 },
   { 0, 0, 0, 0, 0, 0, 0 },
   { 0, 0, 0, 0, 0, 0, 0 },
   { 0, 0, 0, 0, 0, 0, 0 },
   { 0, 0, 0, 0, 0, 0, 0 },
   { 0, 0, 0, 0, 0, 0, 0 },
   { 0, 0, 0, 0, 0, 0, 0 },
   { 0, 0, 0, 0, 0, 0, 0 },
   { 0, 0, 0, 0, 0, 0, 0 },
   { 0, 0, 0, 0, 0, 0, 0 },
   { 0, 0, 0, 0, 0, 0, 0 },
   { 0, 0, 0, 0, 0, 0, 0 },
   { 0, 0, 0, 0, 0, 0, 0 },
   { 0, 0, 0, 0, 0, 0, 0 },
   { 0, 0, 0, 0, 0, 0, 0 },
   { 0, 0, 0, 0, 0, 0, 0 },

   // Accessories -- 85 - 94
   { 4, 0, 2,-2, 0, 0,55 },   // Cloak (cape)      CRE-OA
   { 4,-1, 4, 0, 0, 0,49 },   // Round (shield) C---OA
   { 4, 0, 1, 7, 0, 0,63 },   // Traveler (boots)  CREVOA
   { 4, 0, 0, 0, 0, 1,63 },   // Desert (ring)  CREVOA
   { 4, 0, 0, 0, 5, 0,63 },   // Wizard (ring)  CREVOA
   { 4, 0, 0, 0, 0, 0,55 },   // Camoflauge (cape) CRE-OA
   { 4, 0, 0, 0, 0, 0,63 },   // Golden (necklace) CREOVA
   { 0, 0, 0, 0, 0, 0, 0 },
   { 0, 0, 0, 0, 0, 0, 0 },
   { 0, 0, 0, 0, 0, 0, 0 },

   // blank -- 95 - 99
   { 0, 0, 0, 0, 0, 0, 0 },
   { 0, 0, 0, 0, 0, 0, 0 },
   { 0, 0, 0, 0, 0, 0, 0 },
   { 0, 0, 0, 0, 0, 0, 0 },
   { 0, 0, 0, 0, 0, 0, 0 },

   // Gems
   { 5, 0, 0, 0, 0, 112,    63 },   // ruby     -- 48+64     = 112 =  112
   { 5, 0, 0, 0, 0,-108,    63 },   //    -- 20+128    = 148 = -108
   { 5, 0, 0, 0, 0, -59,    63 },   //    --  5+128+64 = 197 =  -59
   { 5, 0, 0, 0, 0, -59,    63 },   //    --  5+128+64 = 197 =  -59
   { 5, 0, 0, 0, 0,  96,    63 },   //    -- 32+64     =  96 =   96
   { 5, 0, 0, 0, 0,   0,     0 },   // opal
   { 5, 0, 0, 0, 0, 112,    63 },   //    -- 48+64     = 112 =  112
   { 5, 0, 0, 0, 0,  72,    63 },   //    --  8+64     =  72 =   72
   { 5, 0, 0, 0, 0, 112,    63 },   //    -- 48+64     = 112 =  112
   { 5, 0, 0, 0, 0, 112,    63 },   //    -- 48+64     = 112 =  112
   { 5, 0, 0, 0, 0, 112,    63 },   //    -- 48+64     = 112 =  112
   { 5, 0, 0, 0, 0, 112,    63 },   //    -- 48+64     = 112 =  112
   { 5, 0, 0, 0, 0, 112,    63 },   //    -- 48+64     = 112 =  112
   { 5, 0, 0, 0, 0,  88,    63 },   //    -- 24+64     =  88 =   88
   { 5, 0, 0, 0, 0,  80,    63 },   //    -- 16+64     =  80 =   80
   { 5, 0, 0, 0, 0,-118,    63 },   //    -- 10+128    = 138 = -118

   /*{ 2, 1, 0, 0, 0, 0, 0 },
   { 2,50, 0, 0, 0, 0, 0 },
   { 3, 0, 1, 0, 0, 0, 0 },
   { 3, 0,60, 0, 0, 0, 0 },
   { 4, 0, 0, 5,10, 0, 0 },
   { 3, 0,20, 0, 0, 0, 0 },
   { 1, 0, 0, 0, 0, 0, 0 },
   { 0, 0, 0, 0, 0, 0, 0 }, // key*/
};

char label3[] = "InfinityItemsValue";

UWORD item_selldata[] = {
   // blank
   0,

   // items -- 1 - 19
   25,75,80,75,  100,200,350,150,  300,400,1000,250,  100,0,0,0,0,0,0,

   // longswords -- 20 - 32
   0,0,0,0,0,0,0,0,0,0,0,0,0,0,

   // broadswords -- 33 - 38
   0,0,0,0,0,0,

   // blank -- 39
   0,

   // arrows -- 40 - 43
   0,0,0,0,

   // staves -- 44 - 52
   0,0,0,0,0,0,0,0,0,

   // claws -- 53 - 57
   0,0,0,0,0,

   // blank -- 58 - 59
   0,0,

   // armor -- 60 - 84
   10,50,125,75,225,225,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,

   // accessories -- 85 - 94
   20,30,45,100,80,125,400,0,0,0,

   // blank -- 95 - 99
   0,0,0,0,0,
};

#include "data/slurp.h"
#include "data/spawn1.h"
#include "data/slash1.h"
#include "data/whirl1.h"
