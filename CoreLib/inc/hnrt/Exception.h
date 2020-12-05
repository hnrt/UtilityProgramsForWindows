#pragma once


namespace hnrt
{
    class Exception
    {
    public:

        Exception();
        Exception(PCWSTR pszFormat, ...);
        Exception(const Exception& src);
        virtual ~Exception() = default;
        Exception& operator =(const Exception& src);
        PCWSTR get_Message() const;

        __declspec(property(get = get_Message)) PCWSTR Message;

    protected:

        PCWSTR m_pszMessage;
    };

    inline PCWSTR Exception::get_Message() const
    {
        return m_pszMessage ? m_pszMessage : L"";
    }
}
