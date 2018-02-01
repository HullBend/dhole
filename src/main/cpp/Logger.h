/*
 * Copyright (c) 2011           SPZ.
 *
 * http://www.opensource.org/licenses/mit-license.php
 */

/**
 * Refer to http://drdobbs.com/cpp/201804215 for the general idea.
 */
#ifndef LOGGER_INCLUDED_
#define LOGGER_INCLUDED_

#ifndef STDAFX_INCLUDED_
#include "stdafx.h"
#endif /* STDAFX_INCLUDED_ */

#ifndef SLIMSTRING_INCLUDED_
#include "SlimString.h"
#endif /* SLIMSTRING_INCLUDED_ */

#include <stdio.h>  // for FILE*



enum LogLevel {
    LOG_FATAL,
    LOG_ERROR,
    LOG_WARNING,
    LOG_INFO,
    LOG_DEBUG,
    LOG_FINE,
    LOG_FINER
};



#define __LOG_FATAL     Logger().get(LOG_FATAL)
#define __LOG_ERROR     Logger().get(LOG_ERROR)
#define __LOG_WARN      Logger().get(LOG_WARNING)
#define __LOG_INFO      Logger().get(LOG_INFO)
#define __LOG_DEBUG     Logger().get(LOG_DEBUG)
#define __LOG_FINE      Logger().get(LOG_FINE)
#define __LOG_FINER     Logger().get(LOG_FINER)

#define __LARG(arg)     .append(arg)
#define __LARGSZ(arg)   .append(reinterpret_cast<const char*>(arg)) // Beware! Only to be used with caution and taste



class __GCC_DONT_EXPORT Logger {
public: // instance methods, constructor & destructor
    Logger();
    ~Logger(); /* not virtual for performance */

    Logger& get(LogLevel level = LOG_INFO);
    Logger& append(const char*);
    Logger& append(char*);
    Logger& append(const SlimString&);
    Logger& append(SlimString&);

public: // static methods
    static void setStream(FILE*);
    static void setMaxLogLevel(LogLevel level);
    static bool isInitialized();

    inline static bool isLogLevelEnabled(LogLevel level) {
        return level <= Logger::maxLogLevel;
    }


private:
    Logger(const Logger&); // copy constructor
    Logger& operator= (const Logger&); // assignment operator

private: // static methods
    static FILE*& stream();

private: // data
    SlimString str;
    LogLevel loggerLogLevel;

    static LogLevel maxLogLevel;
};



inline
Logger::Logger()
 : loggerLogLevel(LOG_INFO)
{
}

inline
Logger& Logger::append(const char* msg) {
    return append(const_cast<char*>(msg));
}

inline
Logger& Logger::append(char* msg) {
    if (loggerLogLevel <= Logger::maxLogLevel) {
        str.append(msg);
    }
    return *this;
}

inline
Logger& Logger::append(const SlimString& msg) {
    return append(const_cast<SlimString&>(msg));
}

inline
Logger& Logger::append(SlimString& msg) {
    return append(msg.c_str());
}



#endif /* LOGGER_INCLUDED_ */
