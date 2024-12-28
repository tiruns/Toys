#include "common.h"
#include <winrt/base.h>
#include <dxgi.h>
#include <d3d11.h>
#include <Windows.h>
#include <detours/detours.h>

using FnCreateSwapChain = long(IDXGIFactory*, IUnknown*, DXGI_SWAP_CHAIN_DESC*, IDXGISwapChain**);
using FnSetFullscreenState = long(IDXGISwapChain* This, BOOL Fullscreen, IDXGIOutput* pTarget);
using FnPresent = long(IDXGISwapChain* This, UINT SyncInterval, UINT Flags);

FnCreateSwapChain* realCreateSwapChain = nullptr;
FnSetFullscreenState* realSetFullscreenState = nullptr;
FnPresent* realPresent = nullptr;

void* GetMemeberFunctionAddress(void* instance, ptrdiff_t offset)
{
    std::byte* vtable = *reinterpret_cast<std::byte**>(instance);
    size_t* fnPtr = reinterpret_cast<size_t*>(vtable + offset);
    return reinterpret_cast<void*>(*fnPtr);
}

HRESULT FakeSetFullscreenState(IDXGISwapChain* This, BOOL Fullscreen, IDXGIOutput* pTarget)
{
    return realSetFullscreenState(This, FALSE, pTarget);
}

HRESULT FakePresent(IDXGISwapChain* This, UINT SyncInterval, UINT Flags)
{
    return realPresent(This, SyncInterval, Flags);
}

void HookSwapChain(IDXGISwapChain* swapChain)
{
    realSetFullscreenState = (FnSetFullscreenState*)GetMemeberFunctionAddress(
        swapChain, Constants::IDXGISwapChain_SetFullscreenState_Offset);
    realPresent = (FnPresent*)GetMemeberFunctionAddress(
        swapChain, Constants::IDXGISwapChain_Present_Offset);

    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());
    DetourAttach((void**)&realSetFullscreenState, FakeSetFullscreenState);
    DetourAttach((void**)&realPresent, FakePresent);
    Ensure(NO_ERROR == DetourTransactionCommit());
}

void TryHookSwapChain(IDXGISwapChain* swapChain)
{
    static bool isHooked = false;
    if (isHooked)
        return;
    isHooked = true;

    HookSwapChain(swapChain);
}

HRESULT FakeCreateSwapChain(IDXGIFactory* This, IUnknown* pDevice, DXGI_SWAP_CHAIN_DESC* pDesc, IDXGISwapChain** ppSwapChain)
{
    pDesc->Windowed = TRUE;
    pDesc->SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;

    const LONG_PTR windowStyle = WS_VISIBLE | WS_POPUP;
    Ensure(0 != SetWindowLongPtrW(pDesc->OutputWindow, GWL_STYLE, windowStyle));
    Ensure(0 != SetWindowLongPtrW(pDesc->OutputWindow, GWL_EXSTYLE, 0));

    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);
    Ensure(FALSE != SetWindowPos(pDesc->OutputWindow, HWND_TOP, 0, 0, 
        screenWidth, screenHeight, SWP_FRAMECHANGED | SWP_SHOWWINDOW));

    auto hr = realCreateSwapChain(This, pDevice, pDesc, ppSwapChain);

    if (SUCCEEDED(hr))
        TryHookSwapChain(*ppSwapChain);

    return hr;
}

void HookDXGIFactory(ID3D11Device* d3dDevice)
{
    Ensure(d3dDevice != nullptr);

    winrt::com_ptr<IDXGIDevice1> dxgiDevice;
    Ensure(SUCCEEDED(d3dDevice->QueryInterface(dxgiDevice.put())));

    winrt::com_ptr<IDXGIAdapter> adapter;
    Ensure(SUCCEEDED(dxgiDevice->GetAdapter(adapter.put())));

    winrt::com_ptr<IDXGIFactory1> factory;
    Ensure(SUCCEEDED(adapter->GetParent(__uuidof(IDXGIFactory1), (void**)factory.put())));

    realCreateSwapChain = (FnCreateSwapChain*)GetMemeberFunctionAddress(
        factory.get(), Constants::IDXGIFactory1_CreateSwapChain_Offset);

    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());
    DetourAttach((void**)&realCreateSwapChain, FakeCreateSwapChain);
    Ensure(NO_ERROR == DetourTransactionCommit());

    return;
}

void TryHookDXGIFactory(void* d3dDevice)
{
    static bool isHooked = false;
    if (isHooked)
        return;
    isHooked = true;

    HookDXGIFactory(reinterpret_cast<ID3D11Device*>(d3dDevice));
}
