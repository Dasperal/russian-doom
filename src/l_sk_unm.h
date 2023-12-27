//
// Copyright(C) 2023-2025 Leonid Murin (Dasperal)
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

#define UNM_DECLARE_ATTR_VARIABLE(class_name, attr, change_expr) \
static int class_name ## _ ## attr;

#define UNM_DECLARE_PROP_VARIABLE(var_name, property, change_expr) \
static int var_name;

#define UNM_APPLY_ATTR_CHANGE(class_name, attr, change_expr) \
mobjinfo[class_name].attr = class_name ## _ ## attr change_expr;

#define UNM_RESTORE_ATTR(class_name, attr, change_expr) \
mobjinfo[class_name].attr = class_name ## _ ## attr;

#define UNM_APPLY_PROP_CHANGE(var_name, property, change_expr) \
property = var_name change_expr;

#define UNM_RESTORE_PROP(var_name, property, change_expr) \
property = var_name;

#define UNM_IMPLEMENT_APPLY_RESTORE_ATTRS(O, P) \
void UNM_Apply_Restore_Atters(skill_t skill)    \
{                                               \
    if(skill == sk_ultranm)                     \
    {                                           \
        O(UNM_APPLY_ATTR_CHANGE)                \
        P(UNM_APPLY_PROP_CHANGE)                \
    }                                           \
    else                                        \
    {                                           \
        O(UNM_RESTORE_ATTR)                     \
        P(UNM_RESTORE_PROP)                     \
    }                                           \
}

#define UNM_SAVE_ATTR(class_name, attr, change_expr) \
class_name ## _ ## attr = mobjinfo[class_name].attr;

#define UNM_SAVE_PROPERTY(var_name, property, change_expr) \
var_name = property;

#define UNM_IMPLEMENT_SAVE_ATTRS(O, P) \
void UNM_Save_Atters(void)             \
{                                      \
    O(UNM_SAVE_ATTR)                   \
    P(UNM_SAVE_PROPERTY)               \
}

#define UNM_IMPLEMENT(O, P)             \
O(UNM_DECLARE_ATTR_VARIABLE)            \
P(UNM_DECLARE_PROP_VARIABLE)            \
UNM_IMPLEMENT_APPLY_RESTORE_ATTRS(O, P) \
UNM_IMPLEMENT_SAVE_ATTRS(O, P)
