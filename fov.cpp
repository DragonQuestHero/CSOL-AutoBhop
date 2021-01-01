#include "client.h"

void CustomFOV()
{
	int fov = cvar.visual_custom_fov;
	if (bAliveLocal() && DrawVisuals && (!cvar.route_auto || cvar.route_draw_visual) && GetTickCount() - HudRedraw <= 100)
	{
		if (g_Local.iFOV == 90)(*pSetFOV)("SetFOV", 1, &fov);
	}
	else
	{
		fov = 90;
		if (g_Local.iFOV == 90)(*pSetFOV)("SetFOV", 1, &fov);
	}
}