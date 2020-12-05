#pragma once


#include "hnrt/RefObj.h"
#include "hnrt/RefPtr.h"


namespace hnrt
{
    struct StringFileInfo
    {
        unsigned short wLanguage;
        unsigned short wCodePage;
        wchar_t* pszComments;
        wchar_t* pszCompanyName;
        wchar_t* pszFileDescription;
        wchar_t* pszFileVersion;
        wchar_t* pszInternalName;
        wchar_t* pszLegalCopyright;
        wchar_t* pszLegalTrademarks;
        wchar_t* pszOriginalFilename;
        wchar_t* pszProductName;
        wchar_t* pszProductVersion;
        wchar_t* pszPrivateBuild;
        wchar_t* pszSpecialBuild;
    };

    class VersionInfo
        : public RefObj
    {
    public:

        static RefPtr<VersionInfo> Create(PCWSTR pszFileName = nullptr);

        VersionInfo(const VersionInfo&) = delete;
        virtual ~VersionInfo();
        void operator =(const VersionInfo&) = delete;
        virtual bool Select(unsigned short wLanguage = 0xFFFF) = 0;
        unsigned short get_FileVersionMajor() const;
        unsigned short get_FileVersionMinor() const;
        unsigned short get_FileVersionPatch() const;
        unsigned short get_FileVersionBuild() const;
        unsigned short get_ProductVersionMajor() const;
        unsigned short get_ProductVersionMinor() const;
        unsigned short get_ProductVersionPatch() const;
        unsigned short get_ProductVersionBuild() const;
        unsigned long get_FileFlagsMask() const;
        unsigned long get_FileFlags() const;
        unsigned long get_FileOS() const;
        unsigned long get_FileType() const;
        unsigned long get_FileSubtype() const;
        const StringFileInfo& operator[](unsigned long index) const;
        unsigned long get_BlockCount() const;
        unsigned long get_CurrentIndex() const;
        void set_CurrentIndex(unsigned long value);
        unsigned short get_Language() const;
        unsigned short get_CodePage() const;
        PCWSTR get_Comments() const;
        PCWSTR get_CompanyName() const;
        PCWSTR get_FileDescription() const;
        PCWSTR get_FileVersion() const;
        PCWSTR get_InternalName() const;
        PCWSTR get_LegalCopyright() const;
        PCWSTR get_LegalTrademarks() const;
        PCWSTR get_OriginalFilename() const;
        PCWSTR get_ProductName() const;
        PCWSTR get_ProductVersion() const;
        PCWSTR get_PrivateBuild() const;
        PCWSTR get_SpecialBuild() const;
        __declspec(property(get = get_FileVersionMajor)) unsigned short FileVersionMajor;
        __declspec(property(get = get_FileVersionMinor)) unsigned short FileVersionMinor;
        __declspec(property(get = get_FileVersionPatch)) unsigned short FileVersionPatch;
        __declspec(property(get = get_FileVersionBuild)) unsigned short FileVersionBuild;
        __declspec(property(get = get_ProductVersionMajor)) unsigned short ProductVersionMajor;
        __declspec(property(get = get_ProductVersionMinor)) unsigned short ProductVersionMinor;
        __declspec(property(get = get_ProductVersionPatch)) unsigned short ProductVersionPatch;
        __declspec(property(get = get_ProductVersionBuild)) unsigned short ProductVersionBuild;
        __declspec(property(get = get_FileFlagsMask)) unsigned long FileFlagsMask;
        __declspec(property(get = get_FileFlags)) unsigned long FileFlags;
        __declspec(property(get = get_FileOS)) unsigned long FileOS;
        __declspec(property(get = get_FileType)) unsigned long FileType;
        __declspec(property(get = get_FileSubtype)) unsigned long FileSubtype;
        __declspec(property(get = get_BlockCount)) unsigned long BlockCount;
        __declspec(property(get = get_CurrentIndex, put = set_CurrentIndex)) unsigned long CurrentIndex;
        __declspec(property(get = get_Language)) unsigned short Language;
        __declspec(property(get = get_CodePage)) unsigned short CodePage;
        __declspec(property(get = get_Comments)) PCWSTR Comments;
        __declspec(property(get = get_CompanyName)) PCWSTR CompanyName;
        __declspec(property(get = get_FileDescription)) PCWSTR FileDescription;
        __declspec(property(get = get_FileVersion)) PCWSTR FileVersion;
        __declspec(property(get = get_InternalName)) PCWSTR InternalName;
        __declspec(property(get = get_LegalCopyright)) PCWSTR LegalCopyright;
        __declspec(property(get = get_LegalTrademarks)) PCWSTR LegalTrademarks;
        __declspec(property(get = get_OriginalFilename)) PCWSTR OriginalFilename;
        __declspec(property(get = get_ProductName)) PCWSTR ProductName;
        __declspec(property(get = get_ProductVersion)) PCWSTR ProductVersion;
        __declspec(property(get = get_PrivateBuild)) PCWSTR PrivateBuild;
        __declspec(property(get = get_SpecialBuild)) PCWSTR SpecialBuild;

    protected:

        VersionInfo();

        unsigned short m_wFileVersion[4];
        unsigned short m_wProductVersion[4];
        unsigned long m_dwFileFlagsMask;
        unsigned long m_dwFileFlags;
        unsigned long m_dwFileOS;
        unsigned long m_dwFileType;
        unsigned long m_dwFileSubtype;
        unsigned long m_cStringFileInfo;
        StringFileInfo* m_pStringFileInfo;
        unsigned long m_dwCurrentIndex;
    };

    inline VersionInfo::VersionInfo()
        : RefObj()
        , m_wFileVersion { 0, 0, 0, 0 }
        , m_wProductVersion { 0, 0, 0, 0 }
        , m_dwFileFlagsMask(0)
        , m_dwFileFlags(0)
        , m_dwFileOS(0)
        , m_dwFileType(0)
        , m_dwFileSubtype(0)
        , m_cStringFileInfo(0)
        , m_pStringFileInfo(nullptr)
        , m_dwCurrentIndex(0)
    {
    }

    inline VersionInfo::~VersionInfo()
    {
        delete[] m_pStringFileInfo;
    }

    inline unsigned short VersionInfo::get_FileVersionMajor() const { return m_wFileVersion[0]; }

    inline unsigned short VersionInfo::get_FileVersionMinor() const { return m_wFileVersion[1]; }

    inline unsigned short VersionInfo::get_FileVersionPatch() const { return m_wFileVersion[2]; }

    inline unsigned short VersionInfo::get_FileVersionBuild() const { return m_wFileVersion[3]; }

    inline unsigned short VersionInfo::get_ProductVersionMajor() const { return m_wProductVersion[0]; }

    inline unsigned short VersionInfo::get_ProductVersionMinor() const { return m_wProductVersion[1]; }

    inline unsigned short VersionInfo::get_ProductVersionPatch() const { return m_wProductVersion[2]; }

    inline unsigned short VersionInfo::get_ProductVersionBuild() const { return m_wProductVersion[3]; }

    inline unsigned long VersionInfo::get_FileFlagsMask() const { return m_dwFileFlagsMask; }

    inline unsigned long VersionInfo::get_FileFlags() const { return m_dwFileFlags; }

    inline unsigned long VersionInfo::get_FileOS() const { return m_dwFileOS; }

    inline unsigned long VersionInfo::get_FileType() const { return m_dwFileType; }

    inline unsigned long VersionInfo::get_FileSubtype() const { return m_dwFileSubtype; }

    inline const StringFileInfo& VersionInfo::operator[](unsigned long index) const { return m_pStringFileInfo[index]; }

    inline unsigned long VersionInfo::get_BlockCount() const { return m_cStringFileInfo; }

    inline unsigned long VersionInfo::get_CurrentIndex() const { return m_dwCurrentIndex; }

    inline void VersionInfo::set_CurrentIndex(unsigned long value) { m_dwCurrentIndex = value; }

    inline unsigned short VersionInfo::get_Language() const { return m_pStringFileInfo[m_dwCurrentIndex].wLanguage; }

    inline unsigned short VersionInfo::get_CodePage() const { return m_pStringFileInfo[m_dwCurrentIndex].wCodePage; }

    inline PCWSTR VersionInfo::get_Comments() const { return m_pStringFileInfo[m_dwCurrentIndex].pszComments; }

    inline PCWSTR VersionInfo::get_CompanyName() const { return m_pStringFileInfo[m_dwCurrentIndex].pszCompanyName; }

    inline PCWSTR VersionInfo::get_FileDescription() const { return m_pStringFileInfo[m_dwCurrentIndex].pszFileDescription; }

    inline PCWSTR VersionInfo::get_FileVersion() const { return m_pStringFileInfo[m_dwCurrentIndex].pszFileVersion; }

    inline PCWSTR VersionInfo::get_InternalName() const { return m_pStringFileInfo[m_dwCurrentIndex].pszInternalName; }

    inline PCWSTR VersionInfo::get_LegalCopyright() const { return m_pStringFileInfo[m_dwCurrentIndex].pszLegalCopyright; }

    inline PCWSTR VersionInfo::get_LegalTrademarks() const { return m_pStringFileInfo[m_dwCurrentIndex].pszLegalTrademarks; }

    inline PCWSTR VersionInfo::get_OriginalFilename() const { return m_pStringFileInfo[m_dwCurrentIndex].pszOriginalFilename; }

    inline PCWSTR VersionInfo::get_ProductName() const { return m_pStringFileInfo[m_dwCurrentIndex].pszProductName; }

    inline PCWSTR VersionInfo::get_ProductVersion() const { return m_pStringFileInfo[m_dwCurrentIndex].pszProductVersion; }

    inline PCWSTR VersionInfo::get_PrivateBuild() const { return m_pStringFileInfo[m_dwCurrentIndex].pszPrivateBuild; }

    inline PCWSTR VersionInfo::get_SpecialBuild() const { return m_pStringFileInfo[m_dwCurrentIndex].pszSpecialBuild; }
}
