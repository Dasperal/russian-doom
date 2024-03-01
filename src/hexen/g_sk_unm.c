//
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

#include "l_sk_unm.h"
#include "g_sk_unm.h"

#include "doomtype.h"
#include "h2def.h"
#include "m_fixed.h"

#define ENUMERATE_UNM_CHANGES(O)                                                             \
/* Monster's speed */                                                                        \
O(MT_FIREDEMON, speed, + 7)                  /* Affrit (13 to 20) */                         \
O(MT_ETTIN, speed, + 5)                      /* Ettin (13 to 18) */                          \
O(MT_CENTAUR, speed, + 8)                    /* Centaur (13 to 21) */                        \
O(MT_CENTAURLEADER, speed, + 7)              /* Slaughtaur (10 to 17) */                     \
O(MT_DEMON, speed, + 5)                      /* Green Chaos Serpent (13 to 18) */            \
O(MT_DEMON2, speed, + 5)                     /* Brown Chaos Serpent (13 to 18) */            \
O(MT_DEMON_MASH, speed, + 5)                 /* Phantasmal Green Chaos Serpent (13 to 18) */ \
O(MT_DEMON2_MASH, speed, + 5)                /* Phantasmal Brown Chaos Serpent (13 to 18) */ \
O(MT_BISHOP, speed, + 12)                    /* Dark Bishop (10 to 22) */                    \
O(MT_ICEGUY, speed, + 6)                     /* Wendige (14 to 20) */                        \
O(MT_WRAITH, speed, + 13)                    /* Reiver (11 to 24) */                         \
O(MT_WRAITHB, speed, + 13)                   /* Reiver (buried) (11 to 24) */                \
/* Monster's missiles speed */                                                               \
O(MT_FIREDEMON_FX6, speed, + (5 * FRACUNIT)) /* Affrit fireball (10 to 15) */                \
O(MT_DEMON2FX1, speed, + (3 * FRACUNIT))     /* Brown Chaos Serpent fireball (15 to 18) */   \
O(MT_BISH_FX, speed, + (3 * FRACUNIT))       /* Dark Bishop fireball (10 to 13) */           \
O(MT_DEMONFX1, speed, + (3 * FRACUNIT))      /* Green Chaos Serpent fireball (15 to 18) */   \
O(MT_WRAITHFX1, speed, + (3 * FRACUNIT))     /* Reiver fireball (14 to 17) */                \
O(MT_CENTAUR_FX, speed, + (3 * FRACUNIT))    /* Slaughtaur fireball (20 to 23) */            \
O(MT_SERPENTFX, speed, + (3 * FRACUNIT))     /* Stalker fireball (15 to 18) */               \
O(MT_ICEGUY_FX, speed, + (3 * FRACUNIT))     /* Wendige fireball 1 (14 to 17) */             \
O(MT_ICEGUY_FX2, speed, + (3 * FRACUNIT))    /* Wendige fireball 2 (10 to 13) */             \
/* Monster's damage */                                                                       \
O(MT_FIREDEMON_FX6, damage, + 1)             /* Fire Gargolye fireball (1 to 2) */           \
O(MT_DEMON2FX1, damage, + 1)                 /* Brown Chaos Serpent fireball (5 to 6) */     \
O(MT_BISH_FX, damage, + 1)                   /* Dark Bishop fireball (1 to 2) */             \
O(MT_DEMONFX1, damage, + 1)                  /* Green Chaos Serpent fireball (5 to 6) */     \
O(MT_WRAITHFX1, damage, + 1)                 /* Reiver fireball (5 to 6) */                  \
O(MT_CENTAUR_FX, damage, + 1)                /* Slaughtaur fireball (4 to 5) */              \
O(MT_SERPENTFX, damage, + 1)                 /* Stalker fireball (4 to 5) */                 \
/* Monster's health */                                                                       \
O(MT_KORAX, spawnhealth, + 13000)            /* Korax health (5000 to 18000) */              \
/* Monster's painchance */                                                                   \
O(MT_KORAX, painchance, - 16)                /* Korax painchance (20 to 4) */

#define ENUMERATE_UNM_MANA_USAGE_CHANGES(O) \
O(Wraithverge_mana_usage, WeaponManaUse[PCLASS_CLERIC][WP_FOURTH], + 15) /* Wraithverge (18 to 33) */

extern int WeaponManaUse[NUMCLASSES][NUMWEAPONS];

UNM_IMPLEMENT(ENUMERATE_UNM_CHANGES, ENUMERATE_UNM_MANA_USAGE_CHANGES)

boolean UNM_is_slow_monster(mobjtype_t type)
{
    return type == MT_CENTAURLEADER
        || type == MT_BISHOP
        || type == MT_WRAITH
        || type == MT_WRAITHB
        || type == MT_DEMON
        || type == MT_DEMON2
        || type == MT_DEMON_MASH
        || type == MT_DEMON2_MASH;
}
