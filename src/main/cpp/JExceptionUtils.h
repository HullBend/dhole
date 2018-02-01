
#ifndef JEXCEPTIONUTILS_INCLUDED_
#define JEXCEPTIONUTILS_INCLUDED_


#ifndef _JAVASOFT_JNI_H_
#include "jni.h"
#endif /* _JAVASOFT_JNI_H_ */

#ifndef SLIMSTRING_INCLUDED_
#include "SlimString.h"
#endif /* SLIMSTRING_INCLUDED_ */

#ifndef STDAFX_INCLUDED_
#include "stdafx.h"
#endif /* STDAFX_INCLUDED_ */



/*
 * Formats an exception as a string with its stack trace.
 */
bool __GCC_DONT_EXPORT printStackTrace(JNIEnv* env, jthrowable exception, char* buf, size_t bufLen);


/*
 * Formats an exception as a string with its stack trace and additional context information.
 */
bool __GCC_DONT_EXPORT printStackTrace(JNIEnv* env, jthrowable exception, SlimString& msg, char* context);


#endif /* JEXCEPTIONUTILS_INCLUDED_ */
