// PirateSense.

#pragma once

#include <format>
#include <string>
#include <typeinfo>
#include <variant>
#include <vector>
#include <ThirdParty/XORSTR.h>
#include <Windows.h>

namespace std
{
	// ReSharper disable once CppInconsistentNaming
	_NODISCARD inline wstring to_wstring(wstring Val)
	{
		return Val;
	}
}

// Log namespaces. Helpful for grouping modules. Like, Core, Exploits, Memes, etc.
#define LOG_Core xorstr_(L"Core")

enum class LogType : uint8_t
{
	None = 0,

	Info = 1,
	Warning = 2,
	Error = 3,

	Success = 4,
	Failure = 5
};

class Logging
{
public:
	static inline bool bInitialised = false;

	// Attaches to current console window and sets up some custom flags (for colouring
	// This version also sets up a new console for us to use.
	static void Setup();
	static void Clear();

	static std::wstring PrintLogType(const LogType LogType, const bool bClean);

	static void Log(const LogType LogType, const wchar_t* Message, const wchar_t* LogNamespace, const char* Function, int32_t Line);

	// Fancy template version of Log that takes in an array of arguments and can format them.
	template<typename... ArgumentTypes>
	static void Log(const LogType LogType, const wchar_t* Message, const wchar_t* LogNamespace, const char* Function, int32_t Line, const std::vector<std::variant<ArgumentTypes...>>& Arguments)
	{
		std::wstring FormattedMessage = Message;

		if (Arguments.size() > 0)
		{
			int32_t Index = 0;

			for (const auto& Argument : Arguments)
			{
				wchar_t Buffer[8];
				swprintf(Buffer, sizeof(Buffer), xorstr_(L"{%i}"), Index); // @Note: VS thinks this is a buffer overrun. It is not.

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

	static void Log(const LogType LogType, const char* Message, const wchar_t* LogNamespace, const char* Function, int32_t Line)
	{
		const int32_t Length = MultiByteToWideChar(CP_ACP, 0, Message, -1, nullptr, 0);
		wchar_t* TextWStr = new wchar_t[Length];
		MultiByteToWideChar(CP_ACP, 0, Message, -1, TextWStr, Length);

		Log(LogType, TextWStr, LogNamespace, Function, Line);
	}
	template<typename... ArgumentTypes>
	static void Log(const LogType LogType, const char* Message, const wchar_t* LogNamespace, const char* Function, int32_t Line, const std::vector<std::variant<ArgumentTypes...>>& Arguments)
	{
		const int32_t Length = MultiByteToWideChar(CP_ACP, 0, Message, -1, nullptr, 0);
		wchar_t* TextWStr = new wchar_t[Length];
		MultiByteToWideChar(CP_ACP, 0, Message, -1, TextWStr, Length);

		Log(LogType, TextWStr, LogNamespace, Function, Line, Arguments);
	}
};

// Log macros. Use these instead of the raw functions so that the function name and line is logged.
#define PIRATESENSE_LOG(Namespace, LogType, Message) Logging::Log(LogType, Message, Namespace, __FUNCTION__, __LINE__);
#define PIRATESENSE_LOGARGS(Namespace, LogType, Message, ...) Logging::Log(LogType, Message, Namespace, __FUNCTION__, __LINE__, __VA_ARGS__);

// Macro that removes dev logging in Release mode.
#ifdef DEVBUILD
#define PIRATESENSE_DEVLOG(Namespace, LogType, Message) Logging::Log(LogType, Message, Namespace, __FUNCTION__, __LINE__);
#define PIRATESENSE_DEVLOGARGS(Namespace, LogType, Message, ...) Logging::Log(LogType, Message, Namespace, __FUNCTION__, __LINE__, __VA_ARGS__);
#else
#define PIRATESENSE_DEVLOG(Namespace, LogType, Message) ;
#define PIRATESENSE_DEVLOGARGS(Namespace, LogType, Message, ...) ;
#endif // DEVBUILD
