
#include "JHandleCache.h"

#ifndef JEXCEPTION_INCLUDED_
#include "JException.h"
#endif /* JEXCEPTION_INCLUDED_ */

#ifndef SLIMSTRING_INCLUDED_
#include "SlimString.h"
#endif /* SLIMSTRING_INCLUDED_ */



// static data
JHandleCache* JHandleCache::m_instance = 0;


// constructor
JHandleCache::JHandleCache()
{
}

// destructor
JHandleCache::~JHandleCache() {
    m_cache.clear();
}

// member methods
void JHandleCache::add(const char* pszUniqueKey, void* jvmHandle) {
    m_cache[pszUniqueKey] = jvmHandle;
}

void* JHandleCache::get(const char* pszUniqueKey) {
    void* handle = 0;
    bool found = m_cache.getValue(pszUniqueKey, handle);
    if (!found) {
        SlimString msg("JHandleCache::get - Lookup of non-existing key: ");
        msg.append(pszUniqueKey);
        throw JException(msg);
    }
    return handle;
}

// static member methods
JHandleCache* JHandleCache::instance() {
    if (m_instance == 0) {
        m_instance = new JHandleCache();
    }
    return m_instance;
}

void JHandleCache::clear() {
    if (m_instance) {
        delete m_instance;
        m_instance = 0;
    }
}

