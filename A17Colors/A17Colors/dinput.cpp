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

void FixA19Colors()
{
    std::byte* baseAddresss = reinterpret_cast<std::byte*>(0x140FBE000ull);
    const int numColors = 5;

    // ingredient colors
    {
        uint32_t* targetAddress = reinterpret_cast<uint32_t*>(baseAddresss + 0x4068ull);

        const uint32_t originalColors[] = {
            0xFF5656C8, // R
            0xFF915014, // B
            0xFF15853F, // G
            0xFF0FA9D2, // Y
            0xFFD94B87  // P
        };

        const uint32_t newColors[] = {
            0xFF0000A0, // R
            0xFFC00000, // B
            0xFF00C000, // G
            0xFF00F0F0, // Y
            0xFFC000C0  // P
        };

        for (int i = 0; i < numColors; ++i)
        {
            uint32_t* colorAddress = targetAddress + i;
            uint32_t& color = *colorAddress;
            Ensure(color == originalColors[i]);
            color = newColors[i];
        }
    }

    // indicator/border colors
    {
        uint32_t* targetAddress1 = reinterpret_cast<uint32_t*>(baseAddresss + 0x4080ull);
        uint32_t* targetAddress2 = reinterpret_cast<uint32_t*>(baseAddresss + 0x4098ull);

        const uint32_t originalColors[] = {
            0xFF0032C8, // R
            0xFF6E5515, // B
            0xFF007D3B, // G
            0xFF007FBC, // Y
            0xFFC80064  // P
        };

        const uint32_t newColors[] = {
            0xFF0000A0, // R
            0xFFC00000, // B
            0xFF00C000, // G
            0xFF00F0F0, // Y
            0xFFC000C0  // P
        };

        for (auto targetAddress : {targetAddress1, targetAddress2})
        {
            for (int i = 0; i < numColors; ++i)
            {
                uint32_t* colorAddress = targetAddress + i;
                uint32_t& color = *colorAddress;
                Ensure(color == originalColors[i]);
                color = newColors[i];
            }
        }
    }

    // background colors
    {
        uint32_t* targetAddress = reinterpret_cast<uint32_t*>(baseAddresss + 0x40E0ull);

        const uint32_t originalColors[] = {
            0xC03723AF, // R
            0xC08C4B14, // B
            0xC0237300, // G
            0xC03CAAB4, // Y
            0xC0A50A6E  // P
        };

        const uint32_t newColors[] = {
            0xC00000A0, // R
            0xC0C00000, // B
            0xC000C000, // G
            0xC000F0F0, // Y
            0xC0C000C0  // P
        };

        for (int i = 0; i < numColors; ++i)
        {
            uint32_t* colorAddress = targetAddress + i;
            uint32_t& color = *colorAddress;
            Ensure(color == originalColors[i]);
            color = newColors[i];
        }
    }
}

bool IsA19Process()
{
    char buf[1024]{};
    Ensure(GetModuleFileNameA(NULL, buf, 1024) != 0);
    std::string s = buf;
    return s.ends_with("Atelier_Lydie_and_Suelle_DX.exe");
}

extern "C" __declspec(dllexport)
HRESULT WINAPI DirectInput8Create(HINSTANCE hinst, DWORD dwVersion, REFIID riidltf, LPVOID* ppvOut, LPUNKNOWN punkOuter)
{
    using FnType = decltype(DirectInput8Create);
    auto fn = (FnType*)GetDinputFunction("DirectInput8Create");
    HRESULT hr = fn(hinst, dwVersion, riidltf, ppvOut, punkOuter);

    static bool isColorFixed = false;
    if (!isColorFixed)
    {
        isColorFixed = true;
        if (IsA17Process())
            FixA17Colors();
        if (IsA19Process())
            FixA19Colors();
    }

    return hr;
}
