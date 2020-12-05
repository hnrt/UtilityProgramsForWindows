#pragma once


#include <Windows.h>


namespace hnrt
{
    class SidPtr
    {
    public:

        static SidPtr LowIntegrity;
        static SidPtr Everyone;
        static SidPtr AuthenticatedUser;
        static SidPtr Administrators;
        static SidPtr Users;

        SidPtr(SID_IDENTIFIER_AUTHORITY auth, DWORD dwSubAuth0);
        SidPtr(SID_IDENTIFIER_AUTHORITY auth, DWORD dwSubAuth0, DWORD dwSubAuth1);
        SidPtr(SID_IDENTIFIER_AUTHORITY auth, DWORD dwSubAuth0, DWORD dwSubAuth1, DWORD dwSubAuth2);
        SidPtr(SID_IDENTIFIER_AUTHORITY auth, DWORD dwSubAuth0, DWORD dwSubAuth1, DWORD dwSubAuth2, DWORD dwSubAuth3);
        SidPtr(SID_IDENTIFIER_AUTHORITY auth, DWORD dwSubAuth0, DWORD dwSubAuth1, DWORD dwSubAuth2, DWORD dwSubAuth3, DWORD dwSubAuth4);
        SidPtr(SID_IDENTIFIER_AUTHORITY auth, DWORD dwSubAuth0, DWORD dwSubAuth1, DWORD dwSubAuth2, DWORD dwSubAuth3, DWORD dwSubAuth4, DWORD dwSubAuth5);
        SidPtr(SID_IDENTIFIER_AUTHORITY auth, DWORD dwSubAuth0, DWORD dwSubAuth1, DWORD dwSubAuth2, DWORD dwSubAuth3, DWORD dwSubAuth4, DWORD dwSubAuth5, DWORD dwSubAuth6);
        SidPtr(SID_IDENTIFIER_AUTHORITY auth, DWORD dwSubAuth0, DWORD dwSubAuth1, DWORD dwSubAuth2, DWORD dwSubAuth3, DWORD dwSubAuth4, DWORD dwSubAuth5, DWORD dwSubAuth6, DWORD dwSubAuth7);
        SidPtr(const SidPtr&) = delete;
        ~SidPtr();
        void operator =(const SidPtr&) = delete;
        operator PSID() const;

    private:

        PSID m_psid;
    };

    inline SidPtr::operator PSID() const
    {
        return m_psid;
    }
}
