#include "common.h"
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
