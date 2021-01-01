#ifndef _CVARS_
#define _CVARS_

class CVARlist
{
public:
	void init();

	float rage_active;
	float rage_wall;
	float rage_fov;
	float rage_target_selection;
	float rage_perfect_silent;
	float rage_silent;
	float rage_knife_attack;
	float rage_shield_attack;
	float rage_nospread_method;
	float rage_auto_fire;
	float rage_auto_fire_key;
	float rage_team;
	float rage_always_fire;
	float rage_draw_aim;

	float aim_id_mode;

	struct legit_weapons
	{
		float drawfov;
		float active;
		float speed;
		float speed_scale_fov;
		float reaction_time;
		float humanize;
		float recoil_compensation_pitch;
		float recoil_compensation_yaw;
		float recoil_compensation_after_shots_fired;
		float block_attack_after_kill;
		float accuracy;
		float perfect_silent;
		float fov;
		float speed_in_attack;

		float trigger_active;
		float trigger_wall;
		float trigger_accuracy;
		float trigger_delay_shot;
		float trigger_shot_count;
		float trigger_shot_type;
		float trigger_random_max;
	} legit[31];

	struct rage_weapons
	{
		float rage_delay_shot;
		float rage_shot_count;
		float rage_shot_type;
		float rage_random_max;
	} rage[31];

	float legit_trigger_only_zoom;
	float legit_trigger_team;
	float legit_trigger_key;
	float legit_trigger_draw_aim;

	float legit_key;
	float legit_team;
	float legit_draw_aim;

	float menu_legit_global_section;
	float menu_legit_sub_section;
	float menu_rage_global_section;
	float menu_rage_sub_section;

	float knifebot_active;
	float knifebot_attack;
	float knifebot_attack_distance;
	float knifebot_attack2_distance;
	float knifebot_silent;
	float knifebot_perfect_silent;
	float knifebot_fov;
	float knifebot_team;
	float knifebot_draw_aim;

	float bypass_trace_rage;
	float bypass_trace_legit;
	float bypass_trace_trigger;
	float bypass_trace_knife;

	float aa_pitch;
	float aa_edge;
	float aa_yaw;
	float aa_yaw_static;
	float aa_yaw_while_running;

	float fakelag_active;
	float fakelag_while_shooting;
	float fakelag_move;
	float fakelag_type;
	float fakelag_limit;
	float fakelag_variance;

	float snapshot;
	float snapshot_time;

	float misc_quick_change;
	float misc_quick_change_key;
	float misc_wav_speed;

	float chams_view_model;
	float chams_view_model_glow;
	float chams_player;
	float chams_player_glow;
	float chams_player_wall;
	float chams_world;
	float chams_world_wall;
	float chams_world_glow;

	float kz_strafe;
	float kz_strafe_psilent;
	float kz_fast_run;
	float kz_ground_strafe;
	float kz_bhop;
	float kz_bhop_double;
	float kz_bhop_triple;
	float kz_jump_bug;
	float kz_jump_bug_auto;
	float kz_show_kz;
	float kz_display_time;
	float kz_strafe_key;
	float kz_fastrun_key;
	float kz_ground_strafe_key;
	float kz_bhop_key;
	float kz_jumpbug_key;

	float route_mode;
	float route_auto;
	float route_jump;
	float route_jump_step;
	float route_direction;
	float route_direction_step;
	float route_draw_visual;
	float route_draw;
	float route_rush_key;
	float route_activate;

	float visual_model_hitbox;
	float visual_name;
	float visual_model;
	float visual_weapon;
	float visual_reload_bar;
	float visual_box;
	float visual_health;
	float visual_visual_team;
	float visual_sound_index;
	float visual_sound_no_index;
	float visual_sound_steps;
	float visual_idhook_only;
	float visual_rem_flash;
	float visual_chase_cam;
	float visual_chase_back;
	float visual_chase_up;
	float visual_grenade_trajectory;
	float visual_crosshair;
	float visual_vip;
	float visual_custom_fov;
	float visual_wall;
	float visual_lambert;
	float visual_weapon_local;
	float visual_lightmap;
	float visual_sky;
	float visual_skins_player;
	float visual_skins_player_weapon;
	float visual_skins_player_backweapon;
	float visual_skins_backpack;
	float visual_skins_thighpack;
	float visual_skins_viewmodel;
	float visual_skins_viewmodel_hands;
	float visual_skins_viewmodel_nohands;
	float visual_skins_world;
	float visual_skins_bullet_shell;
	float visual_skins_wall;
	float visual_skins_chicken;
	float visual_spawn_points;
	float visual_spawn_scan;
	float visual_spawn_num;
	float visual_viewmodel_fov;

	float skeleton_player_bone;
	float skeleton_player_hitbox;
	float skeleton_view_model_bone;
	float skeleton_view_model_hitbox;
	float skeleton_world_bone;
	float skeleton_world_hitbox;

	float gui_key;
	float gui_chat;
	float gui_chat_key;
	float gui_chat_key_team;

	float radio_kill_sound;
	float radio_kill_volume;

	float model_scan;

	float afk_anti;
	float afk_time;

	float steamid;
	float id1;
	float id2;
	float id3;
	float id4;
	float id5;
	float id6;
	float id7;

	float radar;
	float radar_zoom;
	float radar_point_size;
	float radar_pos_x;
	float radar_pos_y;
	float radar_size_x;
	float radar_size_y;
};

extern CVARlist cvar;

struct Entry
{
	void* data;
};

void SaveCvar();
void LoadCvar();

#endif