/////////////////////////////////////////////////////////////////////////////
// StrToPtrMap.h: interface for the StrToPtrMap class.
/////////////////////////////////////////////////////////////////////////////

#if !defined(__STRTOPTRMAP_H_INCLUDED_)
#define __STRTOPTRMAP_H_INCLUDED_

#ifndef STDAFX_INCLUDED_
#include "stdafx.h"
#endif /* STDAFX_INCLUDED_ */

#include <stddef.h> // size_t


class __GCC_DONT_EXPORT StrToPtrMap  
{
public:

    // Construction/Destruction
    StrToPtrMap(unsigned int nTableSize = 499, unsigned int nBlockSize = 8);
    ~StrToPtrMap();

    // methods
    bool getValue(const char* pszKey, void*& rValue) const; // see if a value matching pszKey exists
    void*& operator[](const char* pszKey); // add if not already there
    void insert(const char* pszKey, void* newValue); // add a new (key, value) pair

    bool erase(const char* pszKey); // remove whole entry identified by key
    void clear(); // remove all entries

    size_t size() const;
    bool empty() const;

    bool containsKey(const char* pszKey) const; // check for existence of key

    // Debug helpers
    //unsigned int getIndex(const char* pszKey) const;
    //unsigned int getOccupation(unsigned int nIndex) const;
    //int getValue(unsigned int nIndex, void*& rValue) const;

private:

    // declarations
    struct CBlock // variable-length structure
    {
        static CBlock* create(CBlock*& head, unsigned int nMax, unsigned int nSize);
        void* data();
        void freeDataChain();

        CBlock* pNext;
    };

    struct CAssociation
    {
        CAssociation* pNext;
        unsigned int nHashValue; // needed for efficient iteration
        char* key;
        void* value;
    };

    // helper methods
    void initHashTable(unsigned int nHashSize);
    unsigned int hashKey(const char* pszKey) const;

    // CAssociation helper methods
    CAssociation* newAssoc();
    void freeAssoc(CAssociation* pAssoc);
    CAssociation* getAssocAndIndex(const char* pszKey, unsigned int& nHash) const;
    CAssociation* getAssoc(const char* pszKey) const;
    // CAssociation Debug helper
    CAssociation* getFirstAssoc(unsigned int nHash, bool& bMore) const;

    // data members
    CAssociation**      m_pHashTable;
    unsigned int        m_nHashTableSize;
    size_t              m_nCount;
    CAssociation*       m_pFreeList;
    CBlock*             m_pBlocks;
    unsigned int        m_nBlockSize; // map extension memory-allocation granularity

};


/////////////////////////////////////////////////////////////////////////////
// inline implementation
/////////////////////////////////////////////////////////////////////////////

inline
void* StrToPtrMap::CBlock::data()
{
    return this + 1;
}

inline
size_t StrToPtrMap::size() const
{
    return m_nCount;
}

inline
bool StrToPtrMap::empty() const
{
    return m_nCount == 0;
}

inline
void StrToPtrMap::insert(const char* pszKey, void* newValue)
{
    (*this)[pszKey] = newValue;
}

inline
unsigned int StrToPtrMap::hashKey(const char* pszKey) const
{
    unsigned int nHash = 2435275227u; // some magic number

    while (*pszKey) {
        nHash = (nHash << 5) + nHash + *pszKey++;
    }

    return nHash;
}


#endif // !defined(__STRTOPTRMAP_H_INCLUDED_)
