#include "client.h"

#include <thread>

char hackdir[256];
char hldir[256];

typedef void(NTAPI* _UNKNOW)();

void WorkThread()
{
	while (true)
	{
		//hw.pplx::details::windows_scheduler::schedule+3A6481 - 89 0D 4474B537        - mov [hw.dll+A57444],ecx { [37DCA7B1] }
		ULONG temp_addr2 = (ULONG)GetModuleHandleA("hw.dll");
		temp_addr2 += 0xA57444;
		ULONG* temp_addr = (ULONG*)temp_addr2;
		*temp_addr = 0xFFFFFFFF;
		Sleep(1);

		if (GetAsyncKeyState(VK_F3) & 1)
		{
			ULONG temp_addr2 = (ULONG)GetModuleHandleA("client.dll");
			temp_addr2 += 0xDB22C4;
			temp_addr = (ULONG*)temp_addr2;
			if (*temp_addr == 2)
			{
				temp_addr2 = (ULONG)GetModuleHandleA("client.dll");
				temp_addr2 += 0xE39360;
				temp_addr = (ULONG*)temp_addr2;
				*temp_addr = 1;


				temp_addr2 = (ULONG)GetModuleHandleA("client.dll");
				temp_addr2 += 0x631110;

				_UNKNOW UNKNOW = (_UNKNOW)temp_addr2;
				UNKNOW();
			}
		}
	}
	
}



DWORD WINAPI Hook()
{
	while (!c_Offset.GetRendererInfo())
		Sleep(100);

	g_pClient = (cl_clientfunc_t*)c_Offset.ClientFuncs();

	if (g_pClient == nullptr)
	{
		MessageBoxA(NULL, NULL, NULL, NULL);
	}

	std::thread t1(&WorkThread);
	t1.detach();


	

	g_pEngine = (cl_enginefunc_t*)c_Offset.EngineFuncs();

	//g_pStudio = (engine_studio_api_t*)c_Offset.StudioFuncs();

	//g_pInterface = (r_studio_interface_t*)c_Offset.FindInterface((DWORD)*g_pClient->HUD_GetStudioModelInterface);

	//g_pStudioModelRenderer = (StudioModelRenderer_t*)c_Offset.FindStudioModelRenderer((DWORD)g_pInterface->StudioDrawModel);

	//glReadPixels_s = (glReadPixels_t)DetourFunction((PBYTE)GetProcAddress(GetModuleHandle("opengl32.dll"), "glReadPixels"), (PBYTE)m_glReadPixels);

	//PreS_DynamicSound_s = (PreS_DynamicSound_t)DetourFunction((LPBYTE)c_Offset.PreS_DynamicSound(), (LPBYTE)&PreS_DynamicSound);

	//CL_Move_s = (CL_Move_t)DetourFunction((LPBYTE)c_Offset.CL_Move(), (LPBYTE)&CL_Move);

	/*c_Offset.GlobalTime();

	c_Offset.dwSpeedPointer = (DWORD)c_Offset.FindSpeed();

	c_Offset.PatchInterpolation();

	c_Offset.ConsoleColorInitalize();*/

	/*LoadCvar();

	LoadHitbox();*/

	while (!g_Client.V_CalcRefdef)
		RtlCopyMemory(&g_Client, g_pClient, sizeof(cl_clientfunc_t));

	while (!g_Engine.V_CalcShake)
		RtlCopyMemory(&g_Engine, g_pEngine, sizeof(cl_enginefunc_t));

	/*while (!g_Studio.StudioSetupSkin)
		RtlCopyMemory(&g_Studio, g_pStudio, sizeof(engine_studio_api_t));

	while (!g_Interface.StudioDrawModel)
		RtlCopyMemory(&g_Interface, g_pInterface, sizeof(r_studio_interface_t));

	while (!g_StudioModelRenderer.StudioSlerpBones)
		RtlCopyMemory(&g_StudioModelRenderer, g_pStudioModelRenderer, sizeof(StudioModelRenderer_t));*/

	//while (!pmove)
	pmove = (playermove_t*)c_Offset.FindPlayerMove();
	/*char p[10] = { 0 };
	itoa((ULONG)pmove, p, 10);
	OutputDebugStringA(std::string("pmove:" + std::string(p)).data());*/

	/*while (!pUserMsgBase)
		pUserMsgBase = c_Offset.FindUserMsgBase();

	if (c_Offset.HLType != RENDERTYPE_UNDEFINED)
	{
		g_pfnSteam_GSInitiateGameConnection = (decltype(g_pfnSteam_GSInitiateGameConnection))GetDestination(c_Offset.Steam_GSInitiateGameConnection());
		SetDestination(c_Offset.Steam_GSInitiateGameConnection(), (uintptr_t)&Steam_GSInitiateGameConnection_Hooked);
	}*/

	HookClientFunctions();

	/*HookStudioFunctions();

	HookStudiModelRendererFunctions();

	HookUserMessages();

	HookOpenGL();

	InitBass();*/

	return NULL;
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD dwReason, LPVOID lpReserved)
{
	if (dwReason == DLL_PROCESS_ATTACH)
	{
		if (GetLastError() != ERROR_ALREADY_EXISTS)
		{
			DisableThreadLibraryCalls(hinstDLL);
			GetModuleFileName(hinstDLL, hackdir, 255);
			char* pos = hackdir + strlen(hackdir);
			while (pos >= hackdir && *pos != '\\')
				--pos;
			pos[1] = 0;
			GetModuleFileName(GetModuleHandle(NULL), hldir, 255);
			pos = hldir + strlen(hldir);
			while (pos >= hldir && *pos != '\\')
				--pos;
			pos[1] = 0;
			CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)Hook, NULL, NULL, NULL);
		}
	}
	return TRUE;
}