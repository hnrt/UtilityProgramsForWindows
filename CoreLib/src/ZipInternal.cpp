#include "pch.h"
#include "hnrt/ComException.h"
#include "hnrt/Variant.h"
#include "ZipInternal.h"


using namespace hnrt;


static IShellDispatch* s_pShellDispatch = nullptr;


Folder* ZipInternal::GetFolder(PCWSTR pszPath)
{
    Folder* pFolder;
    HRESULT hr;
    if (!s_pShellDispatch)
    {
        hr = CoCreateInstance(CLSID_Shell, NULL, CLSCTX_INPROC_SERVER, IID_IShellDispatch, reinterpret_cast<LPVOID*>(&s_pShellDispatch));
        if (FAILED(hr))
        {
            throw ComException(hr, L"IShellDispatch unavailable.");
        }
    }
    Variant varPath(VT_BSTR, pszPath);
    hr = s_pShellDispatch->NameSpace(varPath, &pFolder);
    if (hr != S_OK)
    {
        throw ComException(hr, L"Folder unavailable.");
    }
    return pFolder;
}
