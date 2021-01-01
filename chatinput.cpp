#include "client.h"

bool bInputActive;
bool SetKeyboardFocus;
int iInputMode;
char InputBuf[256];

void DrawChatInputWindow(int buttonsize)
{
	if (bInputActive)
	{
		float y = ImGui::GetIO().DisplaySize.y -
			radiusy * cos(M_PI * 2 * 0 / 360) -
			buttonsize / 2 - 23 - 1;

		ImGui::SetNextWindowPos(ImVec2(0, y), ImGuiCond_Always, ImVec2(0, 0.5));
		ImGui::SetNextWindowSize(ImVec2(0, 0));
		ImGui::Begin("Chatinput", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
		{
			char InputTitle[256];
			if (iInputMode == 1) sprintf(InputTitle, "Chat Open");
			if (iInputMode == 2) sprintf(InputTitle, "Chat Team");
			ImGui::InputText(InputTitle, InputBuf, IM_ARRAYSIZE(InputBuf));
			if (SetKeyboardFocus && bInputActive)
			{
				ImGui::SetKeyboardFocusHere();
				SetKeyboardFocus = false;
			}
			ImGui::End();
		}
	}
	else
	{
		if(InputBuf)
			strcpy(InputBuf, "");
	}
}