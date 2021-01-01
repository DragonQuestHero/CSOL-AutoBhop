#include "client.h"

deque<playeresp_t> PlayerEsp;

void Box(float x, float y, float w, float h, ImU32 team)
{
	if (!cvar.visual_box) return;
	ImGui::GetCurrentWindow()->DrawList->AddRect({ x, y }, { x + w, y + h }, team);
}

void Health(int id, float x, float y, float h)
{
	if (!cvar.visual_health) return;
	int hp = g_Player[id].iHealth;
	if (hp < 10) hp = 10;
	else if (hp > 100) hp = 100;

	for (unsigned int i = 0; i < 10; i++)
	{
		if (hp > 99 - (10 * i))
			ImGui::GetCurrentWindow()->DrawList->AddRect({ x - 7, y + h / 100.f * 10.f * i }, { x - 1, y + h / 100.f * 10.f * (i + 1) }, ImColor(0.1f * (i + 1), 1.f - (0.1f * i), 0.0f, 1.0f));
	}
}

void Vip(int id, float x, float y, float w)
{
	if (!cvar.visual_vip || !g_Player[id].bVip) return;
	ImGui::GetCurrentWindow()->DrawList->AddImage((GLuint*)texture_id[VIP], { x, y - w }, { x + w, y });
}

bool Reload(cl_entity_s* ent, float x, float y, ImU32 team, ImU32 green)
{
	int seqinfo = Cstrike_SequenceInfo[ent->curstate.sequence];
	if (!cvar.visual_reload_bar || seqinfo != 2) return false;
	float label_size = IM_ROUND(ImGui::CalcTextSize("Reloading", NULL, true).x / 2);
	ImGui::GetCurrentWindow()->DrawList->AddRect({ x - label_size - 2, y - 15 }, { x + label_size + 3 , y - 1 }, team);
	ImGui::GetCurrentWindow()->DrawList->AddText({ x - label_size, y - 16 }, green, "Reloading");
	return true;
}

bool Name(int id, float x, float y, ImU32 team, ImU32 white)
{
	if (!cvar.visual_name) return false;
	player_info_s* player = g_Studio.PlayerInfo(id - 1);
	if (!player || !(lstrlenA(player->name) > 0)) return false;
	float label_size = IM_ROUND(ImGui::CalcTextSize(player->name, NULL, true).x / 2);
	ImGui::GetCurrentWindow()->DrawList->AddRect({ x - label_size - 2, y - 15 }, { x + label_size + 3 , y - 1 }, team);
	ImGui::GetCurrentWindow()->DrawList->AddText({ x - label_size, y - 16 }, white, player->name);
	return true;
}

bool Model(int id, float x, float y, ImU32 team, ImU32 white)
{
	if (!cvar.visual_model) return false;
	player_info_s* player = g_Studio.PlayerInfo(id - 1);
	if (!player || !(lstrlenA(player->model) > 0)) return false;
	float label_size = IM_ROUND(ImGui::CalcTextSize(player->model, NULL, true).x / 2);
	ImGui::GetCurrentWindow()->DrawList->AddRect({ x - label_size - 2, y - 15 }, { x + label_size + 3 , y - 1 }, team);
	ImGui::GetCurrentWindow()->DrawList->AddText({ x - label_size, y - 16 }, white, player->model);
	return true;
}

bool Weapon(cl_entity_s* ent, float x, float y, ImU32 team, ImU32 white)
{
	model_s* mdl = g_Studio.GetModelByIndex(ent->curstate.weaponmodel);
	if (!cvar.visual_weapon || !mdl) return false;
	char weapon[256];
	sprintf(weapon, getfilename(mdl->name).c_str() + 2);
	float label_size = IM_ROUND(ImGui::CalcTextSize(weapon, NULL, true).x / 2);
	ImGui::GetCurrentWindow()->DrawList->AddRect({ x - label_size - 2, y - 15 }, { x + label_size + 3 , y - 1 }, team);
	ImGui::GetCurrentWindow()->DrawList->AddText({ x - label_size, y - 16 }, white, weapon);
	return true;
}

bool bCalcScreen(playeresp_t Esp, float& x, float& y, float& w, float& h, float& xo)
{
	float vOrigin[2];
	if (!WorldToScreen(Esp.ent->origin, vOrigin)) return false;
	xo = IM_ROUND(vOrigin[0]);
	float x0 = vOrigin[0], x1 = vOrigin[0], y0 = vOrigin[1], y1 = vOrigin[1];
	for (playeresphitbox_t Hitbox : Esp.PlayerEspHitbox)
	{
		for (unsigned int i = 0; i < 8; i++)
		{
			float vHitbox[2];
			if (!WorldToScreen(Hitbox.HitboxMulti[i], vHitbox)) return false;
			x0 = min(x0, vHitbox[0]);
			x1 = max(x1, vHitbox[0]);
			y0 = min(y0, vHitbox[1]);
			y1 = max(y1, vHitbox[1]);
		}
	}
	x = IM_ROUND(x0);
	y = IM_ROUND(y0);
	w = IM_ROUND(x1) - IM_ROUND(x0) + 1;
	h = IM_ROUND(y1) - IM_ROUND(y0) + 1;
	return true;
}

void DrawPlayerEsp()
{
	for (playeresp_t Esp : PlayerEsp)
	{
		if (cvar.visual_idhook_only && idhook.FirstKillPlayer[Esp.ent->index] != 1)
			continue;
		if (!cvar.visual_visual_team && g_Player[Esp.ent->index].iTeam == g_Local.iTeam)
			continue;
		if (!bAlive(Esp.ent))
			continue;
		float x, y, w, h, xo;
		if (bCalcScreen(Esp, x, y, w, h, xo))
		{
			Box(x, y, w, h, Team(Esp.ent->index));
			Health(Esp.ent->index, x, y, h);
			if (Reload(Esp.ent, xo, y, Team(Esp.ent->index), Green()))
				y -= 15;
			if (Name(Esp.ent->index, xo, y, Team(Esp.ent->index), White()))
				y -= 15;
			if (Model(Esp.ent->index, xo, y, Team(Esp.ent->index), White()))
				y -= 15;
			if (Weapon(Esp.ent, xo, y, Team(Esp.ent->index), White()))
				y -= 15;
			Vip(Esp.ent->index, x, y, w);
		}
	}
}

void DrawPlayerSoundIndexEsp()
{
	for (player_sound_index_t sound_index : Sound_Index)
	{
		cl_entity_s* ent = g_Engine.GetEntityByIndex(sound_index.index);
		if (!ent)
			continue;
		if (cvar.visual_idhook_only && idhook.FirstKillPlayer[sound_index.index] != 1)
			continue;
		if (!cvar.visual_visual_team && g_Player[sound_index.index].iTeam == g_Local.iTeam)
			continue;
		if (ent->curstate.messagenum == g_Engine.GetEntityByIndex(pmove->player_index + 1)->curstate.messagenum)
			continue;

		if (cvar.visual_sound_steps)
		{
			float step = M_PI * 2.0f / 15;
			float radius = 13.0f * (1200 - (GetTickCount() - sound_index.timestamp)) / 1200;
			Vector position = Vector(sound_index.origin.x, sound_index.origin.y, sound_index.origin.z - 36);
			for (float i = 0; i < (IM_PI * 2.0f); i += step)
			{
				Vector vPointStart(radius * cosf(i) + position.x, radius * sinf(i) + position.y, position.z);
				Vector vPointEnd(radius * cosf(i + step) + position.x, radius * sinf(i + step) + position.y, position.z);
				float vStart[2], vEnd[2];
				if (WorldToScreen(vPointStart, vStart) && WorldToScreen(vPointEnd, vEnd))
					ImGui::GetCurrentWindow()->DrawList->AddLine({ IM_ROUND(vStart[0]), IM_ROUND(vStart[1]) }, { IM_ROUND(vEnd[0]), IM_ROUND(vEnd[1]) }, Team(sound_index.index));
			}
		}
		if (GetTickCount() - sound_index.timestamp > 300)
			continue;

		Vector vPointTop = Vector(sound_index.origin.x, sound_index.origin.y, sound_index.origin.z + 10);
		Vector vPointBot = Vector(sound_index.origin.x, sound_index.origin.y, sound_index.origin.z - 10);

		float vTop[2], vBot[2];
		if (WorldToScreen(vPointTop, vTop) && WorldToScreen(vPointBot, vBot))
		{
			float h = IM_ROUND(vBot[1]) - IM_ROUND(vTop[1]), w = h, x = IM_ROUND(vTop[0]) - IM_ROUND(w / 2), y = IM_ROUND(vTop[1]), xo = IM_ROUND(vTop[0]);
			Box(x, y, w, h, Team(sound_index.index));
			Health(sound_index.index, x, y, h);
			if (Name(sound_index.index, xo, y, Team(sound_index.index), White()))
				y -= 15;
			Vip(sound_index.index, x, y, w);
		}
	}
}

void DrawPlayerSoundNoIndexEsp()
{
	for (player_sound_no_index_t sound_no_index : Sound_No_Index)
	{
		if (cvar.visual_sound_steps)
		{
			float step = IM_PI * 2.0f / 15;
			float radius = 13.0f * (1200 - (GetTickCount() - sound_no_index.timestamp)) / 1200;
			Vector position = Vector(sound_no_index.origin.x, sound_no_index.origin.y, sound_no_index.origin.z - 36);
			for (float i = 0; i < (IM_PI * 2.0f); i += step)
			{
				Vector vPointStart(radius * cosf(i) + position.x, radius * sinf(i) + position.y, position.z);
				Vector vPointEnd(radius * cosf(i + step) + position.x, radius * sinf(i + step) + position.y, position.z);
				float vStart[2], vEnd[2];
				if (WorldToScreen(vPointStart, vStart) && WorldToScreen(vPointEnd, vEnd))
					ImGui::GetCurrentWindow()->DrawList->AddLine({ IM_ROUND(vStart[0]), IM_ROUND(vStart[1]) }, { IM_ROUND(vEnd[0]), IM_ROUND(vEnd[1]) }, Green());
			}
		}
		if (GetTickCount() - sound_no_index.timestamp > 300)
			continue;

		Vector vPointTop = Vector(sound_no_index.origin.x, sound_no_index.origin.y, sound_no_index.origin.z + 10);
		Vector vPointBot = Vector(sound_no_index.origin.x, sound_no_index.origin.y, sound_no_index.origin.z - 10);

		float vTop[2], vBot[2];
		if (WorldToScreen(vPointTop, vTop) && WorldToScreen(vPointBot, vBot))
		{
			Box(IM_ROUND(vTop[0]) - IM_ROUND((IM_ROUND(vBot[1]) - IM_ROUND(vTop[1])) / 2), IM_ROUND(vTop[1]), IM_ROUND(vBot[1]) - IM_ROUND(vTop[1]), IM_ROUND(vBot[1]) - IM_ROUND(vTop[1]), Green());
		}
	}
}