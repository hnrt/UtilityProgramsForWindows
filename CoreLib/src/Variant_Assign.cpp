#include "pch.h"
#include "hnrt/Variant.h"
#include "hnrt/Exception.h"


using namespace hnrt;


Variant& Variant::Assign(const Variant& src)
{
	VariantClear(this);
	memcpy_s(this, sizeof(VARIANT), &src, sizeof(VARIANT));
	switch (vt)
	{
    case VT_BSTR: // 8
        bstrVal = SysAllocString(bstrVal);
        break;
    case VT_DISPATCH: // 9
        pdispVal->AddRef();
        break;
    case VT_UNKNOWN: // 13
        punkVal->AddRef();
        break;
    default:
        break;
	}
	return *this;
}


Variant& Variant::Assign(VARTYPE vt, ...)
{
    va_list argList;
    va_start(argList, vt);
    VaAssign(vt, argList);
    va_end(argList);
    return *this;
}


void Variant::VaAssign(VARTYPE vt, va_list argList)
{
    switch (this->vt)
    {
    case VT_BSTR: // 8
        SysFreeString(bstrVal);
        break;
    case VT_DISPATCH: // 9
        pdispVal->Release();
        break;
    case VT_UNKNOWN: // 13
        punkVal->Release();
        break;
    default:
        break;
    }
    memset(this, 0, sizeof(VARIANT));
    this->vt = vt;
    switch (vt)
    {
    case VT_EMPTY: // 0
        break;
    case VT_NULL: // 1
        byref = nullptr;
        break;
    case VT_I2: // 2
        iVal = va_arg(argList, SHORT);
        break;
    case VT_I4: // 3
        lVal = va_arg(argList, LONG);
        break;
    case VT_R4: // 4
        fltVal = va_arg(argList, FLOAT);
        break;
    case VT_R8: // 5
        dblVal = va_arg(argList, DOUBLE);
        break;
    case VT_CY: // 6
        cyVal = va_arg(argList, CY);
        break;
    case VT_DATE: // 7
        date = va_arg(argList, DATE);
        break;
    case VT_BSTR: // 8
        bstrVal = SysAllocString(va_arg(argList, const OLECHAR*));
        break;
    case VT_DISPATCH: // 9
        pdispVal = va_arg(argList, IDispatch*);
        pdispVal->AddRef();
        break;
    case VT_ERROR: // 10
        scode = va_arg(argList, SCODE);
        break;
    case VT_BOOL: // 11
        boolVal = va_arg(argList, VARIANT_BOOL);
        break;
    case VT_VARIANT: // 12
        pvarVal = va_arg(argList, VARIANT*);
        break;
    case VT_UNKNOWN: // 13
        punkVal = va_arg(argList, IUnknown*);
        punkVal->AddRef();
        break;
    case VT_DECIMAL: // 14
        decVal = va_arg(argList, DECIMAL);
        break;
    case VT_I1: // 16
        cVal = va_arg(argList, CHAR);
        break;
    case VT_UI1: // 17
        bVal = va_arg(argList, BYTE);
        break;
    case VT_UI2: // 18
        uiVal = va_arg(argList, USHORT);
        break;
    case VT_UI4: // 19
        ulVal = va_arg(argList, ULONG);
        break;
    case VT_I8: // 20
        llVal = va_arg(argList, LONGLONG);
        break;
    case VT_UI8: // 21
        ullVal = va_arg(argList, ULONGLONG);
        break;
    case VT_INT: // 22
        intVal = va_arg(argList, INT);
        break;
    case VT_UINT: // 23
        uintVal = va_arg(argList, UINT);
        break;
    case VT_VOID: // 24
        byref = va_arg(argList, PVOID);
        break;
    case VT_HRESULT: // 25
        lVal = va_arg(argList, HRESULT);
        break;
    case VT_PTR: // 26
        byref = va_arg(argList, PVOID);
        break;
    case VT_SAFEARRAY: // 27
        throw Exception(L"Variant::Assign: VT_SAFEARRAY not supported");
    case VT_CARRAY: // 28
        throw Exception(L"Variant::Assign: VT_CARRAY not supported");
    case VT_USERDEFINED: // 29
        throw Exception(L"Variant::Assign: VT_USERDEFINED not supported");
    case VT_LPSTR: // 30
        byref = va_arg(argList, LPSTR);
        break;
    case VT_LPWSTR: // 31
        byref = va_arg(argList, LPWSTR);
        break;
    case VT_RECORD: // 36
        throw Exception(L"Variant::Assign: VT_RECORD not supported");
    case VT_INT_PTR: // 37
        pintVal = va_arg(argList, INT*);
        break;
    case VT_UINT_PTR: // 38
        puintVal = va_arg(argList, UINT*);
        break;
    case VT_FILETIME: // 64
        ullVal = va_arg(argList, ULONGLONG);
        break;
    case VT_BLOB: // 65
        throw Exception(L"Variant::Assign: VT_BLOB not supported");
    case VT_STREAM: // 66
        throw Exception(L"Variant::Assign: VT_STREAM not supported");
    case VT_STORAGE: // 67
        throw Exception(L"Variant::Assign: VT_STORAGE not supported");
    case VT_STREAMED_OBJECT: // 68
        throw Exception(L"Variant::Assign: VT_STREAMED_OBJECT not supported");
    case VT_STORED_OBJECT: // 69
        throw Exception(L"Variant::Assign: VT_STORED_OBJECT not supported");
    case VT_BLOB_OBJECT: // 70
        throw Exception(L"Variant::Assign: VT_BLOB_OBJECT not supported");
    case VT_CF: // 71
        throw Exception(L"Variant::Assign: VT_CF not supported");
    case VT_CLSID: // 72
    {
        CLSID clsid = va_arg(argList, CLSID);
        memcpy_s(&ullVal, sizeof(CLSID), &clsid, sizeof(CLSID));
        break;
    }
    default:
        throw Exception(L"Variant::Assign: %d not supported", vt);
    }
}
