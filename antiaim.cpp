#include "client.h"

bool FakeEdge(float& angle)
{
	float edge_dist = 128;
	float closest_distance = 100.0f;
	float radius = edge_dist + 0.1f;
	float step = M_PI * 2.0 / 8;

	for (float a = 0; a < (M_PI * 2.0); a += step)
	{
		Vector vEye = pmove->origin + pmove->view_ofs;
		Vector location(radius * cos(a) + vEye.x, radius * sin(a) + vEye.y, vEye.z);

		pmtrace_t tr;

		g_Engine.pEventAPI->EV_SetTraceHull(2);
		g_Engine.pEventAPI->EV_PlayerTrace(vEye, location, PM_GLASS_IGNORE, -1, &tr);

		float distance = vEye.Distance(tr.endpos);

		if (distance < closest_distance)
		{
			closest_distance = distance;
			angle = RAD2DEG(a);
		}
	}
	return closest_distance < edge_dist;
}

void AntiAim(struct usercmd_s* cmd)
{
	if (cvar.rage_active && bAliveLocal())
	{
		int m_Use = (cmd->buttons & IN_USE);
		int m_InAttack = (cmd->buttons & IN_ATTACK);
		int m_InAttack2 = (cmd->buttons & IN_ATTACK2) && IsCurWeaponKnife();

		if (!m_Use && !IsCurWeaponNonAttack() && !(m_InAttack && CanAttack()) && !(m_InAttack2 && CanAttack()))
		{
			FixMoveStart(cmd);

			int target = NULL;
			Vector origin;
			for (playeraim_t Aim : PlayerAim)
			{
				if (Aim.ent->index != iTargetRage)
					continue;

				origin = Aim.ent->origin;
				target = Aim.ent->index;
			}

			Vector vAngles = cmd->viewangles;
			Vector vEye = pmove->origin + pmove->view_ofs;

			if (target > 0) VectorAngles(origin - vEye, vAngles);

			//Yaw
			if (pmove->velocity.Length2D() > 0)
			{
				if (cvar.aa_yaw_while_running > 0)
				{
					if (cvar.aa_yaw_while_running == 1)
					{//180
						cmd->viewangles.y = vAngles[1] + 180;
					}
					else if (cvar.aa_yaw_while_running == 2)
					{//180 Jitter
						static bool jitter = false;

						if (jitter)
							cmd->viewangles.y = vAngles[1] + 180;
						else
							cmd->viewangles.y = vAngles[1];

						jitter = !jitter;
					}
					else if (cvar.aa_yaw_while_running == 3)
					{//Spin
						int spin = 30;
						cmd->viewangles.y = fmod(g_Engine.GetClientTime() * spin * 360.0f, 360.0f);
					}
					else if (cvar.aa_yaw_while_running == 4)
					{//Jitter
						static unsigned int m_side = 0;

						if (m_side == 0)
							cmd->viewangles.y = vAngles[1] + 0;
						else if (m_side == 1)
							cmd->viewangles.y = vAngles[1] + 90;
						else if (m_side == 2)
							cmd->viewangles.y = vAngles[1] + 180;
						else if (m_side >= 3) {
							cmd->viewangles.y = vAngles[1] + -90;
							m_side = 0;
						}

						m_side++;
					}
					else if (cvar.aa_yaw_while_running == 5) {//Sideway
						static bool jitter = false;

						if (jitter)
							cmd->viewangles.y = vAngles[1] + 90;
						else
							cmd->viewangles.y = vAngles[1] - 90;

						jitter = !jitter;
					}
					else if (cvar.aa_yaw_while_running == 6) {//Random
						cmd->viewangles.y = rand() % 361;
						if (cmd->viewangles.y > 180)
							cmd->viewangles.y -= 360;
					}
					else if (cvar.aa_yaw_while_running == 7) {//Static
						cmd->viewangles.y = vAngles[1] + cvar.aa_yaw_static;
					}
				}

				float angle = cmd->viewangles.y;

				if ((cvar.aa_edge == 2 || cvar.aa_edge == 3) && FakeEdge(angle))
					cmd->viewangles.y = angle;
			}
			else
			{
				if (cvar.aa_yaw > 0)
				{
					if (cvar.aa_yaw == 1)
					{//180
						cmd->viewangles.y = vAngles[1] + 180;
					}
					else if (cvar.aa_yaw == 2)
					{//180 Jitter
						static bool jitter = false;

						if (jitter)
							cmd->viewangles.y = vAngles[1] + 180;
						else
							cmd->viewangles.y = vAngles[1];

						jitter = !jitter;
					}
					else if (cvar.aa_yaw == 3)
					{//Spin
						int spin = 30;
						cmd->viewangles.y = fmod(g_Engine.GetClientTime() * spin * 360.0f, 360.0f);
					}
					else if (cvar.aa_yaw == 4)
					{//Jitter
						static unsigned int m_side = 0;

						if (m_side == 0)
							cmd->viewangles.y = vAngles[1] + 0;
						else if (m_side == 1)
							cmd->viewangles.y = vAngles[1] + 90;
						else if (m_side == 2)
							cmd->viewangles.y = vAngles[1] + 180;
						else if (m_side >= 3) {
							cmd->viewangles.y = vAngles[1] + -90;
							m_side = 0;
						}

						m_side++;
					}
					else if (cvar.aa_yaw == 5)
					{//Sideway
						static bool jitter = false;

						if (jitter)
							cmd->viewangles.y = vAngles[1] + 90;
						else
							cmd->viewangles.y = vAngles[1] - 90;

						jitter = !jitter;
					}
					else if (cvar.aa_yaw == 6)
					{//Random
						cmd->viewangles.y = rand() % 361;
						if (cmd->viewangles.y > 180)
							cmd->viewangles.y -= 360;
					}
					else if (cvar.aa_yaw == 7)
					{//Static
						cmd->viewangles.y = vAngles[1] + cvar.aa_yaw_static;
					}
				}

				float angle = cmd->viewangles.y;

				if ((cvar.aa_edge == 1 || cvar.aa_edge == 3 || cvar.aa_edge == 4) && FakeEdge(angle))
				{
					static float timer = g_Local.weapon.curtime;

					if (g_Local.weapon.curtime - timer > 3 && cvar.aa_edge == 4)
					{
						timer = g_Local.weapon.curtime;

						cmd->viewangles.y = angle + 180;
					}
					else {
						cmd->viewangles.y = angle;
					}
				}
			}

			//Pitch
			if (cvar.aa_pitch > 0)
			{
				if (cvar.aa_pitch == 1)
				{//Fakedown
					cmd->viewangles.x = 180;
				}
				if (cvar.aa_pitch == 2)
				{//Random
					cmd->viewangles.x = rand() % 361;
					if (cmd->viewangles.x > 180)
						cmd->viewangles.x -= 360;
				}
			}

			FixMoveEnd(cmd);
		}
	}
}