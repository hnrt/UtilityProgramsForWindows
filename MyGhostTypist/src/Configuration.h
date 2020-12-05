#pragma once


#include <vector>
#include "hnrt/RefObj.h"
#include "hnrt/RefPtr.h"
#include "hnrt/PasswordHolder.h"
#include "WindowMetrics.h"
#include "CredentialsCollection.h"
#include "TargetCollection.h"


namespace hnrt
{
    class Configuration
        : public RefObj
    {
    public:

        static RefPtr<Configuration> Create();

        Configuration(const Configuration&) = delete;
        virtual ~Configuration();
        void operator =(const Configuration&) = delete;
        virtual void Load() = 0;
        virtual void Save() = 0;
        PCWSTR get_AppDir() const;
        CredentialsCollection& get_CredentialsList();
        long get_Width() const;
        long get_PaddingLeft() const;
        long get_PaddingTop() const;
        long get_PaddingRight() const;
        long get_PaddingBottom() const;
        PCWSTR get_FontName() const;
        long get_FontSize() const;
        long get_ButtonHeight() const;
        long get_ButtonMarginLeft() const;
        long get_ButtonMarginTop() const;
        long get_ButtonMarginRight() const;
        long get_ButtonMarginBottom() const;
        long get_TypingInterval() const;
        long get_TypingDelay() const;
        TargetCollection& get_TargetList();
        __declspec(property(get = get_AppDir)) PCWSTR AppDir;
        __declspec(property(get = get_CredentialsList)) CredentialsCollection& CredentialsList;
        __declspec(property(get = get_Width)) long Width;
        __declspec(property(get = get_PaddingLeft)) long PaddingLeft;
        __declspec(property(get = get_PaddingTop)) long PaddingTop;
        __declspec(property(get = get_PaddingRight)) long PaddingRight;
        __declspec(property(get = get_PaddingBottom)) long PaddingBottom;
        __declspec(property(get = get_FontName)) PCWSTR FontName;
        __declspec(property(get = get_FontSize)) long FontSize;
        __declspec(property(get = get_ButtonHeight)) long ButtonHeight;
        __declspec(property(get = get_ButtonMarginLeft)) long ButtonMarginLeft;
        __declspec(property(get = get_ButtonMarginTop)) long ButtonMarginTop;
        __declspec(property(get = get_ButtonMarginRight)) long ButtonMarginRight;
        __declspec(property(get = get_ButtonMarginBottom)) long ButtonMarginBottom;
        __declspec(property(get = get_TypingInterval)) long TypingInterval;
        __declspec(property(get = get_TypingDelay)) long TypingDelay;
        __declspec(property(get = get_TargetList)) TargetCollection& TargetList;

    protected:

        Configuration();

        PCWSTR m_pszAppDir;
        CredentialsCollection m_CredentialsList;
        WindowMetrics m_Window;
        ButtonMetrics m_Button;
        long m_TypingInterval;
        long m_TypingDelay;
        TargetCollection m_TargetList;
    };

    inline PCWSTR Configuration::get_AppDir() const
    {
        return m_pszAppDir;
    }

    inline CredentialsCollection& Configuration::get_CredentialsList()
    {
        return m_CredentialsList;
    }

    inline long Configuration::get_Width() const
    {
        return m_Window.Width;
    }

    inline long Configuration::get_PaddingLeft() const
    {
        return m_Window.Padding.Left;
    }

    inline long Configuration::get_PaddingTop() const
    {
        return m_Window.Padding.Top;
    }

    inline long Configuration::get_PaddingRight() const
    {
        return m_Window.Padding.Right;
    }

    inline long Configuration::get_PaddingBottom() const
    {
        return m_Window.Padding.Bottom;
    }

    inline PCWSTR Configuration::get_FontName() const
    {
        return m_Window.pszFontName;
    }

    inline long Configuration::get_FontSize() const
    {
        return m_Window.FontSize;
    }

    inline long Configuration::get_ButtonHeight() const
    {
        return m_Button.Height;
    }

    inline long Configuration::get_ButtonMarginLeft() const
    {
        return m_Button.Margin.Left;
    }

    inline long Configuration::get_ButtonMarginTop() const
    {
        return m_Button.Margin.Top;
    }

    inline long Configuration::get_ButtonMarginRight() const
    {
        return m_Button.Margin.Right;
    }

    inline long Configuration::get_ButtonMarginBottom() const
    {
        return m_Button.Margin.Bottom;
    }

    inline long Configuration::get_TypingInterval() const
    {
        return m_TypingInterval;
    }

    inline long Configuration::get_TypingDelay() const
    {
        return m_TypingDelay;
    }

    inline TargetCollection& Configuration::get_TargetList()
    {
        return m_TargetList;
    }
}
