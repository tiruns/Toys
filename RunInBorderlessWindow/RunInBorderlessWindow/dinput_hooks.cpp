#include "common.h"
#include <winrt/base.h>
#include <dinput.h>
#include <Windows.h>
#include <detours/detours.h>

using FnCreateDevice = long(IDirectInput8W* This, REFGUID rguid, LPDIRECTINPUTDEVICE* lplpDirectInputDevice, LPUNKNOWN pUnkOuter);
using FnSetCooperativeLevel = long(IDirectInputDevice8W* This, HWND hwnd, DWORD dwFlags);

FnCreateDevice* realCreateDevice = nullptr;
FnSetCooperativeLevel* realSetCooperativeLevel = nullptr;

HRESULT FakeSetCooperativeLevel(IDirectInputDevice8W* This, HWND hwnd, DWORD dwFlags)
{
    dwFlags = dwFlags & (~DISCL_NOWINKEY);
    return realSetCooperativeLevel(This, hwnd, dwFlags);
}

void HookDinputDevice(IDirectInputDevice8W* device)
{
    realSetCooperativeLevel = (FnSetCooperativeLevel*)GetMemeberFunctionAddress(
        device, Constants::IDirectInputDevice8_SetCooperativeLevel);

    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());
    DetourAttach((void**)&realSetCooperativeLevel, FakeSetCooperativeLevel);
    Ensure(NO_ERROR == DetourTransactionCommit());
}

void TryHookDinputDevice(IDirectInputDevice8W* device)
{
    static bool isHooked = false;
    if (isHooked)
        return;
    isHooked = true;

    HookDinputDevice(device);
}

HRESULT FakeCreateDevice(IDirectInput8W* This, REFGUID rguid, LPDIRECTINPUTDEVICE* lplpDirectInputDevice, LPUNKNOWN pUnkOuter)
{
    HRESULT hr = realCreateDevice(This, rguid, lplpDirectInputDevice, pUnkOuter);
    if (SUCCEEDED(hr))
        TryHookDinputDevice(reinterpret_cast<IDirectInputDevice8W*>(*lplpDirectInputDevice));
    return hr;
}

void HookDinput(IDirectInput8W* dinput)
{
    Ensure(dinput != nullptr);

    realCreateDevice = (FnCreateDevice*)GetMemeberFunctionAddress(
        dinput, Constants::IDirectInput8_CreateDevice_Offset);

    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());
    DetourAttach((void**)&realCreateDevice, FakeCreateDevice);
    Ensure(NO_ERROR == DetourTransactionCommit());

    return;
}

void TryHookDinput(void* dinput)
{
    static bool isHooked = false;
    if (isHooked)
        return;
    isHooked = true;

    HookDinput(reinterpret_cast<IDirectInput8W*>(dinput));
}
