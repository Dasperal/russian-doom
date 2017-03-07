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

// Russian DOOM (C) 2016-2017 Julian Nechaevsky

// Sound control menu

#include <stdlib.h>

#include "m_config.h"
#include "textscreen.h"
#include "mode.h"

#include "compatibility.h"

#define WINDOW_HELP_URL "http://jnechaevsky.users.sourceforge.net/projects/rusdoom/setup/gameplay.html"

extern int english_setup;

// [JN] ������������ ����������� ���������, �.�. � ��� ������ ��� ������
/*
int vanilla_savegame_limit = 1;
int vanilla_demo_limit = 1;
*/

// [JN] �������������� ��������� ����

// - ������� -
int colored_blood = 1;           // ����� ������ ������
int new_ouch_face = 1;           // ���������� ������� "Ouch face"
int invul_sky = 1;               // ������������ ���������� ����
int red_resurrection_flash = 1;  // ������� ������� ����������� ��������
int ssg_blast_enemies = 1;       // ������������ ����� ����� ��������� ������
int translucency = 1;            // ������������ ��������
int no_pickup_flash = 0;         // �� ������ ������� ��� ��������� ���������
// - ���� -
int crushed_corpses_sfx = 1;     // ���� ������������� ������
int blazing_door_fix_sfx = 1;    // ��������� ���� �������� ������� �����
int play_exit_sfx = 1;           // ����������� ���� ��� ������ �� ����
// - �������� -
int secret_notification = 1;     // ����������� �� ����������� �������
int show_total_time = 1;         // ���������� ����� �����
int unlimited_lost_souls = 1;    // ���������� ���� ��� �����������

void CompatibilitySettings(void)
{
    txt_window_t *window;

    /* English language */
    if (english_setup)
    {
        window = TXT_NewWindow("Additional gameplay options");

        TXT_SetWindowHelpURL(window, WINDOW_HELP_URL);

        TXT_AddWidgets(window,
        TXT_If(gamemission == doom,    TXT_NewSeparator("Graphics")),
            TXT_If(gamemission == doom,	TXT_NewCheckBox("Colored blood and squished corpses",          &colored_blood)),
            TXT_If(gamemission == doom,	TXT_NewCheckBox("Correct formula of \"Ouch face\"",            &new_ouch_face)),
            TXT_If(gamemission == doom,	TXT_NewCheckBox("Invulnerability affects sky",                 &invul_sky)),
            TXT_If(gamemission == doom,	TXT_NewCheckBox("Red flash of monsters resurrection",          &red_resurrection_flash)),
            TXT_If(gamemission == doom,	TXT_NewCheckBox("Super Shotgun has a chance to blast enemies", &ssg_blast_enemies)),
            TXT_If(gamemission == doom,	TXT_NewCheckBox("Low-key transparency on some objects",        &translucency)),
            TXT_If(gamemission == doom,	TXT_NewCheckBox("No yellow flash on picking up items",         &no_pickup_flash)),

        TXT_If(gamemission == doom,    TXT_NewSeparator("Sound")), 
            TXT_If(gamemission == doom,	TXT_NewCheckBox("Sound of crushed corpses",           &crushed_corpses_sfx)),
            TXT_If(gamemission == doom,	TXT_NewCheckBox("Single sound of fast closing doors", &blazing_door_fix_sfx)),
            TXT_If(gamemission == doom,	TXT_NewCheckBox("Playing sound on exiting the game",  &play_exit_sfx)),

        TXT_If(gamemission == doom,    TXT_NewSeparator("Gameplay")),
            TXT_If(gamemission == doom,	TXT_NewCheckBox("Notification of discovered secrets",       &secret_notification)),
            TXT_If(gamemission == doom,	TXT_NewCheckBox("Total level times on intermission screen", &show_total_time)),
            TXT_If(gamemission == doom,	TXT_NewCheckBox("Pain Elemental without Lost Soul's limit", &unlimited_lost_souls)),

        NULL);
    }

    /* ������� ���� */
    else
    {
        window = TXT_NewWindow("�������������� ��������� ����");

        TXT_SetWindowHelpURL(window, WINDOW_HELP_URL);

        TXT_AddWidgets(window,
        TXT_If(gamemission == doom,    TXT_NewSeparator("�������")),
            TXT_If(gamemission == doom,	TXT_NewCheckBox("������������ ����� � ����� ��������",       &colored_blood)),
            TXT_If(gamemission == doom,	TXT_NewCheckBox("���������� ������� \"Ouch face\"",          &new_ouch_face)),
            TXT_If(gamemission == doom,	TXT_NewCheckBox("������������ ���������� ����",              &invul_sky)),
            TXT_If(gamemission == doom,	TXT_NewCheckBox("������� ������� ����������� ��������",      &red_resurrection_flash)),
            TXT_If(gamemission == doom,	TXT_NewCheckBox("������������ ����� ����� ��������� ������", &ssg_blast_enemies)),
            TXT_If(gamemission == doom,	TXT_NewCheckBox("������ ������������ � ��������� ��������",  &translucency)),
            TXT_If(gamemission == doom,	TXT_NewCheckBox("�� ������ ������� ��� ��������� ���������", &no_pickup_flash)),

        TXT_If(gamemission == doom,    TXT_NewSeparator("����")), 
            TXT_If(gamemission == doom,	TXT_NewCheckBox("���� ������������� ������",             &crushed_corpses_sfx)),
            TXT_If(gamemission == doom,	TXT_NewCheckBox("��������� ���� �������� ������� �����", &blazing_door_fix_sfx)),
            TXT_If(gamemission == doom,	TXT_NewCheckBox("����������� ���� ��� ������ �� ����",   &play_exit_sfx)),

        TXT_If(gamemission == doom,    TXT_NewSeparator("��������")),
            TXT_If(gamemission == doom,	TXT_NewCheckBox("����������� �� ����������� ��������",   &secret_notification)),
            TXT_If(gamemission == doom,	TXT_NewCheckBox("����� ����� �� ������������� ������",   &show_total_time)),
            TXT_If(gamemission == doom,	TXT_NewCheckBox("���������� ���� ��� ����������� ���",   &unlimited_lost_souls)),

        NULL);
    }
}

void BindCompatibilityVariables(void)
{
    /*
    M_BindIntVariable("vanilla_savegame_limit", &vanilla_savegame_limit);
    M_BindIntVariable("vanilla_demo_limit",     &vanilla_demo_limit);
    */

    // [JN] �������������� ��������� ��������

    // - ������� -
    M_BindIntVariable("colored_blood",          &colored_blood);            // ����� ������ ������
    M_BindIntVariable("new_ouch_face",          &new_ouch_face);            // ���������� ������� "Ouch face"
    M_BindIntVariable("invul_sky",              &invul_sky);                // ������������ ���������� ����
    M_BindIntVariable("red_resurrection_flash", &red_resurrection_flash);   // ������� ������� ����������� ��������
    M_BindIntVariable("ssg_blast_enemies",      &ssg_blast_enemies);        // ������������ ����� ����� ��������� ������
    M_BindIntVariable("translucency",           &translucency);             // ������������ ��������
    M_BindIntVariable("no_pickup_flash",        &no_pickup_flash);          // �� ������ ������� ��� ��������� ���������
    // - ���� -
    M_BindIntVariable("crushed_corpses_sfx",    &crushed_corpses_sfx);      // ���� ������������� ������
    M_BindIntVariable("blazing_door_fix_sfx",   &blazing_door_fix_sfx);     // ��������� ���� �������� ������� �����
    M_BindIntVariable("play_exit_sfx",          &play_exit_sfx);            // ����������� ���� ��� ������ �� ����
    // - �������� -
    M_BindIntVariable("secret_notification",    &secret_notification);      // ����������� �� ����������� �������
    M_BindIntVariable("show_total_time",        &show_total_time);          // ���������� ����� �����
    M_BindIntVariable("unlimited_lost_souls",   &unlimited_lost_souls);     // ���������� ���� ��� �����������
}

