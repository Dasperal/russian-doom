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

#include "m_config.h"
#include "textscreen.h"
#include "mode.h"

#include "compatibility.h"

#define WINDOW_HELP_URL "http://jnechaevsky.users.sourceforge.net/projects/rusdoom/setup/gameplay.html"


// Automap
int automap_color = 0;
int automap_follow = 1;
int automap_overlay = 0;
int automap_rotate = 0;
int automap_grid = 0;

// Gameplay: Graphical
int brightmaps = 1;
int fake_contrast = 0;
int translucency = 1;
int colored_hud = 0;
int messages_color = 0;
int colored_blood = 1;
int swirling_liquids = 1;
int invul_sky = 1;
int red_resurrection_flash = 1;
int draw_shadowed_text = 1;

// Gameplay: Audible
int play_exit_sfx = 1;
int crushed_corpses_sfx = 1;
int blazing_door_fix_sfx = 1;
int noise_alert_sfx = 0;
int correct_endlevel_sfx = 0;

// Gameplay: Tactical
int automap_stats = 1;
int secret_notification = 1;
int negative_health = 0;
int infragreen_visor = 0;

// Gameplay: Physical
int over_under = 0;
int torque = 1;
int weapon_bobbing = 1;
int ssg_blast_enemies = 1;
int randomly_flipcorpses = 1;
int floating_powerups = 0;

// Gameplay: Crosshair
int crosshair_draw = 0;
int crosshair_health = 1;
int crosshair_scale = 0;

// Gameplay: Gameplay
int fix_map_errors = 1;
int extra_player_faces = 1;
int unlimited_lost_souls = 1;
int agressive_lost_souls = 0;
int fast_quickload = 1;
int no_internal_demos = 0;
int flip_levels = 0;

// int show_exit_sequence = 1;  // [Strife]


// [JN] No longer used, now using in-game menu
/*
void CompatibilitySettings(void)
{
    txt_window_t *window;
    txt_table_t  *window_features;

    window = TXT_NewWindow(english_language ?
                           "Optional Gameplay Enhacements" :
                           "�������������� ��������� ����");
    window_features = TXT_NewTable(1);

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

    if (gamemission == doom)
        TXT_AddWidget(window, TXT_NewScrollPane(47, 15, window_features));
    else if (gamemission == heretic)
        TXT_AddWidget(window, TXT_NewScrollPane(47, 15, window_features));
    else if (gamemission == hexen)
        TXT_AddWidget(window, TXT_NewScrollPane(47, 10, window_features));

    TXT_AddWidgets(window_features,

    TXT_NewSeparator(english_language ?
        "Graphical" :
        "�������"),
        TXT_NewCheckBox(english_language ?
            "Brightmaps for textures and sprites" :
            "������������ ������� � ��������",
            &brightmaps),
        TXT_NewCheckBox(english_language ?
            "Apply fake contrast on walls" :
            "�������� ������������ ��������� ����",
            &fake_contrast),
        TXT_If(gamemission == doom, TXT_NewCheckBox(english_language ?
            "Translucency" :
            "������ ������������ � ��������� ��������",
            &translucency)),
        TXT_If(gamemission == doom, TXT_NewCheckBox(english_language ?
            "Swirling liquids" :
            "���������� �������� ���������",
            &swirling_liquids)),
        TXT_If(gamemission == doom || gamemission == heretic, TXT_NewCheckBox(english_language ?
            "Invulberability affects sky" :
            "������������ ���������� ����",
            &invul_sky)),    
        TXT_If(gamemission == doom || gamemission == heretic, TXT_NewCheckBox(english_language ?
            "Colored HUD elements" :
            "������������ �������� HUD",
            &colored_hud)),
        TXT_If(gamemission == doom || gamemission == heretic, TXT_NewCheckBox(english_language ?
            "Colored blood and corpses" :
            "������������ ����� � �����",
            &colored_blood)),
        TXT_If(gamemission == doom, TXT_NewCheckBox(english_language ?
            "Red resurrection flash" :
            "������� ������� ����������� ��������",
            &red_resurrection_flash)),
        TXT_NewCheckBox(english_language ?
            "Texts are dropping shadow" :
            "�������� ���� � ������ ����������� ����",
            &draw_shadowed_text),
        TXT_If(gamemission == doom, TXT_NewCheckBox(english_language ?
            "Show disk icon" :
            "���������� ������ �������",
            &show_diskicon)),

    TXT_If(gamemission == doom, TXT_NewSeparator(english_language ?
        "Audible" :
        "����")), 
        TXT_If(gamemission == doom, TXT_NewCheckBox(english_language ?
            "Sound of crushing corpses" :
            "���� ������������� ������",
            &crushed_corpses_sfx)),
        TXT_If(gamemission == doom, TXT_NewCheckBox(english_language ?
            "Single sound of closing blazing door" :
            "��������� ���� �������� ������� �����",
            &blazing_door_fix_sfx)),
        TXT_If(gamemission == doom, TXT_NewCheckBox(english_language ?
            "Play exit sounds" :
            "����������� ���� ��� ������ �� ����",
            &play_exit_sfx)),
        TXT_If(gamemission == doom, TXT_NewCheckBox(english_language ?
            "Correct sound of level ending" :
            "���������� ���� ���������� ������",
            &correct_endlevel_sfx)),
        TXT_If(gamemission == doom, TXT_NewCheckBox(english_language ?
            "Monsters alert sound waking up others" :
            "������� ������� ���������� ������ ��������",
            &noise_alert_sfx)),

    TXT_If(gamemission == doom || gamemission == heretic, TXT_NewSeparator(english_language ?
    "Tactical" :
    "�������")),
        TXT_If(gamemission == doom || gamemission == heretic, TXT_NewCheckBox(english_language ?
            "Show level stats on automap" :
            "���������� ���������� ������ �� �����",
            &automap_stats)),
        TXT_If(gamemission == doom || gamemission == heretic, TXT_NewCheckBox(english_language ?
            "Notification of revealed secters" :
            "����������� �� ����������� ��������",
            &secret_notification)),
        TXT_If(gamemission == doom || gamemission == heretic, TXT_NewCheckBox(english_language ?
            "Show negative health" :
            "���������� ������������� ��������",
            &negative_health)),
        TXT_If(gamemission == doom, TXT_NewCheckBox(english_language ?
            "Infragreen light amplification visor" :
            "������������ ����� �������� ���������",
            &infragreen_visor)),

    TXT_If(gamemission == doom || gamemission == heretic, TXT_NewSeparator(english_language ?
    "Physical" :
    "������")),
        TXT_If(gamemission == doom, TXT_NewCheckBox(english_language ?
            "Walk over and under monsters" :
            "����� ����� ��������� ��� � ��� ���������",
            &over_under)),
        TXT_If(gamemission == doom || gamemission == heretic, TXT_NewCheckBox(english_language ?
            "Corpses sliding from the ledges" :
            "����� ������������� � �������� � �������",
            &torque)),
        TXT_If(gamemission == doom || gamemission == heretic, TXT_NewCheckBox(english_language ?
            "Weapon bobbing while firing" :
            "����������� ������ ��� �������� � ��������",
            &weapon_bobbing)),
        TXT_If(gamemission == doom, TXT_NewCheckBox(english_language ?
            "Lethal pellet of a point-blank SSG" :
            "������������ ����� ����� ��������� ������",
            &ssg_blast_enemies)),
        TXT_If(gamemission == doom || gamemission == heretic, TXT_NewCheckBox(english_language ?
            "Randomly mirrored corpses" :
            "������������ ���������� ��������� ������",
            &randomly_flipcorpses)),
        TXT_If(gamemission == doom,	TXT_NewCheckBox(english_language ?
            "Floating powerups" :
            "������������ �����-���������",
            &floating_powerups)),

    TXT_NewSeparator(english_language ?
    "Gameplay" :
    "��������"),
        TXT_If(gamemission == doom, TXT_NewCheckBox(english_language ?
            "Fix errors of vanilla maps" :
            "���������� ������ ������������ �������",
            &fix_map_errors)),
        TXT_If(gamemission == doom || gamemission == heretic, TXT_NewCheckBox(english_language ?
            "Flip game levels" :
            "���������� ��������� �������",
            &flip_levels)),
        TXT_If(gamemission == doom,	TXT_NewCheckBox(english_language ?
            "Extra player faces on the HUD" :
            "�������������� ���� ������ � HUD",
            &extra_player_faces)),
        TXT_If(gamemission == doom,	TXT_NewCheckBox(english_language ?
            "Pain Elemental without Souls limit" :
            "���������� ���� ��� ����������� ���",
            &unlimited_lost_souls)),
        TXT_If(gamemission == doom,	TXT_NewCheckBox(english_language ?
            "More agressive Lost Souls" :
            "���������� ������������� ���������� ���",
            &agressive_lost_souls)),
        TXT_If(gamemission == doom,	TXT_NewCheckBox(english_language ?
            "Don't prompt for quick loading/saving" :
            "�� �������� ������ ��� ������� ��������",
            &fast_quickload)),
        TXT_NewCheckBox(english_language ?
            "Don't play internal demos" :
            "�� ����������� ���������� ����������",
            &no_internal_demos),

    TXT_NewSeparator(english_language ?
    "Crosshair" :
    "������"),
        TXT_NewCheckBox(english_language ?
            "Draw crosshair" :
            "���������� ������",
            &crosshair_draw),
        TXT_NewCheckBox(english_language ?
            "Health indication" :
            "��������� ��������",
            &crosshair_health),
        TXT_NewCheckBox(english_language ?
            "Increased size" :
            "����������� ������",
            &crosshair_scale),

        // TXT_If(gamemission == strife, TXT_NewCheckBox("���������� ������ �������� �����",        &show_diskicon)),
        // TXT_If(gamemission == strife, TXT_NewCheckBox("���������� �������� ��� ������",          &show_exit_sequence)),

    NULL);
}
*/

void BindCompatibilityVariables(void)
{
    // Automap
    M_BindIntVariable("automap_color",          &automap_color);
    M_BindIntVariable("automap_follow",         &automap_follow);
    M_BindIntVariable("automap_overlay",        &automap_overlay);
    M_BindIntVariable("automap_rotate",         &automap_rotate);
    M_BindIntVariable("automap_grid",           &automap_grid);

    // Gameplay: Graphical
    M_BindIntVariable("brightmaps",             &brightmaps);
    M_BindIntVariable("fake_contrast",          &fake_contrast);
    M_BindIntVariable("translucency",           &translucency);
    M_BindIntVariable("colored_hud",            &colored_hud);
    M_BindIntVariable("messages_color",         &messages_color);
    M_BindIntVariable("colored_blood",          &colored_blood);
    M_BindIntVariable("swirling_liquids",       &swirling_liquids);
    M_BindIntVariable("invul_sky",              &invul_sky);
    M_BindIntVariable("red_resurrection_flash", &red_resurrection_flash);
    M_BindIntVariable("draw_shadowed_text",     &draw_shadowed_text);

    // Gameplay: Audible
    M_BindIntVariable("play_exit_sfx",          &play_exit_sfx);
    M_BindIntVariable("crushed_corpses_sfx",    &crushed_corpses_sfx);
    M_BindIntVariable("blazing_door_fix_sfx",   &blazing_door_fix_sfx);
    M_BindIntVariable("noise_alert_sfx",        &noise_alert_sfx);
    M_BindIntVariable("correct_endlevel_sfx",   &correct_endlevel_sfx);

    // Gameplay: Tactical
    M_BindIntVariable("automap_stats",          &automap_stats);
    M_BindIntVariable("secret_notification",    &secret_notification);
    M_BindIntVariable("negative_health",        &negative_health);
    M_BindIntVariable("infragreen_visor",       &infragreen_visor);

    // Gameplay: Physical
    M_BindIntVariable("over_under",             &over_under);
    M_BindIntVariable("torque",                 &torque);
    M_BindIntVariable("weapon_bobbing",         &weapon_bobbing);
    M_BindIntVariable("ssg_blast_enemies",      &ssg_blast_enemies);
    M_BindIntVariable("randomly_flipcorpses",   &randomly_flipcorpses);
    M_BindIntVariable("floating_powerups",      &floating_powerups);

    // Gameplay: Crosshair
    M_BindIntVariable("crosshair_draw",         &crosshair_draw);
    M_BindIntVariable("crosshair_health",       &crosshair_health);
    M_BindIntVariable("crosshair_scale",        &crosshair_scale);

    // Gameplay: Gameplay
    M_BindIntVariable("fix_map_errors",         &fix_map_errors);
    M_BindIntVariable("extra_player_faces",     &extra_player_faces);
    M_BindIntVariable("unlimited_lost_souls",   &unlimited_lost_souls);
    M_BindIntVariable("agressive_lost_souls",   &agressive_lost_souls);
    M_BindIntVariable("fast_quickload",         &fast_quickload);
    M_BindIntVariable("no_internal_demos",      &no_internal_demos);
    M_BindIntVariable("flip_levels",            &flip_levels);

    // M_BindIntVariable("show_exit_sequence",  &show_exit_sequence);    // [Strife]
}
