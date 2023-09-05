#include <assert.h>
#include <Windows.h>
#include <detours/detours.h>


using FnDirect3DCreate9 = INT WINAPI(UINT);

extern "C" INT WINAPI Direct3DCreate9(UINT version)
{
    auto lib = LoadLibrary(LR"(C:\Windows\SysWOW64\d3d9.dll)");
    assert(lib != nullptr);
    auto proc = (FnDirect3DCreate9*)GetProcAddress(lib, "Direct3DCreate9");
    assert(proc != nullptr);
    return proc(version);
}

auto RealCreateFontW = CreateFontW;

HFONT WINAPI FakeCreateFontW(_In_ int cHeight, _In_ int cWidth, _In_ int cEscapement, _In_ int cOrientation, _In_ int cWeight, _In_ DWORD bItalic,
    _In_ DWORD bUnderline, _In_ DWORD bStrikeOut, _In_ DWORD iCharSet, _In_ DWORD iOutPrecision, _In_ DWORD iClipPrecision,
    _In_ DWORD iQuality, _In_ DWORD iPitchAndFamily, _In_opt_ LPCWSTR pszFaceName)
{
    auto newFontFaceName = L"SimHei";
    return RealCreateFontW(cHeight, cWidth, cEscapement, cOrientation, cWeight, bItalic, bUnderline, bStrikeOut, iCharSet, iOutPrecision, iClipPrecision, iQuality, iPitchAndFamily, newFontFaceName);
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
    if (DLL_PROCESS_ATTACH == fdwReason)
    {
        // TODO: check return values
        DetourTransactionBegin();
        DetourAttach((void**)&RealCreateFontW, FakeCreateFontW);
        DetourTransactionCommit();
    }

    return TRUE;
}
