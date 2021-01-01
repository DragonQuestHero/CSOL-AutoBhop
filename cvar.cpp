#include "client.h"

CVARlist cvar;
StringFinder names; 
vector<Entry> entries;

inline string getHackDirFile(const char* basename)
{
	if (strstr(basename, "..")) { return ":*?\\/<>\""; }
	string ret = hackdir;
	return (ret + basename);
}

void save_cvars(ofstream& ofs)
{
	ofs << "[CVAR]\n";

	for (names.it_start(); names.it_running(); names.it_next())
	{
		Entry& r = entries[names.num];
		ofs << names.str << "=" << *(float*)r.data << "\n";
	}
}

void load_cvars(void)
{
	char cvar_f[500];
	DWORD size = 500;

	for (names.it_start(); names.it_running(); names.it_next())
	{
		Entry& r = entries[names.num];

		GetPrivateProfileString("CVAR", names.str, "NULL", cvar_f, size, getHackDirFile("cvar.ini").c_str());
		*(float*)r.data = atof(cvar_f);
	}
}

bool fileExists(const char* filename)
{
	WIN32_FIND_DATA finddata;
	HANDLE handle = FindFirstFile(filename, &finddata);
	return (handle != INVALID_HANDLE_VALUE);
}

void AddCvarFloat(const char* name, void* data)
{
	int index = entries.size();
	Entry tmp = { data };
	entries.push_back(tmp);
	names.add(name, index);
}

void CVARlist::init()
{
	#define INITCVAR(name,value) AddCvarFloat(#name, &##name );##name=##value;

	INITCVAR(rage_active, 0.0);
	INITCVAR(rage_wall, 0.0);
	INITCVAR(rage_fov, 180.0);
	INITCVAR(rage_target_selection, 0.0);
	INITCVAR(rage_perfect_silent, 0.0);
	INITCVAR(rage_silent, 0.0); 
	INITCVAR(rage_knife_attack, 1.0);
	INITCVAR(rage_shield_attack, 0.0);
	INITCVAR(rage_nospread_method, 0.0);
	INITCVAR(rage_auto_fire, 0.0);
	INITCVAR(rage_auto_fire_key, -1.0);
	INITCVAR(rage_team, 0.0);
	INITCVAR(rage_always_fire, 0.0);
	INITCVAR(rage_draw_aim, 1.0);

	INITCVAR(aim_id_mode, 1.0);

	for (unsigned int i = 1; i <= 30; i++)
	{
		if (i == 2 || i == 4 || i == 6 || i == 9 || i == 25 || i == 29) continue;

		char str[256];
		sprintf(str, "legit[%d].drawfov", i);
		AddCvarFloat(str, &cvar.legit[i].drawfov); legit[i].drawfov = 0.0;
		sprintf(str, "legit[%d].active", i);
		AddCvarFloat(str, &cvar.legit[i].active); legit[i].active = 0.0;
		sprintf(str, "legit[%d].speed", i);
		AddCvarFloat(str, &cvar.legit[i].speed); legit[i].speed = 0.0;
		sprintf(str, "legit[%d].speed_scale_fov", i);
		AddCvarFloat(str, &cvar.legit[i].speed_scale_fov); legit[i].speed_scale_fov = 0.0;
		sprintf(str, "legit[%d].reaction_time", i);
		AddCvarFloat(str, &cvar.legit[i].reaction_time); legit[i].reaction_time = 0.0;
		sprintf(str, "legit[%d].humanize", i);
		AddCvarFloat(str, &cvar.legit[i].humanize); legit[i].humanize = 0.0;
		sprintf(str, "legit[%d].recoil_compensation_pitch", i);
		AddCvarFloat(str, &cvar.legit[i].recoil_compensation_pitch); legit[i].recoil_compensation_pitch = 0.0;
		sprintf(str, "legit[%d].recoil_compensation_yaw", i);
		AddCvarFloat(str, &cvar.legit[i].recoil_compensation_yaw); legit[i].recoil_compensation_yaw = 0.0;
		sprintf(str, "legit[%d].recoil_compensation_after_shots_fired", i);
		AddCvarFloat(str, &cvar.legit[i].recoil_compensation_after_shots_fired); legit[i].recoil_compensation_after_shots_fired = 0.0;
		sprintf(str, "legit[%d].block_attack_after_kill", i);
		AddCvarFloat(str, &cvar.legit[i].block_attack_after_kill); legit[i].block_attack_after_kill = 0.0;
		sprintf(str, "legit[%d].accuracy", i);
		AddCvarFloat(str, &cvar.legit[i].accuracy); legit[i].accuracy = 0.0;
		sprintf(str, "legit[%d].perfect_silent", i);
		AddCvarFloat(str, &cvar.legit[i].perfect_silent); legit[i].perfect_silent = 0.0;
		sprintf(str, "legit[%d].fov", i);
		AddCvarFloat(str, &cvar.legit[i].fov); legit[i].fov = 5.0;
		sprintf(str, "legit[%d].speed_in_attack", i);
		AddCvarFloat(str, &cvar.legit[i].speed_in_attack); legit[i].speed_in_attack = 100;
		sprintf(str, "legit[%d].trigger_active", i);
		AddCvarFloat(str, &cvar.legit[i].trigger_active); legit[i].trigger_active = 0.0;
		sprintf(str, "legit[%d].trigger_wall", i);
		AddCvarFloat(str, &cvar.legit[i].trigger_wall); legit[i].trigger_wall = 0.0;
		sprintf(str, "legit[%d].trigger_accuracy", i);
		AddCvarFloat(str, &cvar.legit[i].trigger_accuracy); legit[i].trigger_accuracy = 0.0;
		sprintf(str, "legit[%d].trigger_delay_shot", i);
		AddCvarFloat(str, &cvar.legit[i].trigger_delay_shot); legit[i].trigger_delay_shot = 0.0;
		sprintf(str, "legit[%d].trigger_shot_count", i);
		AddCvarFloat(str, &cvar.legit[i].trigger_shot_count); legit[i].trigger_shot_count = 1.0;
		sprintf(str, "legit[%d].trigger_shot_type", i);
		AddCvarFloat(str, &cvar.legit[i].trigger_shot_type); legit[i].trigger_shot_type = 0.0;
		sprintf(str, "legit[%d].trigger_random_max", i);
		AddCvarFloat(str, &cvar.legit[i].trigger_random_max); legit[i].trigger_random_max = 3.0;
	}

	for (unsigned int i = 1; i <= 30; i++)
	{
		if (i == 2 || i == 4 || i == 6 || i == 9 || i == 25 || i == 29) continue;

		char str[256];
		sprintf(str, "rage[%d].rage_delay_shot", i);
		AddCvarFloat(str, &cvar.rage[i].rage_delay_shot); rage[i].rage_delay_shot = 0.0;
		sprintf(str, "rage[%d].rage_shot_count", i);
		AddCvarFloat(str, &cvar.rage[i].rage_shot_count); rage[i].rage_shot_count = 1.0;
		sprintf(str, "rage[%d].rage_shot_type", i);
		AddCvarFloat(str, &cvar.rage[i].rage_shot_type); rage[i].rage_shot_type = 0.0;
		sprintf(str, "rage[%d].rage_random_max", i);
		AddCvarFloat(str, &cvar.rage[i].rage_random_max); rage[i].rage_random_max = 3.0;
	}

	INITCVAR(legit_trigger_only_zoom, 0.0);
	INITCVAR(legit_trigger_team, 0.0);
	INITCVAR(legit_trigger_key, -1.0);
	INITCVAR(legit_trigger_draw_aim, 1.0);

	INITCVAR(legit_team, 0.0);
	INITCVAR(legit_key, -1.0);
	INITCVAR(legit_draw_aim, 1.0);

	INITCVAR(menu_legit_global_section, 0.0);
	INITCVAR(menu_legit_sub_section, 0.0);
	INITCVAR(menu_rage_global_section, 0.0);
	INITCVAR(menu_rage_sub_section, 0.0);

	INITCVAR(knifebot_active, 0.0);
	INITCVAR(knifebot_attack, 1.0);
	INITCVAR(knifebot_attack_distance, 72.0);
	INITCVAR(knifebot_attack2_distance, 64.0);
	INITCVAR(knifebot_silent, 0.0);
	INITCVAR(knifebot_perfect_silent, 0.0);
	INITCVAR(knifebot_fov, 45.0);
	INITCVAR(knifebot_team, 0.0);
	INITCVAR(knifebot_draw_aim, 1.0);

	INITCVAR(bypass_trace_rage, 1.0);
	INITCVAR(bypass_trace_legit, 1.0);
	INITCVAR(bypass_trace_trigger, 1.0);
	INITCVAR(bypass_trace_knife, 1.0);

	INITCVAR(aa_pitch, 0.0);
	INITCVAR(aa_edge, 0.0);
	INITCVAR(aa_yaw, 0.0);
	INITCVAR(aa_yaw_static, 0.0);
	INITCVAR(aa_yaw_while_running, 0.0);

	INITCVAR(fakelag_active, 0.0);
	INITCVAR(fakelag_while_shooting, 0.0);
	INITCVAR(fakelag_move, 2.0);
	INITCVAR(fakelag_type, 2.0);
	INITCVAR(fakelag_limit, 0.0);
	INITCVAR(fakelag_variance, 0.0);

	INITCVAR(snapshot, 0.0);
	INITCVAR(snapshot_time, 10.0);

	INITCVAR(misc_quick_change, 0.0);
	INITCVAR(misc_quick_change_key, -1.0);
	INITCVAR(misc_wav_speed, 1.0);

	INITCVAR(chams_view_model, 0.0);
	INITCVAR(chams_view_model_glow, 0.0);
	INITCVAR(chams_player, 0.0);
	INITCVAR(chams_player_glow, 0.0);
	INITCVAR(chams_player_wall, 0.0);
	INITCVAR(chams_world, 0.0);
	INITCVAR(chams_world_wall, 0.0);
	INITCVAR(chams_world_glow, 0.0);

	INITCVAR(kz_strafe_psilent, 0.0);
	INITCVAR(kz_strafe, 0.0);
	INITCVAR(kz_fast_run, 0.0);
	INITCVAR(kz_ground_strafe, 0.0);
	INITCVAR(kz_bhop, 0.0);
	INITCVAR(kz_bhop_double, 0.0);
	INITCVAR(kz_bhop_triple, 0.0);
	INITCVAR(kz_jump_bug, 0.0);
	INITCVAR(kz_jump_bug_auto, 0.0);
	INITCVAR(kz_show_kz, 0.0);
	INITCVAR(kz_display_time, 10.0);
	INITCVAR(kz_strafe_key, -1.0);
	INITCVAR(kz_fastrun_key, -1.0);
	INITCVAR(kz_ground_strafe_key, -1.0);
	INITCVAR(kz_bhop_key, -1.0);
	INITCVAR(kz_jumpbug_key, -1.0);

	INITCVAR(route_activate, 0.0);
	INITCVAR(route_mode, 1.0);
	INITCVAR(route_auto, 0.0);
	INITCVAR(route_jump, 1.0);
	INITCVAR(route_jump_step, 2.0);
	INITCVAR(route_direction, 1.0);
	INITCVAR(route_direction_step, 4.0);
	INITCVAR(route_draw_visual, 0.0);
	INITCVAR(route_draw, 1.0);
	INITCVAR(route_rush_key, -1.0);

	INITCVAR(visual_model_hitbox, 0.0);
	INITCVAR(visual_name, 1.0);
	INITCVAR(visual_model, 0.0);
	INITCVAR(visual_weapon, 0.0);
	INITCVAR(visual_reload_bar, 0.0);
	INITCVAR(visual_box, 0.0);
	INITCVAR(visual_health, 0.0);
	INITCVAR(visual_visual_team, 1.0);
	INITCVAR(visual_sound_index, 1.0);
	INITCVAR(visual_sound_no_index, 1.0);
	INITCVAR(visual_sound_steps, 0.0);
	INITCVAR(visual_idhook_only, 0.0);
	INITCVAR(visual_rem_flash, 0.0);
	INITCVAR(visual_chase_cam, 0.0);
	INITCVAR(visual_chase_back, 100.0);
	INITCVAR(visual_chase_up, 16.0);
	INITCVAR(visual_grenade_trajectory, 0.0);
	INITCVAR(visual_crosshair, 0.0);
	INITCVAR(visual_vip, 0.0);
	INITCVAR(visual_custom_fov, 90.0);
	INITCVAR(visual_wall, 0.0);
	INITCVAR(visual_lambert, 0.0);
	INITCVAR(visual_weapon_local, 0.0);
	INITCVAR(visual_lightmap, 0.0);
	INITCVAR(visual_sky, 0.0);
	INITCVAR(visual_skins_player, 0.0);
	INITCVAR(visual_skins_player_weapon, 0.0);
	INITCVAR(visual_skins_player_backweapon, 0.0);
	INITCVAR(visual_skins_backpack, 0.0);
	INITCVAR(visual_skins_thighpack, 0.0);
	INITCVAR(visual_skins_viewmodel, 0.0);
	INITCVAR(visual_skins_viewmodel_hands, 0.0);
	INITCVAR(visual_skins_viewmodel_nohands, 0.0);
	INITCVAR(visual_skins_world, 0.0);
	INITCVAR(visual_skins_bullet_shell, 0.0);
	INITCVAR(visual_skins_wall, 0.0);
	INITCVAR(visual_skins_chicken, 0.0);
	INITCVAR(visual_spawn_points, 0.0);
	INITCVAR(visual_spawn_scan, 0.0);
	INITCVAR(visual_spawn_num, 0.0);
	INITCVAR(visual_viewmodel_fov, 0.0);

	INITCVAR(skeleton_player_bone, 0.0);
	INITCVAR(skeleton_player_hitbox, 0.0);
	INITCVAR(skeleton_view_model_bone, 0.0);
	INITCVAR(skeleton_view_model_hitbox, 0.0);
	INITCVAR(skeleton_world_bone, 0.0);
	INITCVAR(skeleton_world_hitbox, 0.0);

	INITCVAR(gui_key, -1.0);
	INITCVAR(gui_chat, 0.0);
	INITCVAR(gui_chat_key, -1.0);
	INITCVAR(gui_chat_key_team, -1.0);

	INITCVAR(radio_kill_sound, 0.0);
	INITCVAR(radio_kill_volume, 15.0);

	INITCVAR(model_scan, 0.0);

	INITCVAR(afk_anti, 0.0);
	INITCVAR(afk_time, 89.0);

	INITCVAR(steamid, 0.0); 
	INITCVAR(id1, 1.0);
	INITCVAR(id2, 2.0);
	INITCVAR(id3, 3.0);
	INITCVAR(id4, 4.0);
	INITCVAR(id5, 5.0);
	INITCVAR(id6, 6.0);
	INITCVAR(id7, 7.0);

	INITCVAR(radar, 0.0);
	INITCVAR(radar_zoom, 5.0);
	INITCVAR(radar_point_size, 5.0);
	INITCVAR(radar_pos_x, 100.0);
	INITCVAR(radar_pos_y, 100.0);
	INITCVAR(radar_size_x, 150.0);
	INITCVAR(radar_size_y, 150.0);
}

void SaveCvar()
{
	ofstream ofs(getHackDirFile("cvar.ini").c_str());
	save_cvars(ofs);
	ofs.close();
}

void LoadCvar()
{
	cvar.init();
	if (fileExists(getHackDirFile("cvar.ini").c_str()))
		load_cvars();
}