#include "client.h"

deque<player_sound_no_index_t> Sound_No_Index;
deque<player_sound_index_t> Sound_Index;

void PreS_DynamicSound(int entid, DWORD entchannel, char* szSoundFile, float* fOrigin, float fVolume, float fAttenuation, int iTimeOff, int iPitch)
{
	if (szSoundFile && fOrigin)
	{
		if (!strstr(szSoundFile, "pl_shell") && !strstr(szSoundFile, "ric") && !strstr(szSoundFile, "die") && !strstr(szSoundFile, "glass") && !strstr(szSoundFile, "debris") && !strstr(szSoundFile, "death") && strstr(szSoundFile, "player"))
		{
			if (entid > 0 && entid < 33)
			{
				if (strstr(szSoundFile, "bhit_helmet"))
					g_Player[entid].iHealth -= 80;
				else if (strstr(szSoundFile, "bhit_kevlar"))
					g_Player[entid].iHealth -= 20;
				else if (strstr(szSoundFile, "bhit_flesh"))
					g_Player[entid].iHealth -= 30;
				else if (strstr(szSoundFile, "headshot"))
					g_Player[entid].iHealth -= 80;
				else if (strstr(szSoundFile, "die") || strstr(szSoundFile, "death"))
					g_Player[entid].iHealth = 100;
				if (cvar.visual_sound_index)
				{
					player_sound_index_t sound_index;
					sound_index.index = entid;
					sound_index.origin = fOrigin;
					sound_index.timestamp = GetTickCount();
					Sound_Index.push_back(sound_index);
				}
			}
			else if (cvar.visual_sound_no_index)
			{
				player_sound_no_index_t sound_no_index;
				sound_no_index.origin = fOrigin;
				sound_no_index.timestamp = GetTickCount();
				Sound_No_Index.push_back(sound_no_index);
			}
		}
	}
	PreS_DynamicSound_s(entid, entchannel, szSoundFile, fOrigin, fVolume, fAttenuation, iTimeOff, iPitch);
}