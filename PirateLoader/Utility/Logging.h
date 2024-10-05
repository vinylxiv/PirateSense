// PirateSense.

#pragma once

#include <format>
#include <iostream>
#include <typeinfo>
#include <variant>
#include <vector>
#include <ThirdParty/XORSTR.h>
#include <Windows.h>

// Log namespaces. Helpful for grouping modules. Like, Core, Exploits, Memes, etc.
#define LOG_Core xorstr_(L"Core")

namespace std
{
	_NODISCARD inline wstring to_wstring(wstring _Val)
	{
		return _Val;
	}
}

enum class ELogType : uint8_t
{
	None = 0,

	Info = 1,
	Warning = 2,
	Error = 3,

	Success = 4,
	Failure = 5
};

class CLogging
{
public:
	inline static bool bInitialised = false;

public:
	// Attaches to current console window and sets up some custom flags (for colouring)
	static void Setup()
	{
		if (bInitialised)
		{
			return;
		}
		
		const HANDLE ConsoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);

		DWORD ConsoleMode = 0;
		GetConsoleMode(ConsoleHandle, &ConsoleMode);
		SetConsoleMode(ConsoleHandle, ConsoleMode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);

		bInitialised = true;
	}

	static void Clear()
	{
		std::system(xorstr_("CLS"));
	}

	static void Log(const ELogType LogType, const wchar_t* Message, const wchar_t* LogNamespace, const char* Function, int32_t Line)
	{
		if (!bInitialised)
		{
			Setup();
		}

		std::wcout << xorstr_(L"[") << xorstr_(L"\x1B[38;2;150;150;150m") << CLogging::GetTimeNow() << xorstr_(L"\033[0m") << xorstr_(L"]");
		std::wcout << xorstr_(L" ");
		std::wcout << xorstr_(L"[") << xorstr_(L"\x1B[38;2;150;150;150m") << LogNamespace << xorstr_(L"\033[0m") << xorstr_(L"]");
		std::wcout << xorstr_(L" ");
		PrintLogType(LogType);
		std::wcout << xorstr_(L" ");
		std::wcout << xorstr_(L"[") << xorstr_(L"\x1B[38;2;150;150;150m") << Function << L":" << Line << xorstr_(L"\033[0m") << xorstr_(L"]");
		std::wcout << xorstr_(L" : ");
		std::wcout << Message << std::endl;

		// Also logs the message to a debugger if attached.
		wchar_t DebugMessage[4096] = { 0 };
		wcscat_s(DebugMessage, 4096, Message);
		wcscat_s(DebugMessage, 4096, L"\n");
		OutputDebugStringW(DebugMessage);
	}

	// Fancy template version of Log that takes in an array of arguments and can format them.
	template<typename... ArgumentTypes>
	static void Log(const ELogType LogType, const wchar_t* Message, const wchar_t* LogNamespace, const char* Function, int32_t Line, const std::vector<std::variant<ArgumentTypes...>>& Arguments)
	{
		std::wstring FormattedMessage = Message;

		if (Arguments.size() > 0)
		{
			int32_t Index = 0;

			for (const auto& Argument : Arguments)
			{
				wchar_t Buffer[8];
				swprintf(Buffer, sizeof(Buffer), L"{%i}", Index); // @Note: VS thinks this is a buffer overrun. It is not.

				size_t Location = FormattedMessage.find(Buffer);

				std::visit([&FormattedMessage, Location](const auto& Value)
					{
						if (Location != std::wstring::npos)
						{
							FormattedMessage = FormattedMessage.replace(Location, 3, std::to_wstring(Value));
						}
					}
				, Argument);

				Index++;
			}
		}

		Log(LogType, FormattedMessage.c_str(), LogNamespace, Function, Line);
	}

	static void PrintLogType(const ELogType LogType)
	{
		// Output message type.
		switch (LogType)
		{
		case ELogType::Warning:
			std::wcout << xorstr_(L"\x1B[38;2;255;255;0m") << xorstr_(L"[WARNING]") << xorstr_(L"\033[0m");

			break;
		case ELogType::Error:
			std::wcout << xorstr_(L"\x1B[38;2;255;0;0m") << xorstr_(L"[ERROR]") << xorstr_(L"\033[0m");

			break;
		case ELogType::Success:
			std::wcout << xorstr_(L"\x1B[38;2;50;205;50m") << xorstr_(L"[SUCCESS]") << xorstr_(L"\033[0m");

			break;
		case ELogType::Failure:
			std::wcout << xorstr_(L"\x1B[38;2;255;0;0m") << xorstr_(L"[FAILURE]") << xorstr_(L"\033[0m");

			break;
		default:
			std::wcout << xorstr_(L"\x1B[38;2;0;191;255m") << xorstr_(L"[INFO]") << xorstr_(L"\033[0m");

			break;
		}
	}

	static std::wstring GetTimeNow()
	{
		const time_t TimeNow = time(nullptr);
		tm TimeStruct;
		wchar_t TimeStringBuffer[256];
		localtime_s(&TimeStruct, &TimeNow);

		wcsftime(TimeStringBuffer, sizeof(TimeStringBuffer), L"%Y-%m-%d.%X", &TimeStruct); // @Note: VS thinks this is a buffer overrun. It is not.

		return TimeStringBuffer;
	}
};

// Log macros. Use these instead of the raw functions so that the function name and line is logged.
#define PIRATESENSE_LOG(Namespace, LogType, Message) CLogging::Log(LogType, Message, Namespace, __FUNCTION__, __LINE__);
#define PIRATESENSE_LOGARGS(Namespace, LogType, Message, ...) CLogging::Log(LogType, Message, Namespace, __FUNCTION__, __LINE__, __VA_ARGS__);
