#include "client.h"

IdHook idhook;

void IdHook::First_Kill_Mode()
{
	if (cvar.aim_id_mode == 0)
		cvar.aim_id_mode = 1;
	else if (cvar.aim_id_mode == 1)
		cvar.aim_id_mode = 2;
	else
		cvar.aim_id_mode = 0;
}

void IdHook::AddPlayer(int i)
{
	if (FirstKillPlayer[i] == 0)
		FirstKillPlayer[i] = 1;
	else if (FirstKillPlayer[i] == 1)
		FirstKillPlayer[i] = 2;
	else
		FirstKillPlayer[i] = 0;
}

void IdHook::ClearPlayer()
{
	for (unsigned int i = 0; i < 33; i++)
		FirstKillPlayer[i] = 0;
}