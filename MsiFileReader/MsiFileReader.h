#pragma once


#include <map>
#include "hnrt/Args.h"
#include "hnrt/StringLessThan.h"
#include "hnrt/RefPtr.h"
#include "hnrt/IMsiDatabase.h"


namespace hnrt
{
    class MsiFileReader
        : public IMsiDatabaseExtractFilesCallback
    {
    public:

        MsiFileReader(int argc, wchar_t* argv[]);
        MsiFileReader(const MsiFileReader&) = delete;
        ~MsiFileReader() = default;
        void operator =(const MsiFileReader&) = delete;
        void Run();

        virtual bool OnMsiDatabaseExtract(PCWSTR pszFile, PCWSTR pszPath, unsigned long status);

    private:

        typedef void (MsiFileReader::*Action)();
        typedef std::map<PCWSTR, Action, StringLessThan> OptionMap;
        typedef std::pair<PCWSTR, Action> OptionMapEntry;
        typedef std::map<PCWSTR, const IMsiRow*, StringLessThan> FileRowMap;
        typedef std::pair<PCWSTR, const IMsiRow*> FileRowMapEntry;

        void ParseGeneralOptions();
        void DispatchCommand();
        void OptionHelp();
        void CommandPrintTableNames();
        void CommandPrintTable();
        void CommandDescribeTable();
        void CommandPrintFilesByFeature();
        void CommandPrintFilesByFilePattern();
        void ParsePrintOptions(PCWSTR* ppszTarget, PCWSTR pszErrorMessage);
        void PrintFeatureByFileName(PCWSTR pszFileName, PCWSTR pszComponent);
        void PrintPathByFeature(PCWSTR pszFeature, PCWSTR pszComponent);
        void PrintFeaturePathHeader();
        void PrintFeaturePath(PCWSTR pszFeature, PCWSTR pszFileName, PCWSTR pszComponent);
        void CommandExtractFiles();
        void CommandExtractFilesByFeature();
        void ParseExtractFilesOptions(PCWSTR* ppszTarget, PCWSTR pszErrorMessage);
        void ValidateOutDir();

        PCWSTR m_pszName;
        Args m_args;
        OptionMap m_optionsGeneral;
        RefPtr<IMsiDatabase> m_pMsiDatabase;
        RefPtr<IMsiTable> m_pComponentTable;
        RefPtr<IMsiTable> m_pDirectoryTable;
        RefPtr<IMsiTable> m_pFeatureComponentsTable;
        RefPtr<IMsiTable> m_pFileTable;
        OptionMap m_optionsCommand;
        PCWSTR m_pszOutDir;
        bool m_bFlat;
        long m_verboseLevel;
        bool m_bHeader;
        PCWSTR m_pszItemSeparator;
    };
}
