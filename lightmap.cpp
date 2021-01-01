#include "client.h"

void Lightmap()
{
	static bool runonce = true;
	if (cvar.visual_lightmap && DrawVisuals && (!cvar.route_auto || cvar.route_draw_visual) && GetTickCount() - HudRedraw <= 100)
	{
		if (runonce)
		{
			g_Engine.OverrideLightmap(1);
			g_Engine.SetLightmapColor(1, 1, 1);
			g_Engine.SetLightmapDarkness(1);
			runonce = false;
		}
	}
	else
	{
		g_Engine.OverrideLightmap(0);
		runonce = true;
	}
}