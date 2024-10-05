// PirateSense.

#include "CrashHandler.h"

#include <csignal>

#include "Logging.h"
#include "ThirdParty/StackWalker.h"

class StackWalkerToConsole : public StackWalker
{
protected:
    virtual void OnOutput(LPCSTR Text) override
    {
        PIRATESENSE_LOG(LOG_Core, LogType::Info, Text);
    }
};

void CrashHandler::Init()
{
    PIRATESENSE_DEVLOG(LOG_Core, LogType::Info, xorstr_(L"Init crash handler."));

    CrashHandler::SetProcessExceptionHandlers();
    CrashHandler::SetThreadExceptionHandlers();
}

void CrashHandler::SetProcessExceptionHandlers()
{
    SetUnhandledExceptionFilter(SEHHandler);    

    _set_purecall_handler(PureCallHandler);

    // @todo
    //std::_set_new_handler(NewHandler);

    _set_invalid_parameter_handler(InvalidParameterHandler); 

    _set_abort_behavior(_CALL_REPORTFAULT, _CALL_REPORTFAULT);

    signal(SIGABRT, Handler_SIGABRT);  

    signal(SIGINT, Handler_SIGINT);     

    signal(SIGTERM, Handler_SIGTERM);  
}

void CrashHandler::SetThreadExceptionHandlers()
{
    set_terminate(TerminateHandler);       

    set_unexpected(UnexpectedHandler);    

    typedef void (*sigh)(int);
    signal(SIGFPE, (sigh)Handler_SIGFPE);     

    signal(SIGILL, Handler_SIGILL);     

    signal(SIGSEGV, Handler_SIGSEGV);  
}

void CrashHandler::TryLogStackTrace(PEXCEPTION_POINTERS ExceptionPtrs)
{
    StackWalker SW;
    SW.ShowCallstack(GetCurrentThread(), ExceptionPtrs->ContextRecord);
}

void CrashHandler::TryLogStackTrace()
{
    StackWalker SW;
    SW.ShowCallstack();
}
