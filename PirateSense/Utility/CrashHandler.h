// PirateSense.

#pragma once

#include <Windows.h>

class CrashHandler
{
public:
    static void Init();
    static void SetProcessExceptionHandlers();
    static void SetThreadExceptionHandlers();

    static LONG WINAPI SEHHandler(PEXCEPTION_POINTERS ExceptionPtrs) { TryLogStackTrace(ExceptionPtrs); return EXCEPTION_EXECUTE_HANDLER; }
    static void __cdecl TerminateHandler() { TryLogStackTrace(); }
    static void __cdecl UnexpectedHandler() { TryLogStackTrace(); }

    static void __cdecl PureCallHandler() { TryLogStackTrace(); }

    static void __cdecl InvalidParameterHandler(const wchar_t* Expression, const wchar_t* Function, const wchar_t* File, unsigned int Line, uintptr_t Reserved) { TryLogStackTrace(); }

    //static int __cdecl NewHandler(size_t) { TryLogStackTrace(); }

    static void Handler_SIGABRT(int Code) { TryLogStackTrace(); }
    static void Handler_SIGFPE(int Code, int SubCode) { TryLogStackTrace(); }
    static void Handler_SIGINT(int Code) { TryLogStackTrace(); }
    static void Handler_SIGILL(int Code) { TryLogStackTrace(); }
    static void Handler_SIGSEGV(int Code) { TryLogStackTrace(); }
    static void Handler_SIGTERM(int Code) { TryLogStackTrace(); }

    static void TryLogStackTrace(PEXCEPTION_POINTERS ExceptionPtrs);
    static void TryLogStackTrace();
};
