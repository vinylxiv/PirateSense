// PirateSense.

#include "Utility/Logging.h"

#include <filesystem>
#include <fstream>
#include <iostream>

void Logging::Setup()
{
	if (bInitialised)
	{
		return;
	}
	
	if (!AllocConsole())
	{
		return;
	}

	FILE* DummyFile;
	freopen_s(&DummyFile, xorstr_("CONIN$"), xorstr_("r"), stdin);
	freopen_s(&DummyFile, xorstr_("CONOUT$"), xorstr_("w"), stderr);
	freopen_s(&DummyFile, xorstr_("CONOUT$"), xorstr_("w"), stdout);
	std::cout.clear();
	std::clog.clear();
	std::cerr.clear();
	std::cin.clear();

	const HANDLE hConOut = CreateFile(xorstr_("CONOUT$"), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
	const HANDLE hConIn = CreateFile(xorstr_("CONIN$"), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
	SetStdHandle(STD_OUTPUT_HANDLE, hConOut);
	SetStdHandle(STD_ERROR_HANDLE, hConOut);
	SetStdHandle(STD_INPUT_HANDLE, hConIn);
	std::wcout.clear();
	std::wclog.clear();
	std::wcerr.clear();
	std::wcin.clear();

	const HANDLE ConsoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	DWORD ConsoleMode = 0;
	GetConsoleMode(ConsoleHandle, &ConsoleMode);
	SetConsoleMode(ConsoleHandle, ConsoleMode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);

	SetConsoleTitle(xorstr_("PirateSense"));

	// Clear log file.
	{
		const time_t TimeNow = time(nullptr);
		tm TimeStruct;
		localtime_s(&TimeStruct, &TimeNow);

		char DayStringBuffer[256];
		strftime(DayStringBuffer, sizeof(DayStringBuffer), xorstr_("%Y-%m-%d"), &TimeStruct); // @Note: VS thinks this is a buffer overrun. It is not.

		const std::filesystem::path& DirectoryToWriteTo = std::filesystem::current_path();
		std::wofstream FileStream(DirectoryToWriteTo.string() + xorstr_("/PirateSense-") + DayStringBuffer + xorstr_(".log"));
		FileStream.close();
	}

	bInitialised = true;
}

void Logging::Clear()
{
	std::system(xorstr_("CLS"));
}

std::wstring Logging::PrintLogType(const LogType LogType, const bool bClean)
{
	std::wstring Message;
	
	// Output message type.
	switch (LogType)
	{
	case LogType::Warning:
		
		Message += !bClean ? xorstr_(L"\x1B[38;2;255;255;0m") : xorstr_(L"");
		Message += xorstr_(L"[WARNING]");
		Message += !bClean ? xorstr_(L"\033[0m") : xorstr_(L"");

		break;
	case LogType::Error:
		
		Message += !bClean ? xorstr_(L"\x1B[38;2;255;0;0m") : xorstr_(L"");
		Message += xorstr_(L"[ERROR]");
		Message += !bClean ? xorstr_(L"\033[0m") : xorstr_(L"");

		break;
	case LogType::Success:
		
		Message += !bClean ? xorstr_(L"\x1B[38;2;50;205;50m") : xorstr_(L"");
		Message += xorstr_(L"[SUCCESS]");
		Message += !bClean ? xorstr_(L"\033[0m") : xorstr_(L"");

		break;
	case LogType::Failure:
		
		Message += !bClean ? xorstr_(L"\x1B[38;2;255;0;0m") : xorstr_(L"");
		Message += xorstr_(L"[FAILURE]");
		Message += !bClean ? xorstr_(L"\033[0m") : xorstr_(L"");

		break;
	case LogType::Info:
	case LogType::None:
		
		Message += !bClean ? xorstr_(L"\x1B[38;2;0;191;255m") : xorstr_(L"");
		Message += xorstr_(L"[INFO]");
		Message += !bClean ? xorstr_(L"\033[0m") : xorstr_(L"");

		break;
	}

	return Message;
}

void Logging::Log(const LogType LogType, const wchar_t* Message, const wchar_t* LogNamespace, const char* Function, int32_t Line)
{
	if (!bInitialised)
	{
		Setup();
	}

	const time_t TimeNow = time(nullptr);
	tm TimeStruct;
	localtime_s(&TimeStruct, &TimeNow);

	wchar_t TimeStringBuffer[256];
	wcsftime(TimeStringBuffer, sizeof(TimeStringBuffer), xorstr_(L"%Y-%m-%d.%X"), &TimeStruct); // @Note: VS thinks this is a buffer overrun. It is not.
	char DayStringBuffer[256];
	strftime(DayStringBuffer, sizeof(DayStringBuffer), xorstr_("%Y-%m-%d"), &TimeStruct); // @Note: VS thinks this is a buffer overrun. It is not.

	int32_t Length = MultiByteToWideChar(CP_ACP, 0, Function, -1, nullptr, 0);
	wchar_t* FunctionWStr = new wchar_t[Length];
	MultiByteToWideChar(CP_ACP, 0, Function, -1, FunctionWStr, Length);
	
	std::wstring DecoratedMessage;
	DecoratedMessage += xorstr_(L"[");
	DecoratedMessage += xorstr_(L"\x1B[38;2;150;150;150m");
	DecoratedMessage += TimeStringBuffer;
	DecoratedMessage += xorstr_(L"\033[0m");
	DecoratedMessage += xorstr_(L"]");
	DecoratedMessage += xorstr_(L" ");
	DecoratedMessage += xorstr_(L"[");
	DecoratedMessage += xorstr_(L"\x1B[38;2;150;150;150m");
	DecoratedMessage += LogNamespace;
	DecoratedMessage += xorstr_(L"\033[0m");
	DecoratedMessage += xorstr_(L"]");
	DecoratedMessage += xorstr_(L" ");
	DecoratedMessage += PrintLogType(LogType, false);
	DecoratedMessage += xorstr_(L" ");
	DecoratedMessage += xorstr_(L"[");
	DecoratedMessage += xorstr_(L"\x1B[38;2;150;150;150m");
	DecoratedMessage += FunctionWStr;
	DecoratedMessage += xorstr_(L":");
	DecoratedMessage += std::to_wstring(Line);
	DecoratedMessage += xorstr_(L"\033[0m");
	DecoratedMessage += xorstr_(L"]");
	DecoratedMessage += xorstr_(L" : ");
	DecoratedMessage += Message;
	DecoratedMessage += xorstr_(L"\n");

	std::wstring CleanMessage;
	CleanMessage += xorstr_(L"[");
	CleanMessage += TimeStringBuffer;
	CleanMessage += xorstr_(L"]");
	CleanMessage += xorstr_(L" ");
	CleanMessage += xorstr_(L"[");
	CleanMessage += LogNamespace;
	CleanMessage += xorstr_(L"]");
	CleanMessage += xorstr_(L" ");
	CleanMessage += PrintLogType(LogType, true);
	CleanMessage += xorstr_(L" ");
	CleanMessage += xorstr_(L"[");
	CleanMessage += FunctionWStr;
	CleanMessage += xorstr_(L":");
	CleanMessage += std::to_wstring(Line);
	CleanMessage += xorstr_(L"]");
	CleanMessage += xorstr_(L" : ");
	CleanMessage += Message;
	CleanMessage += xorstr_(L"\n");
	
	std::wcout << DecoratedMessage;

	const std::filesystem::path& DirectoryToWriteTo = std::filesystem::current_path();
	std::wofstream FileStream(DirectoryToWriteTo.string() + xorstr_("/PirateSense-") + DayStringBuffer + xorstr_(".log"), std::ios::app);
	if (FileStream.is_open())
	{
		FileStream << CleanMessage;
	}
	FileStream.close();

	// Also logs the message to a debugger if attached.
	wchar_t DebugMessage[4096] = { 0 };
	wcscat_s(DebugMessage, 4096, CleanMessage.c_str());
	wcscat_s(DebugMessage, 4096, xorstr_(L"\n"));
	OutputDebugStringW(DebugMessage);

	delete FunctionWStr;
}
