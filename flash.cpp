#include "client.h"

void NoFlash()
{
	if (cvar.visual_rem_flash && DrawVisuals && (!cvar.route_auto || cvar.route_draw_visual) && GetTickCount() - HudRedraw <= 100)
	{
		screenfade_t sf;
		g_Engine.pfnGetScreenFade(&sf);
		if (sf.fadeEnd > g_Engine.GetClientTime() && sf.fader == 255 && sf.fadeg == 255 && sf.fadeb == 255 && sf.fadealpha == 255)
		{
			sf.fader = 0;
			sf.fadeg = 0;
			sf.fadealpha = 128;
			g_Engine.pfnSetScreenFade(&sf);
		}
	}
	else
	{
		screenfade_t sf;
		g_Engine.pfnGetScreenFade(&sf);
		if (sf.fadeEnd > g_Engine.GetClientTime() && sf.fader == 0 && sf.fadeg == 0 && sf.fadeb == 255 && sf.fadealpha == 128)
		{
			sf.fader = 255;
			sf.fadeg = 255;
			sf.fadealpha = 255;
			g_Engine.pfnSetScreenFade(&sf);
		}
	}
}