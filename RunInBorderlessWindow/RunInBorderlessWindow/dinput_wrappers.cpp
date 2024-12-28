#include "common.h"
#include <Windows.h>
// #include <dinput.h>
#include <detours/detours.h>

void* GetDinputFunction(const char* name)
{
    static auto lib = LoadLibrary(LR"(C:\Windows\System32\dinput8.dll)");
    Ensure(lib != INVALID_HANDLE_VALUE);
    auto fn = GetProcAddress(lib, name);
    Ensure(fn != nullptr);
    return fn;
}

extern "C" __declspec(dllexport)
HRESULT WINAPI DirectInput8Create(HINSTANCE hinst, DWORD dwVersion, REFIID riidltf, LPVOID* ppvOut, LPUNKNOWN punkOuter)
{
    using FnType = decltype(DirectInput8Create);
    auto fn = (FnType*)GetDinputFunction("DirectInput8Create");
    HRESULT hr = fn(hinst, dwVersion, riidltf, ppvOut, punkOuter);

    if (SUCCEEDED(hr))
        TryHookDinput(*ppvOut);
    return hr;
}
