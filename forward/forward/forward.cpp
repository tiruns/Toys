#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <iostream>
#include <string>
#include <memory>
#include <atomic>

#ifndef PROGRAM_PATH
#error Program path not defined!
#endif

std::wstring EncapsuleArgument(std::wstring_view argument)
{
    std::wstring encapsuledArg;
    encapsuledArg.reserve(argument.size() + 2);

    if (argument.contains(L" "))
    {
        encapsuledArg.push_back(L'\"');
        for (const auto ch : argument)
            encapsuledArg.push_back(ch);
        encapsuledArg.push_back(L'\"');
    }
    else
    {
        encapsuledArg = argument;
    }

    return encapsuledArg;
}

std::unique_ptr<wchar_t[]> BuildArguments(int argc, wchar_t* argv[])
{
    std::wstring arguments;
    arguments.reserve(1024);
    arguments += EncapsuleArgument(PROGRAM_PATH);

    for (int i = 1; i < argc; ++i)
    {
        arguments += L" ";
        arguments += EncapsuleArgument(argv[i]);
    }

    auto buffer = std::make_unique<wchar_t[]>(arguments.size() + 1);
    buffer[arguments.size()] = L'\0';
    for (int i = 0; i < arguments.size(); ++i)
        buffer[i] = arguments[i];

    return buffer;
}

int Execute(wchar_t* arguments)
{
    STARTUPINFOW startupInfo{};
    PROCESS_INFORMATION processInfo{};
    startupInfo.cb = sizeof(startupInfo);

    if (FALSE == CreateProcessW(PROGRAM_PATH, arguments, NULL,
        NULL, FALSE, NULL, NULL, NULL, &startupInfo, &processInfo))
    {
        std::wcerr << L"forward: invalid program at\n\t" << PROGRAM_PATH;
        return 1;
    }

    // ignore ctrl+c
    SetConsoleCtrlHandler(NULL, TRUE);
    WaitForSingleObject(processInfo.hProcess, INFINITE);

    DWORD exitCode = 0;
    if (GetExitCodeProcess(processInfo.hProcess, &exitCode) == 0)
        exitCode = 1;

    CloseHandle(processInfo.hProcess);
    CloseHandle(processInfo.hThread);

    return exitCode;
}

int wmain(int argc, wchar_t* argv[])
{
    auto arguments = BuildArguments(argc, argv);
    return Execute(arguments.get());
}
