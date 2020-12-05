#pragma once


namespace hnrt
{
    template<typename T>
    class WhileInScope
    {
    public:

        WhileInScope(T& rValue, T initValue, T termValue);
        WhileInScope(const WhileInScope&) = delete;
        ~WhileInScope();
        void operator =(const WhileInScope&) = delete;

    private:

        T& m_rValue;
        T m_TermValue;
    };

    template<typename T>
    WhileInScope<T>::WhileInScope(T& rValue, T initValue, T termValue)
        : m_rValue(rValue)
        , m_TermValue(termValue)
    {
        m_rValue = initValue;
    }

    template<typename T>
    WhileInScope<T>::~WhileInScope()
    {
        m_rValue = m_TermValue;
    }
}
