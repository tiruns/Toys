#include <Windows.h>
#include <stacktrace>
#include <format>

void AbortWithMessage(const char* message)
{
    const size_t kStacktraceMaxDepth = 9;

    auto stack = std::stacktrace::current(1, kStacktraceMaxDepth);
    const auto stackDesc = std::to_string(stack);

    const auto msgBoxText = std::format("Fatal error: {}\nStacktrace:\n{}", message, stackDesc);
    MessageBoxA(NULL, msgBoxText.c_str(), "Run in Borderless Window", MB_OK | MB_ICONERROR);

    abort();
}

#define Ensure(condition) \
    if (!(condition)) \
        AbortWithMessage(#condition)

void* GetDinputFunction(const char* name)
{
    static auto lib = LoadLibrary(LR"(C:\Windows\System32\dinput8.dll)");
    Ensure(lib != INVALID_HANDLE_VALUE);
    auto fn = GetProcAddress(lib, name);
    Ensure(fn != nullptr);
    return fn;
}

void FixA17Colors()
{
    uint32_t* baseAddresss = reinterpret_cast<uint32_t*>(0x140D79000ull + 0x2C340ull);
    const int numColors = 5;

    const uint32_t originalColors[] = {
        0xFFC0C0C0,
        0xFF3F3F96,
        0xFF2A87B8,
        0xFF389C69,
        0xFFA58840
    };

    const uint32_t newColors[] = {
        0xFFDCDCDC,
        0xFF2020B0,
        0xFF10DEDE,
        0xFF80C439,
        0xFFE69C5E
    };

    for (int i = 0; i < numColors; ++i)
    {
        uint32_t* colorAddress = baseAddresss + (i * 6); // 24 bytes per element
        uint32_t& color = *colorAddress;
        Ensure(color == originalColors[i]);
        color = newColors[i];
    }
}

bool IsA17Process()
{
    char buf[1024]{};
    Ensure(GetModuleFileNameA(NULL, buf, 1024) != 0);
    std::string s = buf;
    return s.ends_with("Atelier_Sophie_DX.exe");
}

extern "C" __declspec(dllexport)
HRESULT WINAPI DirectInput8Create(HINSTANCE hinst, DWORD dwVersion, REFIID riidltf, LPVOID* ppvOut, LPUNKNOWN punkOuter)
{
    using FnType = decltype(DirectInput8Create);
    auto fn = (FnType*)GetDinputFunction("DirectInput8Create");
    HRESULT hr = fn(hinst, dwVersion, riidltf, ppvOut, punkOuter);

    static bool isA17ColorFixed = false;
    if (!isA17ColorFixed)
    {
        isA17ColorFixed = true;
        if (IsA17Process())
            FixA17Colors();
    }

    return hr;
}
