//
// Copyright(C) 2005-2014 Simon Howard
// Copyright(C) 2016-2023 Julian Nechaevsky
// Copyright(C) 2020-2025 Leonid Murin (Dasperal)
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
//
// Parses Text substitution sections in dehacked files
//



#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "doomtype.h"
#include "id_lang.h"

#include "z_zone.h"

#include "deh_defs.h"
#include "deh_io.h"
#include "deh_htic.h"
#include "deh_main.h"

//
// Ok, Greg, the action pointers thing was bad enough, but this really
// takes the biscuit.  Why does HHE's text replacement address strings
// by offset??!!  The dehacked way was much nicer, why change it?
//

typedef struct
{
    unsigned int offsets[deh_hhe_num_versions];
    char *string;
} hhe_string_t;

//     Offsets                 String
//       v1.0   v1.2   v1.3

static const hhe_string_t strings[] =
{
    { {   228,   228,   228 }, "PLAYPAL" },
    { {  1240,  1252,  1252 }, "E1M1:  THE DOCKS" },
    { {  1260,  1272,  1272 }, "E1M2:  THE DUNGEONS" },
    { {  1280,  1292,  1292 }, "E1M3:  THE GATEHOUSE" },
    { {  1304,  1316,  1316 }, "E1M4:  THE GUARD TOWER" },
    { {  1328,  1340,  1340 }, "E1M5:  THE CITADEL" },
    { {  1348,  1360,  1360 }, "E1M6:  THE CATHEDRAL" },
    { {  1372,  1384,  1384 }, "E1M7:  THE CRYPTS" },
    { {  1392,  1404,  1404 }, "E1M8:  HELL'S MAW" },
    { {  1412,  1424,  1424 }, "E1M9:  THE GRAVEYARD" },
    { {  1436,  1448,  1448 }, "E2M1:  THE CRATER" },
    { {  1456,  1468,  1468 }, "E2M2:  THE LAVA PITS" },
    { {  1480,  1492,  1492 }, "E2M3:  THE RIVER OF FIRE" },
    { {  1508,  1520,  1520 }, "E2M4:  THE ICE GROTTO" },
    { {  1532,  1544,  1544 }, "E2M5:  THE CATACOMBS" },
    { {  1556,  1568,  1568 }, "E2M6:  THE LABYRINTH" },
    { {  1580,  1592,  1592 }, "E2M7:  THE GREAT HALL" },
    { {  1604,  1616,  1616 }, "E2M8:  THE PORTALS OF CHAOS" },
    { {  1632,  1644,  1644 }, "E2M9:  THE GLACIER" },
    { {  1652,  1664,  1664 }, "E3M1:  THE STOREHOUSE" },
    { {  1676,  1688,  1688 }, "E3M2:  THE CESSPOOL" },
    { {  1696,  1708,  1708 }, "E3M3:  THE CONFLUENCE" },
    { {  1720,  1732,  1732 }, "E3M4:  THE AZURE FORTRESS" },
    { {  1748,  1760,  1760 }, "E3M5:  THE OPHIDIAN LAIR" },
    { {  1776,  1788,  1788 }, "E3M6:  THE HALLS OF FEAR" },
    { {  1804,  1816,  1816 }, "E3M7:  THE CHASM" },
    { {  1824,  1836,  1836 }, "E3M8:  D'SPARIL'S KEEP" },
    { {  1848,  1860,  1860 }, "E3M9:  THE AQUIFER" },
    { {     0,  1880,  1880 }, "E4M1:  CATAFALQUE" },
    { {     0,  1900,  1900 }, "E4M2:  BLOCKHOUSE" },
    { {     0,  1920,  1920 }, "E4M3:  AMBULATORY" },
    { {     0,  1940,  1940 }, "E4M4:  SEPULCHER" },
    { {     0,  1960,  1960 }, "E4M5:  GREAT STAIR" },
    { {     0,  1980,  1980 }, "E4M6:  HALLS OF THE APOSTATE" },
    { {     0,  2012,  2012 }, "E4M7:  RAMPARTS OF PERDITION" },
    { {     0,  2044,  2044 }, "E4M8:  SHATTERED BRIDGE" },
    { {     0,  2068,  2068 }, "E4M9:  MAUSOLEUM" },
    { {     0,  2088,  2088 }, "E5M1:  OCHRE CLIFFS" },
    { {     0,  2108,  2108 }, "E5M2:  RAPIDS" },
    { {     0,  2124,  2124 }, "E5M3:  QUAY" },
    { {     0,  2136,  2136 }, "E5M4:  COURTYARD" },
    { {     0,  2156,  2156 }, "E5M5:  HYDRATYR" },
    { {     0,  2172,  2172 }, "E5M6:  COLONNADE" },
    { {     0,  2192,  2192 }, "E5M7:  FOETID MANSE" },
    { {     0,  2212,  2212 }, "E5M8:  FIELD OF JUDGEMENT" },
    { {     0,  2240,  2240 }, "E5M9:  SKEIN OF D'SPARIL" },
    { {  1868,  2268,  2268 }, "AUTOPAGE" },
    { {  1880,  2280,  2280 }, "FOLLOW MODE ON" },
    { {  1896,  2296,  2296 }, "FOLLOW MODE OFF" },
    { {  1924,  2324,  2324 }, "GREEN:  " },
    { {  1936,  2336,  2336 }, "YELLOW:  " },
    { {  1948,  2348,  2348 }, "RED:  " },
    { {  1956,  2356,  2356 }, "BLUE:  " },
    { {  1964,  2364,  2364 }, "FONTA_S" },
    { {  1972,  2372,  2372 }, "-MESSAGE SENT-" },
    { {  1988,  2388,  2388 }, "THERE ARE NO OTHER PLAYERS IN THE GAME!" },
    { {  2028,  2428,  2428 }, "FONTA59" },
    { {  2036,  2504,  2504 }, "PAUSED" },
    { {  2072,  2540,  2540 }, "ADVISOR" },
    { {  2080,  2548,  2548 }, "TITLE" },
    { {  2088,  2556,  2556 }, "demo1" },
    { {  2096,  2564,  2564 }, "CREDIT" },
    { {  2104,  2572,  2572 }, "demo2" },
    { {  2112,  2580,  2580 }, "ORDER" },
    { {  2120,  2588,  2588 }, "demo3" },
    { {  2304,  2696,  2696 }, "Exited from HERETIC.\n" },
    { {  2412,  2800,  2800 }, "c:\\heretic.cd" },
    { {  2528,  2916,  2916 }, "Playing demo %s.lmp.\n" },
    { {  2592,  2980,  2980 }, "V_Init: allocate screens.\n" },
    { {  2620,  3008,  3008 }, "M_LoadDefaults: Load system defaults.\n" },
    { {  2660,  3048,  3048 }, "Z_Init: Init zone memory allocation daemon.\n" },
    { {  2708,  3096,  3096 }, "W_Init: Init WADfiles.\n" },
    { {  2732,  3120,  3120 }, "E2M1" },
    { {     0,  3128,  3128 }, "EXTENDED" },
    { {  2740,  3140,  3140 }, "LOADING" },
    { {  2748,  3148,  3148 }, "DeathMatch..." },
    { {  2764,  3164,  3164 }, "No Monsters..." },
    { {  2780,  3180,  3180 }, "Respawning..." },
    { {  2796,  3196,  3196 }, "Warp to Episode %d, Map %d, Skill %d " },
    { {  2836,  3236,  3236 }, "MN_Init: Init menu system.\n" },
    { {  2864,  3264,  3264 }, "R_Init: Init Heretic refresh daemon." },
    { {  2904,  3304,  3304 }, "Loading graphics" },
    { {  2924,  3324,  3324 }, "P_Init: Init Playloop state." },
    { {  2956,  3356,  3356 }, "Init game engine." },
    { {  2976,  3376,  3376 }, "I_Init: Setting up machine state.\n" },
    { {  3012,  3412,  3412 }, "D_CheckNetGame: Checking network game status.\n" },
    { {  3060,  3460,  3460 }, "Checking network game status." },
    { {  3092,  3492,  3492 }, "SB_Init: Loading patches.\n" },
    { {     0,  3752,  3752 }, "PLAYER 1 LEFT THE GAME" },
    { {  3508,  3932,  3932 }, "Network game synchronization aborted." },
    { {     0,  3972,  3972 }, "Different DOOM versions cannot play a net game!" },
    { {  3908,  4132,  4132 }, "SKY1" },
    { {  3916,  4140,  4140 }, "SKY2" },
    { {  3924,  4148,  4148 }, "SKY3" },
    { {  3736,  4196,  4196 }, "NET GAME" },
    { {  3748,  4208,  4208 }, "SAVE GAME" },
    { {  3760,  4220,  4220 }, "Only %i deathmatch spots, 4 required" },
    { {  3800,  4260,  4260 }, "version %i" },
    { {  3828,  4372,  4372 }, "c:\\heretic.cd\\hticsav%d.hsg" },
    { {  3856,  4400,  4400 }, "hticsav%d.hsg" },
    { {  3896,  4416,  4416 }, "GAME SAVED" },
    { {  4016,  4456,  4456 }, E1TEXT },
    { {  4536,  4976,  4976 }, E2TEXT },
    { {  5068,  5508,  5508 }, E3TEXT },
    { {     0,  6072,  6072 }, E4TEXT },
    { {     0,  6780,  6780 }, E5TEXT },
    { {  5632,  7468,  7468 }, "FLOOR25" },
    { {  5640,  7476,  7476 }, "FLATHUH1" },
    { {  5652,  7488,  7488 }, "FLTWAWA2" },
    { {     0,  7500,  7500 }, "FLOOR28" },
    { {     0,  7508,  7508 }, "FLOOR08" },
    { {  5664,  7516,  7516 }, "FONTA_S" },
    { {  5704,  7524,  7524 }, "PLAYPAL" },
    { {  5672,  7532,  7532 }, "FINAL1" },
    { {  5680,  7540,  7540 }, "FINAL2" },
    { {  5688,  7548,  7548 }, "E2PAL" },
    { {  5696,  7556,  7556 }, "E2END" },
    { {  7884,  7564,  7564 }, "TITLE" },
    { {  5712,  7572,  7572 }, "ORDER" },
    { {     0,  7580,  7580 }, "CREDIT" },
    { {  5720,  7588,  7588 }, "IMPX" },
    { {  5728,  7596,  7596 }, "ACLO" },
    { {  5736,  7604,  7604 }, "PTN1" },
    { {  5744,  7612,  7612 }, "SHLD" },
    { {  5752,  7620,  7620 }, "SHD2" },
    { {  5760,  7628,  7628 }, "BAGH" },
    { {  5768,  7636,  7636 }, "SPMP" },
    { {  5776,  7644,  7644 }, "INVS" },
    { {  5784,  7652,  7652 }, "PTN2" },
    { {  5792,  7660,  7660 }, "SOAR" },
    { {  5800,  7668,  7668 }, "INVU" },
    { {  5808,  7676,  7676 }, "PWBK" },
    { {  5816,  7684,  7684 }, "EGGC" },
    { {  5824,  7692,  7692 }, "EGGM" },
    { {  5832,  7700,  7700 }, "FX01" },
    { {  5840,  7708,  7708 }, "SPHL" },
    { {  5848,  7716,  7716 }, "TRCH" },
    { {  5856,  7724,  7724 }, "FBMB" },
    { {  5864,  7732,  7732 }, "XPL1" },
    { {  5872,  7740,  7740 }, "ATLP" },
    { {  5880,  7748,  7748 }, "PPOD" },
    { {  5888,  7756,  7756 }, "AMG1" },
    { {  5896,  7764,  7764 }, "SPSH" },
    { {  5904,  7772,  7772 }, "LVAS" },
    { {  5912,  7780,  7780 }, "SLDG" },
    { {  5920,  7788,  7788 }, "SKH1" },
    { {  5928,  7796,  7796 }, "SKH2" },
    { {  5936,  7804,  7804 }, "SKH3" },
    { {  5944,  7812,  7812 }, "SKH4" },
    { {  5952,  7820,  7820 }, "CHDL" },
    { {  5960,  7828,  7828 }, "SRTC" },
    { {  5968,  7836,  7836 }, "SMPL" },
    { {  5976,  7844,  7844 }, "STGS" },
    { {  5984,  7852,  7852 }, "STGL" },
    { {  5992,  7860,  7860 }, "STCS" },
    { {  6000,  7868,  7868 }, "STCL" },
    { {  6008,  7876,  7876 }, "KFR1" },
    { {  6016,  7884,  7884 }, "BARL" },
    { {  6024,  7892,  7892 }, "BRPL" },
    { {  6032,  7900,  7900 }, "MOS1" },
    { {  6040,  7908,  7908 }, "MOS2" },
    { {  6048,  7916,  7916 }, "WTRH" },
    { {  6056,  7924,  7924 }, "HCOR" },
    { {  6064,  7932,  7932 }, "KGZ1" },
    { {  6072,  7940,  7940 }, "KGZB" },
    { {  6080,  7948,  7948 }, "KGZG" },
    { {  6088,  7956,  7956 }, "KGZY" },
    { {  6096,  7964,  7964 }, "VLCO" },
    { {  6104,  7972,  7972 }, "VFBL" },
    { {  6112,  7980,  7980 }, "VTFB" },
    { {  6120,  7988,  7988 }, "SFFI" },
    { {  6128,  7996,  7996 }, "TGLT" },
    { {  6136,  8004,  8004 }, "TELE" },
    { {  6144,  8012,  8012 }, "STFF" },
    { {  6152,  8020,  8020 }, "PUF3" },
    { {  6160,  8028,  8028 }, "PUF4" },
    { {  6168,  8036,  8036 }, "BEAK" },
    { {  6176,  8044,  8044 }, "WGNT" },
    { {  6184,  8052,  8052 }, "GAUN" },
    { {  6192,  8060,  8060 }, "PUF1" },
    { {  6200,  8068,  8068 }, "WBLS" },
    { {  6208,  8076,  8076 }, "BLSR" },
    { {  6216,  8084,  8084 }, "FX18" },
    { {  6224,  8092,  8092 }, "FX17" },
    { {  6232,  8100,  8100 }, "WMCE" },
    { {  6240,  8108,  8108 }, "MACE" },
    { {  6248,  8116,  8116 }, "FX02" },
    { {  6256,  8124,  8124 }, "WSKL" },
    { {  6264,  8132,  8132 }, "HROD" },
    { {  6272,  8140,  8140 }, "FX00" },
    { {  6280,  8148,  8148 }, "FX20" },
    { {  6288,  8156,  8156 }, "FX21" },
    { {  6296,  8164,  8164 }, "FX22" },
    { {  6304,  8172,  8172 }, "FX23" },
    { {  6312,  8180,  8180 }, "GWND" },
    { {  6320,  8188,  8188 }, "PUF2" },
    { {  6328,  8196,  8196 }, "WPHX" },
    { {  6336,  8204,  8204 }, "PHNX" },
    { {  6344,  8212,  8212 }, "FX04" },
    { {  6352,  8220,  8220 }, "FX08" },
    { {  6360,  8228,  8228 }, "FX09" },
    { {  6368,  8236,  8236 }, "WBOW" },
    { {  6376,  8244,  8244 }, "CRBW" },
    { {  6384,  8252,  8252 }, "FX03" },
    { {  6392,  8260,  8260 }, "BLOD" },
    { {  6400,  8268,  8268 }, "PLAY" },
    { {  6408,  8276,  8276 }, "FDTH" },
    { {  6416,  8284,  8284 }, "BSKL" },
    { {  6424,  8292,  8292 }, "CHKN" },
    { {  6432,  8300,  8300 }, "MUMM" },
    { {  6440,  8308,  8308 }, "FX15" },
    { {  6448,  8316,  8316 }, "BEAS" },
    { {  6456,  8324,  8324 }, "FRB1" },
    { {  6464,  8332,  8332 }, "SNKE" },
    { {  6472,  8340,  8340 }, "SNFX" },
    { {  6480,  8348,  8348 }, "HEAD" },
    { {  6488,  8356,  8356 }, "FX05" },
    { {  6496,  8364,  8364 }, "FX06" },
    { {  6504,  8372,  8372 }, "FX07" },
    { {  6512,  8380,  8380 }, "CLNK" },
    { {  6520,  8388,  8388 }, "WZRD" },
    { {  6528,  8396,  8396 }, "FX11" },
    { {  6536,  8404,  8404 }, "FX10" },
    { {  6544,  8412,  8412 }, "KNIG" },
    { {  6552,  8420,  8420 }, "SPAX" },
    { {  6560,  8428,  8428 }, "RAXE" },
    { {  6568,  8436,  8436 }, "SRCR" },
    { {  6576,  8444,  8444 }, "FX14" },
    { {  6584,  8452,  8452 }, "SOR2" },
    { {  6592,  8460,  8460 }, "SDTH" },
    { {  6600,  8468,  8468 }, "FX16" },
    { {  6608,  8476,  8476 }, "MNTR" },
    { {  6616,  8484,  8484 }, "FX12" },
    { {  6624,  8492,  8492 }, "FX13" },
    { {  6632,  8500,  8500 }, "AKYY" },
    { {  6640,  8508,  8508 }, "BKYY" },
    { {  6648,  8516,  8516 }, "CKYY" },
    { {  6656,  8524,  8524 }, "AMG2" },
    { {  6664,  8532,  8532 }, "AMM1" },
    { {  6672,  8540,  8540 }, "AMM2" },
    { {  6680,  8548,  8548 }, "AMC1" },
    { {  6688,  8556,  8556 }, "AMC2" },
    { {  6696,  8564,  8564 }, "AMS1" },
    { {  6704,  8572,  8572 }, "AMS2" },
    { {  6712,  8580,  8580 }, "AMP1" },
    { {  6720,  8588,  8588 }, "AMP2" },
    { {  6728,  8596,  8596 }, "AMB1" },
    { {  6736,  8604,  8604 }, "AMB2" },
    { {  6744,  8612,  8612 }, "K" },
    { {  6748,  8616,  8616 }, "I" },
    { {  6752,  8620,  8620 }, "L" },
    { {  6756,  8624,  8624 }, "E" },
    { {  6760,  8628,  8628 }, "R" },
    { {  6764,  8632,  8632 }, "S" },
    { {  6768,  8636,  8636 }, "PLAYPAL" },
    { {  6776,  8644,  8644 }, "MAPE1" },
    { {  6784,  8652,  8652 }, "MAPE2" },
    { {  6792,  8660,  8660 }, "MAPE3" },
    { {  6800,  8668,  8668 }, "IN_X" },
    { {  6808,  8676,  8676 }, "IN_YAH" },
    { {  6816,  8684,  8684 }, "FONTB16" },
    { {  6824,  8692,  8692 }, "FONTB_S" },
    { {  6832,  8700,  8700 }, "FONTB13" },
    { {  6840,  8708,  8708 }, "FONTB15" },
    { {  6848,  8716,  8716 }, "FONTB05" },
    { {  6856,  8724,  8724 }, "FACEA0" },
    { {  6864,  8732,  8732 }, "FACEB0" },
    { {  6940,  8808,  8808 }, "FLOOR16" },
    { {  6948,  8816,  8816 }, "FINISHED" },
    { {  6960,  8828,  8828 }, "NOW ENTERING:" },
    { {  6976,  8844,  8844 }, "KILLS" },
    { {  6984,  8852,  8852 }, "ITEMS" },
    { {  6992,  8860,  8860 }, "SECRETS" },
    { {  7000,  8868,  8868 }, "TIME" },
    { {  7008,  8876,  8876 }, "BONUS" },
    { {  7016,  8884,  8884 }, "SECRET" },
    { {  7024,  8892,  8892 }, "TOTAL" },
    { {  7032,  8900,  8900 }, "VICTIMS" },
    { {  7040,  8908,  8908 }, ":" },
    { {  7044,  8912,  8912 }, "NEW GAME" },
    { {  7056,  8924,  8924 }, "OPTIONS" },
    { {  7064,  8932,  8932 }, "GAME FILES" },
    { {  7076,  8944,  8944 }, "INFO" },
    { {  7084,  8952,  8952 }, "QUIT GAME" },
    { {  7096,  8964,  8964 }, "CITY OF THE DAMNED" },
    { {  7116,  8984,  8984 }, "HELL'S MAW" },
    { {  7128,  8996,  8996 }, "THE DOME OF D'SPARIL" },
    { {     0,  9020,  9020 }, "THE OSSUARY" },
    { {     0,  9032,  9032 }, "THE STAGNANT DEMESNE" },
    { {  7152,  9056,  9056 }, "LOAD GAME" },
    { {  7164,  9068,  9068 }, "SAVE GAME" },
    { {  7176,  9080,  9080 }, "THOU NEEDETH A WET-NURSE" },
    { {  7204,  9108,  9108 }, "YELLOWBELLIES-R-US" },
    { {  7224,  9128,  9128 }, "BRINGEST THEM ONETH" },
    { {  7244,  9148,  9148 }, "THOU ART A SMITE-MEISTER" },
    { {  7272,  9176,  9176 }, "BLACK PLAGUE POSSESSES THEE" },
    { {  7300,  9204,  9204 }, "END GAME" },
    { {  7312,  9216,  9216 }, "MESSAGES : " },
    { {  7324,  9228,  9228 }, "MOUSE SENSITIVITY" },
    { {  7344,  9248,  9248 }, "MORE..." },
    { {  7352,  9256,  9256 }, "SCREEN SIZE" },
    { {  7364,  9268,  9268 }, "SFX VOLUME" },
    { {  7376,  9280,  9280 }, "MUSIC VOLUME" },
    { {  7416,  9296,  9296 }, "ARE YOU SURE YOU WANT TO QUIT?" },
    { {  7448,  9328,  9328 }, "ARE YOU SURE YOU WANT TO END THE GAME?" },
    { {  7488,  9368,  9368 }, "DO YOU WANT TO QUICKSAVE THE GAME NAMED" },
    { {  7528,  9408,  9408 }, "DO YOU WANT TO QUICKLOAD THE GAME NAMED" },
    { {  7392,  9448,  9448 }, "M_SKL00" },
    { {  7400,  9456,  9456 }, "FONTA_S" },
    { {  7408,  9464,  9464 }, "FONTB_S" },
    { {  7568,  9472,  9472 }, "?" },
    { {  7572,  9476,  9476 }, "M_SLCTR1" },
    { {  7584,  9488,  9488 }, "M_SLCTR2" },
    { {  7596,  9500,  9500 }, "M_HTIC" },
    { {  7604,  9508,  9508 }, "c:\\heretic.cd\\hticsav%d.hsg" },
    { {  7632,  9536,  9536 }, "hticsav%d.hsg" },
    { {  7652,  9556,  9556 }, "M_FSLOT" },
    { {  7660,  9564,  9564 }, "ON" },
    { {  7664,  9568,  9568 }, "OFF" },
    { {     0,  9572,  9572 }, "YOU CAN'T START A NEW GAME IN NETPLAY!" },
    { {     0,  9612,  9612 }, "YOU CAN'T LOAD A GAME IN NETPLAY!" },
    { {  7668,  9648,  9648 }, "MESSAGES ON" },
    { {  7680,  9660,  9660 }, "MESSAGES OFF" },
    { {  7748,  9676,  9676 }, "ONLY AVAILABLE IN THE REGISTERED VERSION" },
    { {  7792,  9720,  9720 }, "PLAYPAL" },
    { {  7800,  9728,  9728 }, "QUICKSAVING...." },
    { {  7816,  9744,  9744 }, "QUICKLOADING...." },
    { {  7836,  9764,  9764 }, "CHOOSE A QUICKSAVE SLOT" },
    { {  7860,  9788,  9788 }, "CHOOSE A QUICKLOAD SLOT" },
    { {     0,  9812,  9812 }, "TITLE" },
    { {  7892,  9820,  9820 }, "M_SLDLT" },
    { {  7900,  9828,  9828 }, "M_SLDMD1" },
    { {  7912,  9840,  9840 }, "M_SLDMD2" },
    { {  7924,  9852,  9852 }, "M_SLDRT" },
    { {  7932,  9860,  9860 }, "M_SLDKB" },
    { {  9016, 10944, 10944 }, "SCREEN SHOT" },
    { {  9028, 10956, 10956 }, "YOU NEED A BLUE KEY TO OPEN THIS DOOR" },
    { {  9068, 10996, 10996 }, "YOU NEED A YELLOW KEY TO OPEN THIS DOOR" },
    { {  9108, 11036, 11036 }, "YOU NEED A GREEN KEY TO OPEN THIS DOOR" },
    { {  9244, 11172, 11172 }, "CRYSTAL VIAL" },
    { {  9260, 11188, 11188 }, "SILVER SHIELD" },
    { {  9276, 11204, 11204 }, "ENCHANTED SHIELD" },
    { {  9296, 11224, 11224 }, "BAG OF HOLDING" },
    { {  9312, 11240, 11240 }, "MAP SCROLL" },
    { {  9324, 11252, 11252 }, "BLUE KEY" },
    { {  9336, 11264, 11264 }, "YELLOW KEY" },
    { {  9348, 11276, 11276 }, "GREEN KEY" },
    { {  9360, 11288, 11288 }, "QUARTZ FLASK" },
    { {  9376, 11304, 11304 }, "WINGS OF WRATH" },
    { {  9392, 11320, 11320 }, "RING OF INVINCIBILITY" },
    { {  9416, 11344, 11344 }, "TOME OF POWER" },
    { {  9432, 11360, 11360 }, "SHADOWSPHERE" },
    { {  9448, 11376, 11376 }, "MORPH OVUM" },
    { {  9460, 11388, 11388 }, "MYSTIC URN" },
    { {  9472, 11400, 11400 }, "TORCH" },
    { {  9480, 11408, 11408 }, "TIME BOMB OF THE ANCIENTS" },
    { {  9508, 11436, 11436 }, "CHAOS DEVICE" },
    { {  9524, 11452, 11452 }, "WAND CRYSTAL" },
    { {  9540, 11468, 11468 }, "CRYSTAL GEODE" },
    { {  9556, 11484, 11484 }, "MACE SPHERES" },
    { {  9572, 11500, 11500 }, "PILE OF MACE SPHERES" },
    { {  9596, 11524, 11524 }, "ETHEREAL ARROWS" },
    { {  9612, 11540, 11540 }, "QUIVER OF ETHEREAL ARROWS" },
    { {  9640, 11568, 11568 }, "CLAW ORB" },
    { {  9652, 11580, 11580 }, "ENERGY ORB" },
    { {  9664, 11592, 11592 }, "LESSER RUNES" },
    { {  9680, 11608, 11608 }, "GREATER RUNES" },
    { {  9696, 11624, 11624 }, "FLAME ORB" },
    { {  9708, 11636, 11636 }, "INFERNO ORB" },
    { {  9720, 11648, 11648 }, "FIREMACE" },
    { {  9732, 11660, 11660 }, "ETHEREAL CROSSBOW" },
    { {  9752, 11680, 11680 }, "DRAGON CLAW" },
    { {  9764, 11692, 11692 }, "HELLSTAFF" },
    { {  9776, 11704, 11704 }, "PHOENIX ROD" },
    { {  9788, 11716, 11716 }, "GAUNTLETS OF THE NECROMANCER" },
    { { 10088, 12016, 12016 }, "FLTWAWA1" },
    { { 10100, 12028, 12028 }, "FLTFLWW1" },
    { { 10112, 12040, 12040 }, "FLTLAVA1" },
    { { 10124, 12052, 12052 }, "FLATHUH1" },
    { { 10136, 12064, 12064 }, "FLTSLUD1" },
    { { 10148, 12076, 12076 }, "END" },
    { { 10236, 12164, 12164 }, "texture2" },
    { { 10444, 12372, 12372 }, "PLAYPAL" },
    { { 10596, 12488, 12488 }, "PNAMES" },
    { { 10604, 12496, 12496 }, "TEXTURE1" },
    { { 10616, 12508, 12508 }, "TEXTURE2" },
    { { 10628, 12520, 12520 }, "S_END" },
    { { 10636, 12528, 12528 }, "S_START" },
    { { 10728, 12620, 12620 }, "F_START" },
    { { 10736, 12628, 12628 }, "F_END" },
    { { 10744, 12636, 12636 }, "COLORMAP" },
    { { 10756, 12648, 12648 }, "\nR_InitTextures " },
    { { 10776, 12668, 12668 }, "R_InitFlats\n" },
    { { 10792, 12684, 12684 }, "R_InitSpriteLumps " },
    { { 10948, 12772, 12772 }, "TINTTAB" },
    { { 10984, 12780, 12780 }, "FLOOR04" },
    { { 10992, 12788, 12788 }, "FLAT513" },
    { { 11000, 12796, 12796 }, "bordt" },
    { { 11008, 12804, 12804 }, "bordb" },
    { { 11016, 12812, 12812 }, "bordl" },
    { { 11024, 12820, 12820 }, "bordr" },
    { { 11032, 12828, 12828 }, "bordtl" },
    { { 11040, 12836, 12836 }, "bordtr" },
    { { 11048, 12844, 12844 }, "bordbr" },
    { { 11056, 12852, 12852 }, "bordbl" },
    { { 11064, 12860, 12860 }, "R_InitData " },
    { { 11076, 12872, 12872 }, "R_InitPointToAngle\n" },
    { { 11096, 12892, 12892 }, "R_InitTables " },
    { { 11128, 12924, 12924 }, "R_InitLightTables " },
    { { 11148, 12944, 12944 }, "R_InitSkyMap\n" },
    { { 11164, 12960, 12960 }, "F_SKY1" },
    { { 12120, 13484, 13484 }, "LTFACE" },
    { { 12128, 13492, 13492 }, "RTFACE" },
    { { 12136, 13500, 13500 }, "BARBACK" },
    { { 12144, 13508, 13508 }, "INVBAR" },
    { { 12152, 13516, 13516 }, "CHAIN" },
    { { 12160, 13524, 13524 }, "STATBAR" },
    { { 12168, 13532, 13532 }, "LIFEBAR" },
    { { 12176, 13540, 13540 }, "LIFEGEM2" },
    { { 12188, 13552, 13552 }, "LIFEGEM0" },
    { { 12200, 13564, 13564 }, "LTFCTOP" },
    { { 12208, 13572, 13572 }, "RTFCTOP" },
    { { 12224, 13580, 13580 }, "SELECTBOX" },
    { { 12236, 13592, 13592 }, "INVGEML1" },
    { { 12248, 13604, 13604 }, "INVGEML2" },
    { { 12260, 13616, 13616 }, "INVGEMR1" },
    { { 12272, 13628, 13628 }, "INVGEMR2" },
    { { 12284, 13640, 13640 }, "BLACKSQ" },
    { { 12292, 13648, 13648 }, "ARMCLEAR" },
    { { 12304, 13660, 13660 }, "CHAINBACK" },
    { { 12316, 13672, 13672 }, "IN0" },
    { { 12320, 13676, 13676 }, "NEGNUM" },
    { { 12328, 13684, 13684 }, "FONTB16" },
    { { 12336, 13692, 13692 }, "SMALLIN0" },
    { { 12348, 13704, 13704 }, "PLAYPAL" },
    { { 12356, 13712, 13712 }, "SPINBK0" },
    { { 12364, 13720, 13720 }, "SPFLY0" },
    { { 12372, 13728, 13728 }, "LAME" },
    { { 12380, 13736, 13736 }, "*** SOUND DEBUG INFO ***" },
    { { 12408, 13764, 13764 }, "NAME" },
    { { 12416, 13772, 13772 }, "MO.T" },
    { { 12424, 13780, 13780 }, "MO.X" },
    { { 12432, 13788, 13788 }, "MO.Y" },
    { { 12440, 13796, 13796 }, "ID" },
    { { 12444, 13800, 13800 }, "PRI" },
    { { 12448, 13804, 13804 }, "DIST" },
    { { 12456, 13812, 13812 }, "------" },
    { { 12464, 13820, 13820 }, "%s" },
    { { 12468, 13824, 13824 }, "%d" },
    { { 12472, 13828, 13828 }, "GOD1" },
    { { 12480, 13836, 13836 }, "GOD2" },
    { { 12488, 13844, 13844 }, "useartia" },
    { { 12500, 13856, 13856 }, "ykeyicon" },
    { { 12512, 13868, 13868 }, "gkeyicon" },
    { { 12524, 13880, 13880 }, "bkeyicon" },
    { { 12216, 13892, 13892 }, "ARTIBOX" },
    { { 12536, 13900, 13900 }, "GOD MODE ON" },
    { { 12548, 13912, 13912 }, "GOD MODE OFF" },
    { { 12564, 13928, 13928 }, "NO CLIPPING ON" },
    { { 12580, 13944, 13944 }, "NO CLIPPING OFF" },
    { { 12596, 13960, 13960 }, "ALL WEAPONS" },
    { { 12608, 13972, 13972 }, "POWER OFF" },
    { { 12620, 13984, 13984 }, "POWER ON" },
    { { 12632, 13996, 13996 }, "FULL HEALTH" },
    { { 12644, 14008, 14008 }, "ALL KEYS" },
    { { 12656, 14020, 14020 }, "SOUND DEBUG ON" },
    { { 12672, 14036, 14036 }, "SOUND DEBUG OFF" },
    { { 12688, 14052, 14052 }, "TICKER ON" },
    { { 12700, 14064, 14064 }, "TICKER OFF" },
    { { 12712, 14076, 14076 }, "CHOOSE AN ARTIFACT ( A - J )" },
    { { 12744, 14108, 14108 }, "HOW MANY ( 1 - 9 )" },
    { { 12764, 14128, 14128 }, "YOU GOT IT" },
    { { 12776, 14140, 14140 }, "BAD INPUT" },
    { { 12788, 14152, 14152 }, "LEVEL WARP" },
    { { 12800, 14164, 14164 }, "CHICKEN OFF" },
    { { 12812, 14176, 14176 }, "CHICKEN ON" },
    { { 12824, 14188, 14188 }, "MASSACRE" },
    { { 12836, 14200, 14200 }, "CHEATER - YOU DON'T DESERVE WEAPONS" },
    { { 12872, 14236, 14236 }, "TRYING TO CHEAT, EH?  NOW YOU DIE!" },
};

// String offsets that are valid but we don't support.

static const int unsupported_strings_1_0[] =
{
        0,     4,    64,   104,   160,   200,   220,   236,
      244,   252,   272,   288,   296,   316,   332,   372,
      436,   500,   504,   536,   544,   560,   576,   584,
      592,   612,   640,   664,   708,   712,   744,   764,
      808,   820,   828,   840,   876,   884,   908,   952,
      992,  1028,  1036,  1048,  1088,  1128,  1160,  1192,
     1212,  1912,  2044,  2056,  2068,  2128,  2140,  2168,
     2184,  2196,  2212,  2228,  2240,  2252,  2260,  2264,
     2284,  2292,  2296,  2300,  2328,  2340,  2352,  2364,
     2372,  2384,  2388,  2404,  2428,  2436,  2444,  2464,
     2496,  2508,  2520,  2552,  2564,  2572,  2584,  3120,
     3128,  3140,  3184,  3220,  3248,  3252,  3256,  3280,
     3304,  3320,  3352,  3380,  3400,  3432,  3464,  3548,
     3600,  3624,  3664,  3696,  3812,  3872,  3932,  3940,
     3976,  3996,  6872,  6896,  7648,  7696,  7940,  7964,
     7968,  7992,  8020,  8028,  8052,  8056,  8076,  8088,
     8104,  8116,  8128,  8136,  8148,  8164,  8180,  8192,
     8204,  8220,  8232,  8248,  8264,  8276,  8292,  8308,
     8320,  8328,  8340,  8352,  8364,  8376,  8392,  8408,
     8424,  8436,  8448,  8460,  8472,  8488,  8504,  8520,
     8536,  8548,  8560,  8572,  8584,  8596,  8608,  8612,
     8624,  8648,  8660,  8668,  8680,  8708,  8720,  8728,
     8740,  8752,  8764,  8788,  8800,  8812,  8824,  8848,
     8860,  8864,  8868,  8876,  8888,  8896,  8916,  8944,
     8948,  8960,  8964,  8968,  8980,  9148,  9172,  9212,
     9216,  9220,  9820,  9860,  9892,  9940,  9972, 10012,
    10036, 10040, 10052, 10080, 10152, 10192, 10248, 10284,
    10320, 10360, 10392, 10452, 10488, 10508, 10556, 10644,
    10684, 10812, 10844, 10880, 10912, 10956, 11172, 11200,
    11232, 11272, 11312, 11348, 11380, 11404, 11436, 11492,
    11548, 11616, 11684, 11748, 11792, 11840, 11896, 11936,
    11980, 12028, 12072, 12908, 12924, 12956, 12960, 12968,
    12976, 13020, 13048, 13076, 13104, 13136, 13168, 13196,
    13240, 13272, 13292, 13296, 13308, 13312, 13320, 13324,
    13364, 13408, 13460, 13492, 13516, 13560, 13612, 13664,
    13700, 13744, 13796, 13848, 13884, 13940, 13996, 14040,
    14084, 14140, 14148, 14156, 14164, 14184, 14192, 14204,
    14208, 14212, 14256, 14272, 14284, 14296, 14300, 14312,
    14320, 14324, 14348, 14356, 14360, 14372, 14380, 14392,
    14432, 14440, 14444, 14472, 14496, 14516, 14536, 14548,
    14560, 14572, 14580, 14588, 14596, 14604, 14612, 14620,
    14636, 14660, 14704, 14740, 14748, 14756, 14760, 14768,
       -1,
};

static const int unsupported_strings_1_2[] =
{
        0,     4,    64,   104,   160,   200,   220,   236,
      244,   252,   272,   288,   296,   316,   332,   372,
      436,   500,   504,   536,   544,   560,   576,   584,
      592,   612,   640,   664,   708,   712,   744,   756,
      776,   820,   832,   840,   852,   888,   896,   920,
      964,  1004,  1040,  1048,  1060,  1100,  1140,  1172,
     1204,  1224,  2312,  2436,  2448,  2464,  2480,  2492,
     2512,  2524,  2536,  2596,  2608,  2636,  2652,  2656,
     2676,  2684,  2688,  2720,  2732,  2744,  2752,  2764,
     2772,  2776,  2792,  2816,  2824,  2832,  2852,  2884,
     2896,  2908,  2940,  2952,  2960,  2972,  3520,  3528,
     3540,  3584,  3620,  3648,  3652,  3656,  3680,  3704,
     3720,  3776,  3804,  3824,  3856,  3888,  4020,  4044,
     4084,  4116,  4156,  4272,  4288,  4296,  4332,  4352,
     4428,  4432,  8740,  8764,  9552,  9868,  9888,  9900,
     9916,  9928,  9940,  9948,  9960,  9976,  9992, 10004,
    10016, 10032, 10044, 10060, 10076, 10088, 10104, 10120,
    10132, 10140, 10152, 10164, 10176, 10188, 10204, 10220,
    10236, 10248, 10260, 10272, 10284, 10300, 10316, 10332,
    10348, 10360, 10372, 10384, 10396, 10408, 10420, 10424,
    10436, 10460, 10472, 10480, 10492, 10520, 10532, 10540,
    10552, 10564, 10576, 10600, 10612, 10624, 10636, 10660,
    10672, 10676, 10700, 10704, 10728, 10756, 10764, 10788,
    10792, 10796, 10804, 10816, 10824, 10844, 10872, 10876,
    10888, 10892, 10896, 10908, 11076, 11100, 11140, 11144,
    11148, 11748, 11788, 11820, 11868, 11900, 11940, 11964,
    11968, 11980, 12008, 12080, 12120, 12176, 12212, 12248,
    12288, 12320, 12380, 12400, 12448, 12536, 12576, 12704,
    12736, 12968, 13000, 13024, 13080, 13136, 13204, 13272,
    13336, 13380, 13428, 14272, 14288, 14320, 14324, 14332,
    14340, 14384, 14412, 14440, 14468, 14500, 14532, 14560,
    14604, 14636, 14656, 14696, 14740, 14792, 14824, 14848,
    14892, 14944, 14996, 15032, 15076, 15128, 15180, 15216,
    15272, 15328, 15372, 15416, 15472, 15480, 15488, 15496,
    15516, 15524, 15536, 15540, 15544, 15588, 15604, 15616,
    15628, 15632, 15644, 15652, 15656, 15680, 15688, 15692,
    15704, 15712, 15724, 15764, 15772, 15776, 15804, 15828,
    15848, 15868, 15880, 15892, 15904, 15912, 15920, 15928,
    15936,    -1,
};

static const int unsupported_strings_1_3[] =
{
        0,     4,    64,   104,   160,   200,   220,   236,
      244,   252,   272,   288,   296,   316,   332,   372,
      436,   500,   504,   536,   544,   560,   576,   584,
      592,   612,   640,   664,   708,   712,   744,   756,
      776,   820,   832,   840,   852,   888,   896,   920,
      964,  1004,  1040,  1048,  1060,  1100,  1140,  1172,
     1204,  1224,  2312,  2436,  2448,  2464,  2480,  2492,
     2512,  2524,  2536,  2596,  2608,  2636,  2652,  2656,
     2676,  2684,  2688,  2720,  2732,  2744,  2752,  2764,
     2772,  2776,  2792,  2816,  2824,  2832,  2852,  2884,
     2896,  2908,  2940,  2952,  2960,  2972,  3520,  3528,
     3540,  3584,  3620,  3648,  3652,  3656,  3680,  3704,
     3720,  3776,  3804,  3824,  3856,  3888,  4020,  4044,
     4084,  4116,  4156,  4272,  4288,  4296,  4332,  4352,
     4428,  4432,  8740,  8764,  9552,  9868,  9888,  9900,
     9916,  9928,  9940,  9948,  9960,  9976,  9992, 10004,
    10016, 10032, 10044, 10060, 10076, 10088, 10104, 10120,
    10132, 10140, 10152, 10164, 10176, 10188, 10204, 10220,
    10236, 10248, 10260, 10272, 10284, 10300, 10316, 10332,
    10348, 10360, 10372, 10384, 10396, 10408, 10420, 10424,
    10436, 10460, 10472, 10480, 10492, 10520, 10532, 10540,
    10552, 10564, 10576, 10600, 10612, 10624, 10636, 10660,
    10672, 10676, 10700, 10704, 10728, 10756, 10764, 10788,
    10792, 10796, 10804, 10816, 10824, 10844, 10872, 10876,
    10888, 10892, 10896, 10908, 11076, 11100, 11140, 11144,
    11148, 11748, 11788, 11820, 11868, 11900, 11940, 11964,
    11968, 11980, 12008, 12080, 12120, 12176, 12212, 12248,
    12288, 12320, 12380, 12400, 12448, 12536, 12576, 12704,
    12736, 12968, 13000, 13024, 13080, 13136, 13204, 13272,
    13336, 13380, 13428, 14272, 14288, 14320, 14324, 14332,
    14340, 14384, 14412, 14440, 14468, 14500, 14532, 14560,
    14604, 14636, 14656, 14696, 14740, 14792, 14824, 14848,
    14892, 14944, 14996, 15032, 15076, 15128, 15180, 15216,
    15272, 15328, 15372, 15416, 15472, 15480, 15488, 15496,
    15516, 15524, 15536, 15540, 15544, 15588, 15604, 15616,
    15628, 15632, 15644, 15652, 15656, 15680, 15688, 15692,
    15704, 15712, 15724, 15764, 15772, 15776, 15804, 15828,
    15848, 15868, 15880, 15892, 15904, 15912, 15920, 15928,
    15936,    -1,
};

static const int *unsupported_strings[] =
{
    unsupported_strings_1_0,
    unsupported_strings_1_2,
    unsupported_strings_1_3,
};

static boolean StringIsUnsupported(unsigned int offset)
{
    const int *string_list;
    int i;

    string_list = unsupported_strings[deh_hhe_version];

    for (i=0; string_list[i] >= 0; ++i)
    {
        if ((unsigned int) string_list[i] == offset)
        {
            return true;
        }
    }

    return false;
}

static boolean GetStringByOffset(unsigned int offset, char **result)
{
    int i;

    for (i=0; i<arrlen(strings); ++i)
    {
        if (strings[i].offsets[deh_hhe_version] == offset)
        {
            *result = strings[i].string;
            return true;
        }
    }

    return false;
}

// Given a string length, find the maximum length of a 
// string that can replace it.

static int MaxStringLength(int len)
{
    // Enough bytes for the string and the NUL terminator

    len += 1;

    // All strings in doom.exe are on 4-byte boundaries, so we may be able
    // to support a slightly longer string.
    // Extend up to the next 4-byte boundary

    len += (4 - (len % 4)) % 4;

    // Less one for the NUL terminator.

    return len - 1;
}

// If a string offset does not match any string, it may be because
// we are running in the wrong version mode, and the patch was generated
// for a different Heretic version.  Search the lookup tables to find
// versiosn that match.

static void SuggestOtherVersions(unsigned int offset)
{
    const int *string_list;
    unsigned int i;
    unsigned int v;

    // Check main string table.

    for (i=0; i<arrlen(strings); ++i)
    {
        for (v=0; v<deh_hhe_num_versions; ++v)
        {
            if (strings[i].offsets[v] == offset)
            {
                DEH_SuggestHereticVersion(v);
            }
        }
    }

    // Check unsupported string tables.

    for (v=0; v<deh_hhe_num_versions; ++v)
    {
        string_list = unsupported_strings[v];

        for (i=0; string_list[i] >= 0; ++i)
        {
            if (string_list[i] == offset)
            {
                DEH_SuggestHereticVersion(v);
            }
        }
    }
}

static void *DEH_TextStart(deh_context_t *context, char *line)
{
    char *repl_text;
    char *orig_text;
    int orig_offset, repl_len;
    int i;

    if (sscanf(line, "Text %i %i", &orig_offset, &repl_len) != 2)
    {
        DEH_Warning(context, "Parse error on section start");
        return NULL;
    }

    repl_text = malloc(repl_len + 1);

    // read in the "to" text

    for (i=0; i<repl_len; ++i)
    {
        int c;

        c = DEH_GetChar(context);

        repl_text[i] = c;
    }
    repl_text[repl_len] = '\0';

    // We don't support all strings, but at least recognise them:

    if (StringIsUnsupported(orig_offset))
    {
        DEH_Warning(context, "Unsupported string replacement: %i", orig_offset);
    }

    // Find the string to replace:

    else if (!GetStringByOffset(orig_offset, &orig_text))
    {
        SuggestOtherVersions(orig_offset);
        DEH_Error(context, "Unknown string offset: %i", orig_offset);
    }

    // Only allow string replacements that are possible in Vanilla Doom.  
    // Chocolate Doom is unforgiving!

    else if (!deh_allow_long_strings
          && repl_len > MaxStringLength(strlen(orig_text)))
    {
        DEH_Error(context, "Replacement string is longer than the maximum "
                           "possible in heretic.exe");
    }
    else
    {
        // Success.

        DEH_AddStringReplacement(orig_text, repl_text);
    }

    // We must always free the replacement text.
    free(repl_text);

    return NULL;
}

static void DEH_TextParseLine(deh_context_t *context, char *line, void *tag)
{
    // not used
}

deh_section_t deh_section_heretic_text =
{
    "Text",
    NULL,
    DEH_TextStart,
    DEH_TextParseLine,
    NULL,
    NULL,
};

