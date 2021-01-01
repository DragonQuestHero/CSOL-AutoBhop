#include "client.h"

float color_red = 1.f;
float color_green = 0.f;
float color_blue = 1.f;

ImColor Red()    { return ImColor(1.f, 0.f, 0.f, 1.f); }
ImColor Green()  { return ImColor(0.f, 1.f, 0.f, 1.f); }
ImColor Blue()   { return ImColor(0.f, 0.f, 1.f, 1.f); }
ImColor White()  { return ImColor(1.f, 1.f, 1.f, 1.f); }
ImColor Black()  { return ImColor(0.f, 0.f, 0.f, 1.f); }
ImColor Wheel1() { return ImColor(color_red, color_green, color_blue, 1.f); }
ImColor Wheel2() { return ImColor(color_green, color_blue, color_red, 1.f); }
ImColor Wheel3() { return ImColor(color_blue, color_red, color_green, 1.f); }

ImColor Team(int i)
{
	if (cvar.rage_active && i == iTargetRage ||
		!cvar.rage_active && cvar.legit[g_Local.weapon.m_iWeaponID].active && i == iTargetLegit ||
		!cvar.rage_active && cvar.legit[g_Local.weapon.m_iWeaponID].trigger_active && i == iTargetTrigger ||
		!cvar.rage_active && cvar.knifebot_active && i == iTargetKnife &&
		bAliveLocal() && !IsCurWeaponNonAttack())
		return Green();
	if (g_Player[i].iTeam == 1)
		return Red();
	if (g_Player[i].iTeam == 2)
		return Blue();
	return White();
}

void ColorChange()
{
	static float Color[3];
	static DWORD Tickcount = 0;
	ImGui::ColorConvertRGBtoHSV(color_red, color_green, color_blue, Color[0], Color[1], Color[2]);
	if (GetTickCount() - Tickcount > 0)
	{
		Color[0] += 0.002f;
		Tickcount = GetTickCount();
	}
	if (Color[0] < 0.0f) Color[0] += 1.0f;
	ImGui::ColorConvertHSVtoRGB(Color[0], Color[1], Color[2], color_red, color_green, color_blue);
}

void ConsolePrintColor(BYTE R, BYTE G, BYTE B, char* fmt, ...)
{
	va_list va_alist;
	char buf[256];
	va_start(va_alist, fmt);
	_vsnprintf(buf, sizeof(buf), fmt, va_alist);
	va_end(va_alist);
	TColor24 DefaultColor;
	PColor24 Ptr;
	Ptr = Console_TextColor;
	DefaultColor = *Ptr;
	Ptr->R = R;
	Ptr->G = G;
	Ptr->B = B;
	g_Engine.Con_Printf(buf);
	*Ptr = DefaultColor;
}