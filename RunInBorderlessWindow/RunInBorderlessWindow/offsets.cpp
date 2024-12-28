#include "common.h"
#include <cstddef>
#define CINTERFACE
#include <dxgi1_2.h>
#include <dinput.h>

namespace Constants
{
    const ptrdiff_t IDXGIFactory1_CreateSwapChain_Offset = offsetof(IDXGIFactory1Vtbl, CreateSwapChain);
    const ptrdiff_t IDXGIFactory2_CreateSwapChainForHwnd_Offset = offsetof(IDXGIFactory2Vtbl, CreateSwapChainForHwnd);
    const ptrdiff_t IDXGISwapChain_SetFullscreenState_Offset = offsetof(IDXGISwapChainVtbl, SetFullscreenState);
    const ptrdiff_t IDXGISwapChain_Present_Offset = offsetof(IDXGISwapChainVtbl, Present);

    const ptrdiff_t IDirectInput8_CreateDevice_Offset = offsetof(IDirectInput8WVtbl, CreateDevice);
    const ptrdiff_t IDirectInputDevice8_SetCooperativeLevel = offsetof(IDirectInputDevice8WVtbl, SetCooperativeLevel);
}
