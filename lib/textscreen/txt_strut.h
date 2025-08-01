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


#pragma once


/**
 * @file txt_strut.h
 *
 * Strut widget.
 */

/**
 * Strut widget.
 *
 * A strut is a widget that takes up a fixed amount of space.  It can
 * be visualised as a transparent box.  Struts are used to provide
 * spacing between widgets.
 */

typedef struct txt_strut_s txt_strut_t;

#include "txt_widget.h"

struct txt_strut_s
{
    txt_widget_t widget;
    int width;
    int height;
};

/**
 * Create a new strut.
 *
 * @param width      Width of the strut, in characters.
 * @param height     Height of the strut, in characters.
 */

txt_strut_t *TXT_NewStrut(int width, int height);
