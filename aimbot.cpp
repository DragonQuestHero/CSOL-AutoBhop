#include "client.h"

deque<fov_t> FOVDraw;
deque<playeraim_t> PlayerAim;
deque<playeraimlegit_t> PlayerAimLegit;

DWORD dwReactionTime;

bool TriggerKeyStatus;
bool LegitKeyStatus;
bool RageKeyStatus;

Vector vAimOriginKnife;
Vector vAimOriginLegit;
Vector vAimOriginRage;

int iTargetTrigger;
int iTargetLegit;
int iTargetRage;
int iTargetKnife;

int iHitboxLegit;
int iHitboxRage;
int iHitboxKnife;

float m_flCurrentFOV;

bool IsBoxIntersectingRay(const Vector& boxMin, const Vector& boxMax, const Vector& origin, const Vector& delta)
{
	ASSERT(boxMin[0] <= boxMax[0]);
	ASSERT(boxMin[1] <= boxMax[1]);
	ASSERT(boxMin[2] <= boxMax[2]);

	float tmin = -FLT_MAX;
	float tmax = FLT_MAX;

	for (unsigned int i = 0; i < 3; ++i)
	{
		if (fabs(delta[i]) < 1e-8)
		{
			if ((origin[i] < boxMin[i]) || (origin[i] > boxMax[i]))
				return false;

			continue;
		}

		float invDelta = 1.0f / delta[i];
		float t1 = (boxMin[i] - origin[i]) * invDelta;
		float t2 = (boxMax[i] - origin[i]) * invDelta;
		if (t1 > t2)
		{
			float temp = t1;
			t1 = t2;
			t2 = temp;
		}
		if (t1 > tmin)
			tmin = t1;
		if (t2 < tmax)
			tmax = t2;
		if (tmin > tmax)
			return false;
		if (tmax < 0)
			return false;
	}

	return true;
}

void TriggerTarget(struct usercmd_s* cmd, playeraim_t Aim, float& m_flBestFOV, DWORD delay, Vector vecSpreadDir)
{
	bool hitboxselected = false;
	for (model_aim_select_t Model_Selected : Model_Aim_Select)
	{
		if (!strstr(Model_Selected.checkmodel, Aim.modelname))
			continue;

		hitboxselected = true;

		Vector vEye = pmove->origin + pmove->view_ofs;

		bool bHitboxPointsVisible[8];

		for (unsigned int point = 0; point < 8; ++point)
		{
			pmtrace_t tr;

			g_Engine.pEventAPI->EV_SetTraceHull(2);

			if (cvar.bypass_trace_trigger)
				g_Engine.pEventAPI->EV_PlayerTrace(vEye, Aim.PlayerAimHitbox[Model_Selected.numhitbox].HitboxMulti[point], PM_WORLD_ONLY, -1, &tr);
			else
				g_Engine.pEventAPI->EV_PlayerTrace(vEye, Aim.PlayerAimHitbox[Model_Selected.numhitbox].HitboxMulti[point], PM_GLASS_IGNORE, -1, &tr);

			int detect = g_Engine.pEventAPI->EV_IndexFromTrace(&tr);

			if ((cvar.bypass_trace_trigger && tr.fraction == 1 && !detect) || (!cvar.bypass_trace_trigger && detect == Aim.ent->index))
			{
				bHitboxPointsVisible[point] = true;
				if (Aim.PlayerAimHitbox[Model_Selected.numhitbox].HitboxPointsFOV[point] < m_flBestFOV)
				{
					m_flBestFOV = Aim.PlayerAimHitbox[Model_Selected.numhitbox].HitboxPointsFOV[point];
					iTargetTrigger = Aim.ent->index;
				}
			}
			else
			{
				bHitboxPointsVisible[point] = false;

				int iOriginalPenetration = CurPenetration();

				if (iOriginalPenetration && cvar.legit[g_Local.weapon.m_iWeaponID].trigger_wall)
				{
					int iDamage = CurDamage();
					int iBulletType = CurBulletType();
					float flDistance = CurDistance();
					float flRangeModifier = CurWallPierce();

					int iCurrentDamage = FireBullets(vEye, Aim.PlayerAimHitbox[Model_Selected.numhitbox].HitboxMulti[point], flDistance, iOriginalPenetration, iBulletType, iDamage, flRangeModifier);

					if (iCurrentDamage > 0)
					{
						bHitboxPointsVisible[point] = true;
						if (Aim.PlayerAimHitbox[Model_Selected.numhitbox].HitboxPointsFOV[point] < m_flBestFOV)
						{
							m_flBestFOV = Aim.PlayerAimHitbox[Model_Selected.numhitbox].HitboxPointsFOV[point];
							iTargetTrigger = Aim.ent->index;
						}
					}
				}
			}
		}
		for (unsigned int i = 0; i < 12; i++)
		{
			if (bHitboxPointsVisible[SkeletonHitboxMatrix[i][0]] && bHitboxPointsVisible[SkeletonHitboxMatrix[i][1]])
			{
				Vector vEye = pmove->origin + pmove->view_ofs;
				if (IsBoxIntersectingRay(Aim.PlayerAimHitbox[Model_Selected.numhitbox].HitboxMulti[SkeletonHitboxMatrix[i][0]], Aim.PlayerAimHitbox[Model_Selected.numhitbox].HitboxMulti[SkeletonHitboxMatrix[i][1]], vEye, vecSpreadDir))
				{
					if (CanAttack() && GetTickCount() - delay > cvar.legit[g_Local.weapon.m_iWeaponID].trigger_delay_shot)
						cmd->buttons |= IN_ATTACK;
					break;
				}
			}
		}
	}
	if (!hitboxselected)
	{
		Vector vEye = pmove->origin + pmove->view_ofs;

		bool bHitboxPointsVisible[8];

		for (unsigned int point = 0; point < 8; ++point)
		{
			pmtrace_t tr;

			int detect = 0;

			g_Engine.pEventAPI->EV_SetTraceHull(2);

			if (cvar.bypass_trace_trigger)
				g_Engine.pEventAPI->EV_PlayerTrace(vEye, Aim.PlayerAimHitbox[HeadBox[Aim.ent->index]].HitboxMulti[point], PM_WORLD_ONLY, -1, &tr);
			else
				g_Engine.pEventAPI->EV_PlayerTrace(vEye, Aim.PlayerAimHitbox[HeadBox[Aim.ent->index]].HitboxMulti[point], PM_GLASS_IGNORE, -1, &tr);

			detect = g_Engine.pEventAPI->EV_IndexFromTrace(&tr);

			if ((cvar.bypass_trace_trigger && tr.fraction == 1 && !detect) || (!cvar.bypass_trace_trigger && detect == Aim.ent->index))
			{
				bHitboxPointsVisible[point] = true;
				if (Aim.PlayerAimHitbox[HeadBox[Aim.ent->index]].HitboxPointsFOV[point] < m_flBestFOV)
				{
					m_flBestFOV = Aim.PlayerAimHitbox[HeadBox[Aim.ent->index]].HitboxPointsFOV[point];
					iTargetTrigger = Aim.ent->index;
				}
			}
			else
			{
				bHitboxPointsVisible[point] = false;

				int iOriginalPenetration = CurPenetration();

				if (iOriginalPenetration && cvar.legit[g_Local.weapon.m_iWeaponID].trigger_wall)
				{
					int iDamage = CurDamage();
					int iBulletType = CurBulletType();
					float flDistance = CurDistance();
					float flRangeModifier = CurWallPierce();

					int iCurrentDamage = FireBullets(vEye, Aim.PlayerAimHitbox[HeadBox[Aim.ent->index]].HitboxMulti[point], flDistance, iOriginalPenetration, iBulletType, iDamage, flRangeModifier);

					if (iCurrentDamage > 0)
					{
						bHitboxPointsVisible[point] = true;
						if (Aim.PlayerAimHitbox[HeadBox[Aim.ent->index]].HitboxPointsFOV[point] < m_flBestFOV)
						{
							m_flBestFOV = Aim.PlayerAimHitbox[HeadBox[Aim.ent->index]].HitboxPointsFOV[point];
							iTargetTrigger = Aim.ent->index;
						}
					}
				}
			}
		}
		for (unsigned int i = 0; i < 12; i++)
		{
			if (bHitboxPointsVisible[SkeletonHitboxMatrix[i][0]] && bHitboxPointsVisible[SkeletonHitboxMatrix[i][1]])
			{
				Vector vEye = pmove->origin + pmove->view_ofs;
				if (IsBoxIntersectingRay(Aim.PlayerAimHitbox[HeadBox[Aim.ent->index]].HitboxMulti[SkeletonHitboxMatrix[i][0]], Aim.PlayerAimHitbox[HeadBox[Aim.ent->index]].HitboxMulti[SkeletonHitboxMatrix[i][1]], vEye, vecSpreadDir))
				{
					if (CanAttack() && GetTickCount() - delay > cvar.legit[g_Local.weapon.m_iWeaponID].trigger_delay_shot)
						cmd->buttons |= IN_ATTACK;
					break;
				}
			}
		}
	}
}

void TriggerAimbot(struct usercmd_s* cmd)
{
	float m_flBestFOV = 180;

	if (!cvar.legit[g_Local.weapon.m_iWeaponID].trigger_active || !IsCurWeaponGun())
		return;

	if (!TriggerKeyStatus && cvar.legit_trigger_key > 0 && cvar.legit_trigger_key < 255)
			return;

	if (cvar.legit_trigger_only_zoom && IsCurWeaponSniper() && g_Local.iFOV == 90)
		return;

	float flAccuracy = cvar.legit[g_Local.weapon.m_iWeaponID].trigger_accuracy;

	Vector vecSpreadDir, vecForward, vecRight, vecUp, vecRandom;

	QAngle QAngles;

	g_Engine.GetViewAngles(QAngles);

	if (flAccuracy > 0)
	{
		QAngles[0] += g_Local.vPunchangle[0];
		QAngles[1] += g_Local.vPunchangle[1];
		QAngles[2] = NULL;
	}

	QAngles.Normalize();

	QAngles.AngleVectors(&vecForward, &vecRight, &vecUp);

	if (flAccuracy > 1)
	{
		GetSpreadXY(g_Local.weapon.random_seed, 1, vecRandom);
		vecSpreadDir = vecForward + (vecRight * vecRandom[0]) + (vecUp * vecRandom[1]);
		vecSpreadDir.Normalize();
	}
	else
	{
		vecSpreadDir = vecForward;
		vecSpreadDir.Normalize();
	}
	static DWORD delay = 0;
	static int tickcount = 0;

	for (playeraim_t Aim : PlayerAim)
	{
		if (idhook.FirstKillPlayer[Aim.ent->index] == 1 || cvar.aim_id_mode == 0)
		{
			if (!bAlive(Aim.ent))
				continue;

			if (!cvar.legit_trigger_team && g_Player[Aim.ent->index].iTeam == g_Local.iTeam)
				continue;

			TriggerTarget(cmd, Aim, m_flBestFOV, delay, vecSpreadDir);
		}
	}

	if (cvar.aim_id_mode != 2 && iTargetTrigger == 0)
	{
		for (playeraim_t Aim : PlayerAim)
		{
			if (idhook.FirstKillPlayer[Aim.ent->index] < 2)
			{
				if (!bAlive(Aim.ent))
					continue;

				if (!cvar.legit_trigger_team && g_Player[Aim.ent->index].iTeam == g_Local.iTeam)
					continue;

				TriggerTarget(cmd, Aim, m_flBestFOV, delay, vecSpreadDir);
			}
		}
	}
	if (cmd->buttons & IN_ATTACK)tickcount++;
	static int random = rand() % (int)cvar.legit[g_Local.weapon.m_iWeaponID].trigger_random_max + 1;
	if (tickcount >= (!cvar.legit[g_Local.weapon.m_iWeaponID].trigger_shot_type ? cvar.legit[g_Local.weapon.m_iWeaponID].trigger_shot_count : random))
	{
		random = rand() % (int)cvar.legit[g_Local.weapon.m_iWeaponID].trigger_random_max + 1;
		delay = GetTickCount();
		tickcount = 0;
	}
}

void SmoothAimAngles(QAngle MyViewAngles, QAngle AimAngles, QAngle& OutAngles, float Smoothing, bool bSpiral, float SpiralX, float SpiralY)
{
	if (Smoothing < 1)
	{
		OutAngles = AimAngles;
		return;
	}

	OutAngles = AimAngles - MyViewAngles;

	OutAngles.Normalize();

	Vector vecViewAngleDelta = OutAngles;

	if (bSpiral && SpiralX != 0 && SpiralY != 0)
		vecViewAngleDelta += Vector(vecViewAngleDelta.y / SpiralX, vecViewAngleDelta.x / SpiralY, 0.0f);

	if (!isnan(Smoothing))
		vecViewAngleDelta /= Smoothing;

	OutAngles = MyViewAngles + vecViewAngleDelta;

	OutAngles.Normalize();
}

void LegitSelect(playeraim_t Aim, Vector vecFOV, float& flBestFOV, float flSpeedScaleFov, float& flSpeed)
{
	bool hitboxselected = false;
	for (model_aim_select_t Model_Selected : Model_Aim_Select)
	{
		if (strstr(Model_Selected.checkmodel, Aim.modelname))
		{
			bool skip = false;
			for (playeraimlegit_t AimLegit : PlayerAimLegit)
			{
				if (!strstr(AimLegit.checkmodel, Model_Selected.checkmodel))
					continue;
				if (AimLegit.numhitbox != Model_Selected.numhitbox)
					continue;
				if (AimLegit.m_iWeaponID != g_Local.weapon.m_iWeaponID)
					continue;
				skip = true;
			}
			if (skip)
				continue;

			hitboxselected = true;

			pmtrace_t tr;

			g_Engine.pEventAPI->EV_SetTraceHull(2);

			Vector vEye = pmove->origin + pmove->view_ofs;

			if (cvar.bypass_trace_legit)
				g_Engine.pEventAPI->EV_PlayerTrace(vEye, Aim.PlayerAimHitbox[Model_Selected.numhitbox].Hitbox, PM_WORLD_ONLY, -1, &tr);
			else
				g_Engine.pEventAPI->EV_PlayerTrace(vEye, Aim.PlayerAimHitbox[Model_Selected.numhitbox].Hitbox, PM_GLASS_IGNORE, -1, &tr);

			int detect = g_Engine.pEventAPI->EV_IndexFromTrace(&tr);

			if ((cvar.bypass_trace_legit && tr.fraction == 1 && !detect) || (!cvar.bypass_trace_legit && detect == Aim.ent->index))
			{
				Vector vEye = pmove->origin + pmove->view_ofs;
				float fov = vecFOV.AngleBetween(Aim.PlayerAimHitbox[Model_Selected.numhitbox].Hitbox - vEye);
				if (fov < flBestFOV)
				{
					flBestFOV = fov;
					iTargetLegit = Aim.ent->index;
					iHitboxLegit = Model_Selected.numhitbox;
					vAimOriginLegit = Aim.PlayerAimHitbox[Model_Selected.numhitbox].Hitbox;
					if (flSpeedScaleFov > 0 && flSpeedScaleFov <= 100 && g_Local.vPunchangle.IsZero() && !isnan(Aim.PlayerAimHitbox[Model_Selected.numhitbox].HitboxFOV))
						flSpeed = flSpeed - (((Aim.PlayerAimHitbox[Model_Selected.numhitbox].HitboxFOV * (flSpeed / m_flCurrentFOV)) * flSpeedScaleFov) / 100);
				}
			}
		}
	}
	if (!hitboxselected)
	{
		pmtrace_t tr;

		g_Engine.pEventAPI->EV_SetTraceHull(2);

		Vector vEye = pmove->origin + pmove->view_ofs;

		if (cvar.bypass_trace_legit)
			g_Engine.pEventAPI->EV_PlayerTrace(vEye, Aim.PlayerAimHitbox[HeadBox[Aim.ent->index]].Hitbox, PM_WORLD_ONLY, -1, &tr);
		else
			g_Engine.pEventAPI->EV_PlayerTrace(vEye, Aim.PlayerAimHitbox[HeadBox[Aim.ent->index]].Hitbox, PM_GLASS_IGNORE, -1, &tr);

		int detect = g_Engine.pEventAPI->EV_IndexFromTrace(&tr);

		if ((cvar.bypass_trace_legit && tr.fraction == 1 && !detect) || (!cvar.bypass_trace_legit && detect == Aim.ent->index))
		{
			Vector vEye = pmove->origin + pmove->view_ofs;
			float fov = vecFOV.AngleBetween(Aim.PlayerAimHitbox[HeadBox[Aim.ent->index]].Hitbox - vEye);
			if (fov < flBestFOV)
			{
				flBestFOV = fov;
				iTargetLegit = Aim.ent->index;
				iHitboxLegit = HeadBox[Aim.ent->index];
				vAimOriginLegit = Aim.PlayerAimHitbox[HeadBox[Aim.ent->index]].Hitbox;
				if (flSpeedScaleFov > 0 && flSpeedScaleFov <= 100 && g_Local.vPunchangle.IsZero() && !isnan(Aim.PlayerAimHitbox[HeadBox[Aim.ent->index]].HitboxFOV))
					flSpeed = flSpeed - (((Aim.PlayerAimHitbox[HeadBox[Aim.ent->index]].HitboxFOV * (flSpeed / m_flCurrentFOV)) * flSpeedScaleFov) / 100);
			}
		}
	}
}

void LegitAimbot(struct usercmd_s* cmd)
{
	static DWORD dwBlockAttack = 0;

	static float flSpeedSpiralX = 1.3;
	static float flSpeedSpiralY = 3.7;

	bool Attacking;
	if (cvar.legit_key == -1)
		Attacking = cmd->buttons & IN_ATTACK;
	else
		Attacking = LegitKeyStatus;

	m_flCurrentFOV = 0;

	if (!IsCurWeaponGun() || g_Local.weapon.m_iInReload || g_Local.weapon.m_iClip < 1 || g_Local.weapon.m_flNextAttack > 0.0)
		return;

	float flFOV = cvar.legit[g_Local.weapon.m_iWeaponID].fov;

	if (!flFOV)
		return;

	float flSpeed = 0;

	if (cvar.legit[g_Local.weapon.m_iWeaponID].speed_in_attack)
		flSpeed = 101 - cvar.legit[g_Local.weapon.m_iWeaponID].speed_in_attack;

	if (cvar.legit[g_Local.weapon.m_iWeaponID].speed && !(Attacking))
		flSpeed = 101 - cvar.legit[g_Local.weapon.m_iWeaponID].speed;

	if (!flSpeed)
		return;

	float flReactionTime = cvar.legit[g_Local.weapon.m_iWeaponID].reaction_time;

	if (flReactionTime > 0 && GetTickCount() - dwReactionTime < flReactionTime)
		return;

	float flSpeedScaleFov = cvar.legit[g_Local.weapon.m_iWeaponID].speed_scale_fov;

	bool bSpeedSpiral = cvar.legit[g_Local.weapon.m_iWeaponID].humanize;

	if (!g_Local.vPunchangle.IsZero2D())
		bSpeedSpiral = false;

	float flRecoilCompensationPitch = 0.02f * cvar.legit[g_Local.weapon.m_iWeaponID].recoil_compensation_pitch;
	float flRecoilCompensationYaw = 0.02f * cvar.legit[g_Local.weapon.m_iWeaponID].recoil_compensation_yaw;

	unsigned int iRecoilCompensationAfterShotsFired = static_cast<int>(cvar.legit[g_Local.weapon.m_iWeaponID].recoil_compensation_after_shots_fired);

	if (iRecoilCompensationAfterShotsFired > 0 && g_Local.weapon.m_iShotsFired <= iRecoilCompensationAfterShotsFired)
	{
		flRecoilCompensationPitch = 0;
		flRecoilCompensationYaw = 0;
	}
	float flBlockAttackAfterKill = cvar.legit[g_Local.weapon.m_iWeaponID].block_attack_after_kill;

	float flAccuracy = cvar.legit[g_Local.weapon.m_iWeaponID].accuracy;

	float flPSilent = cvar.legit[g_Local.weapon.m_iWeaponID].perfect_silent / 100;

	Vector vecFOV = {};
	{
		QAngle QAngles = cmd->viewangles + g_Local.vPunchangle;
		QAngles.Normalize();
		QAngles.AngleVectors(&vecFOV, NULL, NULL);
		vecFOV.Normalize();
	}

	m_flCurrentFOV = flFOV;

	float flBestFOV = flFOV;

	for (playeraim_t Aim : PlayerAim)
	{
		if (idhook.FirstKillPlayer[Aim.ent->index] == 1 || cvar.aim_id_mode == 0)
		{
			if (!bAlive(Aim.ent))
				continue;

			if (!cvar.legit_team && g_Player[Aim.ent->index].iTeam == g_Local.iTeam)
				continue;

			LegitSelect(Aim, vecFOV, flBestFOV, flSpeedScaleFov, flSpeed);
		}
	}

	if (cvar.aim_id_mode != 2 && iTargetLegit == 0)
	{
		for (playeraim_t Aim : PlayerAim)
		{
			if (idhook.FirstKillPlayer[Aim.ent->index] < 2)
			{
				if (!bAlive(Aim.ent))
					continue;

				if (!cvar.legit_team && g_Player[Aim.ent->index].iTeam == g_Local.iTeam)
					continue;

				LegitSelect(Aim, vecFOV, flBestFOV, flSpeedScaleFov, flSpeed);
			}
		}
	}

	if (iTargetLegit)
	{
		bool bAttack = false;
		bool bBlock = false;

		QAngle QMyAngles, QNewAngles, QSmoothAngles, QAimAngles;

		g_Engine.GetViewAngles(QMyAngles);

		Vector vEye = pmove->origin + pmove->view_ofs;

		VectorAngles(vAimOriginLegit - vEye, QAimAngles);

		if (flPSilent > 0 && flPSilent <= 1 && CanAttack())
		{
			QAngle QAnglePerfectSilent = QAimAngles;

			QAnglePerfectSilent += g_Local.vPunchangle;

			QAnglePerfectSilent.Normalize();

			GetSpreadOffset(g_Local.weapon.random_seed, 1, QAnglePerfectSilent, QAnglePerfectSilent);

			Vector vecPsilentFOV;
			QAnglePerfectSilent.AngleVectors(&vecPsilentFOV, NULL, NULL);
			vecPsilentFOV.Normalize();
			float fov = vecPsilentFOV.AngleBetween(vAimOriginLegit - vEye);

			if (fov <= flPSilent)
			{
				cmd->buttons |= IN_ATTACK;

				MakeAngle(QAnglePerfectSilent, cmd);
				bSendpacket(false);

				dwBlockAttack = GetTickCount();
				return;
			}
		}

		QNewAngles[0] = QAimAngles[0] - g_Local.vPunchangle[0] * flRecoilCompensationPitch;
		QNewAngles[1] = QAimAngles[1] - g_Local.vPunchangle[1] * flRecoilCompensationYaw;
		QNewAngles[2] = 0;

		QNewAngles.Normalize();

		SmoothAimAngles(QMyAngles, QNewAngles, QSmoothAngles, flSpeed, bSpeedSpiral, flSpeedSpiralX, flSpeedSpiralY);

		if (flAccuracy > 0)
		{
			bBlock = true;

			QAngle QAngleAccuracy = QAimAngles;

			Vector vecSpreadDir;

			if (flAccuracy == 1)
			{
				QSmoothAngles.AngleVectors(&vecSpreadDir, NULL, NULL);

				vecSpreadDir.Normalize();
			}
			else if (flAccuracy == 2) //Recoil
			{
				Vector vecRandom, vecForward;

				SmoothAimAngles(QMyAngles, QAimAngles, QAngleAccuracy, flSpeed, bSpeedSpiral, flSpeedSpiralX, flSpeedSpiralY);

				QAngleAccuracy[0] += g_Local.vPunchangle[0];
				QAngleAccuracy[1] += g_Local.vPunchangle[1];
				QAngleAccuracy[2] = NULL;

				QAngleAccuracy.Normalize();

				QAngleAccuracy.AngleVectors(&vecForward, NULL, NULL);

				vecSpreadDir = vecForward;

				vecSpreadDir.Normalize();
			}
			else //Recoil / Spread
			{
				Vector vecRandom, vecRight, vecUp, vecForward;

				SmoothAimAngles(QMyAngles, QAimAngles, QAngleAccuracy, flSpeed, bSpeedSpiral, flSpeedSpiralX, flSpeedSpiralY);

				QAngleAccuracy[0] += g_Local.vPunchangle[0];
				QAngleAccuracy[1] += g_Local.vPunchangle[1];
				QAngleAccuracy[2] = NULL;

				QAngleAccuracy.Normalize();

				QAngleAccuracy.AngleVectors(&vecForward, &vecRight, &vecRight);

				GetSpreadXY(g_Local.weapon.random_seed, 1, vecRandom);

				vecSpreadDir = vecForward + (vecRight * vecRandom[0]) + (vecUp * vecRandom[1]);

				vecSpreadDir.Normalize();
			}
			for (playeraim_t Aim : PlayerAim)
			{
				if (Aim.ent->index != iTargetLegit)
					continue;

				bool hitboxselected = false;
				for (model_aim_select_t Model_Selected : Model_Aim_Select)
				{
					if (strstr(Model_Selected.checkmodel, Aim.modelname))
					{
						hitboxselected = true;
						for (unsigned int i = 0; i < 12; i++)
						{
							if (IsBoxIntersectingRay(Aim.PlayerAimHitbox[Model_Selected.numhitbox].HitboxMulti[SkeletonHitboxMatrix[i][0]], Aim.PlayerAimHitbox[Model_Selected.numhitbox].HitboxMulti[SkeletonHitboxMatrix[i][1]], vEye, vecSpreadDir))
							{
								bBlock = false;
								break;
							}
						}
					}
				}
				if (!hitboxselected)
				{
					for (unsigned int i = 0; i < 12; i++)
					{
						if (IsBoxIntersectingRay(Aim.PlayerAimHitbox[HeadBox[Aim.ent->index]].HitboxMulti[SkeletonHitboxMatrix[i][0]], Aim.PlayerAimHitbox[HeadBox[Aim.ent->index]].HitboxMulti[SkeletonHitboxMatrix[i][1]], vEye, vecSpreadDir))
						{
							bBlock = false;
							break;
						}
					}
				}
			}
		}

		if (Attacking)
			bAttack = true;
		else if (cvar.legit[g_Local.weapon.m_iWeaponID].speed)
		{
			bAttack = true;
			bBlock = true;
		}

		if (bAttack)
		{
			QSmoothAngles.Normalize();

			MakeAngle(QSmoothAngles, cmd);
			cmd->viewangles = QSmoothAngles;
			g_Engine.SetViewAngles(QSmoothAngles);

			if (!bBlock)
				cmd->buttons |= IN_ATTACK;
			else if (Attacking)
				cmd->buttons &= ~IN_ATTACK;

			if (!g_Local.vPunchangle.IsZero2D())
				dwBlockAttack = GetTickCount();
		}
	}
	else if (flBlockAttackAfterKill > 0 && GetTickCount() - dwBlockAttack < flBlockAttackAfterKill)
	{
		cmd->buttons &= ~IN_ATTACK;
	}
}

void KnifeSelect(playeraim_t Aim, float& flDist)
{
	bool hitboxselected = false;
	for (model_aim_select_t Model_Selected : Model_Aim_Select)
	{
		if (strstr(Model_Selected.checkmodel, Aim.modelname))
		{
			hitboxselected = true;
			pmtrace_t tr;

			g_Engine.pEventAPI->EV_SetTraceHull(2);

			Vector vEye = pmove->origin + pmove->view_ofs;

			if (cvar.bypass_trace_knife)
				g_Engine.pEventAPI->EV_PlayerTrace(vEye, Aim.PlayerAimHitbox[Model_Selected.numhitbox].Hitbox, PM_WORLD_ONLY, -1, &tr);
			else
				g_Engine.pEventAPI->EV_PlayerTrace(vEye, Aim.PlayerAimHitbox[Model_Selected.numhitbox].Hitbox, PM_GLASS_IGNORE, -1, &tr);

			int detect = g_Engine.pEventAPI->EV_IndexFromTrace(&tr);

			if ((cvar.bypass_trace_knife && tr.fraction == 1 && !detect) || (!cvar.bypass_trace_knife && detect == Aim.ent->index))
			{
				if (Aim.PlayerAimHitbox[Model_Selected.numhitbox].HitboxFOV <= cvar.knifebot_fov)
				{
					if (Aim.PlayerAimHitbox[Model_Selected.numhitbox].Hitbox.Distance(pmove->origin + pmove->view_ofs) < flDist)
					{
						flDist = Aim.PlayerAimHitbox[Model_Selected.numhitbox].Hitbox.Distance(pmove->origin + pmove->view_ofs);
						iTargetKnife = Aim.ent->index;
						vAimOriginKnife = Aim.PlayerAimHitbox[Model_Selected.numhitbox].Hitbox;
						iHitboxKnife = Model_Selected.numhitbox;
						break;
					}
				}
			}
		}
	}
	if (!hitboxselected)
	{
		pmtrace_t tr;

		g_Engine.pEventAPI->EV_SetTraceHull(2);

		Vector vEye = pmove->origin + pmove->view_ofs;

		if (cvar.bypass_trace_knife)
			g_Engine.pEventAPI->EV_PlayerTrace(vEye, Aim.PlayerAimHitbox[HeadBox[Aim.ent->index]].Hitbox, PM_WORLD_ONLY, -1, &tr);
		else
			g_Engine.pEventAPI->EV_PlayerTrace(vEye, Aim.PlayerAimHitbox[HeadBox[Aim.ent->index]].Hitbox, PM_GLASS_IGNORE, -1, &tr);

		int detect = g_Engine.pEventAPI->EV_IndexFromTrace(&tr);

		if ((cvar.bypass_trace_knife && tr.fraction == 1 && !detect) || (!cvar.bypass_trace_knife && detect == Aim.ent->index))
		{
			if (Aim.PlayerAimHitbox[HeadBox[Aim.ent->index]].HitboxFOV <= cvar.knifebot_fov)
			{
				if (Aim.PlayerAimHitbox[HeadBox[Aim.ent->index]].Hitbox.Distance(pmove->origin + pmove->view_ofs) < flDist)
				{
					flDist = Aim.PlayerAimHitbox[HeadBox[Aim.ent->index]].Hitbox.Distance(pmove->origin + pmove->view_ofs);
					iTargetKnife = Aim.ent->index;
					vAimOriginKnife = Aim.PlayerAimHitbox[HeadBox[Aim.ent->index]].Hitbox;
					iHitboxKnife = HeadBox[Aim.ent->index];
				}
			}
		}
	}
}

void KnifeAimBot(struct usercmd_s* cmd)
{
	if (!IsCurWeaponKnife() || !cvar.knifebot_active)
		return;

	float flDist = 8192.f;

	for (playeraim_t Aim : PlayerAim)
	{
		if (idhook.FirstKillPlayer[Aim.ent->index] == 1 || cvar.aim_id_mode == 0)
		{
			if (!bAlive(Aim.ent))
				continue;

			if (!cvar.knifebot_team && g_Player[Aim.ent->index].iTeam == g_Local.iTeam)
				continue;

			KnifeSelect(Aim, flDist);
		}
	}

	if (cvar.aim_id_mode != 2 && iTargetKnife == 0)
	{
		for (playeraim_t Aim : PlayerAim)
		{
			if (idhook.FirstKillPlayer[Aim.ent->index] < 2)
			{
				if (!bAlive(Aim.ent))
					continue;

				if (!cvar.knifebot_team && g_Player[Aim.ent->index].iTeam == g_Local.iTeam)
					continue;

				KnifeSelect(Aim, flDist);
			}
		}
	}

	if (iTargetKnife)
	{
		int dist = NULL;
		if (cvar.knifebot_attack == 0)
			dist = cvar.knifebot_attack_distance;
		else if (cvar.knifebot_attack == 1)
			dist = cvar.knifebot_attack2_distance;

		if (vAimOriginKnife.Distance(pmove->origin + pmove->view_ofs) < dist)
		{
			if (CanAttack())
			{
				QAngle QAimAngles;

				Vector vEye = pmove->origin + pmove->view_ofs;

				VectorAngles(vAimOriginKnife - vEye, QAimAngles);

				QAimAngles.Normalize();

				if (cvar.knifebot_perfect_silent)
				{
					MakeAngle(QAimAngles, cmd);
					bSendpacket(false);
				}
				else if (cvar.knifebot_silent)
				{
					MakeAngle(QAimAngles, cmd);
				}
				else
				{
					cmd->viewangles = QAimAngles;
					g_Engine.SetViewAngles(QAimAngles);
				}

				if (cvar.knifebot_attack == 0)
					cmd->buttons |= IN_ATTACK;
				else if (cvar.knifebot_attack == 1)
					cmd->buttons |= IN_ATTACK2;
			}
			else
			{
				if (cvar.knifebot_attack == 0)
					cmd->buttons &= ~IN_ATTACK;
				else if (cvar.knifebot_attack == 1)
					cmd->buttons &= ~IN_ATTACK2;
			}
		}
	}
}

void RageTarget(playeraim_t Aim, float& m_flBestDist, float& m_flBestFOV, int hitbox)
{
	if (IsCurWeaponKnife())
	{
		if (Aim.PlayerAimHitbox[hitbox].Hitbox.Distance(pmove->origin + pmove->view_ofs) < m_flBestDist)
		{
			m_flBestDist = Aim.PlayerAimHitbox[hitbox].Hitbox.Distance(pmove->origin + pmove->view_ofs);
			iTargetRage = Aim.ent->index;
			vAimOriginRage = Aim.PlayerAimHitbox[hitbox].Hitbox;
			iHitboxRage = hitbox;
		}
	}
	else
	{
		if (cvar.rage_target_selection == 0)
		{
			if (Aim.PlayerAimHitbox[hitbox].HitboxFOV < m_flBestFOV)
			{
				m_flBestFOV = Aim.PlayerAimHitbox[hitbox].HitboxFOV;
				iTargetRage = Aim.ent->index;
				vAimOriginRage = Aim.PlayerAimHitbox[hitbox].Hitbox;
				iHitboxRage = hitbox;
			}
		}
		if (cvar.rage_target_selection == 1)
		{
			if (Aim.PlayerAimHitbox[hitbox].Hitbox.Distance(pmove->origin + pmove->view_ofs) < m_flBestDist)
			{
				m_flBestDist = Aim.PlayerAimHitbox[hitbox].Hitbox.Distance(pmove->origin + pmove->view_ofs);
				iTargetRage = Aim.ent->index;
				vAimOriginRage = Aim.PlayerAimHitbox[hitbox].Hitbox;
				iHitboxRage = hitbox;
			}
		}
		if (cvar.rage_target_selection == 2)
		{
			if (Aim.PlayerAimHitbox[hitbox].HitboxFOV < m_flBestFOV)
			{
				if (Aim.PlayerAimHitbox[hitbox].Hitbox.Distance(pmove->origin + pmove->view_ofs) < m_flBestDist)
				{
					m_flBestFOV = Aim.PlayerAimHitbox[hitbox].HitboxFOV;
					m_flBestDist = Aim.PlayerAimHitbox[hitbox].Hitbox.Distance(pmove->origin + pmove->view_ofs);
					iTargetRage = Aim.ent->index;
					vAimOriginRage = Aim.PlayerAimHitbox[hitbox].Hitbox;
					iHitboxRage = hitbox;
				}
			}
		}
	}
}

void RageSelect(playeraim_t Aim, float& m_flBestDist, float& m_flBestFOV)
{
	bool hitboxselected = false;

	for (model_aim_select_t Model_Selected : Model_Aim_Select)
	{
		if (!strstr(Model_Selected.checkmodel, Aim.modelname))
			continue;

		hitboxselected = true;

		pmtrace_t tr;

		g_Engine.pEventAPI->EV_SetTraceHull(2);

		Vector vEye = pmove->origin + pmove->view_ofs;

		if (cvar.bypass_trace_rage)
			g_Engine.pEventAPI->EV_PlayerTrace(vEye, Aim.PlayerAimHitbox[Model_Selected.numhitbox].Hitbox, PM_WORLD_ONLY, -1, &tr);
		else
			g_Engine.pEventAPI->EV_PlayerTrace(vEye, Aim.PlayerAimHitbox[Model_Selected.numhitbox].Hitbox, PM_GLASS_IGNORE, -1, &tr);

		int detect = g_Engine.pEventAPI->EV_IndexFromTrace(&tr);

		if ((cvar.bypass_trace_rage && tr.fraction == 1 && !detect) || (!cvar.bypass_trace_rage && detect == Aim.ent->index))
		{
			if (Aim.PlayerAimHitbox[Model_Selected.numhitbox].HitboxFOV <= cvar.rage_fov)
			{
				RageTarget(Aim, m_flBestDist, m_flBestFOV, Model_Selected.numhitbox);
				break;
			}
		}
		else
		{
			int iOriginalPenetration = CurPenetration();

			if (iOriginalPenetration && cvar.rage_wall)
			{
				int iDamage = CurDamage();
				int iBulletType = CurBulletType();
				float flDistance = CurDistance();
				float flRangeModifier = CurWallPierce();

				int iCurrentDamage = FireBullets(vEye, Aim.PlayerAimHitbox[Model_Selected.numhitbox].Hitbox, flDistance, iOriginalPenetration, iBulletType, iDamage, flRangeModifier);

				if (iCurrentDamage > 0)
				{
					if (Aim.PlayerAimHitbox[Model_Selected.numhitbox].HitboxFOV <= cvar.rage_fov)
					{
						RageTarget(Aim, m_flBestDist, m_flBestFOV, Model_Selected.numhitbox);
						break;
					}
				}
			}
		}
	}
	if (!hitboxselected)
	{
		pmtrace_t tr;

		g_Engine.pEventAPI->EV_SetTraceHull(2);

		Vector vEye = pmove->origin + pmove->view_ofs;

		if (cvar.bypass_trace_rage)
			g_Engine.pEventAPI->EV_PlayerTrace(vEye, Aim.PlayerAimHitbox[HeadBox[Aim.ent->index]].Hitbox, PM_WORLD_ONLY, -1, &tr);
		else
			g_Engine.pEventAPI->EV_PlayerTrace(vEye, Aim.PlayerAimHitbox[HeadBox[Aim.ent->index]].Hitbox, PM_GLASS_IGNORE, -1, &tr);

		int detect = g_Engine.pEventAPI->EV_IndexFromTrace(&tr);

		if ((cvar.bypass_trace_rage && tr.fraction == 1 && !detect) || (!cvar.bypass_trace_rage && detect == Aim.ent->index))
		{
			if (Aim.PlayerAimHitbox[HeadBox[Aim.ent->index]].HitboxFOV <= cvar.rage_fov)
				RageTarget(Aim, m_flBestDist, m_flBestFOV, HeadBox[Aim.ent->index]);
		}
		else
		{
			int iOriginalPenetration = CurPenetration();

			if (iOriginalPenetration && cvar.rage_wall)
			{
				int iDamage = CurDamage();
				int iBulletType = CurBulletType();
				float flDistance = CurDistance();
				float flRangeModifier = CurWallPierce();

				int iCurrentDamage = FireBullets(vEye, Aim.PlayerAimHitbox[HeadBox[Aim.ent->index]].Hitbox, flDistance, iOriginalPenetration, iBulletType, iDamage, flRangeModifier);

				if (iCurrentDamage > 0)
				{
					if (Aim.PlayerAimHitbox[HeadBox[Aim.ent->index]].HitboxFOV <= cvar.rage_fov)
						RageTarget(Aim, m_flBestDist, m_flBestFOV, HeadBox[Aim.ent->index]);
				}
			}
		}
	}
}

void RageAimbot(struct usercmd_s* cmd)
{
	if (IsCurWeaponNonAttack())
		return;

	float m_flBestFOV = 180;
	float m_flBestDist = 8192;

	for (playeraim_t Aim : PlayerAim)
	{
		if (idhook.FirstKillPlayer[Aim.ent->index] == 1 || cvar.aim_id_mode == 0)
		{
			if (!bAlive(Aim.ent))
				continue;

			if (!cvar.rage_team && g_Player[Aim.ent->index].iTeam == g_Local.iTeam)
				continue;

			if (!cvar.rage_shield_attack && (Aim.ent->curstate.sequence == 97 || Aim.ent->curstate.sequence == 98))
				continue;

			RageSelect(Aim, m_flBestDist, m_flBestFOV);
		}
	}

	if (cvar.aim_id_mode != 2 && iTargetRage == 0)
	{
		for (playeraim_t Aim : PlayerAim)
		{
			if (idhook.FirstKillPlayer[Aim.ent->index] < 2)
			{
				if (!bAlive(Aim.ent))
					continue;

				if (!cvar.rage_team && g_Player[Aim.ent->index].iTeam == g_Local.iTeam)
					continue;

				if (!cvar.rage_shield_attack && (Aim.ent->curstate.sequence == 97 || Aim.ent->curstate.sequence == 98))
					continue;

				RageSelect(Aim, m_flBestDist, m_flBestFOV);
			}
		}
	}

	if (iTargetRage)
	{
		QAngle QAimAngles;

		Vector vEye = pmove->origin + pmove->view_ofs;

		VectorAngles(vAimOriginRage - vEye, QAimAngles);

		static DWORD delay = 0;

		static int tickcount = 0;

		if (cvar.route_auto && cvar.route_mode)
		{
			if (g_Local.weapon.m_iClip)
			{
				if (IsCurWeaponKnife())
				{
					if (cvar.rage_knife_attack == 0)
						cmd->buttons |= IN_ATTACK;
					else if (cvar.rage_knife_attack == 1)
						cmd->buttons |= IN_ATTACK2;
				}
				else
					cmd->buttons |= IN_ATTACK;

				cmd->viewangles = QAimAngles;
				g_Engine.SetViewAngles(QAimAngles);
			}
		}
		else
		{
			if (cvar.rage_always_fire)
			{
				if (cmd->buttons & IN_ATTACK || cmd->buttons & IN_ATTACK2 && IsCurWeaponKnife())
				{
					if (cvar.rage_silent)
					{
						MakeAngle(QAimAngles, cmd);
					}
					else
					{
						cmd->viewangles = QAimAngles;
						g_Engine.SetViewAngles(QAimAngles);
					}
				}
			}
			else if (CanAttack())
			{
				if (GetTickCount() - delay > cvar.rage[g_Local.weapon.m_iWeaponID].rage_delay_shot)
				{
					if (cvar.rage_auto_fire || RageKeyStatus)
					{
						if (IsCurWeaponKnife())
						{
							if (cvar.rage_knife_attack == 0)
								cmd->buttons |= IN_ATTACK;
							else if (cvar.rage_knife_attack == 1)
								cmd->buttons |= IN_ATTACK2;
						}
						else
						{
							cmd->buttons |= IN_ATTACK;
							if (cmd->buttons & IN_ATTACK)tickcount++;
							static int random = rand() % (int)cvar.rage[g_Local.weapon.m_iWeaponID].rage_random_max + 1;
							if (tickcount >= (!cvar.rage[g_Local.weapon.m_iWeaponID].rage_shot_type ? cvar.rage[g_Local.weapon.m_iWeaponID].rage_shot_count : random))
							{
								random = rand() % (int)cvar.rage[g_Local.weapon.m_iWeaponID].rage_random_max + 1;
								delay = GetTickCount();
								tickcount = 0;
							}
						}
					}
					if (cmd->buttons & IN_ATTACK || cmd->buttons & IN_ATTACK2 && IsCurWeaponKnife())
					{
						if (cvar.rage_perfect_silent)
						{
							MakeAngle(QAimAngles, cmd);
							bSendpacket(false);
						}
						else if (cvar.rage_silent)
						{
							MakeAngle(QAimAngles, cmd);
						}
						else
						{
							cmd->viewangles = QAimAngles;
							g_Engine.SetViewAngles(QAimAngles);
						}
					}
				}
			}
			else
			{
				if (IsCurWeaponKnife())
				{
					if (cvar.rage_knife_attack == 0)
						cmd->buttons &= ~IN_ATTACK;
					else if (cvar.rage_knife_attack == 1)
						cmd->buttons &= ~IN_ATTACK2;
				}
				else
					cmd->buttons &= ~IN_ATTACK;
			}
		}
	}
}

void AimBotFOV()
{
	if (bAliveLocal() && IsCurWeaponGun() && !cvar.rage_active && cvar.legit[g_Local.weapon.m_iWeaponID].active && cvar.legit[g_Local.weapon.m_iWeaponID].drawfov && m_flCurrentFOV > 0 && m_flCurrentFOV <= 45 && g_Local.iFOV)
	{
		fov_t FOV;
		float x = ImGui::GetIO().DisplaySize.x / 2;
		float y = ImGui::GetIO().DisplaySize.y / 2;
		float dx = ImGui::GetIO().DisplaySize.x / g_Local.iFOV;
		float dy = ImGui::GetIO().DisplaySize.y / g_Local.iFOV;

		float radius = tanf(DEG2RAD(m_flCurrentFOV) * 0.5f) / tanf(DEG2RAD(g_Local.iFOV) * 0.5f) * ImGui::GetIO().DisplaySize.x;

		ImVec2 positions;
		positions.x = (x - (dx * g_Local.vNoRecoilAngle[1]));
		positions.y = (y + (dy * g_Local.vNoRecoilAngle[0]));

		float ColorHSV[4];

		ImGui::ColorConvertRGBtoHSV(color_red, color_green, color_blue, ColorHSV[0], ColorHSV[1], ColorHSV[2]);
		for (unsigned int i = 1; i < (int)radius; i++)
		{
			ColorHSV[3] = 0.2f / radius * i;
			ColorHSV[0] += 1.0f / radius;
			if (ColorHSV[0] > 1.0f)
				ColorHSV[0] -= 1.0f;
			FOV.Pos = positions;
			FOV.Radius = i;
			FOV.r = ColorHSV[0];
			FOV.g = ColorHSV[1];
			FOV.b = ColorHSV[2];
			FOV.a = ColorHSV[3];
			FOVDraw.push_back(FOV);
		}
	}
}

void AimBot(struct usercmd_s* cmd)
{
	iTargetTrigger = 0;
	iTargetLegit = 0;
	iTargetKnife = 0;
	iTargetRage = 0;
	if (bAliveLocal() && GetTickCount() - HudRedraw <= 100)
	{
		if (cvar.rage_active)
		{
			RageAimbot(cmd);
		}
		else
		{
			if (cvar.legit[g_Local.weapon.m_iWeaponID].active)
				LegitAimbot(cmd);
			else
				TriggerAimbot(cmd);

			KnifeAimBot(cmd);
		}
	}
	AimBotFOV();
}

void KnifeDraw()
{
	if (!cvar.knifebot_active || !IsCurWeaponKnife() || !bAliveLocal() || !cvar.knifebot_draw_aim)
		return;

	if (iTargetKnife)
	{
		for (playeraim_t Aim : PlayerAim)
		{
			if (Aim.ent->index != iTargetKnife)
				continue;
			for (unsigned int i = 0; i < 12; i++)
			{
				float CalcAnglesMin[2], CalcAnglesMax[2];
				if (WorldToScreen(Aim.PlayerAimHitbox[iHitboxKnife].HitboxMulti[SkeletonHitboxMatrix[i][0]], CalcAnglesMin) && WorldToScreen(Aim.PlayerAimHitbox[iHitboxKnife].HitboxMulti[SkeletonHitboxMatrix[i][1]], CalcAnglesMax))
					ImGui::GetCurrentWindow()->DrawList->AddLine({ IM_ROUND(CalcAnglesMin[0]), IM_ROUND(CalcAnglesMin[1]) }, { IM_ROUND(CalcAnglesMax[0]), IM_ROUND(CalcAnglesMax[1]) }, Green());
			}
		}
	}
}

void RageDraw()
{
	if (IsCurWeaponNonAttack() || !bAliveLocal() || !cvar.rage_draw_aim)
		return;

	if (iTargetRage)
	{
		for (playeraim_t Aim : PlayerAim)
		{
			if (Aim.ent->index != iTargetRage)
				continue;
			for (unsigned int i = 0; i < 12; i++)
			{
				float CalcAnglesMin[2], CalcAnglesMax[2];
				if (WorldToScreen(Aim.PlayerAimHitbox[iHitboxRage].HitboxMulti[SkeletonHitboxMatrix[i][0]], CalcAnglesMin) && WorldToScreen(Aim.PlayerAimHitbox[iHitboxRage].HitboxMulti[SkeletonHitboxMatrix[i][1]], CalcAnglesMax))
					ImGui::GetCurrentWindow()->DrawList->AddLine({ IM_ROUND(CalcAnglesMin[0]), IM_ROUND(CalcAnglesMin[1]) }, { IM_ROUND(CalcAnglesMax[0]), IM_ROUND(CalcAnglesMax[1]) }, Green());
			}
		}
	}
}

void LegitDraw()
{
	if (!IsCurWeaponGun() || !bAliveLocal() || !cvar.legit_draw_aim)
		return;

	if (iTargetLegit)
	{
		for (playeraim_t Aim : PlayerAim)
		{
			if (Aim.ent->index != iTargetLegit)
				continue;
			for (unsigned int i = 0; i < 12; i++)
			{
				float CalcAnglesMin[2], CalcAnglesMax[2];
				if (WorldToScreen(Aim.PlayerAimHitbox[iHitboxLegit].HitboxMulti[SkeletonHitboxMatrix[i][0]], CalcAnglesMin) && WorldToScreen(Aim.PlayerAimHitbox[iHitboxLegit].HitboxMulti[SkeletonHitboxMatrix[i][1]], CalcAnglesMax))
					ImGui::GetCurrentWindow()->DrawList->AddLine({ IM_ROUND(CalcAnglesMin[0]), IM_ROUND(CalcAnglesMin[1]) }, { IM_ROUND(CalcAnglesMax[0]), IM_ROUND(CalcAnglesMax[1]) }, Green());
			}
		}
	}
}

void TriggerDrawTarget(playeraim_t Aim)
{
	bool hitboxselected = false;

	for (model_aim_select_t Model_Selected : Model_Aim_Select)
	{
		if (!strstr(Model_Selected.checkmodel, Aim.modelname))
			continue;

		hitboxselected = true;

		Vector vEye = pmove->origin + pmove->view_ofs;

		bool bHitboxPointsVisible[8];

		for (unsigned int point = 0; point < 8; ++point)
		{
			pmtrace_t tr;

			g_Engine.pEventAPI->EV_SetTraceHull(2);

			if (cvar.bypass_trace_trigger)
				g_Engine.pEventAPI->EV_PlayerTrace(vEye, Aim.PlayerAimHitbox[Model_Selected.numhitbox].HitboxMulti[point], PM_WORLD_ONLY, -1, &tr);
			else
				g_Engine.pEventAPI->EV_PlayerTrace(vEye, Aim.PlayerAimHitbox[Model_Selected.numhitbox].HitboxMulti[point], PM_GLASS_IGNORE, -1, &tr);

			int detect = g_Engine.pEventAPI->EV_IndexFromTrace(&tr);

			if ((cvar.bypass_trace_trigger && tr.fraction == 1 && !detect) || (!cvar.bypass_trace_trigger && detect == Aim.ent->index))
				bHitboxPointsVisible[point] = true;
			else
			{
				bHitboxPointsVisible[point] = false;

				int iOriginalPenetration = CurPenetration();

				if (iOriginalPenetration && cvar.legit[g_Local.weapon.m_iWeaponID].trigger_wall)
				{
					int iDamage = CurDamage();
					int iBulletType = CurBulletType();
					float flDistance = CurDistance();
					float flRangeModifier = CurWallPierce();

					int iCurrentDamage = FireBullets(vEye, Aim.PlayerAimHitbox[Model_Selected.numhitbox].HitboxMulti[point], flDistance, iOriginalPenetration, iBulletType, iDamage, flRangeModifier);

					if (iCurrentDamage > 0)
						bHitboxPointsVisible[point] = true;
				}
			}
		}

		for (unsigned int i = 0; i < 12; i++)
		{
			if (bHitboxPointsVisible[SkeletonHitboxMatrix[i][0]] && bHitboxPointsVisible[SkeletonHitboxMatrix[i][1]])
			{
				float CalcAnglesMin[2], CalcAnglesMax[2];
				if (WorldToScreen(Aim.PlayerAimHitbox[Model_Selected.numhitbox].HitboxMulti[SkeletonHitboxMatrix[i][0]], CalcAnglesMin) && WorldToScreen(Aim.PlayerAimHitbox[Model_Selected.numhitbox].HitboxMulti[SkeletonHitboxMatrix[i][1]], CalcAnglesMax))
					ImGui::GetCurrentWindow()->DrawList->AddLine({ IM_ROUND(CalcAnglesMin[0]), IM_ROUND(CalcAnglesMin[1]) }, { IM_ROUND(CalcAnglesMax[0]), IM_ROUND(CalcAnglesMax[1]) }, Green());
			}
		}
	}
	if (!hitboxselected)
	{
		Vector vEye = pmove->origin + pmove->view_ofs;

		bool bHitboxPointsVisible[8];

		for (unsigned int point = 0; point < 8; ++point)
		{
			pmtrace_t tr;

			int detect = 0;

			g_Engine.pEventAPI->EV_SetTraceHull(2);

			if (cvar.bypass_trace_trigger)
				g_Engine.pEventAPI->EV_PlayerTrace(vEye, Aim.PlayerAimHitbox[HeadBox[Aim.ent->index]].HitboxMulti[point], PM_WORLD_ONLY, -1, &tr);
			else
				g_Engine.pEventAPI->EV_PlayerTrace(vEye, Aim.PlayerAimHitbox[HeadBox[Aim.ent->index]].HitboxMulti[point], PM_GLASS_IGNORE, -1, &tr);

			detect = g_Engine.pEventAPI->EV_IndexFromTrace(&tr);

			if ((cvar.bypass_trace_trigger && tr.fraction == 1 && !detect) || (!cvar.bypass_trace_trigger && detect == Aim.ent->index))
				bHitboxPointsVisible[point] = true;
			else
			{
				bHitboxPointsVisible[point] = false;

				int iOriginalPenetration = CurPenetration();

				if (iOriginalPenetration && cvar.legit[g_Local.weapon.m_iWeaponID].trigger_wall)
				{
					int iDamage = CurDamage();
					int iBulletType = CurBulletType();
					float flDistance = CurDistance();
					float flRangeModifier = CurWallPierce();

					int iCurrentDamage = FireBullets(vEye, Aim.PlayerAimHitbox[HeadBox[Aim.ent->index]].HitboxMulti[point], flDistance, iOriginalPenetration, iBulletType, iDamage, flRangeModifier);

					if (iCurrentDamage > 0)
						bHitboxPointsVisible[point] = true;
				}
			}
		}

		for (unsigned int i = 0; i < 12; i++)
		{
			if (bHitboxPointsVisible[SkeletonHitboxMatrix[i][0]] && bHitboxPointsVisible[SkeletonHitboxMatrix[i][1]])
			{
				float CalcAnglesMin[2], CalcAnglesMax[2];
				if (WorldToScreen(Aim.PlayerAimHitbox[HeadBox[Aim.ent->index]].HitboxMulti[SkeletonHitboxMatrix[i][0]], CalcAnglesMin) && WorldToScreen(Aim.PlayerAimHitbox[HeadBox[Aim.ent->index]].HitboxMulti[SkeletonHitboxMatrix[i][1]], CalcAnglesMax))
					ImGui::GetCurrentWindow()->DrawList->AddLine({ IM_ROUND(CalcAnglesMin[0]), IM_ROUND(CalcAnglesMin[1]) }, { IM_ROUND(CalcAnglesMax[0]), IM_ROUND(CalcAnglesMax[1]) }, Green());
			}
		}
	}
}

void TriggerDraw()
{
	if (!cvar.legit[g_Local.weapon.m_iWeaponID].trigger_active || !IsCurWeaponGun() || !bAliveLocal())
		return;

	static DWORD dwTemporaryBlockTimer = GetTickCount();
	static bool old = TriggerKeyStatus;

	if (cvar.legit_trigger_key > 0 && cvar.legit_trigger_key < 255)
	{
		if (TriggerKeyStatus != old)
		{
			old = TriggerKeyStatus;
			dwTemporaryBlockTimer = GetTickCount();
		}

		if (GetTickCount() - dwTemporaryBlockTimer < 2000)
		{
			ImVec2 Screen;
			Screen.x = ImGui::GetWindowSize().x / 2, Screen.y = ImGui::GetWindowSize().y / 2 + ImGui::GetWindowSize().y / 20 * 1;

			if (TriggerKeyStatus)
			{
				int label_size = ImGui::CalcTextSize("Trigger activated", NULL, true).x;
				ImGui::GetCurrentWindow()->DrawList->AddRect({ Screen.x - label_size / 2 - 2, Screen.y }, { Screen.x - label_size / 2 + label_size + 3, Screen.y + 14 }, Green());
				ImGui::GetCurrentWindow()->DrawList->AddText({ Screen.x - label_size / 2, Screen.y}, White(), "Trigger activated");
			}
			else
			{
				int label_size = ImGui::CalcTextSize("Trigger deactivated", NULL, true).x;
				ImGui::GetCurrentWindow()->DrawList->AddRect({ Screen.x - label_size / 2 - 2, Screen.y }, { Screen.x - label_size / 2 + label_size + 3, Screen.y + 14 }, Red());
				ImGui::GetCurrentWindow()->DrawList->AddText({ Screen.x - label_size / 2, Screen.y}, White(), "Trigger deactivated");
			}
		}
	}

	if (!cvar.legit_trigger_draw_aim)
		return;

	if (iTargetTrigger)
	{
		for (playeraim_t Aim : PlayerAim)
		{
			if (Aim.ent->index != iTargetTrigger)
				continue;

			TriggerDrawTarget(Aim);
		}
	}
}

void DrawAimBotFOV()
{
	for (fov_t FOV : FOVDraw)
		ImGui::GetCurrentWindow()->DrawList->AddCircle({ IM_ROUND(FOV.Pos.x), IM_ROUND(FOV.Pos.y) }, FOV.Radius, ImColor().HSV(FOV.r, FOV.g, FOV.b, FOV.a), 32);
}

void DrawAimbot()
{
	if (cvar.rage_active)
		RageDraw();
	else
	{
		if (cvar.legit[g_Local.weapon.m_iWeaponID].active)
			LegitDraw();
		else
			TriggerDraw();

		KnifeDraw();
	}
}

void UTIL_TextureHit(Vector vecSrc, Vector vecEnd, Vector vecDir, int ignore, pmtrace_t& tr)
{
	if (vecSrc[0] == vecEnd[0] && vecSrc[1] == vecEnd[1] && vecSrc[2] == vecEnd[2])
	{
		memset(&tr, NULL, sizeof(pmtrace_t));

		tr.endpos = vecEnd;
		tr.fraction = 1.0f;
		return;
	}

	g_Engine.pEventAPI->EV_SetTraceHull(2);
	g_Engine.pEventAPI->EV_PlayerTrace(vecSrc, vecEnd, PM_GLASS_IGNORE, ignore, &tr);

	if (tr.fraction == 0 && tr.startsolid && !tr.allsolid)
	{
		Vector vecTmp = vecSrc;

		while (!tr.allsolid && tr.fraction == 0.f)
		{
			vecTmp = vecTmp + vecDir;

			g_Engine.pEventAPI->EV_SetTraceHull(2);
			g_Engine.pEventAPI->EV_PlayerTrace(vecTmp, vecEnd, PM_WORLD_ONLY, ignore, &tr);
		}

		if (!tr.allsolid && tr.fraction != 1.0f)
		{
			vecTmp = vecEnd - vecSrc;

			float Length1 = vecTmp.Length();

			vecTmp = tr.endpos - vecSrc;

			float Length2 = vecTmp.Length();

			tr.fraction = Length1 / Length2;
			tr.startsolid = 1.0f;
		}
	}

	if (tr.allsolid)
		tr.fraction = 1.0f;
}

int FireBullets(Vector start, Vector end, float flDistance, int iOriginalPenetration, int iBulletType, int iDamage, float flRangeModifier)
{
	Vector vecSrc, vecEnd, vecDir, vecTmp;
	int iPenetration = iOriginalPenetration + 1;
	int iPenetrationPower;
	int iCurrentDamage = iDamage;
	float flPenetrationDistance;
	float flCurrentDistance;

	pmtrace_t tr;
	vecSrc = start;
	vecEnd = end;

	switch (iBulletType)
	{
	case BULLET_PLAYER_9MM:
		iPenetrationPower = 21;
		flPenetrationDistance = 800;
		break;
	case BULLET_PLAYER_45ACP:
		iPenetrationPower = 15;
		flPenetrationDistance = 500;
		break;
	case BULLET_PLAYER_50AE:
		iPenetrationPower = 30;
		flPenetrationDistance = 1000;
		break;
	case BULLET_PLAYER_762MM:
		iPenetrationPower = 39;
		flPenetrationDistance = 5000;
		break;
	case BULLET_PLAYER_556MM:
		iPenetrationPower = 35;
		flPenetrationDistance = 4000;
		break;
	case BULLET_PLAYER_338MAG:
		iPenetrationPower = 45;
		flPenetrationDistance = 8000;
		break;
	case BULLET_PLAYER_57MM:
		iPenetrationPower = 30;
		flPenetrationDistance = 2000;
		break;
	case BULLET_PLAYER_357SIG:
		iPenetrationPower = 25;
		flPenetrationDistance = 800;
		break;
	default:
		iPenetrationPower = 0;
		flPenetrationDistance = 0;
		break;
	}

	vecDir = vecEnd - vecSrc;

	float Length = vecDir.Length();

	vecDir /= Length;

	vecEnd = vecDir * flDistance + vecSrc;

	float flDamageModifier = 0.5;

	while (iPenetration)
	{
		UTIL_TextureHit(vecSrc, vecEnd, vecDir, -1, tr);

		char cTextureType = GetTextureType(&tr, vecSrc, vecEnd);

		switch (cTextureType)
		{
		case CHAR_TEX_CONCRETE:
			iPenetrationPower *= 0.25;
			break;
		case CHAR_TEX_GRATE:
			iPenetrationPower *= 0.5;
			flDamageModifier = 0.4;
			break;
		case CHAR_TEX_METAL:
			iPenetrationPower *= 0.15;
			flDamageModifier = 0.2;
			break;
		case CHAR_TEX_COMPUTER:
			iPenetrationPower *= 0.4;
			flDamageModifier = 0.45;
			break;
		case CHAR_TEX_TILE:
			iPenetrationPower *= 0.65;
			flDamageModifier = 0.3;
			break;
		case CHAR_TEX_VENT:
			iPenetrationPower *= 0.5;
			flDamageModifier = 0.45;
			break;
		case CHAR_TEX_WOOD:
			flDamageModifier = 0.6;
			break;
		default:
			break;
		}

		if (tr.fraction != 1.0)
		{
			iPenetration--;

			vecTmp = tr.endpos - start;

			float tmplen = vecTmp.Length();

			if (tmplen >= Length)
			{
				vecTmp = end - vecSrc;

				float Length1 = vecTmp.Length();

				vecTmp = vecEnd - vecSrc;

				float Length2 = vecTmp.Length();

				tr.fraction = Length1 / Length2;

				flCurrentDistance = flDistance * tr.fraction;

				iCurrentDamage *= pow(flRangeModifier, flCurrentDistance / 500);

				return iCurrentDamage;
			}

			flCurrentDistance = flDistance * tr.fraction;

			iCurrentDamage *= pow(flRangeModifier, flCurrentDistance / 500);

			if (flCurrentDistance > flPenetrationDistance)
				iPenetration = 0;

			if (iPenetration)
			{
				vecSrc = iPenetrationPower * vecDir + tr.endpos;

				flDistance = (flDistance - flCurrentDistance) * 0.5;

				vecEnd = vecDir * flDistance + vecSrc;

				flCurrentDistance = iCurrentDamage;

				iCurrentDamage = flCurrentDistance * flDamageModifier;
			}
			else
			{
				vecSrc = 42 * vecDir + tr.endpos;

				flDistance = (flDistance - flCurrentDistance) * 0.75;

				vecEnd = vecDir * flDistance + vecSrc;

				iCurrentDamage *= 0.75;
			}
		}
		else iPenetration = 0;
	}
	return 0;
}

void ContinueFire(struct usercmd_s* cmd)
{
	if (cvar.rage_active && cmd->buttons & IN_ATTACK && IsCurWeaponPistol() && !g_Local.weapon.m_iInReload && bAliveLocal())
	{
		static bool bFire = false;

		if (CanAttack() && bFire)
		{
			cmd->buttons |= IN_ATTACK;
			bFire = false;
		}
		else if (!bFire)
		{
			cmd->buttons &= ~IN_ATTACK;
			bFire = true;
		}
	}
}