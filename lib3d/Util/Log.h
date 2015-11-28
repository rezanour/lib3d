#pragma once

namespace Lib3D
{
    enum class LogLevel
    {
        Unspecified = 0,
        Error,
        Warning,
        Info        
    };

    void __cdecl Log(LogLevel level, const wchar_t* format, ...);
    void __cdecl LogError(const wchar_t* format, ...);
    void __cdecl LogWarning(const wchar_t* format, ...);
    void __cdecl LogInfo(const wchar_t* format, ...);
}
