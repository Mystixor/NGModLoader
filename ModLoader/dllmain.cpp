#include "pch.h"

#include "d3d11.h"


D3D11CreateDevice_t Original_D3D11CreateDevice = nullptr;
D3D11CreateDeviceAndSwapChain_t Original_D3D11CreateDeviceAndSwapChain = nullptr;

//	Exported function that redirects to the respective function of the real d3d11.dll
__declspec(dllexport) HRESULT D3D11CreateDevice(
        IDXGIAdapter*               pAdapter,
        D3D_DRIVER_TYPE             DriverType,
        HMODULE                     Software,
        UINT                        Flags,
        const D3D_FEATURE_LEVEL*    pFeatureLevels,
        UINT                        FeatureLevels,
        UINT                        SDKVersion,
        ID3D11Device**              ppDevice,
        D3D_FEATURE_LEVEL*          pFeatureLevel,
        ID3D11DeviceContext**       ppImmediateContext
)
{
	if (Original_D3D11CreateDevice)
	{
		return Original_D3D11CreateDevice(pAdapter, DriverType, Software, Flags, pFeatureLevels, FeatureLevels, SDKVersion, ppDevice, pFeatureLevel, ppImmediateContext);
	}
	return S_FALSE;
}

__declspec(dllexport) HRESULT D3D11CreateDeviceAndSwapChain(
        IDXGIAdapter*               pAdapter,
        D3D_DRIVER_TYPE             DriverType,
        HMODULE                     Software,
        UINT                        Flags,
        const D3D_FEATURE_LEVEL*    pFeatureLevels,
        UINT                        FeatureLevels,
        UINT                        SDKVersion,
        const DXGI_SWAP_CHAIN_DESC* pSwapChainDesc,
        IDXGISwapChain**            ppSwapChain,
        ID3D11Device**              ppDevice,
        D3D_FEATURE_LEVEL*          pFeatureLevel,
        ID3D11DeviceContext**       ppImmediateContext
)
{
	if (Original_D3D11CreateDeviceAndSwapChain)
	{
		return Original_D3D11CreateDeviceAndSwapChain(pAdapter, DriverType, Software, Flags, pFeatureLevels, FeatureLevels, SDKVersion, pSwapChainDesc, ppSwapChain, ppDevice, pFeatureLevel, ppImmediateContext);
	}
	return S_FALSE;
}


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

#ifdef _DEBUG
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
#ifdef _DEBUG
		std::lock_guard<std::mutex> guard(g_coutMutex);
		std::cout << std::format("{:05d}\tLoadFile\tmodded", databinItemIdx) << std::endl;
#endif

		modFile.seekg(0, std::ios::end);
		int length = modFile.tellg();
		modFile.seekg(0, std::ios::beg);
		modFile.read((char*)param_3, length);
		modFile.close();


		//	We need to patch this calling operation, it reads the file from databin and decompresses it, but we already have the file in place.
		char* patch = (char*)(g_moduleBase + 0x147382B);

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
#ifdef _DEBUG
		{
			std::lock_guard<std::mutex> guard(g_coutMutex);
			std::cout << std::format("{:05d}\tLoadFile", databinItemIdx) << std::endl;
		}
#endif
		returnVal = LoadDatabinItem(param_1, databinItemIdx, param_3);
	}
	
	return returnVal;
}


DWORD WINAPI HackThread(HMODULE hModule)
{
	//	Get the path to the real d3d11.dll in the system directories and load it
	char dllName[MAX_PATH];
	GetSystemDirectoryA(dllName, MAX_PATH);
	strcat_s(dllName, "\\d3d11.dll");
	HMODULE dll = LoadLibraryA(dllName);

	if (dll > (HMODULE)31)
	{
		Original_D3D11CreateDevice				= (D3D11CreateDevice_t)				GetProcAddress(dll, "D3D11CreateDevice");
		Original_D3D11CreateDeviceAndSwapChain	= (D3D11CreateDeviceAndSwapChain_t)	GetProcAddress(dll, "D3D11CreateDeviceAndSwapChain");
	}
	else
	{
		FreeLibraryAndExitThread(hModule, 0);
		return 0;
	}


#ifdef _DEBUG
	FILE* fOUT;
	FILE* fERR;
	FILE* fIN;
	ConsoleSetup(fOUT, fERR, fIN);
#endif


	//	Attach all DLL mods
	Sleep(1000);
	std::string path = "mods\\";
	for (const auto& entry : std::filesystem::directory_iterator(path))
	{
		if (entry.is_directory())
		{
			std::string dllPath = (entry.path() / "main.dll").string();

			if (std::filesystem::exists(dllPath))
			{
#ifdef _DEBUG
				std::cout << "Attaching mod " << entry.path().filename() << "...";
#endif

				HMODULE modDLL = LoadLibraryA(dllPath.c_str());

#ifdef _DEBUG
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


	//	gamemodule.dll is the underlying NG Sigma 2 instance, which is responsible for loading the databin archive.
	g_moduleBase = (uintptr_t)GetModuleHandle(L"gamemodule.dll");

	//	Original code addresses which we need to detour.
	LoadDatabinItem		= (LOADDATABINITEM)	(g_moduleBase + 0x14736d0);
	GetDatabinItemSize	= (GETDATABINITEMSIZE)(g_moduleBase + 0x1471990);
	
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

