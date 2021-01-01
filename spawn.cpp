#include "client.h"

deque<spawnorigin_t> SpawnOrigin;
deque<spawndeath_t> SpawnDeath;

void ResetSpawn()
{
	static char CurrentMap[256];
	if (strcmp(CurrentMap, g_Engine.pfnGetLevelName()))
	{
		SpawnOrigin.deque::clear();
		LoadSpawn();
		strcpy(CurrentMap, g_Engine.pfnGetLevelName());
	}
}
		
void FindSpawn()
{
	if (SpawnDeath.size() && GetTickCount() - SpawnDeath.front().Tickcount > 1500)
		SpawnDeath.pop_front();

	if (cvar.visual_spawn_scan && cvar.visual_spawn_points)
	{
		for (unsigned int i = 0; i < SpawnDeath.size(); i++)
		{
			cl_entity_s* entlocal = g_Engine.GetEntityByIndex(pmove->player_index + 1);
			cl_entity_s* ent = g_Engine.GetEntityByIndex(SpawnDeath[i].index);
			if (!ent || !ent->player || !ent->origin || !entlocal)
				continue;
			if (ent->curstate.messagenum < entlocal->curstate.messagenum)
				continue;
			if (!g_Player[ent->index].bAliveInScoreTab)
				continue;
			if (ent->origin == SpawnDeath[i].Origin)
				continue;
			if (Cstrike_SequenceInfo[ent->curstate.sequence] == SEQUENCE_DIE)
				continue;
			if (ent->index == pmove->player_index + 1)
				continue;
			
			player_info_s* player = g_Studio.PlayerInfo(ent->index - 1);
			if (!player || !(lstrlenA(player->name) > 0))
				continue;
			if (!strstr(SpawnDeath[i].name, player->name))
				continue;
			if (g_Player[ent->index].iTeam == 0)
				continue;
			if (ent->curstate.mins.IsZero() || ent->curstate.maxs.IsZero())
				continue;

			Vector vTraceOrigin = ent->origin;
			vTraceOrigin.z -= 8192;
			pmtrace_t tr;
			g_Engine.pEventAPI->EV_SetTraceHull(2);
			g_Engine.pEventAPI->EV_PlayerTrace(ent->origin, vTraceOrigin, PM_GLASS_IGNORE | PM_STUDIO_BOX, -1, &tr);

			bool bBadOrigin = false;
			for (spawnorigin_t SpawnOrigins : SpawnOrigin)
			{
				float distance = SpawnOrigins.Origin.Distance(tr.endpos);
				if (tr.endpos.x == SpawnOrigins.Origin.x && tr.endpos.y == SpawnOrigins.Origin.y)
				{
					bBadOrigin = true;
					break;
				}
				else if (distance < 10)
				{
					bBadOrigin = true;
					break;
				}
			}
			if (!bBadOrigin)
			{
				spawnorigin_t SpawnOrigins;
				SpawnOrigins.Origin.x = tr.endpos.x;
				SpawnOrigins.Origin.y = tr.endpos.y;
				SpawnOrigins.Origin.z = tr.endpos.z;
				SpawnOrigin.push_back(SpawnOrigins);
			}
			SpawnDeath.erase(SpawnDeath.begin() + i);
		}
	}
}

void DrawSpawn()
{
	if (cvar.visual_spawn_points)
	{
		for (unsigned int i = 0; i < SpawnOrigin.size(); i++)
		{
			Vector vTop = SpawnOrigin[i].Origin;
			vTop[2] += 32;
			float SpawnFrom[2], SpawnTo[2];
			if (WorldToScreen(SpawnOrigin[i].Origin, SpawnFrom) && WorldToScreen(vTop, SpawnTo))
			{
				ImGui::GetCurrentWindow()->DrawList->AddLine({ IM_ROUND(SpawnFrom[0]), IM_ROUND(SpawnFrom[1]) }, { IM_ROUND(SpawnTo[0]), IM_ROUND(SpawnTo[1]) }, Green());
				ImGui::GetCurrentWindow()->DrawList->AddCircleFilled({ IM_ROUND(SpawnTo[0]), IM_ROUND(SpawnTo[1]) }, 1 + IM_ROUND((IM_ROUND(SpawnFrom[1]) - IM_ROUND(SpawnTo[1])) / 10), Wheel3());
				if (cvar.visual_spawn_num)
				{
					char str[32];
					sprintf(str, "%d", i);
					float label_size = IM_ROUND(ImGui::CalcTextSize(str, NULL, true).x / 2);
					ImGui::GetCurrentWindow()->DrawList->AddRect({ IM_ROUND(SpawnTo[0]) - label_size - 2, IM_ROUND(SpawnTo[1]) - 15 - IM_ROUND((IM_ROUND(SpawnFrom[1]) - IM_ROUND(SpawnTo[1])) / 10) }, { IM_ROUND(SpawnTo[0]) + label_size + 3, IM_ROUND(SpawnTo[1]) - 1 - IM_ROUND((IM_ROUND(SpawnFrom[1]) - IM_ROUND(SpawnTo[1])) / 10) }, Wheel1());
					ImGui::GetCurrentWindow()->DrawList->AddText({ IM_ROUND(SpawnTo[0]) - label_size, IM_ROUND(SpawnTo[1]) - 16 - IM_ROUND((IM_ROUND(SpawnFrom[1]) - IM_ROUND(SpawnTo[1])) / 10) }, White(), str);
				}
			}
		}
	}
}

void SaveSpawn()
{
	char filename[256], levelname[256], tfilename[256];
	strcpy(levelname, g_Engine.pfnGetLevelName() + 5);
	int len = strlen(levelname);
	if (len > 4)levelname[len - 4] = (char)0;
	sprintf(tfilename, "spawn/%s.point", levelname);
	sprintf(filename, "%s%s", hackdir, tfilename);
	remove(filename);
	char text[256];
	ofstream ofs(filename, ios::binary | ios::app);
	for (spawnorigin_t Spawn : SpawnOrigin)
	{
		sprintf(text, "Spawn: %f,%f,%f", Spawn.Origin[0], Spawn.Origin[1], Spawn.Origin[2]);
		ofs << text << (char)0x0D << (char)0x0A;
	}
	ofs.close();
}

void LoadSpawn()
{
	char filename[256], levelname[256], tfilename[256];
	strcpy(levelname, g_Engine.pfnGetLevelName() + 5);
	int len = strlen(levelname);
	if (len > 4)levelname[len - 4] = (char)0;
	sprintf(tfilename, "spawn/%s.point", levelname);
	sprintf(filename, "%s%s", hackdir, tfilename);
	ifstream ifs(filename);
	while (ifs.good())
	{
		char buf[1024];
		ifs.getline(buf, sizeof(buf));
		float x, y, z;
		if (sscanf(buf, "Spawn: %f,%f,%f", &x, &y, &z))
		{
			if (x != 0 && y != 0 && z != 0)
			{
				Vector origin;
				origin.x = x;
				origin.y = y;
				origin.z = z;
				bool bBadOrigin = false;
				for (spawnorigin_t Spawns : SpawnOrigin)
				{
					float distance = Spawns.Origin.Distance(origin);
					if (x == Spawns.Origin[0] && y == Spawns.Origin[1])
					{
						bBadOrigin = true;
						break;
					}
					else if (distance < 10)
					{
						bBadOrigin = true;
						break;
					}
				}
				if (!bBadOrigin)
				{
					spawnorigin_t Spawn;
					Spawn.Origin = origin;
					SpawnOrigin.push_back(Spawn);
				}
			}
		}
	}
	ifs.close();
}