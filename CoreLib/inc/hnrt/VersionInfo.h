#pragma once


#include "hnrt/RefObj.h"
#include "hnrt/RefPtr.h"


namespace hnrt
{
    struct StringFileInfo
    {
        USHORT wLanguage;
        USHORT wCodePage;
        PWSTR pszComments;
        PWSTR pszCompanyName;
        PWSTR pszFileDescription;
        PWSTR pszFileVersion;
        PWSTR pszInternalName;
        PWSTR pszLegalCopyright;
        PWSTR pszLegalTrademarks;
        PWSTR pszOriginalFilename;
        PWSTR pszProductName;
        PWSTR pszProductVersion;
        PWSTR pszPrivateBuild;
        PWSTR pszSpecialBuild;
    };

    class VersionInfo
        : public RefObj
    {
    public:

        static RefPtr<VersionInfo> Create(PCWSTR pszFileName = nullptr);

        VersionInfo(const VersionInfo&) = delete;
        virtual ~VersionInfo();
        void operator =(const VersionInfo&) = delete;
        virtual bool Select(USHORT wLanguage = 0xFFFF) = 0;
        USHORT get_FileVersionMajor() const;
        USHORT get_FileVersionMinor() const;
        USHORT get_FileVersionPatch() const;
        USHORT get_FileVersionBuild() const;
        USHORT get_ProductVersionMajor() const;
        USHORT get_ProductVersionMinor() const;
        USHORT get_ProductVersionPatch() const;
        USHORT get_ProductVersionBuild() const;
        ULONG get_FileFlagsMask() const;
        ULONG get_FileFlags() const;
        ULONG get_FileOS() const;
        ULONG get_FileType() const;
        ULONG get_FileSubtype() const;
        const StringFileInfo& operator[](ULONG index) const;
        ULONG get_BlockCount() const;
        ULONG get_CurrentIndex() const;
        void set_CurrentIndex(ULONG value);
        USHORT get_Language() const;
        USHORT get_CodePage() const;
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
        __declspec(property(get = get_FileVersionMajor)) USHORT FileVersionMajor;
        __declspec(property(get = get_FileVersionMinor)) USHORT FileVersionMinor;
        __declspec(property(get = get_FileVersionPatch)) USHORT FileVersionPatch;
        __declspec(property(get = get_FileVersionBuild)) USHORT FileVersionBuild;
        __declspec(property(get = get_ProductVersionMajor)) USHORT ProductVersionMajor;
        __declspec(property(get = get_ProductVersionMinor)) USHORT ProductVersionMinor;
        __declspec(property(get = get_ProductVersionPatch)) USHORT ProductVersionPatch;
        __declspec(property(get = get_ProductVersionBuild)) USHORT ProductVersionBuild;
        __declspec(property(get = get_FileFlagsMask)) ULONG FileFlagsMask;
        __declspec(property(get = get_FileFlags)) ULONG FileFlags;
        __declspec(property(get = get_FileOS)) ULONG FileOS;
        __declspec(property(get = get_FileType)) ULONG FileType;
        __declspec(property(get = get_FileSubtype)) ULONG FileSubtype;
        __declspec(property(get = get_BlockCount)) ULONG BlockCount;
        __declspec(property(get = get_CurrentIndex, put = set_CurrentIndex)) ULONG CurrentIndex;
        __declspec(property(get = get_Language)) USHORT Language;
        __declspec(property(get = get_CodePage)) USHORT CodePage;
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

        USHORT m_wFileVersion[4];
        USHORT m_wProductVersion[4];
        ULONG m_dwFileFlagsMask;
        ULONG m_dwFileFlags;
        ULONG m_dwFileOS;
        ULONG m_dwFileType;
        ULONG m_dwFileSubtype;
        ULONG m_cStringFileInfo;
        StringFileInfo* m_pStringFileInfo;
        ULONG m_dwCurrentIndex;
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

    inline USHORT VersionInfo::get_FileVersionMajor() const { return m_wFileVersion[0]; }

    inline USHORT VersionInfo::get_FileVersionMinor() const { return m_wFileVersion[1]; }

    inline USHORT VersionInfo::get_FileVersionPatch() const { return m_wFileVersion[2]; }

    inline USHORT VersionInfo::get_FileVersionBuild() const { return m_wFileVersion[3]; }

    inline USHORT VersionInfo::get_ProductVersionMajor() const { return m_wProductVersion[0]; }

    inline USHORT VersionInfo::get_ProductVersionMinor() const { return m_wProductVersion[1]; }

    inline USHORT VersionInfo::get_ProductVersionPatch() const { return m_wProductVersion[2]; }

    inline USHORT VersionInfo::get_ProductVersionBuild() const { return m_wProductVersion[3]; }

    inline ULONG VersionInfo::get_FileFlagsMask() const { return m_dwFileFlagsMask; }

    inline ULONG VersionInfo::get_FileFlags() const { return m_dwFileFlags; }

    inline ULONG VersionInfo::get_FileOS() const { return m_dwFileOS; }

    inline ULONG VersionInfo::get_FileType() const { return m_dwFileType; }

    inline ULONG VersionInfo::get_FileSubtype() const { return m_dwFileSubtype; }

    inline const StringFileInfo& VersionInfo::operator[](ULONG index) const { return m_pStringFileInfo[index]; }

    inline ULONG VersionInfo::get_BlockCount() const { return m_cStringFileInfo; }

    inline ULONG VersionInfo::get_CurrentIndex() const { return m_dwCurrentIndex; }

    inline void VersionInfo::set_CurrentIndex(ULONG value) { m_dwCurrentIndex = value; }

    inline USHORT VersionInfo::get_Language() const { return m_pStringFileInfo[m_dwCurrentIndex].wLanguage; }

    inline USHORT VersionInfo::get_CodePage() const { return m_pStringFileInfo[m_dwCurrentIndex].wCodePage; }

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
