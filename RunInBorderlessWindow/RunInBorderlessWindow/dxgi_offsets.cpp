#include "common.h"
#include <cstddef>
#define CINTERFACE
#include <dxgi.h>

namespace Constants
{
    const ptrdiff_t IDXGIFactory1_CreateSwapChain_Offset = offsetof(IDXGIFactory1Vtbl, CreateSwapChain);
    const ptrdiff_t IDXGISwapChain_SetFullscreenState_Offset = offsetof(IDXGISwapChainVtbl, SetFullscreenState);
    const ptrdiff_t IDXGISwapChain_Present_Offset = offsetof(IDXGISwapChainVtbl, Present);
}
