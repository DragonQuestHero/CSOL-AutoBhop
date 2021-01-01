#include "client.h"

DWORD ModeChangeDelay = 0;

void DrawModeChange()
{
	if (GetTickCount() - ModeChangeDelay < 5000)
	{
		ImVec2 Screen;
		Screen.x = 3, Screen.y = ImGui::GetWindowSize().y / 2 + ImGui::GetWindowSize().y / 20 * 5;
		int label_size = ImGui::CalcTextSize(cvar.rage_active ? "Rage Mode" : "Legit Mode", NULL, true).x;
		ImGui::GetCurrentWindow()->DrawList->AddRect({ Screen.x - 2, Screen.y + 35 }, { Screen.x + label_size + 3, Screen.y + 49 }, Wheel1());
		ImGui::GetCurrentWindow()->DrawList->AddText({ Screen.x, Screen.y + 35 }, White(), cvar.rage_active ? "Rage Mode" : "Legit Mode");
	}
}