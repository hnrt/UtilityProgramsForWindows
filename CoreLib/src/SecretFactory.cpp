#include "pch.h"
#include "hnrt/SecretFactory.h"
#include "SecretInternal.h"


using namespace hnrt;


RefPtr<Secret> SecretFactory::Create()
{
    return RefPtr<Secret>(new SecretInternal());
}


RefPtr<Secret> SecretFactory::Create(const unsigned char key[SECRET_KEY_LENGTH], const unsigned char iv[SECRET_IV_LENGTH])
{
    return RefPtr<Secret>(new SecretInternal(key, iv));
}


RefPtr<Secret> SecretFactory::Create(PCWSTR pszKey, PCWSTR pszIV)
{
    return RefPtr<Secret>(new SecretInternal(pszKey, pszIV));
}
