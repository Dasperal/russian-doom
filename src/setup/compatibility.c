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

// Russian Doom (C) 2016-2017 Julian Nechaevsky


// Sound control menu

#include <stdlib.h>

#include "m_config.h"
#include "textscreen.h"
#include "mode.h"

#include "compatibility.h"

#define WINDOW_HELP_URL "http://jnechaevsky.users.sourceforge.net/projects/rusdoom/setup/gameplay.html"

// [JN] �������������� ��������� ����

// - ��������� -
int draw_shadowed_text = 1;      // �������� ���� � ������ ����������� ����
int fast_quickload = 1;          // �� �������� ������ ��� ������� ��������
int show_diskicon = 1;           // ������ ������� / [Strife] �������� �����
int show_exit_sequence = 1;      // [Strife] ���������� �������� ��� ������
// - ������� -
int brightmaps = 1;              // ������������ ������� � ��������
int fake_contrast = 0;           // �������� ������������ ��������� ����
int colored_blood = 1;           // ����� ������ ������
int randomly_flipcorpses = 1;    // ������������ ���������� ��������� ������
int new_ouch_face = 1;           // ���������� ������� "Ouch face"
int invul_sky = 1;               // ������������ ���������� ����
int swirling_liquids = 1;        // ���������� �������� ���������
int red_resurrection_flash = 1;  // ������� ������� ����������� ��������
int ssg_blast_enemies = 1;       // ������������ ����� ����� ��������� ������
int translucency = 1;            // ������������ ��������
int weapon_bobbing = 1;          // ����������� ������ ��� �������� � ��������
// - ���� -
int crushed_corpses_sfx = 1;     // ���� ������������� ������
int blazing_door_fix_sfx = 1;    // ��������� ���� �������� ������� �����
int correct_endlevel_sfx = 0;    // ���������� ���� ���������� ������
int play_exit_sfx = 1;           // ����������� ���� ��� ������ �� ����
// - �������� -
int secret_notification = 1;      // ����������� �� ����������� �������
int show_total_time = 1;          // ���������� ����� �����
int unlimited_lost_souls = 1;     // ���������� ���� ��� �����������
int agressive_lost_souls = 0;     // ���������� ������������� ���������� ���
int negative_health = 0;          // ���������� ������������� ��������
int flip_levels = 0;              // ���������� ����������� �������

void CompatibilitySettings(void)
{
    txt_window_t *window;
    txt_table_t  *window_features;

    window = TXT_NewWindow("�������������� ��������� ����");
    window_features = TXT_NewTable(1);

    TXT_SetWindowHelpURL(window, WINDOW_HELP_URL);

    TXT_AddWidget(window, TXT_NewScrollPane(47, 15, window_features));

    TXT_AddWidgets(window_features,
    TXT_If(gamemission == doom || gamemission == heretic, TXT_NewSeparator("���������")),
        TXT_If(gamemission == doom || gamemission == heretic,	TXT_NewCheckBox("�������� ���� � ������ ����������� ����",   &draw_shadowed_text)),
        TXT_If(gamemission == doom,	TXT_NewCheckBox("�� �������� ������ ��� ������� ��������",   &fast_quickload)),
        TXT_If(gamemission == doom,	TXT_NewCheckBox("����� ����� �� ������������� ������",       &show_total_time)),
        TXT_If(gamemission == doom, TXT_NewCheckBox("���������� ������ �������",                 &show_diskicon)),
        
        // [JN] �������� ��� Strife 
        TXT_If(gamemission == strife, TXT_NewCheckBox("���������� ������ �������� �����",        &show_diskicon)),
        TXT_If(gamemission == strife, TXT_NewCheckBox("���������� �������� ��� ������",          &show_exit_sequence)),
    
    TXT_NewSeparator("�������"),
        TXT_NewCheckBox("������������ ������� � ��������",           &brightmaps),
        TXT_If(gamemission == doom || gamemission == heretic,	TXT_NewCheckBox("�������� ������������ ��������� ����",      &fake_contrast)),
        TXT_If(gamemission == doom,	TXT_NewCheckBox("������ ������������ � ��������� ��������",  &translucency)),
        TXT_If(gamemission == doom,	TXT_NewCheckBox("���������� �������� ���������",             &swirling_liquids)),
        TXT_If(gamemission == doom || gamemission == heretic,   TXT_NewCheckBox("������������ ���������� ��������� ������",  &randomly_flipcorpses)),
        TXT_If(gamemission == doom,	TXT_NewCheckBox("������������ ����� � �����",                &colored_blood)),
        TXT_If(gamemission == doom || gamemission == heretic,	TXT_NewCheckBox("������������ ���������� ����",              &invul_sky)),
        TXT_If(gamemission == doom,	TXT_NewCheckBox("������� ������� ����������� ��������",      &red_resurrection_flash)),

    TXT_If(gamemission == doom,    TXT_NewSeparator("����")), 
        TXT_If(gamemission == doom,	TXT_NewCheckBox("���� ������������� ������",                 &crushed_corpses_sfx)),
        TXT_If(gamemission == doom,	TXT_NewCheckBox("��������� ���� �������� ������� �����",     &blazing_door_fix_sfx)),
        TXT_If(gamemission == doom,	TXT_NewCheckBox("����������� ���� ��� ������ �� ����",       &play_exit_sfx)),
        TXT_If(gamemission == doom,	TXT_NewCheckBox("���������� ���� ���������� ������",         &correct_endlevel_sfx)),

    TXT_If(gamemission == doom || gamemission == heretic,    TXT_NewSeparator("��������")),
        TXT_If(gamemission == doom || gamemission == heretic,	TXT_NewCheckBox("����������� �� ����������� ��������",       &secret_notification)),
        TXT_If(gamemission == doom || gamemission == heretic,	TXT_NewCheckBox("����������� ������ ��� �������� � ��������",&weapon_bobbing)),
        TXT_If(gamemission == doom,	TXT_NewCheckBox("���������� ������� \"Ouch face\"",          &new_ouch_face)),
        TXT_If(gamemission == doom,	TXT_NewCheckBox("������������ ����� ����� ��������� ������", &ssg_blast_enemies)),
        TXT_If(gamemission == doom,	TXT_NewCheckBox("���������� ���� ��� ����������� ���",       &unlimited_lost_souls)),
        TXT_If(gamemission == doom,	TXT_NewCheckBox("���������� ������������� ���������� ���",   &agressive_lost_souls)),
        TXT_If(gamemission == doom,	TXT_NewCheckBox("���������� ������������� ��������",         &negative_health)),
        TXT_If(gamemission == doom || gamemission == heretic,	TXT_NewCheckBox("���������� ��������� �������",              &flip_levels)),
    NULL);
}

void BindCompatibilityVariables(void)
{
    // [JN] �������������� ��������� ����

    // - ��������� -
    M_BindIntVariable("draw_shadowed_text",     &draw_shadowed_text);       // �������� ���� � ������ ����������� ����
    M_BindIntVariable("fast_quickload",         &fast_quickload);           // �� �������� ������ ��� ������� ��������
    M_BindIntVariable("show_total_time",        &show_total_time);          // ���������� ����� �����
    M_BindIntVariable("show_exit_sequence",     &show_exit_sequence);       // [Strife] ���������� �������� ��� ������
    M_BindIntVariable("show_diskicon",          &show_diskicon);            // ���������� ������ �������
    // - ������� -
    M_BindIntVariable("brightmaps",             &brightmaps);               // ������������ ������� � ��������
    M_BindIntVariable("fake_contrast",          &fake_contrast);            // �������� ������������ ��������� ����
    M_BindIntVariable("translucency",           &translucency);             // ������������ ��������
    M_BindIntVariable("swirling_liquids",       &swirling_liquids);         // ���������� �������� ���������
    M_BindIntVariable("randomly_flipcorpses",   &randomly_flipcorpses);     // ������������ ���������� ��������� ������
    M_BindIntVariable("colored_blood",          &colored_blood);            // ����� ������ ������
    M_BindIntVariable("invul_sky",              &invul_sky);                // ������������ ���������� ����
    M_BindIntVariable("red_resurrection_flash", &red_resurrection_flash);   // ������� ������� ����������� ��������
    // - ���� -
    M_BindIntVariable("crushed_corpses_sfx",    &crushed_corpses_sfx);      // ���� ������������� ������
    M_BindIntVariable("blazing_door_fix_sfx",   &blazing_door_fix_sfx);     // ��������� ���� �������� ������� �����
    M_BindIntVariable("play_exit_sfx",          &play_exit_sfx);            // ����������� ���� ��� ������ �� ����
    M_BindIntVariable("correct_endlevel_sfx",   &correct_endlevel_sfx);     // ���������� ���� ���������� ������
    // - �������� -
    M_BindIntVariable("secret_notification",    &secret_notification);      // ����������� �� ����������� �������
    M_BindIntVariable("weapon_bobbing",         &weapon_bobbing);           // ����������� ������ ��� �������� � ��������
    M_BindIntVariable("new_ouch_face",          &new_ouch_face);            // ���������� ������� "Ouch face"
    M_BindIntVariable("ssg_blast_enemies",      &ssg_blast_enemies);        // ������������ ����� ����� ��������� ������
    M_BindIntVariable("unlimited_lost_souls",   &unlimited_lost_souls);     // ���������� ���� ��� �����������
    M_BindIntVariable("agressive_lost_souls",   &agressive_lost_souls);     // ���������� ������������� ���������� ���
    M_BindIntVariable("negative_health",        &negative_health);          // ���������� ������������� ��������
    M_BindIntVariable("flip_levels",            &flip_levels);              // ���������� ��������� �������
}

