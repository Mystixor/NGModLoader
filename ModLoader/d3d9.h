#pragma once
#include <unknwn.h>

typedef interface IDirect3D9 IDirect3D9;
typedef IDirect3D9 IDirect3D9Ex;
typedef DWORD D3DCOLOR;

typedef int(*D3DPERF_BeginEvent_t)(
	D3DCOLOR col,
	LPCWSTR wszName
);

typedef int(*D3DPERF_EndEvent_t)(
	void
);

typedef DWORD(*D3DPERF_GetStatus_t)(
	void
);

typedef BOOL(*D3DPERF_QueryRepeatFrame_t)(
	void
);

typedef void(*D3DPERF_SetMarker_t)(
	D3DCOLOR col,
	LPCWSTR wszName
);

typedef int(*D3DPERF_SetOptions_t)(
	DWORD dwOptions
);

typedef void(*D3DPERF_SetRegion_t)(
	D3DCOLOR col,
	LPCWSTR wszName
);

typedef HRESULT(*DebugSetLevel_t)(
	DWORD dw1
);

typedef void(*DebugSetMute_t)(
	void
);

typedef int(*Direct3D9EnableMaximizedWindowedModeShim_t)(
	BOOL mEnable
);

typedef IDirect3D9*(*Direct3DCreate9_t)(
	UINT SDKVersion
);

typedef HRESULT(*Direct3DCreate9Ex_t)(
	UINT			SDKVersion,
	IDirect3D9Ex**	unnamedParam2
);

typedef void(*Direct3DShaderValidatorCreate9_t)(
	void
);

typedef HRESULT(*PSGPError_t)(
	void
);

typedef HRESULT(*PSGPSampleTexture_t)(
	void
);

extern D3DPERF_BeginEvent_t Original_D3DPERF_BeginEvent;
extern D3DPERF_EndEvent_t Original_D3DPERF_EndEvent;
extern D3DPERF_GetStatus_t Original_D3DPERF_GetStatus;
extern D3DPERF_QueryRepeatFrame_t Original_D3DPERF_QueryRepeatFrame;
extern D3DPERF_SetMarker_t Original_D3DPERF_SetMarker;
extern D3DPERF_SetOptions_t Original_D3DPERF_SetOptions;
extern D3DPERF_SetRegion_t Original_D3DPERF_SetRegion;
extern DebugSetLevel_t Original_DebugSetLevel;
extern DebugSetMute_t Original_DebugSetMute;
extern Direct3D9EnableMaximizedWindowedModeShim_t Original_Direct3D9EnableMaximizedWindowedModeShim;
extern Direct3DCreate9_t Original_Direct3DCreate9;
extern Direct3DCreate9Ex_t Original_Direct3DCreate9Ex;
extern Direct3DShaderValidatorCreate9_t Original_Direct3DShaderValidatorCreate9;
extern PSGPError_t Original_PSGPError;
extern PSGPSampleTexture_t Original_PSGPSampleTexture;

extern HMODULE dll;

extern "C"
{
	__declspec(dllexport) int WINAPI D3DPERF_BeginEvent(
		D3DCOLOR col,
		LPCWSTR wszName
	);

	__declspec(dllexport) int WINAPI D3DPERF_EndEvent(
		void
	);

	__declspec(dllexport) DWORD WINAPI D3DPERF_GetStatus(
		void
	);

	__declspec(dllexport) DWORD WINAPI D3DPERF_QueryRepeatFrame(
		void
	);

	__declspec(dllexport) void WINAPI D3DPERF_SetMarker(
		D3DCOLOR col,
		LPCWSTR wszName
	);

	__declspec(dllexport) int WINAPI D3DPERF_SetOptions(
		DWORD dwOptions
	);

	__declspec(dllexport) void WINAPI D3DPERF_SetRegion(
		D3DCOLOR col,
		LPCWSTR wszName
	);

	__declspec(dllexport) HRESULT WINAPI DebugSetLevel(
		DWORD dw1
	);
	
	__declspec(dllexport) void DebugSetMute(
		void
	);

	__declspec(dllexport) int WINAPI Direct3D9EnableMaximizedWindowedModeShim(
		BOOL mEnable
	);

	__declspec(dllexport) IDirect3D9* WINAPI Direct3DCreate9(
		UINT SDKVersion
	);

	__declspec(dllexport) HRESULT Direct3DCreate9Ex(
		UINT			SDKVersion,
		IDirect3D9Ex** unnamedParam2
	);

	__declspec(dllexport) void Direct3DShaderValidatorCreate9(
		void
	);

	__declspec(dllexport) HRESULT WINAPI PSGPError(
		void
	);

	__declspec(dllexport) HRESULT WINAPI PSGPSampleTexture(
		void
	);
}