//
// Copyright(C) 2005-2014 Simon Howard
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

// Russian Doom (C) 2016-2018 Julian Nechaevsky


// Sound control menu

#include <stdlib.h>

#include "m_config.h"
#include "textscreen.h"
#include "mode.h"

#include "compatibility.h"

#define WINDOW_HELP_URL "http://jnechaevsky.users.sourceforge.net/projects/rusdoom/setup/gameplay.html"


// [JN] �������������� ��������� ����

// �������
int uncapped_fps = 0;
int brightmaps = 1;
int fake_contrast = 0;
int translucency = 1;
int swirling_liquids = 1;
int invul_sky = 1;
int colored_blood = 1;
int red_resurrection_flash = 1;
int draw_shadowed_text = 1;
int show_diskicon = 1;

// ����
int crushed_corpses_sfx = 1;
int blazing_door_fix_sfx = 1;
int correct_endlevel_sfx = 0;
int play_exit_sfx = 1;

// �������
int automap_stats = 1;
int secret_notification = 1;
int negative_health = 0;
int infragreen_visor = 0;

// ������
int over_under = 0;
int torque = 1;
int weapon_bobbing = 1;
int ssg_blast_enemies = 1;
int randomly_flipcorpses = 1;
int floating_powerups = 0;

// ��������
int fix_map_errors = 1;
int flip_levels = 0;
int new_ouch_face = 1;
int unlimited_lost_souls = 1;
int agressive_lost_souls = 0;
int fast_quickload = 1;

// ������
int crosshair_draw = 0;
int crosshair_health = 0;
int crosshair_scale = 0;

// int show_exit_sequence = 1;  // [Strife]

void CompatibilitySettings(void)
{
    txt_window_t *window;
    txt_table_t  *window_features;

    window = TXT_NewWindow("�������������� ��������� ����");
    window_features = TXT_NewTable(1);

    TXT_SetWindowHelpURL(window, WINDOW_HELP_URL);

    if (gamemission == doom)
        TXT_AddWidget(window, TXT_NewScrollPane(47, 15, window_features));
    else if (gamemission == heretic)
        TXT_AddWidget(window, TXT_NewScrollPane(47, 15, window_features));
    else if (gamemission == hexen)
        TXT_AddWidget(window, TXT_NewScrollPane(47, 8, window_features));

    TXT_AddWidgets(window_features,

    TXT_NewSeparator("�������"),
        TXT_If(gamemission == doom, TXT_NewCheckBox("����� ����������� � 35 fps",                               &uncapped_fps)),
        TXT_NewCheckBox("������������ ������� � ��������",                                                      &brightmaps),
        TXT_NewCheckBox("�������� ������������ ��������� ����",                                                 &fake_contrast),
        TXT_If(gamemission == doom, TXT_NewCheckBox("������ ������������ � ��������� ��������",                 &translucency)),
        TXT_If(gamemission == doom, TXT_NewCheckBox("���������� �������� ���������",                            &swirling_liquids)),
        TXT_If(gamemission == doom || gamemission == heretic, TXT_NewCheckBox("������������ ���������� ����",   &invul_sky)),    
        TXT_If(gamemission == doom, TXT_NewCheckBox("������������ ����� � �����",                               &colored_blood)),
        TXT_If(gamemission == doom, TXT_NewCheckBox("������� ������� ����������� ��������",                     &red_resurrection_flash)),
        TXT_NewCheckBox("�������� ���� � ������ ����������� ����",                                              &draw_shadowed_text),
        TXT_If(gamemission == doom, TXT_NewCheckBox("���������� ������ �������",                                &show_diskicon)),

    TXT_If(gamemission == doom, TXT_NewSeparator("����")), 
        TXT_If(gamemission == doom, TXT_NewCheckBox("���� ������������� ������",                &crushed_corpses_sfx)),
        TXT_If(gamemission == doom, TXT_NewCheckBox("��������� ���� �������� ������� �����",    &blazing_door_fix_sfx)),
        TXT_If(gamemission == doom, TXT_NewCheckBox("����������� ���� ��� ������ �� ����",      &play_exit_sfx)),
        TXT_If(gamemission == doom, TXT_NewCheckBox("���������� ���� ���������� ������",        &correct_endlevel_sfx)),

    TXT_If(gamemission == doom || gamemission == heretic, TXT_NewSeparator("�������")),
        TXT_If(gamemission == doom || gamemission == heretic, TXT_NewCheckBox("���������� ���������� ������ �� �����",  &automap_stats)),
        TXT_If(gamemission == doom || gamemission == heretic, TXT_NewCheckBox("����������� �� ����������� ��������",    &secret_notification)),
        TXT_If(gamemission == doom, TXT_NewCheckBox("���������� ������������� ��������",                                &negative_health)),
        TXT_If(gamemission == doom, TXT_NewCheckBox("������������ ����� �������� ���������",                            &infragreen_visor)),

    TXT_If(gamemission == doom || gamemission == heretic, TXT_NewSeparator("������")),
        TXT_If(gamemission == doom, TXT_NewCheckBox("����� ����� ��������� ��� � ��� ���������",                            &over_under)),
        TXT_If(gamemission == doom || gamemission == heretic, TXT_NewCheckBox("����� ������������� � �������� � �������",   &torque)),
        TXT_If(gamemission == doom || gamemission == heretic, TXT_NewCheckBox("����������� ������ ��� �������� � ��������", &weapon_bobbing)),
        TXT_If(gamemission == doom, TXT_NewCheckBox("������������ ����� ����� ��������� ������",                            &ssg_blast_enemies)),
        TXT_If(gamemission == doom || gamemission == heretic, TXT_NewCheckBox("������������ ���������� ��������� ������",   &randomly_flipcorpses)),
        TXT_If(gamemission == doom,	TXT_NewCheckBox("������������ �����-���������",                                         &floating_powerups)),
        
    TXT_If(gamemission == doom || gamemission == heretic, TXT_NewSeparator("��������")),
        TXT_If(gamemission == doom, TXT_NewCheckBox("���������� ������ ������������ �������",                   &fix_map_errors)),
        TXT_If(gamemission == doom || gamemission == heretic, TXT_NewCheckBox("���������� ��������� �������",   &flip_levels)),
        TXT_If(gamemission == doom,	TXT_NewCheckBox("���������� ������� \"Ouch face\"",                         &new_ouch_face)),
        TXT_If(gamemission == doom,	TXT_NewCheckBox("���������� ���� ��� ����������� ���",                      &unlimited_lost_souls)),
        TXT_If(gamemission == doom,	TXT_NewCheckBox("���������� ������������� ���������� ���",                  &agressive_lost_souls)),
        TXT_If(gamemission == doom,	TXT_NewCheckBox("�� �������� ������ ��� ������� ��������",                  &fast_quickload)),

    TXT_NewSeparator("������"),
        TXT_NewCheckBox("���������� ������",    &crosshair_draw),
        TXT_NewCheckBox("��������� ��������",   &crosshair_health),
        TXT_NewCheckBox("����������� ������",   &crosshair_scale),

        // TXT_If(gamemission == strife, TXT_NewCheckBox("���������� ������ �������� �����",        &show_diskicon)),
        // TXT_If(gamemission == strife, TXT_NewCheckBox("���������� �������� ��� ������",          &show_exit_sequence)),

    NULL);
}

void BindCompatibilityVariables(void)
{
    // �������
    M_BindIntVariable("uncapped_fps",           &uncapped_fps);
    M_BindIntVariable("brightmaps",             &brightmaps);
    M_BindIntVariable("fake_contrast",          &fake_contrast);
    M_BindIntVariable("translucency",           &translucency);
    M_BindIntVariable("swirling_liquids",       &swirling_liquids);
    M_BindIntVariable("invul_sky",              &invul_sky);
    M_BindIntVariable("colored_blood",          &colored_blood);
    M_BindIntVariable("red_resurrection_flash", &red_resurrection_flash);
    M_BindIntVariable("draw_shadowed_text",     &draw_shadowed_text);
    M_BindIntVariable("show_diskicon",          &show_diskicon);

    // ����
    M_BindIntVariable("crushed_corpses_sfx",    &crushed_corpses_sfx);
    M_BindIntVariable("blazing_door_fix_sfx",   &blazing_door_fix_sfx);
    M_BindIntVariable("play_exit_sfx",          &play_exit_sfx);
    M_BindIntVariable("correct_endlevel_sfx",   &correct_endlevel_sfx);

    // �������
    M_BindIntVariable("automap_stats",          &automap_stats);
    M_BindIntVariable("secret_notification",    &secret_notification);
    M_BindIntVariable("negative_health",        &negative_health);
    M_BindIntVariable("infragreen_visor",       &infragreen_visor);

    // ������
    M_BindIntVariable("over_under",             &over_under);
    M_BindIntVariable("torque",                 &torque);
    M_BindIntVariable("weapon_bobbing",         &weapon_bobbing);
    M_BindIntVariable("ssg_blast_enemies",      &ssg_blast_enemies);
    M_BindIntVariable("randomly_flipcorpses",   &randomly_flipcorpses);
    M_BindIntVariable("floating_powerups",      &floating_powerups);

    // ��������
    M_BindIntVariable("fix_map_errors",         &fix_map_errors);
    M_BindIntVariable("flip_levels",            &flip_levels);
    M_BindIntVariable("new_ouch_face",          &new_ouch_face);
    M_BindIntVariable("unlimited_lost_souls",   &unlimited_lost_souls);
    M_BindIntVariable("agressive_lost_souls",   &agressive_lost_souls);
    M_BindIntVariable("fast_quickload",         &fast_quickload);

    // ������
    M_BindIntVariable("crosshair_draw",         &crosshair_draw);
    M_BindIntVariable("crosshair_health",       &crosshair_health);
    M_BindIntVariable("crosshair_scale",        &crosshair_scale);

    // M_BindIntVariable("show_exit_sequence",     &show_exit_sequence);    // [Strife]
}

