//
// Copyright(C) 1993-1996 Id Software, Inc.
// Copyright(C) 1993-2008 Raven Software
// Copyright(C) 2005-2014 Simon Howard
// Copyright(C) 2016-2021 Julian Nechaevsky
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



#include <stdio.h>
#include <stdlib.h>
#include "h2def.h"
#include "i_system.h"
#include "i_swap.h"
#include "r_local.h"

//void R_DrawTranslatedAltTLColumn(void);

typedef struct
{
    int x1, x2;

    int column;
    int topclip;
    int bottomclip;
} maskdraw_t;

/*

Sprite rotation 0 is facing the viewer, rotation 1 is one angle turn CLOCKWISE around the axis.
This is not the same as the angle, which increases counter clockwise
(protractor).  There was a lot of stuff grabbed wrong, so I changed it...

*/


fixed_t pspritescale, pspriteiscale;

lighttable_t **spritelights;

// [JN] Brightmaps
lighttable_t **fullbrights_greenonly;
lighttable_t **fullbrights_redonly;
lighttable_t **fullbrights_blueonly;
lighttable_t **fullbrights_purpleonly;
lighttable_t **fullbrights_flame;
lighttable_t **fullbrights_yellowred;
lighttable_t **fullbrights_firebull;


// constant arrays used for psprite clipping and initializing clipping
int negonearray[WIDESCREENWIDTH];       // [crispy] 32-bit integer math
int screenheightarray[WIDESCREENWIDTH]; // [crispy] 32-bit integer math

boolean LevelUseFullBright;
/*
===============================================================================

						INITIALIZATION FUNCTIONS

===============================================================================
*/

// variables used to look up and range check thing_t sprites patches
spritedef_t *sprites;
int numsprites;

spriteframe_t sprtemp[30];
int maxframe;
char *spritename;



/*
=================
=
= R_InstallSpriteLump
=
= Local function for R_InitSprites
=================
*/

void R_InstallSpriteLump(int lump, unsigned frame, unsigned rotation,
                         boolean flipped)
{
    int r;

    if (frame >= 30 || rotation > 8)
        I_Error(english_language ?
                "R_InstallSpriteLump: Bad frame characters in lump %i" :
                "R_InstallSpriteLump: некорректные символы фрейма в блоке %i",
                lump);

    if ((int) frame > maxframe)
        maxframe = frame;

    if (rotation == 0)
    {
// the lump should be used for all rotations
        if (sprtemp[frame].rotate == false)
            I_Error(english_language ?
                    "R_InitSprites: Sprite %s frame %c has multip rot=0 lump" :
                    "R_InitSprites: фрейм %c спрайта %s имеет многократный блок rot=0",
                    spritename, 'A' + frame);
        if (sprtemp[frame].rotate == true)
            I_Error(english_language ?
                    "R_InitSprites: Sprite %s frame %c has rotations and a rot=0 lump" :
                    "R_InitSprites: фрейм %c спрайта %s имеет фреймы поворота и блок rot=0",
                    spritename, 'A' + frame);

        sprtemp[frame].rotate = false;
        for (r = 0; r < 8; r++)
        {
            sprtemp[frame].lump[r] = lump - firstspritelump;
            sprtemp[frame].flip[r] = (byte) flipped;
        }
        return;
    }

// the lump is only used for one rotation
    if (sprtemp[frame].rotate == false)
        I_Error(english_language ?
                "R_InitSprites: Sprite %s frame %c has rotations and a rot=0 lump" :
                "R_InitSprites: фрейм спрайта %c спрайта %s имеет фреймы поворота и блок rot=0",
                spritename, 'A' + frame);

    sprtemp[frame].rotate = true;

    rotation--;                 // make 0 based
    if (sprtemp[frame].lump[rotation] != -1)
        I_Error(english_language ?
                "R_InitSprites: Sprite %s : %c : %c has two lumps mapped to it" :
                "R_InitSprites: спрайу %s : %c : %c назначено несколько одинаковых блоков",
                spritename, 'A' + frame, '1' + rotation);

    sprtemp[frame].lump[rotation] = lump - firstspritelump;
    sprtemp[frame].flip[rotation] = (byte) flipped;
}

/*
=================
=
= R_InitSpriteDefs
=
= Pass a null terminated list of sprite names (4 chars exactly) to be used
= Builds the sprite rotation matrixes to account for horizontally flipped
= sprites.  Will report an error if the lumps are inconsistant
=Only called at startup
=
= Sprite lump names are 4 characters for the actor, a letter for the frame,
= and a number for the rotation, A sprite that is flippable will have an
= additional letter/number appended.  The rotation character can be 0 to
= signify no rotations
=================
*/

void R_InitSpriteDefs(char **namelist)
{
    char **check;
    int i, l, frame, rotation;
    int start, end;

// count the number of sprite names
    check = namelist;
    while (*check != NULL)
        check++;
    numsprites = check - namelist;

    if (!numsprites)
        return;

    sprites = Z_Malloc(numsprites * sizeof(*sprites), PU_STATIC, NULL);

    start = firstspritelump - 1;
    end = lastspritelump + 1;

// scan all the lump names for each of the names, noting the highest
// frame letter
// Just compare 4 characters as ints
    for (i = 0; i < numsprites; i++)
    {
        spritename = namelist[i];
        memset(sprtemp, -1, sizeof(sprtemp));

        maxframe = -1;

        //
        // scan the lumps, filling in the frames for whatever is found
        //
        for (l = start + 1; l < end; l++)
            if (!strncmp(lumpinfo[l]->name, namelist[i], 4))
            {
                frame = lumpinfo[l]->name[4] - 'A';
                rotation = lumpinfo[l]->name[5] - '0';
                R_InstallSpriteLump(l, frame, rotation, false);
                if (lumpinfo[l]->name[6])
                {
                    frame = lumpinfo[l]->name[6] - 'A';
                    rotation = lumpinfo[l]->name[7] - '0';
                    R_InstallSpriteLump(l, frame, rotation, true);
                }
            }

        //
        // check the frames that were found for completeness
        //
        if (maxframe == -1)
        {
            //continue;
            sprites[i].numframes = 0;
            if (gamemode == shareware)
                continue;
            I_Error(english_language ?
                    "R_InitSprites: No lumps found for sprite %s" :
                    "R_InitSprites: не найдены блоки в спрайте %s",
                    namelist[i]);
        }

        maxframe++;
        for (frame = 0; frame < maxframe; frame++)
        {
            switch ((int) sprtemp[frame].rotate)
            {
                case -1:       // no rotations were found for that frame at all
                    I_Error(english_language ?
                            "R_InitSprites: No patches found for %s frame %c" :
                            "R_InitSprites: не найдены патчи для спрайта %s, фрейма %c",
                            namelist[i], frame + 'A');
                case 0:        // only the first rotation is needed
                    break;

                case 1:        // must have all 8 frames
                    for (rotation = 0; rotation < 8; rotation++)
                        if (sprtemp[frame].lump[rotation] == -1)
                            I_Error (english_language ?
                                     "R_InitSprites: Sprite %s frame %c is missing rotations" :
                                     "R_InitSprites: в фрейме %c спрайта %s отсутствует информация о вращении",
                                     namelist[i], frame + 'A');
            }
        }

        //
        // allocate space for the frames present and copy sprtemp to it
        //
        sprites[i].numframes = maxframe;
        sprites[i].spriteframes =
            Z_Malloc(maxframe * sizeof(spriteframe_t), PU_STATIC, NULL);
        memcpy(sprites[i].spriteframes, sprtemp,
               maxframe * sizeof(spriteframe_t));
    }

}


/*
===============================================================================

							GAME FUNCTIONS

===============================================================================
*/

vissprite_t vissprites[MAXVISSPRITES], *vissprite_p;
int newvissprite;


/*
===================
=
= R_InitSprites
=
= Called at program start
===================
*/

void R_InitSprites(char **namelist)
{
    int i;

    for (i = 0; i < screenwidth; i++)
    {
        negonearray[i] = -1;
    }

    R_InitSpriteDefs(namelist);
}


/*
===================
=
= R_ClearSprites
=
= Called at frame start
===================
*/

void R_ClearSprites(void)
{
    vissprite_p = vissprites;
}


/*
===================
=
= R_NewVisSprite
=
===================
*/

vissprite_t overflowsprite;

vissprite_t *R_NewVisSprite(void)
{
    if (vissprite_p == &vissprites[MAXVISSPRITES])
        return &overflowsprite;
    vissprite_p++;
    return vissprite_p - 1;
}


/*
================
=
= R_DrawMaskedColumn
=
= Used for sprites and masked mid textures
================
*/

int* mfloorclip;   // [crispy] 32-bit integer math
int* mceilingclip; // [crispy] 32-bit integer math
fixed_t spryscale;
fixed_t sprtopscreen;
fixed_t sprbotscreen;

void R_DrawMaskedColumn(column_t * column, signed int baseclip)
{
    int topscreen, bottomscreen;
    fixed_t basetexturemid;

    basetexturemid = dc_texturemid;
    dc_texheight = 0;

    for (; column->topdelta != 0xff;)
    {
// calculate unclipped screen coordinates for post
        topscreen = sprtopscreen + spryscale * column->topdelta;
        bottomscreen = topscreen + spryscale * column->length;
        dc_yl = (topscreen + FRACUNIT - 1) >> FRACBITS;
        dc_yh = (bottomscreen - 1) >> FRACBITS;

        if (dc_yh >= mfloorclip[dc_x])
            dc_yh = mfloorclip[dc_x] - 1;
        if (dc_yl <= mceilingclip[dc_x])
            dc_yl = mceilingclip[dc_x] + 1;

        if (dc_yh >= baseclip && baseclip != -1)
            dc_yh = baseclip;

        if (dc_yl <= dc_yh)
        {
            dc_source = (byte *) column + 3;
            dc_texturemid = basetexturemid - (column->topdelta << FRACBITS);
//                      dc_source = (byte *)column + 3 - column->topdelta;
            colfunc();          // either R_DrawColumn or R_DrawTLColumn
        }
        column = (column_t *) ((byte *) column + column->length + 4);
    }

    dc_texturemid = basetexturemid;
}


/*
================
=
= R_DrawVisSprite
=
= mfloorclip and mceilingclip should also be set
================
*/

void R_DrawVisSprite(vissprite_t * vis, int x1, int x2)
{
    column_t *column;
    int texturecolumn;
    fixed_t frac;
    patch_t *patch;
    fixed_t baseclip;


    patch = W_CacheLumpNum(vis->patch + firstspritelump, PU_CACHE);

    dc_colormap = vis->colormap;

//      if(!dc_colormap)
//              colfunc = tlcolfunc;  // NULL colormap = shadow draw

    if (vis->mobjflags & (MF_SHADOW | MF_ALTSHADOW))
    {
        if (vis->mobjflags & MF_TRANSLATION)
        {
            colfunc = R_DrawTranslatedTLColumn;
            dc_translation = translationtables - 256
                + vis->class * ((maxplayers - 1) * 256) +
                ((vis->mobjflags & MF_TRANSLATION) >> (MF_TRANSSHIFT - 8));
        }
        else if (vis->mobjflags & MF_SHADOW)
        {                       // Draw using shadow column function
            colfunc = tlcolfunc;
        }
        else
        {
            colfunc = R_DrawAltTLColumn;
        }
    }
    else if (vis->mobjflags & MF_TRANSLATION)
    {
        // Draw using translated column function
        colfunc = R_DrawTranslatedColumn;
        dc_translation = translationtables - 256
            + vis->class * ((maxplayers - 1) * 256) +
            ((vis->mobjflags & MF_TRANSLATION) >> (MF_TRANSSHIFT - 8));
    }

    dc_iscale = abs(vis->xiscale) >> detailshift;
    dc_texturemid = vis->texturemid;
    frac = vis->startfrac;
    spryscale = vis->scale;

    sprtopscreen = centeryfrac - FixedMul(dc_texturemid, spryscale);

    // check to see if vissprite is a weapon
    if (vis->psprite)
    {
        dc_texturemid += FixedMul(((centery - viewheight / 2) << FRACBITS),
                                  pspriteiscale);
        sprtopscreen += (viewheight / 2 - centery) << FRACBITS;
    }

    if (vis->floorclip && !vis->psprite)
    {
        sprbotscreen = sprtopscreen + FixedMul(SHORT(patch->height) << FRACBITS,
                                               spryscale);
        baseclip = (sprbotscreen - FixedMul(vis->floorclip,
                                            spryscale)) >> FRACBITS;
    }
    else
    {
        baseclip = -1;
    }

    for (dc_x = vis->x1; dc_x <= vis->x2; dc_x++, frac += vis->xiscale)
    {
        texturecolumn = frac >> FRACBITS;
#ifdef RANGECHECK
        if (texturecolumn < 0 || texturecolumn >= SHORT(patch->width))
            I_Error(english_language ?
                    "R_DrawSpriteRange: bad texturecolumn" :
                    "R_DrawSpriteRange: некорректныая информация texturecolumn");
#endif
        column = (column_t *) ((byte *) patch +
                               LONG(patch->columnofs[texturecolumn]));
        R_DrawMaskedColumn(column, baseclip);
    }

    colfunc = basecolfunc;
}



/*
===================
=
= R_ProjectSprite
=
= Generates a vissprite for a thing if it might be visible
=
===================
*/

void R_ProjectSprite(mobj_t * thing)
{
    fixed_t trx, try;
    fixed_t gxt, gyt;
    fixed_t tx, tz;
    fixed_t xscale;
    int x1, x2;
    spritedef_t *sprdef;
    spriteframe_t *sprframe;
    int lump;
    unsigned rot;
    boolean flip;
    int index;
    vissprite_t *vis;
    angle_t ang;
    fixed_t iscale;

    fixed_t             interpx;
    fixed_t             interpy;
    fixed_t             interpz;
    fixed_t             interpangle;

    // [AM] Interpolate between current and last position,
    //      if prudent.
    if (uncapped_fps && !vanillaparm &&
        // Don't interpolate if the mobj did something
        // that would necessitate turning it off for a tic.
        thing->interp == true &&
        // Don't interpolate during a paused state.
        !paused && (!menuactive || demoplayback || netgame))
    {
        interpx = thing->oldx + FixedMul(thing->x - thing->oldx, fractionaltic);
        interpy = thing->oldy + FixedMul(thing->y - thing->oldy, fractionaltic);
        interpz = thing->oldz + FixedMul(thing->z - thing->oldz, fractionaltic);
        interpangle = R_InterpolateAngle(thing->oldangle, thing->angle, fractionaltic);
    }
    else
    {
        interpx = thing->x;
        interpy = thing->y;
        interpz = thing->z;
        interpangle = thing->angle;
    }

    if (thing->flags2 & MF2_DONTDRAW)
    {                           // Never make a vissprite when MF2_DONTDRAW is flagged.
        return;
    }

//
// transform the origin point
//
    trx = interpx - viewx;
    try = interpy - viewy;

    gxt = FixedMul(trx, viewcos);
    gyt = -FixedMul(try, viewsin);
    tz = gxt - gyt;

    if (tz < MINZ)
        return;                 // thing is behind view plane
    xscale = FixedDiv(projection, tz);

    gxt = -FixedMul(trx, viewsin);
    gyt = FixedMul(try, viewcos);
    tx = -(gyt + gxt);

    if (abs(tx) > (tz << 2))
        return;                 // too far off the side

//
// decide which patch to use for sprite reletive to player
//
#ifdef RANGECHECK
    if ((unsigned) thing->sprite >= numsprites)
        I_Error(english_language ?
                "R_ProjectSprite: invalid sprite number %i " :
                "R_ProjectSprite: некорректный номер спрайта %i ",
                thing->sprite);
#endif
    sprdef = &sprites[thing->sprite];
#ifdef RANGECHECK
    if ((thing->frame & FF_FRAMEMASK) >= sprdef->numframes)
        I_Error(english_language ?
                "R_ProjectSprite: invalid sprite frame %i : %i " :
                "R_ProjectSprite: некорректный фрейм спрайта %i : %i ",
                thing->sprite, thing->frame);
#endif
    sprframe = &sprdef->spriteframes[thing->frame & FF_FRAMEMASK];

    if (sprframe->rotate)
    {                           // choose a different rotation based on player view
        ang = R_PointToAngle(interpx, interpy);
        rot = (ang - interpangle + (unsigned) (ANG45 / 2) * 9) >> 29;
        lump = sprframe->lump[rot];
        flip = (boolean) sprframe->flip[rot];
    }
    else
    {                           // use single rotation for all views
        lump = sprframe->lump[0];
        flip = (boolean) sprframe->flip[0];
    }

//
// calculate edges of the shape
//
    tx -= spriteoffset[lump];
    x1 = (centerxfrac + FixedMul(tx, xscale)) >> FRACBITS;
    if (x1 > viewwidth)
        return;                 // off the right side
    tx += spritewidth[lump];
    x2 = ((centerxfrac + FixedMul(tx, xscale)) >> FRACBITS) - 1;
    if (x2 < 0)
        return;                 // off the left side


//
// store information in a vissprite
//
    vis = R_NewVisSprite();
    vis->mobjflags = thing->flags;
    vis->psprite = false;
    vis->scale = xscale << detailshift;
    vis->gx = interpx;
    vis->gy = interpy;
    vis->gz = interpz;
    vis->gzt = interpz + spritetopoffset[lump];
    if (thing->flags & MF_TRANSLATION)
    {
        if (thing->player)
        {
            vis->class = thing->player->class;
        }
        else
        {
            vis->class = thing->special1.i;
        }
        if (vis->class > 2)
        {
            vis->class = 0;
        }
    }
    // foot clipping
    vis->floorclip = thing->floorclip;
    vis->texturemid = vis->gzt - viewz - vis->floorclip;

    vis->x1 = x1 < 0 ? 0 : x1;
    vis->x2 = x2 >= viewwidth ? viewwidth - 1 : x2;
    iscale = FixedDiv(FRACUNIT, xscale);
    if (flip)
    {
        vis->startfrac = spritewidth[lump] - 1;
        vis->xiscale = -iscale;
    }
    else
    {
        vis->startfrac = 0;
        vis->xiscale = iscale;
    }
    if (vis->x1 > x1)
        vis->startfrac += vis->xiscale * (vis->x1 - x1);
    vis->patch = lump;
//
// get light level
//

//      if (thing->flags & MF_SHADOW)
//              vis->colormap = NULL;                   // shadow draw
//      else ...

    if (fixedcolormap)
        vis->colormap = fixedcolormap;  // fixed map
    else if (LevelUseFullBright && thing->frame & FF_FULLBRIGHT)
        vis->colormap = colormaps;      // full bright
    else
    {                           // diminished light
        index = xscale >> (LIGHTSCALESHIFT - detailshift + hires);
        if (index >= MAXLIGHTSCALE)
            index = MAXLIGHTSCALE - 1;
        vis->colormap = spritelights[index];

        // [JN] Applying brightmaps to sprites...
        
        if (brightmaps && !vanillaparm && LevelUseFullBright == true)
        {
            // - Red only -

            // Banishment Device
            if (thing->type == MT_TELEPORTOTHER) 
            vis->colormap = fullbrights_redonly[index];

            // Chaos Device
            if (thing->type == MT_ARTITELEPORT)
            vis->colormap = fullbrights_redonly[index];

            // Korax
            if (thing->type == MT_KORAX)
            vis->colormap = fullbrights_redonly[index];

            // Flame Mask
            if (thing->type == MT_ARTIPUZZSKULL2)
            vis->colormap = fullbrights_redonly[index];            

            // - Blue only -

            // Crater of Might
            if (thing->type == MT_SPEEDBOOTS)
            vis->colormap = fullbrights_blueonly[index];

            // Blue candle (lit)
            if (thing->type == MT_ZBLUE_CANDLE)
            vis->colormap = fullbrights_blueonly[index];

            // Wendigo
            if (thing->type == MT_ICEGUY)
            vis->colormap = fullbrights_blueonly[index];

            // Mystic Ambit Incant
            if (thing->type == MT_HEALRADIUS)
            vis->colormap = fullbrights_blueonly[index];

            // - Purple only -

            // Crater of Might
            if (thing->type == MT_BOOSTMANA)
            vis->colormap = fullbrights_purpleonly[index];

            // Dragonskin Bracers
            if (thing->type == MT_BOOSTARMOR)
            vis->colormap = fullbrights_purpleonly[index];

            // Icon of the Defender
            if (thing->type == MT_ARTIINVULNERABILITY)
            vis->colormap = fullbrights_purpleonly[index];

            // - Flame -

            // Torch (Artifact)
            if (thing->type == MT_ARTITORCH)
            vis->colormap = fullbrights_flame[index];

            // 3 candles (lit)
            if (thing->type == MT_BRASSTORCH)
            vis->colormap = fullbrights_flame[index];

            // Skull with Flame
            if (thing->type == MT_FIRETHING)
            vis->colormap = fullbrights_flame[index];

            // Twined Torch
            if (thing->type == MT_ZTWINEDTORCH || thing->type == MT_ZTWINEDTORCH_UNLIT)
            vis->colormap = fullbrights_flame[index];

            // Wall torch
            if (thing->type == MT_ZWALLTORCH || thing->type == MT_ZWALLTORCH_UNLIT)
            vis->colormap = fullbrights_flame[index];

            // Chandelier
            if (thing->type == MT_MISC5 || thing->type == MT_MISC6)
            vis->colormap = fullbrights_flame[index];

            // Cauldron
            if (thing->type == MT_ZCAULDRON)
            vis->colormap = fullbrights_flame[index];
        
            // - Fire Bull -
            
            if (thing->type == MT_ZFIREBULL || thing->type == MT_ZFIREBULL_UNLIT)
            vis->colormap = fullbrights_firebull[index];
        }
        
        // [JN] Fallback. If we are not using brightmaps, apply full brightness
        // to the objects, that no longer lighten up in info.c.
        if ((!brightmaps || vanillaparm) && LevelUseFullBright == true)
        {
            if (thing->type == MT_ARTITORCH
            ||  thing->type == MT_SPEEDBOOTS
            ||  thing->type == MT_BOOSTMANA
            ||  thing->type == MT_BOOSTARMOR
            ||  thing->type == MT_HEALRADIUS
            ||  thing->type == MT_ZTWINEDTORCH
            ||  thing->type == MT_ZWALLTORCH
            ||  thing->type == MT_ZFIREBULL
            ||  thing->type == MT_FIRETHING
            ||  thing->type == MT_BRASSTORCH
            ||  thing->type == MT_ZBLUE_CANDLE
            ||  thing->type == MT_ZCAULDRON)
            vis->colormap = colormaps;
        }
    }
}




/*
========================
=
= R_AddSprites
=
========================
*/

void R_AddSprites(sector_t * sec)
{
    mobj_t *thing;
    int lightnum;

    if (sec->validcount == validcount)
        return;                 // already added

    sec->validcount = validcount;

    lightnum = ((sec->lightlevel + level_brightness) >> LIGHTSEGSHIFT) + extralight;
    if (lightnum < 0)
    {
        spritelights = scalelight[0];

        // [JN] Brightmaps
        fullbrights_greenonly = fullbright_greenonly[0];
        fullbrights_redonly = fullbright_redonly[0];
        fullbrights_blueonly = fullbright_blueonly[0];
        fullbrights_purpleonly = fullbright_purpleonly[0];
        fullbrights_flame = fullbright_flame[0];
        fullbrights_yellowred = fullbright_yellowred[0];
        fullbrights_firebull = fullbright_firebull[0];
    }
    else if (lightnum >= LIGHTLEVELS)
    {
        spritelights = scalelight[LIGHTLEVELS - 1];

        // [JN] Brightmaps
        fullbrights_greenonly = fullbright_greenonly[LIGHTLEVELS - 1];
        fullbrights_redonly = fullbright_redonly[LIGHTLEVELS - 1];
        fullbrights_blueonly = fullbright_blueonly[LIGHTLEVELS - 1];
        fullbrights_purpleonly = fullbright_purpleonly[LIGHTLEVELS - 1];
        fullbrights_flame = fullbright_flame[LIGHTLEVELS - 1];
        fullbrights_yellowred = fullbright_yellowred[LIGHTLEVELS - 1];
        fullbrights_firebull = fullbright_firebull[LIGHTLEVELS - 1];
    }
    else
    {
        spritelights = scalelight[lightnum];

        // [JN] Brightmaps
        fullbrights_greenonly = fullbright_greenonly[lightnum];
        fullbrights_redonly = fullbright_redonly[lightnum];
        fullbrights_blueonly = fullbright_blueonly[lightnum];
        fullbrights_purpleonly = fullbright_purpleonly[lightnum];
        fullbrights_flame = fullbright_flame[lightnum];
        fullbrights_yellowred = fullbright_yellowred[lightnum];
        fullbrights_firebull = fullbright_firebull[lightnum];
    }


    for (thing = sec->thinglist; thing; thing = thing->snext)
        R_ProjectSprite(thing);
}


/*
========================
=
= R_DrawPSprite
=
========================
*/

// Y-adjustment values for full screen (4 weapons)
int PSpriteSY[NUMCLASSES][NUMWEAPONS] = {
    {0, -12 * FRACUNIT, -10 * FRACUNIT, 10 * FRACUNIT}, // Fighter
    {-8 * FRACUNIT, 10 * FRACUNIT, 10 * FRACUNIT, 0},   // Cleric
    {9 * FRACUNIT, 20 * FRACUNIT, 20 * FRACUNIT, 20 * FRACUNIT},        // Mage 
    {10 * FRACUNIT, 10 * FRACUNIT, 10 * FRACUNIT, 10 * FRACUNIT}        // Pig
};

void R_DrawPSprite(pspdef_t * psp)
{
    fixed_t tx;
    int x1, x2;
    spritedef_t *sprdef;
    spriteframe_t *sprframe;
    int lump;
    boolean flip;
    vissprite_t *vis, avis;

    int tempangle;

//
// decide which patch to use
//
#ifdef RANGECHECK
    if ((unsigned) psp->state->sprite >= numsprites)
        I_Error(english_language ?
                "R_ProjectSprite: invalid sprite number %i " :
                "R_ProjectSprite: некорректный номер спрайта %i ",
                psp->state->sprite);
#endif
    sprdef = &sprites[psp->state->sprite];
#ifdef RANGECHECK
    if ((psp->state->frame & FF_FRAMEMASK) >= sprdef->numframes)
        I_Error(english_language ?
                "R_ProjectSprite: invalid sprite frame %i : %i " :
                "R_ProjectSprite: некорректный фрейм спрайта %i : %i ",
                psp->state->sprite, psp->state->frame);
#endif
    sprframe = &sprdef->spriteframes[psp->state->frame & FF_FRAMEMASK];

    lump = sprframe->lump[0];
    flip = (boolean)sprframe->flip[0] ^ flip_levels;

//
// calculate edges of the shape
//
    tx = psp->sx - 160 * FRACUNIT;

    // [crispy] fix sprite offsets for mirrored sprites
    tx -= flip ? 2 * tx - spriteoffset[lump] + spritewidth[lump] : spriteoffset[lump];
    if (viewangleoffset)
    {
        tempangle =
            ((centerxfrac / 1024) * (viewangleoffset >> ANGLETOFINESHIFT));
    }
    else
    {
        tempangle = 0;
    }
    x1 = (centerxfrac + FixedMul(tx, pspritescale) + tempangle) >> FRACBITS;
    if (x1 > viewwidth)
        return;                 // off the right side
    tx += spritewidth[lump];
    x2 = ((centerxfrac + FixedMul(tx, pspritescale) +
           tempangle) >> FRACBITS) - 1;
    if (x2 < 0)
        return;                 // off the left side

//
// store information in a vissprite
//
    vis = &avis;
    vis->mobjflags = 0;
    vis->class = 0;
    vis->psprite = true;
    // [crispy] weapons drawn 1 pixel too high when player is idle
    vis->texturemid = (BASEYCENTER<<FRACBITS)+FRACUNIT/4-(psp->sy-spritetopoffset[lump]);

    if (viewheight == SCREENHEIGHT)
    {
        vis->texturemid -= PSpriteSY[viewplayer->class]
            [players[consoleplayer].readyweapon];
    }
    vis->x1 = x1 < 0 ? 0 : x1;
    vis->x2 = x2 >= viewwidth ? viewwidth - 1 : x2;
    vis->scale = pspritescale << detailshift;
    if (flip)
    {
        vis->xiscale = -pspriteiscale;
        vis->startfrac = spritewidth[lump] - 1;
    }
    else
    {
        vis->xiscale = pspriteiscale;
        vis->startfrac = 0;
    }
    if (vis->x1 > x1)
        vis->startfrac += vis->xiscale * (vis->x1 - x1);
    vis->patch = lump;

    if (viewplayer->powers[pw_invulnerability] && viewplayer->class
        == PCLASS_CLERIC)
    {
        vis->colormap = spritelights[MAXLIGHTSCALE - 1];
        if (viewplayer->powers[pw_invulnerability] > 4 * 32)
        {
            if (viewplayer->mo->flags2 & MF2_DONTDRAW)
            {                   // don't draw the psprite
                vis->mobjflags |= MF_SHADOW;
            }
            else if (viewplayer->mo->flags & MF_SHADOW)
            {
                vis->mobjflags |= MF_ALTSHADOW;
            }
        }
        else if (viewplayer->powers[pw_invulnerability] & 8)
        {
            vis->mobjflags |= MF_SHADOW;
        }
    }
    else if (fixedcolormap)
    {
        // Fixed color
        vis->colormap = fixedcolormap;
    }
    else if (psp->state->frame & FF_FULLBRIGHT)
    {
        // Full bright
        vis->colormap = colormaps;
    }
    else
    {
        // local light
        vis->colormap = spritelights[MAXLIGHTSCALE - 1];
    }
    R_DrawVisSprite(vis, vis->x1, vis->x2);
}

/*
========================
=
= R_DrawPlayerSprites
=
========================
*/

void R_DrawPlayerSprites(void)
{
    int i, lightnum;
    pspdef_t *psp;
    const int state = viewplayer->psprites[ps_weapon].state - states; // [from-crispy] We need to define what "state" actually is

//
// get light level
//
    lightnum =
        ((viewplayer->mo->subsector->sector->lightlevel + level_brightness) >> LIGHTSEGSHIFT) +
        extralight;
    if (lightnum < 0)
    {
        spritelights = scalelight[0];

        // [JN] Applying brightmaps to HUD weapons...
        if (brightmaps && !vanillaparm)
        {
            // Fighter: Axe
            if (state == S_FAXEREADY_G || state == S_FAXEREADY_G1 || state == S_FAXEREADY_G2 || state == S_FAXEREADY_G3 || state == S_FAXEREADY_G4 || state == S_FAXEREADY_G5 || state == S_FAXEDOWN_G || state == S_FAXEUP_G || state == S_FAXEUP_G || state == S_FAXEATK_G1 || state == S_FAXEATK_G2 || state == S_FAXEATK_G3 || state == S_FAXEATK_G4 || state == S_FAXEATK_G5 || state == S_FAXEATK_G6 || state == S_FAXEATK_G7 || state == S_FAXEATK_G8 || state == S_FAXEATK_G9 || state == S_FAXEATK_G10 || state == S_FAXEATK_G11 || state == S_FAXEATK_G12 || state == S_FAXEATK_G13)
                spritelights = fullbright_blueonly[0];
            // Fighter: Sword
            else if (state == S_FSWORDREADY || state == S_FSWORDREADY1 || state == S_FSWORDREADY2 || state == S_FSWORDREADY3 || state == S_FSWORDREADY4 || state == S_FSWORDREADY5 || state == S_FSWORDREADY6 || state == S_FSWORDREADY7 || state == S_FSWORDREADY8 || state == S_FSWORDREADY9 || state == S_FSWORDREADY10 || state == S_FSWORDREADY11 || state == S_FSWORDDOWN || state == S_FSWORDUP || state == S_FSWORDATK_1 || state == S_FSWORDATK_2 || state == S_FSWORDATK_3 || state == S_FSWORDATK_4 || state == S_FSWORDATK_5 || state == S_FSWORDATK_6 || state == S_FSWORDATK_7 || state == S_FSWORDATK_8 || state == S_FSWORDATK_9 || state == S_FSWORDATK_10 || state == S_FSWORDATK_11 || state == S_FSWORDATK_12)
                spritelights = fullbright_greenonly[0];
            // Cleric: Serpent Staff
            else if (state == S_CSTAFFATK_1 || state == S_CSTAFFATK_2 || state == S_CSTAFFATK_3 || state == S_CSTAFFATK_4 || state == S_CSTAFFATK2_1)
                spritelights = fullbright_greenonly[0];
            // Cleric: Flame
            else if (state == S_CFLAMEDOWN || state == S_CFLAMEUP || state == S_CFLAMEREADY1 || state == S_CFLAMEREADY2 || state == S_CFLAMEREADY3 || state == S_CFLAMEREADY4 || state == S_CFLAMEREADY5 || state == S_CFLAMEREADY6 || state == S_CFLAMEREADY7 || state == S_CFLAMEREADY8 || state == S_CFLAMEREADY9 || state == S_CFLAMEREADY10 || state == S_CFLAMEREADY11 || state == S_CFLAMEREADY12 || state == S_CFLAMEATK_1 || state == S_CFLAMEATK_2 || state == S_CFLAMEATK_3 || state == S_CFLAMEATK_7 || state == S_CFLAMEATK_8)
                spritelights = fullbright_yellowred[0];
            // Mage: Frost
            else if (state == S_CONEATK1_2 || state == S_CONEATK1_3 || state == S_CONEATK1_4 || state == S_CONEATK1_5 || state == S_CONEATK1_6 || state == S_CONEATK1_7 || state == S_CONEATK1_8)
                spritelights = fullbright_blueonly[0];
            // Mage: Lightning
            else if (state == S_MLIGHTNINGREADY || state == S_MLIGHTNINGREADY2 || state == S_MLIGHTNINGREADY3 || state == S_MLIGHTNINGREADY4 || state == S_MLIGHTNINGREADY5 || state == S_MLIGHTNINGREADY6 || state == S_MLIGHTNINGREADY7 || state == S_MLIGHTNINGREADY8 || state == S_MLIGHTNINGREADY9 || state == S_MLIGHTNINGREADY10 || state == S_MLIGHTNINGREADY11 || state == S_MLIGHTNINGREADY12 || state == S_MLIGHTNINGREADY13 || state == S_MLIGHTNINGREADY14 || state == S_MLIGHTNINGREADY15 || state == S_MLIGHTNINGREADY16 || state == S_MLIGHTNINGREADY17 || state == S_MLIGHTNINGREADY18 || state == S_MLIGHTNINGREADY19 || state == S_MLIGHTNINGREADY20 || state == S_MLIGHTNINGREADY21 || state == S_MLIGHTNINGREADY22 || state == S_MLIGHTNINGREADY23 || state == S_MLIGHTNINGREADY24 || state == S_MLIGHTNINGDOWN || state == S_MLIGHTNINGUP || state == S_MLIGHTNINGATK_1 || state == S_MLIGHTNINGATK_2 || state == S_MLIGHTNINGATK_3 || state == S_MLIGHTNINGATK_4 || state == S_MLIGHTNINGATK_5 || state == S_MLIGHTNINGATK_6 || state == S_MLIGHTNINGATK_7 || state == S_MLIGHTNINGATK_8 || state == S_MLIGHTNINGATK_9 || state == S_MLIGHTNINGATK_10 || state == S_MLIGHTNINGATK_11)
                spritelights = fullbright_blueonly[0];
            // Mage: Arc
            else if (state == S_MSTAFFREADY || state == S_MSTAFFREADY2 || state == S_MSTAFFREADY3 || state == S_MSTAFFREADY4 || state == S_MSTAFFREADY5 || state == S_MSTAFFREADY6 || state == S_MSTAFFREADY7 || state == S_MSTAFFREADY8 || state == S_MSTAFFREADY9 || state == S_MSTAFFREADY10 || state == S_MSTAFFREADY11 || state == S_MSTAFFREADY12 || state == S_MSTAFFREADY13 || state == S_MSTAFFREADY14 || state == S_MSTAFFREADY15 || state == S_MSTAFFREADY16 || state == S_MSTAFFREADY17 || state == S_MSTAFFREADY18 || state == S_MSTAFFREADY19 || state == S_MSTAFFREADY20 || state == S_MSTAFFREADY21 || state == S_MSTAFFREADY22 || state == S_MSTAFFREADY23 || state == S_MSTAFFREADY24 || state == S_MSTAFFREADY25 || state == S_MSTAFFREADY26 || state == S_MSTAFFREADY27 || state == S_MSTAFFREADY28 || state == S_MSTAFFREADY29 || state == S_MSTAFFREADY30 || state == S_MSTAFFREADY31 || state == S_MSTAFFREADY32 || state == S_MSTAFFREADY33 || state == S_MSTAFFREADY34 || state == S_MSTAFFREADY35 || state == S_MSTAFFDOWN || state == S_MSTAFFUP || state == S_MSTAFFATK_1 || state == S_MSTAFFATK_4 || state == S_MSTAFFATK_5 || state == S_MSTAFFATK_6 || state == S_MSTAFFATK_7)
                spritelights = fullbright_yellowred[0];
        }
        // [JN] Fallback. If we are not using brightmaps, apply full brightness
        // to the objects, that no longer lighten up in info.c.
        if (!brightmaps || vanillaparm)
        {
            if (state == S_FSWORDREADY || state == S_FSWORDREADY1 || state == S_FSWORDREADY2 || state == S_FSWORDREADY3 || state == S_FSWORDREADY4 || state == S_FSWORDREADY5 || state == S_FSWORDREADY6 || state == S_FSWORDREADY7 || state == S_FSWORDREADY8 || state == S_FSWORDREADY9 || state == S_FSWORDREADY10 || state == S_FSWORDREADY11 || state == S_FSWORDDOWN || state == S_FSWORDUP || state == S_FSWORDATK_1 || state == S_FSWORDATK_2 || state == S_FSWORDATK_3 || state == S_FSWORDATK_4 || state == S_FSWORDATK_5 || state == S_FSWORDATK_6 || state == S_FSWORDATK_7 || state == S_FSWORDATK_8 || state == S_FSWORDATK_9 || state == S_FSWORDATK_10 || state == S_FSWORDATK_11 || state == S_FSWORDATK_12 || state == S_MLIGHTNINGREADY || state == S_MLIGHTNINGREADY2 || state == S_MLIGHTNINGREADY3 || state == S_MLIGHTNINGREADY4 || state == S_MLIGHTNINGREADY5 || state == S_MLIGHTNINGREADY6 || state == S_MLIGHTNINGREADY7 || state == S_MLIGHTNINGREADY8 || state == S_MLIGHTNINGREADY9 || state == S_MLIGHTNINGREADY10 || state == S_MLIGHTNINGREADY11 || state == S_MLIGHTNINGREADY12 || state == S_MLIGHTNINGREADY13 || state == S_MLIGHTNINGREADY14 || state == S_MLIGHTNINGREADY15 || state == S_MLIGHTNINGREADY16 || state == S_MLIGHTNINGREADY17 || state == S_MLIGHTNINGREADY18 || state == S_MLIGHTNINGREADY19 || state == S_MLIGHTNINGREADY20 || state == S_MLIGHTNINGREADY21 || state == S_MLIGHTNINGREADY22 || state == S_MLIGHTNINGREADY23 || state == S_MLIGHTNINGREADY24 || state == S_MLIGHTNINGDOWN || state == S_MLIGHTNINGUP || state == S_MLIGHTNINGATK_1 || state == S_MLIGHTNINGATK_2 || state == S_MLIGHTNINGATK_3 || state == S_MLIGHTNINGATK_4 || state == S_MLIGHTNINGATK_5 || state == S_MLIGHTNINGATK_6 || state == S_MLIGHTNINGATK_7 || state == S_MLIGHTNINGATK_8 || state == S_MLIGHTNINGATK_9 || state == S_MLIGHTNINGATK_10 || state == S_MLIGHTNINGATK_11)
                spritelights = scalelight[LIGHTLEVELS - 1];
        }
    }
    else if (lightnum >= LIGHTLEVELS)
    {
        spritelights = scalelight[LIGHTLEVELS - 1];
        
        // [JN] Applying brightmaps to HUD weapons...
        if (brightmaps && !vanillaparm)
        {
            // Fighter: Axe
            if (state == S_FAXEREADY_G || state == S_FAXEREADY_G1 || state == S_FAXEREADY_G2 || state == S_FAXEREADY_G3 || state == S_FAXEREADY_G4 || state == S_FAXEREADY_G5 || state == S_FAXEDOWN_G || state == S_FAXEUP_G || state == S_FAXEUP_G || state == S_FAXEATK_G1 || state == S_FAXEATK_G2 || state == S_FAXEATK_G3 || state == S_FAXEATK_G4 || state == S_FAXEATK_G5 || state == S_FAXEATK_G6 || state == S_FAXEATK_G7 || state == S_FAXEATK_G8 || state == S_FAXEATK_G9 || state == S_FAXEATK_G10 || state == S_FAXEATK_G11 || state == S_FAXEATK_G12 || state == S_FAXEATK_G13)
                spritelights = fullbright_blueonly[LIGHTLEVELS - 1];
            // Fighter: Sword
            else if (state == S_FSWORDREADY || state == S_FSWORDREADY1 || state == S_FSWORDREADY2 || state == S_FSWORDREADY3 || state == S_FSWORDREADY4 || state == S_FSWORDREADY5 || state == S_FSWORDREADY6 || state == S_FSWORDREADY7 || state == S_FSWORDREADY8 || state == S_FSWORDREADY9 || state == S_FSWORDREADY10 || state == S_FSWORDREADY11 || state == S_FSWORDDOWN || state == S_FSWORDUP || state == S_FSWORDATK_1 || state == S_FSWORDATK_2 || state == S_FSWORDATK_3 || state == S_FSWORDATK_4 || state == S_FSWORDATK_5 || state == S_FSWORDATK_6 || state == S_FSWORDATK_7 || state == S_FSWORDATK_8 || state == S_FSWORDATK_9 || state == S_FSWORDATK_10 || state == S_FSWORDATK_11 || state == S_FSWORDATK_12)
                spritelights = fullbright_greenonly[LIGHTLEVELS - 1];
            // Cleric: Serpent Staff
            else if (state == S_CSTAFFATK_1 || state == S_CSTAFFATK_2 || state == S_CSTAFFATK_3 || state == S_CSTAFFATK_4 || state == S_CSTAFFATK2_1)
                spritelights = fullbright_greenonly[LIGHTLEVELS - 1];
            // Cleric: Flame
            else if (state == S_CFLAMEDOWN || state == S_CFLAMEUP || state == S_CFLAMEREADY1 || state == S_CFLAMEREADY2 || state == S_CFLAMEREADY3 || state == S_CFLAMEREADY4 || state == S_CFLAMEREADY5 || state == S_CFLAMEREADY6 || state == S_CFLAMEREADY7 || state == S_CFLAMEREADY8 || state == S_CFLAMEREADY9 || state == S_CFLAMEREADY10 || state == S_CFLAMEREADY11 || state == S_CFLAMEREADY12 || state == S_CFLAMEATK_1 || state == S_CFLAMEATK_2 || state == S_CFLAMEATK_3 || state == S_CFLAMEATK_7 || state == S_CFLAMEATK_8)
                spritelights = fullbright_yellowred[LIGHTLEVELS - 1];
            // Mage: Frost
            else if (state == S_CONEATK1_2 || state == S_CONEATK1_3 || state == S_CONEATK1_4 || state == S_CONEATK1_5 || state == S_CONEATK1_6 || state == S_CONEATK1_7 || state == S_CONEATK1_8)
                spritelights = fullbright_blueonly[LIGHTLEVELS - 1];
            // Mage: Lightning
            else if (state == S_MLIGHTNINGREADY || state == S_MLIGHTNINGREADY2 || state == S_MLIGHTNINGREADY3 || state == S_MLIGHTNINGREADY4 || state == S_MLIGHTNINGREADY5 || state == S_MLIGHTNINGREADY6 || state == S_MLIGHTNINGREADY7 || state == S_MLIGHTNINGREADY8 || state == S_MLIGHTNINGREADY9 || state == S_MLIGHTNINGREADY10 || state == S_MLIGHTNINGREADY11 || state == S_MLIGHTNINGREADY12 || state == S_MLIGHTNINGREADY13 || state == S_MLIGHTNINGREADY14 || state == S_MLIGHTNINGREADY15 || state == S_MLIGHTNINGREADY16 || state == S_MLIGHTNINGREADY17 || state == S_MLIGHTNINGREADY18 || state == S_MLIGHTNINGREADY19 || state == S_MLIGHTNINGREADY20 || state == S_MLIGHTNINGREADY21 || state == S_MLIGHTNINGREADY22 || state == S_MLIGHTNINGREADY23 || state == S_MLIGHTNINGREADY24 || state == S_MLIGHTNINGDOWN || state == S_MLIGHTNINGUP || state == S_MLIGHTNINGATK_1 || state == S_MLIGHTNINGATK_2 || state == S_MLIGHTNINGATK_3 || state == S_MLIGHTNINGATK_4 || state == S_MLIGHTNINGATK_5 || state == S_MLIGHTNINGATK_6 || state == S_MLIGHTNINGATK_7 || state == S_MLIGHTNINGATK_8 || state == S_MLIGHTNINGATK_9 || state == S_MLIGHTNINGATK_10 || state == S_MLIGHTNINGATK_11)
                spritelights = fullbright_blueonly[LIGHTLEVELS - 1];
            // Mage: Arc
            else if (state == S_MSTAFFREADY || state == S_MSTAFFREADY2 || state == S_MSTAFFREADY3 || state == S_MSTAFFREADY4 || state == S_MSTAFFREADY5 || state == S_MSTAFFREADY6 || state == S_MSTAFFREADY7 || state == S_MSTAFFREADY8 || state == S_MSTAFFREADY9 || state == S_MSTAFFREADY10 || state == S_MSTAFFREADY11 || state == S_MSTAFFREADY12 || state == S_MSTAFFREADY13 || state == S_MSTAFFREADY14 || state == S_MSTAFFREADY15 || state == S_MSTAFFREADY16 || state == S_MSTAFFREADY17 || state == S_MSTAFFREADY18 || state == S_MSTAFFREADY19 || state == S_MSTAFFREADY20 || state == S_MSTAFFREADY21 || state == S_MSTAFFREADY22 || state == S_MSTAFFREADY23 || state == S_MSTAFFREADY24 || state == S_MSTAFFREADY25 || state == S_MSTAFFREADY26 || state == S_MSTAFFREADY27 || state == S_MSTAFFREADY28 || state == S_MSTAFFREADY29 || state == S_MSTAFFREADY30 || state == S_MSTAFFREADY31 || state == S_MSTAFFREADY32 || state == S_MSTAFFREADY33 || state == S_MSTAFFREADY34 || state == S_MSTAFFREADY35 || state == S_MSTAFFDOWN || state == S_MSTAFFUP || state == S_MSTAFFATK_1 || state == S_MSTAFFATK_4 || state == S_MSTAFFATK_5 || state == S_MSTAFFATK_6 || state == S_MSTAFFATK_7)
                spritelights = fullbright_yellowred[LIGHTLEVELS - 1];
        }
    }
    else
    {
        spritelights = scalelight[lightnum];
        
        // [JN] Applying brightmaps to HUD weapons...
        if (brightmaps && !vanillaparm)
        {
            // Fighter: Axe
            if (state == S_FAXEREADY_G || state == S_FAXEREADY_G1 || state == S_FAXEREADY_G2 || state == S_FAXEREADY_G3 || state == S_FAXEREADY_G4 || state == S_FAXEREADY_G5 || state == S_FAXEDOWN_G || state == S_FAXEUP_G || state == S_FAXEUP_G || state == S_FAXEATK_G1 || state == S_FAXEATK_G2 || state == S_FAXEATK_G3 || state == S_FAXEATK_G4 || state == S_FAXEATK_G5 || state == S_FAXEATK_G6 || state == S_FAXEATK_G7 || state == S_FAXEATK_G8 || state == S_FAXEATK_G9 || state == S_FAXEATK_G10 || state == S_FAXEATK_G11 || state == S_FAXEATK_G12 || state == S_FAXEATK_G13)
                spritelights = fullbright_blueonly[lightnum];
            // Fighter: Sword
            else if (state == S_FSWORDREADY || state == S_FSWORDREADY1 || state == S_FSWORDREADY2 || state == S_FSWORDREADY3 || state == S_FSWORDREADY4 || state == S_FSWORDREADY5 || state == S_FSWORDREADY6 || state == S_FSWORDREADY7 || state == S_FSWORDREADY8 || state == S_FSWORDREADY9 || state == S_FSWORDREADY10 || state == S_FSWORDREADY11 || state == S_FSWORDDOWN || state == S_FSWORDUP || state == S_FSWORDATK_1 || state == S_FSWORDATK_2 || state == S_FSWORDATK_3 || state == S_FSWORDATK_4 || state == S_FSWORDATK_5 || state == S_FSWORDATK_6 || state == S_FSWORDATK_7 || state == S_FSWORDATK_8 || state == S_FSWORDATK_9 || state == S_FSWORDATK_10 || state == S_FSWORDATK_11 || state == S_FSWORDATK_12)
                spritelights = fullbright_greenonly[lightnum];
            // Cleric: Serpent Staff
            else if (state == S_CSTAFFATK_1 || state == S_CSTAFFATK_2 || state == S_CSTAFFATK_3 || state == S_CSTAFFATK_4 || state == S_CSTAFFATK2_1)
                spritelights = fullbright_greenonly[lightnum];
            // Cleric: Flame
            else if (state == S_CFLAMEDOWN || state == S_CFLAMEUP || state == S_CFLAMEREADY1 || state == S_CFLAMEREADY2 || state == S_CFLAMEREADY3 || state == S_CFLAMEREADY4 || state == S_CFLAMEREADY5 || state == S_CFLAMEREADY6 || state == S_CFLAMEREADY7 || state == S_CFLAMEREADY8 || state == S_CFLAMEREADY9 || state == S_CFLAMEREADY10 || state == S_CFLAMEREADY11 || state == S_CFLAMEREADY12 || state == S_CFLAMEATK_1 || state == S_CFLAMEATK_2 || state == S_CFLAMEATK_3 || state == S_CFLAMEATK_7 || state == S_CFLAMEATK_8)
                spritelights = fullbright_yellowred[lightnum];
            // Mage: Frost
            else if (state == S_CONEATK1_2 || state == S_CONEATK1_3 || state == S_CONEATK1_4 || state == S_CONEATK1_5 || state == S_CONEATK1_6 || state == S_CONEATK1_7 || state == S_CONEATK1_8)
                spritelights = fullbright_blueonly[lightnum];
            // Mage: Lightning
            else if (state == S_MLIGHTNINGREADY || state == S_MLIGHTNINGREADY2 || state == S_MLIGHTNINGREADY3 || state == S_MLIGHTNINGREADY4 || state == S_MLIGHTNINGREADY5 || state == S_MLIGHTNINGREADY6 || state == S_MLIGHTNINGREADY7 || state == S_MLIGHTNINGREADY8 || state == S_MLIGHTNINGREADY9 || state == S_MLIGHTNINGREADY10 || state == S_MLIGHTNINGREADY11 || state == S_MLIGHTNINGREADY12 || state == S_MLIGHTNINGREADY13 || state == S_MLIGHTNINGREADY14 || state == S_MLIGHTNINGREADY15 || state == S_MLIGHTNINGREADY16 || state == S_MLIGHTNINGREADY17 || state == S_MLIGHTNINGREADY18 || state == S_MLIGHTNINGREADY19 || state == S_MLIGHTNINGREADY20 || state == S_MLIGHTNINGREADY21 || state == S_MLIGHTNINGREADY22 || state == S_MLIGHTNINGREADY23 || state == S_MLIGHTNINGREADY24 || state == S_MLIGHTNINGDOWN || state == S_MLIGHTNINGUP || state == S_MLIGHTNINGATK_1 || state == S_MLIGHTNINGATK_2 || state == S_MLIGHTNINGATK_3 || state == S_MLIGHTNINGATK_4 || state == S_MLIGHTNINGATK_5 || state == S_MLIGHTNINGATK_6 || state == S_MLIGHTNINGATK_7 || state == S_MLIGHTNINGATK_8 || state == S_MLIGHTNINGATK_9 || state == S_MLIGHTNINGATK_10 || state == S_MLIGHTNINGATK_11)
                spritelights = fullbright_blueonly[lightnum];
            // Mage: Arc
            else if (state == S_MSTAFFREADY || state == S_MSTAFFREADY2 || state == S_MSTAFFREADY3 || state == S_MSTAFFREADY4 || state == S_MSTAFFREADY5 || state == S_MSTAFFREADY6 || state == S_MSTAFFREADY7 || state == S_MSTAFFREADY8 || state == S_MSTAFFREADY9 || state == S_MSTAFFREADY10 || state == S_MSTAFFREADY11 || state == S_MSTAFFREADY12 || state == S_MSTAFFREADY13 || state == S_MSTAFFREADY14 || state == S_MSTAFFREADY15 || state == S_MSTAFFREADY16 || state == S_MSTAFFREADY17 || state == S_MSTAFFREADY18 || state == S_MSTAFFREADY19 || state == S_MSTAFFREADY20 || state == S_MSTAFFREADY21 || state == S_MSTAFFREADY22 || state == S_MSTAFFREADY23 || state == S_MSTAFFREADY24 || state == S_MSTAFFREADY25 || state == S_MSTAFFREADY26 || state == S_MSTAFFREADY27 || state == S_MSTAFFREADY28 || state == S_MSTAFFREADY29 || state == S_MSTAFFREADY30 || state == S_MSTAFFREADY31 || state == S_MSTAFFREADY32 || state == S_MSTAFFREADY33 || state == S_MSTAFFREADY34 || state == S_MSTAFFREADY35 || state == S_MSTAFFDOWN || state == S_MSTAFFUP || state == S_MSTAFFATK_1 || state == S_MSTAFFATK_4 || state == S_MSTAFFATK_5 || state == S_MSTAFFATK_6 || state == S_MSTAFFATK_7)
                spritelights = fullbright_yellowred[lightnum];
        }
        // [JN] Fallback. If we are not using brightmaps, apply full brightness
        // to the objects, that no longer lighten up in info.c.
        if (!brightmaps || vanillaparm)
        {
            if (state == S_FSWORDREADY || state == S_FSWORDREADY1 || state == S_FSWORDREADY2 || state == S_FSWORDREADY3 || state == S_FSWORDREADY4 || state == S_FSWORDREADY5 || state == S_FSWORDREADY6 || state == S_FSWORDREADY7 || state == S_FSWORDREADY8 || state == S_FSWORDREADY9 || state == S_FSWORDREADY10 || state == S_FSWORDREADY11 || state == S_FSWORDDOWN || state == S_FSWORDUP || state == S_FSWORDATK_1 || state == S_FSWORDATK_2 || state == S_FSWORDATK_3 || state == S_FSWORDATK_4 || state == S_FSWORDATK_5 || state == S_FSWORDATK_6 || state == S_FSWORDATK_7 || state == S_FSWORDATK_8 || state == S_FSWORDATK_9 || state == S_FSWORDATK_10 || state == S_FSWORDATK_11 || state == S_FSWORDATK_12 || state == S_MLIGHTNINGREADY || state == S_MLIGHTNINGREADY2 || state == S_MLIGHTNINGREADY3 || state == S_MLIGHTNINGREADY4 || state == S_MLIGHTNINGREADY5 || state == S_MLIGHTNINGREADY6 || state == S_MLIGHTNINGREADY7 || state == S_MLIGHTNINGREADY8 || state == S_MLIGHTNINGREADY9 || state == S_MLIGHTNINGREADY10 || state == S_MLIGHTNINGREADY11 || state == S_MLIGHTNINGREADY12 || state == S_MLIGHTNINGREADY13 || state == S_MLIGHTNINGREADY14 || state == S_MLIGHTNINGREADY15 || state == S_MLIGHTNINGREADY16 || state == S_MLIGHTNINGREADY17 || state == S_MLIGHTNINGREADY18 || state == S_MLIGHTNINGREADY19 || state == S_MLIGHTNINGREADY20 || state == S_MLIGHTNINGREADY21 || state == S_MLIGHTNINGREADY22 || state == S_MLIGHTNINGREADY23 || state == S_MLIGHTNINGREADY24 || state == S_MLIGHTNINGDOWN || state == S_MLIGHTNINGUP || state == S_MLIGHTNINGATK_1 || state == S_MLIGHTNINGATK_2 || state == S_MLIGHTNINGATK_3 || state == S_MLIGHTNINGATK_4 || state == S_MLIGHTNINGATK_5 || state == S_MLIGHTNINGATK_6 || state == S_MLIGHTNINGATK_7 || state == S_MLIGHTNINGATK_8 || state == S_MLIGHTNINGATK_9 || state == S_MLIGHTNINGATK_10 || state == S_MLIGHTNINGATK_11)
                spritelights = scalelight[LIGHTLEVELS - 1];
        }
    }
//
// clip to screen bounds
//
    mfloorclip = screenheightarray;
    mceilingclip = negonearray;

//
// add all active psprites
//
    for (i = 0, psp = viewplayer->psprites; i < NUMPSPRITES; i++, psp++)
        if (psp->state)
            R_DrawPSprite(psp);

}


/*
========================
=
= R_SortVisSprites
=
========================
*/

vissprite_t vsprsortedhead;

void R_SortVisSprites(void)
{
    int i, count;
    vissprite_t *ds, *best;
    vissprite_t unsorted;
    fixed_t bestscale;

    count = vissprite_p - vissprites;

    unsorted.next = unsorted.prev = &unsorted;
    if (!count)
        return;

    for (ds = vissprites; ds < vissprite_p; ds++)
    {
        ds->next = ds + 1;
        ds->prev = ds - 1;
    }
    vissprites[0].prev = &unsorted;
    unsorted.next = &vissprites[0];
    (vissprite_p - 1)->next = &unsorted;
    unsorted.prev = vissprite_p - 1;

//
// pull the vissprites out by scale
//
    best = 0;                   // shut up the compiler warning
    vsprsortedhead.next = vsprsortedhead.prev = &vsprsortedhead;
    for (i = 0; i < count; i++)
    {
        bestscale = INT_MAX;
        for (ds = unsorted.next; ds != &unsorted; ds = ds->next)
        {
            if (ds->scale < bestscale)
            {
                bestscale = ds->scale;
                best = ds;
            }
        }
        best->next->prev = best->prev;
        best->prev->next = best->next;
        best->next = &vsprsortedhead;
        best->prev = vsprsortedhead.prev;
        vsprsortedhead.prev->next = best;
        vsprsortedhead.prev = best;
    }
}



/*
========================
=
= R_DrawSprite
=
========================
*/

void R_DrawSprite(vissprite_t * spr)
{
    drawseg_t *ds;
    int clipbot[WIDESCREENWIDTH], cliptop[WIDESCREENWIDTH]; // [crispy] 32-bit integer math
    int x, r1, r2;
    fixed_t scale, lowscale;
    int silhouette;

    for (x = spr->x1; x <= spr->x2; x++)
        clipbot[x] = cliptop[x] = -2;

//
// scan drawsegs from end to start for obscuring segs
// the first drawseg that has a greater scale is the clip seg
//
    for (ds = ds_p - 1; ds >= drawsegs; ds--)
    {
        //
        // determine if the drawseg obscures the sprite
        //
        if (ds->x1 > spr->x2 || ds->x2 < spr->x1 ||
            (!ds->silhouette && !ds->maskedtexturecol))
            continue;           // doesn't cover sprite

        r1 = ds->x1 < spr->x1 ? spr->x1 : ds->x1;
        r2 = ds->x2 > spr->x2 ? spr->x2 : ds->x2;
        if (ds->scale1 > ds->scale2)
        {
            lowscale = ds->scale2;
            scale = ds->scale1;
        }
        else
        {
            lowscale = ds->scale1;
            scale = ds->scale2;
        }

        if (scale < spr->scale || (lowscale < spr->scale
                                   && !R_PointOnSegSide(spr->gx, spr->gy,
                                                        ds->curline)))
        {
            if (ds->maskedtexturecol)   // masked mid texture
                R_RenderMaskedSegRange(ds, r1, r2);
            continue;           // seg is behind sprite
        }

//
// clip this piece of the sprite
//
        silhouette = ds->silhouette;
        if (spr->gz >= ds->bsilheight)
            silhouette &= ~SIL_BOTTOM;
        if (spr->gzt <= ds->tsilheight)
            silhouette &= ~SIL_TOP;

        if (silhouette == 1)
        {                       // bottom sil
            for (x = r1; x <= r2; x++)
                if (clipbot[x] == -2)
                    clipbot[x] = ds->sprbottomclip[x];
        }
        else if (silhouette == 2)
        {                       // top sil
            for (x = r1; x <= r2; x++)
                if (cliptop[x] == -2)
                    cliptop[x] = ds->sprtopclip[x];
        }
        else if (silhouette == 3)
        {                       // both
            for (x = r1; x <= r2; x++)
            {
                if (clipbot[x] == -2)
                    clipbot[x] = ds->sprbottomclip[x];
                if (cliptop[x] == -2)
                    cliptop[x] = ds->sprtopclip[x];
            }
        }

    }

//
// all clipping has been performed, so draw the sprite
//

// check for unclipped columns
    for (x = spr->x1; x <= spr->x2; x++)
    {
        if (clipbot[x] == -2)
            clipbot[x] = viewheight;
        if (cliptop[x] == -2)
            cliptop[x] = -1;
    }

    mfloorclip = clipbot;
    mceilingclip = cliptop;
    R_DrawVisSprite(spr, spr->x1, spr->x2);
}


/*
========================
=
= R_DrawMasked
=
========================
*/

void R_DrawMasked(void)
{
    vissprite_t *spr;
    drawseg_t *ds;

    R_SortVisSprites();

    if (vissprite_p > vissprites)
    {
        // draw all vissprites back to front

        for (spr = vsprsortedhead.next; spr != &vsprsortedhead;
             spr = spr->next)
            R_DrawSprite(spr);
    }

//
// render any remaining masked mid textures
//
    for (ds = ds_p - 1; ds >= drawsegs; ds--)
        if (ds->maskedtexturecol)
            R_RenderMaskedSegRange(ds, ds->x1, ds->x2);

//
// draw the psprites on top of everything
//
// Added for the sideviewing with an external device
    if (viewangleoffset <= 1024 << ANGLETOFINESHIFT || viewangleoffset >=
        -1024 << ANGLETOFINESHIFT)
    {                           // don't draw on side views
        R_DrawPlayerSprites();
    }

//      if (!viewangleoffset)           // don't draw on side views
//              R_DrawPlayerSprites ();
}
