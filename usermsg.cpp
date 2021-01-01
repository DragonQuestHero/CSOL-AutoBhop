#include "client.h"

PUserMsg pUserMsgBase;
pfnUserMsgHook pResetHUD;
pfnUserMsgHook pTeamInfo;
pfnUserMsgHook pDeathMsg;
pfnUserMsgHook pSetFOV;
pfnUserMsgHook pScoreAttrib;
pfnUserMsgHook pServerName;
pfnUserMsgHook pSayText;

int ServerName(const char* pszName, int iSize, void* pbuf)
{
	BEGIN_READ(pbuf, iSize);
	char* m_szServerName = READ_STRING();
	sprintf(sServerName, "%s", m_szServerName);
	return pServerName(pszName, iSize, pbuf);
}

int ScoreAttrib(const char* pszName, int iSize, void* pbuf)
{
	BEGIN_READ(pbuf, iSize);
	int id = READ_BYTE();
	int info = READ_BYTE();
	g_Player[id].bVip = (info & (1 << 2));
	g_Player[id].bAliveInScoreTab = !(info & (1 << 0));
	return pScoreAttrib(pszName, iSize, pbuf);
}

int SetFOV(const char *pszName, int iSize, void *pbuf)
{
	BEGIN_READ(pbuf, iSize);
	int iFOV = READ_BYTE();
	g_Local.iFOV = iFOV;
	return pSetFOV(pszName, iSize, pbuf);
}

int ResetHUD(const char *pszName, int iSize, void *pbuf)
{
	static char currentMap[100];
	if (strcmp(currentMap, g_Engine.pfnGetLevelName())) 
	{
		strcpy(currentMap, g_Engine.pfnGetLevelName());
		LoadOverview((char*)getfilename(g_Engine.pfnGetLevelName()).c_str());
	}
	RunHLCommands();
	ContinueRoute(); 
	ResetSpawn();
	Sound_No_Index.deque::clear();
	Sound_Index.deque::clear();
	for (unsigned int i = 0; i < 33; i++)
		g_Player[i].iHealth = 100;
	return pResetHUD(pszName, iSize, pbuf);
}

int DeathMsg(const char *pszName, int iSize, void *pbuf)
{
	BEGIN_READ(pbuf, iSize);
	int killer = READ_BYTE();
	int victim = READ_BYTE();
	int headshot = READ_BYTE();

	if (killer != victim && killer == pmove->player_index + 1 && victim > 0 && victim <= g_Engine.GetMaxClients())
		dwReactionTime = GetTickCount();

	if (victim != pmove->player_index + 1 && victim > 0 && victim <= g_Engine.GetMaxClients())
		g_Player[victim].iHealth = 100;
	
	cl_entity_s* ent = g_Engine.GetEntityByIndex(victim);
	player_info_s* player = g_Studio.PlayerInfo(victim - 1);
	if (player && lstrlenA(player->name) > 0 && ent && victim > 0 && victim <= g_Engine.GetMaxClients() && cvar.visual_spawn_scan)
	{
		spawndeath_t Spawns;
		Spawns.index = ent->index;
		Spawns.Origin = ent->origin;
		Spawns.Tickcount = GetTickCount();
		strcpy(Spawns.name, player->name);
		SpawnDeath.push_back(Spawns);
	}
	KillSound(victim, killer, headshot);

	return pDeathMsg(pszName, iSize, pbuf);
}

int TeamInfo(const char *pszName, int iSize, void *pbuf)
{
	BEGIN_READ(pbuf, iSize);
	int id = READ_BYTE();
	char *szTeam = READ_STRING();
	if (id > 0 && id <= g_Engine.GetMaxClients())
	{
		if (!lstrcmpA(szTeam, "TERRORIST"))
		{
			g_Player[id].iTeam = 1;
			if (id == pmove->player_index + 1) { g_Local.iTeam = 1; }
		}
		else if (!lstrcmpA(szTeam, "CT"))
		{
			g_Player[id].iTeam = 2;
			if (id == pmove->player_index + 1) { g_Local.iTeam = 2; }
		}
		else
		{
			g_Player[id].iTeam = 0;
			if (id == pmove->player_index + 1) { g_Local.iTeam = 0; }
		}
	}
	return pTeamInfo(pszName, iSize, pbuf);
}

PUserMsg UserMsgByName(char* szMsgName)
{
	PUserMsg Ptr = NULL;
	Ptr = pUserMsgBase;
	while (Ptr->next)
	{
		if (!strcmp(Ptr->name, szMsgName))
			return Ptr;
		Ptr = Ptr->next;
	}
	Ptr->pfn = 0;
	return Ptr;
}

pfnUserMsgHook HookUserMsg(char* szMsgName, pfnUserMsgHook pfn)
{
	PUserMsg Ptr = NULL;
	pfnUserMsgHook Original = NULL;
	Ptr = UserMsgByName(szMsgName);
	if (Ptr->pfn != 0) {
		Original = Ptr->pfn;
		Ptr->pfn = pfn;
		return Original;
	}
	else
	{
		char Message[256];
		strcpy(Message, "ERROR: Couldn't find ");
		strcat(Message, szMsgName);
		strcat(Message, " message.");
		c_Offset.Error(Message);
	}
}

void HookUserMessages()
{
#define HOOK_MSG(n) \
	p##n = HookUserMsg(#n, ##n);

	HOOK_MSG(ResetHUD);
	HOOK_MSG(TeamInfo);
	HOOK_MSG(DeathMsg);
	HOOK_MSG(SetFOV);
	HOOK_MSG(ScoreAttrib);
	HOOK_MSG(ServerName);
}