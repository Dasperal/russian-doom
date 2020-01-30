//
// Copyright(C) 2005-2014 Simon Howard
// Copyright(C) 2016-2019 Julian Nechaevsky
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



#include <stdlib.h>

#include "textscreen.h"
#include "doomtype.h"
#include "m_config.h"
#include "m_controls.h"

#include "execute.h"
#include "txt_mouseinput.h"

#include "mode.h"
#include "mouse.h"

#define WINDOW_HELP_URL "https://jnechaevsky.github.io/projects/rusdoom/setup/mouse.html"

static int usemouse = 1;

static int mouseSensitivity = 5;
static float mouse_acceleration = 2.0;
static int mouse_threshold = 10;
static int mlook = 0;
static int grabmouse = 1;

// [JN] ������������ ����������� ��������� �� ���������.
int novert = 1;

static int *all_mouse_buttons[] = {
    &mousebfire,
    &mousebstrafe,
    &mousebforward,
    &mousebstrafeleft,
    &mousebstraferight,
    &mousebbackward,
    &mousebuse,
    &mousebjump,
    &mousebprevweapon,
    &mousebnextweapon
};

static void MouseSetCallback(TXT_UNCAST_ARG(widget), TXT_UNCAST_ARG(variable))
{
    TXT_CAST_ARG(int, variable);
    unsigned int i;

    // Check if the same mouse button is used for a different action
    // If so, set the other action(s) to -1 (unset)

    for (i=0; i<arrlen(all_mouse_buttons); ++i)
    {
        if (*all_mouse_buttons[i] == *variable
         && all_mouse_buttons[i] != variable)
        {
            *all_mouse_buttons[i] = -1;
        }
    }
}

static void AddMouseControl(TXT_UNCAST_ARG(table), char *label, int *var)
{
    TXT_CAST_ARG(txt_table_t, table);
    txt_mouse_input_t *mouse_input;

    TXT_AddWidget(table, TXT_NewLabel(label));

    mouse_input = TXT_NewMouseInput(var);
    TXT_AddWidget(table, mouse_input);

    TXT_SignalConnect(mouse_input, "set", MouseSetCallback, var);
}

static void ConfigExtraButtons(TXT_UNCAST_ARG(widget), TXT_UNCAST_ARG(unused))
{
    txt_window_t *window;
    txt_table_t *buttons_table;
    
    window = TXT_NewWindow(english_language ?
                           "Additional mouse buttons" :
                           "�������������� ������ ����");

    if (english_language)
    TXT_SetWindowHelpURL(window, WINDOW_HELP_URL);
    else
    TXT_SetWindowHelpURL_RUS(window, WINDOW_HELP_URL);

    //
    // [JN] Create translated buttons
    //

    TXT_SetWindowAction(window, TXT_HORIZ_LEFT, english_language ?
                        TXT_NewWindowAbortAction(window) :
                        TXT_NewWindowAbortAction_Rus(window));
    TXT_SetWindowAction(window, TXT_HORIZ_RIGHT, english_language ?
                        TXT_NewWindowSelectAction(window) :
                        TXT_NewWindowSelectAction_Rus(window));

    TXT_AddWidgets(window,
                buttons_table = TXT_NewTable(2),
                NULL);

    TXT_SetColumnWidths(buttons_table, 24, 5);

    AddMouseControl(buttons_table, english_language ?
                                   "Move backward" :
                                   "�������� �����",
                                   &mousebbackward);
    AddMouseControl(buttons_table, english_language ?
                                   "Use" :
                                   "������������",
                                   &mousebuse);
    AddMouseControl(buttons_table, english_language ?
                                   "Strafe left" :
                                   "����� �����",
                                   &mousebstrafeleft);
    AddMouseControl(buttons_table, english_language ?
                                   "Strafe right" :
                                   "����� ������",
                                   &mousebstraferight);

    if (gamemission == hexen || gamemission == strife)
    {
        AddMouseControl(buttons_table, english_language ?
                                       "Jump" :
                                       "������",
                                       &mousebjump);
    }

    AddMouseControl(buttons_table, english_language ?
                                   "Previous weapon" :
                                   "���������� ������",
                                   &mousebprevweapon);
    AddMouseControl(buttons_table, english_language ?
                                   "Next weapon" :
                                   "��������� ������",
                                   &mousebnextweapon);
}

void ConfigMouse(void)
{
    txt_window_t *window;

    window = TXT_NewWindow(english_language ?
                           "Mouse configuration" :
                           "��������� ����");

    TXT_SetTableColumns(window, 2);

    //
    // [JN] Create translated buttons
    //

    TXT_SetWindowAction(window, TXT_HORIZ_LEFT, english_language ?
                        TXT_NewWindowAbortAction(window) :
                        TXT_NewWindowAbortAction_Rus(window));
    TXT_SetWindowAction(window, TXT_HORIZ_CENTER, TestConfigAction());
    TXT_SetWindowAction(window, TXT_HORIZ_RIGHT, english_language ?
                        TXT_NewWindowSelectAction(window) :
                        TXT_NewWindowSelectAction_Rus(window));

    if (english_language)
    TXT_SetWindowHelpURL(window, WINDOW_HELP_URL);
    else
    TXT_SetWindowHelpURL_RUS(window, WINDOW_HELP_URL);

    TXT_AddWidgets(window,
        TXT_NewCheckBox(english_language ?
                        "Enable mouse" :
                        "��������� ������������� ����",
                        &usemouse),
        TXT_TABLE_OVERFLOW_RIGHT,
            TXT_If(gamemission == doom, 
            TXT_NewInvertedCheckBox(english_language ?
                                    "Allow vertical mouse movement" :
                                    "������������ �����������",
                                    &novert)),
        TXT_TABLE_OVERFLOW_RIGHT,
            TXT_NewCheckBox(english_language ?
                            "Allow mouse look" :
                            "��������� ����� �����",
                            &mlook),
        TXT_TABLE_OVERFLOW_RIGHT,
        TXT_NewCheckBox(english_language ?
                        "Grab mouse in windowed mode" :
                        "������ ���� � ������� ������",
                        &grabmouse),
        TXT_TABLE_OVERFLOW_RIGHT,
        TXT_NewCheckBox(english_language ?
                        "Double click acts as \"use\"" :
                        "������� ���� ���������� \"�������������\"",
                        &dclick_use),
        TXT_TABLE_OVERFLOW_RIGHT,

        TXT_NewSeparator(english_language ?
                         "Mouse motion" :
                         "��������� �����������"),
        TXT_NewLabel(english_language ?
                     "Speed" :
                     "��������"),
        TXT_NewSpinControl(&mouseSensitivity, 1, 256),
        TXT_NewLabel(english_language ?
                     "Acceleration" :
                     "�����������"),
        TXT_NewFloatSpinControl(&mouse_acceleration, 1.0, 5.0),
        TXT_NewLabel(english_language ?
                     "Acceleration threshold" :
                     "����� �����������"),
        TXT_NewSpinControl(&mouse_threshold, 0, 32),

        TXT_NewSeparator(english_language ?
                         "Buttons" :
                         "������"),
                         NULL);

    AddMouseControl(window, english_language ?
                            "Fire/Attack" :
                            "�����/��������",
                            &mousebfire);
    AddMouseControl(window, english_language ?
                            "Move forward" :
                            "�������� ������",
                            &mousebforward);
    AddMouseControl(window, english_language ?
                            "Strafe on" :
                            "�������� �����",
                            &mousebstrafe);

    TXT_AddWidget(window,
                TXT_NewButton2(english_language ?
                "More controls..." :
                "�������������...",
                ConfigExtraButtons, NULL));
}

void BindMouseVariables(void)
{
    M_BindIntVariable("use_mouse",               &usemouse);
    M_BindIntVariable("mlook",                   &mlook);
    M_BindIntVariable("mouse_sensitivity",       &mouseSensitivity);
    M_BindIntVariable("novert",                  &novert);
    M_BindIntVariable("grabmouse",               &grabmouse);
    M_BindIntVariable("mouse_threshold",         &mouse_threshold);
    M_BindFloatVariable("mouse_acceleration",    &mouse_acceleration);
}

