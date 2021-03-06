/*
 * Copyright (c) 2002           SPZ.
 *
 * http://www.opensource.org/licenses/mit-license.php
 */

//////////////////////////////////////////////////////////////////////
// ThreadMutex.cpp: implementation of the ThreadMutex class.
//
//////////////////////////////////////////////////////////////////////

#include "ThreadMutex.h"

#include <stdio.h> // sprintf

#ifndef JEXCEPTION_INCLUDED_
#include "JException.h"
#endif /* JEXCEPTION_INCLUDED_ */

#ifndef SLIMSTRING_INCLUDED_
#include "SlimString.h"
#endif /* SLIMSTRING_INCLUDED_ */


#if defined (_WIN64) || defined (_WIN32)
// disable "This function may be unsafe" warning for sprintf()
#pragma warning( disable: 4996 )
#endif /* (_WIN64) || (_WIN32) */




ThreadMutex::ThreadMutex() {
#if defined (_WIN64) || defined (_WIN32)

    // Note: The spincount will be ignored on single-processor machines
    if (!InitializeCriticalSectionAndSpinCount(&m_cs, SPINCOUNT)) {
        // this is unexpected
        unsigned long err = GetLastError();
        const int MAX_LEN = 64;
        char code[MAX_LEN] = {0};
        sprintf(code, "%d", err);
        const char* errMsg = "ThreadMutex::ThreadMutex() failed unexpectedly"
            " (low memory situation?) - Error Code: ";
        SlimString msg(errMsg);
        msg.append(code);
        throw JException(msg);
    }

#else /* POSIX threads */

    int rc = pthread_mutex_init(&m_cs, NULL);

    if (rc != 0) {
        const int MAX_LEN = 64;
        char code[MAX_LEN] = {0};
        sprintf(code, "%d", rc);
        const char* errMsg = "ThreadMutex::ThreadMutex() failed unexpectedly"
            " with error code: ";
        SlimString msg(errMsg);
        msg.append(code);
        throw JException(msg);
    }

#endif /* (_WIN64) || (_WIN32) */
}


void ThreadMutex::acquire() const {
#if defined (_WIN64) || defined (_WIN32)

    __try {
        EnterCriticalSection(&m_cs);
    }
    __except (GetExceptionCode() == EXCEPTION_INVALID_HANDLE
    ? EXCEPTION_EXECUTE_HANDLER : EXCEPTION_CONTINUE_SEARCH) {
        // On Windows 2000 EXCEPTION_INVALID_HANDLE could happen
        // in very low memory situations. I translate this into a C++
        // exception here (Note that due to EXCEPTION_CONTINUE_SEARCH
        // any other SE will be 'rethrown' untranslated!).
        // On XP or later EnterCriticalSection is guaranteed not to
        // fail due to lack of resources.
        const char* pszMsg = "ThreadMutex::acquire() failed with EXCEPTION_INVALID_HANDLE"
            " - this may be an indicator of a very low memory situation";
        throw JException(pszMsg);
    }

#else /* POSIX threads*/

    int rc = pthread_mutex_lock(&m_cs);

    if (rc != 0) {
        const int MAX_LEN = 64;
        char code[MAX_LEN] = {0};
        sprintf(code, "%d", rc);
        const char* errMsg = "ThreadMutex::acquire() failed with return code ";
        SlimString msg(errMsg);
        msg.append(code);
        throw JException(msg);
    }

#endif /* (_WIN64) || (_WIN32) */
}
