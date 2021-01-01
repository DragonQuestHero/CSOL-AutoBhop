#include "client.h"

#include <iostream>
#include <string>

deque<strafe_t> StrafeDraw;

float FrameCount;
float FpsCount;
float InterpFps;
float PreStrafe;
float JumpOff;

float Keyforwardm = false;
float Keymoveleft = false;
float Keymoveright = false;
float Keyback = false;
float Keyduck = false;
float Keyjump = false;

bool Strafe = false;
bool Fastrun = false;
bool Gstrafe = false;
bool Bhop = false;
bool Jumpbug = false;
bool Cstrafe = false;

Vector vStart, vEnd;
Vector vStartjumppos;
float flJumpdist;
float flJumpmesstime;
bool bJumped;

bool AllGo = false;
bool BhopModul = false;

float YawForVec(float* fwd)
{
	if (fwd[1] == 0 && fwd[0] == 0)
		return 0;
	else
	{
		float yaw = atan2(fwd[1], fwd[0]) * (180 / M_PI);
		if (yaw < 0)yaw += 360;
		return yaw;
	}
}


void StrafeHack(struct usercmd_s* cmd)
{
	if (GetAsyncKeyState(0x46) && !(*(ULONG*)((char*)pmove + 0xC8) == 0x208))
	{
		float dir = 0.0f;
		

		int dir_value = 1;
		if (cmd->buttons & IN_MOVERIGHT)
			dir_value = 2;
		if (cmd->buttons & IN_BACK)
			dir_value = 3;
		if (cmd->buttons & IN_MOVELEFT)
			dir_value = 4;

		if (dir_value == 1)dir = 0 * M_PI / 180.0f;
		else if (dir_value == 2)	dir = 90 * M_PI / 180.0f;
		else if (dir_value == 3)	dir = 180 * M_PI / 180.0f;
		else if (dir_value == 4)	dir = -90 * M_PI / 180.0f;


		

		//if (g_Local.fSpeed < 15.0f)
		if (sqrt(POW(pmove->velocity[0]) + POW(pmove->velocity[1])) < 15)
		{
			if (cmd->buttons & IN_FORWARD)
			{
				if (cmd->buttons & IN_MOVELEFT)
				{
					cmd->forwardmove = 900;
					cmd->sidemove = -900;
				}
				else if (cmd->buttons & IN_MOVERIGHT)
				{
					cmd->forwardmove = 900;
					cmd->sidemove = 900;
				}
				else
					cmd->forwardmove = 900;
			}
			else if (cmd->buttons & IN_BACK)
			{
				if (cmd->buttons & IN_MOVELEFT)
				{
					cmd->forwardmove = -900;
					cmd->sidemove = -900;
				}
				else if (cmd->buttons & IN_MOVERIGHT)
				{
					cmd->forwardmove = -900;
					cmd->sidemove = 900;
				}
				else
					cmd->forwardmove = -900;
			}
			else if (cmd->buttons & IN_MOVELEFT)
				cmd->sidemove = -900;
			else if (cmd->buttons & IN_MOVERIGHT)
				cmd->sidemove = 900;
			else
				cmd->forwardmove = 900;
		}
		else
		{
			float va_speed = atan2(pmove->velocity.y, pmove->velocity.x);

			float va[3] = {};
			g_Engine.GetViewAngles(va);

			float adif = va_speed - va[1] * M_PI / 180.0f - dir;

			adif = sin(adif);
			adif = atan2(adif, sqrt(1 - adif * adif));

			cmd->sidemove = (437.8928) * (adif > 0 ? 1 : -1);
			cmd->forwardmove = 0;

			float angle;
			float osin, ocos, nsin, ncos;

			angle = cmd->viewangles.y * M_PI / 180.0f;
			osin = sin(angle);
			ocos = cos(angle);

			angle = 2.0f * cmd->viewangles.y * M_PI / 180.0f - va_speed + dir;
			nsin = sin(angle);
			ncos = cos(angle);

			cmd->forwardmove = cmd->sidemove * (osin * ncos - ocos * nsin);
			cmd->sidemove *= osin * nsin + ocos * ncos;

			float fs = 0;
			if (atan2(10000 / va_speed, 1.0f) >= abs(adif))
			{
				Vector vBodyDirection;

				if (dir_value & 1)
					vBodyDirection = pmove->forward;
				else
					vBodyDirection = pmove->right;

				vBodyDirection[2] = 0;
				vBodyDirection = vBodyDirection.Normalize();

				float vel = POW(vBodyDirection[0] * pmove->velocity[0]) + POW(vBodyDirection[1] * pmove->velocity[1]);

				fs = sqrt(69.0 * 100000 / vel);
			}

			cmd->forwardmove += fs/2;
		}

		float sdmw = cmd->sidemove;
		float fdmw = cmd->forwardmove;

		switch (dir_value)
		{
		case 1:
			cmd->forwardmove = fdmw;
			cmd->sidemove = sdmw;
			break;
		case 2:
			cmd->forwardmove = -sdmw;
			cmd->sidemove = fdmw;
			break;
		case 3:
			cmd->forwardmove = -fdmw;
			cmd->sidemove = -sdmw;
			break;
		case 4:
			cmd->forwardmove = sdmw;
			cmd->sidemove = -fdmw;
			break;
		}

	}
}
//
//void StrafeHack(struct usercmd_s* cmd)
//{
//	static bool packet = false;
//	//if (GetAsyncKeyState(0x46) && !(pmove->flags & FL_ONGROUND) && (pmove->movetype != MOVETYPE_FLY) && !(cmd->buttons & IN_ATTACK) && !(cmd->buttons & IN_ATTACK2 && IsCurWeaponKnife()))
//	{
//		if (cvar.kz_strafe_psilent?!packet:1)
//		{
//			if(cvar.kz_strafe_psilent)bSendpacket(false);
//			if (sqrt(POW(pmove->velocity[0]) + POW(pmove->velocity[1])) < 15)
//				cmd->forwardmove = 400, cmd->sidemove = 0;
//
//			float dir = 0;
//			if (cmd->buttons & IN_MOVERIGHT)
//				dir = 90;
//			if (cmd->buttons & IN_BACK)
//				dir = 180;
//			if (cmd->buttons & IN_MOVELEFT)
//				dir = -90;
//
//			Vector ViewAngles;
//			g_Engine.GetViewAngles(ViewAngles);
//			ViewAngles.y += dir;
//			Vector vspeed = Vector(pmove->velocity.x / pmove->velocity.Length(), pmove->velocity.y / pmove->velocity.Length(), 0.0f);
//			float va_speed = YawForVec(vspeed);
//			float adif = va_speed - ViewAngles.y;
//			while (adif < -180)adif += 360;
//			while (adif > 180)adif -= 360;
//			cmd->sidemove = (437.8928) * (adif > 0 ? 1 : -1);
//			cmd->forwardmove = 0;
//			cmd->viewangles.y -= (-adif);
//
//			float sdmw = cmd->sidemove;
//			float fdmw = cmd->forwardmove;
//
//			if (cmd->buttons & IN_MOVERIGHT)
//				cmd->forwardmove = -sdmw, cmd->sidemove = fdmw;
//			if (cmd->buttons & IN_BACK)
//				cmd->forwardmove = -fdmw, cmd->sidemove = -sdmw;
//			if (cmd->buttons & IN_MOVELEFT)
//				cmd->forwardmove = sdmw, cmd->sidemove = -fdmw;
//		}
//		if (cvar.kz_strafe_psilent)packet = !packet;
//	}
//	/*else
//		packet = false;*/
//}

void FastRun(struct usercmd_s *cmd)
{
	//if(GetAsyncKeyState(0x45) && sqrt(POW(pmove->velocity[0]) + POW(pmove->velocity[1])) && pmove->flFallVelocity == 0 && !Gstrafe && pmove->flags&FL_ONGROUND)
	{
		static bool Run = false;
		if((cmd->buttons&IN_FORWARD && cmd->buttons&IN_MOVELEFT) || (cmd->buttons&IN_BACK && cmd->buttons&IN_MOVERIGHT))
		{
			if (Run)
			{
				Run = false;
				cmd->sidemove -= 89.6f;
				cmd->forwardmove -= 89.6f;
			}
			else
			{ 
				Run = true;
				cmd->sidemove += 89.6f; 
				cmd->forwardmove += 89.6f; 
			}
		} 
		else if((cmd->buttons&IN_FORWARD && cmd->buttons&IN_MOVERIGHT) || (cmd->buttons&IN_BACK && cmd->buttons&IN_MOVELEFT))
		{
			if (Run)
			{ 
				Run = false;
				cmd->sidemove -= 89.6f; 
				cmd->forwardmove += 89.6f; 
			}
			else			
			{ 
				Run = true;
				cmd->sidemove += 89.6f; 
				cmd->forwardmove -= 89.6f; 
			}
		} 
		else if(cmd->buttons&IN_FORWARD || cmd->buttons&IN_BACK)
		{
			if (Run)
			{ 
				Run = false;
				cmd->sidemove -= 126.6f; 
			}
			else			
			{ 
				Run = true;
				cmd->sidemove += 126.6f; 
			}
		} 
		else if(cmd->buttons&IN_MOVELEFT || cmd->buttons&IN_MOVERIGHT)
		{
			if (Run)
			{ 
				Run = false;
				cmd->forwardmove -= 126.6f; 
			}
			else			
			{ 
				Run = true;
				cmd->forwardmove += 126.6f; 
			}
		}
	}
}

void GroundStrafe(struct usercmd_s *cmd)
{
	if(Gstrafe && !Jumpbug)
	{
		static int gs_state = 0;
		if(gs_state == 0 && pmove->flags&FL_ONGROUND)
		{
			cmd->buttons |=IN_DUCK;
			gs_state = 1;
		}
		else if(gs_state == 1)
		{
			cmd->buttons &= ~IN_DUCK;
			gs_state = 0;
		}
	}
}

float HeightOrigin()
{
	Vector vTempOrigin = pmove->origin;
	vTempOrigin[2] -= 8192;
	pmtrace_t pTrace;
	g_Engine.pEventAPI->EV_SetTraceHull((pmove->flags & FL_DUCKING) ? 1 : 0);
	g_Engine.pEventAPI->EV_PlayerTrace(pmove->origin, vTempOrigin, PM_STUDIO_BOX, -1, &pTrace);

	float flHeightorigin = abs(pTrace.endpos.z - pmove->origin.z);
	g_Engine.pEventAPI->EV_SetTraceHull((pmove->flags & FL_DUCKING) ? 1 : 0);
	g_Engine.pEventAPI->EV_PlayerTrace(pmove->origin, pTrace.endpos, PM_STUDIO_BOX, -1, &pTrace);
	if (pTrace.fraction < 1.0f)//not working
	{
		flHeightorigin = abs(pTrace.endpos.z - pmove->origin.z);

		int i = g_Engine.pEventAPI->EV_IndexFromTrace(&pTrace);
		if (i > 0 && i <= g_Engine.GetMaxClients())
		{
			cl_entity_s* ent = g_Engine.GetEntityByIndex(i);
			if (ent)
			{
				float dst = pmove->origin.z - (pmove->flags & FL_DUCKING ? 18 : 32) - ent->curstate.origin.z - flHeightorigin;
				if (dst < 30) flHeightorigin -= 14.0;
			}
		}
	}
	return flHeightorigin;
}

inline float EndSpeed(float StartSpeed, float gravity, float frametime, float distance)
{
	while (distance > 0)
	{
		StartSpeed += gravity * frametime;
		float dist = StartSpeed * frametime;
		distance -= dist;
	}
	return StartSpeed;
}

inline float interp(float s1, float s2, float s3, float f1, float f3)
{
	if (s2 == s1)return f1;
	if (s2 == s3)return f3;
	if (s3 == s1)return f1;
	return f1 + ((s2 - s1) / (s3 - s1)) * ((f3 - f1)/*/1*/);
}

float Damage()
{
	Vector start = pmove->origin;
	Vector vForward, vecEnd;
	float va[3];
	g_Engine.GetViewAngles(va);
	g_Engine.pfnAngleVectors(va, vForward, NULL, NULL);
	vecEnd[0] = start[0] + vForward[0] * 8192; vecEnd[1] = start[1] + vForward[1] * 8192; vecEnd[2] = start[2] + vForward[2] * 8192;
	pmtrace_t* trace = g_Engine.PM_TraceLine(start, vecEnd, 1, 2, -1);
	float fDistance = ((start.z) - (trace->endpos.z)) - (pmove->usehull == 0 ? (36) : (18));
	float endSpeed = EndSpeed(pmove->flFallVelocity, 800, 1 / 1000.0f, fDistance);
	if (interp(504.80001f, endSpeed, 1000, 1, 100) > 0)
		return interp(504.80001f, endSpeed, 1000, 1, 100);
	else return 0;
}

float Damage2()
{
	float endSpeed = EndSpeed(pmove->flFallVelocity, 800, 1 / 1000.0f, HeightOrigin());
	if (interp(504.80001f, endSpeed, 1000, 1, 100) > 0)
		return interp(504.80001f, endSpeed, 1000, 1, 100);
	else return 0;
}

int temp3 = 10;
DWORD time_a = 0;
void BHop(struct usercmd_s* cmd)
{
	cmd->buttons &= ~IN_JUMP;
	if (*(ULONG*)((char*)pmove+0xC8) == 0x208 || *(ULONG*)((char*)pmove + 0xC8) == 0x4208 || *(ULONG*)((char*)pmove + 0xC8) == 0x18)
	{
		//time_a = GetTickCount();
		cmd->buttons |= IN_JUMP;
	}
	//if (GetTickCount() - time_a > 15)
}

void BHop3(struct usercmd_s* cmd)
{
	cmd->buttons &= ~IN_JUMP;
	cmd->buttons |= IN_DUCK;
	if (*(ULONG*)((char*)pmove + 0xC8) == 0x208)
	{
		cmd->buttons |= IN_JUMP;
		
	}

	if (*(ULONG*)((char*)pmove + 0xAC) < 0x20000000)
	{
		cmd->buttons &= ~IN_DUCK;
		cmd->buttons |= IN_JUMP;
	}
}


int temp4 = 0;
//DWORD time_a2 = 0;
bool last_duck = false;
bool duck2 = false;
void BHop2(struct usercmd_s* cmd)
{
	if (duck2)
	{
		cmd->buttons &= ~IN_DUCK;
		duck2 = false;
		return;
	}
	if (*(ULONG*)((char*)pmove + 0xC8) == 0x208)
	{
		cmd->buttons |= IN_DUCK;
		duck2 = true;
		return;
	}
	



	/*if (last_duck)
	{
		if (*(ULONG*)((char*)pmove + 0xC8) == 0x208)
		{
			cmd->buttons &= ~IN_DUCK;
		}
		last_duck = !last_duck;
	}
	else
	{
		
		if (*(ULONG*)((char*)pmove + 0xC8) == 0x208)
		{
			cmd->buttons |= IN_DUCK;
		}
		last_duck = !last_duck;
	}*/

	//time_a = GetTickCount();

	//if (temp4 < temp3)
	/*if (temp4 < 8)
	{
		temp4++;
		return;
	}
	temp4 = 0;
	if (last_duck)
	{
		cmd->buttons &= ~IN_DUCK;
		last_duck = !last_duck;
	}
	else
	{
		cmd->buttons |= IN_DUCK;
		last_duck = !last_duck;
	}*/
	//cmd->buttons &= ~IN_DUCK;
	//if (pmove->flags & FL_ONGROUND)
	//if (GetTickCount() - time_a2 > temp3)
	//{
	//	time_a = GetTickCount();
	//	cmd->buttons |= IN_DUCK;
	//}
}

double _my_abs(double n) 
{
	if (n >= 0)return n; //if positive, return without ant change
	else return 0 - n; //if negative, return a positive version
}

float GroundAngle()
{
	Vector vTemp1 = pmove->origin;
	vTemp1[2] -= 8192;
	pmtrace_t* trace = g_Engine.PM_TraceLine(pmove->origin, vTemp1, 1, (pmove->flags & FL_DUCKING) ? 1 : 0, -1);

	return acos(trace->plane.normal[2]) / M_PI * 180;
}

void JumpBug(float frametime, struct usercmd_s *cmd)
{
	static int state = 0;

	bool autojb = false;

	if (cvar.kz_jump_bug_auto && pmove->flFallVelocity >= 404.8f)
	{
		if (HeightOrigin() - (pmove->flFallVelocity * frametime / cvar.misc_wav_speed * 15) <= 0)
			autojb = true;
	}

	if ((Jumpbug || autojb) && pmove->flFallVelocity > 0)
	{
		bool curveang = false;
		float fpheight = 0;
		if (GroundAngle() > 1)
		{
			curveang = true;
			Vector vTemp = pmove->origin;
			vTemp[2] -= 8192;
			pmtrace_t* trace = g_Engine.PM_TraceLine(pmove->origin, vTemp, 1, 2, -1);
			fpheight = abs(pmove->origin.z - trace->endpos.z - (pmove->usehull == 1 ? 18.0f : 36.0f));
		}
		else fpheight = HeightOrigin();


		static float last_h = 0.0f;
		float cur_frame_zdist = abs((pmove->flFallVelocity + (800 * frametime)) * frametime);
		cmd->buttons |= IN_DUCK;
		cmd->buttons &= ~IN_JUMP;
		switch (state)
		{
		case 1:
			cmd->buttons &= ~IN_DUCK;
			cmd->buttons |= IN_JUMP;
			state = 2;
			break;
		case 2:
			state = 0;
			break;
		default:
			if (_my_abs(fpheight - cur_frame_zdist * 1.5) <= (20.0) && cur_frame_zdist > 0.0f)
			{
				float needspd = _my_abs(fpheight - (19.0));
				float scale = abs(needspd / cur_frame_zdist);
				AdjustSpeed(scale);
				state = 1;
			}
			break;
		}
		last_h = fpheight;
	}
	else state = 0;
}

inline float EdgeDistance() {
#define TraceEdge(x,y){\
    Vector start=pmove->origin;\
	start[2]-=0.1f;\
	Vector end=start;\
	end[1]+=x*mind;\
	end[0]+=y*mind;\
	pmtrace_s* t1 = g_Engine.PM_TraceLine(end,start,1,pmove->usehull,-1);\
	if(!(t1->startsolid))mind=(t1->endpos-start).Length2D();\
	}
	float mind = 250;
	TraceEdge(-1, 0);
	TraceEdge(1, 0);
	TraceEdge(0, 1);
	TraceEdge(0, -1);
	TraceEdge(-1, -1);
	TraceEdge(1, 1);
	TraceEdge(1, -1);
	TraceEdge(-1, 1);
	return mind;
}

void LongJump()
{
	if (flJumpmesstime > GetTickCount())
	{
		strafe_t Strafe;
		Strafe.Pos1 = vStart;
		Strafe.Pos2 = vEnd;
		StrafeDraw.push_back(Strafe);
	}
}

void Kz(float frametime, struct usercmd_s *cmd)
{
	//if (bAliveLocal())
	{
		/*if (GetAsyncKeyState(VK_RIGHT) & 1)
		{
			temp3++;
			char p[10] = { 0 };
			itoa(temp3, p, 10);
			OutputDebugStringA(std::string("temp3:" + std::string(p)).data());
		}
		if (GetAsyncKeyState(VK_LEFT) & 1)
		{
			temp3--;
		}*/
		if (GetAsyncKeyState(VK_F1) & 1)
		{
			AllGo = !AllGo;
		}
		if (GetAsyncKeyState(VK_F2) & 1)
		{
			BhopModul = !BhopModul;
		}
		if (AllGo)
		{
			if (GetAsyncKeyState(0x46))
				StrafeHack(cmd);
			if (GetAsyncKeyState(0x45))
				FastRun(cmd);
			/*if (cvar.kz_ground_strafe)
				GroundStrafe(cmd);*/
				//if (cvar.kz_bhop)
				/*char p[10] = { 0 };
				itoa(pmove->flags, p, 10);
				OutputDebugStringA(std::string("pmove->flags:" + std::string(p)).data());*/
				//if(Bhop)
			if (GetAsyncKeyState(VK_SPACE))
			{
				if (BhopModul == false)
				{
					BHop(cmd);
				}
				else
				{
					BHop3(cmd);
				}
			}
			if (GetAsyncKeyState(VK_CONTROL))
			{
				BHop2(cmd);
			}
		}
		
		/*if (cvar.kz_jump_bug || cvar.kz_jump_bug_auto)
			JumpBug(frametime, cmd);
		if (cvar.kz_show_kz)
			LongJump();*/
	}
	/*if (bJumped && (pmove->flags & FL_ONGROUND || pmove->movetype == MOVETYPE_FLY))
	{
		Vector endpos = pmove->origin;
		endpos.z -= pmove->usehull == 0 ? 36.0 : 18.0;
		vEnd = endpos;
		if (endpos.z == vStartjumppos.z)
		{
			Vector lj = endpos - vStartjumppos;
			float dist = lj.Length() + 32.0625f + 0.003613;
			if (dist >= 200)
			{
				InterpFps = FpsCount / FrameCount;
				FpsCount = 0;
				FrameCount = 0;
				flJumpdist = dist;
				flJumpmesstime = GetTickCount() + (int)cvar.kz_display_time * 1000;
			}
		}
		bJumped = false;
	}
	if (!bJumped && (pmove->flags & FL_ONGROUND) && cmd->buttons & IN_JUMP)
	{
		PreStrafe = sqrt(POW(pmove->velocity[0]) + POW(pmove->velocity[1]));
		if (EdgeDistance() != 250)
			JumpOff = EdgeDistance();
		else JumpOff = 0;
		vStartjumppos = pmove->origin;
		vStartjumppos.z -= pmove->usehull == 0 ? 36.0 : 18.0;
		vStart = vStartjumppos;
		bJumped = true;
	}*/
	/*if (cmd->buttons & IN_FORWARD) { Keyforwardm = true; }
	else { Keyforwardm = false; }
	if (cmd->buttons & IN_MOVELEFT) { Keymoveleft = true; }
	else { Keymoveleft = false; }
	if (cmd->buttons & IN_MOVERIGHT) { Keymoveright = true; }
	else { Keymoveright = false; }
	if (cmd->buttons & IN_BACK) { Keyback = true; }
	else { Keyback = false; }
	if (cmd->buttons & IN_DUCK) { Keyduck = true; }
	else { Keyduck = false; }
	if (cmd->buttons & IN_JUMP) { Keyjump = true;; }
	else { Keyjump = false; }*/
}

void KzFameCount()
{
	if (bJumped)
	{
		FrameCount += 1;
		FpsCount += (1 / pmove->frametime);
	}
}

void DrawLongJump()
{
	for (strafe_t Strafe : StrafeDraw)
	{
		float VecScreenMin[2];
		float VecScreenMax[2];
		if (WorldToScreen(Strafe.Pos1, VecScreenMin) && WorldToScreen(Strafe.Pos2, VecScreenMax))
			ImGui::GetCurrentWindow()->DrawList->AddLine({ IM_ROUND(VecScreenMin[0]), IM_ROUND(VecScreenMin[1]) }, { IM_ROUND(VecScreenMax[0]), IM_ROUND(VecScreenMax[1]) }, Wheel1());

		if (WorldToScreen(Strafe.Pos1, VecScreenMin))
			ImGui::GetCurrentWindow()->DrawList->AddRectFilled({ IM_ROUND(VecScreenMin[0]) - 1, IM_ROUND(VecScreenMin[1]) - 1 }, { IM_ROUND(VecScreenMin[0]) + 2, IM_ROUND(VecScreenMin[1]) + 2 }, Wheel2());

		if (WorldToScreen(Strafe.Pos2, VecScreenMax))
			ImGui::GetCurrentWindow()->DrawList->AddRectFilled({ IM_ROUND(VecScreenMax[0]) - 1, IM_ROUND(VecScreenMax[1]) - 1 }, { IM_ROUND(VecScreenMax[0]) + 2, IM_ROUND(VecScreenMax[1]) + 2 }, Wheel2());

		if (WorldToScreen(Strafe.Pos1, VecScreenMin))
		{
			float label_size = IM_ROUND(ImGui::CalcTextSize("Start", NULL, true).x / 2);
			ImGui::GetCurrentWindow()->DrawList->AddRect({ IM_ROUND(VecScreenMin[0]) - label_size - 2, IM_ROUND(VecScreenMin[1]) - 24 }, { IM_ROUND(VecScreenMin[0]) + label_size + 3, IM_ROUND(VecScreenMin[1]) - 10 }, Wheel1());
			ImGui::GetCurrentWindow()->DrawList->AddText({ IM_ROUND(VecScreenMin[0]) - label_size, IM_ROUND(VecScreenMin[1]) - 25 }, White(), "Start");
		}

		if (WorldToScreen(Strafe.Pos2, VecScreenMax))
		{
			float label_size = IM_ROUND(ImGui::CalcTextSize("Stop", NULL, true).x / 2);
			ImGui::GetCurrentWindow()->DrawList->AddRect({ IM_ROUND(VecScreenMax[0]) - label_size - 2, IM_ROUND(VecScreenMax[1]) - 24 }, { IM_ROUND(VecScreenMax[0]) + label_size + 3, IM_ROUND(VecScreenMax[1]) - 10 }, Wheel1());
			ImGui::GetCurrentWindow()->DrawList->AddText({ IM_ROUND(VecScreenMax[0]) - label_size, IM_ROUND(VecScreenMax[1]) - 25 }, White(), "Stop");
		}
	}
}

void DrawKzWindows()
{
	if (cvar.kz_show_kz && bAliveLocal())
	{
		ImVec2 windowpos;
		ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f, 0.0f, 0.0f, 0.25f));
		ImGui::SetNextWindowPos(ImVec2(20, (ImGui::GetIO().DisplaySize.y / 2) - ImGui::GetIO().DisplaySize.y / 20 * 1), ImGuiCond_Once);
		ImGui::Begin("kz", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize);
		{
			ImVec4 col_default_text = ImGui::GetStyleColorVec4(ImGuiCol_Text);
			ImVec4 col = col_default_text;
			ImVec4 col2 = col_default_text;
			static float fMaxPspeed = 0.f;
			if (sqrt(POW(pmove->velocity[0]) + POW(pmove->velocity[1])) == 0)
				fMaxPspeed = 0.0;
			if (sqrt(POW(pmove->velocity[0]) + POW(pmove->velocity[1])) > fMaxPspeed)
				fMaxPspeed = sqrt(POW(pmove->velocity[0]) + POW(pmove->velocity[1]));
			if (Damage() >= g_Local.iPostHealth)
				col = ImColor(1.f, 0.f, 0.f, 1.0f);
			if (Damage2() >= g_Local.iPostHealth)
				col2 = ImColor(1.f, 0.f, 0.f, 1.0f);

			if (flJumpmesstime > GetTickCount())
			{
				ImGui::Text("Display Time:   %.1f", (flJumpmesstime - GetTickCount()) / 1000);
				ImGui::Separator();
				ImGui::TextColored(ImVec4(1.f, 0.f, 1.f, 1.f), "Fps:            %.1f", InterpFps);
				ImGui::TextColored(ImVec4(1.f, 0.f, 1.f, 1.f), "Jump Off Edge:  %.1f", JumpOff);
				ImGui::TextColored(ImVec4(1.f, 0.f, 1.f, 1.f), "Jump Distance:  %.1f", flJumpdist);
				ImGui::TextColored(ImVec4(1.f, 0.f, 1.f, 1.f), "Start speed:    %.1f", PreStrafe);
			}
			ImGui::TextColored(col, "Damage Predict: %.1f", Damage());
			ImGui::TextColored(col2, "Damage In Fall: %.1f", Damage2());
			ImGui::Text("Height:         %.1f", HeightOrigin());
			ImGui::Text("Ground Angle:   %.1f", GroundAngle());
			ImGui::Text("Speed:          %.1f", sqrt(POW(pmove->velocity[0]) + POW(pmove->velocity[1])));
			ImGui::Text("Speed Max:      %.1f", fMaxPspeed);
			ImGui::Text("Speed In Fall:  %.1f", pmove->flFallVelocity);
			ImGui::TextColored(ImVec4(1.f, 0.f, EdgeDistance(), 1.f), "Edge Distance:  %.1f", EdgeDistance());

			windowpos.x = ImGui::GetWindowPos().x + ImGui::GetWindowSize().x + 5;
			windowpos.y = ImGui::GetWindowPos().y;
		}
		ImGui::PopStyleColor();
		ImGui::End();

		ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f, 0.0f, 0.0f, 0.25f));
		ImGui::SetNextWindowPos(ImVec2(windowpos.x, windowpos.y), ImGuiCond_Always);
		if (ImGui::Begin("kzkeys", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::Text("%s", Keyforwardm ? "  W" : "  |");
			ImGui::Text("%s %s %s", Keymoveleft ? "A" : "-", Keyback ? "S" : "+", Keymoveright ? "D" : "-");
			ImGui::Text(Keyduck ? " Duck" : "  |");
			ImGui::Text(Keyjump ? " Jump" : "  |");
		}
		ImGui::PopStyleColor();
		ImGui::End();
	}
}