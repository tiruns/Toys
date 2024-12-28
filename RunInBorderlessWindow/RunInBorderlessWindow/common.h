#pragma once

#include <cstdint>

namespace Constants
{
    extern const ptrdiff_t IDXGIFactory1_CreateSwapChain_Offset;
    extern const ptrdiff_t IDXGISwapChain_SetFullscreenState_Offset;
    extern const ptrdiff_t IDXGISwapChain_Present_Offset;
}

void AbortWithMessage(const char* message);

#define Ensure(condition) \
    if (!(condition)) \
        AbortWithMessage(#condition)

void TryHookDXGIFactory(void* d3dDevice);
