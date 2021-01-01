#include "client.h"

PlayerInfoLocal g_Local;
PlayerInfo g_Player[33];

bool bAlive(cl_entity_s* ent)
{
	if (ent->index != pmove->player_index + 1 &&
		!ent->curstate.mins.IsZero() &&
		!ent->curstate.maxs.IsZero() &&
		g_Player[ent->index].bAliveInScoreTab &&
		!(g_Engine.GetEntityByIndex(pmove->player_index + 1)->curstate.iuser1 == OBS_IN_EYE && g_Engine.GetEntityByIndex(pmove->player_index + 1)->curstate.iuser2 == ent->index))
		return true;
	return false;
}

bool bAliveLocal()
{
	cl_entity_s* ent = g_Engine.GetEntityByIndex(pmove->player_index + 1);
	if (ent && 
		ent->player &&
		g_Local.iTeam != 0 &&
		!ent->curstate.mins.IsZero() &&
		!ent->curstate.maxs.IsZero() &&
		ent->curstate.iuser1 == OBS_NONE &&
		pmove->view_ofs[2] != PM_DEAD_VIEWHEIGHT &&
		g_Local.iPrevHealth > 0)
		return true;
	return false;
}

void ThirdPerson(struct ref_params_s* pparams)
{
	if (cvar.visual_chase_cam && bAliveLocal() && DrawVisuals && (!cvar.route_auto || cvar.route_draw_visual) && GetTickCount() - HudRedraw <= 100)
	{
		Vector Offset(0, 0, 0);
		Vector r, u, b;
		VectorMul(pparams->right, 0, r);
		VectorMul(pparams->up, cvar.visual_chase_up, u);
		VectorMul(pparams->forward, -cvar.visual_chase_back, b);
		Offset = Offset + r;
		Offset = Offset + u;
		Offset = Offset + b;
		pparams->vieworg[0] += Offset[0];
		pparams->vieworg[1] += Offset[1];
		pparams->vieworg[2] += Offset[2];
	}
}

unsigned int Cstrike_SequenceInfo[] =
{
	0,	0,	0,	0,	0,	0,	0,	0,	0,	0, // 0..9   
	0,	1,	2,	0,	1,	2,	0,	1,	2,	0, // 10..19 
	1,	2,	0,	1,	1,	2,	0,	1,	1,	2, // 20..29 
	0,	1,	2,	0,	1,	2,	0,	1,	2,	0, // 30..39 
	1,	2,	0,	1,	2,	0,	1,	2,	0,	1, // 40..49 
	2,	0,	1,	2,	0,	0,	0,	8,	0,	8, // 50..59 
	0, 16,	0, 16,	0,	0,	1,	1,	2,	0, // 60..69 
	1,	1,	2,	0,	1,	0,	1,	0,	1,	2, // 70..79 
	0,	1,	2, 	32, 40, 32, 40, 32, 32, 32, // 80..89
	33, 64, 33, 34, 64, 65, 34, 32, 32, 4, // 90..99
	4,	4,	4,	4,	4,	4,	4,	4,	4,	4, // 100..109
	4                                      	// 110
};

void RunHLCommands()
{
	static bool run = true;
	if (run)
	{
		g_Engine.pfnClientCmd("alias get_b67 \"primammo;secammo;vesthelm;sgren;hegren;flash\"");
		g_Engine.pfnClientCmd("alias +rrr \"+reload;get_b67\"");
		g_Engine.pfnClientCmd("alias -rrr \"-reload\"");
		g_Engine.pfnClientCmd("bind r \"+rrr\"");
		g_Engine.pfnClientCmd("cl_showfps 1");
		g_Engine.pfnClientCmd("cl_minmodels 0");
		run = false;
	}
}

void LogToFile(const char* fmt, ...)
{
	va_list		va_alist;
	char		buf[256];
	char		logbuf[1024];
	char		cDirectory[600];
	FILE* file;
	struct tm* current_tm;
	time_t		current_time;

	time(&current_time);
	current_tm = localtime(&current_time);
	sprintf(logbuf, "%02d:%02d:%02d: ", current_tm->tm_hour, current_tm->tm_min, current_tm->tm_sec);
	va_start(va_alist, fmt);
	vsprintf(buf, fmt, va_alist);
	va_end(va_alist);

	strcat(logbuf, buf);
	strcat(logbuf, "\n");
	strcpy(cDirectory, hackdir);
	strcat(cDirectory, "console.log");

	if ((file = fopen(cDirectory, "a+")) != NULL)
	{
		fputs(logbuf, file);
		fclose(file);
	}
}

