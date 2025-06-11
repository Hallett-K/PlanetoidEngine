#pragma once

#include <Planetoid/PlanetoidCore.h>

#include <Planetoid/Core/Delegate.h>

#include <string>

namespace PlanetoidEngine
{
    enum class LogLevel
    {
        Info = 0,
        Success,
        Warn,
        Error
    };

    DECLARE_DELEGATE(OnLogDelegate, LogLevel, std::string);

    class PE_API Log
    {
    public:
        static void Info(const std::string& message);
        static void Success(const std::string& message);
        static void Warn(const std::string& message);
        static void Error(const std::string& message);

        static OnLogDelegate OnLog;
    };
}