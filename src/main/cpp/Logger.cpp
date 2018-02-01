/*
 * Copyright (c) 2011           SPZ.
 *
 * http://www.opensource.org/licenses/mit-license.php
 */

#include "Logger.h"

#include <stdio.h>      // sprintf

#ifndef POTABILITY_INCLUDED_
#include "Portability.h"    // for getCurrentThreadId_portable()
#endif /* POTABILITY_INCLUDED_ */


#if defined (_WIN64) || defined (_WIN32)
#include <windows.h>    // for timeNow() and dateToday() helpers impl
#else
#include <time.h>
#include <sys/time.h>   // gettimeofday
#endif /* (_WIN64) || (_WIN32) */



#if defined (_WIN64) || defined (_WIN32)
// disable "This function may be unsafe" warnings for sprintf()
#pragma warning( disable: 4996 )
#endif /* (_WIN64) || (_WIN32) */



// internal helpers' forward declaration
static const SlimString datetimeNow();
static inline const char* levelToString(LogLevel level);


// static data
LogLevel Logger::maxLogLevel = LOG_INFO;




FILE*& Logger::stream() {
    static FILE* pStream = NULL;
    return pStream;
}

void Logger::setStream(FILE* pFile) {
    if (pFile) {
        stream() = pFile;
    }
}

void Logger::setMaxLogLevel(LogLevel level) {
    Logger::maxLogLevel = level;
}

bool Logger::isInitialized() {
    return (stream() != NULL);
}

Logger& Logger::get(LogLevel level) {
    if (level <= Logger::maxLogLevel) {
        loggerLogLevel = level;

        const SlimString& datetime = datetimeNow();
        const SlimString& threadId = getCurrentThreadId_portable();
        str.append(datetime.c_str()).append(" ");
        str.append(levelToString(level));
        str.append(" [").append(threadId.c_str());
        str.append("]\t- ");
    } else {
        loggerLogLevel = static_cast<LogLevel>(Logger::maxLogLevel + 1);
    }
    return *this;
}

Logger::~Logger() {
    if (loggerLogLevel <= Logger::maxLogLevel) {
        FILE* pStream = stream();
        if (pStream) {
            str.append("\n");
            if (fprintf(pStream, "%s", str.c_str()) >= 0) {
                fflush(pStream);
            }
        }
    }
}




inline const char* levelToString(LogLevel level) {
    static const char* const buffer[] = {"FATAL", "ERROR", "WARN ", "INFO ", "DEBUG", "FINE ", "FINER"};
    return buffer[level];
}



// The following two helper functions are only declared for Windows
#if defined (_WIN64) || defined (_WIN32)

// This function is only used on Windows
static const SlimString dateToday()
{
    const int MAX_LEN = 64;
    char buffer[MAX_LEN];
    if (GetDateFormatA(LOCALE_USER_DEFAULT, 0, 0, "yyyy'-'MM'-'dd", buffer, MAX_LEN) == 0) {
        return SlimString("Error in dateToday()");
    }
    return SlimString(buffer);
}

// This function is only used on Windows
static const SlimString timeNow()
{
    const char* errTxt = "Error in timeNow()";
    // Number of milliseconds since the system was started
    static DWORD firstTick = GetTickCount();

    const int MAX_LEN = 64;
    char buffer[MAX_LEN];
    if (GetTimeFormatA(LOCALE_USER_DEFAULT, 0, 0, "HH':'mm':'ss", buffer, MAX_LEN) == 0) {
        return SlimString(errTxt);
    }

    DWORD currTick = GetTickCount();
    if (currTick < firstTick) {
        // Repair the GetTickCount() overflow after 49.7 days
        firstTick = 0L;
    }

    char result[MAX_LEN] = {0};
    if (sprintf(result, "%s.%03ld", buffer, (long) (currTick - firstTick) % 1000) <= 0) {
        return SlimString(errTxt);
    }
    return SlimString(result);
}
#endif /* (_WIN64) || defined (_WIN32) */



const SlimString datetimeNow()
{
#if defined (_WIN64) || defined (_WIN32)

    const SlimString& today = dateToday();
    const SlimString& now = timeNow();

    SlimString datetime(today);
    datetime.append(" ").append(now.c_str());

    return datetime;

#else /* Linux or Unix */

    const int MAX_LEN = 64;
    const char* errTxt = "Error in datetimeNow()";
    tm r = {0};
    time_t t;
    time(&t);

    struct tm* pLocal = localtime_r(&t, &r);
    if (!pLocal) {
        return SlimString(errTxt);
    }

    char buffer[MAX_LEN] = {0};
    if (strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", pLocal) <= 0) {
        return SlimString(errTxt);
    }
    struct timeval tv;
    if (gettimeofday(&tv, 0) != 0) {
        return SlimString(errTxt);
    }
    char result[MAX_LEN] = {0};
    if (sprintf(result, "%s.%03ld", buffer, (long) tv.tv_usec / 1000) <= 0) {
        return SlimString(errTxt);
    }
    return SlimString(result);

#endif /* (_WIN64) || (_WIN32) */
}
