#pragma once
#include <unknwn.h>

#include "dxgi.h"
#include "d3dcommon.h"

typedef IUnknown ID3D11Device;
typedef IUnknown ID3D11DeviceContext;

typedef HRESULT(*D3D11CreateDevice_t)(
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
    );

typedef HRESULT(*D3D11CreateDeviceAndSwapChain_t)(
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
);

extern D3D11CreateDevice_t Original_D3D11CreateDevice;
extern D3D11CreateDeviceAndSwapChain_t Original_D3D11CreateDeviceAndSwapChain;
extern HMODULE dll;

extern "C"
{
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
    );

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
    );
}