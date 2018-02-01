/* ---------------------------------------------------------------------- */
/* stdafx.h :                                                             */
/* Include file for standard system include files, or project specific    */
/* include files that are used frequently, but are changed infrequently.  */
/* ---------------------------------------------------------------------- */

#ifndef STDAFX_INCLUDED_
#define STDAFX_INCLUDED_


#if defined (_WIN64) || defined (_WIN32)

/* Specifies that the minimum required platform is Windows Server 2008 or Windows Vista */
#ifndef WINVER
#define WINVER _WIN32_WINNT_LONGHORN
#endif

#ifndef _WIN32_WINNT
#define _WIN32_WINNT _WIN32_WINNT_LONGHORN
#endif

#ifndef _WIN32_WINDOWS
#define _WIN32_WINDOWS _WIN32_WINNT_LONGHORN
#endif

/* Specifies that the minimum required IE platform is IE 6.0 SP2 */
#ifndef _WIN32_IE
#define _WIN32_IE _WIN32_IE_IE60SP2
#endif



/* Expand the g++ shared library "hidden" attribute to nothing */
#define __GCC_DONT_EXPORT




/* Exclude rarely-used stuff from Windows headers */
#define WIN32_LEAN_AND_MEAN

/* Windows Header File(s) */
#include <windows.h>


//namespace abi
//{  
//  /** 
//   * A dummy replacement for the 'magic' Linux NPTL Posix exception that
//   * can be caught by reference to recognize forced stack unwinding.
//   * This is thrown on Linux when a Posix thread gets cancelled.
//   */
//  class __forced_unwind
//  {
//    ~__forced_unwind() throw();
//    virtual void dummy() = 0; // prevent catch by value
//  };
//}


#else /* Unix or Linux */


#if defined (__linux) && defined (__GNUG__)


#define __GCC_DONT_EXPORT __attribute ((visibility ("hidden")))

#ifndef IDL_PROTOTYPES
#define IDL_PROTOTYPES 1
#endif /* IDL_PROTOTYPES */


#else /* Unix or non-g++ Linux */


/* Expand the g++ shared library "hidden" attribute to nothing */
#define __GCC_DONT_EXPORT




//#error "This is Unix (or Linux with non-g++)!"
//#error "You should add your standard includes in stdafx.h or remove this #error macro line in stdafx.h"

#endif /* (__linux) && (__GNUG__) */


#endif /* WIN32/64 versus Unix/Linux */


#endif /* STDAFX_INCLUDED_ */
