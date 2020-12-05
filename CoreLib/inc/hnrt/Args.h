#pragma once


namespace hnrt
{
    class Args
    {
    public:

        ~Args() {}
        Args(int argc, wchar_t *argv[], int startIndex = 1);
        Args(const Args&) = delete;
        bool Exists(int delta = 0) const { return 0 < m_index + delta && m_index + delta < m_argc; }
        PCWSTR operator *() const { return m_index < m_argc ? m_argv[m_index] : nullptr; }
        PCWSTR operator [](int delta) const { return Exists(delta) ? m_argv[m_index + delta] : nullptr; }
        Args& operator ++() { m_index++; return *this; }
        Args& operator ++(int) { m_index++; return *this; }
        operator bool() const { return m_index < m_argc; }
        PCWSTR get_CommandPath() const { return m_argv[0]; }

        __declspec(property(get = get_CommandPath)) PCWSTR CommandPath;

    private:

        wchar_t** m_argv;
        int m_argc;
        int m_index;
    };
}
