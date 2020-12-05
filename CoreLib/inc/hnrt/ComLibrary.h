#pragma once


#include <combaseapi.h>


namespace hnrt
{
    class ComLibrary
    {
    public:

        ComLibrary(DWORD dwCoInit = COINIT_APARTMENTTHREADED);
        ComLibrary(const ComLibrary&) = delete;
        ~ComLibrary();
        void operator =(const ComLibrary&) = delete;
        HRESULT get_InitResult() const;
        __declspec(property(get = get_InitResult)) HRESULT InitResult;

        static PCWSTR ToString(HRESULT hr);

    private:

        HRESULT m_hrInit;
    };

    inline HRESULT ComLibrary::get_InitResult() const
    {
        return m_hrInit;
    }
}
