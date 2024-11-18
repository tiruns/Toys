#include "common.h"
#include <cstddef>
#define CINTERFACE
#include <dxgi.h>

namespace Constants
{
    const ptrdiff_t IDXGIFactory1_CreateSwapChain_Offset = offsetof(IDXGIFactory1Vtbl, CreateSwapChain);
    const ptrdiff_t IDXGISwapChain_SetFullscreenState_Offset = offsetof(IDXGISwapChainVtbl, SetFullscreenState);
}


void test(void* p)
{
    IDXGIFactory1 fac1 = *(IDXGIFactory1*)p;
    auto y = *fac1.lpVtbl;
    return;
}

