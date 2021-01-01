#include "client.h"

bool WorldToScreen(float* pflOrigin, float* pflVecScreen)
{
	int iResult = g_Engine.pTriAPI->WorldToScreen(pflOrigin, pflVecScreen);
	if (pflVecScreen[0] < 1.5f && pflVecScreen[1] < 1.5f && pflVecScreen[0] > -1.5f && pflVecScreen[1] > -1.5f && !iResult && bInitializeImGui)
	{
		pflVecScreen[0] = pflVecScreen[0] * (ImGui::GetIO().DisplaySize.x / 2) + (ImGui::GetIO().DisplaySize.x / 2);
		pflVecScreen[1] = -pflVecScreen[1] * (ImGui::GetIO().DisplaySize.y / 2) + (ImGui::GetIO().DisplaySize.y / 2);
		return true;
	}
	return false;
}

void DrawFullScreenWindow()
{
	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
	ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
	ImGui::SetNextWindowSize(ImVec2(ImGui::GetIO().DisplaySize.x, ImGui::GetIO().DisplaySize.y), ImGuiCond_Always);
	ImGui::Begin("FullScreen", reinterpret_cast<bool*>(true), ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoInputs);
	{
		DrawSpawn();
		DrawRouteLine();
		DrawLongJump();
		DrawSkeletonWorld();
		DrawPlayerSoundNoIndexEsp();
		DrawPlayerSoundIndexEsp();
		DrawSkeletonPlayer();
		DrawPlayerEsp();
		DrawAimbot();
		DrawTraceGrenade();
		DrawCrossHair();
		DrawWeaponName();
		DrawModeChange();
		DrawAimBotFOV();
	}
	ImGui::End();
	ImGui::PopStyleColor();
}