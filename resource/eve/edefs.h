#include "formdefs.h"

#define CEL_DOWN1       0
#define CEL_DOWN2       1
#define CEL_LEFT1       2
#define CEL_LEFT2       3
#define CEL_RIGHT1      4
#define CEL_RIGHT2      5
#define CEL_UP1         6
#define CEL_UP2         7
#define CEL_WIPE1      36
#define CEL_WIPE2      37
#define CEL_LOOK1      38
#define CEL_LOOK2      39
#define CEL_SIT1       40
#define CEL_HRAISE     44
#define CEL_ATTEN      45
#define CEL_TIRED      46
#define CEL_ASLEEP     47

#define CEL_SKEL1       4
#define CEL_SKEL2       5

#define PARTY_CONNOR    1
#define PARTY_RYAN      2
#define PARTY_ELYA      3
#define PARTY_VICTOR    4
#define PARTY_ROLAND    5
#define PARTY_ALUTHA    6

#define CH_CONNOR       0
#define CH_RYAN         1
#define CH_ELYA         2
#define CH_VICTOR       3
#define CH_ROLAND       4
#define CH_ALUTHA       5
#define CH_VICTORWOLF   6


#define EN_YAK          7
#define EN_FLAME        13
#define EN_GUTANA       14
#define EN_TWOLF1       15
#define EN_TWOLF2       16
#define EN_TWOLF3       17
#define EN_ROOT         18
#define EN_TREE1        19
#define EN_GARROW       30
#define EN_HENCH        31
#define EN_RAGNOP       32

#define EN_EXANIMA      42
#define EN_BSARU        44

#define EN_PIRATE       51
#define EN_SSARU        53

#define EN_GOLEM2       67
#define EN_GOLEM3       68
#define EN_GOLEM4       69
#define EN_OMEGA        70


#define NPC_MAN1        150
#define NPC_MAN2        151
#define NPC_LADY1       152
#define NPC_LADY2       153
#define NPC_LADY3       154
#define NPC_LADY4       155
#define NPC_LADY5       156
#define NPC_BOY1        157
#define NPC_BOY2        158
#define NPC_GIRL1       159
#define NPC_GIRL2       160
#define NPC_SOLDIER     161
#define NPC_MAYOR       162
#define NPC_KIDRAGON    163
#define NPC_KIDHERO     164
#define NPC_GOLEM       165
#define NPC_KING        166
#define NPC_ADVISOR     167
#define NPC_WOLF        168
#define NPC_KRAIT       169
#define NPC_DEAD        170
#define NPC_HASSAN      171
#define NPC_BOAT        172
#define NPC_FIRE        173
#define NPC_OLDMAN      174
#define NPC_OLDLADY     175
#define NPC_GEIST       176
#define NPC_ARCHMAGI    177
#define NPC_KIDMAGI     178
#define NPC_OLDMAGI     179
#define NPC_WOOMAGI     180
#define NPC_SOULGEM     181
#define NPC_SMOKE       182
#define NPC_EX1         183
#define NPC_FIRE2 184
#define NPC_BARRIER     185
#define NPC_FIRE4       186
#define NPC_FIRE5       187
#define NPC_FIRE6       188
#define NPC_FIRE7       189
#define NPC_FIRE8       190
#define NPC_WAKE2A      191
#define NPC_WAKE2B      192
#define NPC_PORTALA     193
#define NPC_PORTALB     194
#define NPC_FIRE3       195
#define NPC_KRAIT2      196
#define NPC_GILUS       197
#define NPC_SELERA      198
#define NPC_ANNA  199
#define NPC_EMBRACE     200
#define NPC_EXH         201
#define NPC_UNNAMED  202
#define NPC_HEAD1 203
#define NPC_HEAD2 204
#define NPC_HEAD3 205
#define NPC_IZUAR 206

#define NPC_AKING       NPC_KING
#define NPC_YETI  32

#define DEAD_GUARD      2
#define DEAD_MAGI       6
#define DEAD_MAGI2   7

#define ARCH_ARM        0
#define ARCH_KNEEL      1
#define ARCH_DEAD       2

#define MAP_OVERWORLD   0
#define MAP_SELERA      1
#define MAP_SLR_CONNR   2
#define MAP_SLR_HOUS1   3
#define MAP_SLR_HOUS2   4
#define MAP_SLR_INN     5
#define MAP_SLR_WEAPN   6
#define MAP_SLR_ITEM    7
#define MAP_TAGEL       8
#define MAP_TGL_GATE    9
#define MAP_TGL_HALL   10
#define MAP_TGL_KING   11
#define MAP_TGL_THRON  12
#define MAP_TGL_BASE1  13
#define MAP_TGL_BASE2  14
#define MAP_TGL_TOW1   15
#define MAP_TGL_TOW2   16
#define MAP_TGL_TOW3   17
#define MAP_LAB        18
#define MAP_NPS_P1     19
#define MAP_NPS_P2     20
#define MAP_NPS_P3     21
#define MAP_NPS_P4     22
#define MAP_NPS_P5     23
#define MAP_NPS_P6     24
#define MAP_XCV_P1     25
#define MAP_XCV_P2     26
#define MAP_XCV_P3     27
#define MAP_XCV_P4     28
#define MAP_XCV_P5     29
#define MAP_KASSIM     30
#define MAP_KSM_INN    31
#define MAP_KSM_HOUS1  32
#define MAP_KSM_HOUS2  33
#define MAP_KSM_HOUS3  34

#define MAP_FRC_P1     35
#define MAP_FRC_P2     36
#define MAP_FRC_P3     37
#define MAP_FRC_P4     38
#define MAP_FRC_P5     39
#define MAP_FRC_P6     40
#define MAP_FRC_P7     41
#define MAP_FRC_P8     42
#define MAP_FRC_P9     43
#define MAP_FRC_P10    44

#define MAP_TURREN     45
#define MAP_COTTAGE    46
#define MAP_COT_INN    47
#define MAP_TUR_P1     48
#define MAP_TUR_P2     49
#define MAP_TUR_P3     50
#define MAP_TUR_P4     51
#define MAP_GRW        52
#define MAP_TUR_P5     53

#define MAP_HOL        54
#define MAP_HOL_HOUS1  55
#define MAP_HOL_HOUS2  56
#define MAP_HOL_INN    MAP_COT_INN
#define MAP_HOL_ITEM   57
#define MAP_HOL_WEP    58

#define MAP_DUR        59

#define MAP_SEW_P1     60
#define MAP_SEW_P2     61
#define MAP_SEW_P3     62
#define MAP_SEW_INN    63
#define MAP_SEW_P4     64

#define MAP_DUR_P1     65
#define MAP_DUR_P2     66
#define MAP_DUR_P3     67
#define MAP_DUR_P4     68

#define MAP_SHIP       69

#define MAP_FAR        70
#define MAP_FAR_INN1   71
#define MAP_FAR_INN2   72
#define MAP_FAR_MNSA   73
#define MAP_FAR_MNSB   74
#define MAP_FAR_MNSC   75
#define MAP_FAR_PUB    76
#define MAP_FAR_ITEM   77
#define MAP_FAR_WEP    78
#define MAP_FAR_HOUS1  79
#define MAP_FAR_HOUS2  80
#define MAP_FAR_HOUS3  81

#define MAP_PIR_P1     82
#define MAP_PIR_P2     83
#define MAP_PIR_P3     84
#define MAP_PIR_P4     85

#define MAP_COS_P1     86
#define MAP_COS_P2     87
#define MAP_COS_P3     88

#define MAP_MAG_P1     89
#define MAP_MAG_P2     90
#define MAP_MAG_P3     91
#define MAP_MAG_P4     92
#define MAP_MAG_P5     93
#define MAP_MAG_P6     94
#define MAP_MAG_ARCH   95
#define MAP_MAG_SHOP   96
#define MAP_MAG_INN    97
#define MAP_MAG_LIB    98

#define MAP_TRIAL1     99
#define MAP_TRIAL2     100
#define MAP_TRIAL3     101

#define MAP_SNOW       102
#define MAP_SNOW2      103

#define MAP_ALT        104
#define MAP_ALT_HOUS   105

#define MAP_ALU        106
#define MAP_ALU_BASE   107
#define MAP_ALU_THRON  108
#define MAP_ALU_HALL   109
#define MAP_ALU_TOW1   110
#define MAP_ALU_TOW2   111
#define MAP_ALU_TOW3   112
#define MAP_ALU_TOW4   113

#define MAP_WMAG_P1    114
#define MAP_WMAG_P2    115
#define MAP_WMAG_P3    116
#define MAP_WMAG_P4    117
#define MAP_WMAG_P5    118
#define MAP_WMAG_P6    119
#define MAP_WMAG_ARCH  120
#define MAP_WMAG_SHOP  121
#define MAP_WMAG_INN   122
#define MAP_WMAG_LIB   123

#define MAP_WTAGEL     124
#define MAP_WSELERA    125
#define MAP_WKASSIM    126
#define MAP_ALU_CRYPT   127

#define MAP_SNOW3      128
#define MAP_SNOW4      129
#define MAP_SNOW5      130
#define MAP_SNOW6      131

#define MAP_ICE_P1     132
#define MAP_ICE_P2     133
#define MAP_ICE_P3     134
#define MAP_ICE_P4     135
#define MAP_ICE_P5     136
#define MAP_ICE_P6     137

#define MAP_SND_P1     138
#define MAP_SND_P2     139
#define MAP_SND_P3     140
#define MAP_SND_P4     141
#define MAP_SND_P5     142
#define MAP_SND_P6     143
#define MAP_SND_P7     144

#define MAP_WAV_P1     145
#define MAP_WAV_P2     146
#define MAP_WAV_P3     147
#define MAP_WAV_P4     148
#define MAP_WAV_P5     149
#define MAP_WAV_P6     150
#define MAP_WAV_P7     151
#define MAP_WAV_P8     152

#define MAP_GRDKISLE   153

#define MAP_FIN_P1     154
#define MAP_FIN_P2     155
#define MAP_FIN_P3     156
#define MAP_FIN_P4     157
#define MAP_FIN_P5     158
#define MAP_FIN_P6     159
#define MAP_FIN_P7     160
#define MAP_FIN_P8     161
#define MAP_FIN_P9     162

#define MAP_JSELERA    163

#define MAP_ALU_GARDEN 164
#define MAP_DUR_P5     165
#define MAP_FARM       166
#define MAP_THEEND     167
#define MAP_CLIFF      168


#define MAP_BATTLE     180

#include"sfx.h"

#define VAR_EVEPROG     0
#define VAR_WORLDX      1
#define VAR_WORLDY      2
#define VAR_LAB         3
#define VAR_HASSAN1     4
#define VAR_FRCFLAME    5
#define VAR_GUTANA      6
#define VAR_TURWOLVES   7
#define VAR_WELLFIRST   8
#define VAR_A           9
#define VAR_B          10
#define VAR_C          11
#define VAR_TURWOODS   12
#define VAR_TURTRES1   13
#define VAR_GRW        14
#define VAR_NPS6_G1    15
#define VAR_NPS6_G2    16
#define VAR_NPS6_G3    17
#define VAR_TURMAYOR   18

#define VAR_HOL1       19
#define VAR_HOL2       20

#define VAR_DURTOP     21

#define VAR_SEWWATER   22
#define VAR_SEW1       23
#define VAR_SEWHIT     24
#define VAR_SEW2       25

#define VAR_TGLESC     26
#define VAR_TGLGEM     27

#define VAR_DUR1       28
#define VAR_DURBLOCK   29

#define VAR_PIRDONE    30
#define VAR_PIRSW1     31
#define VAR_PIRSW2     32
#define VAR_PIRSW3     33
#define VAR_PIRSW4     34
#define VAR_PIRSW5     35
#define VAR_PIRSW6     36
#define VAR_PIRDOOR1   37
#define VAR_PIRDOOR2   38
#define VAR_PIRDOOR3   39
#define VAR_PIRDOOR4   40
#define VAR_PIRDOOR5   41
#define VAR_PIRDOOR6   42
#define VAR_PIRDOOR7   43
#define VAR_PIROPEN    44
#define VAR_PIRTRAP1   45
#define VAR_PIRTRAP2   46

#define VAR_MAGVISIT   47
#define VAR_MAG1       48

; VAR_RET_TUR
;   0 - all good
;   1 - turren empty except for mayor, grw blocked
;   2 - turren empty completely, grw still blocked
;   3 - turren empty completely, grw no longer blocked
#define VAR_RET_TUR    50

; VAR_KSM1
;   0 - kassim is normal
;   1 - kassim is under attack
;   2 - kassim after attack
#define VAR_KSM1     51

; VAR_FRC1
;   0 - normal
;   1 - run firecave event
#define VAR_FRC1     52

; VAR_FRC2
;   0 - all good
;   1 - no exiting on kassim side
#define VAR_FRC2     53

; VAR_RET_GRW
;   0 - nothing
;   1 - show scene41
#define VAR_RET_GRW  54

; VAR_RET_DUR
;   0 - normal
;   1 - trashed before visit
;   2 - trashed after visit
#define VAR_RET_DUR  55

; VAR_RET_MAG
;   0 - normal
;   1 - trashed before visit
;   2 - trashed after intro talk
;   3 - trashed after archmage death
#define VAR_RET_MAG  56

; VAR_ALU1
;   0 - before war
;   1 - after war
#define VAR_ALU1     57

; VAR_ALU2
;   0 - king room blocked
;   1 - no block
#define VAR_ALU2     58

#define VAR_TGL1     59

#define VAR_NONLINEAR   60

#define VAR_RET_SLR  61

#define VAR_PIR_CHEST1 62

#define VAR_SEW3      63
#define VAR_NPSBLOCK  64

#define VAR_SNO1      65

; VAR_TURTRES
;   1 - treasure taken
#define VAR_TURTRES   66

#define VAR_SND1      67
#define VAR_SNDC1     68
#define VAR_SNDC2     69
#define VAR_SNDC3     70
#define VAR_SNDC4     71
#define VAR_SND2      72

#define VAR_WAVHIT    73
#define VAR_WAVWATER1 74
#define VAR_WAVWATER2 75

#define VAR_GRD1      76
#define VAR_GRDREADY  77

; VAR_FIN1
;   0 - started
;   1 - after midscene
;   2 - after unnamed killing
;   3 - after getting alutha
#define VAR_FIN1      78

#define VAR_JOY1A     79

#define VAR_ALUSAIL   80
#define VAR_BURNED    81

#define VAR_ICE_DONE  82
#define VAR_SND_DONE  83
#define VAR_WAV_DONE  84

#define VAR_ATDEMO    85

#define VAR_FINFIND1  86
#define VAR_FINFIND2  87

#define VAR_ARCHITEM  88

#define VAR_JOY2A     89

#define VAR_BOATMODE  90

#define VAR_ANNATOMB  91

#define F_TREAS       100

#define IT_HPLO         1

#define IT_HPLo         1
#define IT_SPLo         2
#define IT_HALo         3
#define IT_LFLo         4

#define IT_HPMd         5
#define IT_SPMd         6
#define IT_HAMd         7
#define IT_LFMd         8

#define IT_HPHi         9
#define IT_SPHi        10
#define IT_HAHi        11
#define IT_LFHi        12


#define IT_WATER       13
#define IT_CONTRACT    14

#define IT_AU_50       116
#define IT_AU_100      117
#define IT_AU_200      118
#define IT_AU_500      119
#define IT_AU_1000     120
#define IT_AU_2000     121
#define IT_AU_5000     122

#define SVL_SELERA      1
#define SVL_KASSIM      2
#define SVL_COTTAGE     3
#define SVL_TURREN      4
#define SVL_HOL         5
#define SVL_SEW         6
#define SVL_FAR         7
#define SVL_SHIP        8
#define SVL_MAG         9
#define SVL_OVER        10
#define SVL_CALUTHA  11
#define SVL_SNO      12
#define SVL_SND         13
#define SVL_FIN         14


#define X_FADE          0
#define X_ROLL          1
#define X_FADESLOW   2
#define X_FADEWHITE   3

#define C_BLACK      0
#define C_WHITE      1
#define C_DBLUE         5
#define C_BROWN         6

#define SLR_DTSIZE      16
#define FAR_DTSIZE      16
#define ALU_DTSIZE      19
#define MAG_DTSIZE      10
#define JOY1_DTSIZE     11


#define ZONE_NPS        32
#define ZONE_XCV        33
#define ZONE_FRC        34
#define ZONE_GRW        35
#define ZONE_SEW        36
#define ZONE_PIR        37
#define ZONE_COS        38
#define ZONE_TRI        39
#define ZONE_FRC2 40
#define ZONE_GRW2 41
#define ZONE_SNO  42
#define ZONE_SNO2 43
#define ZONE_MAG  44
#define ZONE_ICE  45
#define ZONE_SND  46
#define ZONE_WAV  47

#define BOSSPAL_OMEGA    4

