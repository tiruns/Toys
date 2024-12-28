#pragma once

#include <cstdint>
#include <cstddef>

namespace Constants
{
    extern const ptrdiff_t IDXGIFactory1_CreateSwapChain_Offset;
    extern const ptrdiff_t IDXGIFactory2_CreateSwapChainForHwnd_Offset;
    extern const ptrdiff_t IDXGISwapChain_SetFullscreenState_Offset;
    extern const ptrdiff_t IDXGISwapChain_Present_Offset;
    
    extern const ptrdiff_t IDirectInput8_CreateDevice_Offset;
    extern const ptrdiff_t IDirectInputDevice8_SetCooperativeLevel;
}

void AbortWithMessage(const char* message);

#define Ensure(condition) \
    if (!(condition)) \
        AbortWithMessage(#condition)

void TryHookDXGIFactory(void* d3dDevice);
void TryHookDinput(void* dinput);

inline void* GetMemeberFunctionAddress(void* instance, ptrdiff_t offset)
{
    std::byte* vtable = *reinterpret_cast<std::byte**>(instance);
    size_t* fnPtr = reinterpret_cast<size_t*>(vtable + offset);
    return reinterpret_cast<void*>(*fnPtr);
}
