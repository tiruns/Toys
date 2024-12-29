#include "common.h"
#include <winrt/base.h>
#include <dxgi1_2.h>
#include <d3d11.h>
#include <Windows.h>
#include <detours/detours.h>

using FnCreateSwapChain = long(IDXGIFactory*, IUnknown*, DXGI_SWAP_CHAIN_DESC*, IDXGISwapChain**);
using FnCreateSwapChainForHwnd = long(IDXGIFactory2* This, IUnknown* pDevice,HWND hWnd, const DXGI_SWAP_CHAIN_DESC1* pDesc, const DXGI_SWAP_CHAIN_FULLSCREEN_DESC* pFullscreenDesc,IDXGIOutput* pRestrictToOutput,IDXGISwapChain1** ppSwapChain);
using FnSetFullscreenState = long(IDXGISwapChain* This, BOOL Fullscreen, IDXGIOutput* pTarget);
using FnPresent = long(IDXGISwapChain* This, UINT SyncInterval, UINT Flags);

FnCreateSwapChain* realCreateSwapChain = nullptr;
FnCreateSwapChainForHwnd* realCreateSwapChainForHwnd = nullptr;
FnSetFullscreenState* realSetFullscreenState = nullptr;
FnPresent* realPresent = nullptr;

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

void SetupBorderlessWindow(HWND hWnd)
{
    const LONG_PTR windowStyle = WS_VISIBLE | WS_POPUP;
    SetLastError(0);
    Ensure(SetWindowLongPtrW(hWnd, GWL_STYLE, windowStyle) != 0 || GetLastError() == 0);
    Ensure(SetWindowLongPtrW(hWnd, GWL_EXSTYLE, 0) != 0 || GetLastError() == 0);

    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);
    Ensure(FALSE != SetWindowPos(hWnd, HWND_TOP, 0, 0,
        screenWidth, screenHeight, SWP_FRAMECHANGED | SWP_SHOWWINDOW));
}

HRESULT FakeCreateSwapChain(IDXGIFactory* This, IUnknown* pDevice, DXGI_SWAP_CHAIN_DESC* pDesc, IDXGISwapChain** ppSwapChain)
{
    DXGI_SWAP_CHAIN_DESC newDesc{};
    if (pDesc != nullptr)
    {
        newDesc = *pDesc;
        newDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
        newDesc.Windowed = TRUE;
        SetupBorderlessWindow(newDesc.OutputWindow);
    }

    auto hr = realCreateSwapChain(This, pDevice, pDesc == nullptr ? nullptr : &newDesc, ppSwapChain);
    if (SUCCEEDED(hr))
        TryHookSwapChain(*ppSwapChain);
    return hr;
}

HRESULT FakeCreateSwapChainForHwnd(IDXGIFactory2* This, IUnknown* pDevice, HWND hWnd, const DXGI_SWAP_CHAIN_DESC1* pDesc, const DXGI_SWAP_CHAIN_FULLSCREEN_DESC* pFullscreenDesc, IDXGIOutput* pRestrictToOutput, IDXGISwapChain1** ppSwapChain)
{
    DXGI_SWAP_CHAIN_DESC1 newDesc{};
    if (pDesc != nullptr)
    {
        newDesc = *pDesc;
        newDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    }

    if (hWnd != INVALID_HANDLE_VALUE)
        SetupBorderlessWindow(hWnd);

    DXGI_SWAP_CHAIN_FULLSCREEN_DESC fullScreenDesc{};
    fullScreenDesc.Windowed = TRUE;

    auto hr = realCreateSwapChainForHwnd(This, pDevice, hWnd, pDesc == nullptr ? nullptr : &newDesc, &fullScreenDesc, pRestrictToOutput, ppSwapChain);
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

    winrt::com_ptr<IDXGIFactory1> factory1;
    Ensure(SUCCEEDED(adapter->GetParent(__uuidof(IDXGIFactory1), (void**)factory1.put())));
    
    bool isFactory2 = false;
    winrt::com_ptr<IDXGIFactory2> factory2;
    if (SUCCEEDED(adapter->GetParent(__uuidof(IDXGIFactory2), (void**)factory2.put())))
        isFactory2 = true;

    realCreateSwapChain = (FnCreateSwapChain*)GetMemeberFunctionAddress(
        factory1.get(), Constants::IDXGIFactory1_CreateSwapChain_Offset);
    if (isFactory2)
        realCreateSwapChainForHwnd = (FnCreateSwapChainForHwnd*)GetMemeberFunctionAddress(
            factory1.get(), Constants::IDXGIFactory2_CreateSwapChainForHwnd_Offset);

    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());
    DetourAttach((void**)&realCreateSwapChain, FakeCreateSwapChain);
    if (isFactory2)
        DetourAttach((void**)&realCreateSwapChainForHwnd, FakeCreateSwapChainForHwnd);

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
