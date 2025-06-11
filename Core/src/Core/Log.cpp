#include <Planetoid/Core/Log.h>

#include <stdio.h>
#include <time.h>

namespace PlanetoidEngine
{
    OnLogDelegate Log::OnLog;

    void Log::Info(const std::string& message)
    {
        time_t rawtime;
        struct tm* timeinfo;
        char buffer[80];

        time(&rawtime);
        timeinfo = localtime(&rawtime);

        strftime(buffer, sizeof(buffer), "%H:%M:%S", timeinfo);
        std::string str(buffer);

        std::string msg = "[" + str + "] [INFO] " + message;

        printf("\033[0m");
        printf("%s\n", msg.c_str());
        OnLog.Broadcast(LogLevel::Info, msg);
    }

    void Log::Success(const std::string& message)
    {
        time_t rawtime;
        struct tm* timeinfo;
        char buffer[80];

        time(&rawtime);
        timeinfo = localtime(&rawtime);

        strftime(buffer, sizeof(buffer), "%H:%M:%S", timeinfo);
        std::string str(buffer);

        std::string msg = "[" + str + "] [SUCCESS] " + message;

        printf("\033[0;32m");
        printf("%s\n", msg.c_str());
        printf("\033[0m");
        OnLog.Broadcast(LogLevel::Success, msg);
    }

    void Log::Warn(const std::string& message)
    {
        time_t rawtime;
        struct tm* timeinfo;
        char buffer[80];

        time(&rawtime);
        timeinfo = localtime(&rawtime);

        strftime(buffer, sizeof(buffer), "%H:%M:%S", timeinfo);
        std::string str(buffer);

        // Add the time and type to the message
        std::string msg = "[" + str + "] [WARN] " + message;

        printf("\033[0;33m");
        printf("%s\n", msg.c_str());
        printf("\033[0m");

        OnLog.Broadcast(LogLevel::Warn, msg);
    }

    void Log::Error(const std::string& message)
    {
        time_t rawtime;
        struct tm* timeinfo;
        char buffer[80];

        time(&rawtime);
        timeinfo = localtime(&rawtime);

        strftime(buffer, sizeof(buffer), "%H:%M:%S", timeinfo);
        std::string str(buffer);

        std::string msg = "[" + str + "] [ERROR] " + message;

        printf("\033[0;31m");
        printf("%s\n", msg.c_str());
        printf("\033[0m");
        OnLog.Broadcast(LogLevel::Error, msg);
    }
}