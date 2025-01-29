#pragma once
#include <unknwn.h>

#include "d3d9helper.h"

typedef IDirect3D9 IDirect3D9Ex;

typedef HRESULT(*Direct3DCreate9Ex_t)(
	UINT			SDKVersion,
	IDirect3D9Ex**	unnamedParam2
);

typedef int(*D3DPERF_BeginEvent_t)(
	D3DCOLOR col,
	LPCWSTR wszName
);

typedef int(*D3DPERF_EndEvent_t)(
	void
);

extern Direct3DCreate9Ex_t Original_Direct3DCreate9Ex;
extern D3DPERF_BeginEvent_t Original_D3DPERF_BeginEvent;
extern D3DPERF_EndEvent_t Original_D3DPERF_EndEvent;
extern HMODULE dll;

extern "C"
{
	__declspec(dllexport) HRESULT Direct3DCreate9Ex(
		UINT			SDKVersion,
		IDirect3D9Ex**	unnamedParam2
	);

	__declspec(dllexport) int D3DPERF_BeginEvent(
		D3DCOLOR col,
		LPCWSTR wszName
	);

	__declspec(dllexport) int D3DPERF_EndEvent(
		void
	);
}