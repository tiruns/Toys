#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <cstdio>
#include <string>
#include <memory>


#ifndef PROGRAM_PATH
#error Program path not defined!
#endif

#ifdef USE_NAMED_MUTEX
auto kMutexName = L"528c2c15-a3aa-427b-b5ed-e946d7436b3b";

class NamedMutex
{
public:
	HANDLE hMutex = NULL;
public:
	NamedMutex() = default;
	~NamedMutex()
	{
		if (hMutex != NULL)
			ReleaseMutex(hMutex);
	}

	bool TreCreate()
	{
		hMutex = OpenMutexW(MUTEX_ALL_ACCESS, FALSE, kMutexName);
		if (hMutex != NULL)
			return false;
		hMutex = CreateMutex(NULL, FALSE, kMutexName);
		return true;
	}
};
#endif // USE_NAMED_MUTEX

int wmain(int argc, wchar_t* argv[])
{
#ifdef USE_NAMED_MUTEX
	NamedMutex namedMutex;
	if (!namedMutex.TreCreate())
	{
		printf_s("forward: can not create multiple instances simultaneously.");
		return 1;
	}
#endif // USE_NAMED_MUTEX


	STARTUPINFOW si = { 0 };
	PROCESS_INFORMATION pi = { 0 };
	si.cb = sizeof(si);
	
	std::wstring cmdBuilder;
	cmdBuilder += PROGRAM_PATH;
	for (int i = 1; i < argc; ++i)
	{
		cmdBuilder += L" ";
		cmdBuilder += argv[i];
	}

	auto buffer = std::make_unique<wchar_t[]>(cmdBuilder.size() + 1);
	buffer[cmdBuilder.size()] = L'\0';
	for (int i = 0; i < cmdBuilder.size(); ++i)
		buffer[i] = cmdBuilder[i];

	if (FALSE == CreateProcessW(PROGRAM_PATH, buffer.get(), NULL,
		NULL, FALSE, NULL, NULL, NULL, &si, &pi))
	{
		printf_s("forward: invalid program at\n");
		wprintf_s(PROGRAM_PATH);
		return 1;
	}

	WaitForSingleObject(pi.hProcess, INFINITE);

	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);

	return 0;
}