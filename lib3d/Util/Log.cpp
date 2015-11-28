#include "Precomp.h"
#include "Log.h"

namespace Lib3D
{
    static const wchar_t* GetLevelString(LogLevel level)
    {
        switch (level)
        {
        case LogLevel::Error:           return L"[ERROR] ";
        case LogLevel::Warning:         return L"[WARNING] ";
        case LogLevel::Info:            return L"[INFO] ";

        default:
        case LogLevel::Unspecified:     return L"";
        }
    }

    static void LogInternal(LogLevel level, const wchar_t* format, const va_list& args)
    {
        wchar_t message[1024]{};
        vswprintf_s(message, format, args);

        // TODO: handle filtering based on level
        OutputDebugString(GetLevelString(level));
        OutputDebugString(message);
    }

    void __cdecl Log(LogLevel level, const wchar_t* format, ...)
    {
        va_list args;
        va_start(args, format);
        LogInternal(level, format, args);
        va_end(args);
    }

    void __cdecl LogError(const wchar_t* format, ...)
    {
        va_list args;
        va_start(args, format);
        LogInternal(LogLevel::Error, format, args);
        va_end(args);
    }

    void __cdecl LogWarning(const wchar_t* format, ...)
    {
        va_list args;
        va_start(args, format);
        LogInternal(LogLevel::Warning, format, args);
        va_end(args);
    }

    void __cdecl LogInfo(const wchar_t* format, ...)
    {
        va_list args;
        va_start(args, format);
        LogInternal(LogLevel::Info, format, args);
        va_end(args);
    }
}
