#pragma once

#include "hnrt/BCryptHandle.h"

namespace hnrt
{
    class BCryptAlgHandle
        : protected BCryptHandle
    {
    public:

        BCryptAlgHandle();
        BCryptAlgHandle(const BCryptAlgHandle&) = delete;
        virtual ~BCryptAlgHandle();
        void operator =(const BCryptAlgHandle&) = delete;
        operator BCRYPT_ALG_HANDLE() const;
        void Open(PCWSTR);
        void Close();
        String get_AlgorithmName() const;
        DWORD get_ObjectLength() const;
        DWORD get_HashLength() const;
        DWORD get_BlockLength() const;
        Array<DWORD> get_BlockSizeList() const;
        Array<DWORD> get_KeyLengths() const;
        String get_ChainingMode() const;
        void set_ChainingMode(String) const;
        String get_ChainingModeShort() const;
        Array<DWORD> get_AuthTagLengths() const;

        __declspec(property(get = get_AlgorithmName)) String AlgorithmName;
        __declspec(property(get = get_ObjectLength)) DWORD ObjectLength;
        __declspec(property(get = get_HashLength)) DWORD HashLength;
        __declspec(property(get = get_BlockLength)) DWORD BlockLength;
        __declspec(property(get = get_BlockSizeList)) Array<DWORD> BlockSizeList;
        __declspec(property(get = get_KeyLengths)) Array<DWORD> KeyLengths;
        __declspec(property(get = get_ChainingMode, put = set_ChainingMode)) String ChainingMode;
        __declspec(property(get = get_ChainingModeShort)) String ChainingModeShort;
        __declspec(property(get = get_AuthTagLengths)) Array<DWORD> AuthTagLengths;
    };

    inline BCryptAlgHandle::BCryptAlgHandle()
        : BCryptHandle()
    {
    }

    inline BCryptAlgHandle::~BCryptAlgHandle()
    {
        if (m_h)
        {
            Close();
        }
    }

    inline BCryptAlgHandle::operator BCRYPT_ALG_HANDLE() const
    {
        return m_h;
    }

    inline String BCryptAlgHandle::get_AlgorithmName() const
    {
        return GetPropertyString(BCRYPT_ALGORITHM_NAME);
    }

    inline DWORD BCryptAlgHandle::get_ObjectLength() const
    {
        return GetPropertyDWORD(BCRYPT_OBJECT_LENGTH);
    }

    inline DWORD BCryptAlgHandle::get_HashLength() const
    {
        return GetPropertyDWORD(BCRYPT_HASH_LENGTH);
    }

    inline DWORD BCryptAlgHandle::get_BlockLength() const
    {
        return GetPropertyDWORD(BCRYPT_BLOCK_LENGTH);
    }

    inline Array<DWORD> BCryptAlgHandle::get_BlockSizeList() const
    {
        return GetPropertyArrayDWORD(BCRYPT_BLOCK_SIZE_LIST);
    }

    inline Array<DWORD> BCryptAlgHandle::get_KeyLengths() const
    {
        return GetPropertyKeyLengths(BCRYPT_KEY_LENGTHS);
    }

    inline String BCryptAlgHandle::get_ChainingMode() const
    {
        return GetPropertyString(BCRYPT_CHAINING_MODE);
    }

    inline void BCryptAlgHandle::set_ChainingMode(String szChainingMode) const
    {
        SetProperty(BCRYPT_CHAINING_MODE, szChainingMode);
    }

    inline Array<DWORD> BCryptAlgHandle::get_AuthTagLengths() const
    {
        return GetPropertyKeyLengths(BCRYPT_AUTH_TAG_LENGTH);
    }
}
