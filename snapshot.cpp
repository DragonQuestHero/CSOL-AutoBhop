#include "client.h"

glReadPixels_t glReadPixels_s;
bool ScreenFirst = true, bAntiSSTemp = true, DrawVisuals, FirstFrame = true;
PBYTE BufferScreen;
int temp;
DWORD dwSize, time_scr;

void __stdcall m_glReadPixels(GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, GLvoid* pixels)
{
	if (ScreenFirst || !cvar.snapshot)
	{
		dwSize = (width * height) * 3;
		BufferScreen = (PBYTE)malloc(dwSize);
		glReadPixels_s(x, y, width, height, format, type, pixels);
		memcpy(BufferScreen, pixels, dwSize);
		DrawVisuals = true;
		ScreenFirst = false;
		return;
	}
	memcpy(pixels, BufferScreen, dwSize);
}

void Snapshot()
{
	if (!bInitializeImGui)
		return;

	if (cvar.snapshot)
	{
		if (bAntiSSTemp)
		{
			time_scr = GetTickCount();
			temp = 0;

			bAntiSSTemp = false;
		}

		if (GetTickCount() - time_scr > cvar.snapshot_time * 1000)
		{
			DrawVisuals = false;
			temp++;

			if (temp > 4)
			{
				bAntiSSTemp = true;
				ScreenFirst = true;
				DWORD sz = ImGui::GetIO().DisplaySize.x * ImGui::GetIO().DisplaySize.y * 3;
				free((PBYTE)BufferScreen);
				PBYTE buf = (PBYTE)malloc(sz);
				glReadPixels(0, 0, ImGui::GetIO().DisplaySize.x, ImGui::GetIO().DisplaySize.y, GL_RGB, GL_UNSIGNED_BYTE, buf);
				free((PBYTE)buf);
			}
		}
	}
	if (FirstFrame)
	{
		DWORD sz = ImGui::GetIO().DisplaySize.x * ImGui::GetIO().DisplaySize.y * 3;
		PBYTE buf = (PBYTE)malloc(sz);
		glReadPixels(0, 0, ImGui::GetIO().DisplaySize.x, ImGui::GetIO().DisplaySize.y, GL_RGB, GL_UNSIGNED_BYTE, buf);
		free((PBYTE)buf);

		FirstFrame = false;
	}
}