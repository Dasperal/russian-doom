//
// Copyright(C) 1993-1996 Id Software, Inc.
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

#include <stdint.h>

typedef enum
{
    direct_x11,
    direct_x12,
    direct_x,
    metal,
    opengl,
    opengles2,
    vulkan,
    gpu,
    software
} render_drivers_t;

typedef struct
{
    char* display_name;
} render_driver_option_t;

static const render_driver_option_t render_driver_options[] = {
    {
        display_name: "Direct X 11"
    },
    {
        display_name: "Direct X 12"
    },
    {
        display_name: "Direct X"
    },
    {
        display_name: "Metal"
    },
    {
        display_name: "OpenGL"
    },
    {
        display_name: "OpenGLES 2"
    },
    {
        display_name: "Vulkan"
    },
    {
        display_name: "GPU"
    },
    {
        display_name: "Software"
    }
};

extern const char** available_render_drivers;
extern int8_t num_of_available_render_drivers;
extern char* render_driver_option;
extern int32_t render_driver_cursor;
extern int8_t render_driver_index;

void init_available_render_drivers();
int8_t get_render_driver_index(const char* driver_name);