#include "pch.h"

#include "d3d9.h"


//#define v1060

#define v1070


D3DPERF_BeginEvent_t						Original_D3DPERF_BeginEvent							= nullptr;
D3DPERF_EndEvent_t							Original_D3DPERF_EndEvent							= nullptr;
D3DPERF_GetStatus_t							Original_D3DPERF_GetStatus							= nullptr;
D3DPERF_QueryRepeatFrame_t					Original_D3DPERF_QueryRepeatFrame					= nullptr;
D3DPERF_SetMarker_t							Original_D3DPERF_SetMarker							= nullptr;
D3DPERF_SetOptions_t						Original_D3DPERF_SetOptions							= nullptr;
D3DPERF_SetRegion_t							Original_D3DPERF_SetRegion							= nullptr;
DebugSetLevel_t								Original_DebugSetLevel								= nullptr;
DebugSetMute_t								Original_DebugSetMute								= nullptr;
Direct3D9EnableMaximizedWindowedModeShim_t	Original_Direct3D9EnableMaximizedWindowedModeShim	= nullptr;
Direct3DCreate9_t							Original_Direct3DCreate9							= nullptr;
Direct3DCreate9Ex_t							Original_Direct3DCreate9Ex							= nullptr;
Direct3DShaderValidatorCreate9_t			Original_Direct3DShaderValidatorCreate9				= nullptr;
PSGPError_t									Original_PSGPError									= nullptr;
PSGPSampleTexture_t							Original_PSGPSampleTexture							= nullptr;

__declspec(dllexport) int D3DPERF_BeginEvent(
	D3DCOLOR col,
	LPCWSTR wszName
)
{
	if (Original_D3DPERF_BeginEvent)
	{
		return Original_D3DPERF_BeginEvent(col, wszName);
	}
	return S_FALSE;
}

__declspec(dllexport) int D3DPERF_EndEvent(
	void
)
{
	if (Original_D3DPERF_EndEvent)
	{
		return Original_D3DPERF_EndEvent();
	}
	return S_FALSE;
}

__declspec(dllexport) DWORD WINAPI D3DPERF_GetStatus(
	void
)
{
	if (Original_D3DPERF_GetStatus)
	{
		return Original_D3DPERF_GetStatus();
	}
	return NULL;
}

__declspec(dllexport) DWORD WINAPI D3DPERF_QueryRepeatFrame(
	void
)
{
	if (Original_D3DPERF_QueryRepeatFrame)
	{
		return Original_D3DPERF_QueryRepeatFrame();
	}
	return NULL;
}

__declspec(dllexport) void WINAPI D3DPERF_SetMarker(
	D3DCOLOR col,
	LPCWSTR wszName
)
{
	if (Original_D3DPERF_SetMarker)
	{
		return Original_D3DPERF_SetMarker(col, wszName);
	}
	return;
}

__declspec(dllexport) int WINAPI D3DPERF_SetOptions(
	DWORD dwOptions
)
{
	if (Original_D3DPERF_SetOptions)
	{
		return Original_D3DPERF_SetOptions(dwOptions);
	}
	return NULL;
}

__declspec(dllexport) void WINAPI D3DPERF_SetRegion(
	D3DCOLOR col,
	LPCWSTR wszName
)
{
	if (Original_D3DPERF_SetRegion)
	{
		return Original_D3DPERF_SetRegion(col, wszName);
	}
	return;
}

__declspec(dllexport) HRESULT WINAPI DebugSetLevel(
	DWORD dw1
)
{
	if (Original_DebugSetLevel)
	{
		return Original_DebugSetLevel(dw1);
	}
	return S_FALSE;
}

__declspec(dllexport) void DebugSetMute(
	void
)
{
	if (Original_DebugSetMute)
	{
		return Original_DebugSetMute();
	}
	return;
}

__declspec(dllexport) int WINAPI Direct3D9EnableMaximizedWindowedModeShim(
	BOOL mEnable
)
{
	if (Original_Direct3D9EnableMaximizedWindowedModeShim)
	{
		return Original_Direct3D9EnableMaximizedWindowedModeShim(mEnable);
	}
	return NULL;
}

__declspec(dllexport) IDirect3D9* WINAPI Direct3DCreate9(
	UINT SDKVersion
)
{
	if (Original_Direct3DCreate9)
	{
		return Original_Direct3DCreate9(SDKVersion);
	}
	return nullptr;
}

__declspec(dllexport) HRESULT Direct3DCreate9Ex(
	UINT			SDKVersion,
	IDirect3D9Ex** unnamedParam2
)
{
	if (Original_Direct3DCreate9Ex)
	{
		return Original_Direct3DCreate9Ex(SDKVersion, unnamedParam2);
	}
	return S_FALSE;
}

__declspec(dllexport) void Direct3DShaderValidatorCreate9(
	void
)
{
	if (Original_Direct3DShaderValidatorCreate9)
	{
		return Original_Direct3DShaderValidatorCreate9();
	}
	return;
}

__declspec(dllexport) HRESULT WINAPI PSGPError(
	void
)
{
	if (Original_PSGPError)
	{
		return Original_PSGPError();
	}
	return S_FALSE;
}

__declspec(dllexport) HRESULT WINAPI PSGPSampleTexture(
	void
)
{
	if (Original_PSGPSampleTexture)
	{
		return Original_PSGPSampleTexture();
	}
	return S_FALSE;
}


#ifdef ML_DEBUG
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


uintptr_t g_moduleBase;


typedef int32_t(__fastcall* GETDATABINITEMSIZE)(int16_t databinItemIdx);
static GETDATABINITEMSIZE GetDatabinItemSize;

//	This function usually gets the size of a file stored in a databin from the databin itself. For mod files instead we need to determine the size on the disk every time it is needed, since the mod could change.
static int32_t Hook_GetDatabinItemSize(int16_t databinItemIdx)
{
	int32_t databinItemSize = 0;

	std::ifstream modFile(std::format("mods\\{:05d}.dat", databinItemIdx), std::ios::binary);
	if (modFile)
	{
		modFile.seekg(0, std::ios::end);
		databinItemSize = modFile.tellg();
	}
	else
	{
		databinItemSize = GetDatabinItemSize(databinItemIdx);
	}

#ifdef ML_DEBUG
	{
		std::lock_guard<std::mutex> guard(g_coutMutex);
		std::cout << std::format("{:05d}\tGetFileSize\t{:d} bytes", databinItemIdx, databinItemSize) << std::endl;
	}
#endif

	return databinItemSize;
}


typedef int64_t(__fastcall* LOADDATABINITEM)(int64_t param_1, uint64_t databinItemIdx, uint64_t param_3);
static LOADDATABINITEM LoadDatabinItem;

//	This function loads a file from the databin archive. We let the game do this 
static int64_t Hook_LoadDatabinItem(int64_t param_1, uint64_t databinItemIdx, uint64_t param_3)
{
	// The purpose of the return value is currently unknown. It does not look like the game uses it though anyway.
	int64_t returnVal = 0;

	//	Try to load a mod file from disk. If successful, the mod is loaded into memory. Otherwise, we let the game load it from databin as usual.
	std::ifstream modFile(std::format("mods\\{:05d}.dat", databinItemIdx), std::ios::binary);
	if (modFile)
	{
#ifdef ML_DEBUG
		std::lock_guard<std::mutex> guard(g_coutMutex);
		std::cout << std::format("{:05d}\tLoadFile\tmodded", databinItemIdx) << std::endl;
#endif

		modFile.seekg(0, std::ios::end);
		int length = modFile.tellg();
		modFile.seekg(0, std::ios::beg);
		modFile.read((char*)param_3, length);
		modFile.close();


		//	We need to patch this calling operation, it reads the file from databin and decompresses it, but we already have the file in place.
#ifdef v1060
		char* patch = (char*)(g_moduleBase + 0x147382B);
#endif
#ifdef v1070
		char* patch = (char*)(g_moduleBase + 0x147413b);
#endif

		DWORD oldProtect;
		VirtualProtect(patch, 3, PAGE_EXECUTE_READWRITE, &oldProtect);

		//	mov al,1		(simulate the "return true" which we would have gotten from the patched out function)
		*(patch + 0) = 0xB0;
		*(patch + 1) = 0x01;

		//	nop
		*(patch + 2) = 0x90;

		VirtualProtect(patch, 3, oldProtect, &oldProtect);


		//	Call the original function that loads a databin item, it will perform everything normally except for the function which we just patched out.
		returnVal = LoadDatabinItem(param_1, databinItemIdx, param_3);


		//	Return the patched function call to how it was before for loading files the normal way.
		VirtualProtect(patch, 3, PAGE_EXECUTE_READWRITE, &oldProtect);

		//	call qword ptr [rax+08]
		*(patch + 0) = 0xFF;
		*(patch + 1) = 0x50;
		*(patch + 2) = 0x08;

		VirtualProtect(patch, 3, oldProtect, &oldProtect);
	}
	else
	{
#ifdef ML_DEBUG
		{
			std::lock_guard<std::mutex> guard(g_coutMutex);
			std::cout << std::format("{:05d}\tLoadFile", databinItemIdx) << std::endl;
		}
#endif
		returnVal = LoadDatabinItem(param_1, databinItemIdx, param_3);
	}
	
	return returnVal;
}


static DWORD WINAPI HackThread(HMODULE hModule)
{
	//	Get the path to the real d3d11.dll in the system directories and load it
	char dllName[MAX_PATH];
	GetSystemDirectoryA(dllName, MAX_PATH);
	strcat_s(dllName, "\\d3d9.dll");
	HMODULE dll = LoadLibraryA(dllName);

	if (dll > (HMODULE)31)
	{
		Original_D3DPERF_BeginEvent							= (D3DPERF_BeginEvent_t)						GetProcAddress(dll, "D3DPERF_BeginEvent");
		Original_D3DPERF_EndEvent							= (D3DPERF_EndEvent_t)							GetProcAddress(dll, "D3DPERF_EndEvent");
		Original_D3DPERF_GetStatus							= (D3DPERF_GetStatus_t)							GetProcAddress(dll, "D3DPERF_GetStatus");
		Original_D3DPERF_QueryRepeatFrame					= (D3DPERF_QueryRepeatFrame_t)					GetProcAddress(dll, "D3DPERF_QueryRepeatFrame");
		Original_D3DPERF_SetMarker							= (D3DPERF_SetMarker_t)							GetProcAddress(dll, "D3DPERF_SetMarker");
		Original_D3DPERF_SetOptions							= (D3DPERF_SetOptions_t)						GetProcAddress(dll, "D3DPERF_SetOptions");
		Original_D3DPERF_SetRegion							= (D3DPERF_SetRegion_t)							GetProcAddress(dll, "D3DPERF_SetRegion");
		Original_DebugSetLevel								= (DebugSetLevel_t)								GetProcAddress(dll, "DebugSetLevel");
		Original_DebugSetMute								= (DebugSetMute_t)								GetProcAddress(dll, "DebugSetMute");
		Original_Direct3D9EnableMaximizedWindowedModeShim	= (Direct3D9EnableMaximizedWindowedModeShim_t)	GetProcAddress(dll, "Direct3D9EnableMaximizedWindowedModeShim");
		Original_Direct3DCreate9							= (Direct3DCreate9_t)							GetProcAddress(dll, "Direct3DCreate9");
		Original_Direct3DCreate9Ex							= (Direct3DCreate9Ex_t)							GetProcAddress(dll, "Direct3DCreate9Ex");
		Original_Direct3DShaderValidatorCreate9				= (Direct3DShaderValidatorCreate9_t)			GetProcAddress(dll, "Direct3DShaderValidatorCreate9");
		Original_PSGPError									= (PSGPError_t)									GetProcAddress(dll, "PSGPError");
		Original_PSGPSampleTexture							= (PSGPSampleTexture_t)							GetProcAddress(dll, "PSGPSampleTexture");
	}
	else
	{
		FreeLibraryAndExitThread(hModule, 0);
		return 0;
	}


#ifdef ML_DEBUG
	FILE* fOUT;
	FILE* fERR;
	FILE* fIN;
	ConsoleSetup(fOUT, fERR, fIN);
#endif


	//	gamemodule.dll is the underlying NG Sigma 2 instance, which is responsible for loading the databin archive.
	g_moduleBase = (uintptr_t)GetModuleHandle(L"gamemodule.dll");

	//	Original code addresses which we need to detour.
#ifdef v1060
	LoadDatabinItem		= (LOADDATABINITEM)	(g_moduleBase + 0x14736d0);
	GetDatabinItemSize	= (GETDATABINITEMSIZE)(g_moduleBase + 0x1471990);
#endif
#ifdef v1070
	LoadDatabinItem		= (LOADDATABINITEM)	(g_moduleBase + 0x1473fe0);
	GetDatabinItemSize	= (GETDATABINITEMSIZE)(g_moduleBase + 0x14722a0);
#endif
	
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


	//	Attach all DLL mods
	bool modBinFound = false;
	std::filesystem::directory_iterator modBin_DirIt;
	for (int i = 0; i < 100; i++)
	{
		//	We try to find the "mods\bin\" directory 100 times, 100 milliseconds apart each.
		//	Trying more than once is necessary due to directory_iterator() failing close to application startup.
		//	Not trying infinitely is necessary because not all users would even have a "bin" folder.
		try
		{
			modBin_DirIt = std::filesystem::directory_iterator("mods\\bin\\");
			modBinFound = true;
			break;
		}
		catch (std::filesystem::filesystem_error e)
		{
			Sleep(100);
		}
	}

	if (modBinFound)
	{
#ifdef ML_DEBUG
		std::cout << "\"mods\\bin\\\" directory found. Searching for DLL mods to attach." << std::endl;
#endif

		for (const auto& entry : modBin_DirIt)
		{
			if (entry.is_directory())
			{
				//	Load DLL mod sub-directory
				std::string dllPath = (entry.path() / "main.dll").string();

				if (std::filesystem::exists(dllPath))
				{
#ifdef ML_DEBUG
					std::cout << "Attaching mod " << entry.path().filename() << "...";
#endif

					HMODULE modDLL = LoadLibraryA(dllPath.c_str());

#ifdef ML_DEBUG
					if (modDLL > (HMODULE)31)
					{
						std::cout << "\tsuccessful" << std::endl;
					}
					else
					{
						std::cout << "\tfailed" << std::endl;
					}
#endif
				}
			}
			else
			{
				//	Load single DLL mod
				std::string dllPath = entry.path().string();

#ifdef ML_DEBUG
				std::cout << "Attaching mod " << entry.path().filename() << "...";
#endif

				HMODULE modDLL = LoadLibraryA(dllPath.c_str());

#ifdef ML_DEBUG
				if (modDLL > (HMODULE)31)
				{
					std::cout << "\tsuccessful" << std::endl;
				}
				else
				{
					std::cout << "\tfailed" << std::endl;
				}
#endif
			}
		}
	}
	else
	{
#ifdef ML_DEBUG
		std::cout << "\"mods\\bin\\\" directory not found within 10 seconds. No DLL mods will be attached." << std::endl;
#endif
	}


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

