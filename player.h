#ifndef _PLAYERS_
#define _PLAYERS_

class PlayerInfoLocal
{
public:
	int iFOV;
	int iTeam;
	int iPrevHealth;
	int iPostHealth;
	Vector vPrevForward;
	Vector vPostForward;
	Vector vNoSpreadAngle;
	Vector vNoRecoilAngle;
	Vector vPunchangle;
	CBasePlayerWeapon weapon;
};
extern PlayerInfoLocal g_Local;

class PlayerInfo
{
public:
	bool bAliveInScoreTab;
	bool bVip;
	int iHealth;
	int iTeam;
};
extern PlayerInfo g_Player[33];

unsigned int Cstrike_SequenceInfo[];
void ThirdPerson(struct ref_params_s* pparams);
void RunHLCommands();
bool bAlive(cl_entity_s* ent);
bool bAliveLocal();
void LogToFile(const char* fmt, ...);

#endif