#include "pch.h"


#ifdef _DEBUG
//	Mutex to make sure std::cout never prints twice at the same time.
std::mutex g_coutMutex;

//	Setup the console for debug information.
static void ConsoleSetup(FILE*& fOUT, FILE*& fERR, FILE*& fIN)
{
	AllocConsole();
	SetConsoleTitle(L"ModLoader Debug");
	freopen_s(&fOUT, "CONOUT$", "w", stdout);
	freopen_s(&fERR, "CONOUT$", "w", stderr);
	freopen_s(&fIN, "CONIN$", "r", stdin);
}
#endif


typedef int32_t(__fastcall* GETDATABINITEMSIZE)(int16_t databinItemIdx);
static GETDATABINITEMSIZE GetDatabinItemSize;

//	This function usually gets the size of a file stored in a databin from the databin itself. For mod files instead we need to determine the size on the disk every time it is needed, since the mod could change.
static int32_t Hook_GetDatabinItemSize(int16_t databinItemIdx)
{
#ifdef _DEBUG
	{
		std::lock_guard<std::mutex> guard(g_coutMutex);
		std::cout << std::format("Determining file size of file number {:05d}", databinItemIdx) << std::endl;
	}
#endif

	std::ifstream modFile(std::format("mods\\{:05d}.dat", databinItemIdx), std::ios::binary);
	if (modFile)
	{
		modFile.seekg(0, std::ios::end);
		return modFile.tellg();
	}
	return GetDatabinItemSize(databinItemIdx);
}


typedef int64_t(__fastcall* LOADDATABINITEM)(int64_t param_1, uint64_t databinItemIdx, uint64_t param_3);
static LOADDATABINITEM LoadDatabinItem;

//	This function loads a file from the databin archive. We let the game do this 
static int64_t Hook_LoadDatabinItem(int64_t param_1, uint64_t databinItemIdx, uint64_t param_3)
{
#ifdef _DEBUG
	{
		std::lock_guard<std::mutex> guard(g_coutMutex);
		std::cout << std::format("Load file number {:05d}", databinItemIdx) << std::endl;
	}
#endif

	// The purpose of the return value is currently unknown. It does not look like the game uses it though anyway.
	int64_t returnVal = 0;

	//	Try to load a mod file from disk. If successful, the mod is loaded into memory. Otherwise, we let the game load it from databin as usual.
	std::ifstream modFile(std::format("mods\\{:05d}.dat", databinItemIdx), std::ios::binary);
	if (modFile)
	{
		modFile.seekg(0, std::ios::end);
		int length = modFile.tellg();
		modFile.seekg(0, std::ios::beg);
		modFile.read((char*)param_3, length);
		modFile.close();
		
#ifdef _DEBUG
		{
			std::lock_guard<std::mutex> guard(g_coutMutex);
			std::cout << "[+] Loaded mod for aforementioned file." << std::endl;
		}
#endif
	}
	else
	{
		returnVal = LoadDatabinItem(param_1, databinItemIdx, param_3);
	}

	return returnVal;
}


DWORD WINAPI HackThread(HMODULE hModule)
{
#ifdef _DEBUG
	FILE* fOUT;
	FILE* fERR;
	FILE* fIN;
	ConsoleSetup(fOUT, fERR, fIN);
#endif

	//	gamemodule.dll is the underlying NG Sigma 2 instance, which is responsible for loading the databin archive.
	uintptr_t moduleBase = (uintptr_t)GetModuleHandle(L"gamemodule.dll");

	//	Original code addresses which we need to detour.
	LoadDatabinItem		= (LOADDATABINITEM)	(moduleBase + 0x14736d0);
	GetDatabinItemSize	= (GETDATABINITEMSIZE)(moduleBase + 0x1471990);
	
	//	Detour GetDatabinItemSize
	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());
	DetourAttach(&(LPVOID&)GetDatabinItemSize, (PBYTE)Hook_GetDatabinItemSize);
	DetourTransactionCommit();

	//	Detour LoadDatabinItem
	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());
	DetourAttach(&(LPVOID&)LoadDatabinItem, (PBYTE)Hook_LoadDatabinItem);
	DetourTransactionCommit();

	while (true)
	{
		//	We keep the thread alive, but don't need to do anything from here anymore.
		Sleep(1000);
	}

	//	This code will not be hit as of now, but I guess it does not hurt to keep it here in case this ever changes.
	FreeLibraryAndExitThread(hModule, 0);
	return 0;
}


BOOL APIENTRY DllMain( HMODULE hModule,
					   DWORD  ul_reason_for_call,
					   LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		DisableThreadLibraryCalls(hModule);
		CloseHandle(CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)HackThread, hModule, 0, nullptr));
		break;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

