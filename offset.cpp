#include "client.h"

AutoOffset c_Offset;

#define CompareMemory(Buff1, Buff2, Size) __comparemem((const UCHAR *)Buff1, (const UCHAR *)Buff2, (UINT)Size)
#define FindMemoryClone(Start, End, Clone, Size) __findmemoryclone((const ULONG)Start, (const ULONG)End, (const ULONG)Clone, (UINT)Size)
#define FindReference(Start, End, Address) __findreference((const ULONG)Start, (const ULONG)End, (const ULONG)Address)

typedef void* Pointer;
typedef void** PPointer;
typedef unsigned Cardinal;
typedef unsigned* PCardinal;
typedef unsigned** PPCardinal;
typedef int** PPLongint;

bool AutoOffset::GetRendererInfo()
{
	DWORD GameUI = (DWORD)GetModuleHandle("GameUI.dll");
	DWORD vgui = (DWORD)GetModuleHandle("vgui.dll");
	DWORD vgui2 = (DWORD)GetModuleHandle("vgui2.dll");
	DWORD d3dim = (DWORD)GetModuleHandle("d3dim.dll");

	HwBase = (DWORD)GetModuleHandle("hw.dll"); // Hardware

	if (HwBase == NULL)
	{
		HwBase = (DWORD)GetModuleHandle("sw.dll"); // Software
		if (HwBase == NULL)
		{
			HwBase = (DWORD)GetModuleHandle(NULL); // Non-Steam?
			if (HwBase == NULL) // Invalid module handle.
			{
				Error("Invalid module handle.");
			}
			else
				HLType = RENDERTYPE_UNDEFINED;
		}
		else
			HLType = RENDERTYPE_SOFTWARE;
	}
	else
	{
		if (d3dim == NULL)
			HLType = RENDERTYPE_HARDWARE;
		else
			HLType = RENDERTYPE_D3D;
	}

	HwSize = (DWORD)GetModuleSize(HwBase);

	if (HwSize == NULL)
	{
		switch (HwSize)
		{
		case RENDERTYPE_HARDWARE: HwSize = 0x122A000; break;
		case RENDERTYPE_UNDEFINED: HwSize = 0x2116000; break;
		case RENDERTYPE_SOFTWARE: HwSize = 0xB53000; break;
		default:Error("Invalid renderer type.");
		}
	}

	HwEnd = HwBase + HwSize - 1;

	ClBase = (DWORD)GetModuleHandle("client.dll");
	
	if (ClBase != NULL) {
		ClSize = (DWORD)GetModuleSize(ClBase);
		ClEnd = ClBase + ClSize - 1;
	}
	else {
		ClBase = HwBase;
		ClEnd = HwEnd;
		ClSize = HwSize;
	}

	if (GameUI != NULL)
	{
		UiBase = GameUI;
		UiSize = (DWORD)GetModuleSize(UiBase);
		UiEnd = UiBase + UiSize - 1;
	}

	HlBase = (DWORD)GetModuleHandle(NULL);
	HlSize = (DWORD)GetModuleSize(HlBase);
	HlEnd = HlBase + HlSize - 1;

	return (HwBase && ClBase && GameUI && vgui && vgui2 && HlBase);
}

unsigned AutoOffset::Absolute(DWORD Addr)
{
	return Cardinal(Addr) + *(PCardinal)(Addr)+4;
}

void AutoOffset::Error(char* fmt, ...)
{
	va_list va_alist;
	char buf[256];
	va_start(va_alist, fmt);
	vsprintf_s(buf, fmt, va_alist);
	va_end(va_alist);

	MessageBox(0, buf, "Fatal Error", MB_OK | MB_ICONERROR);

	ExitProcess(0);
}

DWORD AutoOffset::GetModuleSize(const DWORD Address)
{
	return PIMAGE_NT_HEADERS(Address + (DWORD)PIMAGE_DOS_HEADER(Address)->e_lfanew)->OptionalHeader.SizeOfImage;
}

DWORD AutoOffset::FarProc(const DWORD Address, DWORD LB, DWORD HB)
{
	return ((Address < LB) || (Address > HB));
}

BOOL AutoOffset::__comparemem(const UCHAR *buff1, const UCHAR *buff2, UINT size)
{
	for (UINT i = 0; i < size; i++, buff1++, buff2++)
	{
		if ((*buff1 != *buff2) && (*buff2 != 0xFF))
			return FALSE;
	}
	return TRUE;
}

ULONG AutoOffset::__findmemoryclone(const ULONG start, const ULONG end, const ULONG clone, UINT size)
{
	for (ULONG ul = start; (ul + size) < end; ul++)
	{
		if (CompareMemory(ul, clone, size))
			return ul;
	}
	return NULL;
}

ULONG AutoOffset::__findreference(const ULONG start, const ULONG end, const ULONG address)
{
	UCHAR Pattern[5];
	Pattern[0] = 0x68;
	*(ULONG*)&Pattern[1] = address;
	return FindMemoryClone(start, end, Pattern, sizeof(Pattern)-1);
}

bool AutoOffset::EnablePageWrite(DWORD addr, DWORD size)
{
	return VirtualProtect((void*)addr, size, PAGE_EXECUTE_READWRITE, &dwOldPageProtection) != 0;
}

bool AutoOffset::RestorePageProtection(DWORD addr, DWORD size)
{
	bool ret = VirtualProtect((void*)addr, size, dwOldPageProtection, &dwOldPageProtection) != 0;
	FlushInstructionCache(GetCurrentProcess(), (void*)addr, size);
	return ret;
}

void native_memwrite(uintptr_t adr, uintptr_t ptr, int size)
{
	DWORD dwOldProtect;
	VirtualProtect(LPVOID(adr), size, PAGE_EXECUTE_READWRITE, &dwOldProtect);
	memcpy(LPVOID(adr), LPVOID(ptr), size);
	VirtualProtect(LPVOID(adr), size, dwOldProtect, &dwOldProtect);
}

DWORD FindPattern(PCHAR pattern, PCHAR mask, DWORD start, DWORD end, DWORD offset)
{
	int patternLength = lstrlenA(pattern);
	bool found = false;
	for (DWORD i = start; i < end - patternLength; i++)
	{
		found = true;
		for (unsigned int idx = 0; idx < patternLength; idx++)
		{
			if (mask[idx] == 'x' && pattern[idx] != *(PCHAR)(i + idx))
			{
				found = false;
				break;
			}
		}
		if (found)
		{
			return i + offset;
		}
	}
	return 0;
}

DWORD FindPattern(PCHAR pattern, DWORD start, DWORD end, DWORD offset)
{
	if (start > end)
	{
		DWORD reverse = end;
		end = start;
		start = reverse;
	}
	size_t patternLength = strlen(pattern);
	bool found = false;
	for (DWORD i = start; i < end - patternLength; i++)
	{
		found = true;

		for (size_t idx = 0; idx < patternLength; idx++)
		{
			if (pattern[idx] != *(PCHAR)(i + idx))
			{
				found = false;
				break;
			}
		}

		if (found)
			return i + offset;
	}
	return 0;
}

DWORD FindPush(DWORD start, DWORD end, PCHAR Message)
{
	char bPushAddrPattern[] = { 0x68 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 };
	DWORD Address = FindPattern(Message, start, end, 0);
	*(PDWORD)& bPushAddrPattern[1] = Address;
	Address = FindPattern((PCHAR)bPushAddrPattern, start, end, 0);
	return Address;
}

BOOL bCompareMemory(LPBYTE bAddress, LPBYTE bCode, UINT uSize, BOOL bPattern)
{
	UINT i;
	for (i = 0; i < uSize; i++, bCode++, bAddress++) {
		if ((*bAddress != *bCode) && (!bPattern || *bCode != 0xFF))
			return FALSE;
	}
	return TRUE;
}

PVOID AutoOffset::ClientFuncs()
{
	DWORD Old = NULL;
	PCHAR String = "ScreenFade";
	DWORD Address = (DWORD)FindMemoryClone(HwBase, HwEnd, String, strlen(String));
	PVOID ClientPtr = (PVOID)*(PDWORD)(FindReference(HwBase, HwEnd, Address) + 0x13); // all patch

	if (FarProc((DWORD)ClientPtr, HwBase, HwEnd))
		Error("Couldn't find %s.",__FUNCTION__);

	VirtualProtect(ClientPtr,sizeof(double),PAGE_READWRITE,&Old);

	return ClientPtr;
}

PVOID AutoOffset::EngineFuncs()
{
	DWORD Old = NULL;
	////PVOID EnginePtr = (cl_enginefunc_t*)*(DWORD*)((DWORD)g_pClient->Initialize + 0x0D); // old patch
	//PVOID EnginePtr = (cl_enginefunc_t*)*(DWORD*)((DWORD)g_pClient->Initialize + 0x22); // old patch
	//if (FarProc((DWORD)EnginePtr, HwBase, HwEnd) && FarProc((DWORD)EnginePtr, HlBase, HlEnd))
	//{
	//	EnginePtr = (cl_enginefunc_t*)*(DWORD*)((DWORD)g_pClient->Initialize + 0x1C); // new patch
	//	if (FarProc((DWORD)EnginePtr, ClBase, ClEnd))
	//	{
	//		EnginePtr = (cl_enginefunc_t*)*(DWORD*)((DWORD)g_pClient->Initialize + 0x1D); // steam
	//		if (FarProc((DWORD)EnginePtr, ClBase, ClEnd))
	//		{
	//			EnginePtr = (cl_enginefunc_t*)*(DWORD*)((DWORD)g_pClient->Initialize + 0x37); // hl-steam
	//			if (FarProc((DWORD)EnginePtr, ClBase, ClEnd))
	//			{
	//				EnginePtr = (cl_enginefunc_t*)*(DWORD*)((DWORD)g_pClient->Initialize + 0x0D); // hl-steam
	//				if (FarProc((DWORD)EnginePtr, ClBase, ClEnd))
	//				{
	//					Error("Couldn't find %s.", __FUNCTION__);
	//				}
	//			}
	//		}
	//	}
	//}

	PCHAR String = "ScreenFade";
	DWORD Address = (DWORD)FindMemoryClone(HwBase, HwEnd, String, strlen(String));
	PVOID EnginePtr = (PVOID) * (PDWORD)(FindReference(HwBase, HwEnd, Address) + 0x0D);

	VirtualProtect(EnginePtr,sizeof(double),PAGE_READWRITE,&Old);

	return EnginePtr;
}

PVOID AutoOffset::StudioFuncs()
{
	DWORD Old = NULL;
	PVOID StudioPtr = (engine_studio_api_t*)*(DWORD*)((DWORD)g_pClient->HUD_GetStudioModelInterface + 0x30); // old patch
	if (FarProc((DWORD)StudioPtr, HwBase, HwEnd) && FarProc((DWORD)StudioPtr, HlBase, HlEnd))
	{
		StudioPtr = (engine_studio_api_t*)*(DWORD*)((DWORD)g_pClient->HUD_GetStudioModelInterface + 0x1A); // new patch / steam	

		if (FarProc((DWORD)StudioPtr, ClBase, ClEnd))
			Error("Couldn't find %s.", __FUNCTION__);
	}

	VirtualProtect(StudioPtr,sizeof(double),PAGE_READWRITE,&Old);

	return StudioPtr;
}

PUserMsg AutoOffset::FindUserMsgBase(void)
{
	DWORD Address = (DWORD)g_Engine.pfnHookUserMsg;

	DWORD UserMsgBase = Absolute(FindPattern("\x52\x50\xE8\xFF\xFF\xFF\xFF\x83", "xxx????x", Address, Address + 0x32, 3));

	if (FarProc(UserMsgBase, HwBase, HwEnd))
		Error("Couldn't find %s.", __FUNCTION__);

	UserMsgBase = FindPattern("\xFF\xFF\xFF\x0C\x56\x8B\x35\xFF\xFF\xFF\xFF\x57", "???xxxx????x", UserMsgBase, UserMsgBase + 0x32, 7);

	if (FarProc(UserMsgBase, HwBase, HwEnd))
		Error("Couldn't find %s2.", __FUNCTION__);

	return PUserMsg(**(PDWORD*)UserMsgBase);
}

DWORD AutoOffset::CL_Move(void)
{
	DWORD Address = FindPattern("\x56\x57\x33\xFF\x3B\xC7\x0F\x84\x00\x00\x00\x00\x83\xF8\x01\x0F\x84\x00\x00\x00\x00\x83\xF8\x02\x0F\x84\x00\x00\x00\x00\x83\xF8\x03\x75\x22", "xxxxxxx????xxxxx????xxxxx????xxxxx", HwBase, HwEnd, 0);

	if (FarProc((DWORD)Address, HwBase, HwEnd))
		Error("Couldn't find %s.", __FUNCTION__);
	else 
	{
		Address = FindPattern("\xC3\x90", "xx", Address - 0x12, HwEnd, 0x2);

		if (FarProc((DWORD)Address, HwBase, HwEnd))
			Error("Couldn't find %s2.", __FUNCTION__);
	}

	return Address;
}

void AutoOffset::GlobalTime()
{
	dwSendPacketPointer = FindPattern("\x75\x13\xD9\x05\x00\x00\x00\x00\xD8\x1D\x00\x00\x00\x00\xDF\xE0\xF6\xC4\x00\x00\x00\xD9\x05\x00\x00\x00\x00\xDC\x1D\x00\x00\x00\x00\xDF\xE0\xF6\xC4\x41", "xxxx????xx????xxxx???xx????xx????xxxxx", HwBase, HwEnd, 0x1b) + 2;

	if (FarProc(dwSendPacketPointer, HwBase, HwEnd))
		Error("Couldn't find %s.", __FUNCTION__);

	dwSendPacketBackup = *((uintptr_t *)(dwSendPacketPointer));

	EnablePageWrite(dwSendPacketPointer, sizeof(DWORD));
}

DWORD AutoOffset::FindSpeed(void)
{
	DWORD Address = FindPattern("Texture load: %6.1fms", HwBase, HwEnd, 0);
	PVOID SpeedPtr = (PVOID)*(DWORD*)(FindReference(HwBase, HwEnd, Address) - 7);

	if (FarProc((DWORD)SpeedPtr, HwBase, HwEnd))
		Error("Couldn't find %s.", __FUNCTION__);
	else
		EnablePageWrite((DWORD)SpeedPtr, sizeof(double));

	return (DWORD)SpeedPtr;
}

PVOID AutoOffset::FindPlayerMove(void)
{
	//DWORD Address = FindPattern("ScreenFade", HwBase, HwEnd, 0);
	////PVOID Ptr = (PVOID)*(PDWORD)(FindReference(HwBase, HwEnd, Address) + 0x18);
	//PVOID Ptr = (PVOID) * (PDWORD)(FindReference(HwBase, HwEnd, Address) + 0x24);

	//if (FarProc((DWORD)Ptr, HwBase, HwEnd))
	//	Error("Couldn't find %s.", __FUNCTION__);

	//return Ptr;
	PCHAR String = "ScreenFade";
	DWORD Address = (DWORD)FindMemoryClone(HwBase, HwEnd, String, strlen(String));
	PVOID Ptr = (PVOID) * (PDWORD)(FindReference(HwBase, HwEnd, Address) + 0x24);
	return Ptr;
}

DWORD AutoOffset::FindStudioModelRenderer(DWORD StudioDrawModel)
{
	PDWORD retAddress = (*(PDWORD*)((DWORD)StudioDrawModel + 0x05));
	if (retAddress) return *retAddress;
	Error("Couldn't find %s.", __FUNCTION__);
}

void AutoOffset::PatchInterpolation(void)
{
	static bool patched = false;

	if (patched)
		return;

	DWORD Address = FindPush(HwBase, HwEnd, "cl_updaterate minimum is %f") - 0x18;

	native_memwrite(Address, (uintptr_t)"\xEB", 1);

	Address = FindPush(HwBase, HwEnd, "cl_updaterate maximum is %f") - 0x18;

	native_memwrite(Address, (uintptr_t)"\xEB", 1);

	Address = FindPush(HwBase, HwEnd, "ex_interp forced up to %i msec\n");
	Address = FindPattern("\x7D", Address - 0x9, Address, 0);

	native_memwrite(Address, (uintptr_t)"\xEB", 1);

	Address = FindPush(HwBase, HwEnd, "ex_interp forced down to %i msec\n") - 0x5;

	native_memwrite(Address, (uintptr_t)"\xEB", 1);

	patched = true;
}

DWORD AutoOffset::PreS_DynamicSound(void)
{
	DWORD Address = Absolute(FindPush(HwBase, HwEnd, "CL_Parse_Sound: ent = %i, cl.max_edicts %i") - 0x10);

	if (FarProc(Address, HwBase, HwEnd))
	{
		Address = Absolute(FindPush(HwBase, HwEnd, "CL_Parse_Sound: ent = %i, cl.max_edicts %i") - 0x11);

		if (FarProc(Address, HwBase, HwEnd))
			Error("Couldn't find %s.", __FUNCTION__);

		return Address;
	}

	return Address;
}

DWORD AutoOffset::Steam_GSInitiateGameConnection(void)
{
	DWORD Address = FindPattern("%c%c%c%cconnect %i %i \"%s\" \"%s\"\n", HwBase, HwEnd, 0);

	Address = FindPattern((PCHAR)"\xE8\xFF\xFF\xFF\xFF\x83\xC4\x1C\x8B\xF8",
		(PCHAR)"x????xxxxx", HwBase, HwEnd, 0);

	return Address;
}

DWORD AutoOffset::FindGameConsole()
{
	DWORD PatternAddress = FindPattern("GameConsole003", UiBase, UiEnd, 0);
	DWORD ReferenAddress = FindReference(UiBase, UiEnd, PatternAddress) + 0x21;

	if (FarProc(ReferenAddress, UiBase, UiEnd))
	{
		Error("Couldn't find %s.", __FUNCTION__);
		return 0;
	}

	DWORD GameConsole = *(PDWORD)ReferenAddress;

	return GameConsole;
}

void AutoOffset::ConsoleColorInitalize()
{
	DWORD GameConsole = FindGameConsole();

	if (GameConsole)
	{
		DWORD Panel = (*(PDWORD)(GameConsole + 8) - GameConsole);

		Console_TextColor = PColor24(Panel + GameConsole + 288 + sizeof(DWORD));

		if (*(PDWORD)(DWORD(Console_TextColor) + 8) != 0)
		{
			Console_TextColor = PColor24(Panel + GameConsole + 288 + (sizeof(DWORD) * 2));
		}
	}
}

DWORD FindCodeAddress(DWORD dwStart, DWORD dwEnd, LPBYTE bCode, UINT CodeSize, INT OpcodeNum, BOOL bPattern)
{
	DWORD i;
	for (i = dwStart; (i + CodeSize) < dwEnd; i++)
	{
		if (bCompareMemory((LPBYTE)i, bCode, CodeSize, bPattern))
			return (DWORD)(i + OpcodeNum);
	}
	return 0;
}

DWORD AutoOffset::FindInterface(DWORD GetStudioModelInterfaceAddress)
{
	BYTE studioPattern[] = { 0xbf,0xff,0xff,0xff,0xff,0xc7 };
	PDWORD retAddress = (PDWORD)FindCodeAddress(GetStudioModelInterfaceAddress, GetStudioModelInterfaceAddress + 0x50, studioPattern, 6, 7, TRUE);
	if (retAddress)
	{
		return *retAddress;
	}
	Error("Couldn't find %s.", __FUNCTION__);
}

int* AutoOffset::FindSkyTexNumber()
{
	static DWORD Address;
	if (!Address)
	{
		Address = FindPattern("\xD9\x05\xFF\xFF\xFF\xFF\xD8\x4D\xFF\xDE\xC1\xDC\x1D\xFF\xFF\xFF\xFF\xDF\xE0\xF6\xC4\xFF\x0F\xFF\xFF\xFF\xFF\xFF\x8B\x04\xB5\xFF\xFF\xFF\xFF\x8B\x0C\x85\xFF\xFF\xFF\xFF\xFF\xE8\xFF\xFF\xFF\xFF",
			"xx????xx?xxxx????xxxx?x?????xxx????xxx?????x????", HwBase, HwBase + HwSize, 0) + 0x26;

		if (FarProc((DWORD)Address, HwBase, HwEnd))
		{
			Address = FindPattern("\xD9\x05\xFF\xFF\xFF\xFF\xD8\x4C\x24\xFF\xDE\xC1\xDC\x1D\xFF\xFF\xFF\xFF\xDF\xE0\xF6\xC4\xFF\x0F\xFF\xFF\xFF\xFF\xFF\x8B\x04\xB5\xFF\xFF\xFF\xFF\x8B\x0C\x85\xFF\xFF\xFF\xFF\xFF\xE8\xFF\xFF\xFF\xFF",
				"xx????xxx?xxxx????xxxx?x?????xxx????xxx?????x????", HwBase, HwEnd, 0) + 0x27;

			if (FarProc((DWORD)Address, HwBase, HwEnd))
				Error("Couldn't find %s.", __FUNCTION__);
		}
	}

	return (int*)*(DWORD*)(Address);
}