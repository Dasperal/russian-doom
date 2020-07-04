//
// Copyright(C) 2005-2014 Simon Howard
// Copyright(C) 2016-2020 Julian Nechaevsky
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



#include "textscreen.h"
#include "doomtype.h"
#include "m_config.h"
#include "m_controls.h"
#include "m_misc.h"

#include "execute.h"
#include "txt_keyinput.h"

#include "mode.h"
#include "joystick.h"
#include "keyboard.h"

#define WINDOW_HELP_URL "https://jnechaevsky.github.io/projects/rusdoom/setup/index.html"

int vanilla_keyboard_mapping = 1;

static int always_run = 0;

// Keys within these groups cannot have the same value.

static int *controls[] = { &key_left, &key_right, &key_up, &key_down,
                           &key_toggleautorun, &key_togglecrosshair, &key_togglemlook, &key_togglefliplvls,
                           &key_strafeleft, &key_straferight, &key_fire,
                           &key_use, &key_strafe, &key_speed, &key_jump,
                           &key_flyup, &key_flydown, &key_flycenter,
                           &key_lookup, &key_lookdown, &key_lookcenter,
                           &key_invleft, &key_invright, /*&key_invquery,*/
                           &key_invuse, &key_invpop, &key_mission, &key_invkey,
                           &key_invhome, &key_invend, &key_invdrop,
                           &key_useartifact, &key_pause, &key_usehealth,
                           &key_weapon1, &key_weapon2, &key_weapon3,
                           &key_weapon4, &key_weapon5, &key_weapon6,
                           &key_weapon7, &key_weapon8,
                           &key_arti_quartz, &key_arti_urn, &key_arti_bomb, 
                           &key_arti_tome, &key_arti_ring, &key_arti_morphegg, &key_arti_chaosdevice, 
                           &key_arti_shadowsphere, &key_arti_wings, &key_arti_torch,                           
                           &key_arti_all, &key_arti_health, &key_arti_poisonbag,
                           &key_arti_blastradius, &key_arti_teleport,
                           &key_arti_teleportother, &key_arti_egg,
                           &key_arti_invulnerability, &key_arti_superhealth, &key_arti_boostarmor,
                           &key_arti_boostmana, &key_arti_summon, &key_arti_fly, &key_arti_speed,
                           &key_arti_torch, &key_arti_healingradius,
                           &key_prevweapon, &key_nextweapon, NULL };

static int *menu_nav[] = { &key_menu_activate, &key_menu_up, &key_menu_down,
                           &key_menu_left, &key_menu_right, &key_menu_back,
                           &key_menu_forward, NULL };

static int *shortcuts[] = { &key_menu_help, &key_menu_save, &key_menu_load,
                            &key_menu_volume, &key_menu_detail, &key_menu_qsave,
                            &key_menu_endgame, &key_menu_messages, &key_spy,
                            &key_menu_qload, &key_menu_quit, &key_menu_gamma,
                            &key_menu_incscreen, &key_menu_decscreen, 
                            &key_menu_screenshot,
                            &key_message_refresh, &key_multi_msg,
                            &key_multi_msgplayer[0], &key_multi_msgplayer[1],
                            &key_multi_msgplayer[2], &key_multi_msgplayer[3] };

static int *map_keys[] = { &key_map_north, &key_map_south, &key_map_east,
                           &key_map_west, &key_map_zoomin, &key_map_zoomout,
                           &key_map_toggle, &key_map_maxzoom, &key_map_follow,
                           &key_map_grid, &key_map_mark, &key_map_clearmark,
                           &key_map_overlay, &key_map_rotate,
                           NULL };

static void UpdateJoybSpeed(TXT_UNCAST_ARG(widget), TXT_UNCAST_ARG(var))
{
    if (always_run)
    {
        /*
         <Janizdreg> if you want to pick one for chocolate doom to use, 
                     pick 29, since that is the most universal one that 
                     also works with heretic, hexen and strife =P

         NB. This choice also works with original, ultimate and final exes.
        */

        joybspeed = 29;
    }
    else
    {
        joybspeed = 0;
    }
}

static int VarInGroup(int *variable, int **group)
{
    unsigned int i;

    for (i=0; group[i] != NULL; ++i)
    {
        if (group[i] == variable)
        {
            return 1;
        }
    }

    return 0;
}

static void CheckKeyGroup(int *variable, int **group)
{
    unsigned int i;

    // Don't check unless the variable is in this group.

    if (!VarInGroup(variable, group))
    {
        return;
    }

    // If another variable has the same value as the new value, reset it.

    for (i=0; group[i] != NULL; ++i)
    {
        if (*variable == *group[i] && group[i] != variable)
        {
            // A different key has the same value.  Clear the existing
            // value. This ensures that no two keys can have the same
            // value.

            *group[i] = 0;
        }
    }
}

// Callback invoked when a key control is set

static void KeySetCallback(TXT_UNCAST_ARG(widget), TXT_UNCAST_ARG(variable))
{
    TXT_CAST_ARG(int, variable);

    CheckKeyGroup(variable, controls);
    CheckKeyGroup(variable, menu_nav);
    CheckKeyGroup(variable, shortcuts);
    CheckKeyGroup(variable, map_keys);
}

// Add a label and keyboard input to the specified table.

static void AddKeyControl(TXT_UNCAST_ARG(table), char *name, int *var)
{
    TXT_CAST_ARG(txt_table_t, table);
    txt_key_input_t *key_input;

    TXT_AddWidget(table, TXT_NewLabel(name));
    key_input = TXT_NewKeyInput(var);
    TXT_AddWidget(table, key_input);

    TXT_SignalConnect(key_input, "set", KeySetCallback, var);
}

static void AddSectionLabel(TXT_UNCAST_ARG(table), char *title,
                            boolean add_space)
{
    TXT_CAST_ARG(txt_table_t, table);
    char buf[64];

    if (add_space)
    {
        TXT_AddWidgets(table,
                       TXT_NewStrut(0, 1),
                       TXT_TABLE_EOL,
                       NULL);
    }

    M_snprintf(buf, sizeof(buf), " - %s - ", title);

    TXT_AddWidgets(table,
                   TXT_NewLabel(buf),
                   TXT_TABLE_EOL,
                   NULL);
}
static void ConfigExtraKeys(TXT_UNCAST_ARG(widget), TXT_UNCAST_ARG(unused))
{
    txt_window_t *window;
    txt_scrollpane_t *scrollpane;
    txt_table_t *table;
    boolean extra_keys = gamemission == doom
                      || gamemission == heretic
                      || gamemission == hexen
                      || gamemission == strife;

    window = TXT_NewWindow(english_language ?
                           "Extra keyboard controls" :
                           "�������������� ����������");

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

    table = TXT_NewTable(2);

    TXT_SetColumnWidths(table, 21, 9);

    if (extra_keys)
    {
        // When we have extra controls, a scrollable pane must be used.

        scrollpane = TXT_NewScrollPane(0, 13, table);
        TXT_AddWidget(window, scrollpane);

        AddSectionLabel(table, english_language ?
                               "View" :
                               "������",
                               false);

        if (gamemission == heretic || gamemission == hexen)
        {
            AddKeyControl(table, english_language ?
                                 "Look up" :
                                 "�������� �����",
                                 &key_lookup);
            AddKeyControl(table, english_language ?
                                 "Look down" :
                                 "�������� ����",
                                 &key_lookdown);
            AddKeyControl(table, english_language ?
                                 "Center view" :
                                 "������������",
                                 &key_lookcenter);
        }

        AddKeyControl(table, english_language ?
                             "Mouse look" :
                             "����� �����",
                             &key_togglemlook);

        if (gamemission == heretic || gamemission == hexen)
        {
            AddSectionLabel(table, english_language ?
                                   "Flying" :
                                   "�����",
                                   true);

            AddKeyControl(table, english_language ?
                                 "Fly up" :
                                 "������ �����",
                                 &key_flyup);
            AddKeyControl(table, english_language ?
                                 "Fly down" :
                                 "������ ����",
                                 &key_flydown);
            AddKeyControl(table, english_language ?
                                 "Fly center" :
                                 "������������",
                                 &key_flycenter);
        }

        if (gamemission == heretic || gamemission == hexen)
        {
            AddSectionLabel(table, english_language ?
                                   "Inventory" :
                                   "���������",
                                   true);

            AddKeyControl(table, english_language ?
                                 "Inventory left" :
                                 "���������� �����",
                                 &key_invleft);
            AddKeyControl(table, english_language ?
                                 "Inventory right" :
                                 "���������� ������",
                                 &key_invright);


            if (gamemission == strife)
            {
                AddKeyControl(table, english_language ?
                                     "Home" :
                                     "���������� � ������",
                                     &key_invhome);
                AddKeyControl(table, english_language ?
                                     "End" :
                                     "���������� � �����",
                                     &key_invend);
                // AddKeyControl(table, "Query",             &key_invquery);
                AddKeyControl(table, english_language ?
                                     "Drop" :
                                     "������� �������",
                                     &key_invdrop);
                AddKeyControl(table, english_language ?
                                     "Show weapons" :
                                     "�������� ������",
                                     &key_invpop);
                AddKeyControl(table, english_language ?
                                     "Show mission" :
                                     "�������� �������",
                                     &key_mission);
                AddKeyControl(table, english_language ?
                                     "Show keys" :
                                     "�������� �����",
                                     &key_invkey);
                AddKeyControl(table, english_language ?
                                     "Use" :
                                     "������������ �������",
                                     &key_invuse);
                AddKeyControl(table, english_language ?
                                     "Use health" :
                                     "������� �������",
                                     &key_usehealth);
            }
            else
            {
                AddKeyControl(table, english_language ?
                                     "Use artifact" :
                                     "������������ �������� ",
                                     &key_useartifact);
            }
        }

        if (gamemission == heretic)
        {
            AddSectionLabel(table, english_language ?
                                   "Artifacts" :
                                   "���������",
                                   true);

            AddKeyControl(table, english_language ?
                                 "Quartz Flask" :
                                 "��������� ������",
                                 &key_arti_quartz);
            AddKeyControl(table, english_language ?
                                 "Mystic Urn" :
                                 "����������� ����",
                                 &key_arti_urn);
            AddKeyControl(table, english_language ? 
                                 "Timebomb" :
                                 "������� ����� ������� ",
                                 &key_arti_bomb);
            AddKeyControl(table, english_language ?
                                 "Tome of Power" :
                                 "��� ����������",
                                 &key_arti_tome);
            AddKeyControl(table, english_language ?
                                 "Ring of Invincibility " :
                                 "������ ������������",
                                 &key_arti_ring);
            AddKeyControl(table, english_language ?
                                 "Morph Ovum" :
                                 "���� �����������",
                                 &key_arti_morphegg);
            AddKeyControl(table, english_language ?
                                 "Chaos Device" :
                                 "������� �����",
                                 &key_arti_chaosdevice);
            AddKeyControl(table, english_language ?
                                 "Shadowsphere" :
                                 "������� �����",
                                 &key_arti_shadowsphere);
            AddKeyControl(table, english_language ?
                                 "Wings of Wrath" :
                                 "������ �����",
                                 &key_arti_wings);
            AddKeyControl(table, english_language ?
                                 "Torch" :
                                 "�����",
                                 &key_arti_torch);
        }

        if (gamemission == hexen)
        {
            AddSectionLabel(table, english_language ?
                                   "Artifacts" :
                                   "���������",
                                   true);

            AddKeyControl(table, english_language ?
                                 "One of each" :
                                 "��� ����������",
                                 &key_arti_all);
            AddKeyControl(table, english_language ?
                                 "Quartz Flask" :
                                 "��������� ������",
                                 &key_arti_health);
            AddKeyControl(table, english_language ?
                                 "Flechette" :
                                 "�����",
                                 &key_arti_poisonbag);
            AddKeyControl(table, english_language ?
                                 "Disc of Repulsion" :
                                 "���� ����������",
                                 &key_arti_blastradius);
            AddKeyControl(table, english_language ?
                                 "Chaos Device" :
                                 "������� �����",
                                 &key_arti_teleport);
            AddKeyControl(table, english_language ?
                                 "Banishment Device" :
                                 "������� ��������",
                                 &key_arti_teleportother);
            AddKeyControl(table, english_language ?
                                 "Porkalator" :
                                 "�����������",
                                 &key_arti_egg);
            AddKeyControl(table, english_language ?
                                 "Icon of the Defender" :
                                 "������ ���������",
                                 &key_arti_invulnerability);
            
            AddKeyControl(table, english_language ?
                                 "Mystic Urn" :
                                 "����������� ����",
                                 &key_arti_superhealth);
            AddKeyControl(table, english_language ?
                                 "Dragonskin Bracers" :
                                 "������ �� ��������� ����",
                                 &key_arti_boostarmor);
            AddKeyControl(table, english_language ?
                                 "Krater of Might" :
                                 "���� ����������",
                                 &key_arti_boostmana);
            AddKeyControl(table, english_language ?
                                 "Dark Servant" :
                                 "������ �����",
                                 &key_arti_summon);
            AddKeyControl(table, english_language ?
                                 "Wings of Wrath" :
                                 "������ �����",
                                 &key_arti_fly);
            AddKeyControl(table, english_language ?
                                 "Boots of Speed" :
                                 "������-���������",
                                 &key_arti_speed);
            AddKeyControl(table, english_language ?
                                 "Torch" :
                                 "�����",
                                 &key_arti_torch);
            AddKeyControl(table, english_language ?
                                 "Mystic Ambit Incant" :
                                 "���� ����������� �������� ",
                                 &key_arti_healingradius);
        }
    }
    else
    {
        TXT_AddWidget(window, table);
    }

    AddSectionLabel(table, english_language ?
                           "Weapons" :
                           "������",
                           extra_keys);

    AddKeyControl(table, english_language ?
                         "Weapon 1" :
                         "������ 1",
                         &key_weapon1);
    AddKeyControl(table, english_language ?
                         "Weapon 2" :
                         "������ 2",
                         &key_weapon2);
    AddKeyControl(table, english_language ?
                         "Weapon 3" :
                         "������ 3",
                         &key_weapon3);
    AddKeyControl(table, english_language ?
                         "Weapon 4" :
                         "������ 4",
                         &key_weapon4);
    if (gamemission != hexen)
    {   // [JN] Weapons 5, 6, 7 are not available in Hexen
        AddKeyControl(table, english_language ?
                             "Weapon 5" :
                             "������ 5",
                             &key_weapon5);
        AddKeyControl(table, english_language ?
                             "Weapon 6" :
                             "������ 6",
                             &key_weapon6);
        AddKeyControl(table, english_language ?
                             "Weapon 7" :
                             "������ 7",
                             &key_weapon7);
    }
    if (gamemission == doom)
    {   // [JN] Weapon 8 only available in Doom
        AddKeyControl(table, english_language ?
                             "Weapon 8" :
                             "������ 8",
                             &key_weapon8);
    }
    AddKeyControl(table, english_language ?
                         "Previous weapon" :
                         "���������� ������",
                         &key_prevweapon);
    AddKeyControl(table, english_language ?
                         "Next weapon" :
                         "��������� ������",
                         &key_nextweapon);

    AddSectionLabel(table, english_language ?
                           "Movement" :
                           "��������",
                           true);
    AddKeyControl(table, english_language ?
                         "Toggle always run" :
                         "���������� ���",
                         &key_toggleautorun);
    
    AddSectionLabel(table, english_language ?
                           "Crosshair" :
                           "������",
                           true);
    AddKeyControl(table, english_language ?
                         "Toggle crosshair" :
                         "������������ �������",
                         &key_togglecrosshair);


    AddSectionLabel(table, english_language ?
                           "Extra" :
                           "������",
                           extra_keys);

    AddKeyControl(table, english_language ?
                         "Flip game levels" :
                         "�������������� ������ ",
                         &key_togglefliplvls);
}

static void OtherKeysDialog(TXT_UNCAST_ARG(widget), TXT_UNCAST_ARG(unused))
{
    txt_window_t *window;
    txt_table_t *table;
    txt_scrollpane_t *scrollpane;

    window = TXT_NewWindow(english_language ?
                           "Other keys" :
                           "������ �������");

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

    table = TXT_NewTable(2);

    TXT_SetColumnWidths(table, 25, 9);

    AddSectionLabel(table, english_language ?
                           "Menu navigation" :
                           "��������� � ����",
                           false);

    AddKeyControl(table, english_language ?
                         "Activate menu" :
                         "������������ ����",
                         &key_menu_activate);
    AddKeyControl(table, english_language ?
                         "Move cursor up" :
                         "������ �����",
                         &key_menu_up);
    AddKeyControl(table, english_language ?
                         "Move cursor down" :
                         "������ ����",
                         &key_menu_down);
    AddKeyControl(table, english_language ?
                         "Move slider left" :
                         "�������� �����",
                         &key_menu_left);
    AddKeyControl(table, english_language ?
                         "Move slider right" :
                         "�������� ������",
                         &key_menu_right);
    AddKeyControl(table, english_language ?
                         "Go to previous menu" :
                         "���������� �����",
                         &key_menu_back);
    AddKeyControl(table, english_language ?
                         "Activate menu item" :
                         "������������ ����� ����",
                         &key_menu_forward);
    AddKeyControl(table, english_language ?
                         "Confirm action" :
                         "����������� ��������",
                         &key_menu_confirm);
    AddKeyControl(table, english_language ?
                         "Cancel action" :
                         "�������� ��������",
                         &key_menu_abort);

    AddSectionLabel(table, english_language ?
                           "Shortcut keys" :
                           "������� �������� �������",
                           true);

    AddKeyControl(table, english_language ?
                         "Pause game" :
                         "�����",
                         &key_pause);
    AddKeyControl(table, english_language ?
                         "Help screen" :
                         "������",
                         &key_menu_help);
    AddKeyControl(table, english_language ?
                         "Save game" :
                         "����������",
                         &key_menu_save);
    AddKeyControl(table, english_language ?
                         "Load game" :
                         "��������",
                         &key_menu_load);
    AddKeyControl(table, english_language ?
                         "Sound volume" :
                         "���������",
                         &key_menu_volume);
    AddKeyControl(table, english_language ?
                         "Toggle detail" :
                         "�����������",
                         &key_menu_detail);
    AddKeyControl(table, english_language ?
                         "Quick save" :
                         "������� ����������",
                         &key_menu_qsave);
    AddKeyControl(table, english_language ?
                         "End game" :
                         "��������� ����",
                         &key_menu_endgame);
    AddKeyControl(table, english_language ?
                         "Toggle messages" :
                         "���������",
                         &key_menu_messages);
    AddKeyControl(table, english_language ?
                         "Quick load" :
                         "������� ��������",
                         &key_menu_qload);
    AddKeyControl(table, english_language ?
                         "Quit game" :
                         "����� �� ����",
                         &key_menu_quit);
    AddKeyControl(table, english_language ?
                         "Toggle gamma" :
                         "�����-���������",
                         &key_menu_gamma);
    AddKeyControl(table, english_language ?
                         "Multiplayer spy" :
                         "��� ������� ������",
                         &key_spy);

    AddKeyControl(table, english_language ?
                         "Increase screen size" :
                         "��������� �����",
                         &key_menu_incscreen);
    AddKeyControl(table, english_language ?
                         "Decrease screen size" :
                         "��������� �����",
                         &key_menu_decscreen);
    AddKeyControl(table, english_language ?
                         "Save a screenshot" :
                         "��������",
                         &key_menu_screenshot);

    AddKeyControl(table, english_language ?
                         "Display last message" :
                         "�������� ��������� ��������� ",
                         &key_message_refresh);
    AddKeyControl(table, english_language ?
                         "Finish recording demo" :
                         "��������� ������ ����",
                         &key_demo_quit);

    AddSectionLabel(table, english_language ?
                           "Map" :
                           "�����",
                           true);
    AddKeyControl(table, english_language ?
                         "Toggle map" :
                         "������� �����",
                         &key_map_toggle);
    AddKeyControl(table, english_language ?
                         "Zoom in" :
                         "����������",
                         &key_map_zoomin);
    AddKeyControl(table, english_language ?
                         "Zoom out" :
                         "��������",
                         &key_map_zoomout);
    AddKeyControl(table, english_language ?
                         "Maximum zoom out" :
                         "������ �������",
                         &key_map_maxzoom);
    AddKeyControl(table, english_language ?
                         "Follow mode" :
                         "����� ����������",
                         &key_map_follow);
    if (gamemission == doom || gamemission == heretic)
    {
    AddKeyControl(table, english_language ?
                         "Overlay mode" :
                         "����� ���������",
                         &key_map_overlay);
    AddKeyControl(table, english_language ?
                         "Rotate mode" :
                         "����� ��������",
                         &key_map_rotate);
    }
    AddKeyControl(table, english_language ?
                         "Pan north" :
                         "���������� �����",
                         &key_map_north);
    AddKeyControl(table, english_language ?
                         "Pan south" :
                         "���������� ����",
                         &key_map_south);
    AddKeyControl(table, english_language ?
                         "Pan east" :
                         "���������� ������",
                         &key_map_east);
    AddKeyControl(table, english_language ?
                         "Pan west" :
                         "���������� �����",
                         &key_map_west);
    if (gamemission == doom)
    {
    AddKeyControl(table, english_language ?
                         "Toggle grid" :
                         "���������� �����",
                         &key_map_grid);
    AddKeyControl(table, english_language ?
                         "Mark location" :
                         "��������� �������",
                         &key_map_mark);
    AddKeyControl(table, english_language ?
                         "Clear all marks" :
                         "������ �������",
                         &key_map_clearmark);
    }

    AddSectionLabel(table, english_language ?
                           "Multiplayer" :
                           "������� ����",
                           true);

    AddKeyControl(table, english_language ?
                         "Send message" :
                         "��������� ���������",
                         &key_multi_msg);
    AddKeyControl(table, english_language ?
                         "- to player 1" :
                         "- ������ 1",
                         &key_multi_msgplayer[0]);
    AddKeyControl(table, english_language ?
                         "- to player 2" :
                         "- ������ 2",
                         &key_multi_msgplayer[1]);
    AddKeyControl(table, english_language ?
                         "- to player 3" :
                         "- ������ 3",
                         &key_multi_msgplayer[2]);
    AddKeyControl(table, english_language ?
                         "- to player 4" :
                         "- ������ 4",
                         &key_multi_msgplayer[3]);

    if (gamemission == hexen || gamemission == strife)
    {
        AddKeyControl(table, english_language ?
                             "- to player 5" :
                             "- ������ 5",
                             &key_multi_msgplayer[4]);
        AddKeyControl(table, english_language ?
                             "- to player 6" :
                             "- ������ 6",
                             &key_multi_msgplayer[5]);
        AddKeyControl(table, english_language ?
                             "- to player 7" :
                             "- ������ 7",
                             &key_multi_msgplayer[6]);
        AddKeyControl(table, english_language ?
                             "- to player 8" :
                             "- ������ 8",
                             &key_multi_msgplayer[7]);
    }

    scrollpane = TXT_NewScrollPane(0, 13, table);

    TXT_AddWidget(window, scrollpane);
}

void ConfigKeyboard(void)
{
    txt_window_t *window;
    txt_checkbox_t *run_control;

    always_run = joybspeed >= 20;

    window = TXT_NewWindow(english_language ?
                           "Keyboard configuration" :
                           "��������� ����������");

    if (english_language)
    TXT_SetWindowHelpURL(window, WINDOW_HELP_URL);
    else
    TXT_SetWindowHelpURL_RUS(window, WINDOW_HELP_URL);

    // The window is on a 5-column grid layout that looks like:
    // Label | Control | | Label | Control
    // There is a small gap between the two conceptual "columns" of
    // controls, just for spacing.
    TXT_SetTableColumns(window, 5);
    TXT_SetColumnWidths(window, 16, 8, 2, 16, 8);

    TXT_AddWidget(window, TXT_NewSeparator(english_language ?
                                           "Movement" :
                                           "��������"));
    AddKeyControl(window, english_language ?
                          "Move Forward" :
                          "�������� ������  ",
                          &key_up);
    TXT_AddWidget(window, TXT_TABLE_EMPTY);
    AddKeyControl(window, english_language ?
                          "Strafe Left" :
                          " ����� �����",
                          &key_strafeleft);

    AddKeyControl(window, english_language ?
                          "Move Backward" :
                          "�������� ����� ",
                          &key_down);
    TXT_AddWidget(window, TXT_TABLE_EMPTY);
    AddKeyControl(window, english_language ?
                          "Strafe Right" :
                          " ����� ������",
                          &key_straferight);

    AddKeyControl(window, english_language ?
                          "Turn Left" :
                          "������� ������ ",
                          &key_left);
    TXT_AddWidget(window, TXT_TABLE_EMPTY);
    AddKeyControl(window, english_language ?
                          "Speed On" :
                          " ���",
                          &key_speed);

    AddKeyControl(window, english_language ?
                          "Turn Right" :
                          "������� ������� ",
                          &key_right);
    TXT_AddWidget(window, TXT_TABLE_EMPTY);
    AddKeyControl(window, english_language ?
                          "Strafe On" :
                          " �������� ����� ",
                          &key_strafe);

    if (gamemission == hexen || gamemission == strife)
    {
        AddKeyControl(window, english_language ?
        "Jump" :
        "������",
        &key_jump);
    }

    TXT_AddWidget(window, TXT_NewSeparator(english_language ?
                                           "Action" :
                                           "��������"));
    AddKeyControl(window, english_language ?
                          "Fire/Attack" :
                          "�����/��������  ",
                          &key_fire);
    TXT_AddWidget(window, TXT_TABLE_EMPTY);
    AddKeyControl(window, english_language ?
                          "Use" :
                          " ������������   ",
    &key_use);
    TXT_AddWidgets(window,
        TXT_NewButton2(english_language ?
                       "More controls..." :
                       "�������������...",
                       ConfigExtraKeys, NULL),
        TXT_TABLE_OVERFLOW_RIGHT,
        TXT_TABLE_EMPTY,
        TXT_NewButton2(english_language ?
                       "Other keys..." :
                       " ������ �������...",
                       OtherKeysDialog, NULL),
        TXT_TABLE_OVERFLOW_RIGHT,

        TXT_NewSeparator(english_language ?
                         "Misc." :
                         "�������������"),
        run_control = TXT_NewCheckBox(english_language ?
                                      "Always run" :
                                      "����� ����������� ����",
                                      &always_run),
        TXT_TABLE_EOL,
        // [JN] Hidden, non-vanilla mapping is not 
        // very friendly with Russian keyboard layout.
        // TXT_NewInvertedCheckBox("������������ �������� ���������",
        //                         &vanilla_keyboard_mapping),
        TXT_TABLE_EOL,
        NULL);

    TXT_SignalConnect(run_control, "changed", UpdateJoybSpeed, NULL);

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
}

void BindKeyboardVariables(void)
{
    M_BindIntVariable("vanilla_keyboard_mapping", &vanilla_keyboard_mapping);
}

