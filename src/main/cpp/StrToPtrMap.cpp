/////////////////////////////////////////////////////////////////////////////
// StrToPtrMap.cpp: implementation of the StrToPtrMap class.
/////////////////////////////////////////////////////////////////////////////

#include "StrToPtrMap.h"

#include <string.h> // memset, memcpy, strlen, strcmp


/////////////////////////////////////////////////////////////////////////////
// StrToPtrMap
/////////////////////////////////////////////////////////////////////////////

StrToPtrMap::StrToPtrMap(unsigned int nTableSize, unsigned int nBlockSize)
    :   m_pHashTable(NULL),
        m_nHashTableSize(nTableSize),   // default table size: 499
        m_nCount(0),
        m_pFreeList(NULL),
        m_pBlocks(NULL),
        m_nBlockSize(nBlockSize)        // default list-length: 8
{
}

StrToPtrMap::~StrToPtrMap()
{
    clear();
}

//  force allocation of a hash table or
//  override the default hash table size
void StrToPtrMap::initHashTable(unsigned int nHashSize)
{
    if (m_pHashTable) {
        delete[] m_pHashTable;
    }

    m_pHashTable = new CAssociation* [nHashSize];
    memset(m_pHashTable, 0, sizeof(CAssociation*) * nHashSize);

    m_nHashTableSize = nHashSize;
}


void StrToPtrMap::clear()
{
    if (m_pHashTable)
    {
        CAssociation* pAssoc;

        // destroy elements
        for (unsigned int nHash = 0; nHash < m_nHashTableSize; nHash++)
        {
            for (pAssoc = m_pHashTable[nHash];
                pAssoc != NULL;
                pAssoc = pAssoc->pNext)
            {
                // free up string data
                delete [] pAssoc->key;
            }
        }

        // free hash table
        delete [] m_pHashTable;
        m_pHashTable = NULL;
    }

    m_nCount = 0;
    m_pFreeList = NULL;
    m_pBlocks->freeDataChain();
    m_pBlocks = NULL;
}


void*& StrToPtrMap::operator[](const char* pszKey)
{
    unsigned int nHash;

    CAssociation* pAssoc;

    if ((pAssoc = getAssocAndIndex(pszKey, nHash)) == NULL)
    {
        if (m_pHashTable == NULL) {
            initHashTable(m_nHashTableSize);
        }

        // it doesn't exist, add a new Association
        pAssoc = newAssoc();
        pAssoc->nHashValue = nHash;

        // strcpy pszKey into location pAssoc->key
        size_t nLen = strlen(pszKey) + 1;
        pAssoc->key = new char[nLen];
        memcpy(pAssoc->key, pszKey, sizeof(char) * nLen);

        // put into hash table
        pAssoc->pNext = m_pHashTable[nHash];
        m_pHashTable[nHash] = pAssoc;
    }

    return pAssoc->value;  // return new reference
}

// remove key -- return true if removed
bool StrToPtrMap::erase(const char* pszKey)
{
    if (m_pHashTable == NULL) {
        return false; // nothing in table
    }

    CAssociation** ppAssocPrev;
    ppAssocPrev = &m_pHashTable[hashKey(pszKey) % m_nHashTableSize];

    CAssociation* pAssoc;
    for (pAssoc = *ppAssocPrev; pAssoc != NULL; pAssoc = pAssoc->pNext)
    {
        if (strcmp(pAssoc->key, pszKey) == 0)
        {
            // remove it
            *ppAssocPrev = pAssoc->pNext; // remove from list
            freeAssoc(pAssoc);
            return true;
        }
        ppAssocPrev = &pAssoc->pNext;
    }

    return false; // not found
}


bool StrToPtrMap::getValue(const char* pszKey, void*& rValue) const
{
    CAssociation* pAssoc = getAssoc(pszKey);

    if (pAssoc) {
        rValue = pAssoc->value;
        return true;
    } else {
        return false; // not in map
    }
}


bool StrToPtrMap::containsKey(const char* pszKey) const
{
    if (getAssoc(pszKey)) {
        return true;
    } else {
        return false; // not in map
    }
}

/////////////////////////////////////////////////////////////////////////////
// CAssociation helpers
/////////////////////////////////////////////////////////////////////////////

StrToPtrMap::CAssociation* StrToPtrMap::newAssoc()
{
    if (!m_pFreeList)
    {
        // add another block
        CBlock* newBlock = CBlock::create(m_pBlocks,
                                        m_nBlockSize,
                                        sizeof(StrToPtrMap::CAssociation));

        // chain them into free list
        StrToPtrMap::CAssociation* pAssoc = (StrToPtrMap::CAssociation*) newBlock->data();

        // free in reverse order to make it easier to debug
        pAssoc += m_nBlockSize - 1;

        for (int i = m_nBlockSize-1; i >= 0; i--, pAssoc--)
        {
            pAssoc->pNext = m_pFreeList;
            m_pFreeList = pAssoc;
        }
    }


    StrToPtrMap::CAssociation* pAssoc = m_pFreeList;
    m_pFreeList = m_pFreeList->pNext;
    m_nCount++;

    // initialize (key, value) pair
    pAssoc->key = NULL;
    pAssoc->value = NULL;

    return pAssoc;
}


void StrToPtrMap::freeAssoc(StrToPtrMap::CAssociation* pAssoc)
{
    // free up string data
    delete [] pAssoc->key;

    pAssoc->pNext = m_pFreeList;
    m_pFreeList = pAssoc;
    m_nCount--;

    // if no more elements, cleanup completely
    if (m_nCount == 0) {
        clear();
    }
}

// find association and compute index in hash table
StrToPtrMap::CAssociation* StrToPtrMap::getAssocAndIndex(const char* pszKey, unsigned int& nHash) const
{
    nHash = hashKey(pszKey) % m_nHashTableSize;

    if (!m_pHashTable) {
        return NULL;
    }

    CAssociation* pAssoc;

    // see if it exists
    for (pAssoc = m_pHashTable[nHash]; pAssoc != NULL; pAssoc = pAssoc->pNext)
    {
        if (strcmp(pAssoc->key, pszKey) == 0) {
            return pAssoc;
        }
    }

    return NULL;
}

// find association
StrToPtrMap::CAssociation* StrToPtrMap::getAssoc(const char* pszKey) const
{
    if (!m_pHashTable) {
        return NULL;
    }

    CAssociation* pAssoc;
    unsigned int nHash = hashKey(pszKey) % m_nHashTableSize;

    // see if it exists
    for (pAssoc = m_pHashTable[nHash]; pAssoc != NULL; pAssoc = pAssoc->pNext)
    {
        if (strcmp(pAssoc->key, pszKey) == 0) {
            return pAssoc;
        }
    }

    return NULL;
}

StrToPtrMap::CAssociation* StrToPtrMap::getFirstAssoc(unsigned int nHash, bool& bMore) const
{
    if (!m_pHashTable) {
        return NULL;
    }

    CAssociation* pFirstAssoc = NULL;
    unsigned int nCount = 0;

    // see if it exists
    for (CAssociation* pAssoc = m_pHashTable[nHash]; pAssoc != NULL; pAssoc = pAssoc->pNext)
    {
        if (nCount == 0) {
            pFirstAssoc = pAssoc;
        }

        nCount++;
    }

    if (nCount == 1)
    {
        bMore = false;
        return pFirstAssoc;
    }
    else if (nCount > 1)
    {
        bMore = true;
        return pFirstAssoc;
    }
    else {
        return NULL;
    }
}

/////////////////////////////////////////////////////////////////////////////
// CBlock methods
/////////////////////////////////////////////////////////////////////////////

StrToPtrMap::CBlock* StrToPtrMap::CBlock::create(CBlock*& pHead, unsigned int nMax, unsigned int nSize)
{
    CBlock* p = (CBlock*) new unsigned char[sizeof(CBlock) + nMax * nSize];

    p->pNext = pHead;
    pHead = p;  // change head (adds in reverse order for simplicity)
    return p;
}


void StrToPtrMap::CBlock::freeDataChain()  // free this one and links
{
    CBlock* p = this;
    while (p != NULL)
    {
        unsigned char* bytes = (unsigned char*) p;
        CBlock* pNext = p->pNext;
        delete[] bytes;
        p = pNext;
    }
}

/////////////////////////////////////////////////////////////////////////////
// Debug helpers
/////////////////////////////////////////////////////////////////////////////

/*
unsigned int StrToPtrMap::getIndex(const char* pszKey) const
{
    return hashKey(pszKey) % m_nHashTableSize;
}

unsigned int StrToPtrMap::getOccupation(unsigned int nIndex) const
{
    unsigned int nCount = 0;
    CAssociation* pAssoc;

    for (pAssoc = m_pHashTable[nIndex]; pAssoc != NULL; pAssoc = pAssoc->pNext)
    {
        nCount++;
    }

    return nCount;
}

int StrToPtrMap::getValue(unsigned int nIndex, void*& rValue) const
{
    bool bMore = true;  // be pessimistic

    CAssociation* pAssoc = getFirstAssoc(nIndex, bMore);

    if (pAssoc && !bMore)
    {
        rValue = pAssoc->value;
        return 1;   // unique
    }
    else if (pAssoc && bMore)
    {
        rValue = NULL;
        return 0;   // in map, but not unique
    }
    else
    {
        rValue = NULL;
        return -1;  // not in map
    }
}
*/
