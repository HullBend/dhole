
#ifndef JHANDLECACHE_INCLUDED_
#define JHANDLECACHE_INCLUDED_

#ifndef STDAFX_INCLUDED_
#include "stdafx.h"
#endif /* STDAFX_INCLUDED_ */

#ifndef __STRTOPTRMAP_H_INCLUDED_
#include "StrToPtrMap.h"
#endif /* __STRTOPTRMAP_H_INCLUDED_ */



class __GCC_DONT_EXPORT JHandleCache {
public:
    ~JHandleCache();

    static JHandleCache* instance();
    static void clear();

    void add(const char* pszUniqueKey, void* jvmHandle);
    void* get(const char* pszUniqueKey);

private: // constructor
    JHandleCache();
private: // data
    static JHandleCache* m_instance;
    StrToPtrMap m_cache;
};


#endif /* JHANDLECACHE_INCLUDED_ */
