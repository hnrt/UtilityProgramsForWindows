#include "pch.h"
#include "hnrt/BCryptAuthenticatedCipherModeInfo.h"
#include "hnrt/BCryptKeyHandle.h"
#include "hnrt/Heap.h"
#include "hnrt/Interlocked.h"


#define COPY_PUCHAR_MEMBER(x,y,z) x = z.x && z.y ? Allocate<UCHAR>(z.y) : NULL; y = z.x && z.y ? z.y : 0; memcpy_s(x, y, z.x, z.y)


using namespace hnrt;


BCryptAuthenticatedCipherModeInfo::BCryptAuthenticatedCipherModeInfo()
	: BCRYPT_AUTHENTICATED_CIPHER_MODE_INFO()
{
	BCRYPT_INIT_AUTH_MODE_INFO(*this);
}


BCryptAuthenticatedCipherModeInfo::BCryptAuthenticatedCipherModeInfo(const BCryptAuthenticatedCipherModeInfo& other)
	: BCRYPT_AUTHENTICATED_CIPHER_MODE_INFO()
{
	BCRYPT_INIT_AUTH_MODE_INFO(*this);
	COPY_PUCHAR_MEMBER(pbNonce, cbNonce, other);
	COPY_PUCHAR_MEMBER(pbAuthData, cbAuthData, other);
	COPY_PUCHAR_MEMBER(pbTag, cbTag, other);
	COPY_PUCHAR_MEMBER(pbMacContext, cbMacContext, other);
	cbAAD = other.cbAAD;
	cbData = other.cbData;
	dwFlags = other.dwFlags;
}


BCryptAuthenticatedCipherModeInfo::~BCryptAuthenticatedCipherModeInfo()
{
	free(pbNonce);
	free(pbAuthData);
	free(pbTag);
	free(pbMacContext);
}


BCryptAuthenticatedCipherModeInfo& BCryptAuthenticatedCipherModeInfo::operator =(const BCryptAuthenticatedCipherModeInfo& other)
{
	free(pbNonce);
	free(pbAuthData);
	free(pbTag);
	free(pbMacContext);
	BCRYPT_INIT_AUTH_MODE_INFO(*this);
	COPY_PUCHAR_MEMBER(pbNonce, cbNonce, other);
	COPY_PUCHAR_MEMBER(pbAuthData, cbAuthData, other);
	COPY_PUCHAR_MEMBER(pbTag, cbTag, other);
	COPY_PUCHAR_MEMBER(pbMacContext, cbMacContext, other);
	cbAAD = other.cbAAD;
	cbData = other.cbData;
	dwFlags = other.dwFlags;
	return *this;
}


BCryptAuthenticatedCipherModeInfo& BCryptAuthenticatedCipherModeInfo::SetNonce(const void* ptr, size_t cb)
{
	free(Interlocked<PUCHAR>::ExchangePointer(&pbNonce, Allocate<UCHAR>(cb)));
	cbNonce = static_cast<ULONG>(cb);
	memcpy_s(pbNonce, cbNonce, ptr, cb);
	return *this;
}


BCryptAuthenticatedCipherModeInfo& BCryptAuthenticatedCipherModeInfo::SetAuthDataSize(size_t cb)
{
	free(Interlocked<PUCHAR>::ExchangePointer(&pbAuthData, Allocate<UCHAR>(cb)));
	cbAuthData = static_cast<ULONG>(cb);
	memset(pbAuthData, 0, cbAuthData);
	return *this;
}


BCryptAuthenticatedCipherModeInfo& BCryptAuthenticatedCipherModeInfo::SetAuthData(const void* ptr, size_t cb)
{
	free(Interlocked<PUCHAR>::ExchangePointer(&pbAuthData, Allocate<UCHAR>(cb)));
	cbAuthData = static_cast<ULONG>(cb);
	memcpy_s(pbAuthData, cbAuthData, ptr, cb);
	return *this;
}


BCryptAuthenticatedCipherModeInfo& BCryptAuthenticatedCipherModeInfo::SetTagSize(size_t cb)
{
	free(Interlocked<PUCHAR>::ExchangePointer(&pbTag, Allocate<UCHAR>(cb)));
	cbTag = static_cast<ULONG>(cb);
	memset(pbTag, 0, cbTag);
	return *this;
}


BCryptAuthenticatedCipherModeInfo& BCryptAuthenticatedCipherModeInfo::SetTag(const void* ptr, size_t cb)
{
	free(Interlocked<PUCHAR>::ExchangePointer(&pbTag, Allocate<UCHAR>(cb)));
	cbTag = static_cast<ULONG>(cb);
	memcpy_s(pbTag, cbTag, ptr, cb);
	return *this;
}


BCryptAuthenticatedCipherModeInfo& BCryptAuthenticatedCipherModeInfo::SetMacContextSize(size_t cb)
{
	free(Interlocked<PUCHAR>::ExchangePointer(&pbMacContext, Allocate<UCHAR>(cb)));
	cbMacContext = static_cast<ULONG>(cb);
	memset(pbMacContext, 0, cbMacContext);
	return *this;
}


BCryptAuthenticatedCipherModeInfo& BCryptAuthenticatedCipherModeInfo::SetDataSize(size_t cb)
{
	cbData = static_cast<ULONG>(cb);
	return *this;
}


BCryptAuthenticatedCipherModeInfo& BCryptAuthenticatedCipherModeInfo::SetFlags(ULONG dw)
{
	dwFlags |= dw;
	return *this;
}


BCryptAuthenticatedCipherModeInfo& BCryptAuthenticatedCipherModeInfo::ResetFlags(ULONG dw)
{
	dwFlags &= ~dw;
	return *this;
}
