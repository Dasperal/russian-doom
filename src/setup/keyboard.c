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

// =--------------------------------------------------------------=
// Copyright(C) 2016-2017 Julian Nechaevsky
//
// ��������:
// * ������� ������� Setup.exe
// =--------------------------------------------------------------=

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

#define WINDOW_HELP_URL "http://jnechaevsky.users.sourceforge.net/projects/rusdoom/setup/keyboard.html"

int vanilla_keyboard_mapping = 1;

static int always_run = 0;

// Keys within these groups cannot have the same value.

static int *controls[] = { &key_left, &key_right, &key_up, &key_down,
                           &key_toggleautorun,
                           &key_strafeleft, &key_straferight, &key_fire,
                           &key_use, &key_strafe, &key_speed, &key_jump,
                           &key_flyup, &key_flydown, &key_flycenter,
                           &key_lookup, &key_lookdown, &key_lookcenter,
                           &key_invleft, &key_invright, &key_invquery,
                           &key_invuse, &key_invpop, &key_mission, &key_invkey,
                           &key_invhome, &key_invend, &key_invdrop,
                           &key_useartifact, &key_pause, &key_usehealth,
                           &key_weapon1, &key_weapon2, &key_weapon3,
                           &key_weapon4, &key_weapon5, &key_weapon6,
                           &key_weapon7, &key_weapon8,
                           &key_arti_all, &key_arti_health, &key_arti_poisonbag,
                           &key_arti_blastradius, &key_arti_teleport,
                           &key_arti_teleportother, &key_arti_egg,
                           &key_arti_invulnerability,
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
    boolean extra_keys = gamemission == heretic
                      || gamemission == hexen
                      || gamemission == strife;

	// "Extra keyboard controls"
    window = TXT_NewWindow("�������������� ����������");

    TXT_SetWindowHelpURL(window, WINDOW_HELP_URL);

    table = TXT_NewTable(2);

    TXT_SetColumnWidths(table, 21, 9);

    if (extra_keys)
    {
        // When we have extra controls, a scrollable pane must be used.

        scrollpane = TXT_NewScrollPane(0, 13, table);
        TXT_AddWidget(window, scrollpane);

		// "View"
        AddSectionLabel(table, "������", false);

		// "Look up"
        AddKeyControl(table, "�������� �����", &key_lookup);
		// "Look down"
        AddKeyControl(table, "�������� ����", &key_lookdown);
		// "Center view"
        AddKeyControl(table, "������������", &key_lookcenter);

        if (gamemission == heretic || gamemission == hexen)
        {
			// "Flying"
            AddSectionLabel(table, "�����", true);

			// "Fly up"
            AddKeyControl(table, "������ �����", &key_flyup);
			// "Fly down"
            AddKeyControl(table, "������ ����", &key_flydown);
			// "Fly center"
            AddKeyControl(table, "������������", &key_flycenter);
        }

		// "Inventory"
        AddSectionLabel(table, "���������", true);

		// "Inventory left"
        AddKeyControl(table, "���������� �����", &key_invleft);
		// "Inventory right"
        AddKeyControl(table, "���������� ������", &key_invright);

		// [JN] Strife � ��� �� ��������������, ����������.
        if (gamemission == strife)
        {
            AddKeyControl(table, "Home", &key_invhome);
            AddKeyControl(table, "End", &key_invend);
            AddKeyControl(table, "Query", &key_invquery);
            AddKeyControl(table, "Drop", &key_invdrop);
            AddKeyControl(table, "Show weapons", &key_invpop);
            AddKeyControl(table, "Show mission", &key_mission);
            AddKeyControl(table, "Show keys", &key_invkey);
            AddKeyControl(table, "Use", &key_invuse);
            AddKeyControl(table, "Use health", &key_usehealth);
        }
        else
        {
			// "Use artifact"
            AddKeyControl(table, "������������ �������� ", &key_useartifact);
        }

        if (gamemission == hexen)
        {
			// "Artifacts"
            AddSectionLabel(table, "���������", true);

			// "One of each"
            AddKeyControl(table, "��� ����������", &key_arti_all);
			// "Quartz Flask"
            AddKeyControl(table, "��������� ������", &key_arti_health);
			// "Flechette"
            AddKeyControl(table, "�����", &key_arti_poisonbag);
			// "Disc of Repulsion"
            AddKeyControl(table, "���� ����������", &key_arti_blastradius);
			// "Chaos Device"
            AddKeyControl(table, "������� �����", &key_arti_teleport);
			// Banishment Device
            AddKeyControl(table, "������� ��������", &key_arti_teleportother);
			// "Porkalator"
            AddKeyControl(table, "�����������", &key_arti_egg);
			// "Icon of the Defender"
            AddKeyControl(table, "������ ���������",
                          &key_arti_invulnerability);
        }
    }
    else
    {
        TXT_AddWidget(window, table);
    }

    if (gamemission == doom)
    {
        AddSectionLabel(table, "��������", false);
        AddKeyControl(table, "����� ����������� ���� ", &key_toggleautorun);
    }
    
	// "Weapons"
    AddSectionLabel(table, "������", extra_keys);

	// "Weapon N"
    AddKeyControl(table, "������ 1", &key_weapon1);
    AddKeyControl(table, "������ 2", &key_weapon2);
    AddKeyControl(table, "������ 3", &key_weapon3);
    AddKeyControl(table, "������ 4", &key_weapon4);
    AddKeyControl(table, "������ 5", &key_weapon5);
    AddKeyControl(table, "������ 6", &key_weapon6);
    AddKeyControl(table, "������ 7", &key_weapon7);
    AddKeyControl(table, "������ 8", &key_weapon8);
	// "Previous weapon"
    AddKeyControl(table, "���������� ������", &key_prevweapon);
	// "Next weapon"
    AddKeyControl(table, "��������� ������", &key_nextweapon);
}

static void OtherKeysDialog(TXT_UNCAST_ARG(widget), TXT_UNCAST_ARG(unused))
{
    txt_window_t *window;
    txt_table_t *table;
    txt_scrollpane_t *scrollpane;

	// "Other keys"
    window = TXT_NewWindow("������ �������");

    TXT_SetWindowHelpURL(window, WINDOW_HELP_URL);

    table = TXT_NewTable(2);

    TXT_SetColumnWidths(table, 25, 9);

	// "Menu navigation"
    AddSectionLabel(table, "��������� � ����", false);

	// "Activate menu"
    AddKeyControl(table, "������������ ����",		&key_menu_activate);
	// "Move cursor up"
    AddKeyControl(table, "������ �����",			&key_menu_up);
	// "Move cursor down"
    AddKeyControl(table, "������ ����",				&key_menu_down);
	// "Move slider left"
    AddKeyControl(table, "�������� �����",			&key_menu_left);
	// "Move slider right"
    AddKeyControl(table, "�������� ������",			&key_menu_right);
	// "Go to previous menu"
    AddKeyControl(table, "���������� �����",		&key_menu_back);
	// "Activate menu item"
    AddKeyControl(table, "������������ ����� ����",	&key_menu_forward);
	// "Confirm action"
    AddKeyControl(table, "����������� ��������",	&key_menu_confirm);
	// "Cancel action"
    AddKeyControl(table, "�������� ��������",		&key_menu_abort);

	// "Shortcut keys"
    AddSectionLabel(table, "������� �������� �������", true);

	// "Pause game"
    AddKeyControl(table, "�����",					&key_pause);
	// "Help screen"
    AddKeyControl(table, "������",					&key_menu_help);
	// "Save game"
    AddKeyControl(table, "����������",				&key_menu_save);
	// "Load game"
    AddKeyControl(table, "��������",				&key_menu_load);
	// "Sound volume"
    AddKeyControl(table, "���������",				&key_menu_volume);
	// "Toggle detail"
    AddKeyControl(table, "�����������",				&key_menu_detail);
	// "Quick save"
    AddKeyControl(table, "������� ����������",		&key_menu_qsave);
	// "End game"
    AddKeyControl(table, "��������� ����",			&key_menu_endgame);
	// "Toggle messages"
    AddKeyControl(table, "���������",				&key_menu_messages);
	// "Quick load"
    AddKeyControl(table, "������� ��������",		&key_menu_qload);
	// "Quit game"
    AddKeyControl(table, "����� �� ����",			&key_menu_quit);
	// "Toggle gamma"
    AddKeyControl(table, "�����-���������",			&key_menu_gamma);
	// "Multiplayer spy"
    AddKeyControl(table, "��� ������� ������",		&key_spy);

	
	// Increase screen size
    AddKeyControl(table, "��������� �����",			&key_menu_incscreen);
	// Decrease screen size
    AddKeyControl(table, "��������� �����",			&key_menu_decscreen);
	// Save a screenshot
    AddKeyControl(table, "��������",				&key_menu_screenshot);

	// "Display last message"
    AddKeyControl(table, "�������� ��������� ��������� ",	&key_message_refresh);
	// "Finish recording demo"
    AddKeyControl(table, "��������� ������ ����",			&key_demo_quit);

	// "Map"
    AddSectionLabel(table, "�����", true);
	// "Toggle map"
    AddKeyControl(table, "������� �����",            &key_map_toggle);
	// "Zoom in"
    AddKeyControl(table, "����������",               &key_map_zoomin);
	// "Zoom out"
    AddKeyControl(table, "��������",              &key_map_zoomout);
	// "Maximum zoom out"
    AddKeyControl(table, "������������ ���������",      &key_map_maxzoom);
	// "Follow mode"
    AddKeyControl(table, "����� ����������",           &key_map_follow);
	// "Pan north"
    AddKeyControl(table, "���������� �����",             &key_map_north);
	// "Pan south"
    AddKeyControl(table, "���������� ����",             &key_map_south);
	// "Pan east"
    AddKeyControl(table, "���������� ������",              &key_map_east);
	// "Pan west"
    AddKeyControl(table, "���������� �����",              &key_map_west);
	// "Toggle grid"
    AddKeyControl(table, "���������� �����",           &key_map_grid);
	// "Mark location"
    AddKeyControl(table, "��������� �������",         &key_map_mark);
	// "Clear all marks"
    AddKeyControl(table, "������ �������",       &key_map_clearmark);

	// "Multiplayer"
    AddSectionLabel(table, "������� ����", true);

	// "Send message"
    AddKeyControl(table, "��������� ���������",          &key_multi_msg);
	// "- to player N"
    AddKeyControl(table, "- ������ 1",         &key_multi_msgplayer[0]);
    AddKeyControl(table, "- ������ 2",         &key_multi_msgplayer[1]);
    AddKeyControl(table, "- ������ 3",         &key_multi_msgplayer[2]);
    AddKeyControl(table, "- ������ 4",         &key_multi_msgplayer[3]);

    if (gamemission == hexen || gamemission == strife)
    {
        AddKeyControl(table, "- ������ 5",     &key_multi_msgplayer[4]);
        AddKeyControl(table, "- ������ 6",     &key_multi_msgplayer[5]);
        AddKeyControl(table, "- ������ 7",     &key_multi_msgplayer[6]);
        AddKeyControl(table, "- ������ 8",     &key_multi_msgplayer[7]);
    }

    scrollpane = TXT_NewScrollPane(0, 13, table);

    TXT_AddWidget(window, scrollpane);
}

void ConfigKeyboard(void)
{
    txt_window_t *window;
    txt_checkbox_t *run_control;

    always_run = joybspeed >= 20;

	// "Keyboard configuration"
    window = TXT_NewWindow("��������� ����������");

    TXT_SetWindowHelpURL(window, WINDOW_HELP_URL);

    // The window is on a 5-column grid layout that looks like:
    // Label | Control | | Label | Control
    // There is a small gap between the two conceptual "columns" of
    // controls, just for spacing.
    TXT_SetTableColumns(window, 5);
    TXT_SetColumnWidths(window, 16, 8, 2, 16, 8);

	// "Movement"
    TXT_AddWidget(window, TXT_NewSeparator("��������"));
	// "Move Forward"
    AddKeyControl(window, "�������� ������  ", &key_up);
    TXT_AddWidget(window, TXT_TABLE_EMPTY);
	// "Strafe Left"
    AddKeyControl(window, " ����� �����", &key_strafeleft);

	// "Move Backward"
    AddKeyControl(window, "�������� ����� ", &key_down);
    TXT_AddWidget(window, TXT_TABLE_EMPTY);
	// "Strafe Right"
    AddKeyControl(window, " ����� ������", &key_straferight);

	// "Turn Left"
    AddKeyControl(window, "������� ������ ", &key_left);
    TXT_AddWidget(window, TXT_TABLE_EMPTY);
	// "Speed On"
    AddKeyControl(window, " ���", &key_speed);

	// "Turn Right"
    AddKeyControl(window, "������� ������� ", &key_right);
    TXT_AddWidget(window, TXT_TABLE_EMPTY);
	// "Strafe On"
    AddKeyControl(window, " �������� ����� ", &key_strafe);

    if (gamemission == hexen || gamemission == strife)
    {
		// "Jump"
        AddKeyControl(window, "������", &key_jump);
    }

	// "Action"
    TXT_AddWidget(window, TXT_NewSeparator("��������"));
	// "Fire/Attack"
    AddKeyControl(window, "�����/��������  ", &key_fire);
    TXT_AddWidget(window, TXT_TABLE_EMPTY);
	// "Use"
    AddKeyControl(window, " ������������   ", &key_use);

    TXT_AddWidgets(window,
				   // "More controls..."
                   TXT_NewButton2("�������������...", ConfigExtraKeys, NULL),
                   TXT_TABLE_OVERFLOW_RIGHT,
                   TXT_TABLE_EMPTY,
				   // "Other keys..."
                   TXT_NewButton2(" ������ �������...", OtherKeysDialog, NULL),
                   TXT_TABLE_OVERFLOW_RIGHT,

				   // "Misc."
                   TXT_NewSeparator("�������������"),
				   // "Always run"
                   run_control = TXT_NewCheckBox("����� ����������� ����", &always_run),
                   TXT_TABLE_EOL,
				   // "Use native keyboard mapping"
                   TXT_NewInvertedCheckBox("������������ �������� ���������",
                                           &vanilla_keyboard_mapping),
                   TXT_TABLE_EOL,
                   NULL);

    TXT_SignalConnect(run_control, "changed", UpdateJoybSpeed, NULL);
    TXT_SetWindowAction(window, TXT_HORIZ_CENTER, TestConfigAction());
}

void BindKeyboardVariables(void)
{
    M_BindIntVariable("vanilla_keyboard_mapping", &vanilla_keyboard_mapping);
}
