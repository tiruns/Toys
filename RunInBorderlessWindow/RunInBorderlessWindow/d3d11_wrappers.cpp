#include "common.h"
#include <Windows.h>
#include <d3dcommon.h>
#include <dxgi.h>
// #include <d3d11.h>
#include <detours/detours.h>

using ID3D11Device = IUnknown;
using ID3D11DeviceContext = IUnknown;

void* GetD3D11Function(const char* name)
{
    static auto lib = LoadLibrary(LR"(C:\Windows\System32\d3d11.dll)");
    Ensure(lib != INVALID_HANDLE_VALUE);
    auto fn = GetProcAddress(lib, name);
    Ensure(fn != nullptr);
    return fn;
}

extern "C" __declspec(dllexport)
HRESULT WINAPI D3D11CreateDevice(IDXGIAdapter* pAdapter, D3D_DRIVER_TYPE DriverType, HMODULE Software, UINT Flags,
    const D3D_FEATURE_LEVEL* pFeatureLevels, UINT FeatureLevels, UINT SDKVersion, ID3D11Device** ppDevice,
    D3D_FEATURE_LEVEL* pFeatureLevel, ID3D11DeviceContext** ppImmediateContext)
{
    using FnType = decltype(D3D11CreateDevice);
    auto fn = (FnType*)GetD3D11Function("D3D11CreateDevice");
    auto hr = fn(pAdapter, DriverType, Software, Flags, pFeatureLevels, FeatureLevels, SDKVersion, ppDevice, pFeatureLevel, ppImmediateContext);

    if (SUCCEEDED(hr))
        TryHookDXGIFactory(*ppDevice);
    return hr;
}

extern "C" __declspec(dllexport)
HRESULT WINAPI D3D11CreateDeviceAndSwapChain(IDXGIAdapter* pAdapter, D3D_DRIVER_TYPE DriverType, HMODULE Software,
    UINT Flags, const D3D_FEATURE_LEVEL* pFeatureLevels, UINT FeatureLevels, UINT SDKVersion,
    const DXGI_SWAP_CHAIN_DESC* pSwapChainDesc, IDXGISwapChain** ppSwapChain, ID3D11Device** ppDevice,
    D3D_FEATURE_LEVEL* pFeatureLevel, ID3D11DeviceContext** ppImmediateContext)
{
    using FnType = decltype(D3D11CreateDeviceAndSwapChain);
    auto fn = (FnType*)GetD3D11Function("D3D11CreateDeviceAndSwapChain");
    auto hr =  fn(pAdapter, DriverType, Software, Flags, pFeatureLevels, FeatureLevels, SDKVersion,
        pSwapChainDesc, ppSwapChain, ppDevice, pFeatureLevel, ppImmediateContext);

    if (SUCCEEDED(hr))
        TryHookDXGIFactory(*ppDevice);
    return hr;
}
