#pragma once


#include "hnrt/Secret.h"
#include "hnrt/RefPtr.h"


namespace hnrt
{
    class SecretFactory
    {
    public:

        static RefPtr<Secret> Create();
        static RefPtr<Secret> Create(const unsigned char key[SECRET_KEY_LENGTH], const unsigned char iv[SECRET_IV_LENGTH]);
        static RefPtr<Secret> Create(PCWSTR pszKey, PCWSTR pszIV);
    };
}
